#include <string>
#include <algorithm>
#include <chrono>
#include <utility>
#include <ctime>
#include <thread>

#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <myhtml/myhtml.h>

#include "homura.h"
#include "magnet_table.h"
#include "errlib.h"

// mix of c and c++ for torrent scraping
// use char* for html parsing, store results in string

int debug_level = 0;

// in order to use SSL in a multithreaded context, we must place mutex callback setups
// https://curl.haxx.se/libcurl/c/threaded-ssl.html
static pthread_mutex_t *lockarray;

extern "C"
{
  #ifndef USE_OPENSSL
  #include <openssl/crypto.h>
  static void lock_callback(int mode, int type, char *file, int line)
  {
    (void)file;
    (void)line;
    if(mode & CRYPTO_LOCK) {
      pthread_mutex_lock(&(lockarray[type]));
    }
    else {
      pthread_mutex_unlock(&(lockarray[type]));
    }
  }
  
  static unsigned long thread_id(void)
  {
    unsigned long ret;
  
    ret = (unsigned long)pthread_self();
    return ret;
  }
  
  static void init_locks(void)
  {
    int i;
  
    lockarray = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() *
                                                  sizeof(pthread_mutex_t));
    for(i = 0; i<CRYPTO_num_locks(); i++) {
      pthread_mutex_init(&(lockarray[i]), NULL);
    }
  
    CRYPTO_set_id_callback((unsigned long (*)())thread_id);
    CRYPTO_set_locking_callback((void (*)())lock_callback);
  }
  
  static void kill_locks(void)
  {
    int i;
  
    CRYPTO_set_locking_callback(NULL);
    for(i = 0; i<CRYPTO_num_locks(); i++)
      pthread_mutex_destroy(&(lockarray[i]));
  
    OPENSSL_free(lockarray);
  }
  #endif
  
  #ifdef USE_GNUTLS
  #include <gcrypt.h>
  #include <errno.h>
  
  GCRY_THREAD_OPTION_PTHREAD_IMPL;
  
  void init_locks(void)
  {
    gcry_control(GCRYCTL_SET_THREAD_CBS);
  }
  
  #define kill_locks()
  #endif
}

/* terminate program if curl reports an error */
bool curlcode_pass(CURLcode code,std::string where){
  std::string s;
  if (CURLE_OK != code){
    switch(code){
      case CURLE_UNSUPPORTED_PROTOCOL:
        s = "CURLE_UNSUPPORTED_PROTOCOL";
      case CURLE_WRITE_ERROR:
        s = "CURLE_WRITE_ERROR";
      case CURLE_UNKNOWN_OPTION:
        s = "CURLE_UNKNOWN_OPTION";
      case CURLE_OUT_OF_MEMORY:
        s = "CURLE_OUT_OF_MEMORY";
      default: 
        s = "CURLM_unkown"; break;
    }
    errprintf (ERRCODE::FAILED_CURL,"ERROR: %s returns %s\n",where.c_str(), s.c_str());
    return false;
  }
  return true;
}

/* container for html data */
struct html_s {
  char *ptr;
  size_t len;
}; 

void html_s_init(struct html_s *s){
  s->len = 0;
  s->ptr = (char *) malloc(1);
  if (s->ptr == nullptr){
    errprintf (ERRCODE::FAILED_MALLOC, "malloc() failed in html_s_init");
    return;
  }
  s->ptr[0] = '\0';
}

void free_html_s(struct html_s *s){
  if (s){
    free (s->ptr);
    free (s);
  }
}

/* callback function write webpage to memory */
static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, struct html_s *s) {
  size_t realsize = s->len + size * nmemb;
  s->ptr = (char *) realloc(s->ptr,realsize+1);
  if (s->ptr == nullptr){
    errprintf(ERRCODE::FAILED_REALLOC, "malloc() failed ");
    return 0; 
  }
  memcpy(s->len+s->ptr, ptr, size * nmemb);
  s->ptr[realsize] = '\0';
  s->len = realsize;
  return size * nmemb;
}

/* download one webpage */
struct html_s *curl_one(std::string args){
  CURLcode code;
  CURL *conn = curl_easy_init();
  if (!conn){
    errprintf (ERRCODE::FAILED_CURL, "failed to recieve the curl handle");
    return nullptr;
  }
  struct html_s *store = (html_s*) malloc (sizeof(html_s));
  html_s_init(store);

  // add URL to curl handle
  code = curl_easy_setopt(conn,CURLOPT_URL,args.c_str());
  if (!curlcode_pass(code,"curl_one: CURLOPT_URL")) goto curlone_fail;

