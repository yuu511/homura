#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include "errlib.h"

namespace homura 
{
  using urlpair = std::pair<std::string,const char*>; // webpage URL, raw HTML
  
  struct generic_torrent_results {
    std::string torrent;
    std::string magnet;
    std::string size;
    std::string date;
  };

  typedef urlpair (*downloadNextPage)(std::string URL);
  typedef generic_torrent_results (*parseHTMLPage)(urlpair);

  class url_table_base {
  public:
    url_table_base(std::string website,
                   downloadNextPage download_next_URL;
                   parseHTMLPage parse_next_page);
                     
    void addURLs(std::vector<std::string> newURLs);

    bool ready_for_request();
    downloadNextPage download_next_URL;
    parseHTMLPage parse_next_page;

    std::chrono::milliseconds get_delay(); 

    void cache();
    void decache();
    void print(); 
  private:
    std::string website;
    std::vector<std::string> remainingURLs;   
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
  };
}
#endif
