#ifndef __NYAASI_EXTRACTOR_H_
#define __NYAASI_EXTRACTOR_H_

#include "errlib.h"
#include "tree_container.h"
#include "curl_container.h"
#include "url_table.h"
#include <memory>
#include <myhtml/myhtml.h>
#include <utility>
#include <deque>

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
    HOMURA_ERRCODE downloadPage(std::string URL, std::vector<generic_torrent_result> &results); 
    HOMURA_ERRCODE downloadFirstPage(std::string searchtag,std::vector<generic_torrent_result> &results);
    const char *curlHTML(std::string URL);
    HOMURA_ERRCODE generateURLs();
    std::deque<std::string> getURLs();
    inline HOMURA_ERRCODE getTorrents(std::string URL, std::vector<generic_torrent_result> &result);
    int getExpectedResults();
    int getResultsPerPage();
  private:
    curl_container curler;
    tree_container html_parser;
    pagination_information pageinfo;
    std::string ref_page;
    std::deque<std::string> URLs;
    HOMURA_ERRCODE parseMetadata();
  };
}
#endif
