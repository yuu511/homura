#ifndef __TREECONTAINER_H_
#define __TREECONTAINER_H_ 

#include <myhtml/myhtml.h>

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
    tree_container(int threads = 1);
    ~tree_container();

    bool tree_parseHTML(const char *html_page);

    bool parse_pagination_information();
    int pageinfo_first_result();
    int pageinfo_last_result();
    int pageinfo_total_result();

  private:
    myhtml_t *handle;
    myhtml_tree *tree; 
    pagination_information *pageinfo;
  };
}

#endif
