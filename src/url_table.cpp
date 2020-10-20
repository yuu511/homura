#include "url_table.h"

using namespace homura;

url_table_base::~url_table_base(){}

url_table_base::url_table_base(std::string _website,
                               std::chrono::milliseconds _delay)
: website(_website),
  delay(_delay),
  last_request(std::chrono::steady_clock::now())
{
}

void url_table_base::addURLs(std::string query, std::vector<std::string> newURLs)
{
  if (!newURLs.empty()) {
    remainingURLs.push_back(std::make_pair(query,newURLs));
  }
}

// void url_table_base::addAnticipatedResults(int _expected_results)
// {
//   expected_results = _expected_results; 
// }

bool url_table_base::ready_for_request()
{
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>
  (std::chrono::steady_clock::now() - last_request);
  return diff.count() >= delay.count() ? true : false;
}

bool url_table_base::empty()
{
  return remainingURLs.empty();
}

HOMURA_ERRCODE url_table_base::download_next_URL()
{
  fprintf (stdout,"you should never see this.");
  return {};
}

void url_table_base::addNewResults(std::string query, std::vector<generic_torrent_result> torrents)
{
  last_request = std::chrono::steady_clock::now();
  auto found = results.find(query);
  if (found != results.end()) {
    found->second.insert(found->second.end(),torrents.begin(),torrents.end());    
  }
  else {
    results[query] = torrents;
  }
}

std::chrono::milliseconds url_table_base::get_delay()
{
  return delay;
}

std::string url_table_base::get_website()
{
  return website;
}

// TODO: pwuid_r once this thing is multithreaded
std::string url_table_base::get_cache_basedir()
{
  const char *homedir = getenv("HOME");

  if (!homedir) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  std::filesystem::path basedir = std::filesystem::path(homedir) / "/.homuracache";
  if (!std::filesystem::is_directory(basedir) && !std::filesystem::exists(basedir)) {
    bool made = std::filesystem::create_directory(basedir);
    if (!made) return "";
  }

  if (options::debug_level) {
    fprintf(stderr,"basedir %s\n",basedir.string().c_str());
  }

  return basedir.string();
}

std::string url_table_base::get_cache_fullpath(std::string basedir,std::string query)
{
  std::filesystem::path fullpath = std::filesystem::path(basedir) / ("/" + website + "_" + query);
  return fullpath.string();
}

HOMURA_ERRCODE url_table_base::cache()
{
  std::string cachedir = get_cache_basedir();
  if (cachedir == "") return ERRCODE::FAILED_CACHE_CREATION; 
  for (auto itor : results) {
    std::ofstream cache(get_cache_fullpath(cachedir,itor.first));
    boost::archive::text_oarchive oa(cache);
    oa << itor.second;
  }
  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE url_table_base::decache(std::string query, int expected_results, int results_per_page)
{
  HOMURA_ERRCODE Status = ERRCODE::SUCCESS;

  if (options::force_refresh_cache) return Status;

  std::string cachedir = get_cache_basedir();
  // assume we always download the first page.
  // for (auto itor : remainingURLs) {
  //   if (itor.first == query) {
  //     
  //   }
  // }

  return Status;
}

void url_table_base::print()
{
  for (auto queries : results) {

    if (options::print.test(1)) {
      fprintf (stdout, "=== Searchterm %s ===\n\n\n", queries.first.c_str());
    }

    for (auto entry : queries.second) {
      if (options::print.test(1)) {
        fprintf(stdout,"\n%s\n\n",entry.name.c_str());
      }
      if (options::print.test(0)) {
        fprintf(stdout,"%s\n",entry.magnet.c_str());
      }
    }

    if (options::print.test(1)) {
      fprintf (stdout, "\n\n\n=====================\n\n\n");
    }
  }
}

