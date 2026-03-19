#include "service_functions.h"

#include <stdlib.h>

void *UseMalloc(size_t size, GrepConfig *config) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    config->error = kGrepErrorMemory;
  }
  return ptr;
}

void *UseRealloc(void *ptr, size_t size, GrepConfig *config) {
  void *new_ptr = realloc(ptr, size);
  if (new_ptr == NULL) {
    config->error = kGrepErrorMemory;
  }
  return new_ptr;
}

void FreeMemory(GrepConfig *config) {
  if (config->need_free_patterns) {
    for (int i = 0; i < config->pattern_count; i++) {
      if (config->patterns_need_free[i]) {
        free(config->patterns[i]);
      }
    }
    free(config->patterns);
    free(config->patterns_need_free);
  }
  if (config->need_free_files) {
    free(config->files);
  }
  if (config->need_free_regexes) {
    free(config->regexes);
  }
  if (config->need_free_results) {
    for (int i = 0; i < config->results_count; i++) {
      if (config->results[i].line != NULL &&
          config->results[i].line_number > 0) {
        free((char *)config->results[i].line);
      }
    }
    free(config->results);
  }
}
