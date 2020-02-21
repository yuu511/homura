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
  printopt(5,"[-d,--debug]"," : more extensive logging, prints out full html files");
  printopt(5,"[-c,--refresh_cache]"," : Force homura to not use cache.");
  printopt(5,"[-r,--regex] REGEX"," : Filter results by regular expression [REGEX]");
  printopt(5,"[-p,--print_options] MASK"," : [MASK] is hex or decimal. Print magnets(bit 0),and/or titles(bit 1)");
  printopt(5,"","   e.g. 0x3 to print both, 0x2 for torrent titles only (default 0x1)");
  printopt(5,"[-n,--num_pages] NUMBER"," : load up to [NUMBER] pages ");
  printopt(5,"","   [NUMBER] is a positive number in decimal only");
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
  println(7,"\% homura -p 0x2 search \"Initial D\" // will print all titles of torrents matching query Initial D");
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

HOMURA_ERRCODE parse_flags (int argc, char **argv) 
{
   int opt;
   long p_opts;
   int n_opts;
   std::smatch sm;
   std::regex ishex("0x\\d+");
   std::regex validzero("(0x)?0+");
   std::string cast;
   while (1) {  
     int option_index = 0;
     static struct option long_options[] = 
     {
       { "verbose" , no_argument       ,  0   , 'v' },
       { "debug"   , no_argument       ,  0   , 'd' },
       { "help"    , no_argument       ,  0   , 'h' },
       { "refresh_cache" , no_argument ,  0   , 'c' },
       { "reverse_results" , no_argument ,  0   , 'b' },
       { "regex" , required_argument ,  0   , 'r' },
       { "print_options" , required_argument ,  0   , 'p' },
       { "num_pages" , required_argument ,  0   , 'n' },
       {  NULL     , 0                 , NULL ,  0  }
     };
     opt = getopt_long(argc,argv, "cvdbr:p:n:",
                 long_options, &option_index);
     if (opt == -1)		 
       break;
     switch (opt) {
       case 'v':
         options::debug_level = 1;
         break;
       case 'd':
         options::debug_level = 2;
         break;
       case 'h':
	     print_usage();
         exit(ERRCODE::SUCCESS);
       case 'r':
         options::regex = std::string(optarg);
         break;
       case 'c':
         options::force_refresh_cache = 1;
         break;
       case 'b':
         options::reverse_results = 1;
         break;
       case 'p':
         cast = optarg;
         if (std::regex_match(cast,sm,validzero)) {
           options::print.reset(); 
           break; 
         }
         p_opts = std::stoul(cast,NULL,std::regex_match(cast,sm,ishex) ? 16 : 10);
         if (p_opts <= 0 || p_opts > 3) {
           errprintf(ERRCODE::FAILED_ARGPARSE, "incorrect number for option -p,--print_options\n"
                     "(accepts either hex or decimal, values [0-3] only, provided arg %s\n",optarg);
           return ERRCODE::FAILED_ARGPARSE;
         }
         options::print.set(0,p_opts & 0x1);
         options::print.set(1,(p_opts >> 1) & 0x1);
         if (options::debug_level) {
           fprintf (stderr,"print settings set to %s\n",options::print.to_string().c_str());
         }
         break;
       case 'n':
         cast = optarg;
         n_opts = std::stoi(cast,NULL,10);
         if (p_opts <=0) { 
           errprintf(ERRCODE::FAILED_ARGPARSE, "incorrect number for option -n --num_pages\n"
                     "(accepts a -positive- decimal number) %s\n",optarg);
           return ERRCODE::FAILED_ARGPARSE;
         }
         if (options::debug_level) {
           fprintf (stderr,"print settings set to %s\n",options::print.to_string().c_str());
         }
         options::number_pages = n_opts;
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
 options::command = std::string(argv[optind]);
 homura_instance homuhomu = homura_instance();
  if (options::command == "search") {
    int search_index = optind + 1;
    if (optind + 1 >= argc) {
      errprintf(ERRCODE::FAILED_ARGPARSE,"Incorrect # of options for search\n"); 
      return ERRCODE::FAILED_ARGPARSE;
    }
    int status; 
    options::search_term = std::string(argv[search_index]);
    status = homuhomu.query_nyaasi(options::search_term);
    if (status != ERRCODE::SUCCESS) return status;
    status = homuhomu.crawl();
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
