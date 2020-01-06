#include "nyaasi_extractor.h"
#include <climits>

using namespace homura;

pagination_information::pagination_information(int first_, 
                                               int last_,
                                               int total_) 
{
  first_result = first_;
  last_result = last_;
  total_result = total_;
}

nyaasi_extractor::nyaasi_extractor() 
  : curler(std::make_shared<curl_container>()), 
    html_parser(tree_container()),
    pageinfo(pagination_information(0,0,0))
{}


HOMURA_ERRCODE nyaasi_extractor::curl_and_create_tree(std::string url)
{
  int status;
  status = curler->perform_curl(url);
  if (status != ERRCODE::SUCCESS) return status; 

  html_parser.reset_tree();
  status = html_parser.create_tree(curler->get_HTML_aschar());
  if (status != ERRCODE::SUCCESS) return status; 

  return ERRCODE::SUCCESS;
}

HOMURA_ERRCODE nyaasi_extractor::extract_pageinfo() 
{
  auto tree = html_parser.get_tree();
  if (!tree) { 
    errprintf(ERRCODE::FAILED_MYHTML_TREE_INIT, "No tree detected in"
      "get_pagination_information\n");
    return ERRCODE::FAILED_MYHTML_TREE_INIT;
  }

  const char *page_information;
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
    if (myhtml_collection_destroy(found)) {
      errprintf(ERRCODE::FAILED_FREE, "Failed to free MyHTML collection.");
      return ERRCODE::FAILED_FREE;
    }
  } 
  else {
    errprintf(ERRCODE::FAILED_PARSE, "Failed to parse first page (Pagination information not found)\n");
    return ERRCODE::FAILED_PARSE;
  }

  // parse the pagination information
  std::vector<int> stk;
  // copy so we can modify the string
  char *copy = strdup(page_information);
  char *endptr = copy;
  while (*endptr) {
    if (isdigit(*endptr)) {
       long int parse = strtol(endptr,&endptr,10);
       if (parse <= INT_MAX) {
         stk.push_back((int) parse);
       } 
       else {
         errprintf(ERRCODE::FAILED_INTCAST,"Failed to convert long to int\n");
         return ERRCODE::FAILED_INTCAST;
       }
    }
    else 
      ++endptr;
  }
  free(copy);
  if (stk.size() != 3) {
    errprintf(ERRCODE::FAILED_PARSE,"Incorrect pagination string parse.\n");
    return ERRCODE::FAILED_PARSE;
  }

  pageinfo = pagination_information(stk[0],stk[1],stk[2]);

  return ERRCODE::SUCCESS;
}

name_magnet nyaasi_extractor::extract_tree_magnets() 
{
  name_magnet magnet_and_name;
  html_parser.ptree(html_parser.get_tree());
  // const char *mag_k = "href";
  // const char *mag_v = "magnet";
  // myhtml_collection_t *magnets = 
  // myhtml_get_nodes_by_attribute_value_contain(html_parser.get_tree(), NULL, NULL, true,
  //                                             mag_k, strlen(mag_k),
  //                                             mag_v, strlen(mag_v), NULL);
  // 
  // if(magnets && magnets->list && magnets->length) {
  //   for (size_t i = 0; i < magnets->length; i++){
  //     myhtml_tree_attr_t *attr = myhtml_node_attribute_first(magnets->list[i]);
  //     const char *magnet_link = myhtml_attribute_value(attr,NULL);
  //     if (magnet_link) {
  //       magnet_list.push_back(magnet_link);
  //     }
  //   }
  // }
  // myhtml_collection_destroy(magnets);
  return magnet_and_name;
}

std::vector<std::string> nyaasi_extractor::populate_url_list(std::string page)
{
  /* nyaa.si has no official api, and we must manually
     find out how many results to expect by sending a request 
     and parsing the query result information */

  int status;
  std::vector<std::string>urls;
  status = curl_and_create_tree(page);
  if (status != ERRCODE::SUCCESS) return urls; 
  status = extract_pageinfo();
  if (status != ERRCODE::SUCCESS) return urls; 

  int total = pageinfo.total_result;
  int per_page = pageinfo.last_result;
  if ( total <= 1 || per_page <= 1) {
    errprintf(ERRCODE::FAILED_NO_RESULTS,"no results found for %s!\n",page.c_str());
    return urls;
  }
  int num_pages = ( total + (per_page - 1) ) / per_page;
  for (int i = 2; i <= num_pages; i++) {
    urls.emplace_back(page + "&p=" + std::to_string(i)) ;  
  }
  return urls;
}

name_magnet nyaasi_extractor::get_magnets(std::string url)
{
  int status = curl_and_create_tree(url);
  return status != ERRCODE::SUCCESS ? name_magnet() : extract_tree_magnets();
}

name_magnet nyaasi_extractor::parse_first_page()
{
  return extract_tree_magnets();
}
