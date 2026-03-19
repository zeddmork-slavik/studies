
#define _GNU_SOURCE
#include "task.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "service_functions.h"

void AddResult(GrepConfig* config, const char* filename, int line_number, const char* line, int start_pos,
               int end_pos, int match_count) {
    if (config->results == NULL) {
        config->results = UseMalloc(config->results_capacity * sizeof(GrepResult), config);
        if (!config->error) {
            config->need_free_results = true;
        }
    }

    if (config->results_count >= config->results_capacity) {
        config->results_capacity *= 2;
        config->results = UseRealloc(config->results, config->results_capacity * sizeof(GrepResult), config);
    }

    if (!config->error) {
        GrepResult* result = &config->results[config->results_count++];
        result->filename = filename;
        result->line_number = line_number;
        result->line = line;
        result->start_pos = start_pos;
        result->end_pos = end_pos;
        result->match_count = match_count;
    }
}

void CreateRegex(GrepConfig* config) {
    config->regexes = UseMalloc(config->pattern_count * sizeof(regex_t), config);

    if (!config->error) {
        config->need_free_regexes = true;
    }

    int flags = 0;
    if (config->i) {
        flags |= REG_ICASE;
    }

    for (int i = 0; i < config->pattern_count && !config->error; i++) {
        int result = regcomp(&config->regexes[i], config->patterns[i], flags);
        if (result != 0) {
            regerror(result, &config->regexes[i], config->error_context, sizeof(config->error_context));
            config->error = kGrepErrorInvalidRegex;
        }
    }
}

void ProcessAllFiles(GrepConfig* config) {
    for (int i = 0; i < config->file_count && !config->error; i++) {
        ProcessFile(config, config->files[i]);
    }
}

void FreeRegex(GrepConfig* config) {
    if (config->regexes != NULL) {
        for (int i = 0; i < config->pattern_count; i++) {
            regfree(&config->regexes[i]);
        }
    }
}

int SearchInLine(const char* line, GrepConfig* config) {
    int match_found = 0;
    int pattern = 0;
    while (pattern < config->pattern_count && !match_found) {
        if (SearchSinglePattern(line, &config->regexes[pattern])) {
            match_found = 1;
        }
        pattern++;
    }
    return match_found;
}

int SearchSinglePattern(const char* line, regex_t* regex) { return regexec(regex, line, 0, NULL, 0) == 0; }

void ProcessFile(GrepConfig* config, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        if (!config->s) {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "my_grep: %s: No such file or directory", filename);
            AddResult(config, filename, -1, error_msg, 0, 0, 0);
        }
    } else {
        char* original_line = NULL;
        size_t len = 0;
        ssize_t read;
        int line_number = 0;
        int match_count = 0;

        while ((read = getline(&original_line, &len, file)) != -1) {
            line_number++;
            if (read > 0 && original_line[read - 1] == '\n') {
                original_line[read - 1] = '\0';
            }
            char* line = strdup(original_line);
            ProcessLine(config, filename, line, line_number, &match_count);
        }
        free(original_line);
        fclose(file);
        if (config->l || config->c) {
            AddResult(config, filename, 0, NULL, 0, 0, match_count);
        }
    }
}

void ProcessLine(GrepConfig* config, const char* filename, char* line, int line_number, int* match_count) {
    bool line_used = false;
    if (config->o) {
        line_used = FindAllMatches(config, filename, line_number, line);
    } else {
        int match_found = SearchInLine(line, config);
        if (match_found) {
        }
        if ((match_found && !config->v) || (!match_found && config->v)) {
            AddResult(config, filename, line_number, line, 0, 0, match_found ? 1 : 0);
            line_used = true;
            (*match_count)++;
        }
    }
    if (!line_used) {
        free(line);
    }
}

bool FindAllMatches(GrepConfig* config, const char* filename, int line_number, const char* line) {
    int matches_capacity = 16;
    int match_count = 0;
    MatchInfo* matches = UseMalloc(matches_capacity * sizeof(MatchInfo), config);
    bool has_matches = false;
    if (!config->error) {
        for (int i = 0; i < config->pattern_count && !config->error; i++) {
            FindPatternMatchesForO(config, line, i, matches, &match_count, &matches_capacity);
        }

        if (!config->error && match_count > 0) {
            has_matches = true;
            SortMatchesByPosition(matches, match_count);
            for (int j = 0; j < match_count; j++) {
                AddResult(config, filename, line_number, line, matches[j].start_pos, matches[j].end_pos,
                          match_count);
            }
        }
    }

    if (matches != NULL) {
        free(matches);
    }
    return has_matches;
}

void FindPatternMatchesForO(GrepConfig* config, const char* line, int pattern_index, MatchInfo* matches,
                            int* match_count, int* matches_capacity) {
    regmatch_t match;
    const char* search_start = line;

    while (regexec(&config->regexes[pattern_index], search_start, 1, &match, 0) == 0) {
        if (match.rm_so != -1) {
            matches[*match_count].start_pos = (search_start - line) + match.rm_so;
            matches[*match_count].end_pos = (search_start - line) + match.rm_eo;
            matches[*match_count].pattern_index = pattern_index;
            (*match_count)++;

            if (*match_count >= *matches_capacity && !config->error) {
                (*matches_capacity) *= 2;
                matches = UseRealloc(matches, (*matches_capacity) * sizeof(MatchInfo), config);
            }
        }

        search_start += match.rm_eo;
        if (match.rm_so == match.rm_eo) {
            search_start++;
        }
    }
}

int CompareMatches(const void* a, const void* b) {
    const MatchInfo* match_a = (const MatchInfo*)a;
    const MatchInfo* match_b = (const MatchInfo*)b;
    return match_a->start_pos - match_b->start_pos;
}

void SortMatchesByPosition(MatchInfo* matches, int match_count) {
    qsort(matches, match_count, sizeof(MatchInfo), CompareMatches);
}