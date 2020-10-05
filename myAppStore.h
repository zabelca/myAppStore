#ifndef MYAPPSTORE
#define MYAPPSTORE

#include <stdio.h>

#define CAT_NAME_LEN 25
#define APP_NAME_LEN 50
#define VERSION_LEN 10
#define UNIT_SIZE 3

struct categories {
  char category[CAT_NAME_LEN]; // Name of the category
  struct tree *root; // pointer to the root of the search tree
};

struct app_info {
  char category[CAT_NAME_LEN]; // category name
  char app_name[APP_NAME_LEN]; // Application name
  char version[VERSION_LEN]; // version #
  float size; // size of app
  char units[UNIT_SIZE]; // unit of size
  float price; // price of app
};

// Binary Search Tree
struct tree {
  struct app_info record; // application information
  struct tree *left; // left subtree
  struct tree *right; // right subtree
};

struct hash_table_entry {
  char app_name[APP_NAME_LEN]; // app name
  struct tree *app_node; // pointer to node containing app info
  struct hash_table_entry *next; // pointer to next entry
};

int parseNumberOfCategories(FILE *stream);
void parseCategories(FILE *stream, struct categories categories[], int numberOfCategories);

#endif
