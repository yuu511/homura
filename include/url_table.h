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
    url_table(tree_container parser,
              std::chrono::milliseconds delay);

    void insert_url(std::string url);
    void update_time();
    bool ready_for_request();

    std::chrono::milliseconds get_delay();
    std::string get_website();
    std::vector<std::string> get_url_list();

    void parse_one_url();
    const char *get_last_download();
    bool empty();
  private:
    tree_container parser;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<std::string> website_urls;
    curl_container curler;
  };
}

#endif
