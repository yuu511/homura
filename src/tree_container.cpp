#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <vector>
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
    
HOMURA_ERRCODE tree_container::create_tree(const char *html_page) 
{
  if (myhtml_parse(tree, MyENCODING_UTF_8, html_page,strlen(html_page)) != MyHTML_STATUS_OK){
    return ERRCODE::FAILED_PARSE;
  }
  return ERRCODE::SUCCESS;
}

myhtml_t *tree_container::get_handle() 
{
  return handle;
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
}

tree_container::~tree_container() 
{
  if (tree)
    myhtml_tree_destroy(tree);
  if (handle)
    myhtml_destroy(handle);
}

tree_container::tree_container(const tree_container& c) 
{
  handle = myhtml_create();
  myhtml_init(handle, MyHTML_OPTIONS_DEFAULT, c.threads, 0);
  tree = myhtml_tree_create();
  myhtml_tree_init(tree, handle);
}
