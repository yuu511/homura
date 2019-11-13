#include <cstdlib>
#include <cstdio>
#include <memory>
#include <boost/make_shared.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>

#include <libtorrent/extensions/ut_metadata.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/extensions/smart_ban.hpp>

#include "torrent_parser.h"
#include "errlib.h"

using namespace homura;

torrent_parser::torrent_parser()
{}

void torrent_parser::extract_magnet_information(std::string magnet) 
{
  fprintf (stderr,"magnet %s ",magnet.c_str());
  lt::session s;
  s.add_extension(&libtorrent::create_ut_metadata_plugin);
  s.add_extension(&libtorrent::create_ut_pex_plugin);
  s.add_extension(&libtorrent::create_smart_ban_plugin);
  // s.start_lsd();
  // s.start_upnp();
  // s.start_natpmp();

  lt::add_torrent_params p;
  p.url = magnet;
  p.save_path = "/tmp/";
  p.flags |= p.flag_upload_mode;
  p.flags &= ~p.flag_auto_managed;
  lt::torrent_handle h = s.add_torrent(p);
  while(!h.is_valid()){;}
  p.ti = boost::make_shared<lt::torrent_info>(h.info_hash());
  // auto info = lt::torrent_info(h.info_hash());
  std::fprintf(stderr,"name %d\n",p.ti->num_pieces());
}
