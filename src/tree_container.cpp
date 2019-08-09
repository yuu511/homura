#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "tree_container.h"
#include "errlib.h"

using namespace homura;

pagination_information::pagination_information(int first, int last, int total){
  first_result = first;
  last_result = last;
  total_result = total;
}

tree_container::tree_container(int threads)
{
  handle = myhtml_create();
  myhtml_init(handle, MyHTML_OPTIONS_DEFAULT, threads, 0);

  tree = myhtml_tree_create();
  myhtml_tree_init(tree, handle);

  pageinfo = nullptr;
}

bool tree_container::tree_parseHTML(const char *html_page)
{
  return MyHTML_STATUS_OK == myhtml_parse(tree, MyENCODING_UTF_8, html_page,strlen(html_page));
}

tree_container::~tree_container()
{
  if (tree)
    myhtml_tree_destroy(tree);
  if (handle)
    myhtml_destroy(handle);
  delete pageinfo; 
}

bool tree_container::parse_pagination_information(){
  if (!tree || !handle)
  { 
    errprintf(ERRCODE::FAILED_MYHTML_TREE_INIT, "No tree or handle detected in"
      "get_pagination_information\n");
    return false;
  }

  const char *page_information;
  myhtml_collection_t *found = 
    myhtml_get_nodes_by_attribute_value(tree,NULL,NULL,true,"class",5,"pagination-page-info",20,NULL);
  if (found && found->list && found->length) 
  {
    myhtml_tree_node_t *node = found->list[0];
    node = myhtml_node_child(node);
    myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);
    if (tag_id == MyHTML_TAG__TEXT || tag_id == MyHTML_TAG__COMMENT)
    {
      page_information = myhtml_node_text(node,NULL);
      // if (debug_level) 
      // {
      //   fprintf (stdout,"== First page pagination Information: ==\n%s\n\n",page_information);
      // }
    } 
    else 
    {
      errprintf(ERRCODE::FAILED_FIRST_PARSE, "Failed to parse first page \n (Pagination information not found)");
      return false;
    }
    if (myhtml_collection_destroy(found)) 
    {
      errprintf(ERRCODE::FAILED_FREE, "Failed to free MyHTML collection.");
      return false;
    }
  } 
  else 
  {
    errprintf(ERRCODE::FAILED_FIRST_PARSE, "Failed to parse first page (Pagination information not found)\n");
    return false;
  }

  // parse the pagination information
  std::vector<int> stk;
  // copy so we can modify the string
  char *copy = strdup(page_information);
  char *endptr = copy;
  while (*endptr) 
  {
    if (isdigit(*endptr)) 
    {
       long int parse = strtol(endptr,&endptr,10);
       if (parse <= INT_MAX) 
       {
         stk.push_back((int) parse);
       } 
       else 
       {
         errprintf(ERRCODE::FAILED_INTCAST,"Failed to convert long to int\n");
         return false;
       }
    }
    else 
      ++endptr;
  }
  free(copy);
  if (stk.size() != 3)
  {
    errprintf(ERRCODE::FAILED_FIRST_PARSE,"Incorrect pagination string parse.\n");
    return false;
  }
  pageinfo = new pagination_information(stk[0],stk[1],stk[2]);
  return true;
}

int tree_container::pageinfo_first_result()
{
  if (pageinfo)
    return pageinfo->first_result;
  else 
    fprintf(stdout,"WARNING: Pagination information does not exist.\n");  
  return 0;
}

int tree_container::pageinfo_last_result()
{
  if (pageinfo)
    return pageinfo->last_result;
  else 
    fprintf(stdout,"WARNING: Pagination information does not exist.\n");  
  return 0;
}

int tree_container::pageinfo_total_result()
{
  if (pageinfo)
    return pageinfo->total_result;
  else 
    fprintf(stdout,"WARNING: Pagination information does not exist.\n");  
  return 0;
}