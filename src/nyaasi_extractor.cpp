#include <iterator>
#include <utility>
#include <algorithm>
#include <regex>
#include <boost/lexical_cast.hpp>
#include "nyaasi_extractor.h"

using namespace homura;

pagination_information::pagination_information(int total_, 
                                               int last_,
                                               int first_) 
{
  total_result = total_;
  last_result = last_;
  first_result = first_;
}

nyaasi_extractor::nyaasi_extractor() 
  : curler(std::move(curl_container())), 
    html_parser(std::move(tree_container())),
    pageinfo(pagination_information(0,0,0)),
    URLs({}),
    ref_page("https://nyaa.si/?f=0&c=0_0&q=")
{}

HOMURA_ERRCODE nyaasi_extractor::parseMetadata() 
{
  auto tree = html_parser.get_tree();

  std::string page_information;

  myhtml_collection_t *found = 
    myhtml_get_nodes_by_attribute_value(tree,NULL,NULL,true,"class",5,"pagination-page-info",20,NULL);
  if (found && found->list && found->length) {
    myhtml_tree_node_t *node = found->list[0];
    node = myhtml_node_child(node);
    myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);
    if (tag_id == MyHTML_TAG__TEXT || tag_id == MyHTML_TAG__COMMENT) {
      page_information = myhtml_node_text(node,NULL);
    } 
    else {
      errprintf(ERRCODE::FAILED_PARSE, "Failed to parse first page \n (Pagination information not found)");
      myhtml_collection_destroy(found);
      return ERRCODE::FAILED_PARSE;
    }
  } 
  else {
    errprintf(ERRCODE::FAILED_PARSE, "Failed to parse first page \n (Pagination information not found)");
    return ERRCODE::FAILED_PARSE;
  }
  std::regex pattern("(\\d+)");
  std::string pginfo(page_information);

  std::sregex_iterator itor(pginfo.begin(),pginfo.end(),pattern);
  auto end = std::sregex_iterator();

  if (std::distance(itor,end) != 3) {
    myhtml_collection_destroy(found);
    errprintf(ERRCODE::FAILED_PARSE,"Incorrect pagination string parse.\n");
    return ERRCODE::FAILED_PARSE;
  }

  pageinfo = pagination_information(boost::lexical_cast<int>(itor->str()),
                                    boost::lexical_cast<int>((itor++)->str()),
                                    boost::lexical_cast<int>((itor++)->str()));
    
  myhtml_collection_destroy(found);
  generateURLs();
  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE nyaasi_extractor::generateURLs()
{
  int total = pageinfo.total_result;
  int per_page = pageinfo.last_result;
  if ( total < 1 || per_page < 1) {
    errprintf(ERRCODE::FAILED_NO_RESULTS,"no results found!\n");
    return ERRCODE::FAILED_NO_RESULTS;
  }
  int num_pages = ( total + (per_page - 1) ) / per_page;
  if (options::number_pages > 0 && options::number_pages < num_pages) {
    num_pages = options::number_pages;
  }
  for (int i = num_pages; i >= 2; --i) {
    URLs.emplace_back(ref_page + "&p=" + std::to_string(i)) ;  
    if (options::debug_level) {
      fprintf(stderr,"Adding url %s\n",((ref_page + "&p=" + std::to_string(i)).c_str()));
    }
  }
  return ERRCODE::SUCCESS;
}

const char *nyaasi_extractor::curlHTML(std::string URL) 
{
  HOMURA_ERRCODE status;
  status = curler.perform_curl(URL);
  if (status != ERRCODE::SUCCESS) {
      fprintf(stderr,"Failed CURL!");
    error_handler::set_error_exitcode(status);
    return NULL;
  }
  return curler.get_HTML_aschar();
}

// pre : must have already called html_parser.create_tree
inline std::vector<generic_torrent_result> nyaasi_extractor::getTorrents(std::string URL) 
{
  auto tree = html_parser.get_tree();

  const char *name = NULL;
  const char *magnet = NULL;
  
  const char *tagname = "tr";
  myhtml_collection_t *table = myhtml_get_nodes_by_name(tree,NULL,tagname,strlen(tagname),NULL);
  std::vector<generic_torrent_result> res = {};
  
  if (table && table->length > 1) {
    generic_torrent_result torrent = {"","","","",""};
    const char *title_attr = "colspan";
    const char *title_val = "2";
    for (size_t i = 1; i < table->length; ++i){
      myhtml_collection_t *title_table1 =
      myhtml_get_nodes_by_attribute_value(tree, NULL,
                                          table->list[i], true,
                                          title_attr, strlen(title_attr),
                                          title_val, strlen(title_val),
                                          NULL);
      if (title_table1 && title_table1->length && title_table1->list) {
        myhtml_tree_node_t *child = myhtml_node_last_child(title_table1->list[0]);
        child = myhtml_node_prev(child);
        if (child) {
            child = myhtml_node_child(child);
            name = myhtml_node_text(child,NULL);
            if (options::debug_level > 1) {
              if (name) fprintf(stderr,"name %s \n",name);
            }
        }
      }
      myhtml_collection_destroy(title_table1);
      const char *mag_k = "href";
      const char *mag_v = "magnet";
      myhtml_collection_t *magnets = 
      myhtml_get_nodes_by_attribute_value_contain(tree, NULL, 
                                                  table->list[i], true,
                                                  mag_k, strlen(mag_k),
                                                  mag_v, strlen(mag_v), 
                                                  NULL);
      
       if(magnets && magnets->list && magnets->length) {
         for (size_t i = 0; i < magnets->length; i++){
           myhtml_tree_attr_t *attr = myhtml_node_attribute_first(magnets->list[i]);
           magnet = myhtml_attribute_value(attr,NULL);
           if (options::debug_level > 1) {
             if (magnet) {
               fprintf(stderr,"magnet : %s \n\n",magnet);
             }
           }
         }
       }
       myhtml_collection_destroy(magnets);
  
       if (!magnet || !name){
         fprintf(stderr,"No torrent found at index %zu \n",i);    
         continue;
       }
       torrent.name = name;
       torrent.magnet = magnet;
       torrent.webpage = URL;
       res.push_back(torrent);
     }
   }
   myhtml_collection_destroy(table);
   return res;
}

std::vector<generic_torrent_result> nyaasi_extractor::downloadPage(std::string URL)
{
  HOMURA_ERRCODE status;
  const char *rawHTML = curlHTML(URL);

  html_parser.reset_tree();
  status = html_parser.create_tree(rawHTML);
  if ( status != ERRCODE::SUCCESS ) return {};

  if (!rawHTML) return {};

  return getTorrents(URL);
}

/* nyaa.si has no official api, and we must manually
   find out how many results to expect by sending a request 
   and parsing the query result information */
std::vector<generic_torrent_result> nyaasi_extractor::downloadFirstPage(std::string searchtag)
{
  HOMURA_ERRCODE status;
  std::replace(searchtag.begin(), searchtag.end(), ' ', '+');
  ref_page = ref_page + searchtag;

  const char *rawHTML = curlHTML(ref_page);

  html_parser.reset_tree();
  status = html_parser.create_tree(rawHTML);
  if ( status != ERRCODE::SUCCESS ) return {};

  if (!rawHTML) return {};
  
  parseMetadata();

  return getTorrents(ref_page);
}

std::vector<std::string> nyaasi_extractor::getURLs()
{
  return URLs;
}

int nyaasi_extractor::getExpectedResults() 
{ 
  return pageinfo.total_result;
}
