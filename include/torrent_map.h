#ifndef __TORRENT_MAP_ 
#define __TORRENT_MAP_ 

#include <string>
#include <vector>
#include <unordered_map>

namespace homura 
{
  class torrent_map_node {
  public:
    torrent_map_entry(std::string website_url);
    insert_entry(std::string torrent_link,std::string torrent_title);
  private:
    std::string website_url;
    std::unordered_map<std::string,std::string> scraped_torrents; 
  }
}
#endif
