#define _GNU_SOURCE

#include "myAppStore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

int parseNumberOfCategories(FILE *stream) {
  return parseInteger(stream);
}

void parseCategories(FILE *stream, struct categories categories[], int numberOfCategories) {
  for (int i = 0; i < numberOfCategories; i++) {
    parseString(stream, categories[i].category, CAT_NAME_LEN);
  }
}

int parseNumberOfApps(FILE *stream) {
  return parseInteger(stream);
}

void parseApps(FILE *stream, struct app_info appInfo[], int numberOfApps) {
  parseString(stream, appInfo[0].category, CAT_NAME_LEN);
  parseString(stream, appInfo[0].app_name, CAT_NAME_LEN);
  parseString(stream, appInfo[0].version, CAT_NAME_LEN);
  appInfo[0].size = parseFloat(stream);
}

