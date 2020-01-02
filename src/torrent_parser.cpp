#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <libtorrent/torrent_flags.hpp>
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
  lt::error_code ec;
  lt::add_torrent_params p = lt::parse_magnet_uri(magnet,ec);
  p.save_path = "/tmp/";
  p.flags |= lt::torrent_flags::upload_mode;
  p.flags &= ~lt::torrent_flags::auto_managed;
  lt::torrent_handle h = s.add_torrent(p);
  while(!h.status().has_metadata){;}
}
