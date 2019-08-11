#ifndef __ERRLIB_H_
#define __ERRLIB_H_

#include <string>

// small error and option handling module

/* options */
namespace homura {
  namespace options{
    extern int debug_level;
    extern int threads;
    void set_debug_level(int level);
    void set_thread_level(int numt);
  }
}

/* error */
enum ERRCODE{
  success,
  FAILED_ARGPARSE,
  FAILED_CURL,
  FAILED_PARSE,
  FAILED_NEW,
  FAILED_FREE,
  FAILED_INTCAST,
  FAILED_MYHTML_TREE_INIT
};

struct error_handler {
  static int exit_code;  
};

void set_error_exitcode(int code);
std::string parse_error_exitcode(int code);

// sets the return, prints out error message through stderr
void errprintf(int error_code, const char *format, ...);


#endif
