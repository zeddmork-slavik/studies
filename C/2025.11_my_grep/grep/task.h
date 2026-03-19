#ifndef TASK_H
#define TASK_H

#include <regex.h>
#include <stdbool.h>

#include "config.h"

void AddResult(GrepConfig* config, const char* filename, int line_number,
               const char* line, int start_pos, int end_pos, int match_count);
void CreateRegex(GrepConfig* config);
void ProcessAllFiles(GrepConfig* config);
void FreeRegex(GrepConfig* config);
int SearchInLine(const char* line, GrepConfig* config);
int SearchSinglePattern(const char* line, regex_t* regex);
void ProcessFile(GrepConfig* config, char* filename);
void ProcessLine(GrepConfig* config, const char* filename, char* line,
                 int line_number, int* match_count);
bool FindAllMatches(GrepConfig* config, const char* filename, int line_number,
                    const char* line);
void FindPatternMatchesForO(GrepConfig* config, const char* line,
                            int pattern_index, MatchInfo* matches,
                            int* match_count, int* matches_capacity);
int CompareMatches(const void* a, const void* b);
void SortMatchesByPosition(MatchInfo* matches, int match_count);
#endif