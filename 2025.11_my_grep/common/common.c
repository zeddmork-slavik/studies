#include "common.h"

#include <stdio.h>

void PrintErrorInvalidFlag(int is_grep, char flag) {
    fprintf(stderr, "%s: invalid option -- '%c'\n", is_grep ? "grep" : "cat", flag);
}

void PrintErrorFileNotFound(int is_grep, char* filename) {
    fprintf(stderr, "%s: %s: No such file or directory\n", is_grep ? "grep" : "cat", filename);
}
