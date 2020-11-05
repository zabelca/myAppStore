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

static struct categories *findCategory(struct categories *categories, int categoriesCount, char *appInfoCategory) {
  for (int j = 0; j < categoriesCount; j++) {
    if (strcmp(appInfoCategory, categories[j].category) == 0) {
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
    struct categories *foundCategory = findCategory(categories, categoriesCount, appInfo.category);
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
  sscanf(queryString, "%*s %*s \"%[^\n\"]\"", appName);

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

static void printAppNamesInCategory(FILE *outStream, struct tree *root) {
  if (root->left != NULL) {
    printAppNamesInCategory(outStream, root->left);
  }

  fprintf(outStream, "\t%s\n", root->record.app_name);

  if (root->right != NULL) {
    printAppNamesInCategory(outStream, root->right);
  }
}

static void findCatQuery(char *queryString,
                         FILE *outStream,
                         struct hash_table_entry *hashTable,
                         int hashTableSize,
                         struct categories *categories,
                         int categoriesCount) {
  char catName[CAT_NAME_LEN];
  sscanf(queryString, "%*s %*s \"%[^\n\"]\"", catName);
  struct categories *category = findCategory(categories, categoriesCount, catName);
  if (category) {
    if (category->root != NULL /* how can we tell if a category has apps. to check if the root is not null */) {
      fprintf(outStream, "Category: %s\n", catName);
      printAppNamesInCategory(outStream, category->root);
    } else {
      fprintf(outStream, "Category %s no apps found\n", catName);
    }
  } else {
    fprintf(outStream, "Category foo bar not found\n");
  }
}

static int printFreeApps(FILE *outStream, struct tree *root) {
  int freeAppCount = 0;
  
  if (root != NULL) {
    if (root->left != NULL) {
      freeAppCount += printFreeApps(outStream, root->left);
    }

    if (root->record.price == 0.0f) {
      freeAppCount++;
      fprintf(outStream, "\t%s\n", root->record.app_name);
    }

    if (root->right != NULL) {
      freeAppCount += printFreeApps(outStream, root->right);
    }
  }

  return freeAppCount;
}

static void findPriceFreeQuery(char *queryString,
                               FILE *outStream,
                               struct hash_table_entry *hashTable,
                               int hashTableSize,
                               struct categories *categories,
                               int categoriesCount) {
  for (int i = 0; i < categoriesCount; i++) {
    fprintf(outStream, "Free Applications in Category: %s\n", categories[i].category);
    int freeAppCount = printFreeApps(outStream, categories[i].root);
    if (freeAppCount == 0) {
      fprintf(outStream, "\tNo free applications found\n");
    }
  }
}

static void printRangePrice(FILE *outStream, struct tree *root, float low, float high) {
  if (root != NULL) {
    if (root->left != NULL) {
      printRangePrice(outStream, root->left, low, high);
    }

    if (root->record.price >= low && root->record.price <= high) {
      fprintf(outStream, "\t%s\n", root->record.app_name);
    }

    if (root->right != NULL) {
      printRangePrice(outStream, root->right, low, high);
    }
  }
}

static int getAppCountForRangePrice(FILE *outStream, struct tree *root, float low, float high) {
  int freeAppCount = 0;

  if (root != NULL) {
    if (root->left != NULL) {
      freeAppCount += getAppCountForRangePrice(outStream, root->left, low, high);
    }

    if (root->record.price >= low && root->record.price <= high) {
      freeAppCount++;
    }

    if (root->right != NULL) {
      freeAppCount += getAppCountForRangePrice(outStream, root->right, low, high);
    }
  }
  return freeAppCount;
}

static void rangePriceQuery(char *queryString,
                            FILE *outStream,
                            struct hash_table_entry *hashTable,
                            int hashTableSize,
                            struct categories *categories,
                            int categoriesCount) {
  float low = 0.0;
  float high = 0.0;
  char catName[CAT_NAME_LEN];
  sscanf(queryString, "%*s \"%[^\"]\" %*s %f %f", catName, &low, &high);
  struct categories *category = findCategory(categories, categoriesCount, catName);
  int appCount = getAppCountForRangePrice(outStream, category->root, low, high);
  if (appCount) {
    fprintf(outStream, "Applications in Price Range (%.2f,%.2f) in Category: %s\n", low, high, catName);
    printRangePrice(outStream, category->root, low, high);
  } else {
    fprintf(outStream, "No applications found in %s for the given price range (%.2f,%.2f)\n", catName, low, high);
  }
}

static void printRangeApp(FILE *outStream, struct tree *root, char *low, char *high) {
  if (root != NULL) {
    if (root->left != NULL) {
      printRangeApp(outStream, root->left, low, high);
    }

    if ((strcmp(root->record.app_name, high) < 0) && (strcmp(root->record.app_name, low) > 0)) {
      fprintf(outStream, "\t%s\n", root->record.app_name);
    }

    if (root->right != NULL) {
      printRangeApp(outStream, root->right, low, high);
    }
  }
}

static int getAppCountForRangeApp(FILE *outStream, struct tree *root, char *low, char *high) {
  int freeAppCount = 0;

  if (root != NULL) {
    if (root->left != NULL) {
      freeAppCount += getAppCountForRangeApp(outStream, root->left, low, high);
    }

    if ((strcmp(root->record.app_name, high) < 0) && (strcmp(root->record.app_name, low) > 0)) {
      freeAppCount++;
    }

    if (root->right != NULL) {
      freeAppCount += getAppCountForRangeApp(outStream, root->right, low, high);
    }
  }
  return freeAppCount;
}

static void rangeAppQuery(char *queryString,
                          FILE *outStream,
                          struct hash_table_entry *hashTable,
                          int hashTableSize,
                          struct categories *categories,
                          int categoriesCount) {
  char low[32];
  char high[32];
  char catName[CAT_NAME_LEN];
  sscanf(queryString, "%*s \"%[^\"]\" %*s %s %s", catName, low, high);
  struct categories *category = findCategory(categories, categoriesCount, catName);
  int appCount = getAppCountForRangeApp(outStream, category->root, low, high);
  if (appCount) {
    fprintf(outStream, "Applications in Range (%s,%s) in Category: %s\n", low, high, catName);
    printRangeApp(outStream, category->root, low, high);
  } else {
    fprintf(outStream, "No applications found in %s for the given range (%s,%s)\n", catName, low, high);
  }
}

struct tree *findInOrderSuccessor(struct tree *branch) {
  if (branch->left) {
    return findInOrderSuccessor(branch->left);
  } else {
    return branch;
  }
}

static bool deleteFromTree(struct tree *branch, struct tree *parent, char *appName) {
  bool appDeleted = false;

  if (branch != NULL) {
    if (branch->left != NULL) {
      appDeleted |= deleteFromTree(branch->left, branch, appName);
    }

    if (strcmp(branch->record.app_name, appName) == 0) {
      appDeleted = true;

      if (branch->left != NULL && branch->right == NULL) {
        parent->left = branch->left;
        free(branch);
        branch = NULL;
      } else if (branch->left == NULL && branch->right != NULL) {
        parent->right = branch->right;
        free(branch);
        branch = NULL;
      } else if (branch->left != NULL && branch->right != NULL) {
        struct tree *pSuccessor = findInOrderSuccessor(branch->right);
        branch->record = pSuccessor->record;
        branch->left = pSuccessor->left;
        branch->right = pSuccessor->right;
        free(pSuccessor);
        pSuccessor = NULL;
      }
    }

    if (branch->right != NULL) {
      appDeleted |= deleteFromTree(branch->right, branch, appName);
    }
  }

  return appDeleted;
}

static bool deleteFromHashTable(char *appName, struct hash_table_entry *hashTable, int hashTableSize) {
  int hashPosition = searchHashTable(hashTableSize, appName);

  bool appFound = false;
  bool atRoot = true;
  struct hash_table_entry *previousHashTableEntry = NULL;
  struct hash_table_entry *currentHashTableEntry = &(hashTable[hashPosition]);

  while (currentHashTableEntry != NULL) {
    if (strcmp(appName, currentHashTableEntry->app_name) == 0) {
      if (atRoot) {
        if (currentHashTableEntry->next != NULL) {
          strcpy(currentHashTableEntry->app_name, currentHashTableEntry->next->app_name);
          currentHashTableEntry->app_node = currentHashTableEntry->next->app_node;
          currentHashTableEntry->next = currentHashTableEntry->next->next;
        } else {
          strcpy(currentHashTableEntry->app_name, "");
          currentHashTableEntry->app_node = NULL;
          currentHashTableEntry->next = NULL;
        }
      } else {
        previousHashTableEntry->next = currentHashTableEntry->next;
      }
      appFound = true;
      break;
    }

    previousHashTableEntry = currentHashTableEntry;
    currentHashTableEntry = currentHashTableEntry->next;
    atRoot = false;
  }

  return appFound;
}

static void deleteQuery(char *queryString,
                        FILE *outStream,
                        struct hash_table_entry *hashTable,
                        int hashTableSize,
                        struct categories *categories,
                        int categoriesCount) {
  bool appDeleted = false;
  char catName[CAT_NAME_LEN];
  char appName[APP_NAME_LEN];
  sscanf(queryString, "%*s \"%[^\"]\" \"%[^\"]\"", catName, appName);
  struct categories *foundCategory = findCategory(categories, categoriesCount, catName);

  if (foundCategory) {
    if (deleteFromTree(foundCategory->root, NULL, appName)) {
      if (deleteFromHashTable(appName, hashTable, hashTableSize)) {
        fprintf(outStream, "Application %s from Category %s successfully deleted\n", appName, catName);
        appDeleted = true;
      }
    }
  }

  if (! appDeleted) {
    fprintf(outStream, "Application %s not found in category %s; unable to delete\n", appName, catName);
  }
}

void parseQueries(FILE *inStream,
                  FILE *outStream,
                  struct hash_table_entry *hashTable,
                  int hashTableSize,
                  struct categories *categories,
                  int categoriesCount) {
  int queryCount = 0;
  queryCount = parseInteger(inStream);
  for (int i = 0; i < queryCount; i++) {
    char queryString[1024];
    parseString(inStream, queryString, APP_NAME_LEN);

    if (strncmp("find app", queryString, 8) == 0) {
      if (i != 0) fprintf(outStream, "\n");
      findAppQuery(queryString, outStream, hashTable, hashTableSize);
    } else if (strncmp("find category", queryString, 13) == 0) {
      if (i != 0) fprintf(outStream, "\n");
      findCatQuery(queryString, outStream, hashTable, hashTableSize, categories, categoriesCount);
    } else if (strncmp("find price free", queryString, 15) == 0) {
      if (i != 0) fprintf(outStream, "\n");
      findPriceFreeQuery(queryString, outStream, hashTable, hashTableSize, categories, categoriesCount);
    } else if (strncmp("range", queryString, 5) == 0) {
      char rangeQueryString[5];
      sscanf(queryString, "%*s %*s %s %*[^\n]", rangeQueryString);
      if (strcmp(rangeQueryString, "price") == 0) {
        rangePriceQuery(queryString, outStream, hashTable, hashTableSize, categories, categoriesCount);
      } else if (strcmp(rangeQueryString, "app") == 0) {
        rangeAppQuery(queryString, outStream, hashTable, hashTableSize, categories, categoriesCount);
      }
    } else if (strncmp("delete", queryString, 5) == 0) {
      deleteQuery(queryString, outStream, hashTable, hashTableSize, categories, categoriesCount);
    }
  }
}

