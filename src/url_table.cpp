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
     last_written(last_written),
     it(urls.begin()) {}

void url_table::insert(std::string url) {
  urls.push_back(url);   
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

void url_table::set_begin() {
  it = urls.begin();
}

void url_table::increment_iterator() {
  if (!end()) {
    ++it;
  }
}

bool url_table::end() {
  if (it == urls.end()){
    return true;
  }
  return false;
}

bool url_table::ready_for_request() {
  auto diff = 
    std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::steady_clock::now() - last_written);

  if ( diff.count() > delay.count() ) 
    return true;
  return false;
}

std::chrono::steady_clock::time_point url_table::get_time() {
  return last_written;
}

std::chrono::milliseconds url_table::get_delay() {
  return delay;
}

int url_table::get_website() {
  return website;
}

std::vector <std::string> url_table::get_urls(){
  return urls;
}

std::string url_table::get_itor_element() {
  if (!end())
    return *it;
  errprintf ( ERRCODE::FAILED_BOUNDS, "get_itor element() called out of bounds.\n");
  return "";
}
