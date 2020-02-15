#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <stdio.h>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "curl_container.h"
#include "tree_container.h"


namespace homura 
{
  using torrent_map_entry = std::vector<std::pair<std::string,std::string>>;
  using torrent_map = std::vector<std::pair<std::string,torrent_map_entry>>;
  class url_table_base {
  public:
    url_table_base(std::string website_,
                   std::chrono::milliseconds delay_);
    url_table_base();
    virtual ~url_table_base();

    void set_search_tag(std::string tag);

    void update_time();
    bool ready_for_request();

    void copy_url(const std::vector<std::string> &urls);
    void copy_nm_pair(const std::string &URL, const torrent_map_entry &MAGNETS_IN_URL);

    std::string cache_name_protocol();
    void cache();
    void load_cache();

    bool empty();
    std::string pop_one_url();

    std::string get_website();
    std::chrono::milliseconds get_delay();

    virtual void populate_url_list(int cached_pages, std::string first_page);
    virtual const char *download_next_URL();
    virtual torrent_map_entry parse_page(const char *HTML);

    void print();
  private:
    std::string website;
    std::string search_tag;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<std::string> website_urls;
    torrent_map torrentmap;
  };

  template <typename parser>
  class url_table : public url_table_base {
  public:
    url_table(std::string website_,
              std::chrono::milliseconds delay_,
              parser extractor_)
      : url_table_base(website_,delay_),         
        extractor(std::move(extractor_)){}
    // template functions
    void populate_url_list(int cached_pages,std::string first_page) 
    {
      auto urls = extractor.getURLs(cached_pages,first_page);
      copy_url(urls);
      update_time();
      auto nm = extractor.parse_first_page();
      copy_nm_pair(first_page,nm);
    }

    const char *download_next_URL()
    {
      auto result = extractor.downloadOne(pop_one_url());
      update_time();
      return result;
    }

    torrent_map_entry parse_page(const char *HTML)
    {
      return extractor.parse_HTML(HTML);
    }
  private:
    parser extractor;
  };
}

#endif
