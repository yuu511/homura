#ifndef __MAGNET_TABLE_H_
#define __MAGNET_TABLE_H_

#include <vector>
#include <string>

/* container for torrent title, magnet link */
struct name_magnet{
  const std::string *name;  
  const std::string *magnet;
  name_magnet();
  ~name_magnet();
};

using magnet_table = std::vector<name_magnet*>;

#endif
