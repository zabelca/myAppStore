#include "myAppStore.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  parseAndCreateCategories(stdin, &categories, &categoriesCount);

  struct hash_table_entry *hashTable = NULL;
  int hashTableSize = 0;
  parseAndCreateApplications(stdin, categories, categoriesCount, &hashTable, &hashTableSize);

  parseQueries(stdin, stdout, hashTable, hashTableSize);

  return 0;
}
