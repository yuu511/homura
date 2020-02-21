#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

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
  return website_urls.empty() ? true : false;
}

std::string url_table_base::pop_one_url()
{
  std::string back = website_urls.back();
  website_urls.pop_back();
  return back;
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

torrent_map_entry url_table_base::parse_page(const char *HTML)
{
  torrent_map_entry pl;
  fprintf(stderr,"you should never see this.");
  return pl;
}

void url_table_base::push_search_tag(std::string tag, size_t num_urls)
{
  if (options::debug_level > 1) {
    fprintf (stderr, "pushing tag %s, size %zd\n",tag.c_str(),num_urls);
  }
  searchtags.push_back(std::make_pair(tag,num_urls));
}

void url_table_base::copy_url_table(std::vector<std::string> &urls)
{
  if (options::number_pages){
    if (static_cast<size_t>(options::number_pages) < urls.size()) {
      size_t diff = urls.size() - static_cast<size_t>(options::number_pages);
      urls.erase(urls.begin(),urls.begin() + diff);
    }
  }
  website_urls.insert(website_urls.end(),urls.begin(),urls.end());
}

void url_table_base::copy_nm_pair(const std::string &URL, const torrent_map_entry &MAGNETS_IN_URL)
{
  torrentmap.push_back(std::make_pair(URL,MAGNETS_IN_URL));
}

std::string url_table_base::cache_name_protocol(std::string searchtag) 
{
  return ".homuracache_" + get_website() + "_" + searchtag; 
}

// Merge caches by default (new cache has precedence, but
// if old cache exists for a page that isn't in the new cache put it in)
void url_table_base::cache()
{
  size_t index = torrentmap.size() - 1;
  for (auto const &itor : searchtags) {
    torrent_cache cached; 
    auto num_urls = itor.second;
    auto filename = cache_name_protocol(itor.first);
    while (num_urls--) {
      auto entry = torrentmap[index];
      cached.emplace(entry.first,entry.second);
      --index;
      if (options::debug_level)
        fprintf(stderr,"caching %s\n",entry.first.c_str());
    }
    if (!options::force_refresh_cache && std::filesystem::exists(filename)) {
      torrent_cache oldcache;
      std::ifstream ifs(filename);
      boost::archive::text_iarchive ia(ifs);
      ia >> oldcache;
      for (auto const &itor : oldcache) {
        auto index = cached.find(itor.first);
        if (index == cached.end()) {
          cached.emplace(itor.first,itor.second);
          if (options::debug_level)
            fprintf(stderr,"caching %s from old cache \n",itor.first.c_str());
        }
      }
    }
    std::ofstream ofs(filename);
    boost::archive::text_oarchive oa(ofs);
    oa << cached;
  }
}

void url_table_base::load_cache(std::string searchtag,std::string first_page) 
{
  auto filename = cache_name_protocol(searchtag);
  if (!std::filesystem::exists(filename)) return;
  torrent_cache cached;
  std::ifstream ifs(filename);
  boost::archive::text_iarchive ia(ifs);
  ia >> cached;
  // never use cache for the first page
  auto fitor = cached.find(first_page);
  if (fitor !=cached.end()) {
    if (options::debug_level)
      fprintf(stderr,"cache: ignoring page %s\n",first_page.c_str());
    cached.erase(fitor);
  }
  website_urls.erase(std::remove_if(website_urls.begin(), website_urls.end(), 
  [this, cached](std::string p)  
  {
    auto index = cached.find(p);
    if (index != cached.end()) {
      printTmapEntry(index->second);
      torrentmap.push_back(std::make_pair(p,index->second)); 
      if (options::debug_level) {
        fprintf(stderr,"loading page %s from the cache!\n",p.c_str());
      }
      return true;
    }
    else {
      return false;
    }
  }),website_urls.end());
}

void url_table_base::print()
{
  for (auto const &itor : torrentmap) {
    printTmapEntry(itor.second);
  }
}
