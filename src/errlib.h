#ifndef __ERRLIB_H_
#define __ERRLIB_H_

#include <string>
#include <vector>

// small error and option handling module

namespace homura {
  /* error */
  enum ERRCODE {
    success,
    FAILED_ARGPARSE,
    FAILED_CURL,
    FAILED_PARSE,
    FAILED_NEW,
    FAILED_FREE,
    FAILED_INTCAST,
    FAILED_MYHTML_TREE_INIT,
    FAILED_BOUNDS
  };

  /* options */
  namespace options {
    extern int debug_level;
    extern int threads;
    void set_debug_level(int level);
    void set_thread_level(int numt);
  }

  namespace error_handler {
    void set_error_exitcode(int code);
    extern int exit_code;  
    extern std::vector <int> exitcode_stack;
  }

  void unwind_exit_code_stack(std::vector<int> exitcode_stack);
  std::string parse_error_exitcode(int code);
  // sets the return, prints out error message through stderr
  void errprintf(int error_code, const char *format, ...);
}
#endif
