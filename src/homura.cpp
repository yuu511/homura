#include <algorithm>
#include <chrono>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>
#include <algorithm>

#include "homura.h"
#include "nyaasi_extractor.h"
#include "select_ssl.h"

using namespace homura;

homura_instance::homura_instance() 
  : torrenter(),
    scheduler()
{
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();
}

homura_instance::~homura_instance() 
{
  curl_global_cleanup();
}

HOMURA_ERRCODE homura_instance::crawl() 
{
  scheduler.crawl();
  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE homura_instance::query_nyaasi(std::string searchtag) 
{
  const std::string key= "nyaa.si";

  auto iterator = scheduler.table_position(key);
  if (scheduler.exists_in_table(iterator)) {
    auto it = iterator->second;
    it->populate_url_list(searchtag);
  } 
  else {
    auto new_table = std::make_shared <url_table<nyaasi_extractor>>
                     (key,
                      std::chrono::milliseconds(5000),
                      nyaasi_extractor());
    scheduler.insert_table(new_table);
    new_table->populate_url_list(searchtag);
  }
  return ERRCODE::SUCCESS;
}

void homura_instance::print_tables() 
{
  scheduler.print_tables();
}
