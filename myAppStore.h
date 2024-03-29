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

// Reads from `stream` and parses the input to create the categories array.
// INPUT:
//    stream - input stream where the text to parse will be read
//    categories - (OUTPUT) pointer that will be redirected to the newly allocated categories array
//    categoriesCount - (OUTPUT) number of categories created
void parseAndCreateCategories(FILE *stream, struct categories **categories, int *categoriesCount);

// Frees memory allocated in `parseAndCreateCategories`
// INPUT:
//    categories - (OUTPUT) pointer that will be set to NULL after freed
//    categoriesCount - number of categories
void freeCategories(struct categories **categories, int categoriesCount);

// Puts the app into the binary tree and hash table.
// INPUT:
//    stream - input stream where the text to parse will be read
//    categories - Array of all categories
//    categoriesCount - Number of categories
//    hashTable - (OUTPUT) pointer that will be redirected to a newly allocated hash table entry
//    hashTableSize - (OUTPUT) size of hash table
void parseAndCreateApplications(FILE *stream,
                                struct categories *categories,
                                int categoriesCount,
                                struct hash_table_entry **hashTable,
                                int *hashTableSize);

// Frees memory allocated for hashTable.
// INPUT:
//   hashTable - (IN/OUT) the hash table created in `parseAndCreateApplications`
void freeHashTable(struct hash_table_entry **hashTable);

// Parses queries.
// INPUT:
//    inStream - input stream where the text to parse will be read
//    outStream - output stream where query results will be written
//    hashTable - the hash table with application data
//    hashTableSize - the size of the hash table
void parseQueries(FILE *inStream,
                  FILE *outStream,
                  struct hash_table_entry *hashTable,
                  int hashTableSize,
                  struct categories *categories,
                  int categoriesCount);

#endif
