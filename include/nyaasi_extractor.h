#ifndef __NYAASI_EXTRACTOR_H_
#define __NYAASI_EXTRACTOR_H_

#include "errlib.h"
#include "tree_container.h"
#include "curl_container.h"
#include <myhtml/myhtml.h>

namespace homura {
  struct pagination_information {
    pagination_information(int first_result,int last_result,int total_result);
    int first_result;
    int last_result;
    int total_result;
  };
  
  class nyaasi_extractor {
  public:
    nyaasi_extractor(const std::string base_url);
    HOMURA_ERRCODE extract_pageinfo();
    HOMURA_ERRCODE get_urls(std::shared_ptr <curl_container> curler);
    HOMURA_ERRCODE get_magnets(std::shared_ptr <curl_container> curler);
  private:
    const std::string base_url;
    tree_container html_parser;
    pagination_information pageinfo;
  };
}
#endif
