#include <time.h>
#include <thread>
#include <iostream>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table::url_table(std::string website, 
                     std::chrono::milliseconds delay)
  : website(website),
    delay(delay),
    last_request(std::chrono::steady_clock::now())
{}

void url_table::insert_url(std::string new_url) 
{
  this->website_urls.emplace_back(new_url);
}

void url_table::update_time()
{
  this->last_request = std::chrono::steady_clock::now();
}

bool url_table::ready_for_request() 
{
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>
    (std::chrono::steady_clock::now() - this->last_request);

  bool ready = diff.count() >= this->delay.count() ? true : false;
  return ready;
}

std::string url_table::get_website()
{
  return this->website;
}

std::vector<std::string> url_table::get_url_list()
{
  return this->website_urls;
}
