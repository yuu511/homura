#include <stdexcept>
#include "curl_container.h"
#include "errlib.h"

using namespace homura;
const char *curl_container::user_agent = "libcurl-agent/1.0";

std::string curl_container::get_url()
{
  return url;
}

std::vector<unsigned char> curl_container::get_buffer()
{
  return buffer;
}

const char *curl_container::get_buffer_char()
{
  return reinterpret_cast<const char*>(buffer.data());
}

size_t curl_container::get_data_sz()
{
  return data_sz;
}

bool curl_container::curlcode_pass( CURLcode code,std::string where )
{
  std::string s;
  response = code;
  if ( CURLE_OK != code )
  {
    switch( code )
    {
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
    errprintf ( ERRCODE::FAILED_CURL,"ERROR: %s returns %s\n",where.c_str(), s.c_str() );
    return false;
  }
  return true;
}

size_t curl_container::writecb(const unsigned char *ptr, size_t size, size_t nmemb, void *userp){
  curl_container *data = static_cast<curl_container*>(userp); 
  size_t len = size * nmemb;
  data->buffer.resize (data->data_sz + len + 1);
  std::copy(ptr, ptr + len, data->buffer.begin() + data->data_sz);
  data->buffer[data->data_sz + len] = '\0';
  data->data_sz += len;
  return len;
}

curl_container::curl_container( std::string url ){
  CURLcode code;
  this->easyhandle = curl_easy_init();  
  this->url = url;
  data_sz = 0;
  try 
  {
     code = curl_easy_setopt(easyhandle,CURLOPT_URL,url.c_str());
     if (!curlcode_pass(code, "curl_container: CURLOPT_URL" )) throw std::runtime_error("CURLOPT_URL");

     code = curl_easy_setopt(easyhandle,CURLOPT_WRITEFUNCTION,&curl_container::writecb);
     if (!curlcode_pass(code,"curl_one: CURLOPT_WRITEFUNCTION")) throw std::runtime_error("CURLOPT_WRITEFUNCTION");

     code = curl_easy_setopt(easyhandle,CURLOPT_WRITEDATA, this);
     if (!curlcode_pass(code,"curl_one: CURLOPT_WRITEDATA")) throw std::runtime_error("CURLOPT_WRITEDATA");

     code = curl_easy_setopt(easyhandle,CURLOPT_USERAGENT,"libcurl-agent/1.0");
     if (!curlcode_pass(code,"curl_one: CURLOPT_USERAGENT")) throw std::runtime_error("CURLOPT_USERAGENT");

     response = code;
  }
  catch ( const std::runtime_error &e ) 
  {
    errprintf(ERRCODE::FAILED_CURL, "CURL initialization exited at %s\n",e);
    return;
  }
}

bool curl_container::perform_curl()
{
  buffer.clear(); 
  data_sz = 0;
  CURLcode code = curl_easy_perform(easyhandle);
  return CURLE_OK == code;
}

curl_container::~curl_container(){
  if (easyhandle)
    curl_easy_cleanup(easyhandle);
}
