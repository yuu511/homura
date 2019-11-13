#ifndef __NYAASI_EXTRACTOR_H_
#define __NYAASI_EXTRACTOR_H_

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
  
  class nyaasi_extractor {
  public:
    nyaasi_extractor();
    HOMURA_ERRCODE curl_and_create_tree(std::string url);
    HOMURA_ERRCODE extract_pageinfo();
    std::vector<std::string> extract_tree_magnets();

    // template functions
    std::vector<std::string> populate_url_list(std::string page);
    std::vector<std::string> get_magnets(std::string url);
    std::vector<std::string> parse_first_page();
  private:
    std::shared_ptr<curl_container> curler;
    tree_container html_parser;
    pagination_information pageinfo;
  };
}
#endif
