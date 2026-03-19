#define _GNU_SOURCE

#include "parsing_grep_argv.h"

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "service_functions.h"

void InitGrepConfig(GrepConfig* config) {
  config->i = false;
  config->v = false;
  config->c = false;
  config->l = false;
  config->n = false;
  config->h = false;
  config->s = false;
  config->o = false;
  config->patterns = NULL;
  config->pattern_count = 0;
  config->patterns_capacity = 0;
  config->patterns_need_free = NULL;
  config->files = NULL;
  config->file_count = 0;
  config->patterns_capacity = 4;
  config->regexes = NULL;
  config->error = 0;
  config->error_context[0] = '\0';
  config->need_free_files = false;
  config->need_free_patterns = false;
  config->need_free_regexes = false;
  config->need_free_results = false;
  config->results = NULL;
  config->results_count = 0;
  config->results_capacity = 16;
}

void ParsingGrepArgv(int argc, char* argv[], GrepConfig* config) {
  config->files = UseMalloc((argc - 1) * sizeof(char*), config);
  if (!config->error) {
    config->need_free_files = true;
  }
  int pattern_founded = 0;
  bool expect_pattern = false;
  bool expect_pattern_file = false;
  for (int i = 1; i < argc && !config->error; i++) {
    if (expect_pattern_file) {
      ReadPatternsFromFile(config, argv[i]);
      expect_pattern_file = false;
      pattern_founded = 1;
    } else if (expect_pattern) {
      AddPattern(config, argv[i], false);
      pattern_founded = 1;
      expect_pattern = false;
    } else if (argv[i][0] == '-') {
      ProcessGrepFlag(argv[i], config, &expect_pattern_file);
      if (ContainsEFlag(argv[i])) {
        expect_pattern = true;
      }
    } else {
      if (!pattern_founded) {
        AddPattern(config, argv[i], false);
        pattern_founded = 1;
      } else {
        config->files[config->file_count++] = argv[i];
      }
    }
  }
  if (config->pattern_count == 0) {
    config->error = kGrepErrorInvalidRegex;
  }
  if (config->file_count == 1) {
    config->h = true;
  }
}

void ProcessGrepFlag(char* argv, GrepConfig* config,
                     bool* expect_pattern_file) {
  for (int j = 1; argv[j] != '\0'; j++) {
    switch (argv[j]) {
      case 'e':
        // empty
        break;
      case 'i':
        config->i = true;
        break;
      case 'v':
        OnFlagV(config);
        break;
      case 'c':
        OnFlagC(config);
        break;
      case 'l':
        OnFlagL(config);
        break;
      case 'n':
        OnFlagN(config);
        break;
      case 'h':
        OnFlagH(config);
        break;
      case 's':
        config->s = true;
        break;
      case 'f':
        OnFlagF(config, argv, j + 1, expect_pattern_file);
        break;
      case 'o':
        OnFlagO(config);
        break;
      default:
        config->error = kGrepErrorInvalidFlag;
        if (!config->s) {
          config->error_context[0] = argv[j];
          config->error_context[1] = '\0';
        }
        break;
    }
  }
}

void OnFlagV(GrepConfig* config) {
  if (config->o) {
    config->error = kGrepErrorEmptyOutput;
  } else {
    config->v = true;
  }
}

void OnFlagC(GrepConfig* config) {
  if (!config->l) {
    config->c = true;
    config->n = false;
    config->o = false;
  }
}

void OnFlagL(GrepConfig* config) {
  config->l = true;
  config->c = false;
  config->n = false;
  config->h = false;
  config->o = false;
}

void OnFlagN(GrepConfig* config) {
  if (!config->l && !config->c) config->n = true;
}

void OnFlagH(GrepConfig* config) {
  if (!config->l) config->h = true;
}

void OnFlagF(GrepConfig* config, char* argv, int index,
             bool* expect_pattern_file) {
  if (argv[index] != '\0') {
    ReadPatternsFromFile(config, &argv[index]);
  } else {
    *expect_pattern_file = true;
  }
}

void OnFlagO(GrepConfig* config) {
  if (config->v) {
    config->error = kGrepErrorEmptyOutput;
  } else {
    if (!config->l && !config->c) {
      config->o = true;
    }
  }
}

void ReadPatternsFromFile(GrepConfig* config, const char* filename) {
  FILE* file = fopen(filename, "r");
  if (file) {
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
      line[strcspn(line, "\n")] = '\0';
      if (strlen(line) > 0) {
        char* pattern_copy = strdup(line);
        AddPattern(config, pattern_copy, true);
      }
    }
    fclose(file);
  } else {
    config->error = kGrepErrorInvalidFileWithPatterns;
    strncpy(config->error_context, filename, sizeof(config->error_context) - 1);
    config->error_context[sizeof(config->error_context) - 1] = '\0';
  }
}

void AddPattern(GrepConfig* config, char* pattern, bool need_free) {
  if (config->patterns == NULL) {
    config->patterns =
        UseMalloc((config->patterns_capacity * sizeof(char*)), config);
    config->patterns_need_free =
        UseMalloc(config->patterns_capacity * sizeof(bool), config);
    if (!config->error) {
      config->need_free_patterns = true;
    }
  }
  if (config->pattern_count >= config->patterns_capacity) {
    config->patterns_capacity *= 2;
    config->patterns = UseRealloc(
        config->patterns, config->patterns_capacity * sizeof(char*), config);
    config->patterns_need_free =
        UseRealloc(config->patterns_need_free,
                   config->patterns_capacity * sizeof(bool), config);
  }
  if (!config->error) {
    config->patterns[config->pattern_count] = pattern;
    config->patterns_need_free[config->pattern_count] = need_free;
    config->pattern_count++;
  }
}

int ContainsEFlag(char* flag_str) {
  int length = strlen(flag_str);
  return (length > 1 && flag_str[length - 1] == 'e');
}
