#include <cstdlib>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>

#include "torrent_parser.h"
#include "errlib.h"

using namespace homura;

torrent_parser::torrent_parser()
{}

void torrent_parser::extract_magnet_information(std::string magnet) 
{
  fprintf (stderr,"magnet %s",magnet.c_str());
  lt::session s;
  lt::add_torrent_params p;
  p.url = magnet;
  p.save_path = "/tmp/";
  p.flags |= p.flag_upload_mode;
  p.flags &= ~p.flag_auto_managed;
  lt::torrent_handle h = s.add_torrent(p);
  // add the info hash
  // auto info = lt::torrent_info(p.info_hash);
  // while (!info.is_valid())
  // {;}
  // fprintf(stderr," %s\n",info.name().c_str());
}
