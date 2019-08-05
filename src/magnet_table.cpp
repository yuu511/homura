#include "magnet_table.h"

name_magnet::name_magnet(){
  name = nullptr;
  magnet = nullptr;
}

name_magnet::~name_magnet(){
  delete name;
  delete magnet;
}
