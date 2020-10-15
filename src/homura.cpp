#include <chrono>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>

#include "homura.h"
#include "select_ssl.h"

#include "nyaasi_extractor.h"
#include "nyaasi_results.h"

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
  std::chrono::milliseconds delay(5000);
  HOMURA_ERRCODE ret = ERRCODE::SUCCESS;

  auto nyaaparser = std::make_shared<nyaasi_extractor>(); 
  auto newResults = nyaaparser->downloadFirstPage(searchtag); // download metadata and first page

  auto table = std::make_shared<url_table<nyaasi_extractor>>(key,delay,nyaaparser);
  table->addURLs(searchtag,nyaaparser->getURLs());
  table->addExpectedResults(nyaaparser->getExpectedResults());
  table->addNewResults(std::make_pair(searchtag,newResults));

  // nyaaparser.downloadFirstPage(searchtag);
  // auto iterator = scheduler.table_position(key);
  // if (scheduler.exists_in_table(iterator)) {
  //   auto it = iterator->second;
  //   ret = it->populate_url_list(searchtag);
  // } 
  // else {
  //   auto new_table = std::make_shared <url_table<nyaasi_extractor,nyaasi_results>>
  //                    (key,
  //                     std::chrono::milliseconds(5000),
  //                     nyaasi_extractor());
  //   scheduler.insert_table(new_table);
  //   ret = new_table->populate_url_list(searchtag);
  // }
  return ret;
}

void homura_instance::print_tables() 
{
  scheduler.print_tables();
}
