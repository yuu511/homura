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
  class url_table {
  public:
    url_table(std::string website,
              std::chrono::milliseconds delay);

    void insert_url(std::string url);
    void update_time();
    bool ready_for_request();

    std::chrono::milliseconds get_delay();
    std::vector<std::string> get_url_list();
    std::string get_website();

    void download_one_url();
    const char *get_last_download();
    bool empty();

   // std::vector<std::string> populate_table();
   // std::vector<std::string> getTorrent();
  private:
    curl_container curler;
   //  m magnet_extractor;
   //  p url_populator;
    std::string website;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<std::string> website_urls;
    std::vector<std::string> magnets;
  };
}

#endif
