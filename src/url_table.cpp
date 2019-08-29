#include <time.h>
#include <thread>
#include <iostream>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table::url_table (
 int website, 
 std::chrono::milliseconds delay, 
 std::chrono::steady_clock::time_point last_written )
   : website(website), 
     delay(delay), 
     url_list(std::make_shared<urls>()),
     curler(std::make_shared<curl_container>()),
     last_written(last_written) {}

void url_table::insert(std::string url) {
  url_list->push_back(url);   
  if (homura::options::debug_level) {
    std::cout << "Inserted key " << url << std::endl;
  }
}

void url_table::update_time() {
  auto orig = last_written;
  last_written = std::chrono::steady_clock::now();
  if (homura::options::debug_level > 2) {
    std::cout << "it has been " 
    << std::chrono::duration_cast<std::chrono::microseconds>(last_written - orig).count() 
    << " microseconds since the url table of " 
    << website 
    << " has been updated" << std::endl;
  }
}

std::chrono::milliseconds url_table::get_delay() {
  return delay;
}

bool url_table::ready_for_request() {
  auto diff = 
    std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::steady_clock::now() - last_written);

  if ( diff.count() >= delay.count() ) 
    return true;
  return false;
}

std::shared_ptr<urls> url_table::get_urls() {
  return url_list;
}

std::shared_ptr<curl_container> url_table::get_curler() {
  return curler;
}
