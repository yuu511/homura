#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <memory>

#include "url_table.h"
#include "url_scheduler.h"
#include "torrent_parser.h"
#include "magnet_table.h"
#include "errlib.h"

using milliseconds = std::chrono::milliseconds;

namespace homura 
{
  enum website {
    nyaasi, 
    nyaapantsu, 
    horriblesubs
  };

  class homura_instance {
  public:
    homura_instance();
    ~homura_instance();
    
    HOMURA_ERRCODE crawl();
    HOMURA_ERRCODE query_nyaasi(std::string args);
  private:
    magnet_table *results;
    torrent_parser torrenter;
    url_scheduler scheduler;
  };
}

#endif
