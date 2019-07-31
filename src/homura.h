#ifndef __HOMURA_H_
#define __HOMURA_H_

#include <curl/curl.h>
#include <string>

void query_packages(std::string args, int LOG_LEVEL, int threadcount);

#endif
