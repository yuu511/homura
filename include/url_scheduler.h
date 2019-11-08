#ifndef __URLSCHEDULER_H_
#define __URLSCHEDULER_H_

#include <string>
#include <memory>
#include <unordered_map>
#include "url_table.h"

namespace homura
{
  class url_scheduler {
  public:
    url_scheduler();
    void extract_magnets();
    std::shared_ptr<url_table> 
      get_or_insert(std::string check, 
                    std::chrono::milliseconds delay);
    void print_table();
  private:
    std::vector<std::shared_ptr<url_table>> entries;
    std::unordered_map<std::string,std::shared_ptr<url_table>> entry_hashtable;
    std::vector<std::string> magnets;
  };
}

#endif
