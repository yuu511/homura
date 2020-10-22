#include <iostream>
#include "url_scheduler.h"
#include "errlib.h"

using namespace homura;

url_scheduler::url_scheduler(){}

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
        table->do_caching_operations();
        continue;
      }
      finished = false;
      if (table->ready_for_request()) {
        table->download_next_URL();
      }
    }
  }

  // for (auto &table: sorted_url_tables) {
  //   table->cache();
  // }
  return ERRCODE::SUCCESS;
}

void url_scheduler::delay_end()
{
  bool ready_to_end = false;
  while (!ready_to_end) {
    ready_to_end = true;
    for (auto &table: sorted_url_tables) {
      if (!table->ready_for_request()) {
        ready_to_end = false;
      }
    }
  }
}

void url_scheduler::print_tables() {
  for (auto const &itor : sorted_url_tables) {
    itor->print();
  }
}
