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

    virtual void populate_url_list(std::string page);
    virtual void extract_magnets();
    virtual void parse_first_page();

    void sort_urltable();
    void sort_print();

    void copy_url(std::vector<std::string> &urls);
    void copy_nm_pair(name_magnet &nm);

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
    void populate_url_list(std::string page) 
    {
      auto urls = extractor->populate_url_list(page);
      copy_url(urls);
      update_time();
    }
    void extract_magnets()
    {
      auto nm = extractor->get_magnets(pop_one_url());
      copy_nm_pair(nm);
      update_time();
    }
    void parse_first_page()
    {
      auto nm = extractor->parse_first_page();
      copy_nm_pair(nm);
      update_time();
    }
  private:
    std::shared_ptr<parser> extractor;
  };
}

#endif
