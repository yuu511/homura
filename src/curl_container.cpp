#include <stdexcept>
#include "curl_container.h"
#include "errlib.h"

using namespace homura;

curl_container::curl_container()
  : buffer (std::make_unique<std::vector<unsigned char>>()),
    data_sz(0),
    easyhandle (curl_easy_init()) 
{
    curl_easy_setopt(easyhandle,CURLOPT_WRITEDATA, this);
    curl_easy_setopt(easyhandle,CURLOPT_WRITEFUNCTION,&curl_container::writecb);
    curl_easy_setopt(easyhandle,CURLOPT_USERAGENT,"libcurl-agent/1.0");
}

curl_container::curl_container(const curl_container& c) 
{
  easyhandle = curl_easy_init();
  curl_easy_setopt(easyhandle,CURLOPT_WRITEDATA, this);
  curl_easy_setopt(easyhandle,CURLOPT_WRITEFUNCTION,&curl_container::writecb);
  curl_easy_setopt(easyhandle,CURLOPT_USERAGENT,"libcurl-agent/1.0");
  buffer = std::make_unique<std::vector<unsigned char>>();   
  buffer->resize(c.buffer->size());
  std::copy(c.buffer->begin(),c.buffer->end(),buffer->begin());
  data_sz = c.data_sz;
  response = c.response;
  time_sent = c.time_sent;
}


curl_container::~curl_container() 
{
  curl_easy_cleanup(easyhandle);
}


const char *curl_container::get_HTML_aschar() 
{
  return reinterpret_cast<const char*>(buffer->data());
}

std::chrono::steady_clock::time_point curl_container::get_time_sent() 
{
  return time_sent;
}

bool check_curlcode(CURLcode code) 
{
  std::string s;
  if (CURLE_OK != code) {
    switch(code) {
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
    errprintf (ERRCODE::FAILED_CURL,"ERROR: curl failed with error %s", s.c_str());
    return false;
  }
  return true;
}

size_t curl_container::writecb(const unsigned char *ptr, 
                               size_t size, 
                               size_t nmemb,
                               void *userp) 
{
  try {
    curl_container *data = static_cast<curl_container*>(userp); 
    size_t len = size * nmemb;
    data->buffer->resize (data->data_sz + len + 1);
    std::copy(ptr, ptr + len, data->buffer->begin() + data->data_sz);
    (*data->buffer)[data->data_sz + len] = '\0';
    data->data_sz += len;
    return len;
  }
  catch (std::bad_alloc &ba) {
    errprintf(ERRCODE::FAILED_NEW, "ERROR: gg oom\n(out of memory)\n");
  }
  return 0;
}

HOMURA_ERRCODE curl_container::perform_curl(const std::string &url) 
{
  buffer.reset(); 
  buffer = std::make_unique<std::vector<unsigned char>>();
  data_sz = 0;
  time_sent = std::chrono::steady_clock::now();

  curl_easy_setopt(easyhandle,CURLOPT_URL,url.c_str());
  
  bool pass = check_curlcode(curl_easy_perform(easyhandle));

  if (pass) {
    if (homura::options::debug_level > 1) {
      fprintf (stdout,"== Size of string ==\n");
      fprintf (stdout,"sizeof string %zd\n\n",
               buffer->size());
      fprintf (stdout,"%s\n\n", get_HTML_aschar());
    }
  }
  else {
    errprintf(ERRCODE::FAILED_CURL,"Curl failed at url %s",url.c_str());
    return ERRCODE::FAILED_CURL;
  }

  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE curl_container::try_curl_N(const std::string &url,int attempts) 
{
  while (attempts) {
    if (perform_curl(url) == ERRCODE::SUCCESS) return ERRCODE::SUCCESS;
    --attempts;
  }
  return ERRCODE::FAILED_CURL;
}
