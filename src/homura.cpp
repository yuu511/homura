#include <algorithm>
#include <chrono>
#include <thread>
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

namespace homura 
{
  int debug_level = 0;
  
  magnet_table *alloc_table_nyaasi(int total) 
  {
    magnet_table *names = nullptr;
    if (!total)
    {
      fprintf(stdout,"No results found for query\n");
      return nullptr;
    }
    try 
    {
      names = new magnet_table(total);
      for (size_t i = 0; i < names->size(); ++i)
      {
        (*names)[i] = new name_magnet();
      }
    }
    catch (std::bad_alloc &ba)
    {
      errprintf(ERRCODE::FAILED_NEW, "Failed new allocation %s\n",ba);
      return nullptr;
    }
    return names;
  }
  
  std::vector<std::string*> *alloc_urls_nyaasi (int total, int results_per_page, std::string base_url) 
  {
    if (!total)
      return nullptr;
  
    if (debug_level)
      fprintf (stderr, "== URLS: ==\n");
  
    // rounds up integer division, (overflow not expected, max results = 1000)
    int num_elements = ( total + (results_per_page - 1) ) / results_per_page; 
    std::vector<std::string*> *urls;
    try 
    {
      // already tried the first page
      urls = new std::vector<std::string*>(num_elements-1);
      // first page in memory already (nyaasi)
      for (size_t i = 1; i < urls->size(); ++i)
      {
        (*urls)[i] = new std::string (base_url + "&p=" + std::to_string(i+1));   
        if (debug_level)
          fprintf (stdout, "%s\n", (*urls)[i]->c_str());
      }
    }
    catch (std::bad_alloc &ba)
    {
      errprintf(ERRCODE::FAILED_NEW, "Failed new allocation %s\n",ba);
      return nullptr;
    }
  
    if (debug_level)
      fprintf(stdout,"Pages to parse: %zd\n",urls->size()+1);
  
    return urls;
  }
  
  void free_urls (std::vector<std::string*> *urls)
  {
    if (urls)
    {
      for (auto itor : *urls)
      {
        delete itor; 
      }
      delete urls;
    }
  }
  
  /* given a string, scrape all torrent names and magnets */
  magnet_table *search_nyaasi(std::string args, int LOG_LEVEL, int threads)
  {
    namespace clock = std::chrono;
    clock::seconds crawl_delay(5);
    // auto now = clock::steady_clock::now();
    // auto new_request = now + crawl_delay;
  
    debug_level = LOG_LEVEL;
    curl_global_init(CURL_GLOBAL_ALL);
    init_locks();

    /* nyaa.si has no official api, and we must manually
       find out how many pages to parse by sending a request */

    std::replace(args.begin(),args.end(),' ','+');
    std::string first_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;

    curl_container *first_request = new curl_container(first_url);
    first_request->perform_curl();
    
    if (debug_level) 
    {
      fprintf (stdout,"sizeof data: %zd\nsizeof data_sz %zd\n",
        first_request->get_HTML()->size(),first_request->get_data_sz());
      fprintf (stdout, "== First Page URL: == \n%s\n\n",first_request->get_url().c_str());
    }

    if (debug_level > 1) 
      fprintf (stdout,"html pg: %s", first_request->get_HTML_char());

    tree_container *first_page_tree = new tree_container(); 
    first_page_tree->tree_parseHTML(first_request->get_HTML_char());
    if (! first_page_tree->parse_pagination_information() ) 
    {
      errprintf(ERRCODE::FAILED_FIRST_PARSE, "ERROR:Failed initial parsing first page.\n");
      delete (first_request);
      delete (first_page_tree);
      curl_global_cleanup();
      return nullptr;
    }

    if (debug_level) 
      fprintf (stdout,"== First page result information == \nresults per page %d\ntotal pages %d\n"
              ,first_page_tree->pageinfo_first_result()
              ,first_page_tree->pageinfo_total_result());
    

    // magnet_table *names = alloc_table_nyaasi(results[2]);
    // if (!names){ return nullptr; }
    // std::vector<std::string*> *urls = alloc_urls_nyaasi(results[2],results[1],first_request->get_url());
    // if (!urls){ return nullptr; }
    // 
  
    // /* process first url we pulled here */
    // now = clock::steady_clock::now();
    // std::this_thread::sleep_for(clock::duration_cast<clock::milliseconds>(new_request - now));
  
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

    delete (first_request);
    delete (first_page_tree);
    curl_global_cleanup();
    return nullptr;
  }
}
