
#include <time.h>
#include <thread>
#include <iostream>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table::url_table (
 int website, 
 std::chrono::seconds delay, 
 std::chrono::steady_clock::time_point last_written )
   : website(website), 
     delay(delay), 
     last_written(last_written) {}

void url_table::insert(std::string url) {
  urls.push_back(url);   
  if (homura::options::debug_level) {
    std::cout << "Inserted key " << url << std::endl;
  }
}

void url_table::update_time() {
  auto orig = last_written;
  last_written = std::chrono::steady_clock::now();
  if (homura::options::debug_level > 1) {
    std::cout << "it has been " 
    << std::chrono::duration_cast<std::chrono::microseconds>(last_written - orig).count() 
    << " microseconds since the url table has been updated" << std::endl;
  }
}

std::chrono::steady_clock::time_point url_table::get_time() {
  return last_written;
}

std::chrono::seconds url_table::get_delay() {
  return delay;
}

int url_table::get_website() {
  return website;
}

std::vector <std::string> url_table::get_urls(){
  return urls;
}
