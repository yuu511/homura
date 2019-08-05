#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <curl/curl.h>
#include <string>
#include "magnet_table.h"

namespace homura{
  magnet_table *query_packages(std::string args, int LOG_LEVEL, int threadcount);
  void free_mtable(magnet_table*);
}

#endif
