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

using namespace homura;

// mix of c and c++ for torrent scraping
// use char* for html parsing, store results in string

// in order to use SSL in a multithreaded context, we must place mutex callback setups
#ifdef GNUTLS
#include <gcrypt.h>
#include <errno.h>

GCRY_THREAD_OPTION_PTHREAD_IMPL;

void init_locks(void) {
  gcry_control(GCRYCTL_SET_THREAD_CBS);
  if (homura::options::debug_level)
    fprintf(stdout, "curl with GNUTLS selected\n");
}

#else // the default, openssl
#include <openssl/crypto.h>
#include <deque>
#include <mutex>

std::deque<std::mutex> locks;

static void lock_callback(int mode, int type, char *file, int line) {
  (void)file;
  (void)line;
  if(mode & CRYPTO_LOCK) {
    locks[type].lock();
  }
  else {
    locks[type].unlock();
  }
}

static unsigned long thread_id() {
  return static_cast<unsigned long> (pthread_self());
}

static void init_locks() {
  locks.resize(CRYPTO_num_locks());
  CRYPTO_set_id_callback(&thread_id);
  CRYPTO_set_locking_callback(&lock_callback);
  if (homura::options::debug_level)
    fprintf(stdout, "curl with OpenSSL selected\n");
}
#endif

homura_instance::homura_instance() {
  curl_global_init(CURL_GLOBAL_ALL);
  init_locks();
}

void homura_instance::cleanup() {
  curl_global_cleanup();
}

homura_instance::~homura_instance() {
  cleanup(); 
}

std::unique_ptr<tree_container> parse_webpage(const std::string url) { 
  std::unique_ptr<curl_container> request(new curl_container(url));
  if ( !request->perform_curl() ) {
    errprintf (ERRCODE::FAILED_CURL, "Failed Curl for URL : %s\n", url.c_str());
    return nullptr;
  }

  std::unique_ptr<tree_container>
    page_tree (new tree_container(request->get_time_sent())); 
  if ( !page_tree->tree_parseHTML(request->get_HTML_char()) ) {
    errprintf (ERRCODE::FAILED_PARSE, "Failed Parse for URL: %s\n", url.c_str()); 
    return nullptr;
  }
    
  return page_tree;
}

bool homura_instance::query_nyaasi(std::string args) {
  /* nyaa.si has no official api, and we must manually
     find out how many results to expect by sending a request 
     and parsing the query result information */

  std::replace(args.begin(), args.end(), ' ', '+');
  const std::string first_url = "https://nyaa.si/?f=0&c=0_0&q=" + args;

  std::unique_ptr<tree_container> first_page_tree = parse_webpage(first_url); 

  if (!(first_page_tree->parse_pagination_information())) {
    errprintf(ERRCODE::FAILED_PARSE, "Failed to retrieve number of results.\n");
    return false;
  }

  // for (auto itor : *urls) 
  //  {
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

  return true;
}