  // determine the function that will handle the data
  code = curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,WriteCallback);
  if (!curlcode_pass(code,"curl_one: CURLOPT_WRITEFUNCTION")) goto curlone_fail;

  // determine the location that the data will be stored
  code = curl_easy_setopt(conn,CURLOPT_WRITEDATA,&*store);
  if (!curlcode_pass(code,"curl_one: CURLOPT_WRITEDATA")) goto curlone_fail;

  // some websites require a user-agent name.
  code = curl_easy_setopt(conn,CURLOPT_USERAGENT,"libcurl-agent/1.0");
  if (!curlcode_pass(code,"curl_one: CURLOPT_USERAGENT")) goto curlone_fail;

  // retrieve content
  code = curl_easy_perform(conn);
  if (!curlcode_pass(code,"curl_one: easy_preform")) goto curlone_fail;

  curl_easy_cleanup(conn);
  return store;

  curlone_fail:
    set_error_exitcode(ERRCODE::FAILED_CURL); 
    curl_global_cleanup();
    free_html_s(store);
    return nullptr;
}

magnet_table *alloc_table_nyaasi(int total) {
  magnet_table *names = nullptr;
  if (!total){
    fprintf(stdout,"No results found for query\n");
    return nullptr;
  }
  try {
    names = new magnet_table(total);
    for (size_t i = 0; i < names->size(); ++i){
      (*names)[i] = new name_magnet();
    }
  }
  catch (std::bad_alloc &ba){
    errprintf(ERRCODE::FAILED_NEW, "Failed new allocation %s\n",ba);
    return nullptr;
  }
  return names;
}

std::vector<std::string*> *alloc_urls_nyaasi
(int total, int results_per_page, std::string base_url) {
  if (!total){
    return nullptr;
  }

  if (debug_level){
    fprintf (stderr, "== URLS: ==\n");
  }

  // rounds up integer division, (overflow not expected, max results = 1000)
  int num_elements = ( total + (results_per_page - 1) ) / results_per_page; 
  std::vector<std::string*> *urls;
  try {
    // already tried the first page
    urls = new std::vector<std::string*>(num_elements-1);
    // first page in memory already (nyaasi)
    for (size_t i = 1; i < urls->size(); ++i){
      (*urls)[i] = new std::string (base_url + "&p=" + std::to_string(i+1));   

      if (debug_level){
        fprintf (stdout, "%s\n", (*urls)[i]->c_str());
      }

    }
  }
  catch (std::bad_alloc &ba){
      errprintf(ERRCODE::FAILED_NEW, "Failed new allocation %s\n",ba);
      return nullptr;
  }

  if (debug_level){
    fprintf(stdout,"Pages to parse: %zd\n",urls->size()+1);
  }

  return urls;
}

void free_urls (std::vector<std::string*> *urls){
  if (urls){
    for (auto itor : *urls){
      delete itor; 
    }
    delete urls;
  }
}

void print_node_attr(myhtml_tree_node_t *node)
{
    myhtml_tree_attr_t *attr = myhtml_node_attribute_first(node);
    
    while (attr) {
        const char *name = myhtml_attribute_key(attr, NULL);
        
        if(name) {
            printf(" %s", name);
            
            const char *value = myhtml_attribute_value(attr, NULL);
            
            if(value)
                printf("=\"%s\"", value);
        }
        
        attr = myhtml_attribute_next(attr);
    }
}

void print_tree(myhtml_tree_t* tree, myhtml_tree_node_t *node, size_t inc)
{
    while (node)
    {
        for(size_t i = 0; i < inc; i++)
            printf("\t");
        
        // print current element
        const char *tag_name = myhtml_tag_name_by_id(tree, myhtml_node_tag_id(node), NULL);
        
        if(tag_name)
            printf("<%s", tag_name);
        else
            // it can not be
            printf("<!something is wrong!");
        
        // print node attributes
        print_node_attr(node);
        
        if(myhtml_node_is_close_self(node))
            printf(" /");
        
        myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);
        
        if(tag_id == MyHTML_TAG__TEXT || tag_id == MyHTML_TAG__COMMENT) {
            const char* node_text = myhtml_node_text(node, NULL);
            printf("---->: %s\n", node_text);
        }
        else {
            printf(">\n");
        }
        
        // print children
        print_tree(tree, myhtml_node_child(node), (inc + 1));
        node = myhtml_node_next(node);
    }
}

