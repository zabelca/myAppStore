#define _GNU_SOURCE

#include "myAppStore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

static bool isPrime(int num)
{
  if (num <= 1) return false;
  if (num % 2 == 0 && num > 2) return 0;
  for(int i = 3; i < num / 2; i+= 2) {
    if (num % i == 0)
    return false;
  }
  return true;
}

static int nextPrime(int num) {
  while (1) {
    num++;
    if (isPrime(num)) return num;
  }
}

static int parseInteger(FILE *stream) {
  char *line = NULL;
  size_t n;
  getline(&line, &n, stream);
  int number = atoi(line);
  free(line);
  return number;
}

static float parseFloat(FILE *stream) {
  char *line = NULL;
  size_t n;
  getline(&line, &n, stream);
  float number = atof(line);
  free(line);
  return number;
}

static void parseString(FILE *stream, char *dest, size_t maxLen) {
  char *line = NULL;
  size_t n;
  getline(&line, &n, stream);
  line[strlen(line) - 1] = '\0';
  strncpy(dest, line, maxLen);
  free(line);
}

static void parseApp(FILE *stream, struct app_info *appInfo) {
  parseString(stream, appInfo->category, CAT_NAME_LEN);
  parseString(stream, appInfo->app_name, APP_NAME_LEN);
  parseString(stream, appInfo->version, VERSION_LEN);
  appInfo->size = parseFloat(stream);
  parseString(stream, appInfo->units, UNIT_SIZE);
  appInfo->price = parseFloat(stream);
}

void parseAndCreateCategories(FILE *stream, struct categories **categories, int *categoriesCount) {
  *categoriesCount = parseInteger(stream);
  *categories = (struct categories *)malloc(*categoriesCount * sizeof(struct categories));
  for (int i = 0; i < *categoriesCount; i++) {
    parseString(stream, (*categories)[i].category, CAT_NAME_LEN);
    (*categories)[i].root = NULL; // Should be NULL until first app is added to category.
  }
}

static void freeTree(struct tree **branch) {
  if (*branch != NULL) {
    if ((*branch)->left != NULL) {
      freeTree(&((*branch)->left));
    }
    if ((*branch)->right != NULL) {
      freeTree(&((*branch)->right));
    }
    free(*branch);
    *branch = NULL;
  }
}

void freeCategories(struct categories **categories, int categoriesCount) {
  if (*categories) {
    for (int i = 0; i < categoriesCount; i++) {
      freeTree(&((*categories)[i].root));
    }
    free(*categories);
    *categories = NULL;
  }
}

static struct categories *findCategory(struct categories *categories, int categoriesCount, struct app_info appInfo) {
  for (int j = 0; j < categoriesCount; j++) {
    if (strcmp(appInfo.category, categories[j].category) == 0) {
      return &(categories[j]);
    }
  }
  return NULL;
}

static void createTree(struct tree **branch, struct app_info appInfo) {
  *branch = (struct tree*)malloc(sizeof(struct tree));
  (*branch)->record = appInfo;
  (*branch)->left = NULL;
  (*branch)->right = NULL;
}

static void addAppToTree(struct tree *branch, struct app_info appInfo) {
  if (strcmp(appInfo.app_name, branch->record.app_name) < 0) {
    if (branch->left == NULL) {
      createTree(&(branch->left), appInfo);
    } else {
      addAppToTree(branch->left, appInfo);
    }
  } else if (strcmp(appInfo.app_name, branch->record.app_name) > 0) {
    if (branch->right == NULL) {
      createTree(&(branch->right), appInfo);
    } else {
      addAppToTree(branch->right, appInfo);
    }
  } else {
    fprintf(stderr, "Error: Found duplicate app name\n");
    assert(0); // We should never get here.
  }
}

static void addAppToCategory(struct categories *category, struct app_info appInfo) {
  if (category->root == NULL) {
    createTree(&(category->root), appInfo);
  } else {
    addAppToTree(category->root, appInfo);
  }
}

static struct hash_table_entry *createHashTable(int numberOfApps, int *hashTableSize) {
  *hashTableSize = nextPrime(numberOfApps * 2);
  return malloc(sizeof(struct hash_table_entry) * (*hashTableSize));
}

void freeHashTable(struct hash_table_entry **hashTable) {
  if (*hashTable) {
    free(*hashTable);
    *hashTable = NULL;
  }
}

