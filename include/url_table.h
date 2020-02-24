#ifndef __URLTABLE_H_
#define __URLTABLE_H_

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <stdio.h>
#include <regex>

#include <filesystem>
#include <fstream>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "curl_container.h"
#include "tree_container.h"
#include "errlib.h"


namespace homura 
{
  using urlpair = std::pair<std::string,const char*>;

  class url_table_base {
  public:
    url_table_base(std::string website_,
                   std::chrono::milliseconds delay_);
    url_table_base();
    virtual ~url_table_base();
    url_table_base(const url_table_base &) = delete;
    url_table_base &operator= (const url_table_base&) = delete;

    std::string pop_one_url();
    bool empty();
    void update_time();
    bool ready_for_request();

    std::vector<std::string> *get_url_table();

    std::string get_website();
    std::chrono::milliseconds get_delay();

    // template functions
    virtual HOMURA_ERRCODE populate_url_list(std::string searchtag);
    virtual std::pair<std::string,const char *> download_next_URL();
    virtual HOMURA_ERRCODE parse_next_page(urlpair &pair);
    virtual void cache();
    virtual void print();
  private:
    std::string website;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<std::string> *website_urls;
  };

  template <typename parser, typename result_type>
  class url_table : public url_table_base {
  using resultmap = std::unordered_map<std::string,std::vector<result_type>*>;
  using resultcache = std::unordered_map<std::string,std::vector<result_type>>;
  public:
    url_table(std::string website_,
              std::chrono::milliseconds delay_,
              parser extractor_)
      : url_table_base(website_,delay_),         
        extractor(std::move(extractor_)){}

    ~url_table() {
      for (auto itor : results) {
        delete (itor.second); 
      }
    }
    url_table(const url_table &) = delete;
    url_table &operator= (const url_table&) = delete;

    // template functions
    void print_entry(result_type &res)
    {
      bool magnet = options::print[0];
      bool others = options::print[1]; 
      if (!options::regex.size()) {
        if (others) {
          res.print_others();
        }
        if (magnet) {
          res.print_torrent_magnet();
        }
        if (others) {
          fprintf(stdout,"\n\n");
        }
      }
      else {
        std::smatch sm;
        std::regex pattern(options::regex);
        if (std::regex_match(res.torrent,sm,pattern)) {
          if (others) {
            res.print_others();
          }
          if (magnet) {
            res.print_torrent_magnet();
          }
          if (others) {
            fprintf(stdout,"\n\n");
          }
        }
      }
    }

    void print_table(std::vector<result_type> *table) {
      for (auto &itor : *table) {
        print_entry(itor);
      }
    }

    void print () 
    {
      for(auto &itor : results) {
        print_table(itor.second);
      }
    }

    void load_cache()
    {
      if(options::force_refresh_cache) return;
      for (auto &itor : searchtags ) {
        auto archive_name = cache_name_protocol(itor.first);
        if (!std::filesystem::exists(archive_name)) return;
        std::ifstream ifs(archive_name);
        boost::archive::text_iarchive ia(ifs);
        ia >> results;
        fprintf (stderr,"cache size %zd\n",results.size());
        this->print();
        // // if the # of urls in the cache != # of urls in the queue, ignore it
        // auto ccitor = cache.find(itor.first);
        // if (ccitor == cache.end() || itor.second.size() != ccitor->second->size()) continue;
        // auto website_list = get_url_table();
        // website_list->erase(std::remove_if(website_list->begin(),website_list->end(),
        // [this,cache](std::string p)
        // {
        //   auto index = cache.find(p);
        //   if ( index != cache.end()) {
        //     // results.emplace(p,index->second());    
        //     if (options::debug_level){
        //       fprintf(stderr,"loading %s into the cache\n",p.c_str());
        //     }
        //   }
        // }),website_list->end());
      }
    }

    void cache() 
    {
      for (auto &itor : searchtags ) {
        std::string archive_name = cache_name_protocol(itor.first);
        resultmap cache;
        for (auto &vecitor : itor.second) {
          auto found = results.find(vecitor);
          if (found != results.end()) {
             cache.emplace(vecitor,found->second);
          }
        }
        if (options::debug_level) {
          for (auto &debugitor : cache) {
            fprintf (stderr,"Added url %s to the cache\n",debugitor.first.c_str());
          }
        }
        std::ofstream ofs (archive_name);  
        boost::archive::text_oarchive oa (ofs);
        oa << cache;
      }
    }

    HOMURA_ERRCODE populate_url_list(std::string searchtag) 
    {
      HOMURA_ERRCODE status;

      urlpair firstpair = extractor.download_first_page(searchtag);
      auto result_vector = new std::vector<result_type>;
      status = extractor.parse_HTML(firstpair.second,result_vector);
      if (status != ERRCODE::SUCCESS) {
        delete result_vector;
        return status;
      }
      results.emplace(firstpair.first,result_vector);
      print_table(result_vector);
      
      // url list should be all urls we need to parse 
      // (excluding the first one, which is parsed above)
      auto tblptr = get_url_table();
      status = extractor.getURLs(firstpair.second,tblptr);
      if (status != ERRCODE::SUCCESS) return status;
      std::vector<std::string> searchtag_urls;
      // copy url table for cache
      searchtag_urls.insert(searchtag_urls.end(),tblptr->begin(),tblptr->end());
      searchtag_urls.push_back(firstpair.first);
      searchtags.emplace(searchtag,searchtag_urls);

      update_time();
      load_cache();
      
      return ERRCODE::SUCCESS;
    }

    urlpair download_next_URL()
    {
      std::string url = pop_one_url();
      const char *result = extractor.downloadOne(url);
      update_time();
      return std::make_pair(url,result);
    }

    HOMURA_ERRCODE parse_next_page(urlpair &pair)
    {
      HOMURA_ERRCODE status;
      auto result_vector = new std::vector<result_type>;
      status = extractor.parse_HTML(pair.second,result_vector);
      if (status != ERRCODE::SUCCESS) return status;
      results.emplace(pair.first,result_vector);
      print_table(result_vector);
      return ERRCODE::SUCCESS;
    }

    std::string cache_name_protocol(std::string searchtag)
    {
      return ".homuracache_" + get_website() + "_" + searchtag;
    }
  private:
    parser extractor;
    resultmap results;
    std::unordered_map<std::string,std::vector<std::string>> searchtags;
  };
}

#endif
