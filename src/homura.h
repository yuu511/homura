#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <string>
#include <curl/curl.h>

namespace homura{
  // query all results for search term
  void query_packages(std::string args, bool print);
  std::string curl_one(CURL *&conn, std::string args);
}
#endif
