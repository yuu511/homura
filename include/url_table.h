#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include "curl_container.h"

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
    std::string get_website();
    std::vector<std::string> get_url_list();
  private:
    std::string website;
    std::chrono::milliseconds delay;
    std::vector<std::string> website_urls;
    std::chrono::steady_clock::time_point last_request;
  };
}

#endif
