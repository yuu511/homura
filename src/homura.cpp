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
  : //results(nullptr),
    torrenter(),
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

HOMURA_ERRCODE homura_instance::query_nyaasi(std::string args) 
{
  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string base_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;

  std::string key= "nyaa.si";

  auto iterator = scheduler.table_position(key);
  if (scheduler.exists_in_table(iterator)) {
    iterator->second->get_urls(base_url);
  } 
  else {
    auto new_extractor = std::make_shared<nyaasi_extractor>();
    auto new_table = std::make_shared
    <url_table<nyaasi_extractor>>(key,
                                  std::chrono::milliseconds(5000),
                                  new_extractor);
    scheduler.insert_table(new_table);
    new_table->get_urls(base_url);
  }
  return ERRCODE::SUCCESS;
}
