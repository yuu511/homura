#include <algorithm>
#include <chrono>
#include <thread>
#include <memory>
#include <curl/curl.h>
#include <pthread.h>

#include "homura.h"
#include "magnet_table.h"
#include "errlib.h"
#include "select_ssl.h"
#include "curl_container.h"
#include "tree_container.h"

// mix of c and c++ for torrent scraping
// use char* for html parsing, store results in string
  
/* given a string, scrape all torrent names and magnets */
homura::magnet_table *homura::search_nyaasi(std::string args)
{
  std::chrono::seconds crawl_delay(5);
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();

  /* nyaa.si has no official api, and we must manually
     find out how many pages to parse by sending a request */

  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string first_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;

  std::unique_ptr<curl_container> first_request(new curl_container(first_url));
  first_request -> perform_curl();

  std::unique_ptr<tree_container> first_page_tree(new tree_container()); 
  first_page_tree -> tree_parseHTML(first_request->get_HTML_char());

  if ( !(first_page_tree -> parse_pagination_information()) )
  {
    errprintf(ERRCODE::FAILED_PARSE, "Failed to retrieve number of results.\n");
    curl_global_cleanup();
    return nullptr;
  }
  
  // for (auto itor : *urls) 
  // {
  //   new_request = clock::steady_clock::now() + crawl_delay;
  //   // do stuff
  //   now = clock::steady_clock::now();
  //   if (debug_level) 
  //   {
  //     fprintf(stdout,"sleeping for %lu milliseconds\n",
  //             clock::duration_cast<clock::milliseconds>(new_request - now).count()); 
  //   }
  //   std::this_thread::sleep_for(clock::duration_cast<clock::milliseconds>(new_request - now));
  // }

  // delete (first_request);
  // first_request.release();
  // delete (first_page_tree);
  curl_global_cleanup();
  return nullptr;
}
