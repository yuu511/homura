#ifndef __URLSCHEDULER_H_
#define __URLSCHEDULER_H_

#include <string>
#include <memory>
#include <iterator>
#include <unordered_map>
#include "url_table.h"

namespace homura
{
  typedef std::unordered_map<std::string,std::shared_ptr<url_table>>::iterator table_iterator;
  class url_scheduler {
  public:
    url_scheduler();
    void extract_magnets();
    table_iterator get_table_position(std::string check);
    bool table_exists(table_iterator check); 
    table_iterator insert_table(std::string check, std::chrono::milliseconds delay);
    void print_table();
    void crawl();
  private:
    std::vector<std::shared_ptr<url_table>> entries;
    std::unordered_map<std::string,std::shared_ptr<url_table>> entry_hashtable;
    std::vector<std::string> magnets;
  };
}

#endif
