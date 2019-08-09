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
    case FAILED_ARGPARSE:
      message = "FAILED_ARGPARSE";
      break;
    case FAILED_CURL:
      message = "FAILED_CURL";
      break;
    case FAILED_MALLOC:
      message = "FAILED_MALLOC";
      break;
    case FAILED_REALLOC:
      message = "FAILED_REALLOC";
      break;
    case FAILED_FIRST_PARSE:
      message = "FAILED_FIRST_PARSE";
      break;
    case FAILED_NEW:
      message = "FAILED_NEW";
      break;
    case FAILED_INTCAST:
      message = "FAILED_INTCAST";
      break;
    case FAILED_FREE:
      message = "FAILED_FREE";
      break;
    case FAILED_MYHTML_TREE_INIT:
      message = "FAILED_MYHTML_TREE_INIT";
      break;
    default:
      message = "UNKNOWN_ERROR";
      break;
  }
  return "ERROR: homura returned non-zero exit code " + message;
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
