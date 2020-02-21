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

    std::string pop_one_url();
    bool empty();
    void update_time();
    bool ready_for_request();

    void copy_url_table(std::vector<std::string> &urls);

    std::string get_website();
    std::chrono::milliseconds get_delay();

    virtual HOMURA_ERRCODE populate_url_list(std::string searchtag);
    virtual std::pair<std::string,const char *> download_next_URL();
    virtual void parse_next_page(urlpair &pair);

    virtual void print();
  private:
    std::string website;
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point last_request;
    std::vector<std::string> website_urls;
  };

  template <typename parser, typename result_type>
  class url_table : public url_table_base {
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

    HOMURA_ERRCODE populate_url_list(std::string searchtag) 
    {
      // first url, HTML contents of first url
      urlpair firstpair = extractor.download_first_page(searchtag);
      if (!firstpair.second) { return error_handler::exit_code; }
      auto extract = extractor.parse_HTML(firstpair.second);
      results.emplace(firstpair.first,extract);
      print_table(extract);
      
      // url list should be all urls we need to parse 
      // (excluding the first one, which is parsed above)
      std::vector<std::string> urls = extractor.getURLs(firstpair.second);
      copy_url_table(urls);
      update_time();

      return ERRCODE::SUCCESS;
    }

    urlpair download_next_URL()
    {
      std::string url = pop_one_url();
      const char *result = extractor.downloadOne(url);
      update_time();
      return std::make_pair(url,result);
    }

    void parse_next_page(urlpair &pair)
    {
      auto extract = extractor.parse_HTML(pair.second);
      results.emplace(pair.first,extract);
      print_table(extract);
    }

  private:
    parser extractor;
    std::unordered_map<std::string,std::vector<result_type>*> results;
  };
}

#endif
