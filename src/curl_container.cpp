#include <stdexcept>
#include "curl_container.h"
#include "errlib.h"

using namespace homura;
const char *curl_container::user_agent = "libcurl-agent/1.0";

std::shared_ptr<std::vector<unsigned char>> curl_container::get_HTML() {
  return buffer;
}

const char *curl_container::get_HTML_aschar() {
  if (!this->data_sz)
    return nullptr;
  return reinterpret_cast<const char*>(buffer->data());
}

size_t curl_container::get_data_sz() {
  return data_sz;
}

std::chrono::steady_clock::time_point curl_container::get_time_sent() {
  return time_sent;
}

bool curl_container::curlcode_pass(CURLcode code,std::string where) {
  std::string s;
  response = code;
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
    errprintf (ERRCODE::FAILED_CURL,"ERROR: %s returns %s\n",where.c_str(), s.c_str());
    return false;
  }
  return true;
}

size_t curl_container::writecb(const unsigned char *ptr, size_t size, size_t nmemb, void *userp) {
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

curl_container::curl_container()
  : buffer (std::make_shared<std::vector<unsigned char>>()),
    data_sz(0),
    easyhandle (curl_easy_init()) {

  CURLcode code;
  try {
     code = curl_easy_setopt(easyhandle,CURLOPT_WRITEDATA, &*this);
     if (!curlcode_pass(code,"curl_one: CURLOPT_WRITEDATA")) throw std::runtime_error("CURLOPT_WRITEDATA");

     code = curl_easy_setopt(easyhandle,CURLOPT_WRITEFUNCTION,&curl_container::writecb);
     if (!curlcode_pass(code,"curl_one: CURLOPT_WRITEFUNCTION")) throw std::runtime_error("CURLOPT_WRITEFUNCTION");

     code = curl_easy_setopt(easyhandle,CURLOPT_USERAGENT,"libcurl-agent/1.0");
     if (!curlcode_pass(code,"curl_one: CURLOPT_USERAGENT")) throw std::runtime_error("CURLOPT_USERAGENT");

     response = code;
  }
  catch ( const std::runtime_error &e ) {
    errprintf(ERRCODE::FAILED_CURL, "CURL initialization exited at %s\n",e);
    return;
  }
}

bool curl_container::perform_curl(const std::string &url) {
  CURLcode code;
  try {
     buffer.reset();
     buffer = std::make_unique<std::vector<unsigned char>>();
     data_sz = 0;

     code = curl_easy_setopt(easyhandle,CURLOPT_URL,url.c_str());
     if (!curlcode_pass(code, "curl_container: CURLOPT_URL" )) throw std::runtime_error("CURLOPT_URL");
  }
  catch ( const std::runtime_error &e ) {
    errprintf(ERRCODE::FAILED_CURL, "CURL initialization exited at %s\n",e);
    return false;
  }
  
  this->time_sent = std::chrono::steady_clock::now();
  bool pass = CURLE_OK == curl_easy_perform(easyhandle);

  if (pass) {
    if (homura::options::debug_level) {
      fprintf (stdout,"== Size of data / size of string ==\n");
      fprintf (stdout,"sizeof data: %zd\nsizeof string %zd\n\n",
        this->get_HTML()->size(),this->get_data_sz());
    }
    if (homura::options::debug_level > 2) {
      fprintf (stdout,"%s\n\n", this->get_HTML_aschar());
    }
  }

  return pass;
}

void curl_container::clear() {
  curl_easy_cleanup(easyhandle);
}

curl_container::~curl_container() {
  clear();
}
