#include <string>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include "homura.h"
#include "errlib.h"

using namespace homura;

/* print_usage helper functions */
// n space indent for all strings
// first part of option string will be a minimum of 30 characters long 
void println(int whitespace, const char *line) 
{
  fprintf(stdout,"%*s%s\n",whitespace,"", line);
}

void printopt(int whitespace, const char *option, const char *description) 
{
  fprintf(stdout,"%*s%-30s%s\n",whitespace, "", option, description);
}

/* print_usage */ 
void print_usage() 
{
  fprintf(stdout,"\n");
  println(5,"USAGE:");
  println(5,"homura [FLAGS] [OPTION]");
  println(7,"Refer to flags / options sections");
  fprintf(stdout,"\n");
  println(5,"homura --help");
  println(7,"print out usage message");
  fprintf(stdout,"\n");
  println(5,"FLAGS:");
  printopt(5,"[-v,--verbose]"," : logging, prints out actions as they are preformed");
  printopt(5,"[-d,--debug]"," : more extensive logging, prints out full html files");
  printopt(5,"[-t,--threads] THREADCOUNT"," : use a pool of THREADCOUNT threads ");
  printopt(5,"","   (THREADCOUNT is a positive integer)");
  printopt(5,"[--help]"," : print out usage message");
  fprintf (stdout,"\n");
  println(5,"OPTIONS:");
  printopt(5,"search [TARGET]"," : query results for TARGET and print to stdout");
  fprintf (stdout,"\n");
  println(5,"EXAMPLES:");
  fprintf (stdout,"\n");
  println(5,"[search]");
  println(5,"all site-defined advanced search options should work. ( \"\",|,(),- )");
  println(5,"for more information about advanced search options : https://nyaa.si/help");
  println(5,"REMINDER: if you need to use the \" operator, "
                   "use \\\" when inside a quote.");
  fprintf(stdout,"\n");
  println(5,"simple search");
  println(7,"\% homura search \"Ping Pong The Animation\"");
  println(7,"\% homura --threads 5 search \"Initial D\"");
  fprintf(stdout,"\n");
  println(5,"advanced search");
  println(7,"\% homura search \"Monogatari|Madoka\"");
  println(7,"display results for \"Monogatari\" OR \"Madoka\"");
  println(7,"\% homura search \"\\\"School Days\\\"\"");
  println(7,"search for \"School Days\" but not \"Days School\".");
  fprintf (stdout,"\n");
}

HOMURA_ERRCODE parse_flags (int argc, char **argv) 
{
   int opt;
   int numt;
   while (1) {  
     int option_index = 0;
     static struct option long_options[] = 
     {
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
         options::set_debug_level(1);
         break;
       case 'd':
         options::set_debug_level(2);
         break;
       case 'h':
	     print_usage();
         exit(ERRCODE::SUCCESS);
       case 't':
         numt = atoi(optarg);
         if (numt < 1) {
           errprintf(ERRCODE::FAILED_ARGPARSE,
	         "error:-t,--thread expects a positive integer\n"
             "(recieved %s)\n",optarg);
           return ERRCODE::FAILED_ARGPARSE;
         } 
         options::set_thread_level(numt);
         break;
       case '?':
         errprintf(ERRCODE::FAILED_ARGPARSE,"incorrect option %c\n",optopt);
         errprintf(ERRCODE::FAILED_ARGPARSE,"for usage: homura --help\n");
         return ERRCODE::FAILED_ARGPARSE;
         break;
     }
   }
   return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE execute_command(int argc, char **argv) 
{
 if (optind + 1 > argc) {
   errprintf (ERRCODE::FAILED_ARGPARSE,"No command provided.\n");
   errprintf (ERRCODE::FAILED_ARGPARSE,"for usage: homura --help \n");
   return ERRCODE::FAILED_ARGPARSE;
 }
 options::set_command(std::string(argv[optind]));
 homura_instance homuhomu = homura_instance();
  if (options::command == "search") {
    int search_index = optind + 1;
    if (optind + 1 >= argc) {
      errprintf(ERRCODE::FAILED_ARGPARSE,"Incorrect # of options for search\n"); 
      return ERRCODE::FAILED_ARGPARSE;
    }
    int status; 
    options::set_search_term(std::string(argv[search_index]));
    status = homuhomu.query_nyaasi(options::search_term);
    if (status != ERRCODE::SUCCESS) return status;
    status = homuhomu.crawl();
    if (status != ERRCODE::SUCCESS) return status;
    homuhomu.print_tables();
  }
  else {
    errprintf(ERRCODE::FAILED_INVALID_COMMAND,"Invalid command \"%s\""
    ", use homura --help for all possible options\n",options::command.c_str());
  }
  return ERRCODE::SUCCESS;
}

int main (int argc, char **argv) 
{
  int status;
  status = parse_flags(argc,argv);

  if (status == ERRCODE::SUCCESS) { 
    status = execute_command(argc,argv);
  }

  if (options::debug_level) {
    parse_error_exitcode(status);
  }

  return status;
}
