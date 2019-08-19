#include "url_table.h"

using namespace homura;

url_request(const std::string &url, int website, std::chrono::seconds delay) 
  : url(url),
    website(website),
    delay(delay) {}
 
const std::string get_url { return url; }

int get_website { return website; }

std::chrono::seconds get_delay { return delay; }
