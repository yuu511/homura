#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include "curl_container.h"

using urls = std::vector<std::string>;

namespace homura {
  class url_table {
  public:
    url_table(int website, std::chrono::milliseconds delay, 
      std::chrono::steady_clock::time_point last_written = std::chrono::steady_clock::now());  

    void insert(std::string url);

    void update_time();
    bool ready_for_request();
    std::chrono::milliseconds get_delay();

    std::shared_ptr <urls> get_urls();
    std::shared_ptr <curl_container> get_curler();
  private:
    int website;
    std::chrono::milliseconds delay;
    std::shared_ptr<urls> url_list;
    std::shared_ptr<curl_container> curler;
    std::chrono::steady_clock::time_point last_written;
  };
}

#endif
