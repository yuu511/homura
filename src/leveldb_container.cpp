#include <filesystem>
#include "leveldb_container.h"

using namespace homura;


std::filesystem::path get_homura_basedir()
{
  const char *homedir = getenv("HOME");

  if (!homedir) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  if (options::debug_level > 1) {
    fprintf(stderr,"homedir %s\n",homedir);
  }

  std::filesystem::path basedir = std::filesystem::path(homedir);
  basedir /= ".homura";
  if (options::debug_level > 1) {
    fprintf(stderr,"basedir %s\n",basedir.string().c_str());
  }
  if (!std::filesystem::is_directory(basedir) && !std::filesystem::exists(basedir)) {
    bool made = std::filesystem::create_directory(basedir);
    if (!made) return std::filesystem::path();
  }

  return basedir;
}

std::filesystem::path get_db_dir()
{
  std::filesystem::path cachedir = get_basedir();
  if (cachedir.empty()) return std::filesystem::path();

  cachedir /= "homuradb";
  if (options::debug_level > 1) {
    fprintf(stderr,"cachedir %s\n",cachedir.string().c_str());
  }

  if (!std::filesystem::is_directory(cachedir) && !std::filesystem::exists(cachedir)) {
    bool made = std::filesystem::create_directory(cachedir);
    if (!made) return std::filesystem::path();
  }

  return cachedir;
}

leveldb::leveldb_container() 
{
  db = nullptr;
  options.create_if_missing = true;
  
  std::filesystem::path dbpath = get_db_dir();
  leveldb::Status status = leveldb::DB::open(options, (dbpath /= "db").string()),


}

leveldb::~leveldb_container() 
{
  if (db)
    delete db;  
}
