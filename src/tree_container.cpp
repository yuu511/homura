#include <string.h>
#include <stdio.h>
#include <vector>
#include <cstring>
#include "tree_container.h"
#include "errlib.h"

using namespace homura;

tree_container::tree_container()
{
  handle = myhtml_create();
  myhtml_init(handle, MyHTML_OPTIONS_DEFAULT, threads, 0);
  tree = myhtml_tree_create();
  myhtml_tree_init(tree, handle);
}

tree_container::~tree_container() 
{
  myhtml_tree_destroy(tree);
  myhtml_destroy(handle);
}

// will have to reparse whatever (copying myhtml_tree not well defined)
tree_container::tree_container(const tree_container &lhs)
: threads(lhs.threads),
  handle(myhtml_create()),
  tree(myhtml_tree_create())
{
  myhtml_init(handle, MyHTML_OPTIONS_DEFAULT, threads, 0);
  myhtml_tree_init(tree,handle);
}

tree_container::tree_container(tree_container &&lhs)
: threads(lhs.threads),
  handle(lhs.handle),
  tree(lhs.tree)
{
  lhs.handle = nullptr;
  lhs.tree = nullptr;
}
    
HOMURA_ERRCODE tree_container::create_tree(const char *html_page) 
{
  if (myhtml_parse(tree, MyENCODING_UTF_8, html_page,strlen(html_page)) != MyHTML_STATUS_OK){
    return ERRCODE::FAILED_PARSE;
  }
  return ERRCODE::SUCCESS;
}

void tree_container::reset_tree()
{
  myhtml_tree_destroy(tree);  
  tree = myhtml_tree_create();
  myhtml_tree_init(tree, handle);
}

myhtml_tree *tree_container::get_tree()
{
  return tree;
}

int tree_container::get_threads()
{
  return threads;
}

void tree_container::set_threads(int threads_)
{
  threads = threads_;  
  myhtml_tree_destroy(tree);
  myhtml_destroy(handle);
  handle = myhtml_create();
  myhtml_init(handle, MyHTML_OPTIONS_DEFAULT, threads, 0);
  tree = myhtml_tree_create();
  myhtml_tree_init(tree, handle);
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

void tree_container::ptree(myhtml_tree_t *tree_)
{
  myhtml_tree_node_t *node = myhtml_tree_get_document(tree_);
  print_tree(tree, myhtml_node_child(node), 0);
}
