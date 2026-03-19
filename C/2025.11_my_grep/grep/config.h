#ifndef CONFIG_H
#define CONFIG_H

#include <regex.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  int start_pos;
  int end_pos;
  int pattern_index;
} MatchInfo;

typedef struct {
  const char *filename;
  int line_number;
  const char *line;
  int match_count;
  int start_pos;
  int end_pos;
} GrepResult;

typedef enum {
  kGrepSucess = 0,
  kGrepErrorMemory = 1,
  kGrepErrorInvalidFlag = 2,
  kGrepErrorInvalidFileWithPatterns = 3,
  kGrepErrorInvalidRegex = 4,
  kGrepErrorEmptyOutput = 5
} GrepErrorType;

typedef struct GrepConfig {
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool o;
  char **patterns;
  int pattern_count;
  int patterns_capacity;
  bool *patterns_need_free;
  char **files;
  int file_count;
  int files_capacity;
  regex_t *regexes;
  GrepErrorType error;
  char error_context[256];
  bool need_free_patterns;
  bool need_free_files;
  bool need_free_regexes;
  bool need_free_results;
  GrepResult *results;
  int results_count;
  int results_capacity;
} GrepConfig;

#endif