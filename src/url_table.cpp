#include <time.h>
#include <thread>
#include <iostream>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table_base::url_table_base(std::string website_,
                               std::chrono::milliseconds delay_) 
  : curler(std::make_shared<curl_container>()), 
    website(website_),
    delay(delay_),
    last_request(std::chrono::steady_clock::now())
{}

url_table_base::~url_table_base(){}

void url_table_base::insert_url(std::string new_url) 
{
  website_urls.emplace_back(new_url);
}

void url_table_base::update_time()
{
  last_request = std::chrono::steady_clock::now();
}

bool url_table_base::ready_for_request() 
{
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>
    (std::chrono::steady_clock::now() - last_request);

  bool ready = diff.count() >= delay.count() ? true : false;
  return ready;
}

std::chrono::milliseconds url_table_base::get_delay()
{
  return delay;
}

std::vector<std::string> url_table_base::get_url_list()
{
  return website_urls;
}

std::string url_table_base::get_website() 
{
  return website;
}

std::shared_ptr<curl_container> url_table_base::get_curler()
{
  return curler;
}

void url_table_base::download_one_url() 
{
  curler->perform_curl(website_urls.back());
  website_urls.pop_back();
  update_time();
}

const char *url_table_base::get_last_download()
{
  return curler->get_HTML_aschar();
}

bool url_table_base::empty() 
{
  return website_urls.empty() ? true : false;
}

HOMURA_ERRCODE url_table_base::populate_url_list(){return 1;}
HOMURA_ERRCODE url_table_base::extract_magnets(){return 1;}

// std::vector<std::string> url_table::getURLS() 
// {
//   url_populator.get_urls();
// }
// 
// std::vector<std::string> getTorrent()
// {
//   new_magnets = extractor.extract_magnets();  
//   magnets.insert (magnets.end(),new_magnets.begin(),new_magnets.end());
// }
