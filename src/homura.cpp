#include <algorithm>
#include <chrono>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>

#include "homura.h"
#include "magnet_table.h"
#include "errlib.h"
#include "curl_container.h"
#include "tree_container.h"
#include "select_ssl.h"

//testing only, rm later
#include <thread>

using namespace homura;

// mix of c and c++ for torrent scraping
// use char* for html parsing, store results in string

homura_instance::homura_instance() 
  : results(nullptr),
    torrenter() {
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();
}

homura_instance::~homura_instance() {
  curl_global_cleanup();
}

// if the table exists for a kind of url, return pointer to it
// otherwise allcoate a new one and insert it into request hash and request vector
std::shared_ptr<homura::url_table> homura_instance::get_table
(int website, std::chrono::milliseconds delay) {
  
  // lookup
  auto it = requests_hash.find(website); 
  if (it != requests_hash.end() ) {
    return it->second;
  }
  
  // not found in hash
  auto new_table = std::make_shared<url_table>(website,delay);
  requests_hash.emplace(website, new_table);
  // tables sorted by delay, descending
  auto itor = requests.begin();
  while (itor != requests.end()) {
    if (delay.count() > (*itor)->get_delay().count()) {
      break;
    }
    ++itor;
  }
  requests.insert(itor, new_table);
  return new_table;
}

HOMURA_ERRCODE homura_instance::crawl() {
  bool finished = false;
  while (!finished) {
    finished = true;
    for (auto table : requests) {
      if (!table->get_urls()->empty()) {
        finished = false;
        if (table->ready_for_request()) {
          table->update_time();
          if (options::debug_level) {
            fprintf(stdout, "Parsing url %s\n" , table->get_urls()->back().first.c_str());
          }
	  // auto p = table->get_urls()->back();
	  // table->get_urls().second->perform_curl(table->get_urls()->
          // p.second->perform_curl(p.first);
	  table->get_urls()->back().second->perform_curl(table->get_urls()->back().first);
          table->get_urls()->pop_back();
        }
      } else {
        continue;
      }
    }
  }
  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE homura_instance::query_nyaasi(std::string args) {

   std::shared_ptr<url_table> table = get_table(website::nyaasi,std::chrono::milliseconds(5000));

  /* nyaa.si has no official api, and we must manually
     find out how many results to expect by sending a request 
     and parsing the query result information */

  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string base_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;

  curl_container first = curl_container();
  first.perform_curl(base_url);

  tree_container first_tree = tree_container();
  first_tree.parse_HTML(first.get_HTML_aschar());
  int failed_parse = first_tree.nyaasi_parse_pageinfo();
  if (failed_parse) return failed_parse;

  int total = first_tree.nyaasi_pageinfo_total();
  int per_page = first_tree.nyaasi_pageinfo_results_per_page();
  if ( total <= 1 || per_page <= 1) {
    fprintf(stdout, "no results found for %s!\n",args.c_str());
    return ERRCODE::FAILED_NO_RESULTS;
  }
  // rounds up integer division (overflow not expected, max results = 1000)
  int num_pages = ( total + (per_page - 1) ) / per_page;

  for (int i = 2; i <= num_pages; i++) {
    table->insert( base_url + "&p=" + std::to_string(i) );  
  }

  table->update_time();

  // parse the first page we already downloaded for torrents

  std::vector<std::string> magnets = first_tree.nyaasi_parse_torrents();
  // for (auto itor: magnets) {
  //   torrenter.extract_magnet_information(itor);
  // }
  return ERRCODE::SUCCESS;
}
