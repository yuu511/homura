#ifndef __NYAASI_RESULTS_H_
#define __NYAASI_RESULTS_H_

#include <string>

  struct nyaasi_results {
    nyaasi_results(std::string torrent, 
                   std::string torrent_title,
                   std::string torrent_size);
    void print_torrent_magnet();
    void print_others();
    std::string torrent;
    std::string torrent_title;
    std::string torrent_size;
  };
#endif
