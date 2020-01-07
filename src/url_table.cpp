#include <time.h>
#include <thread>
#include <iostream>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table_base::url_table_base(std::string website_,
                               std::chrono::milliseconds delay_) 
  : website(website_),
    delay(delay_),
    last_request(std::chrono::steady_clock::now())
{}

url_table_base::~url_table_base(){}

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

std::string url_table_base::get_website() 
{
  return website;
}

bool url_table_base::empty() 
{
  return website_urls.empty() ? true : false;
}

std::string url_table_base::pop_one_url()
{
  std::string back = website_urls.back();
  website_urls.pop_back();
  return back;
}

void url_table_base::populate_url_list(std::string page)
{
  fprintf(stderr,"you should never see this.");
  return;
}
void url_table_base::extract_magnets()
{
  fprintf(stderr,"you should never see this.");
  return;
}

void url_table_base::parse_first_page()
{
  fprintf(stderr,"you should never see this.");
  return;
}

void url_table_base::copy_url(std::vector<std::string> &urls)
{
  website_urls = urls;
}

void url_table_base::copy_nm_pair(name_magnet &nm)
{
  magnet_name_pair = nm;
}

