#ifndef __NYAASI_EXTRACTOR_H_
#define __NYAASI_EXTRACTOR_H_

#include "errlib.h"
#include "tree_container.h"
#include "curl_container.h"
#include "url_table.h"
#include <memory>
#include <myhtml/myhtml.h>
#include <unordered_map>

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

    // template functions
    std::vector<std::string> populate_url_list(std::string page);
    name_magnet parse_first_page();
    name_magnet get_magnets(std::string url);
  private:
    // std::shared_ptr<curl_container> curler;
    curl_container curler;
    tree_container html_parser;
    pagination_information pageinfo;
  };
}
#endif
