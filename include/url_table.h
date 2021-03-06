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
#include <cstdint>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include "errlib.h"

#define GENERIC_TORRENT_RESULT_VERSION 2

namespace homura { struct generic_torrent_result; }
BOOST_CLASS_VERSION(homura::generic_torrent_result,GENERIC_TORRENT_RESULT_VERSION);

namespace homura 
{
  struct generic_torrent_result {
    std::string name;
    std::string magnet;
    std::string sizestring;
    std::uint64_t sizebytes;
    std::string date;
    std::string webpage;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & name;
      ar & magnet;
      ar & sizestring;
      if (version > 1) {
        ar & sizebytes; 
      }
      ar & date;
      ar & webpage;
    }
  };

  using urlpair = std::pair <std::string, std::deque<std::string>>; // search query, URLs generated

  struct url_table_base {
    url_table_base(homura_settings *_settings,
                   std::string _website,
                   std::chrono::milliseconds _delay);
    url_table_base(homura_settings *_settings, 
                   std::string _website,
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
    homura_settings *settings;
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
    url_table(homura_settings *_settings,
              std::string _website,
              std::chrono::milliseconds _delay,
              extractor _parser)
    : url_table_base(_settings,_website,_delay),
      parser(_parser){}

    url_table(homura_settings *_settings,
              std::string _website,
              std::chrono::milliseconds _delay,
              int _numRetries,
              extractor _parser)
    : url_table_base(_settings,_website,_delay,_numRetries),
      parser(_parser){}

    HOMURA_ERRCODE download_next_URL()
    {
      HOMURA_ERRCODE Status = ERRCODE::SUCCESS;
      auto lastElement = remainingURLs.rbegin();

      last_request = std::chrono::steady_clock::now();

      auto found = results.find(lastElement->first);
      if (found == results.end()) {
        found = results.emplace(std::make_pair(lastElement->first,std::vector<generic_torrent_result>())).first;
      }

      if (settings->verbose_mode) {
        fprintf(stderr,"Downloading URL %s...",lastElement->second.front().c_str()); 
      }

      Status = parser.downloadPage(lastElement->second.front() , found->second);

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
        fprintf (stderr, "\nFailed at %s, num_retries left = %d\n", lastElement->second.front().c_str(),
                                                                  num_retries);
        --num_retries;
      }

      if (settings->verbose_mode) {
        fprintf(stderr,"Complete!\nWaiting for crawl delay...\n"); 
      }

      return ERRCODE::SUCCESS;
    }

    extractor parser;
  };
}
#endif
