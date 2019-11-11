#ifndef __TREECONTAINER_H_
#define __TREECONTAINER_H_ 

#include <myhtml/myhtml.h>
#include <chrono>
#include <string>
#include "errlib.h"

namespace homura 
{
  class tree_container {
  public:
    tree_container(int threads = 1);
    ~tree_container();

    HOMURA_ERRCODE create_tree(const char *html_page);

    myhtml_t *get_handle();
    myhtml_tree *get_tree();

  private:
    myhtml_t *handle;
    myhtml_tree *tree; 
  };
}
#endif
