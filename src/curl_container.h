#ifndef __CURLCONTAINER_H_
#define __CURLCONTAINER_H_ 

#include <curl/curl.h>
#include <vector>
#include <string>
#include <chrono>
#include <memory>

namespace homura {
  class curl_container {
  public:
    curl_container();
    ~curl_container();
    curl_container(const curl_container&) = delete;
    curl_container &operator=(const curl_container&) = delete;
    void clear();
    

    std::shared_ptr<std::vector<unsigned char>> get_HTML();
    const char *get_HTML_aschar(); 
    size_t get_data_sz();
    std::chrono::steady_clock::time_point get_time_sent();

    bool curlcode_pass(CURLcode code,std::string where);
    bool perform_curl(const std::string &url);
  private:
    static size_t writecb( const unsigned char *ptr, size_t size, size_t nmemb, void *s ); 
    static const char *user_agent;
    std::shared_ptr<std::vector<unsigned char>> buffer;
    size_t data_sz;
    CURL *easyhandle;
    CURLcode response;
    std::chrono::steady_clock::time_point time_sent;
  };
}

#endif
