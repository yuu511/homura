#include <csignal>
#include <stdio.h>
#include <thread>
#include <chrono>

#include "homura.h"
#include "errlib.h"

using namespace homura;

void sigterm_handler(int signal)
{
  switch (signal) {  
    case SIGTERM:
      while (homura::error_handler::cache) ; // stall for cache
      exit(homura::error_handler::exit_code);
      break;
    case SIGHUP:
      // TODO
      break;
    default:
      break;
  }
}

int main ()
{
  std::signal(SIGTERM,sigterm_handler);
  std::signal(SIGHUP,sigterm_handler);

  homura_instance homuhomu = homura_instance();
  for(;;) {
    printf("start!\n"); 
    std::this_thread::sleep_for(std::chrono::minutes(1));
  }

}
