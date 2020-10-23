#include "url_table.h"

using namespace homura;

url_table_base::~url_table_base(){}

url_table_base::url_table_base(std::string _website,
                               std::chrono::milliseconds _delay)
: website(_website),
  delay(_delay),
  last_request(std::chrono::steady_clock::now()),
  cache_done(false)
{
}

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

std::filesystem::path get_basedir()
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

// TODO: pwuid_r once this thing is multithreaded
std::filesystem::path url_table_base::get_cache_dir()
{
  std::filesystem::path cachedir = get_basedir();
  if (cachedir.empty()) return std::filesystem::path();

  cachedir /= "homuracache";
  if (options::debug_level > 1) {
    fprintf(stderr,"cachedir %s\n",cachedir.string().c_str());
  }

  if (!std::filesystem::is_directory(cachedir) && !std::filesystem::exists(cachedir)) {
    bool made = std::filesystem::create_directory(cachedir);
    if (!made) return std::filesystem::path();
  }

  return cachedir;
}

std::filesystem::path url_table_base::generate_cache_fullpath(std::filesystem::path cachedir,std::string query)
{
  return cachedir /= (website + "_" + query);
}

HOMURA_ERRCODE url_table_base::cache()
{
  std::filesystem::path cachedir = get_cache_dir();
  if (cachedir.empty()) return ERRCODE::FAILED_CACHE_CREATION; 
  for (auto &itor : results) {
    std::ofstream cache(generate_cache_fullpath(cachedir,itor.first));
    boost::archive::text_oarchive oa(cache);
    oa << itor.second;
  }
  return ERRCODE::SUCCESS;
}

void url_table_base::addURLs_and_decache(std::string query,
                                         std::deque<std::string> newURLs, 
                                         size_t expected_results, size_t results_per_page)
{
  std::filesystem::path cachedir = get_cache_dir();
  if (cachedir.empty() || options::force_refresh_cache || results_per_page == 0 || options::number_pages) {
    if (!newURLs.empty()) {
      remainingURLs.push_back(std::make_pair(query,newURLs));
    }
    return;
  }

  std::filesystem::path cachepath = generate_cache_fullpath(cachedir,query);

  if ( std::filesystem::exists (cachepath) && 
       std::filesystem::is_regular_file (cachepath) ) {

    std::vector<generic_torrent_result> cachedresults;
    std::ifstream cachefile(cachepath);
    boost::archive::text_iarchive ia(cachefile);
    ia >> cachedresults;

    if (cachedresults.size() == expected_results) {
      fprintf(stderr,"Amount of expected results same as last time, "
                     "using cached results for \"%s\"\n",query.c_str());
      results[query] = cachedresults;
      newURLs = std::deque<std::string>(); // clear
    }
    else if (cachedresults.size() < expected_results) {
      if (expected_results > results_per_page) {
        size_t results_to_decache = expected_results - results_per_page; // always download the first page
        if (results_to_decache > cachedresults.size()) results_to_decache = cachedresults.size();
        size_t URLs_to_delete = (results_to_decache + (results_per_page -1)) / results_per_page; 
        if (URLs_to_delete) {
          if (options::debug_level > 1) {
            fprintf(stderr,"results to cache %zu urls to delete %zu\n"
                     "sizeof decached results %zu results_per_age %zu\n", 
              results_to_decache, URLs_to_delete, cachedresults.size(),results_per_page);
          }
          while (URLs_to_delete-- && (!newURLs.empty())) {
            if (options::debug_level) {
              fprintf(stderr,"using cached results for page %s\n", newURLs.back().c_str());
            }
            newURLs.pop_back();       
          }
          cachedresults.erase(cachedresults.begin(),cachedresults.end() -results_to_decache); 
          cached_results[query] = cachedresults;
        }
      }
    }
  }
  else {
    if (options::debug_level) {
      fprintf(stderr,"cachefile %s not found\n",cachepath.c_str());
    }
  }

  if (!newURLs.empty()) {
    remainingURLs.push_back(std::make_pair(query,newURLs));
  }

}

void url_table_base::decache()
{
  if (options::debug_level) {
    fprintf(stderr,"decaching \n");
  }
  for (auto &itor : results) {
    auto find = cached_results.find(itor.first);
    if (find != cached_results.end()) {
      if (options::debug_level) {
        fprintf(stderr,"sizeof noncached %zu cached %zu \n",itor.second.size(),find->second.size());
      }
      itor.second.insert(itor.second.end(),find->second.begin(),find->second.end());
    }
  }
}

void url_table_base::do_caching_operations()
{
  if (!cache_done || (options::number_pages != 0)) {
    decache();
    cache();
    cache_done = true;
  }
}

void url_table_base::print()
{
  for (auto &queries : results) {

    if (options::print.test(1)) {
      fprintf (stdout, "=== Searchterm %s ===\n\n\n", queries.first.c_str());
    }

    for (auto &entry : queries.second) {
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

