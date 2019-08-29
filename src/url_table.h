#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include "curl_container.h"

namespace homura {
  class url_table {
  public:
    url_table(int website, std::chrono::milliseconds delay, 
      std::chrono::steady_clock::time_point last_written = std::chrono::steady_clock::now());  

    void insert(std::string url);
    void update_time();
    void set_begin();
    void increment_iterator();
    bool end();
    bool ready_for_request();

    int get_website();
    std::chrono::steady_clock::time_point get_time();
    std::chrono::milliseconds get_delay();
    std::vector <std::string> get_urls();
    std::string get_itor_element();
    std::unique_ptr <curl_container> &get_curler();

  private:
    int website;
    std::chrono::milliseconds delay;
    std::vector <std::string> urls;
    std::unique_ptr<curl_container> curler;
    std::chrono::steady_clock::time_point last_written;
    std::vector<std::string>::iterator it;
  };
}

#endif
