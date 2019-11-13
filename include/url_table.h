#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include "curl_container.h"
#include "tree_container.h"

namespace homura 
{
  class url_table_base {
  public:
    url_table_base(std::string website_,
                   std::chrono::milliseconds delay_);
    virtual ~url_table_base();

    void update_time();
    bool ready_for_request();

    std::chrono::milliseconds get_delay();
    std::vector<std::string> get_url_list();
    std::vector<std::string> get_magnets();
    std::string get_website();

    bool empty();
    std::string pop_one_url();

    virtual void populate_url_list(std::string page);
    virtual void extract_magnets();

    void insert_urls(std::vector<std::string> urls);
    void insert_magnets(std::vector<std::string> urls);

  private:
    std::string website;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<std::string> website_urls;
    std::vector<std::string> magnets;
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
      insert_urls(extractor->populate_url_list(page));  
      insert_magnets(extractor->parse_first_page());
      update_time();
    }
    void extract_magnets()
    {
      extractor->get_magnets(pop_one_url());  
      update_time();
    }
  private:
    std::shared_ptr<parser> extractor;
  };
}

#endif
