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

    template <typename extractor> 
    std::shared_ptr<url_table<extractor>> getTable(homura_settings *settings,
                                                   std::string key,
                                                   std::chrono::milliseconds delay,
                                                   int _num_retries = 0)
    {
      auto find = hashed_url_tables.find(key);
      if (find != hashed_url_tables.end()) {
        return std::dynamic_pointer_cast<url_table<extractor>>(find->second);
      }

      std::shared_ptr<url_table<extractor>> newtable = 
        std::make_shared<url_table<extractor>> (settings,
                                                key, delay,
                                                _num_retries, extractor());

      hashed_url_tables.emplace(std::make_pair (key, newtable));

      auto itor = sorted_url_tables.begin();
      while (itor != sorted_url_tables.end()) {
        if (delay.count() > (*itor)->get_delay().count()) {
          break;
        }
        ++itor;
      }
      sorted_url_tables.insert(itor,newtable);

      return newtable; 
   }

    urlvector return_table();

    void print_tables();
    HOMURA_ERRCODE crawl();
    void delay_end();
  private:
    urlhash hashed_url_tables;
    urlvector sorted_url_tables;
  };
}

#endif
