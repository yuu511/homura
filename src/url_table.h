#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>

namespace homura {

  enum website {
    nyaasi, 
    nyaapantsu, 
    horriblesubs
  };

  struct url_request {
  public:
    url_request(const std::string &url, int website, std::chrono::seconds delay);
    const std::string get_url();
    int get_website();
    std::chrono::seconds get_delay();
  private:
    const std::string url; 
    int website;
    std::chrono::seconds delay;
  };
  using url_table = std::vector<url_request*>;  
  
}

#endif
