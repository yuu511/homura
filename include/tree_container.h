#ifndef __TREECONTAINER_H_
#define __TREECONTAINER_H_ 

#include <myhtml/myhtml.h>
#include <chrono>
#include <string>
#include "errlib.h"

namespace homura 
{
  struct pagination_information {
    pagination_information(int first_result,int last_result,int total_result);
    int first_result;
    int last_result;
    int total_result;
  };

  class tree_container {
  public:
    tree_container(int threads = 1);
    ~tree_container();

    HOMURA_ERRCODE parse_HTML(const char *html_page);

    HOMURA_ERRCODE nyaasi_extract_pageinfo();
    std::vector<std::string> nyaasi_parse_torrents();

    int nyaasi_pageinfo_first();
    int nyaasi_per_page();
    int nyaasi_pages_total();

  private:
    myhtml_t *handle;
    myhtml_tree *tree; 
    pagination_information nyaasi_pageinfo;
  };

}

#endif