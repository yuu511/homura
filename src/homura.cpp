#include <algorithm>
#include <chrono>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>

#include "homura.h"
#include "nyaasi_parser.h"
#include "select_ssl.h"

using namespace homura;

homura_instance::homura_instance() 
  : results(nullptr),
    torrenter()
    // scheduler()
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
//   scheduler.crawl();
  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE homura_instance::query_nyaasi(std::string args) 
{
  int status;
  std::string key= "nyaa.si";
  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string base_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;
  nyaasi_parser new_parser = nyaasi_parser(base_url);

  url_table<nyaasi_parser> new_table(key,
                                     std::chrono::milliseconds(5000),
                                     new_parser);  

  // status = new_table.populate_url_list();

//  std::string key = "nyaa.si";
//  auto pos = scheduler.get_table_position(key);
//
//  std::shared_ptr<url_table> table;
//
//  if (scheduler.table_exists(pos)) {
//    table = pos->second;
//  }
//  else {
//    auto newtable = url_table(nyaasi_tree(),std::chrono::milliseconds(5000));
//  }
//
//  std::string key = "nyaa.si";
//  std::shared_ptr<url_table> table;
//  auto pos = scheduler.get_table_position(key);
//
//  if (scheduler.table_exists(pos)) {
//    table = pos->second;
//  }
//  else {
//    table = scheduler.insert_table(key,std::chrono::milliseconds(5000))->second;
//  }
//
//  std::replace(args.begin(), args.end(), ' ', '+');
//  const std::string base_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;
//
//  table->insert_url(base_url);
//  table->parse_one_url();
//  int status;

//  curl_container curler = curl_container();
//
//  status = curler.perform_curl(base_url);
//  if (status != ERRCODE::SUCCESS) return status;
//
//  tree_container html_tree_creator = tree_container();
//
//  status = html_tree_creator.parse_HTML(table->get_last_download());
//  if (status != ERRCODE::SUCCESS) return status;
//
//  status = html_tree_creator.nyaasi_extract_pageinfo();
//  if (status != ERRCODE::SUCCESS) return status;
//
//  int total = html_tree_creator.nyaasi_pages_total();
//  int per_page = html_tree_creator.nyaasi_per_page();
//  if ( total <= 1 || per_page <= 1) {
//    errprintf(ERRCODE::FAILED_NO_RESULTS,"no results found for %s!\n",args.c_str());
//    return ERRCODE::FAILED_NO_RESULTS;
//  }
//  // rounds up integer division (overflow not expected, max results = 1000)
//  int num_pages = ( total + (per_page - 1) ) / per_page;
//
//  for (int i = 2; i <= num_pages; i++) {
//    table->insert_url( base_url + "&p=" + std::to_string(i) );  
//  }
//  table->update_time();
//
//  // parse the first page we already downloaded for torrents
//  std::vector<std::string> magnets = html_tree_creator.nyaasi_parse_torrents();
//  if (options::debug_level > 0) {
//    for (auto itor: magnets) {
//      fprintf(stderr,"%s\n\n", itor.c_str());
//    }
//  }
//
//  // for (auto itor: magnets) {
//  //   torrenter.extract_magnet_information(itor);
//  // }
  return ERRCODE::SUCCESS;
}
