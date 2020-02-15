#include <time.h>
#include <thread>
#include <algorithm>
#include <stdio.h>
#include <fstream>

#include "url_table.h"
#include "errlib.h"

using namespace homura;

url_table_base::url_table_base(std::string website_,
                               std::chrono::milliseconds delay_) 
  : website(website_),
    delay(delay_),
    last_request(std::chrono::steady_clock::now())
{}

url_table_base::url_table_base()
:  last_request(std::chrono::steady_clock::now()){}

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

std::string url_table_base::get_website() 
{
  return website;
}

std::chrono::milliseconds url_table_base::get_delay()
{
  return delay;
}

void url_table_base::set_search_tag(std::string tag) 
{
  search_tag = tag;
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

const char *url_table_base::download_next_URL()
{
  fprintf(stderr,"you should never see this.");
  return "";
}

torrent_map_entry url_table_base::parse_page(const char *HTML)
{
  torrent_map_entry pl;
  fprintf(stderr,"you should never see this.");
  return pl;
}

void url_table_base::copy_url(const std::vector<std::string> &urls)
{
  website_urls.insert(website_urls.end(),urls.begin(),urls.end());
}

void url_table_base::copy_nm_pair(const std::string &URL, const torrent_map_entry &MAGNETS_IN_URL)
{
  torrentmap.push_back(std::make_pair(URL,MAGNETS_IN_URL));
}

void url_table_base::print()
{
  if (options::debug_level) {
    fprintf (stdout," == url_table print() %s size %zd == \n\n",
                     website.c_str(),torrentmap.size());
    for (auto const &itor : torrentmap) {
      fprintf(stdout, "URL: %s\n NAME OF TORRENT / TORRENT LINK: \n\n", itor.first.c_str());
      for (auto const &itor2 : itor.second) {
        fprintf(stdout, "%s : %s\n",itor2.first.c_str(),itor2.second.c_str());
      }
    }
  }
}

std::string url_table_base::cache_name_protocol()
{
  return search_tag; 
}

void url_table_base::cache() 
{
}

void url_table_base::load_cache()
{
}
