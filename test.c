#define _GNU_SOURCE

#include "myAppStore.h"

#include <stdio.h>
#include <string.h>
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
  mu_assert("TEST FAIL: count of categories != 3", categoriesCount == 3);
  return 0;
}

static char *test_can_parse_and_create_category_names() {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  mu_assert("TEST FAIL: categories[0].category != 'Games'", strcmp(categories[0].category, "Games") == 0);
  mu_assert("TEST FAIL: categories[1].category != 'Medical'", strcmp(categories[1].category, "Medical") == 0);
  mu_assert("TEST FAIL: categories[2].category != 'Social Networking'", strcmp(categories[2].category, "Social Networking") == 0);
  return 0;
}

static char *test_categories_start_with_null_tree() {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  mu_assert("TEST FAIL: categories[0].root != NULL", categories[0].root == NULL);
  mu_assert("TEST FAIL: categories[0].root != NULL", categories[1].root == NULL);
  mu_assert("TEST FAIL: categories[0].root != NULL", categories[2].root == NULL);
  return 0;
}

static char *test_can_destroy_categories() {
  struct categories *categories = NULL;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  destroyCategories(&categories, categoriesCount);
  mu_assert("TEST FAIL: categories != NULL", categories == NULL);
  return 0;
}

static void appTestInit(struct categories *categories, int categoriesCount, struct hash_table_entry **hashTable, int *hashTableSize) {
  char buffer[] = "2\nGames\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n";
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
  mu_assert("TEST FAIL: root app name != 'Minecraft ...'", strcmp(categories[0].root->record.app_name, "Minecraft: Pocket Edition") == 0);
  return 0;
}

static char *test_leaf_nodes_have_null_left_and_right() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("TEST FAIL: right != NULL", categories[0].root->right == NULL);
  mu_assert("TEST FAIL: left != NULL", categories[0].root->left->left == NULL);
  mu_assert("TEST FAIL: left != NULL", categories[0].root->left->right == NULL);
  return 0;
}

static char *test_fifa_is_left_of_minecraft() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("TEST FAIL: left app name != 'FIFA ...'", strcmp(categories[0].root->left->record.app_name, "FIFA 16 Ultimate Team") == 0);
  return 0;
}

static char *test_hash_table_size_is_next_prime() {
  struct categories *categories = NULL;
  struct hash_table_entry *hashTable = NULL;
  int hashTableSize;
  int categoriesCount = 0;
  categoryTestInit(&categories, &categoriesCount);
  appTestInit(categories, categoriesCount, &hashTable, &hashTableSize);
  mu_assert("TEST FAIL: hashTableSize != 5", hashTableSize == 5);
  return 0;
}

static char *allTests() {
  mu_run_test(test_can_parse_category_count);
  mu_run_test(test_can_parse_and_create_category_names);
  mu_run_test(test_categories_start_with_null_tree);
  mu_run_test(test_can_destroy_categories);
  mu_run_test(test_can_create_root_record);
  mu_run_test(test_leaf_nodes_have_null_left_and_right);
  mu_run_test(test_fifa_is_left_of_minecraft);
  mu_run_test(test_hash_table_size_is_next_prime);
  return 0;
}

/* static char *test_can_parse_count_for_two_apps() { */
/*   char buffer[] = "2\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   int appCount = parseNumberOfApps(stream); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: count of apps != 2", appCount == 2); */
/*   return 0; */
/* } */

/* static char *test_can_parse_app_category() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[1]; */
/*   parseApps(stream, appInfo, 1); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: category for first app != 'Games'", strcmp(appInfo[0].category, "Games") == 0); */
/*   return 0; */
/* } */

/* static char *test_can_parse_app_name() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[1]; */
/*   parseApps(stream, appInfo, 1); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: name for first app != 'Minecraft: Pocket Edition'", strcmp(appInfo[0].app_name, "Minecraft: Pocket Edition") == 0); */
/*   return 0; */
/* } */

/* static char *test_can_parse_app_version() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[1]; */
/*   parseApps(stream, appInfo, 1); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: verson for first app != '0.12.1'", strcmp(appInfo[0].version, "0.12.1") == 0); */
/*   return 0; */
/* } */

/* static char *test_can_parse_app_size() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[1]; */
/*   parseApps(stream, appInfo, 1); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: size for first app != 24.1", appInfo[0].size == (float)24.1); */
/*   return 0; */
/* } */

/* static char *test_can_parse_app_units() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[1]; */
/*   parseApps(stream, appInfo, 1); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: units for first app != 'MB'", strcmp(appInfo[0].units, "MB") == 0); */
/*   return 0; */
/* } */

/* static char *test_can_parse_app_price() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[1]; */
/*   parseApps(stream, appInfo, 1); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: price for first app != 6.99", appInfo[0].price == (float)6.99); */
/*   return 0; */
/* } */

/* static char *test_can_parse_a_second_app() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[2]; */
/*   parseApps(stream, appInfo, 2); */
/*   fclose(stream); */
/*   mu_assert("TEST FAIL: category for second app != 'Games'", strcmp(appInfo[1].category, "Games") == 0); */
/*   mu_assert("TEST FAIL: name for second app != 'FIFA 16 Ultimate Team'", strcmp(appInfo[1].app_name, "FIFA 16 Ultimate Team") == 0); */
/*   mu_assert("TEST FAIL: verson for second app != '2.0'", strcmp(appInfo[1].version, "2.0") == 0); */
/*   mu_assert("TEST FAIL: size for second app != 1.25", appInfo[1].size == (float)1.25); */
/*   mu_assert("TEST FAIL: units for second app != 'GB'", strcmp(appInfo[1].units, "GB") == 0); */
/*   mu_assert("TEST FAIL: price for second app != 0.00", appInfo[1].price == (float)0.00); */
/*   return 0; */
/* } */

/* static char *test_find_missing_app_with_no_parsed_apps() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[2]; */
/*   parseApps(stream, appInfo, 2); */
/*   fclose(stream); */

/*   char obuffer[256] = {0}; */
/*   FILE *ostream = fmemopen(obuffer, 256, "w"); */
/*   queryAppStore(appInfo, 2, "find app foo", ostream); */
/*   fflush(ostream); */
/*   mu_assert("TEST FAIL: find missing app != 'Application foo not found<CR>'", strcmp(obuffer, "Application foo not found\n") == 0); */
/*   fclose(ostream); */
/*   return 0; */
/* } */

/* static char *test_find_app_prints_found_message() { */
/*   char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n"; */
/*   FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r"); */
/*   struct app_info appInfo[2]; */
/*   parseApps(stream, appInfo, 2); */
/*   fclose(stream); */

/*   char obuffer[256] = {0}; */
/*   FILE *ostream = fmemopen(obuffer, 256, "w"); */
/*   queryAppStore(appInfo, 2, "find app Minecraft: Pocket Edition", ostream); */
/*   fflush(ostream); */

/*   mu_assert("TEST FAIL: find app first line != 'Found Application: Minecraft: Pocket Edition<CR>'", strcmp(obuffer, "Found Application: Minecraft: Pocket Edition\n") == 0); */

/*   fclose(ostream); */
/*   return 0; */
/* } */

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
