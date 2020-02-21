#include "nyaasi_results.h"
#include "errlib.h"
#include <regex>
#include <stdio.h>

nyaasi_results::nyaasi_results(std::string torrent_,
                               std::string torrent_title_,
                               std::string torrent_size_) 
: torrent(torrent_),
  torrent_title(torrent_title_),
  torrent_size(torrent_size_)
{}

void nyaasi_results::print_torrent_magnet() 
{
  fprintf(stdout,"%s\n",torrent.c_str());
}

void nyaasi_results::print_others()
{
  fprintf(stdout,"%s\n",torrent_title.c_str());
  fprintf(stdout,"%s\n",torrent_size.c_str());
}
