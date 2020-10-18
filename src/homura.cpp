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
  static const std::string key= "nyaa.si";
  std::chrono::milliseconds delay(5000);
  HOMURA_ERRCODE ret = ERRCODE::SUCCESS;

  auto tablePtr = scheduler.template find_table<nyaasi_extractor>(key);
  if (!tablePtr) {
    tablePtr = scheduler.template insert_table<nyaasi_extractor>(key,delay);
  }
   
  auto extractor = tablePtr->parser;
  std::vector<generic_torrent_result> newResults = extractor.downloadFirstPage(searchtag);

  tablePtr->addNewResults(searchtag,newResults);
  tablePtr->addURLs(searchtag,extractor.getURLs());
  // tablePtr->addAnticipatedResults(searchtag,extractor.getExpectedResults());

  return ret;
}

void homura_instance::print_tables() 
{
  scheduler.print_tables();
}
