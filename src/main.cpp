#include <iostream>
#include <string>
#include <unistd.h>
#include <libgen.h>
#include <homura.h>

void parseargs (int argc, char** argv){
   int opt;
   while ((opt = getopt(argc,argv,"v")) != -1 ){  
     switch (opt){ 
       case 'v':
         ;
       case '?':
       std::cerr << "bad option: " << optopt << std::endl;
       break;
     }
   }
   if (optind > argc){
     std::cerr << "usage: " << basename (argv[0]) << " [-v] " << std::endl;
     exit(1);
   }
}

int main (int argc, char ** argv){
  parseargs(argc,argv);
  return 0;
}
