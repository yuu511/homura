#include <iostream> 
#include <string>
#include <boost/format.hpp>
#include <getopt.h>
#include <libgen.h>
#include "homura.h"

void print_usage(char** argv){
  std::cout << boost::format("usage: %s [OPTIONS]\n\n") 
               % basename(argv[0]);
  std::cout << "given no OPTIONS, homura will print out list of all "
               "currently tracked torrents. \n\n";
  std::cout << "OPTIONS:\n";
  std::cout << boost::format("%-20s %s\n") 
	           % "[-v,--verbose]"	       
               % ": print out a verbose version of currently tracked torrents";
  std::cout << boost::format("%-20s %s\n") 
	           % "[-q,--query] arg"	       
               % ": Searches nyaa for (arg)";
  std::cout << boost::format("%-20s %s\n") 
	           % "[--help]"	       
               % ": print out usage message";
  std::cout << std::endl;
}

void parse_args (int argc, char **argv) {
   int opt;
   while (1) {  
     int option_index = 0;
     static struct option long_options[] = {
       { "verbose" , no_argument       ,  0   , 'v' },
       { "help"    , no_argument       ,  0   , 'h' },
       { "query"   , required_argument ,  0   , 'q' },
       {  NULL     , 0                 , NULL ,  0  }
     };
     opt = getopt_long(argc,argv, "vq:",
                 long_options, &option_index);
     if (opt == -1)		 
       break;
     switch (opt) {
       case 'v':
         std::cout << "verbose \n";
         break;
       case 'q':
         homura::query_packages(std::string(optarg),false);
         break;
       case 'h':
	     print_usage(argv);
         break;
       case '?':
	     print_usage(argv);
         break;
     }
   }
   if (optind > argc) {
     print_usage(argv);
     exit(1);
   }
}

int main (int argc, char ** argv) {
  
  parse_args(argc,argv);
  return 0;
}
