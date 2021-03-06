#include <string>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#include "homura.h"
#include "errlib.h"

using namespace homura;

/* print_usage helper functions */
// n space indent for all strings
// first part of option string will be a minimum of 30 characters long 
void println(int whitespace, const char *line) 
{
  fprintf(stderr,"%*s%s\n",whitespace,"", line);
}

void printopt(int whitespace, const char *option, const char *description) 
{
  fprintf(stderr,"%*s%-30s%s\n",whitespace, "", option, description);
}

/* print_usage */ 
void print_usage() 
{
  fprintf(stderr,"\n");
  println(5,"USAGE:");
  println(5,"homura [FLAGS] [OPTION]");
  println(7,"Refer to flags / options sections");
  fprintf(stderr,"\n");
  println(5,"homura --help");
  println(7,"print out usage message");
  fprintf(stderr,"\n");
  println(5,"FLAGS:");
  printopt(5,"[-v,--verbose]"," : logging, prints out actions as they are preformed");
  printopt(5,"[-c,--refresh_cache]"," : Force homura to not use cache.");
  printopt(5,"[-t,--torrents_only] "," : Print magnets only");
  printopt(5,"","   e.g. 0x3 to print both, 0x2 for torrent titles only (default 0x1)");
  printopt(5,"[-p,--num_pages] NUMBER"," : load up to [NUMBER] pages ");
  printopt(5,"","   [NUMBER] is a positive number in decimal only");
  printopt(5,"[-w,--wait_end]"," : respect request delay at end of torrent downloading");
  printopt(5,"[-s,--size]"," : sort and print results by size.");
  printopt(5,"[--help]"," : print out usage message");
  fprintf (stderr,"\n");
  println(5,"OPTIONS:");
  printopt(5,"search [TARGET]"," : query results for TARGET and print to stdout");
  fprintf (stderr,"\n");
  println(5,"EXAMPLES:");
  fprintf (stderr,"\n");
  println(5,"[search]");
  println(5,"nyaa.si : all site-defined advanced search options should work. ( \"\",|,(),- )");
  println(5,"for more information about advanced search options : https://nyaa.si/help");
  fprintf(stderr,"\n");
  println(5,"simple search");
  println(7,"\% homura search \"Ping Pong The Animation\"");
  println(7,"\% homura -t search \"Initial D\" // will print all torrents from query \"Initial D\"");
  println(7,"\% homura --regex \"\\[HorribleSubs\\].*1080p.*\" search \"Ishuzoku\"");
  println(9,"example match : [HorribleSubs] Ishuzoku Reviewers - 02 [1080p].mkv");
  fprintf(stderr,"\n");
  println(5,"advanced search");
  println(7,"\% homura search \"Monogatari|Madoka\"");
  println(7,"display results for \"Monogatari\" OR \"Madoka\"");
  println(7,"\% homura search \"\\\"School Days\\\"\"");
  println(7,"search for \"School Days\" but not \"Days School\".");
  fprintf (stderr,"\n");
}

HOMURA_ERRCODE parse_flags (int argc, char **argv, homura_settings &settings) 
{
   int opt;
   int n_opts;
   std::smatch sm;
   std::regex ishex("0x\\d+");
   std::regex validzero("(0x)?0+");
   std::string cast;
   (void) settings;

   while (1) {  
     int option_index = 0;
     static struct option long_options[] = 
     {
       { "verbose" , no_argument       ,  0    , 'v' },
       { "help"    , no_argument       ,  0    , 'h' },
       { "refresh_cache" , no_argument ,  0    , 'c' },
       { "torrents_only" , no_argument ,  0    , 't' },
       { "num_pages" , required_argument ,  0  , 'p' },
       { "delay_end" , no_argument ,  0        , 'w' },
       { "size"      , no_argument ,  0        , 's' },
       {  NULL     , 0                 , NULL  ,  0  }
     };
     opt = getopt_long(argc,argv, "vhctp:ws",
                 long_options, &option_index);
     if (opt == -1)		 
       break;
     switch (opt) {
       case 'v':
         settings.verbose_mode = true;
         break;
       case 'h':
	     print_usage();
         exit(ERRCODE::SUCCESS);
       case 'c':
         settings.force_refresh_cache = true;
         break;
       case 't':
         settings.print_opts.set(0,1);
         settings.print_opts.set(1,0);
         break;
       case 'p':
         cast = optarg;
         n_opts = std::stoi(cast,NULL,10);
         if (n_opts <=0) { 
           errprintf(ERRCODE::FAILED_ARGPARSE, "incorrect number for option -p --num_pages\n"
                     "(accepts a -positive- decimal number) %s\n",optarg);
           return ERRCODE::FAILED_ARGPARSE;
         }
         DBG("print settings set to %s\n",settings.print_opts.to_string().c_str());
         settings.number_pages = n_opts;
         break;
       case 'w':
         settings.wait_end = true;
         break;
       case 's':
         settings.sort_by_size = true;
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

HOMURA_ERRCODE execute_command(int argc, char **argv, homura_settings &settings) 
{
 if (optind + 1 > argc) {
   errprintf (ERRCODE::FAILED_ARGPARSE,"No command provided.\n");
   errprintf (ERRCODE::FAILED_ARGPARSE,"for usage: homura --help \n");
   return ERRCODE::FAILED_ARGPARSE;
 }
 std::string command = std::string(argv[optind]);
 homura_instance homuhomu = homura_instance(settings);
 if (!homuhomu.get_settings()) return ERRCODE::FAILED_ARGPARSE;
 if (command == "search") {
    int search_index = optind + 1;
    if (optind + 1 >= argc) {
      errprintf(ERRCODE::FAILED_ARGPARSE,"Incorrect # of options for search\n"); 
      return ERRCODE::FAILED_ARGPARSE;
    }
    HOMURA_ERRCODE Status; 

    Status = homuhomu.query_nyaasi(std::string(argv[search_index]));
    if (Status != ERRCODE::SUCCESS) return Status;

    Status = homuhomu.crawl();
    if (Status != ERRCODE::SUCCESS) return Status;

    homuhomu.print_tables();
    if (homuhomu.get_settings()->wait_end) {
      homuhomu.wait_at_end();
    }
  }
  else {
    errprintf(ERRCODE::FAILED_INVALID_COMMAND,"Invalid command \"%s\""
    ", use homura --help for all possible options\n",command.c_str());
    return ERRCODE::FAILED_INVALID_COMMAND;
  }
  return ERRCODE::SUCCESS;
}

int main (int argc, char **argv) 
{
  HOMURA_ERRCODE Status;
  homura_settings hsettings = homura_settings();

  Status = parse_flags(argc,argv,hsettings);

  if (Status == ERRCODE::SUCCESS) { 
    Status = execute_command(argc,argv,hsettings);
  }

  if (hsettings.verbose_mode) {
    parse_error_exitcode(Status);
  }

  return Status;
}
