#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include "errlib.h"

// small error handling module

int error_handler::exit_code = EXIT_SUCCESS;

void set_error_exitcode(int code){
  error_handler::exit_code = code;
}

std::string parse_error_exitcode(int code){
  std::string message;
  switch(code){
    case FAILED_CURL:
      message = "FAILED_CURL";
    case FAILED_MALLOC:
      message = "FAILED_MALLOC";
    case FAILED_REALLOC:
      message = "FAILED_REALLOC";
    default:
      message = "UNKNOWN_ERROR";
  }
  return "ERROR: homura RETURNED NON ZERO EXIT CODE " + message;
}

void errprintf(int error_code, const char *format, ...) {
  assert(format != nullptr);
  fflush(nullptr);
  set_error_exitcode(error_code);
  va_list args;
  va_start (args,format);
  vfprintf(stderr, format ,args);
  va_end(args);
}
