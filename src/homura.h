#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <memory>

#include "magnet_table.h"
#include "url_table.h"
#include "torrent_parser.h"

using milliseconds = std::chrono::milliseconds;

namespace homura {
  enum website {
    nyaasi, 
    nyaapantsu, 
    horriblesubs
  };

  class homura_instance {
  public:
    homura_instance();
    ~homura_instance();
    
    void crawl();
    bool query_nyaasi(std::string args);
  private:
    magnet_table *results;
    std::vector<std::shared_ptr<homura::url_table>> requests;
    std::unordered_map<int,std::shared_ptr<homura::url_table>> requests_hash;
    torrent_parser torrenter;
   
    // helper methods
    std::shared_ptr<homura::url_table> get_table(int website, std::chrono::milliseconds delay);
  };
}

#endif
