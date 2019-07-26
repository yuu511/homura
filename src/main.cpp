#include <iostream> 
#include <string>
#include <boost/format.hpp>
#include <getopt.h>
#include <libgen.h>
#include "homura.h"

/* 
   print_usage: print out usage.
   use %5s with a vacuous string to indent a line by 5 spaces.
   (boost::format has no "*" printf-like operator)
*/

  //helper functions for print_usage
  void printl(std::string line){
    std::cout << boost::format("%5s%s\n")
      % "" 
      % line;
  }

  void printopt(std::string option, std::string description){
    std::cout << boost::format("%5s%-30s %s\n") 
      % "" 
      % option 
      % description;
  }

  // print_usage
  void print_usage(char** argv){
    std::cout << boost::format
      ("\n%5susage: %s [OPTIONS] ARG\n\n") 
      % "" 
      % basename(argv[0]);
    // %5s%s / %5s%-30s %s is one line
    printl("ARG is the name of the term to be searched.");
    printl("Make sure to enclose the term within a \"\".");
    printl("Given no OPTIONS, homura queries nyaa.si for ARG,");
    printl("and displays the results of the query.");
    std::cout << std::endl;
    printl("All special search options should work. ( \"\",|,(),- )");
    printl("REMINDER: if you need to use the \" operator, "
                     "use \\\" on the command line. (see examples)");
    printl("for more information about special search terms, go to https://nyaa.si/help");
    std::cout << std::endl;
    printl("OPTIONS:");
    printopt("[-v,--verbose]",": enable verbose logging");
    printopt("[-t,--threads] THREADCOUNT",": use a pool of THREADCOUNT threads");
    printopt("[--help]",": print out usage message");
    std::cout << std::endl;
    printl("EXAMPLES:");
    printl("search:");
    printl("\% homura \"Ping Pong the Animation\"");
    printl("\% homura \"Initial D\"");
    std::cout << std::endl;
    printl("advanced search:");
    printl("\% homura \"(Monogatari) | Madoka\"");
    printl("// search for Monogatari and Madoka. Show Monogatari results first.");
    std::cout << std::endl;
    printl("\% homura \"\\\"school days\\\"\"");
    printl("// search for \"school days\" but not \"days school\".");
    std::cout << std::endl;
}

void parse_args (int argc, char **argv) {
   bool _VERBOSE = false;
   int  _THREADCOUNT = 1;
   int opt;
   while (1) {  
     int option_index = 0;
     static struct option long_options[] = {
       { "verbose" , no_argument       ,  0   , 'v' },
       { "help"    , no_argument       ,  0   , 'h' },
       { "threads" , required_argument ,  0   , 't' },
       {  NULL     , 0                 , NULL ,  0  }
     };
     opt = getopt_long(argc,argv, "vt:",
                 long_options, &option_index);
     if (opt == -1)		 
       break;
     switch (opt) {
       case 'v':
         _VERBOSE = true;
         break;
       case 'h':
	 print_usage(argv);
	 exit(0);
       case 't':
         break;
       case '?':
         std::cerr << boost::format("incorrect option %c\n") % optopt;
         std::cerr << "for usage: homura --help\n";
	 exit(1);
         break;
     }
   }
   if (optind + 1 > argc) {
     std::cerr << "for usage: homura --help\n";
     std::cerr << boost::format("incorrect usage: expected %i arguments, recieved %i\n")
                  % (argc + 1)
		  % argc;
     exit(1);
   }
   homura::query_packages(std::string(argv[optind]),_VERBOSE,_THREADCOUNT);
}

int main (int argc, char ** argv) {
  parse_args(argc,argv);
  return 0;
}
