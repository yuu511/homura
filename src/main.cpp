#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>
#include "homura.h"
#include "errlib.h"

int main(int argc, char const* argv[]) {
  if (argc != 2) {
          fprintf(stderr, "usage: %s <magnet-url>\n");
          return 1;
  }
  lt::session ses;
  lt::add_torrent_params atp;
  // lt::parse_magnet_uri(argv[1],atp,);
  // atp.save_path = "."; // save in current dir
  // lt::torrent_handle h = ses.add_torrent(atp);

  // ...
  return 0;
}
 

