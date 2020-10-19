#ifndef __NYAASI_EXTRACTOR_H_
#define __NYAASI_EXTRACTOR_H_

#include "errlib.h"
#include "tree_container.h"
#include "curl_container.h"
#include "url_table.h"
#include <memory>
#include <myhtml/myhtml.h>
#include <utility>

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
    std::vector<generic_torrent_result> downloadPage(std::string URL); 
    std::vector<generic_torrent_result> downloadFirstPage(std::string searchtag);
    const char *curlHTML(std::string URL);
    HOMURA_ERRCODE generateURLs();
    std::vector<std::string> getURLs();
    inline std::vector<generic_torrent_result> getTorrents(std::string URL);
    int getExpectedResults();
  private:
    curl_container curler;
    tree_container html_parser;
    pagination_information pageinfo;
    std::string ref_page;
    std::vector<std::string> URLs;
    HOMURA_ERRCODE parseMetadata();
  };
}
#endif
