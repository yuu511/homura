#ifndef __ERRLIB_H_
#define __ERRLIB_H_

#include <string>
#include <vector>
#include <bitset>

// small error and option handling module

typedef int HOMURA_ERRCODE;

namespace homura 
{
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
    FAILED_INVALID_COMMAND
  };

  /* options */
  namespace options {
    extern int debug_level;
    extern int force_refresh_cache;
    extern std::bitset<2> print;
    extern std::string regex;
    extern std::string search_term;
    extern std::string command;
  }

  namespace error_handler {
    void set_error_exitcode(int code);
    extern int exit_code;  
  }

  void parse_error_exitcode(int code);
  // sets the return, prints out error message through stderr
  void errprintf(int error_code, const char *format, ...);
}
#endif
