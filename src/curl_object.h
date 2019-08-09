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

   //  void set_url(std::string url);
   std::string get_url();
   std::vector<unsigned char> get_buffer();
   size_t get_data_sz();
   //  std::string get_buffer();
   bool perform_curl();
   bool curlcode_pass(CURLcode code,std::string where);
  private:
    static size_t writecb(const unsigned char *ptr, size_t size, size_t nmemb, void *s); 
    static const char *user_agent;
    size_t data_sz;
    CURL *easyhandle;
    CURLcode response;
    std::string url;
    std::vector<unsigned char> buffer;
  };
}

#endif
