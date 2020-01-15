#include <time.h>
#include <thread>
#include <algorithm>
#include <stdio.h>

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

void url_table_base::populate_url_list(int cached_pages, std::string page)
{
  fprintf(stderr,"you should never see this.");
  return;
}

void url_table_base::check_cache()
{
  fprintf(stderr,"you should never see this.");
  return;
}

const char *url_table_base::download_next_URL()
{
  fprintf(stderr,"you should never see this.");
  return "";
}

name_magnet url_table_base::parse_page(const char *HTML)
{
  name_magnet pl;
  fprintf(stderr,"you should never see this.");
  return pl;
}

void url_table_base::copy_results(const name_magnet &new_magnets)
{
  fprintf(stderr,"you should never see this.");
  return;
}

void url_table_base::copy_url(std::vector<std::string> &urls)
{
  website_urls.insert(website_urls.end(),urls.begin(),urls.end());
}

void url_table_base::copy_nm_pair(const name_magnet &nm)
{
  magnet_name_pair.insert(magnet_name_pair.end(),nm.begin(),nm.end());
}

void url_table_base::sort_urltable()
{
  std::sort(magnet_name_pair.begin(),magnet_name_pair.end(), 
              [](const std::pair<std::string,std::string> &x,  
                 const std::pair<std::string,std::string> &y)
  {
     return x.second < y.second;
  });
}

void url_table_base::print()
{
  if (options::debug_level) {
    fprintf (stdout," == url_table print() %s size %zd == \n\n",
                     website.c_str(),magnet_name_pair.size());
  }
  for (auto const &itor : magnet_name_pair) {
    fprintf (stdout, "%s\n",itor.second.c_str());
  }
}
