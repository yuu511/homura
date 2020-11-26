#include <iterator>
#include <utility>
#include <algorithm>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include <string.h>
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
    ref_page("https://nyaa.si/?f=0&c=0_0&q="),
    URLs(std::deque<std::string>())
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
  for (int i = 2; i <= num_pages; ++i) {
    URLs.emplace_back(ref_page + "&p=" + std::to_string(i)) ;  
    if (options::verbose_mode) {
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

inline std::uint64_t convertToNumber(const char *str) 
{
  if (!str) return 0;
  double base = 0; 
  std::uint64_t ret = 0;
  std::uint64_t factor = 1;

  char *cpy = strdup(str);
  char *token = std::strtok(cpy," ");
  if (token) {
    base = boost::lexical_cast<double>(token);

    token = std::strtok(NULL," "); 

    while ( std::floor(base) != base ) {
      base *= 10;
      factor *= 10;

      DBG("base %lf\n",base);
    }

    if (token) {
      if (strcmp(token,"KiB") == 0 ) {
        ret = (std::uint64_t) base * std::pow(2,10);
      }
      else if (strcmp(token,"MiB") == 0 ) {
        ret = (std::uint64_t) base * std::pow(2,20);
      }
      else if (strcmp(token,"GiB") == 0 ) {
        ret = (std::uint64_t) base * std::pow(2,30);
      }
      else if (strcmp(token,"TiB") == 0 ) {
        ret = (std::uint64_t) base * std::pow(2,40);
      }
      else {
        ret = (std::uint64_t) base;
      }
      free(cpy);
      ret /= factor;
    }
    else {
      free(cpy);
      return (std::uint64_t) base;
    }
  }

  DBG("size in bytes 0x%lx\n",ret);

  return ret;
}

// pre : must have already called html_parser.create_tree
inline HOMURA_ERRCODE nyaasi_extractor::getTorrents(std::string URL, 
                                                    std::vector<generic_torrent_result> &result) 
{
  auto tree = html_parser.get_tree();

  const char *ttype = NULL;
  const char *title = NULL;
  const char *magnet = NULL;
  const char *sizestr = NULL;
  const char *datestr = NULL;
  std::uint64_t size = 0;
  
  const char *tagname = "tr";
  myhtml_collection_t *table = myhtml_get_nodes_by_name(tree,NULL,tagname,strlen(tagname),NULL);
  
  if (table && table->length > 1) {
    const char *td = "td";
    const char *ftype = "title";
    const char *title_attr = "colspan";
    const char *title_val = "2";
    const char *mag_k = "href";
    const char *mag_v = "magnet";

    for (size_t i = 1; i < table->length; ++i){
       std::string combinedname;
       myhtml_collection_t *td_table = 
         myhtml_get_nodes_by_name_in_scope(tree, NULL,
                                           table->list[i], td, strlen(td),
                                           NULL);

       if (td_table && td_table->length > 4 && td_table->list) {
       // NAME
         // FILETYPE (subbed,raw,etc)
        myhtml_collection_t *torrent_type = 
          myhtml_get_nodes_by_attribute_key(tree, NULL,
                                            td_table->list[0], ftype, strlen(ftype),
                                            NULL);

        if (torrent_type && torrent_type->length && torrent_type->list) {
           myhtml_tree_attr_t *attr = myhtml_node_attribute_first(torrent_type->list[0]);
           if (attr) {
             attr = myhtml_attribute_next(attr);
             if (attr)
               ttype = myhtml_attribute_value(attr, NULL);
           }
           combinedname.append("(");
           combinedname.append(ttype);
           combinedname.append(") ");
        }
        myhtml_collection_destroy(torrent_type);

        // TITLE
        myhtml_collection_t *title_table = 
        myhtml_get_nodes_by_attribute_value(tree, NULL,
                                            td_table->list[1], true,
                                            title_attr, strlen(title_attr),
                                            title_val, strlen(title_val),
                                            NULL);
        if (title_table && title_table->length && title_table->list) {
          myhtml_tree_node_t *child = myhtml_node_last_child(title_table->list[0]);
          child = myhtml_node_prev(child);
          if (child) {
              child = myhtml_node_child(child);
              title = myhtml_node_text(child,NULL);
              if (title) {
                combinedname.append(title);
                DBG("title %s \n",title);
              }
          }
        }
        myhtml_collection_destroy(title_table);

        // TORRENT (MAGNET)
        myhtml_collection_t *magnets = 
        myhtml_get_nodes_by_attribute_value_contain(tree, NULL, 
                                                    td_table->list[2], true,
                                                    mag_k, strlen(mag_k),
                                                    mag_v, strlen(mag_v), 
                                                    NULL);
        
         if(magnets && magnets->list && magnets->length) {
           for (size_t i = 0; i < magnets->length; i++){
             myhtml_tree_attr_t *attr = myhtml_node_attribute_first(magnets->list[i]);
             magnet = myhtml_attribute_value(attr,NULL);
             #ifdef DEBUG
             if (magnet) {
               DBG("magnet : %s \n\n",magnet);
             }
             #endif
           }
         }
         myhtml_collection_destroy(magnets);


         // SIZE
         myhtml_tree_node_t *child = myhtml_node_child(td_table->list[3]);
         if (child) {
           sizestr = myhtml_node_text(child,NULL);   
         }
         #ifdef DEBUG
         if (sizestr) { 
           DBG("sizestr %s \n",sizestr);
         }
         #endif

         // DATE
         child = myhtml_node_child(td_table->list[4]);
         if (child) {
           datestr = myhtml_node_text(child,NULL);   
         }
         #ifdef DEBUG
         if (datestr) {
           DBG("datestr %s \n",datestr);
         }
         #endif
       }

       if (sizestr) {
         size = convertToNumber(sizestr);
       }
       else {
         sizestr = "";
       }

       myhtml_collection_destroy(td_table);
  
       if (!magnet || !title || !sizestr || !datestr) {
         fprintf(stderr,"No torrent found at index %zu \n",i);    
         continue;
       }

       result.push_back({combinedname, magnet, sizestr, size, datestr, URL});
     }
   }
   myhtml_collection_destroy(table);
   return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE nyaasi_extractor::downloadPage(std::string URL, 
                                              std::vector <generic_torrent_result> &results)
{
  HOMURA_ERRCODE status;
  const char *rawHTML = curlHTML(URL);

  if (!rawHTML) return ERRCODE::FAILED_CURL;

  html_parser.reset_tree();
  status = html_parser.create_tree(rawHTML);
  if ( status != ERRCODE::SUCCESS ) return ERRCODE::FAILED_PARSE;

  return getTorrents(URL,results);
}

/* nyaa.si has no official api, and we must manually
   find out how many results to expect by sending a request 
   and parsing the query result information */
HOMURA_ERRCODE nyaasi_extractor::downloadFirstPage(std::string searchtag,
                                                   std::vector <generic_torrent_result> &results)
{
  HOMURA_ERRCODE Status;
  std::replace(searchtag.begin(), searchtag.end(), ' ', '+');
  ref_page = ref_page + searchtag;

  const char *rawHTML = curlHTML(ref_page);

  if (!rawHTML) return ERRCODE::FAILED_CURL;

  html_parser.reset_tree();
  Status = html_parser.create_tree(rawHTML);
  if ( Status != ERRCODE::SUCCESS ) return Status;
  
  Status = parseMetadata();

  if (Status != ERRCODE::SUCCESS) return Status;

  return getTorrents(ref_page,results);
}

std::deque<std::string> nyaasi_extractor::getURLs()
{
  return URLs;
}

int nyaasi_extractor::getExpectedResults() 
{ 
  return pageinfo.total_result;
}

int nyaasi_extractor::getResultsPerPage() 
{ 
  return 75;
}
