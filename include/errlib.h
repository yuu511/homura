#ifndef __ERRLIB_H_
#define __ERRLIB_H_

#include <string>
#include <vector>
#include <bitset>

#ifdef DEBUG
#define DBG(fmt, ...) \
  do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)
#else
#define DBG(fmt,...) \
  do {} while(0)
#endif


// small error and option handling module

typedef int HOMURA_ERRCODE;

namespace homura 
{
  /* options */
  struct homura_settings {
    homura_settings();
    bool verbose_mode;
    int number_pages;
    bool force_refresh_cache;
    bool wait_end;
    bool sort_by_size;
    // Bit 0 (LSB) : print magnet, default = 1
    // Bit 1 : print name, default = 1
    std::bitset<2> print_opts;
  };

  /* error */
  enum ERRCODE {
    SUCCESS,
    FAILED_ARGPARSE,
    FAILED_CURL,
    FAILED_PARSE,
    FAILED_NEW,
    FAILED_FREE,
    FAILED_INTCAST,
    FAILED_MYHTML_TREE_INIT,
    FAILED_BOUNDS,
    FAILED_NO_RESULTS,
    FAILED_INVALID_COMMAND,
    FAILED_CACHE_CREATION
  };

  namespace error_handler {
    void set_error_exitcode(int code);
    extern int exit_code;  
    extern bool cache;
  }

  void parse_error_exitcode(int code);
  // sets the return, prints out error message through stderr
  void errprintf(int error_code, const char *format, ...);
}
#endif
