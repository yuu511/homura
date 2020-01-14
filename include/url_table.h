#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <stdio.h>

#include "curl_container.h"
#include "tree_container.h"


namespace homura 
{
  using name_magnet = std::vector<std::pair<std::string,std::string>>;
  class url_table_base {
  public:
    url_table_base(std::string website_,
                   std::chrono::milliseconds delay_);
    virtual ~url_table_base();

    void update_time();
    bool ready_for_request();

    std::chrono::milliseconds get_delay();
    std::string get_website();

    bool empty();
    std::string pop_one_url();

    virtual void populate_url_list(int cached_pages, std::string ref_page);

    virtual const char *download_next_URL();
    virtual name_magnet parse_page(const char *HTML);
    virtual void copy_results(const name_magnet &new_magnets); 

    virtual void parse_first_page();
    virtual void check_cache();

    void sort_urltable();
    void print();

    void copy_url(std::vector<std::string> &urls);
    void copy_nm_pair(const name_magnet &nm);

  private:
    std::string website;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<std::string> website_urls;
    name_magnet magnet_name_pair;
  };

  template <typename parser>
  class url_table : public url_table_base {
  public:
    url_table(std::string website_,
              std::chrono::milliseconds delay_,
              std::shared_ptr<parser> extractor_)
      : url_table_base(website_,delay_),         
        extractor(extractor_){}
    // template functions
    void populate_url_list(int cached_pages,std::string ref_page) 
    {
      auto urls = extractor->getURLs(cached_pages,ref_page);
      copy_url(urls);
      update_time();
    }

    void parse_first_page()
    {
      auto nm = extractor->parse_first_page();
      copy_nm_pair(nm);
      update_time();
    }

    const char *download_next_URL()
    {
      auto result = extractor->downloadOne(pop_one_url());
      update_time();
      return result;
    }

    name_magnet parse_page(const char *HTML)
    {
      return extractor->parse_HTML(HTML);
    }

    void copy_results(const name_magnet &new_magnets)
    {
       copy_nm_pair(new_magnets);
    }

    void check_cache()
    {
      auto nm = extractor->get_cached(); 
      copy_nm_pair(nm);
    }
  private:
    std::shared_ptr<parser> extractor;
  };
}

#endif
