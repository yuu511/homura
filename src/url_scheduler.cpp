#include "url_scheduler.h"
#include "errlib.h"

using namespace homura;

url_scheduler::url_scheduler() {}

void url_scheduler::extract_magnets(){};

void url_scheduler::insert_entry_back(url_table new_entry) 
{
  this->entries.emplace_back(new_entry);
}

url_table 
url_scheduler::get_or_insert(std::string check,
                                   std::chrono::milliseconds delay) 
{
  for (auto itor:this->entries){
    if (itor.get_website() == check)
      return itor;
  }
  url_table new_entry = url_table(check,delay); 
  insert_entry_back(new_entry);
  return this->entries.back();
}
