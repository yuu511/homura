#include <iostream>
#include "url_scheduler.h"
#include "errlib.h"

using namespace homura;

url_scheduler::url_scheduler(){}

urlhash::iterator url_scheduler::table_position(std::string key) 
{
  return hashed_url_tables.find(key);
}

bool url_scheduler::exists_in_table(urlhash::iterator it)
{
  return it == hashed_url_tables.end() ? false : true;
}

void url_scheduler::insert_table(std::shared_ptr<url_table_base> new_entry)
{
  auto itor = sorted_url_tables.begin();
  while (itor != sorted_url_tables.end()) {
    if (new_entry->get_delay().count() > (*itor)->get_delay().count()) {
      break;
    }
    ++itor;
  }
  sorted_url_tables.insert(itor,new_entry);
}

urlvector url_scheduler::return_table()
{
  return sorted_url_tables;
}

HOMURA_ERRCODE url_scheduler::crawl() 
{
  bool finished = false;
  while (!finished) {
    finished = true;
    for (auto &table: sorted_url_tables) {
      if (table->empty()) {
        // table->cache();
        continue;
      }
      finished = false;
      if (table->ready_for_request()) {
        auto webpage = table->download_next_URL();
        auto results = table->parse_page(webpage);
        table->copy_nm_pair(webpage,results);
      }
    }
  }
  if (options::debug_level > 1) {
    this->print_tables();
  }

  return ERRCODE::SUCCESS;
}

void url_scheduler::print_tables() {
  for (auto const &itor : sorted_url_tables) {
    itor->print();
  }
}
