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

    void insert_url(std::string url);
    void update_time();
    bool ready_for_request();

    std::chrono::milliseconds get_delay();
    std::vector<std::string> get_url_list();
    std::string get_website();
    std::shared_ptr<curl_container> get_curler();

    void download_one_url();
    const char *get_last_download();
    bool empty();

    virtual HOMURA_ERRCODE populate_url_list();
    virtual HOMURA_ERRCODE extract_magnets();

  private:
    std::shared_ptr<curl_container> curler;
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
              parser extractor_)
      : url_table_base(website_,delay_),         
        extractor(extractor_){}
    HOMURA_ERRCODE populate_url_list() 
    {
      return extractor.get_urls(url_table_base::get_curler());  
    }
    HOMURA_ERRCODE extract_magnets()
    {
      return extractor.get_magnets(url_table_base::get_curler());  
    }
  private:
    parser extractor;
  };
}

#endif
