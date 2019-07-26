#include <string>
#include <iostream>
#include <algorithm>
#include <boost/format.hpp>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "homura.h"

// objective: write mostly* c for curl/parsing, c++ for libtorrent 
// *multithreading (boost::asio) + string manipulation(stdio,string) will be done by c++

/* terminate program immediately if curl reports an error */
// @in CURLcode to parse
// @in string to indicated the calling location
// https://curl.haxx.se/libcurl/c/hiperfifo.html
void sucess_or_exit(CURLcode code,std::string where){
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
    exit(EXIT_FAILURE);
  }
}

/* container for html data */
struct memobject {
  char *ptr;
  size_t len;
}; 

void memobject_init(struct memobject *s){
  s->len = 0;
  s->ptr = (char *) malloc(1);
  if (s->ptr == nullptr){
    std::cerr << "malloc failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

/* callback function write webpage to memory */
static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, struct memobject *s) {
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

/* download one webpage */
// @in curl handle
// @in name of webpage to download
struct memobject curl_one(CURL *&conn, std::string args){
  CURLcode code;
  conn = curl_easy_init();
  struct memobject store;
  memobject_init(&store);
  if (!conn) exit(EXIT_FAILURE);
  // add URL to curl handle
  code = curl_easy_setopt(conn,CURLOPT_URL,args.c_str());
  sucess_or_exit(code,"curl_one: CURLOPT_URL");
  // determine the function that will handle the data
  code = curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,WriteCallback);
  sucess_or_exit(code,"curl_one: CURLOPT_WRITEFUNCTION");
  // determine the location that the data will be stored
  code = curl_easy_setopt(conn,CURLOPT_WRITEDATA,&store);
  sucess_or_exit(code,"curl_one: CURLOPT_WRITEDATA");
  // some websites require a user-agent name.
  code = curl_easy_setopt(conn,CURLOPT_USERAGENT,"libcurl-agent/1.0");
  sucess_or_exit(code,"curl_one: CURLOPT_USERAGENT");
  // retrieve content
  code = curl_easy_perform(conn);
  sucess_or_exit(code,"curl_one: easy_preform");
  return store;
}

/* given a string, scrape all torrent names and magnets */
// @in: args : name of item to be searched
// @in: print : verbose logging
// @in: threads : how many threads to utilize in our program
void homura::query_packages(std::string args, int LOG_LEVEL, int threads = 1){
  /* nyaa.si has no official api, and we must manually
     find out how many pages to parse by sending a request */
  std::replace(args.begin(),args.end(),' ','+');
  std::string FIRST_ = "https://nyaa.si/?f=0&c=0_0&q=" + args; 
  CURL *curl;
  curl_global_init(CURL_GLOBAL_ALL);
  struct memobject FIRST_PAGE = curl_one(curl,FIRST_);
  if (LOG_LEVEL){
    std::cout << " == FIRST_PAGE_URL == \n";
    std::cout << FIRST_ << std::endl;
    if (LOG_LEVEL>1){
      std::cout << std::string(FIRST_PAGE.ptr);
    }
  }
  free(FIRST_PAGE.ptr);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}
