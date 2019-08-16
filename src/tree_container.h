#ifndef __TREECONTAINER_H_
#define __TREECONTAINER_H_ 

#include <myhtml/myhtml.h>
#include <chrono>

namespace homura 
{
  struct pagination_information {
    pagination_information(int first_result,int last_result,int total_result);
    int first_result;
    int last_result;
    int total_result;
  };

  class tree_container
  {
  public:
    tree_container(std::chrono::steady_clock::time_point time_sent, int threads = 1);
    ~tree_container();

    bool tree_parseHTML(const char *html_page);
    bool parse_pagination_information();

    std::chrono::steady_clock::time_point get_time_sent();
    int get_pageinfo_first_result();
    int get_pageinfo_last_result();
    int get_pageinfo_total_result();

  private:
    myhtml_t *handle;
    myhtml_tree *tree; 
    std::chrono::steady_clock::time_point time_sent;
    pagination_information *pageinfo;
  };
}

#endif
