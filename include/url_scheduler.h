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

    template <typename extractor> std::shared_ptr<url_table<extractor>> find_table (std::string key) 
    {
      auto find = hashed_url_tables.find(key);
      if (find != hashed_url_tables.end()) {
        return std::dynamic_pointer_cast<url_table<extractor>>(find->second);
      }
      return nullptr; 
    }

    template <typename extractor>
    std::shared_ptr<url_table<extractor>> insert_table (std::string key, std::chrono::milliseconds delay)
    {
      auto table = std::make_shared<url_table<extractor>>(key,delay,std::make_shared<extractor>());
      hashed_url_tables[key] = table;

      auto itor = sorted_url_tables.begin();
      while (itor != sorted_url_tables.end()) {
        if (delay.count() > (*itor)->get_delay().count()) {
          break;
        }
        ++itor;
      }
      sorted_url_tables.insert(itor,table);

      return std::dynamic_pointer_cast<url_table<extractor>>(table);
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
