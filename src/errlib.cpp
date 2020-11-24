#include <string>
#include <algorithm>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include "errlib.h"

// small error / option handling module

/* options */
namespace homura {
  namespace options {
    bool verbose_mode = false;
    int number_pages = 0;
    bool force_refresh_cache = false;
    bool wait_end = false;
    bool sort_by_size = false;
    // Bit 0 (LSB) : print magnet, default = 1
    // Bit 1 : print name, default = 1
    std::bitset<2> print(0x3);
    std::string regex = "";
    std::string search_term = "";
    std::string command = "";
  }
}

/* error */
int homura::error_handler::exit_code = ERRCODE::SUCCESS;
bool homura::error_handler::cache = false;

void homura::error_handler::set_error_exitcode(int code) 
{
  homura::error_handler::exit_code = code;
}

void homura::parse_error_exitcode(int code) 
{
  std::string message;
  switch(code) {
    case SUCCESS:
      fprintf(stderr,"Exited Sucessfully.\n");
      return;
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
    case FAILED_INVALID_COMMAND:
      message = "FAILED_INVALID_COMMAND";
      break;
    default:
      message = "UNKNOWN_ERROR";
      break;
  }
  fprintf (stderr,"Exited with error code %s.\n",message.c_str());
}

void homura::errprintf(int error_code, const char *format, ...) 
{
  assert(format != nullptr);
  fflush(nullptr);
  homura::error_handler::set_error_exitcode(error_code);
  va_list args;
  va_start (args,format);
  vfprintf(stderr, format ,args);
  va_end(args);
}
