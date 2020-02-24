#ifndef __NYAASI_RESULTS_H_
#define __NYAASI_RESULTS_H_

#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

struct nyaasi_results;
namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data ( Archive &ar, const nyaasi_results *T, unsigned int file_version);
}}


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
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) 
  {
  }

};

namespace boost { namespace serialization {
template <class Archive>
inline void save_construct_data (
Archive &ar, const nyaasi_results *T, unsigned int file_version) {
  ar & T->torrent;
  ar & T->torrent_title;
  ar & T->torrent_size;
}

template <class Archive>
inline void load_construct_data(
  Archive &ar, nyaasi_results *t, unsigned int file_version) {
  std::string torrent_;
  std::string torrent_title_;
  std::string torrent_size_;
  ar >> torrent_;
  ar >> torrent_title_;
  ar >> torrent_size_;
  ::new(t)nyaasi_results(torrent_,torrent_title_,torrent_size_);
}
}}
#endif
