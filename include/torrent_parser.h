#ifndef __TORRENT_PARSER_H_
#define __TORRENT_PARSER_H_

#include <string>

namespace homura 
{
  class torrent_parser {
  public:
    torrent_parser();
    void extract_magnet_information(std::string magnet);
  private:
    std::string magnet_information;
  };
}

#endif
