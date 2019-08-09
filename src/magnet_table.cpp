#include "magnet_table.h"

using namespace homura;

name_magnet::name_magnet()
{
  name = nullptr;
  magnet = nullptr;
}

name_magnet::~name_magnet()
{
  delete name;
  delete magnet;
}
  
void homura::free_magnet_table(magnet_table *names)
{
  if (names)
  {
    for (auto itor : *names)
    {
      delete itor;
    }
    delete names;
  }
}
