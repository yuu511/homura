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
    cached_pages(0)
{}


nyaasi_extractor::nyaasi_extractor(nyaasi_extractor &&lhs)
: curler(std::move(lhs.curler)),
  html_parser(std::move(lhs.html_parser)),
  pageinfo(lhs.pageinfo),
  cached_pages(lhs.cached_pages)
 {}

HOMURA_ERRCODE nyaasi_extractor::curl_and_create_tree(std::string url)
{
  int status;
  status = curler.perform_curl(url);
  if (status != ERRCODE::SUCCESS) return status; 

  if (options::debug_level) {
    fprintf(stderr,"Parsing page %s\n",url.c_str());
  }
  html_parser.reset_tree();
  html_parser.create_tree(curler.get_HTML_aschar());

  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE nyaasi_extractor::extract_pageinfo() 
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
    errprintf(ERRCODE::FAILED_PARSE,"Incorrect pagination string parse.\n");
    return ERRCODE::FAILED_PARSE;
  }

  pageinfo = pagination_information(boost::lexical_cast<int>(itor->str()),
                                    boost::lexical_cast<int>((itor++)->str()),
                                    boost::lexical_cast<int>((itor++)->str()));
    
  return ERRCODE::SUCCESS;
}

void extract_tree_magnets(myhtml_tree_t *tree, torrent_map_entry &name_and_magnet) 
{
  const char *name = NULL;
  const char *magnet = NULL;

  const char *tagname = "tr";
  myhtml_collection_t *table = myhtml_get_nodes_by_name(tree,NULL,tagname,strlen(tagname),NULL);

  if (table && table->length > 1) {
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
       name_and_magnet.push_back(std::make_pair(std::string(name),std::string(magnet)));
     }
  }
  myhtml_collection_destroy(table);
}

std::vector<std::string> nyaasi_extractor::getURLs(std::string searchtag,const char *&firstpage)
{
  /* nyaa.si has no official api, and we must manually
     find out how many results to expect by sending a request 
     and parsing the query result information */
  std::replace(searchtag.begin(), searchtag.end(), ' ', '+');
  const std::string ref_page = "https://nyaa.si/?f=0&c=0_0&q=" + searchtag;

  int status;
  std::vector<std::string>urls;
  status = curl_and_create_tree(ref_page);
  if (status != ERRCODE::SUCCESS) return urls; 
  status = extract_pageinfo();
  if (status != ERRCODE::SUCCESS) return urls; 

  int total = pageinfo.total_result;
  int per_page = pageinfo.last_result;
  if ( total <= 1 || per_page <= 1) {
    errprintf(ERRCODE::FAILED_NO_RESULTS,"no results found for %s!\n",ref_page.c_str());
    return urls;
  }
  int num_pages = ( total + (per_page - 1) ) / per_page;
  for (int i = num_pages; i >= 1; --i) {
    urls.emplace_back(ref_page + "&p=" + std::to_string(i)) ;  
    if (options::debug_level) {
      fprintf(stderr,"Adding url %s\n",((ref_page + "&p=" + std::to_string(i)).c_str()));
    }
  }
  firstpage = curler.get_HTML_aschar();
  return urls;
}

const char *nyaasi_extractor::downloadOne(std::string url) 
{
  int status;
  status = curler.perform_curl(url);
  return (status != ERRCODE::SUCCESS ? "" : curler.get_HTML_aschar());
}

torrent_map_entry nyaasi_extractor::parse_HTML(const char *HTML)
{
  torrent_map_entry nm_map;
  if (HTML[0] == '\0') return nm_map;   
  html_parser.reset_tree();
  if (html_parser.create_tree(HTML) == ERRCODE::SUCCESS) {
    extract_tree_magnets(html_parser.get_tree(),nm_map);
  }
  if (options::debug_level) {
    fprintf(stderr,"Number of magnet entries %zd\n", nm_map.size());
  }
  return nm_map;
}

torrent_map_entry nyaasi_extractor::parse_first_page()
{
  torrent_map_entry nm_map;
  extract_tree_magnets(html_parser.get_tree(),nm_map);
  if (options::debug_level) {
    fprintf(stderr,"Number of magnet entries %zd\n", nm_map.size());
  }
  return nm_map;
}
