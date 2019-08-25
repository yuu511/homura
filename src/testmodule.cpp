// small fkin testmodule
#include <thread>
#include <iostream>
#include <ctime>
#include <map>
#include <unordered_map>
#include <deque>
#include <memory>

#include "url_table.h"
#include "errlib.h"

namespace homura {
  enum website {
    nyaasi, 
    nyaapantsu, 
    horriblesubs
  };
}

void thread_function(homura::url_table *test, std::string insert, std::chrono::steady_clock::time_point epoch) {
  test->update_time();    
}

void test_urltable(){
  homura::options::set_debug_level(2); 
  auto now = std::chrono::steady_clock::now();
  std::string url1 = "url1";
  std::string url2 = "url2";
  homura::url_table *test = new homura::url_table(homura::website::nyaasi, std::chrono::seconds(5));
  std::thread t1(thread_function,test,"T1",now);
  std::thread t2(thread_function,test,"T2",now);
  std::thread t3(thread_function,test,"T3",now);
  std::thread t4(thread_function,test,"T4",now);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  delete (test);
}

//void url_vector_insert(std::vector<std::unique_ptr<homura::url_table>> table, std::unique_ptr<homura::url_table> urls) {
//  // std::vector<int, homura::url_table*>::iterator itor;
//  // for (auto itor = table->begin() ; itor != table->end(); ++itor ) {
//  //   if ( itor->second->get_delay().count() <  urls->get_delay().count() ) { 
//  //     std::cout << itor->second->get_delay().count() << std::endl;
//  //     std::cout << urls->get_delay().count() << std::endl;
//  //     break;
//  //   }
//  // }
//  // table->insert(table->end(), std::make_pair(website,urls));
//}

void inserter(std::unique_ptr<std::vector<std::shared_ptr<homura::url_table>>> &url, 
              std::unique_ptr<std::unordered_map<int,std::shared_ptr<homura::url_table>>> &url_hash, 
	      std::shared_ptr<homura::url_table> &table ) {

  for (auto itor : *url_hash) {
    if ( itor.first == table->get_website() ) {
      for ( auto entries : table->get_urls() ) {
        itor.second->insert(entries);
      }
      return;
    }
  }
  // not in hash
  url_hash->emplace(table->get_website(),table); 
  for (auto itor = url->begin() ; itor != url->end(); ++itor ) {
    if (table->get_delay().count() > (*itor)->get_delay().count()) {
      url->insert(itor, table);
      return;
    }
  }
  url->push_back(table);
}

void main_loop ( std::unique_ptr<std::vector<std::shared_ptr<homura::url_table>>> &url ) {
  // make vector of interators
  std::vector<std::pair< std::vector<std::string>,std::vector<std::string>::iterator> > iterators;
  for (auto itor : *url) {
    iterators.push_back(std::make_pair(itor->get_urls(), itor->get_urls().begin()));
  }

  for (auto itor : iterators) {
    if (itor.first->get_urls().end() == itor.second){
      std::cout << "IT's good"; 
    }  
  }

  for (auto itor : iterators) {
    for (itor.second != itor.first.end() ; ++itor.second){
      std::cout << *itor.second();
    }
  }

  // once all iterators at end, terminate the loop

  // for (;;) {
  //   
  // }
}


void test_urlinserter() {

  homura::options::set_debug_level(2); 

  // kinda messy but meh
  auto url = std::make_unique<std::vector<std::shared_ptr<homura::url_table>>>();
  auto url_hash = std::make_unique<std::unordered_map<int,std::shared_ptr<homura::url_table>>>() ;

  // insert 10 horriblesubs urls with 2s delay
  auto hsub_urls = std::make_shared<homura::url_table>(homura::website::horriblesubs, std::chrono::seconds(2));
  for (int i = 0 ; i < 10 ; ++i ) {
    hsub_urls->insert("HORRIBLESUBS_URL " + std::to_string(i) );
  }
  inserter(url,url_hash,hsub_urls); 

  // add 10 nyaapantsu urls with 0s delay
  auto nyaapantsu_urls = std::make_shared<homura::url_table>(homura::website::nyaapantsu, std::chrono::seconds(0));
  for (int i = 0 ; i < 10 ; ++i ) {
    nyaapantsu_urls->insert("NYAAPANTSU_URL " + std::to_string(i) );
  }
  inserter(url,url_hash, nyaapantsu_urls); 


  // add 10 more hsub urls 
  auto hsub_urls2 = std::make_shared<homura::url_table>(homura::website::horriblesubs, std::chrono::seconds(2));
  for (int i = 0 ; i < 10 ; ++i ) {
    hsub_urls2->insert("HORRIBLESUBS_URL_2 " + std::to_string(i) );
  }
  inserter(url,url_hash,hsub_urls2); 

  // add some nyaasi urls 
  auto nyaasi_urls = std::make_shared<homura::url_table>(homura::website::nyaasi, std::chrono::seconds(5));
  for (int i = 0 ; i < 10 ; ++i ) {
    nyaasi_urls->insert("NYAASI_URL " + std::to_string(i) );
  }
  inserter(url,url_hash,nyaasi_urls); 

  for (auto itor : *url) {
    for (auto entry : itor->get_urls()) {
      std::cout << "ENTRY : " << entry << " DELAY : " << itor->get_delay().count() << std::endl;
    }
  }
  main_loop(url);

}

int main () {
  // test_urltable(); 
  test_urlinserter();
  return 0;
}

