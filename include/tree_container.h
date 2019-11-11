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
    tree_container(std::string website, int threads = 1);
    virtual ~tree_container();

    HOMURA_ERRCODE create_tree(const char *html_page);

    virtual std::vector<std::string> parse_torrents();

    myhtml_t *get_handle();
    myhtml_tree *get_tree();
    std::string get_website();

  private:
    std::string website;
    myhtml_t *handle;
    myhtml_tree *tree; 
  };

  struct pagination_information {
    pagination_information(int first_result,int last_result,int total_result);
    int first_result;
    int last_result;
    int total_result;
  };

  class nyaasi_tree : public tree_container {
  public:
    nyaasi_tree(int threads = 1);
    ~nyaasi_tree();

    HOMURA_ERRCODE nyaasi_extract_pageinfo();
    std::vector<std::string> parse_torrents();
    int pageinfo_first();
    int per_page();
    int pages_total();
  private:
    pagination_information nyaasi_pageinfo;
  };
}
#endif
