#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/announce_entry.hpp"
#include "libtorrent/bdecode.hpp"
#include "libtorrent/magnet_uri.hpp"

#include "torrent_parser.h"
#include "errlib.h"

using namespace homura;

torrent_parser::torrent_parser()
: ses()
{}

void torrent_parser::extract_magnet_information(std::string magnet) 
{

  int item_limit = 1000000;
  int depth_limit = 1000;

  if ( options::debug_level ) {
    fprintf(stdout," == EXTRACTING MAGNET INFORMATION FOR %s == \n",magnet.c_str());
  }

  std::fstream in;
  in.exceptions(std::ifstream::failbit);
  in.open(magnet.c_str(), std::ios_base::in | std::ios_base::binary);
  in.seekg(0, std::ios_base::end);
  size_t const size = size_t(in.tellg());
  in.seekg(0, std::ios_base::beg);

  std::vector<char> buf(size);
  in.read(buf.data(), size);

  lt::bdecode_node e;
  int pos = -1;
  lt::error_code ec;
  std::cout << "decoding. recursion limit: " << depth_limit
    << " total item count limit: " << item_limit << "\n";
  int const ret = lt::bdecode(&buf[0], &buf[0] + buf.size(), e, ec, &pos
    , depth_limit, item_limit);

  std::printf("\n\n----- raw info -----\n\n%s\n", print_entry(e).c_str());

  if (ret != 0) {
    std::cerr << "failed to decode: '" << ec.message() << "' at character: " << pos<< "\n";
    return;
  }

  lt::torrent_info const t(e);
  e.clear();
  std::vector<char>().swap(buf);

  // print info about torrent
  std::printf("\n\n----- torrent file info -----\n\n"
    "nodes:\n");

}
