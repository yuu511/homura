#include "url_table.h"

using namespace homura;

url_table::url_table(std::chrono::seconds delay, 
  std::chrono::steady_clock::time_point last_written)
 : delay(delay), last_written(last_written) {};

url_table::~url_table() {}

void url_table::insert(std::string url) {
  urls.push_back(url);   
}

void url_table::update_time() {
  std::lock_guard<std::mutex> lock = time_lock;
  last_written = std::chrono::steady_clock::now();
}
