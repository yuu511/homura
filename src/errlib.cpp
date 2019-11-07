#include <string>
#include <algorithm>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include "errlib.h"

// small error / option handling module

/* options */
int homura::options::debug_level = 0;
int homura::options::threads = 1;
std::string homura::options::search_term = "Gochuumon Wa Usagi Desu Ka?";

void homura::options::set_debug_level(int level) {
  homura::options::debug_level = 
    std::max(level,homura::options::debug_level);
}

void homura::options::set_thread_level(int numt) {
  homura::options::threads = numt;
}

void homura::options::set_search_term(std::string term) {
  homura::options::search_term = term;
}

/* error */
int homura::error_handler::exit_code = ERRCODE::SUCCESS;

void homura::error_handler::set_error_exitcode(int code) {
  homura::error_handler::exit_code = code;
}

std::string homura::parse_error_exitcode(int code) {
  std::string message;
  switch(code) {
    case FAILED_ARGPARSE:
      message = "FAILED_ARGPARSE";
      break;
    case FAILED_CURL:
      message = "FAILED_CURL";
      break;
    case FAILED_PARSE:
      message = "FAILED_FIRST_PARSE";
      break;
    case FAILED_NEW:
      message = "FAILED_NEW";
      break;
    case FAILED_FREE:
      message = "FAILED_FREE";
      break;
    case FAILED_INTCAST:
      message = "FAILED_INTCAST";
      break;
    case FAILED_MYHTML_TREE_INIT:
      message = "FAILED_MYHTML_TREE_INIT";
      break;
    case FAILED_BOUNDS:
      message = "FAILED_BOUNDS";
      break;
    case FAILED_NO_RESULTS:
      message = "FAILED_NO_RESULTS";
      break;
    default:
      message = "UNKNOWN_ERROR";
      break;
  }
  return message;
}

void homura::errprintf(int error_code, const char *format, ...) {
  assert(format != nullptr);
  fflush(nullptr);
  homura::error_handler::set_error_exitcode(error_code);
  va_list args;
  va_start (args,format);
  vfprintf(stderr, format ,args);
  va_end(args);
}
