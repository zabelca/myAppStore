#define _GNU_SOURCE

#include "myAppStore.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minunit.h"
 
int tests_run = 0;

static void categoryTestInit(struct categories **categories, int *categoriesCount) {
  char buffer[] = "3\nGames\nMedical\nSocial Networking\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer), "r");
  parseAndCreateCategories(stream, categories, categoriesCount);
  fclose(stream);
}

static char *test_can_parse_category_count() {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  mu_assert("count of categories != 3", categoriesCount == 3);
  return 0;
}

static char *test_can_parse_and_create_category_names() {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  mu_assert("categories[0].category != 'Games'", strcmp(categories[0].category, "Games") == 0);
  mu_assert("categories[1].category != 'Medical'", strcmp(categories[1].category, "Medical") == 0);
  mu_assert("categories[2].category != 'Social Networking'", strcmp(categories[2].category, "Social Networking") == 0);
  return 0;
}

static char *test_categories_start_with_null_tree() {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  mu_assert("categories[0].root != NULL", categories[0].root == NULL);
  mu_assert("categories[0].root != NULL", categories[1].root == NULL);
  mu_assert("categories[0].root != NULL", categories[2].root == NULL);
  return 0;
}

static char *test_can_free_categories() {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  freeCategories(&categories, categoriesCount);
  mu_assert("categories != NULL", categories == NULL);
  return 0;
}

static void appTestInit(struct categories *categories, int categoriesCount, struct hash_table_entry **hashTable, int *hashTableSize) {
  char buffer[] = "2\nGames\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  parseAndCreateApplications(stream, categories, categoriesCount, hashTable, hashTableSize);
  fclose(stream);
}

static void appTestCollisionInit(struct categories *categories, int categoriesCount, struct hash_table_entry **hashTable, int *hashTableSize) {
  char buffer[] = "2\nGames\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nPocket Edition: Minecraft\n2.0\n1.25\nGB\n0.00\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  parseAndCreateApplications(stream, categories, categoriesCount, hashTable, hashTableSize);
  fclose(stream);
}

static char *test_can_create_root_record() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("root app name != 'Minecraft ...'", strcmp(categories[0].root->record.app_name, "Minecraft: Pocket Edition") == 0);
  return 0;
}

static char *test_leaf_nodes_have_null_left_and_right() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("right != NULL", categories[0].root->right == NULL);
  mu_assert("left != NULL", categories[0].root->left->left == NULL);
  mu_assert("left != NULL", categories[0].root->left->right == NULL);
  return 0;
}

static char *test_fifa_is_left_of_minecraft() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("left app name != 'FIFA ...'", strcmp(categories[0].root->left->record.app_name, "FIFA 16 Ultimate Team") == 0);
  return 0;
}

static char *test_hash_table_size_is_next_prime() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("hashTableSize != 5", hashTableSize == 5);
  return 0;
}

static char *test_hash_table_contains_app_name_at_correct_position() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("Minecraft app_name is not at position 3", strcmp(hashTable[3].app_name, "Minecraft: Pocket Edition") == 0);
  mu_assert("FIFA app_name is not at position 0", strcmp(hashTable[0].app_name, "FIFA 16 Ultimate Team") == 0);
  return 0;
}

static char *test_hash_table_contains_app_node_at_correct_position() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("Minecraft app_node is not at position 3", hashTable[3].app_node == categories[0].root);
  mu_assert("FIFA app_node is not at position 0", hashTable[0].app_node == categories[0].root->left);
  return 0;
}

static char *test_collision_replaces_app_in_hash_table() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestCollisionInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("Pocket Edition app_name is not at position 3", strcmp(hashTable[3].app_name, "Pocket Edition: Minecraft") == 0);
  return 0;
}

static char *test_collision_moves_previous_app_to_next() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestCollisionInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert(".next should not be NULL", hashTable[3].next != NULL);
  mu_assert("Minecraft app_name is not at position 3.next", strcmp(hashTable[3].next->app_name, "Minecraft: Pocket Edition") == 0);
  return 0;
}

static char *test_find_app_query_with_missing_app() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  char inBuffer[] = "1\nfind app foo bar\n";
  FILE *inStream = fmemopen(inBuffer, sizeof(inBuffer) * sizeof(char), "r");
  char outBuffer[128];
  FILE *outStream = fmemopen(outBuffer, sizeof(outBuffer) * sizeof(char), "w");
  parseQueries(inStream, outStream, hashTable, hashTableSize);
  fflush(outStream);
  mu_assert("query didn't print 'Application foo bar not found'", strncmp(outBuffer, "Application foo bar not found\n", 30) == 0);
  return 0;
}

static char *test_app_found_query() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  char inBuffer[] = "1\nfind app Minecraft: Pocket Edition\n";
  FILE *inStream = fmemopen(inBuffer, sizeof(inBuffer) * sizeof(char), "r");
  char outBuffer[1024];
  char *pOutBuffer = outBuffer;
  FILE *outStream = fmemopen(outBuffer, sizeof(outBuffer) * sizeof(char), "w");
  parseQueries(inStream, outStream, hashTable, hashTableSize);
  fflush(outStream);
  mu_assert("query didn't print 'Found Application: Minecraft: Pocket Edition<CR>'", strncmp(pOutBuffer, "Found Application: Minecraft: Pocket Edition\n", 45) == 0);
  pOutBuffer+=45;
  mu_assert("query didn't print '<TAB>Category: Games<CR>'", strncmp(pOutBuffer, "\tCategory: Games\n", 17) == 0);
  pOutBuffer+=17;
  mu_assert("query didn't print '<TAB>Application Name: Minecraft: Pocket Edition<CR>'", strncmp(pOutBuffer, "\tApplication Name: Minecraft: Pocket Edition\n", 45) == 0);
  pOutBuffer+=45;
  mu_assert("query didn't print '<TAB>Version: 0.12.1<CR>'", strncmp(pOutBuffer, "\tVersion: 0.12.1\n", 17) == 0);
  pOutBuffer+=17;
  mu_assert("query didn't print '<TAB>Size: 24.10<CR>'", strncmp(pOutBuffer, "\tSize: 24.10\n", 13) == 0);
  pOutBuffer+=13;
  mu_assert("query didn't print '<TAB>Units: MB<CR>'", strncmp(pOutBuffer, "\tUnits: MB\n", 11) == 0);
  pOutBuffer+=11;
  mu_assert("query didn't print '<TAB>Price: 6.99<CR>'", strncmp(pOutBuffer, "\tPrice: 6.99\n", 13) == 0);
  pOutBuffer+=13;
  return 0;
}

static char *allTests() {
  mu_run_test(test_can_parse_category_count);
  mu_run_test(test_can_parse_and_create_category_names);
  mu_run_test(test_categories_start_with_null_tree);
  mu_run_test(test_can_free_categories);
  mu_run_test(test_can_create_root_record);
  mu_run_test(test_leaf_nodes_have_null_left_and_right);
  mu_run_test(test_fifa_is_left_of_minecraft);
  mu_run_test(test_hash_table_size_is_next_prime);
  mu_run_test(test_hash_table_contains_app_name_at_correct_position);
  mu_run_test(test_hash_table_contains_app_node_at_correct_position);
  mu_run_test(test_collision_replaces_app_in_hash_table);
  mu_run_test(test_collision_moves_previous_app_to_next);
  mu_run_test(test_find_app_query_with_missing_app);
  mu_run_test(test_app_found_query);
  return 0;
}

int main(int argc, char **argv) {
  char *result = allTests();

  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }

  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
