#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "tree_container.h"
#include "errlib.h"

using namespace homura;

pagination_information::pagination_information(int first, int last, int total) {
  first_result = first;
  last_result = last;
  total_result = total;
}

tree_container::tree_container(int threads)
  : nyaasi_pageinfo (pagination_information(0,0,0)) {
  handle = myhtml_create();
  myhtml_init(handle, MyHTML_OPTIONS_DEFAULT, threads, 0);

  tree = myhtml_tree_create();
  myhtml_tree_init(tree, handle);
}

bool tree_container::parse_HTML(const char *html_page) {
  if (!html_page)
    return false;
  return MyHTML_STATUS_OK == myhtml_parse(tree, MyENCODING_UTF_8, html_page,strlen(html_page));
}

tree_container::~tree_container() {
  if (tree)
    myhtml_tree_destroy(tree);
  if (handle)
    myhtml_destroy(handle);
}

bool tree_container::nyaasi_parse_pageinfo() {
  if (!tree || !handle) { 
    errprintf(ERRCODE::FAILED_MYHTML_TREE_INIT, "No tree or handle detected in"
      "get_pagination_information\n");
    return false;
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
      return false;
    }
    if (myhtml_collection_destroy(found)) {
      errprintf(ERRCODE::FAILED_FREE, "Failed to free MyHTML collection.");
      return false;
    }
  } 
  else {
    errprintf(ERRCODE::FAILED_PARSE, "Failed to parse first page (Pagination information not found)\n");
    return false;
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
         return false;
       }
    }
    else 
      ++endptr;
  }
  free(copy);
  if (stk.size() != 3) {
    errprintf(ERRCODE::FAILED_PARSE,"Incorrect pagination string parse.\n");
    return false;
  }

  nyaasi_pageinfo = pagination_information(stk[0],stk[1],stk[2]);

  if (homura::options::debug_level) {
    fprintf (stdout,"== First page result information == \n");
    fprintf (stdout,"String: \"%s\" \nfirst result "
                    "%d\nlast result (results per page) %d\ntotal results %d\n\n",
            page_information,
            this->nyaasi_pageinfo_first(),
            this->nyaasi_pageinfo_results_per_page(),
            this->nyaasi_pageinfo_total());
  }

  return true;
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

bool tree_container::nyaasi_parse_torrents() {
  // parse html
  // myhtml_collection_t *collection = myhtml_get_nodes_by_tag_id(tree, NULL, MyHTML_TAG_TBODY, NULL);
  const char *mag_k = "href";
  const char *mag_v = "magnet";
  myhtml_collection_t *magnets = myhtml_get_nodes_by_attribute_value_contain(tree, NULL, NULL, true,
                                                                             mag_k, strlen(mag_k),
                                                                             mag_v, strlen(mag_v), NULL);
  
  if(magnets && magnets->list && magnets->length) {
    for (size_t i = 0; i < magnets->length; i++){
      myhtml_tree_attr_t *attr = myhtml_node_attribute_first(magnets->list[i]);
      const char *magnet_link = myhtml_attribute_value(attr,NULL);
      if (magnet_link) {
        printf ("%s\n",magnet_link);
      }
    }
  }
  return true;
}

int tree_container::nyaasi_pageinfo_first() {
  return nyaasi_pageinfo.first_result;
}

int tree_container::nyaasi_pageinfo_results_per_page() {
  return nyaasi_pageinfo.last_result;
}

int tree_container::nyaasi_pageinfo_total() {
  return nyaasi_pageinfo.total_result;
}

