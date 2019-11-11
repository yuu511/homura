#ifndef __NYAASI_PARSER_H_
#define __NYAASI_PARSER_H_

#include "errlib.h"
#include <myhtml/myhtml.h>

namespace homura {
  struct pagination_information {
    pagination_information(int first_result,int last_result,int total_result);
    int first_result;
    int last_result;
    int total_result;
  };
  
  class nyaasi_parser {
  public:
    nyaasi_parser(myhtml_tree *tree);
    HOMURA_ERRCODE extract_pageinfo();
    std::vector<std::string> extract_magnets();
  private:
    myhtml_tree *tree;
    pagination_information pageinfo;
  };
}
#endif
