#ifndef __URLSCHEDULER_H_
#define __URLSCHEDULER_H_

#include <string>
#include <memory>
#include <iterator>
#include <unordered_map>
#include "url_table.h"

namespace homura 
{
  typedef std::unordered_map<std::string,std::shared_ptr<url_table_base>> urlhash; 
  typedef std::vector<std::shared_ptr<url_table_base>> urlvector;
  class url_scheduler {
  public:
    url_scheduler();

    template <typename parser>
    std::shared_ptr<url_table_base> insert_table (std::string key, std::chrono::milliseconds delay)
    {
      auto find = hashed_url_tables.find(std::string key);
      if (find != hashed_url_tables.end()) {
        return find.second;
      }
      
      auto table = std::make_shared<url_table
    }

    // urlhash::iterator insert_table(std::shared_ptr<url_table_base> to_insert);
    urlvector return_table();

    void print_tables();

    HOMURA_ERRCODE crawl();
  private:
    urlhash hashed_url_tables;
    urlvector sorted_url_tables;
  };
}

#endif
