#ifndef __TREECONTAINER_H_
#define __TREECONTAINER_H_ 

#include <myhtml/myhtml.h>
#include <chrono>

namespace homura {
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
    tree_container(const tree_container&) = delete; 
    tree_container &operator=(const tree_container&) = delete;

    bool parse_HTML(const char *html_page);

    bool parse_nyaasi_pageinfo();
    bool parse_nyaasi_torrents();

    int pageinfo_first();
    int pageinfo_results_per_page();
    int pageinfo_total();

  private:
    myhtml_t *handle;
    myhtml_tree *tree; 
    pagination_information pageinfo;
  };

}

#endif
