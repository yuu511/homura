#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <curl/curl.h>
#include <string.h>
#include "homura.h"


static std::string store;
// https://curl.haxx.se/libcurl/c/hiperfifo.html
bool curl_code_parser(CURLcode code,std::string where){
  std::string s;
  if (CURLE_OK != code){
    switch(code){
      case CURLE_UNSUPPORTED_PROTOCOL:
        s= "CURLE_UNSUPPORTED_PROTOCOL";
      case CURLE_WRITE_ERROR:
        s= "CURLE_WRITE_ERROR";
      case CURLE_UNKNOWN_OPTION:
        s= "CURLE_UNKNOWN_OPTION";
      case CURLE_OUT_OF_MEMORY:
        s= "CURLE_OUT_OF_MEMORY";
      default: 
        s = "CURLM_unkown"; break;
    }
    std::cerr << boost::format("ERROR: %s returns %s\n") % s % where;
    return false;
  }
  return true;
}

// we decide to not use c style strings out of concern for sanity.
static size_t
WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  ((std::string*)userp)->append((char*)contents,realsize);
  return realsize;
}

std::string homura::curl_one(CURL *&conn, std::string args){
  CURLcode code;
  std::string store;
  conn = curl_easy_init();
  if (!conn) exit(1);
  code = curl_easy_setopt(conn,CURLOPT_URL,args.c_str());
  if (!curl_code_parser(code,"curl_one: CURLOPT_URL")) exit(1); 
  code = curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,WriteCallback);
  if (!curl_code_parser(code,"curl_one: CURLOPT_WRITEFUNCTION")) exit(1); 
  code = curl_easy_setopt(conn,CURLOPT_WRITEDATA,&store);
  if (!curl_code_parser(code,"curl_one: CURLOPT_WRITEDATA")) exit(1); 
  code = curl_easy_setopt(conn,CURLOPT_USERAGENT,"libcurl-agent/1.0");
  if (!curl_code_parser(code,"curl_one: CURLOPT_USERAGENT")) exit(1); 
  // retrieve content
  code = curl_easy_perform(conn);
  if (!curl_code_parser(code,"curl_one: easy_preform")) exit (1);
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
  curl_one(curl,FIRST_);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
}
