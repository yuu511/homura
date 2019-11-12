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
    tree_container();
    ~tree_container();
    tree_container(const tree_container& c);

    HOMURA_ERRCODE create_tree(const char *html_page);
    void reset_tree();

    myhtml_t *get_handle();
    myhtml_tree *get_tree();
    int get_threads();

    void set_threads(int threads);
  private:
    int threads = 1;
    myhtml_t *handle;
    myhtml_tree *tree; 
  };
}
#endif
