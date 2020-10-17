#include <iostream>
#include "url_scheduler.h"
#include "errlib.h"

using namespace homura;

url_scheduler::url_scheduler(){}

// urlhash::iterator url_scheduler::insert_table(std::shared_ptr<url_table_base> new_entry)
// {
//   // auto website_name = new_entry->get_website();
//   // auto inTable = hashed_url_tables.find(website_name);
//   // if (inTable != hashed_url_tables.end()) {
//   //   return inTable;
//   // }
// 
//   // hashed_url_tables[website_name] = new_entry;
// 
//   // auto itor = sorted_url_tables.begin();
//   // while (itor != sorted_url_tables.end()) {
//   //   if (new_entry->get_delay().count() > (*itor)->get_delay().count()) {
//   //     break;
//   //   }
//   //   ++itor;
//   // }
// 
//   // return hashed_url_tables.find(website_name);
// }

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
        continue;
      }
      finished = false;
      if (table->ready_for_request()) {
        table->download_next_URL();
      }
    }
  }

  for (auto &table: sorted_url_tables) {
    table->cache();
  }
  return ERRCODE::SUCCESS;
}

void url_scheduler::print_tables() {
  for (auto const &itor : sorted_url_tables) {
    itor->print();
  }
}
