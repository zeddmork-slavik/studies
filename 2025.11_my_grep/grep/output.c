#include "output.h"

#include <stdio.h>

#include "common.h"
#include "config.h"

void PrintError(GrepConfig* config) {
    if (config->error == kGrepErrorInvalidFlag && !config->s) {
        PrintErrorInvalidFlag(kIsGrep, config->error_context[0]);
    }

    switch (config->error) {
        case kGrepErrorMemory: {
            fprintf(stderr, "my_grep: error memory'\n");
        } break;
        case kGrepErrorInvalidFileWithPatterns: {
            PrintErrorFileNotFound(kIsGrep, config->error_context);
        } break;
        case kGrepErrorInvalidRegex: {
            PrintErrorInvalidRegex(config);
        } break;
        default:
            break;
    }
}

void PrintErrorInvalidRegex(GrepConfig* config) {
    fprintf(stderr, "my_grep: invalid regular expression: %s\n",
            config->pattern_count == 0 ? "patterns are missing" : config->error_context);
}

void PrintResults(GrepConfig* config) {
    if (config->l) {
        PrintByFiles(config, PRINT_NAMES_ONLY);
    } else if (config->c) {
        if (config->h) {
            PrintByFiles(config, PRINT_COUNTS_ONLY);
        } else {
            PrintByFiles(config, PRINT_NAMES_AND_COUNTS);
        }
    } else {
        PrintLineByLine(config);
    }
}

void PrintByFiles(GrepConfig* config, int mode) {
    for (int i = 0; i < config->results_count; i++) {
        GrepResult* result = &config->results[i];

        PrintNoSuchFile(result);

        if (result->line_number == 0) {
            if (mode == 0 && result->match_count > 0) {
                printf("%s\n", result->filename);
            } else if (mode == 1) {
                printf("%d\n", result->match_count);
            } else if (mode == 2) {
                printf("%s:%d\n", result->filename, result->match_count);
            }
        }
    }
}

void PrintLineByLine(GrepConfig* config) {
    for (int i = 0; i < config->results_count; i++) {
        GrepResult* result = &config->results[i];

        PrintNoSuchFile(result);

        if (result->line_number > 0) {
            if (!config->h) {
                fputs(result->filename, stdout);
                putchar(':');
            }
            if (config->n) {
                printf("%d:", result->line_number);
            }
            if (config->o) {
                printf("%.*s", result->end_pos - result->start_pos, result->line + result->start_pos);
            } else {
                printf("%s", result->line);
            }
            putchar('\n');
        }
    }
}

void PrintNoSuchFile(GrepResult* result) {
    if (result->line_number == -1) {
        printf("%s\n", result->line);
    }
}