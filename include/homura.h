#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <memory>

#include "url_table.h"
#include "url_scheduler.h"
#include "errlib.h"

namespace homura 
{
  class homura_instance {
  public:
    homura_instance();
    homura_instance(homura_settings _settings);
    ~homura_instance();
    
    HOMURA_ERRCODE query_nyaasi(std::string args);

    HOMURA_ERRCODE crawl();
    void print_tables();
    void wait_at_end();
    homura_settings *get_settings();
  private:
    url_scheduler scheduler;
    std::unique_ptr<homura_settings> settings;
  };
}

#endif
