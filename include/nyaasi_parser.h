#ifndef __NYAASI_PARSER_H_
#define __NYAASI_PARSER_H_

#include "errlib.h"
#include "tree_container.h"
#include "curl_container.h"
#include <memory>
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

    HOMURA_ERRCODE curl_and_create_tree(const std::string url);
    HOMURA_ERRCODE extract_pageinfo();
    std::vector<std::string> extract_tree_magnets();

    // template functions
    std::vector<std::string> get_urls();
    std::vector<std::string> get_magnets(const std::string url);
  private:
    std::shared_ptr<curl_container> curler;
    const std::string first_website;
    tree_container html_parser;
    pagination_information pageinfo;
  };
}
#endif
