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

/* static void queryAppStore(struct app_info appInfo[], int numberOfApps, char *queryString, FILE *ostream) { */
/*   char name[APP_NAME_LEN]; */
/*   sscanf(queryString, "%*s %*s %[^\n]", name); */
/*   for (int i = 0; i < numberOfApps; i++) { */
/*     if (strcmp(name, appInfo[i].app_name) == 0) { */
/*       fprintf(ostream, "Found Application: %s\n", name); */
/*       return; */
/*     } */
/*   } */
/*   fprintf(ostream, "Application %s not found\n", name); */
/* } */

void parseAndCreateCategories(FILE *stream, struct categories **categories, int *categoriesCount) {
  *categoriesCount = parseInteger(stream);
  *categories = (struct categories *)malloc(*categoriesCount * sizeof(struct categories));
  for (int i = 0; i < *categoriesCount; i++) {
    parseString(stream, (*categories)[i].category, CAT_NAME_LEN);
    (*categories)[i].root = NULL; // Should be NULL until first app is added to category.
  }
}

void destroyTree(struct tree **branch) {
  if (*branch != NULL) {
    if ((*branch)->left != NULL) {
      destroyTree(&((*branch)->left));
    }
    if ((*branch)->right != NULL) {
      destroyTree(&((*branch)->right));
    }
    free(*branch);
    *branch = NULL;
  }
}

void destroyCategories(struct categories **categories, int categoriesCount) {
  for (int i = 0; i < categoriesCount; i++) {
    destroyTree(&((*categories)[i].root));
  }
  free(*categories);
  *categories = NULL;
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
  return (struct hash_table_entry*)malloc(sizeof(struct hash_table_entry) * (*hashTableSize));
}

static void destroyHashTable(struct hash_table_entry **hashTable) {
  free(*hashTable);
  *hashTable = NULL;
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
  }
}

