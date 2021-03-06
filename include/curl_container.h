#ifndef __CURLCONTAINER_H_
#define __CURLCONTAINER_H_ 

#include <curl/curl.h>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include "errlib.h"

namespace homura 
{
  class curl_container {
  public:
    curl_container();
    ~curl_container();
    curl_container(const curl_container&);
    curl_container &operator= (const curl_container&) = delete;
    curl_container(curl_container&&);
    
    const char *get_HTML_aschar(); 
    std::chrono::steady_clock::time_point get_time_sent();

    HOMURA_ERRCODE perform_curl(const std::string url);
    HOMURA_ERRCODE try_curl_N(const std::string url,int attempts);
  private:
    static size_t writecb( const unsigned char *ptr, size_t size, size_t nmemb, void *s ); 
    std::unique_ptr<std::vector<unsigned char>> buffer;
    size_t data_sz;
    CURL *easyhandle;
    CURLcode response;
    std::chrono::steady_clock::time_point time_sent;
  };
}

#endif