static struct tree *findAppNodeInTree(struct tree *branch, struct app_info *appInfo) {
  struct tree *retval = NULL;
  if (strcmp(branch->record.app_name, appInfo->app_name) == 0) {
    return branch;
  } else if (branch->left != NULL && (retval = findAppNodeInTree(branch->left, appInfo))) {
    return retval;
  } else if (branch->right != NULL && (retval = findAppNodeInTree(branch->right, appInfo))) {
    return retval;
  } else {
    return NULL;
  }
}

static void addAppToHashTable(struct tree *branch,
                              struct app_info *appInfo,
                              struct hash_table_entry *hashTable,
                              int hashTableSize) {
  char *letter = appInfo->app_name;
  int appNameTotal = 0;
  while (*letter) {
    appNameTotal = appNameTotal + (int)*letter;
    letter++;
  }
  int position = (appNameTotal % hashTableSize);

  if (hashTable[position].app_node != NULL) {
    // Collision case
    hashTable[position].next = malloc(sizeof(struct hash_table_entry));
    strcpy(hashTable[position].next->app_name, hashTable[position].app_name);
    hashTable[position].next->app_node = hashTable[position].app_node; 
    hashTable[position].next->next = NULL;
  } else {
    hashTable[position].next = NULL;
  }

  strcpy(hashTable[position].app_name, appInfo->app_name);
  hashTable[position].app_node = findAppNodeInTree(branch, appInfo);
}

void parseAndCreateApplications(FILE *stream,
                                struct categories *categories,
                                int categoriesCount,
                                struct hash_table_entry **hashTable,
                                int *hashTableSize) {
  int numberOfApps = parseInteger(stream);
  *hashTable = createHashTable(numberOfApps, hashTableSize);
  for (int i = 0; i < numberOfApps; i++) {
    struct app_info appInfo;
    parseApp(stream, &appInfo);
    struct categories *foundCategory = findCategory(categories, categoriesCount, appInfo);
    addAppToCategory(foundCategory, appInfo);
    addAppToHashTable(foundCategory->root, &appInfo, *hashTable, *hashTableSize);
  }
}

static int searchHashTable(int hashTableSize, char *appName) {
  char *letter = appName;
  int appNameTotal = 0;
  while (*letter) {
    appNameTotal = appNameTotal + (int)*letter;
    letter++;
  }
  int position = (appNameTotal % hashTableSize);
  return position;
}

static bool findAppInHashTable(char *appName, struct hash_table_entry *hashTable, int hashPosition) {
  if (strcmp(appName, hashTable[hashPosition].app_name) == 0) {
    return true;
  } else if (hashTable->next != NULL) {
    return findAppInHashTable(appName, hashTable->next, hashPosition);
  } else {
    return false;
  }
}

static void findAppQuery(char *queryString,
                         FILE *outStream,
                         struct hash_table_entry *hashTable,
                         int hashTableSize) {
  char appName[APP_NAME_LEN];
  sscanf(queryString, "%*s %*s %[^\n]", appName);

  int hashPosition = searchHashTable(hashTableSize, appName);
  bool result = findAppInHashTable(appName, hashTable, hashPosition);
  if (result) {
    fprintf(outStream, "Found Application: %s\n", appName);
    fprintf(outStream, "\tCategory: %s\n", hashTable[hashPosition].app_node->record.category);
    fprintf(outStream, "\tApplication Name: %s\n", hashTable[hashPosition].app_node->record.app_name);
    fprintf(outStream, "\tVersion: %s\n", hashTable[hashPosition].app_node->record.version);
    fprintf(outStream, "\tSize: %.2f\n", hashTable[hashPosition].app_node->record.size);
    fprintf(outStream, "\tUnits: %s\n", hashTable[hashPosition].app_node->record.units);
    fprintf(outStream, "\tPrice: %.2f\n", hashTable[hashPosition].app_node->record.price);
  } else {
    fprintf(outStream, "Application %s not found\n", appName);
  }
}

void parseQueries(FILE *inStream,
                  FILE *outStream,
                  struct hash_table_entry *hashTable,
                  int hashTableSize) {
  int queryCount = 0;
  queryCount = parseInteger(inStream);
  for (int i = 0; i < queryCount; i++) {
    char queryString[1024];
    parseString(inStream, queryString, APP_NAME_LEN);

    if (strncmp("find app", queryString, 8) == 0) {
      if (i != 0) fprintf(outStream, "\n");
      findAppQuery(queryString, outStream, hashTable, hashTableSize);
    }
  }
}

