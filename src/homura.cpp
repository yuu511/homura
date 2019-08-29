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
  : results(nullptr) {
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();
}

void homura_instance::cleanup() {
  curl_global_cleanup();
}

homura_instance::~homura_instance() {
  cleanup(); 
}

// if the table exists for a kind of url, return pointer to it
// otherwise allcoate a new one and insert it into request hash and request vector
std::shared_ptr<homura::url_table> homura_instance::get_table
(int website, std::chrono::milliseconds delay) {
  for (auto itor : requests_hash) {
    if ( itor.first == website ) {
      return itor.second; 
    }
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

void homura_instance::crawl() {
  if (homura::options::debug_level) {
    fprintf(stdout,"== Crawl called ==\n");
    fprintf(stdout,"Number of url types %zd\n",requests.size());
    for (auto itor : requests) {
      size_t sz = itor->get_urls().size();
      fprintf(stdout,"URL %d SZ %zd \n\n",itor->get_website(),sz);
    }
  }

  for (auto table : requests) {
    table->set_begin();
  }

  bool finished = false;
  while (!finished) { 
    finished = true;
    for (auto table : requests) {
      // end condition
      if (!table->end()) {
        finished = false;
        if (table->ready_for_request()) {

          if (homura::options::debug_level) {
            fprintf(stdout, "Parsing url %s\n" , table->get_itor_element().c_str());
          }

          table->increment_iterator();
          table->update_time();
        }
      } else {
        continue;
      }
    }
  }

}

bool homura_instance::query_nyaasi(std::string args) {

   std::shared_ptr<url_table> table = get_table(website::nyaasi,std::chrono::milliseconds(5000));

  /* nyaa.si has no official api, and we must manually
     find out how many results to expect by sending a request 
     and parsing the query result information */

   homura::options::set_debug_level(3);

  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string base_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;

  table->get_curler()->perform_curl(base_url);

  std::shared_ptr<std::vector<unsigned char>> results = table->get_curler()->get_HTML();

  // should be able to start another thread and just rip it
 std::shared_ptr<tree_container> tree = std::make_shared<tree_container>(); 
 const char *HTML = reinterpret_cast<const char*>(results->data());
// testan
 std::thread t1(&tree_container::tree_parseHTML,tree, HTML);
 table->get_curler()->perform_curl("http://example.com");
 t1.join();

 // check for the fkd up tree
 if (!(tree->parse_pagination_information())) {
   errprintf(ERRCODE::FAILED_PARSE, "Failed to retrieve number of results.\n");
   return false;
 }

 table->update_time();

 int total = tree->pageinfo_total();
 int per_page = tree->pageinfo_results_per_page();
 // rounds up integer division (overflow not expected, max results = 1000)
 int num_pages = ( total + (per_page - 1) ) / per_page;

 /* push urls to table */
 for (int i = 1; i < num_pages; i++) {
   table->insert( base_url + "&p=" + std::to_string(i) );  
 }
 return true;
}
