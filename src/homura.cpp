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
  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE homura_instance::query_nyaasi(std::string args) 
{
  int status;
  std::string key= "nyaa.si";
  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string base_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;
  auto new_parser = std::make_shared<nyaasi_parser>(base_url);

  url_table<nyaasi_parser> new_table(key,
                                     std::chrono::milliseconds(5000),
                                     new_parser);  

  new_table.get_urls();
  std::vector<std::string> first_magnets = new_parser->extract_tree_magnets();
 //  for (auto itor : first_magnets) {
 //    fprintf (stderr,"magnet %s\n",itor.c_str());
 //  }
 //  // We have to download the first page to get urls; get magnets from this page
  // instead of putting it in the queue to be downloaded again.
  new_table.insert_magnets(first_magnets);
  //new_table.insert_magnets(new_parser->extract_tree_magnets());
  return ERRCODE::SUCCESS;
}
