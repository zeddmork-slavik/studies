#ifndef OUTPUT_H
#define OUTPUT_H

#include "config.h"

typedef enum {
  PRINT_NAMES_ONLY,
  PRINT_COUNTS_ONLY,
  PRINT_NAMES_AND_COUNTS
} PrintMode;

void PrintError(GrepConfig* config);
void PrintErrorInvalidRegex(GrepConfig* config);
void PrintResults(GrepConfig* config);
void PrintByFiles(GrepConfig* config, int mode);
void PrintLineByLine(GrepConfig* config);
void PrintNoSuchFile(GrepResult* result);
#endif