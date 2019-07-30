#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include "homura.h"

#define MAX_BUF = 1024

  /* print_usage helper functions */
  // 5 space indent for all strings
  // first part of option string will be a minimum of 30 characters long 
  void printl(const char *line){
    fprintf(stdout,"%5s %s\n","", line);
  }
  void printopt(const char *option, const char *description){
    fprintf(stdout,"%5s %-30s%s\n", "", option, description);
  }

  /* print_usage */ 
  // @param[in] argument array (used to parse executable name)
  void print_usage(){
    fprintf(stdout,"\n");
    printl("USAGE:");
    printl("homura [-vdt:] ARG");
    printl("  Search nyaa.si for expression ARG.");
    printl("  Make sure to enclose the expression within a \"\".");
    printl("  optional args -v,-d,-t defined in OPTIONS");
    fprintf(stdout,"\n");
    printl("homura --help");
    printl("  print out usage message");
    fprintf(stdout,"\n");

    printl("OPTIONS:");
    printopt("[-v,--verbose]"," : logging, prints out actions as they are preformed");
    printopt("[-d,--debug]"," : more extensive logging, prints out full html files");
    printopt("[-t,--threads] THREADCOUNT"," : use a pool of THREADCOUNT threads ");
    printopt("","   (THREADCOUNT is a positive integer)");
    printopt("[--help]"," : print out usage message");
    fprintf (stdout,"\n");


    printl("EXAMPLES:");
    printl("all site-defined advanced search options should work. ( \"\",|,(),- )");
    printl("for more information about advanced search options : https://nyaa.si/help");
    printl("REMINDER: if you need to use the \" operator, "
                     "use \\\" when inside a quote.");
    fprintf (stdout,"\n");
    printl("search examples:");
    printl("  \% homura \"Ping Pong The Animation\"");
    printl("  \% homura --threads 5 \"Initial D\"");
    fprintf (stdout,"\n");
    printl("advanced search examples:");
    printl("  \% homura \"Monogatari|Madoka\"");
    printl("  // display results for \"Monogatari\" OR \"Madoka\"");
    printl("  \% homura \"\\\"School Days\\\"\"");
    printl("  // search for \"School Days\" but not \"Days School\".");
    fprintf (stdout,"\n");
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
     opt = getopt_long(argc,argv, "vdt:",
                 long_options, &option_index);
     if (opt == -1)		 
       break;
     switch (opt) {
       case 'v':
         if (!_VERBOSELEVEL)
           _VERBOSELEVEL = 1;
         break;
       case 'd':
         _VERBOSELEVEL = 2;
         break;
       case 'h':
	 print_usage();
	 exit(EXIT_SUCCESS);
       case 't':
         _THREADCOUNT =  atoi(optarg);
         if (_THREADCOUNT < 1){
           fprintf(stderr,"error:-t,--thread expects a positive integer\n");
	   fprintf(stderr,"(recieved %s)\n", optarg);
           exit(EXIT_FAILURE);
         } 
         break;
       case '?':
         fprintf(stderr,"incorrect option %c\n",optopt);
         fprintf(stderr,"for usage: homura --help\n");
	 exit(EXIT_FAILURE);
         break;
     }
   }
   if (optind + 1 > argc) {
     fprintf (stderr,"No search term provided.\n");
     fprintf (stderr,"for usage: homura --help \n");
     exit(EXIT_FAILURE);
   }
   query_packages(argv[optind],_VERBOSELEVEL,_THREADCOUNT);
}

int main (int argc, char ** argv) {
  parse_args(argc,argv);
  return EXIT_SUCCESS;
}
