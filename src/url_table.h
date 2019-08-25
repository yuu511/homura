#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <memory>

namespace homura {
  class url_table {
  public:
    url_table(int website, std::chrono::seconds delay, 
      std::chrono::steady_clock::time_point last_written = std::chrono::steady_clock::now());  
    void insert(std::string url);
    void update_time();

    int get_website();
    std::chrono::steady_clock::time_point get_time();
    std::chrono::seconds get_delay();
    std::vector <std::string> get_urls();
  private:
    int website;
    std::chrono::seconds delay;
    std::vector <std::string> urls;
    std::chrono::steady_clock::time_point last_written;
  };
}

#endif
