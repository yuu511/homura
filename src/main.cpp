#include <iostream> 
#include <string>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <getopt.h>
#include <libgen.h>
#include "homura.h"

  /* print_usage helper functions */
  // 5 indents for all strings
  // first part of option string will be a minimum of 30 characters long 
  void printl(std::string line){
    std::cout << boost::format("%5s%s\n")
      % "" % line;
  }
  void printopt(std::string option, std::string description){
    std::cout << boost::format("%5s%-30s %s\n") 
      % "" % option % description;
  }

  /* print_usage */ 
  // @param[in] argument array (used to parse executable name)
  void print_usage(){
    std:: cout << std::endl;
    printl("USAGE:");
    printl("homura [-vdt:] ARG");
    printl("  Search nyaa.si for expression ARG.");
    printl("  Make sure to enclose the expression within a \"\".");
    printl("  optional args -v,-d,-t defined in OPTIONS");
    std::cout << std::endl;
    printl("homura --help");
    printl("  print out usage message");
    std::cout << std::endl;

    printl("OPTIONS:");
    printopt("[-v,--verbose]"," : logging, prints out actions as they are preformed");
    printopt("[-d,--debug]"," : more extensive logging, prints out full html files");
    printopt("[-t,--threads] THREADCOUNT"," : use a pool of THREADCOUNT threads ");
    printopt("","   (THREADCOUNT is a positive integer)");
    printopt("[--help]"," : print out usage message");
    std::cout << std::endl;


    printl("EXAMPLES:");
    printl("all site-defined advanced search options should work. ( \"\",|,(),- )");
    printl("for more information about advanced search options : https://nyaa.si/help");
    printl("REMINDER: if you need to use the \" operator, "
                     "use \\\" when inside a quote.");
    std::cout << std::endl;
    printl("search examples:");
    printl("  \% homura \"Ping Pong The Animation\"");
    printl("  \% homura --threads 5 \"Initial D\"");
    std::cout << std::endl;
    printl("advanced search examples:");
    printl("  \% homura \"Monogatari|Madoka\"");
    printl("  // display results for \"Monogatari\" OR \"Madoka\"");
    printl("  \% homura \"\\\"School Days\\\"\"");
    printl("  // search for \"School Days\" but not \"Days School\".");
    std::cout << std::endl;
}

void parse_args (int argc, char **argv) {
   int _VERBOSELEVEL = 0;
   int  _THREADCOUNT = 1;
   int opt;
   while (1) {  
     int option_index = 0;
     static struct option long_options[] = {
       { "verbose" , no_argument       ,  0   , 'v' },
       { "debug"   , no_argument       ,  0   , 'd' },
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
         _VERBOSELEVEL = 1;
         break;
       case 'd':
         _VERBOSELEVEL = 2;
         break;
       case 'h':
	 print_usage();
	 exit(0);
       case 't':
         _THREADCOUNT =  atoi(optarg);
         if (_THREADCOUNT < 1){
           std::cerr << "error:-t,--thread expects a positive integer\n";
           std::cerr << boost::format("(recieved %s)") % std::string(optarg);
           std::cerr << std::endl;
           exit(1);
         } 
         break;
       case '?':
         std::cerr << boost::format("incorrect option %c\n") % optopt;
         std::cerr << "for usage: homura --help\n";
	 exit(1);
         break;
     }
   }
   if (optind + 1 > argc) {
     std::cerr << "No search term provided.\n";
     std::cerr << "for usage: homura --help\n";
     exit(1);
   }
   homura::query_packages(std::string(argv[optind]),_VERBOSELEVEL,_THREADCOUNT);
}

int main (int argc, char ** argv) {
  parse_args(argc,argv);
  return 0;
}
