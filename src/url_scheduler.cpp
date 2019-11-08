#include <iostream>
#include "url_scheduler.h"
#include "errlib.h"

using namespace homura;

url_scheduler::url_scheduler() {}

void url_scheduler::extract_magnets(){};

std::shared_ptr<url_table> 
url_scheduler::get_or_insert(std::string check,
                             std::chrono::milliseconds delay) 
{
  auto it = entry_hashtable.find(check);
  if (it != entry_hashtable.end()) {
    return it->second;
  }

  auto new_table = std::make_shared<url_table>(check,delay);
  auto itor = entries.begin();
  while (itor != entries.end()) {
    if (new_table->get_delay().count() > (*itor)->get_delay().count()){
      break;
    }
    ++itor;
  }
  entries.insert(itor,new_table);
  entry_hashtable.emplace(new_table->get_website(),new_table);
  return new_table;
}

void url_scheduler::print_table(){
  for (auto itor : entries) {
    std::cout << "Entry Name: " 
    << itor->get_website()
    << " Entry Delay: "
    << itor->get_delay().count() << std::endl;
  }
}
