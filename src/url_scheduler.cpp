#include <iostream>
#include "url_scheduler.h"
#include "errlib.h"

using namespace homura;

url_scheduler::url_scheduler() {}

void url_scheduler::extract_magnets(){};

table_iterator url_scheduler::get_table_position(std::string check) 
{
  return entry_hashtable.find(check);
}

bool url_scheduler::table_exists(table_iterator check) 
{
  return check != entry_hashtable.end() ? true : false;
}

table_iterator url_scheduler::insert_table (std::string key,
                                            std::chrono::milliseconds delay) 
{
  auto new_table = std::make_shared<url_table>(key,delay);
  auto itor = entries.begin();
  while (itor != entries.end()) {
    if (new_table->get_delay().count() > (*itor)->get_delay().count()){
      break;
    }
    ++itor;
  }
  entries.insert(itor,new_table);
  entry_hashtable.emplace(new_table->get_website(),new_table);
  return get_table_position(key);
}

void url_scheduler::crawl() 
{
  bool finished = false;
  while (!finished) {
    finished = true;
    for (auto table: entries) {
      if (table->empty()) continue;
      finished = false;
      if (table->ready_for_request()) {
        table->parse_one_url();
      }
    }
  }
}

void url_scheduler::print_table(){
  for (auto itor : entries) {
    std::cout << "Entry Name: " 
    << itor->get_website()
    << " Entry Delay: "
    << itor->get_delay().count() << std::endl;
  }
}
