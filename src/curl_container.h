#ifndef __CURLCONTAINER_H_
#define __CURLCONTAINER_H_ 

#include <curl/curl.h>
#include <vector>
#include <string>
#include <chrono>

namespace homura 
{
  class curl_container
  {
  public:
    curl_container( const std::string &url );
    ~curl_container();

    const std::string get_url();
    std::vector<unsigned char> *get_HTML();
    const char *get_HTML_char(); 
    size_t get_data_sz();
    std::chrono::steady_clock::time_point get_time_sent();

    bool curlcode_pass(CURLcode code,std::string where);
    bool perform_curl();
  private:
    static size_t writecb( const unsigned char *ptr, size_t size, size_t nmemb, void *s ); 
    static const char *user_agent;
    std::vector<unsigned char> *buffer;
    size_t data_sz;
    const std::string url;
    CURL *easyhandle;
    CURLcode response;
    std::chrono::steady_clock::time_point time_sent;
  };
}

#endif
