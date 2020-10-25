#include <chrono>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>

#include "homura.h"
#include "select_ssl.h"

#include "nyaasi_extractor.h"

using namespace homura;

homura_instance::homura_instance() 
  : scheduler()
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
  HOMURA_ERRCODE ret = ERRCODE::SUCCESS;
  static const std::string key= "nyaa.si";
  std::chrono::milliseconds delay(5000);

  auto tablePtr = scheduler.template find_table<nyaasi_extractor>(key);
  if (!tablePtr) {
    tablePtr = scheduler.template insert_table<nyaasi_extractor>(key,delay);
  }
   
  nyaasi_extractor extractor = tablePtr->parser;
  std::vector<generic_torrent_result> newResults = extractor.downloadFirstPage(searchtag);

  tablePtr->addNewResults(searchtag,newResults);
  tablePtr->processURLs_Cache( searchtag, extractor.getURLs(),
                               (size_t)extractor.getExpectedResults(),(size_t)extractor.getResultsPerPage());

  return ret;
}

void homura_instance::print_tables() 
{
  scheduler.print_tables();
}

void homura_instance::wait_at_end()
{
  scheduler.delay_end();
}
