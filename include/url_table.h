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
    std::string get_website();

    bool empty();
    std::string pop_one_url();

    virtual void get_urls();
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
    void get_urls() 
    {
      update_time();
      insert_urls(extractor->get_urls());  
    }
    void extract_magnets()
    {
      update_time();
      insert_magnets(extractor->get_magnets(pop_one_url()));  
    }
   std::shared_ptr<parser> return_extractor()
    {
      return extractor;
    }
  private:
    std::shared_ptr<parser> extractor;
  };
}

#endif
