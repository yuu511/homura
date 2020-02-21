#ifndef __NYAASI_EXTRACTOR_H_
#define __NYAASI_EXTRACTOR_H_

#include "nyaasi_results.h"
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

    nyaasi_extractor(const nyaasi_extractor&) = delete;
    nyaasi_extractor &operator= (const nyaasi_extractor&) = delete;
    nyaasi_extractor(nyaasi_extractor&&);

    // template functions
    std::vector<nyaasi_results> *parse_HTML(const char *HTML); 
    urlpair download_first_page(std::string searchtag);
    std::vector<std::string> getURLs(const char *firstHTML);
    const char *downloadOne(std::string url);

  private:
    curl_container curler;
    tree_container html_parser;
    pagination_information pageinfo;
    HOMURA_ERRCODE extract_pageinfo();
    std::string ref_page;
  };
}
#endif
