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
