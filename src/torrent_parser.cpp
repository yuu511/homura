#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <libtorrent/session_handle.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/settings_pack.hpp>

#include <libtorrent/extensions/ut_metadata.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/extensions/smart_ban.hpp>

#include "torrent_parser.h"
#include "errlib.h"

using namespace homura;

// session settings
torrent_parser::torrent_parser()
{
  s.apply_settings(libtorrent::default_settings());
}

void torrent_parser::extract_magnet_information(std::string magnet) 
{
  fprintf (stderr,"%s\n",magnet.c_str());
  lt::add_torrent_params p;
  lt::error_code ec;
  libtorrent::parse_magnet_uri(magnet,p,ec);
  p.save_path = "/tmp/";
  p.flags |= p.flag_upload_mode;
  p.flags &= ~p.flag_auto_managed;
  lt::torrent_handle h = s.add_torrent(p);
  lt::torrent_status stat = h.status();
  while(!h.is_valid() && !stat.has_metadata){;}
  lt::torrent_info const t(h.info_hash());
  std::stringstream ih;
  ih << t.info_hash();
  if (options::debug_level > 0 ) {
    std::printf("number of pieces: %d\n"
      "piece length %d\n"
      "info hash %s\n"
      "comment %s\n"
      "created by %s\n"
      "magnet link %s\n"
      "name: %s\n"
      "number of files %d\n\n"
      , t.num_pieces()
      , t.piece_length()
      , ih.str().c_str()
      , t.comment().c_str()
      , t.creator().c_str()
      , make_magnet_uri(t).c_str()
      , t.name().c_str()
      , t.num_files());

      std::printf("Number of DHT nodes: %zd\n",t.nodes().size());
      for (auto itor : t.nodes()) {
         std::printf("Tracker %s Port %d \n "
                     , itor.first.c_str()
                     , itor.second);
      }
   }
   s.post_dht_stats();
}
