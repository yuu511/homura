#include <time.h>
#include <thread>
#include <iostream>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table::url_table(tree_container parser_, 
                     std::chrono::milliseconds delay_)
  : parser(parser_),
    delay(delay_),
    last_request(std::chrono::steady_clock::now()),
    curler(curl_container())
{}

void url_table::insert_url(std::string new_url) 
{
  website_urls.emplace_back(new_url);
}

void url_table::update_time()
{
  last_request = std::chrono::steady_clock::now();
}

bool url_table::ready_for_request() 
{
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>
    (std::chrono::steady_clock::now() - last_request);

  bool ready = diff.count() >= delay.count() ? true : false;
  return ready;
}

std::chrono::milliseconds url_table::get_delay()
{
  return delay;
}

std::string url_table::get_website()
{
  return parser.get_website();
}

std::vector<std::string> url_table::get_url_list()
{
  return website_urls;
}

void url_table::parse_one_url() 
{
  curler.perform_curl(website_urls.back());
  website_urls.pop_back();
  update_time();
}

const char *url_table::get_last_download()
{
  return curler.get_HTML_aschar();
}

bool url_table::empty() 
{
  return website_urls.empty() ? true : false;
}
