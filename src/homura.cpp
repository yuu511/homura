#include <algorithm>
#include <chrono>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>

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
  auto table = scheduler.return_table();
  for (auto itor : table) {
    std::vector magnets = itor->get_magnets();
    for (auto entry : magnets) {
      torrenter.extract_magnet_information(entry);
    }
  }
  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE homura_instance::query_nyaasi(std::string args) 
{
  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string base_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;

  std::string key= "nyaa.si";

  auto iterator = scheduler.table_position(key);
  if (scheduler.exists_in_table(iterator)) {
    iterator->second->populate_url_list(base_url);
  } 
  else {
    auto new_extractor = std::make_shared<nyaasi_extractor>();
    auto new_table = std::make_shared
    <url_table<nyaasi_extractor>>(key,
                                  std::chrono::milliseconds(5000),
                                  new_extractor);
    scheduler.insert_table(new_table);
    new_table->populate_url_list(base_url);
  }
  return ERRCODE::SUCCESS;
}
