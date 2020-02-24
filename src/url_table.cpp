#include <stdio.h>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table_base::url_table_base(std::string website_,
                               std::chrono::milliseconds delay_) 
  : website(website_),
    delay(delay_),
    last_request(std::chrono::steady_clock::now()),
    website_urls(new std::vector<std::string>)
{}

url_table_base::~url_table_base()
{
  delete website_urls;
}

void url_table_base::update_time()
{
  last_request = std::chrono::steady_clock::now();
}

bool url_table_base::ready_for_request() 
{
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>
    (std::chrono::steady_clock::now() - last_request);

  return diff.count() >= delay.count() ? true : false;
}

std::string url_table_base::get_website() 
{
  return website;
}

std::chrono::milliseconds url_table_base::get_delay()
{
  return delay;
}

bool url_table_base::empty() 
{
  return website_urls->empty() ? true : false;
}

std::string url_table_base::pop_one_url()
{
  std::string back = website_urls->back();
  website_urls->pop_back();
  return back;
}

std::vector<std::string> *url_table_base::get_url_table()
{
  return website_urls;
}

HOMURA_ERRCODE url_table_base::populate_url_list(std::string searchtag)
{
  fprintf(stderr,"you should never see this.");
  return ERRCODE::SUCCESS;
}

std::pair<std::string,const char *> url_table_base::download_next_URL()
{
  fprintf(stderr,"you should never see this.");
  return {"",0};
}

HOMURA_ERRCODE url_table_base::parse_next_page(urlpair &pair)
{
  fprintf(stderr,"you should never see this.");
  return ERRCODE::SUCCESS;
}

void url_table_base::print()
{
  fprintf(stderr,"you should never see this.");
}

void url_table_base::cache()
{
  fprintf(stderr,"you should never see this.");
}
