#define _GNU_SOURCE

#include "myAppStore.h"

#include <stdio.h>
#include <string.h>
#include "minunit.h"
 
int tests_run = 0;

static char *test_can_parse_count_for_one_category() {
  char buffer[] = "1\nGames";
  FILE *stream;
  stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  int categoriesCount = parseNumberOfCategories(stream);
  fclose(stream);
  mu_assert("TEST FAIL: count of categories != 1", categoriesCount == 1);
  return 0;
}

static char *test_can_parse_count_for_two_category() {
  char buffer[] = "2\nGames\nMedical";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  int categoriesCount = parseNumberOfCategories(stream);
  mu_assert("TEST FAIL: count of categories != 2", categoriesCount == 2);
  return 0;
}

static char *test_can_parse_three_category() {
  char buffer[] = "Games\nMedical\nSocial Networking\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct categories categories[3];
  parseCategories(stream, categories, 3);
  fclose(stream);
  mu_assert("TEST FAIL: categories[0].category != 'Games'", strcmp(categories[0].category, "Games") == 0);
  mu_assert("TEST FAIL: categories[1].category != 'Medical'", strcmp(categories[1].category, "Medical") == 0);
  mu_assert("TEST FAIL: categories[2].category != 'Social Networking'", strcmp(categories[2].category, "Social Networking") == 0);
  return 0;
}

static char *test_can_parse_count_for_one_app() {
  char buffer[] = "1\n";
  FILE *stream;
  stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  int appCount = parseNumberOfApps(stream);
  fclose(stream);
  mu_assert("TEST FAIL: count of apps != 1", appCount == 1);
  return 0;
}

static char *test_can_parse_count_for_two_apps() {
  char buffer[] = "2\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  int appCount = parseNumberOfApps(stream);
  fclose(stream);
  mu_assert("TEST FAIL: count of apps != 2", appCount == 2);
  return 0;
}

static char *test_can_parse_app_category() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[1];
  parseApps(stream, appInfo, 1);
  fclose(stream);
  mu_assert("TEST FAIL: category for first app != 'Games'", strcmp(appInfo[0].category, "Games") == 0);
  return 0;
}

static char *test_can_parse_app_name() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[1];
  parseApps(stream, appInfo, 1);
  fclose(stream);
  mu_assert("TEST FAIL: name for first app != 'Minecraft: Pocket Edition'", strcmp(appInfo[0].app_name, "Minecraft: Pocket Edition") == 0);
  return 0;
}

static char *test_can_parse_app_version() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[1];
  parseApps(stream, appInfo, 1);
  fclose(stream);
  mu_assert("TEST FAIL: verson for first app != '0.12.1'", strcmp(appInfo[0].version, "0.12.1") == 0);
  return 0;
}

static char *test_can_parse_app_size() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[1];
  parseApps(stream, appInfo, 1);
  fclose(stream);
  mu_assert("TEST FAIL: size for first app != 24.1", appInfo[0].size == (float)24.1);
  return 0;
}

static char *test_can_parse_app_units() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[1];
  parseApps(stream, appInfo, 1);
  fclose(stream);
  mu_assert("TEST FAIL: units for first app != 'MB'", strcmp(appInfo[0].units, "MB") == 0);
  return 0;
}

static char *test_can_parse_app_price() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[1];
  parseApps(stream, appInfo, 1);
  fclose(stream);
  mu_assert("TEST FAIL: price for first app != 6.99", appInfo[0].price == (float)6.99);
  return 0;
}

static char *test_can_parse_a_second_app() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[2];
  parseApps(stream, appInfo, 2);
  fclose(stream);
  mu_assert("TEST FAIL: category for second app != 'Games'", strcmp(appInfo[1].category, "Games") == 0);
  mu_assert("TEST FAIL: name for second app != 'FIFA 16 Ultimate Team'", strcmp(appInfo[1].app_name, "FIFA 16 Ultimate Team") == 0);
  mu_assert("TEST FAIL: verson for second app != '2.0'", strcmp(appInfo[1].version, "2.0") == 0);
  mu_assert("TEST FAIL: size for second app != 1.25", appInfo[1].size == (float)1.25);
  mu_assert("TEST FAIL: units for second app != 'GB'", strcmp(appInfo[1].units, "GB") == 0);
  mu_assert("TEST FAIL: price for second app != 0.00", appInfo[1].price == (float)0.00);
  return 0;
}

static char *test_find_missing_app_with_no_parsed_apps() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[2];
  parseApps(stream, appInfo, 2);
  fclose(stream);

  char obuffer[256] = {0};
  FILE *ostream = fmemopen(obuffer, 256, "w");
  queryAppStore(appInfo, 2, "find app foo", ostream);
  fflush(ostream);
  mu_assert("TEST FAIL: find missing app != 'Application foo not found<CR>'", strcmp(obuffer, "Application foo not found\n") == 0);
  fclose(ostream);
  return 0;
}

static char *test_find_app_prints_found_message() {
  char buffer[] = "Games\nMinecraft: Pocket Edition\n0.12.1\n24.1\nMB\n6.99\nGames\nFIFA 16 Ultimate Team\n2.0\n1.25\nGB\n0.00\n";
  FILE *stream = fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  struct app_info appInfo[2];
  parseApps(stream, appInfo, 2);
  fclose(stream);

  char obuffer[256] = {0};
  FILE *ostream = fmemopen(obuffer, 256, "w");
  queryAppStore(appInfo, 2, "find app Minecraft: Pocket Edition", ostream);
  fflush(ostream);

  mu_assert("TEST FAIL: find app first line != 'Found Application: Minecraft: Pocket Edition<CR>'", strcmp(obuffer, "Found Application: Minecraft: Pocket Edition\n") == 0);

  fclose(ostream);
  return 0;
}

static char *allTests() {
  mu_run_test(test_can_parse_count_for_one_category);
  mu_run_test(test_can_parse_count_for_two_category);
  mu_run_test(test_can_parse_three_category);
  mu_run_test(test_can_parse_count_for_one_app);
  mu_run_test(test_can_parse_count_for_two_apps);
  mu_run_test(test_can_parse_app_category);
  mu_run_test(test_can_parse_app_name);
  mu_run_test(test_can_parse_app_version);
  mu_run_test(test_can_parse_app_size);
  mu_run_test(test_can_parse_app_units);
  mu_run_test(test_can_parse_app_price);
  mu_run_test(test_can_parse_a_second_app);
  mu_run_test(test_find_missing_app_with_no_parsed_apps);
  mu_run_test(test_find_app_prints_found_message);
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
