#include <string>
#include <algorithm>
#include <regex>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "magnet_table.h"
#include "homura.h"
#include "errlib.h"

// mix of c and c++ for torrent scraping
// use char* for html parsing, store results in string

int debug_level = 0;

/* terminate program if curl reports an error */
bool check_curlcode(CURLcode code,std::string where){
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
  if (!check_curlcode(code,"curl_one: CURLOPT_URL")) {
    free_memobject(store);
    return nullptr;
  }
  // determine the function that will handle the data
  code = curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,WriteCallback);
  if (!check_curlcode(code,"curl_one: CURLOPT_WRITEFUNCTION")){
    free_memobject(store);
    return nullptr;
  }
  // determine the location that the data will be stored
  code = curl_easy_setopt(conn,CURLOPT_WRITEDATA,&*store);
  if (!check_curlcode(code,"curl_one: CURLOPT_WRITEDATA")){
    free_memobject(store);
    return nullptr;
  }
  // some websites require a user-agent name.
  code = curl_easy_setopt(conn,CURLOPT_USERAGENT,"libcurl-agent/1.0");
  if (!check_curlcode(code,"curl_one: CURLOPT_USERAGENT")){
    free_memobject(store);
    return nullptr;
  }
  // retrieve content
  code = curl_easy_perform(conn);
  if (!check_curlcode(code,"curl_one: easy_preform")) {
    free_memobject(store);
    return nullptr;
  }
  curl_easy_cleanup(conn);
  return store;
}

void homura::free_mtable(magnet_table *names){
  if (names){
    for (auto itor : *names){
      delete itor;
    }
  }
  delete names;
}

/* given a string, scrape all torrent names and magnets */
magnet_table *homura::query_packages(std::string args, int LOG_LEVEL, int threads){
  int wait_time = 5;
  debug_level = LOG_LEVEL;
  curl_global_init(CURL_GLOBAL_ALL);
  /* nyaa.si has no official api, and we must manually
     find out how many pages to parse by sending a request */

  int num_elements = 0;

  std::replace(args.begin(),args.end(),' ','+');
  std::string FIRST_ = "https://nyaa.si/?f=0&c=0_0&q=" + args;
  struct memobject *FIRST_PAGE = curl_one(FIRST_);
  if (!FIRST_PAGE){
    set_error_exitcode(ERRCODE::FAILED_CURL); 
    return nullptr;
  }

  if (debug_level){
    fprintf (stderr, " == FIRST_PAGE_URL == \n");
    fprintf (stderr, "%s\n",FIRST_.c_str());
    if (debug_level > 1){
      fprintf (stderr, "%s\n",FIRST_PAGE->ptr);
    }
  }

  std::regex exp("([0-9]+)-([0-9]+) out of ([0-9]+)(?= results)");
  std::smatch sm;
  std::string text = std::string(FIRST_PAGE->ptr);
  if(!regex_search(text, sm, exp)) {
    errprintf(ERRCODE::FAILED_FIRST_PARSE,
      "Failed to parse first page \n");
    free_memobject(FIRST_PAGE);
    return nullptr;
  }
  num_elements = std::stoi(sm[3].str()); 
  if (!num_elements){
    fprintf(stdout,"No results found for %s\n", args.c_str());
    free_memobject(FIRST_PAGE);
    return nullptr;
  }
  magnet_table *names = new magnet_table(num_elements);
  for (size_t i = 0; i < names->size(); i++){
    (*names)[i] = new name_magnet();
  }
  free_memobject(FIRST_PAGE);
  curl_global_cleanup();
  return names;
}
