#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <string>
#include <chrono>
#include <unordered_map>
#include "magnet_table.h"
#include "url_table.h"

namespace homura {
  struct homura_instance {
  public:
    homura_instance();
    void cleanup();
    ~homura_instance();
    bool query_nyaasi(std::string args);
  private:
    magnet_table *results;
    url_table *requests;
  };
}

#endif
