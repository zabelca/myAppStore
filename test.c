#define _GNU_SOURCE

#include "myAppStore.h"

#include <stdio.h>
#include <string.h>
#include "minunit.h"
 
int tests_run = 0;

static char *test_can_parse_count_for_one_category() {
  char buffer[] = "1\nGames";
  FILE *stream;
  stream = (FILE*)fmemopen(buffer, sizeof(buffer) * sizeof(char), "r");
  int categoriesCount = parseNumberOfCategories(stream);
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

static char *allTests() {
  mu_run_test(test_can_parse_count_for_one_category);
  mu_run_test(test_can_parse_count_for_two_category);
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
