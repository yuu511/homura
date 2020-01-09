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
#include "errlib.h"

namespace homura 
{
  class homura_instance {
  public:
    homura_instance();
    ~homura_instance();
    
    HOMURA_ERRCODE crawl();
    HOMURA_ERRCODE query_nyaasi(std::string args);
    void print_tables();
  private:
    torrent_parser torrenter;
    url_scheduler scheduler;
  };
}

#endif
