#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <vector>
#include <deque>
#include <string>
#include <utility>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <algorithm>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>


#include "errlib.h"


namespace homura 
{
  struct generic_torrent_result {
    std::string name;
    std::string magnet;
    std::string size;
    std::string date;
    std::string webpage;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & name;
      ar & magnet;
      ar & size;
      ar & date;
      ar & webpage;
    }
  };

  using urlpair = std::pair <std::string, std::deque<std::string>>; // search query, URLs generated

  struct url_table_base {
    url_table_base(std::string _website,
                   std::chrono::milliseconds _delay);
    url_table_base(std::string _website,
                   std::chrono::milliseconds _delay,
                   int _num_retries);
    virtual ~url_table_base();
                     
    // builder funcs                      
    void addURLs(std::string query, std::deque<std::string> URLs);
    void findAndProcessCache(std::string query, size_t expected_results, size_t results_per_page);
    void addNewResults(std::string query, std::vector<generic_torrent_result> torrents);    
    //

    bool ready_for_request();
    bool empty();
    void do_caching_operations();
    virtual HOMURA_ERRCODE download_next_URL();  

    std::chrono::milliseconds get_delay(); 
    std::string get_website();

    void print(); 

    //vars
    std::string website;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<urlpair> remainingURLs;   
    std::unordered_map <std::string, std::vector<generic_torrent_result>> results;
    std::unordered_map <std::string, std::vector<generic_torrent_result>> cached_results;
    int num_retries;
    bool cache_done;

    //serialization
    friend class boost::serialization::access;
    void decache();
    HOMURA_ERRCODE cache();
    std::filesystem::path get_cache_dir();
    std::filesystem::path generate_cache_fullpath(std::filesystem::path basedir,std::string query);
  };

  template <typename extractor>
  struct url_table : public url_table_base {
    url_table(std::string _website,
              std::chrono::milliseconds _delay,
              extractor _parser)
    : url_table_base(_website,_delay),
      parser(_parser){}

    HOMURA_ERRCODE download_next_URL()
    {
      HOMURA_ERRCODE Status = ERRCODE::SUCCESS;
      auto lastElement = remainingURLs.rbegin();

      last_request = std::chrono::steady_clock::now();

      auto found = results.find(lastElement->first);
      if (found != results.end()) {
       Status = parser.downloadPage(lastElement->second.front() , found->second);
      }
      else {
        found = results.emplace(std::make_pair(lastElement->first,std::vector<generic_torrent_result>())).first;
        Status = parser.downloadPage(lastElement->second.front() , found->second);
      }

      if (Status == ERRCODE::SUCCESS) {
        lastElement->second.pop_front();
        if (lastElement->second.empty()) remainingURLs.pop_back();  
      }
      else {
        if (!num_retries) {
          lastElement->second.pop_front();
          if (lastElement->second.empty()) remainingURLs.pop_back();  
          return Status;
        }
        fprintf (stderr, "Failed at %s, num_retries left = %d\n", lastElement->second.front().c_str(),
                                                                  num_retries);
        --num_retries;
      }

      return ERRCODE::SUCCESS;
    }

    extractor parser;
  };

}
#endif
