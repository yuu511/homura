#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "homura.h"

// objective: write c for curl / html parsing part
// (limit cpp to string mainuplation and boost::thread)
// write c++ for libtorrent part (after everything is parsed)

// https://curl.haxx.se/libcurl/c/hiperfifo.html
bool curl_or_die(CURLcode code,std::string where){
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
    std::cerr << boost::format("ERROR: %s returns %s\n") % s % where;
    // curl die
    return false;
  }
  // curl live 
  return true;
}

struct memobject {
  char *ptr;
  size_t len;
}; 

void init_memobject(struct memobject *s){
  s->len = 0;
  s->ptr = (char *) malloc(1);
  if (s->ptr == nullptr){
    std::cerr << "malloc failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

static size_t
WriteCallback(void *ptr, size_t size, size_t nmemb, struct memobject *s) {
  size_t realsize = s->len + size * nmemb;
  s->ptr = (char *) realloc(s->ptr,realsize+1);
  if (s->ptr == nullptr){
    std::cerr << "realloc failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  memcpy(s->len+s->ptr, ptr, size * nmemb);
  s->ptr[realsize] = '\0';
  s->len = realsize;
  return size * nmemb;
}

struct memobject curl_one(CURL *&conn, std::string args){
  CURLcode code;
  conn = curl_easy_init();
  struct memobject store;
  init_memobject(&store);
  if (!conn) exit(EXIT_FAILURE);
  code = curl_easy_setopt(conn,CURLOPT_URL,args.c_str());
  if (!curl_or_die(code,"curl_one: CURLOPT_URL")) exit(EXIT_FAILURE); 
  code = curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,WriteCallback);
  if (!curl_or_die(code,"curl_one: CURLOPT_WRITEFUNCTION")) exit(EXIT_FAILURE); 
  code = curl_easy_setopt(conn,CURLOPT_WRITEDATA,&store);
  if (!curl_or_die(code,"curl_one: CURLOPT_WRITEDATA")) exit(EXIT_FAILURE); 
  code = curl_easy_setopt(conn,CURLOPT_USERAGENT,"libcurl-agent/1.0");
  if (!curl_or_die(code,"curl_one: CURLOPT_USERAGENT")) exit(EXIT_FAILURE); 
  // retrieve content
  code = curl_easy_perform(conn);
  if (!curl_or_die(code,"curl_one: easy_preform")) exit(EXIT_FAILURE);
  return store;
}

/* given a string, scrape all torrent names and magnets */
// @in: args : name of item to be searched
// @in: print : verbose logging
// @in: threads : how many threads to utilize in our program
void homura::query_packages(std::string args, bool print){
  /* nyaa.si has no official api, and we must manually
     find out how many pages to parse by sending a request */
  std::string FIRST_ = "https://nyaa.si/?f=0&c=0_0&q=" + args;
  CURL *curl;
  curl_global_init(CURL_GLOBAL_ALL);
  struct memobject FIRST_PAGE = curl_one(curl,FIRST_);
  free(FIRST_PAGE.ptr);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}
