#ifndef __MAGNET_TABLE_H_
#define __MAGNET_TABLE_H_

#include <vector>
#include <string>

namespace homura 
{
    /* container for torrent title, magnet link */
    class name_magnet {
      public:
        name_magnet();
        ~name_magnet();
      private:
        const std::string *name;  
        const std::string *magnet;
    };
    
    using magnet_table = std::vector<name_magnet*>;
    void free_magnet_table(magnet_table *names);
}


#endif
