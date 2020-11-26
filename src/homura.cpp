#include <chrono>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>

#include "homura.h"
#include "select_ssl.h"

#include "nyaasi_extractor.h"

using namespace homura;

homura_instance::homura_instance() 
: scheduler(),
  settings(std::make_unique<homura_settings>())
{
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();
}

homura_instance::homura_instance(homura_settings _settings)
: scheduler(),
  settings(std::make_unique<homura_settings>())
{
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();
  *settings = _settings;
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
  int num_retries = 5;

  std::shared_ptr<url_table<nyaasi_extractor>> 
  tablePtr = scheduler.template getTable<nyaasi_extractor>(settings.get(), 
                                                           key, delay,
                                                           num_retries);
   
  nyaasi_extractor extractor = tablePtr->parser;

  std::vector<generic_torrent_result> firstPageResults;
  if (settings->verbose_mode) {
    fprintf(stderr,"Downloading first page and parsing metadata for %s query %s..."
              ,key.c_str(),searchtag.c_str());
  }
  ret = extractor.downloadFirstPage(searchtag,firstPageResults);

  if (ret != ERRCODE::SUCCESS) return ret; 

  if (settings->verbose_mode) {
    fprintf(stderr,"Complete!\nWaiting for crawl delay...\n"); 
  }

  tablePtr->addNewResults(searchtag,firstPageResults);
  tablePtr->addURLs(searchtag, extractor.getURLs());
  tablePtr->findAndProcessCache(searchtag, 
                                (size_t) extractor.getExpectedResults(),
                                (size_t) extractor.getResultsPerPage());

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

homura_settings *homura_instance::get_settings() 
{
  return settings.get();
}
