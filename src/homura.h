#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <string>
#include "magnet_table.h"

namespace homura
{
  magnet_table *search_nyaasi(std::string args, int LOG_LEVEL, int threadcount);
}

#endif
