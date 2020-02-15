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

    nyaasi_extractor(const nyaasi_extractor&) = delete;
    nyaasi_extractor &operator= (const nyaasi_extractor&) = delete;
    nyaasi_extractor(nyaasi_extractor&&);

    HOMURA_ERRCODE curl_and_create_tree(std::string url);
    HOMURA_ERRCODE extract_pageinfo();
    int gen_num_cached_pages(torrent_map_entry magnets);

    // template functions
    std::vector<std::string> getURLs(int cached_pages, std::string ref_page);
    const char *downloadOne(std::string url);
    torrent_map_entry parse_HTML(const char *HTML);
    torrent_map_entry parse_first_page();
  private:
    curl_container curler;
    tree_container html_parser;
    pagination_information pageinfo;
    int cached_pages;
  };
}
#endif
