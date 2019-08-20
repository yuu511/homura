#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <mutex>

namespace homura {
  class url_table {
  public:
    url_table(std::chrono::seconds delay, 
      std::chrono::steady_clock::time_point last_written);  
    ~url_table();
    void insert(std::string url);
    void update_time();
  private:
    std::chrono::seconds delay;
    std::vector <std::string> urls;
    std::chrono::steady_clock::time_point last_written;
    std::mutex time_lock;
  };
}

#endif
