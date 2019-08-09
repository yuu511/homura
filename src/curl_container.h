#ifndef __CURLCONTAINER_H_
#define __CURLCONTAINER_H_ 

#include <curl/curl.h>
#include <vector>
#include <string>

namespace homura 
{
  class curl_container
  {
  public:
    curl_container(std::string url);
    ~curl_container();

   std::string get_url();
   std::vector<unsigned char> *get_HTML();
   const char *get_HTML_char(); 
   size_t get_data_sz();

   bool curlcode_pass(CURLcode code,std::string where);
   bool perform_curl();
  private:
    static size_t writecb(const unsigned char *ptr, size_t size, size_t nmemb, void *s); 
    static const char *user_agent;
    size_t data_sz;
    CURL *easyhandle;
    CURLcode response;
    std::string url;
    std::vector<unsigned char> *buffer;
  };
}

#endif
