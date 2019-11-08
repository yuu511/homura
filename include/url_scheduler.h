#ifndef __URLSCHEDULER_H_
#define __URLSCHEDULER_H_

#include <string>
#include "url_table.h"

namespace homura
{
  class url_scheduler {
  public:
    url_scheduler();
    void extract_magnets();
    void insert_entry_back(url_table new_entry);   
    url_table get_or_insert(std::string check, 
                             std::chrono::milliseconds delay) ;
  private:
    std::vector<url_table> entries;
    std::vector<std::string> magnets;
  };
}

#endif
