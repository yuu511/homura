#ifndef __ERRLIB_H_
#define __ERRLIB_H_

#include <string>

// small error handling module

enum ERRCODE{
  success,
  FAILED_ARGPARSE,
  FAILED_CURL,
  FAILED_MALLOC,
  FAILED_REALLOC
};

struct error_handler {
  static int exit_code;  
};

void set_error_exitcode(int code);
std::string parse_error_exitcode(int code);

void errprintf(int error_code, const char *format, ...);
// sets the return, prints out error message through stderr

#endif
