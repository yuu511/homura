#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "tree_container.h"
#include "errlib.h"

using namespace homura;

tree_container::tree_container(std::string website_, int threads)
  : website(website_)
{
  handle = myhtml_create();
  myhtml_init(handle, MyHTML_OPTIONS_DEFAULT, threads, 0);

  tree = myhtml_tree_create();
  myhtml_tree_init(tree, handle);
}

HOMURA_ERRCODE tree_container::create_tree(const char *html_page) 
{
  if (myhtml_parse(tree, MyENCODING_UTF_8, html_page,strlen(html_page)) != MyHTML_STATUS_OK){
    return ERRCODE::FAILED_PARSE;
  }
  return ERRCODE::SUCCESS;
}

std::vector<std::string> tree_container::parse_torrents()
{
  std::vector<std::string> a;
  return a;
}

myhtml_t *tree_container::get_handle() 
{
  return handle;
}

myhtml_tree *tree_container::get_tree()
{
  return tree;
}

std::string tree_container::get_website() 
{
  return website;
}

tree_container::~tree_container() 
{
  if (tree)
    myhtml_tree_destroy(tree);
  if (handle)
    myhtml_destroy(handle);
}

pagination_information::pagination_information(int first, 
                                               int last,
                                               int total) 
{
  first_result = first;
  last_result = last;
  total_result = total;
}



nyaasi_tree::nyaasi_tree(int threads) 
  : tree_container("nyaa.si",threads),
    nyaasi_pageinfo(pagination_information(0,0,0))
{}

nyaasi_tree::~nyaasi_tree(){}

HOMURA_ERRCODE nyaasi_tree::nyaasi_extract_pageinfo() 
{
  auto tree = get_tree();
  auto handle = get_handle();

  if (!tree || !handle) { 
    errprintf(ERRCODE::FAILED_MYHTML_TREE_INIT, "No tree or handle detected in"
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

  nyaasi_pageinfo = pagination_information(stk[0],stk[1],stk[2]);

  if (homura::options::debug_level) {
    fprintf (stdout,"== First page result information == \n");
    fprintf (stdout,"String: \"%s\" \nfirst result "
                    "%d\nlast result (results per page) %d\ntotal results %d\n\n",
            page_information,
            pageinfo_first(),
            per_page(),
            pages_total());
  }

  return ERRCODE::SUCCESS;
}

void print_node_attr(myhtml_tree_node_t *node)
{
    myhtml_tree_attr_t *attr = myhtml_node_attribute_first(node);
    
    while (attr) {
        const char *name = myhtml_attribute_key(attr, NULL);
        
        if(name) {
            printf(" %s", name);
            
            const char *value = myhtml_attribute_value(attr, NULL);
            
            if(value)
                printf("=\"%s\"", value);
        }
        
        attr = myhtml_attribute_next(attr);
    }
}

void print_tree(myhtml_tree_t* tree, myhtml_tree_node_t *node, size_t inc)
{
    while (node)
    {
        for(size_t i = 0; i < inc; i++)
            printf("\t");
        
        // print current element
        const char *tag_name = myhtml_tag_name_by_id(tree, myhtml_node_tag_id(node), NULL);
        
        if(tag_name)
            printf("<%s", tag_name);
        else
            // it can not be
            printf("<!something is wrong!");
        
        // print node attributes
        print_node_attr(node);
        
        if(myhtml_node_is_close_self(node))
            printf(" /");
        
        myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);
        
        if(tag_id == MyHTML_TAG__TEXT || tag_id == MyHTML_TAG__COMMENT) {
            const char* node_text = myhtml_node_text(node, NULL);
            printf(">: %s\n", node_text);
        }
        else {
            printf(">\n");
        }
        
        // print children
        print_tree(tree, myhtml_node_child(node), (inc + 1));
        node = myhtml_node_next(node);
    }
}

std::vector<std::string> nyaasi_tree::parse_torrents() 
{
  std::vector<std::string> magnet_list;
  const char *mag_k = "href";
  const char *mag_v = "magnet";
  myhtml_collection_t *magnets = myhtml_get_nodes_by_attribute_value_contain(get_tree(), NULL, NULL, true,
                                                                             mag_k, strlen(mag_k),
                                                                             mag_v, strlen(mag_v), NULL);
  
  if(magnets && magnets->list && magnets->length) {
    for (size_t i = 0; i < magnets->length; i++){
      myhtml_tree_attr_t *attr = myhtml_node_attribute_first(magnets->list[i]);
      const char *magnet_link = myhtml_attribute_value(attr,NULL);
      if (magnet_link) {
        magnet_list.push_back(magnet_link);
      }
    }
  }

  return magnet_list;
}

int nyaasi_tree::pageinfo_first() 
{
  return nyaasi_pageinfo.first_result;
}

int nyaasi_tree::per_page() 
{
  return nyaasi_pageinfo.last_result;
}

int nyaasi_tree::pages_total() 
{
  return nyaasi_pageinfo.total_result;
}
