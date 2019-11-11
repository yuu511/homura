#ifndef __NYAASI_PARSER_H_
#define __NYAASI_PARSER_H_

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
  
  class nyaasi_parser {
  public:
    nyaasi_parser(const std::string first_website);
    HOMURA_ERRCODE extract_pageinfo();
    HOMURA_ERRCODE get_urls(std::shared_ptr <curl_container> curler);
    HOMURA_ERRCODE get_magnets(std::shared_ptr <curl_container> curler);
  private:
    const std::string first_website;
    tree_container html_parser;
    pagination_information pageinfo;
  };
}
#endif
