#ifndef __NYAASI_RESULTS_H_
#define __NYAASI_RESULTS_H_

#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

  struct nyaasi_results {
    nyaasi_results(std::string torrent, 
                   std::string torrent_title,
                   std::string torrent_size);
    void print_torrent_magnet();
    void print_others();
    std::string torrent;
    std::string torrent_title;
    std::string torrent_size;
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version) 
    {
      ar << torrent; 
      ar << torrent_title; 
      ar << torrent_size; 
    }
};
#endif
