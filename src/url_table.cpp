#include "url_table.h"

using namespace homura;

url_table_base::~url_table_base(){}

url_table_base::url_table_base(std::string _website,
                               std::chrono::milliseconds _delay)
: website(_website),
  delay(_delay),
  last_request(std::chrono::steady_clock::now()),
  expected_results(0)
{
}

void url_table_base::addURLs(std::string query, std::vector<std::string> newURLs)
{
  remainingURLs.push_back(std::make_pair(query,newURLs));
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
    found->second.insert(found->second.begin(),torrents.begin(),torrents.end());    
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

void url_table_base::cache()
{
}


void url_table_base::decache()
{
  if (!expected_results) return;
}

void url_table_base::print()
{
}