/* given a string, scrape all torrent names and magnets */
magnet_table *homura::search_nyaasi(std::string args, int LOG_LEVEL, int threads){
  namespace clock = std::chrono;
  clock::seconds crawl_delay(5);
  auto now = clock::steady_clock::now();
  auto new_request = now + crawl_delay;

  debug_level = LOG_LEVEL;
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();
  /* nyaa.si has no official api, and we must manually
     find out how many pages to parse by sending a request */
  std::replace(args.begin(),args.end(),' ','+');
  std::string FIRST_ = "https://nyaa.si/?f=0&c=0_0&q=" + args;
  struct html_s *FIRST_PAGE = curl_one(FIRST_);
  if (!FIRST_PAGE) return nullptr;

  if (debug_level){
    fprintf (stdout, "== First Page URL: == \n%s\n\n",FIRST_.c_str());
  }

  if (debug_level > 1){
    fprintf (stdout, "%s\n",FIRST_PAGE->ptr);
  }

  // basic init
  myhtml_t *myhtml = myhtml_create();
  myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);
  
  // init tree
  myhtml_tree_t *tree = myhtml_tree_create();
  myhtml_tree_init(tree, myhtml);
  
  // parse html
  myhtml_parse(tree, MyENCODING_UTF_8, FIRST_PAGE->ptr, FIRST_PAGE->len+1);

  // attempt to find the pagination information
  const char *page_information;
  myhtml_collection_t *found = 
    myhtml_get_nodes_by_attribute_value(tree,NULL,NULL,true,"class",5,"pagination-page-info",20,NULL);
  if (found && found->list && found->length){
    myhtml_tree_node_t *node = found->list[0];
    node = myhtml_node_child(node);
    myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);
    if (tag_id == MyHTML_TAG__TEXT || tag_id == MyHTML_TAG__COMMENT){
      page_information = myhtml_node_text(node,NULL);

      if (debug_level){
        fprintf (stdout,"== First page pagination Information: ==\n%s\n\n",page_information);
      }

    } 
    else {
      errprintf(ERRCODE::FAILED_FIRST_PARSE, "Failed to parse first page \n (Pagination information not found)");
    }
    if (myhtml_collection_destroy(found)){
      errprintf(ERRCODE::FAILED_FREE, "Failed to free MyHTML collection.");
    }
  } 
  else{
    errprintf(ERRCODE::FAILED_FIRST_PARSE, "Failed to parse first page \n (Pagination information not found)");
  }

  // Displaying results[0]-results[1] out of results[2]
  int results[3];
  int idx = 0;
  // copy so we can modify the string
  char *copy = strdup(page_information);
  char *orig_location = copy;
  while (*copy){
    if (isdigit(*copy)){
       long int parse = strtol(copy,&copy,10);
       if (parse <= INT_MAX){
         results[idx] = (int) parse;
         idx++;
       } 
       else {
         errprintf(ERRCODE::FAILED_INTCAST,"Failed to convert long to int");
	 free(orig_location);
       }
    }
    else {
      copy++;
    }
  }
  free(orig_location);

  if (debug_level){
    fprintf (stdout,"results per page %d\ntotal pages%d\n",results[2],results[1]);
  }

  magnet_table *names = alloc_table_nyaasi(results[2]);
  std::vector<std::string*> *urls = alloc_urls_nyaasi(results[2],results[1],FIRST_);
  if (!names || !urls) {
    myhtml_tree_destroy(tree);
    myhtml_destroy(myhtml);
    free_html_s(FIRST_PAGE);
    free_urls(urls);
    homura::free_mtable(names);
    curl_global_cleanup();
    return nullptr;
  }

  /* process first url we pulled here */
  now = clock::steady_clock::now();
  std::this_thread::sleep_for(clock::duration_cast<clock::milliseconds>(new_request - now));

  for (auto itor : *urls) {
    new_request = clock::steady_clock::now() + crawl_delay;
    // do stuff
    now = clock::steady_clock::now();
    if (debug_level) {
      fprintf(stdout,"sleeping for %lu milliseconds\n",
              clock::duration_cast<clock::milliseconds>(new_request - now).count()); 
    }
    std::this_thread::sleep_for(clock::duration_cast<clock::milliseconds>(new_request - now));
  }

  // release resources
  free_html_s(FIRST_PAGE);
  free_urls(urls);
  curl_global_cleanup();
  kill_locks();
  myhtml_tree_destroy(tree);
  myhtml_destroy(myhtml);
  return names;
}

void homura::free_mtable(magnet_table *names){
  if (names){
    for (auto itor : *names){
      delete itor;
    }
    delete names;
  }
}
