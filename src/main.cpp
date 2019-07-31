#include <string>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include "homura.h"
#include "errlib.h"

  /* print_usage helper functions */
  // n space indent for all strings
  // first part of option string will be a minimum of 30 characters long 
  void println(int whitespace, const char *line){
    fprintf(stdout,"%*s%s\n",whitespace,"", line);
  }
  void printopt(int whitespace, const char *option, const char *description){
    fprintf(stdout,"%*s%-30s%s\n",whitespace, "", option, description);
  }

  /* print_usage */ 
  // @param[in] argument array (used to parse executable name)
  void print_usage(){
    fprintf(stdout,"\n");
    println(5,"USAGE:");
    println(5,"homura [-vdt:] ARG");
    println(7,"Search nyaa.si for expression ARG.");
    println(7,"Make sure to enclose the expression within a \"\".");
    println(7,"optional args -v,-d,-t defined in OPTIONS");
    fprintf(stdout,"\n");
    println(5,"homura --help");
    println(7,"print out usage message");
    fprintf(stdout,"\n");
    println(5,"OPTIONS:");
    printopt(5,"[-v,--verbose]"," : logging, prints out actions as they are preformed");
    printopt(5,"[-d,--debug]"," : more extensive logging, prints out full html files");
    printopt(5,"[-t,--threads] THREADCOUNT"," : use a pool of THREADCOUNT threads ");
    printopt(5,"","   (THREADCOUNT is a positive integer)");
    printopt(5,"[--help]"," : print out usage message");
    fprintf (stdout,"\n");
    println(5,"EXAMPLES:");
    println(5,"all site-defined advanced search options should work. ( \"\",|,(),- )");
    println(5,"for more information about advanced search options : https://nyaa.si/help");
    println(5,"REMINDER: if you need to use the \" operator, "
                     "use \\\" when inside a quote.");
    fprintf (stdout,"\n");
    println(5,"search examples:");
    println(7,"\% homura \"Ping Pong The Animation\"");
    println(7,"\% homura --threads 5 \"Initial D\"");
    fprintf(stdout,"\n");
    println(5,"advanced search examples:");
    println(7,"\% homura \"Monogatari|Madoka\"");
    println(7,"// display results for \"Monogatari\" OR \"Madoka\"");
    println(7,"\% homura \"\\\"School Days\\\"\"");
    println(7,"// search for \"School Days\" but not \"Days School\".");
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
         return;
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
   homura::query_packages(std::string(argv[optind]),_VERBOSELEVEL,_THREADCOUNT);
}

int main (int argc, char ** argv) {
  error_handler::exit_code = EXIT_SUCCESS;
  parse_args(argc,argv);
  if (error_handler::exit_code){
    fprintf(stderr,"%s\n",parse_error_exitcode(error_handler::exit_code).c_str());
  }
  return error_handler::exit_code;
}
