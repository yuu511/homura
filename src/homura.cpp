#include <string>
#include <algorithm>
#include <regex>
#include <chrono>
#include <utility>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "homura.h"
#include "magnet_table.h"
#include "errlib.h"

// mix of c and c++ for torrent scraping
// use char* for html parsing, store results in string

int debug_level = 0;

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
struct memobject {
  char *ptr;
  size_t len;
}; 

void memobject_init(struct memobject *s){
  s->len = 0;
  s->ptr = (char *) malloc(1);
  if (s->ptr == NULL){
    errprintf (ERRCODE::FAILED_MALLOC, "malloc() failed in memobject_init");
    return;
  }
  s->ptr[0] = '\0';
}

void free_memobject(struct memobject *s){
  if (s){
    free (s->ptr);
    free (s);
  }
}

/* callback function write webpage to memory */
static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, struct memobject *s) {
  size_t realsize = s->len + size * nmemb;
  s->ptr = (char *) realloc(s->ptr,realsize+1);
  if (s->ptr == NULL){
    errprintf (ERRCODE::FAILED_REALLOC, "malloc() failed ");
    exit(EXIT_FAILURE);
  }
  memcpy(s->len+s->ptr, ptr, size * nmemb);
  s->ptr[realsize] = '\0';
  s->len = realsize;
  return size * nmemb;
}

/* download one webpage */
struct memobject *curl_one(std::string args){
  CURLcode code;
  CURL *conn = curl_easy_init();
  if (!conn){
    errprintf (ERRCODE::FAILED_CURL, "failed to recieve the curl handle");
    return nullptr;
  }
  struct memobject *store = (memobject*) malloc (sizeof(memobject));
  memobject_init(store);

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
    free_memobject(store);
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
  if (debug_level)
    fprintf (stderr, "== URLS: ==\n");
  // rounds up integer division, (overflow not expected, max results = 1000)
  int num_elements = ( total + (results_per_page -1) ) / results_per_page; 
  std::vector<std::string*> *urls;
  try {
    // already tried the first page
    urls = new std::vector<std::string*>(num_elements);
    // first page in memory already (nyaasi)
    for (size_t i = 1; i < urls->size(); ++i){
     (*urls)[i] = new std::string (base_url + "&p=" + std::to_string(i+1));   
     if (debug_level)
       fprintf (stderr, "%s\n", (*urls)[i]->c_str());
    }
  }
  catch (std::bad_alloc &ba){
      errprintf(ERRCODE::FAILED_NEW, "Failed new allocation %s\n",ba);
      return nullptr;
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
/* given a string, scrape all torrent names and magnets */
magnet_table *homura::search_nyaasi(std::string args, int LOG_LEVEL, int threads){
  std::chrono::seconds crawl_delay(5);
  debug_level = LOG_LEVEL;
  curl_global_init(CURL_GLOBAL_ALL);
  /* nyaa.si has no official api, and we must manually
     find out how many pages to parse by sending a request */
  std::replace(args.begin(),args.end(),' ','+');
  std::string FIRST_ = "https://nyaa.si/?f=0&c=0_0&q=" + args;
  struct memobject *FIRST_PAGE = curl_one(FIRST_);
  if (!FIRST_PAGE) return nullptr;

  if (debug_level)
    fprintf (stderr, "== FIRST_PAGE_URL == \n %s\n",FIRST_.c_str());
  if (debug_level > 1) 
    fprintf (stderr, "%s\n",FIRST_PAGE->ptr);

  std::smatch sm;
  std::regex exp("([0-9]+)-([0-9]+) out of ([0-9]+)(?= results)");
  std::string text = std::string(FIRST_PAGE->ptr);
  if(!regex_search(text, sm, exp)) {
    errprintf(ERRCODE::FAILED_FIRST_PARSE, "Failed to parse first page \n");
    free_memobject(FIRST_PAGE);
    return nullptr;
  }

  if (debug_level)
    fprintf(stderr,"== SMATCH_INFO ==\n"
      "smatch: %s\nsmatch_sz: %zd\n",sm[0].str().c_str(),sm.size());

  int total = std::stoi(sm[3].str());
  int results_per_page = std::stoi(sm[2].str());

  if (debug_level)
    printf ("results per page %d\ntotal %d\n",total,results_per_page);

  magnet_table *names = alloc_table_nyaasi(total);
  std::vector<std::string*> *urls = alloc_urls_nyaasi(total,results_per_page,FIRST_);
  if (!names || !urls) goto failed_search_cleanup;

  free_memobject(FIRST_PAGE);
  free_urls(urls);
  curl_global_cleanup();
  return names;

  failed_search_cleanup:
    free_memobject(FIRST_PAGE);
    free_urls(urls);
    homura::free_mtable(names);
    curl_global_cleanup();
    return nullptr;
}

void homura::free_mtable(magnet_table *names){
  if (names){
    for (auto itor : *names){
      delete itor;
    }
    delete names;
  }
}
