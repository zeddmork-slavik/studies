#ifndef PARSING_GREP_ARGV_H
#define PARSING_GREP_ARGV_H

#include "config.h"

void InitGrepConfig(GrepConfig* config);
void ParsingGrepArgv(int argc, char* argv[], GrepConfig* config);
void ProcessGrepFlag(char* argv, GrepConfig* config, bool* expect_pattern_file);
void OnFlagV(GrepConfig* config);
void OnFlagC(GrepConfig* config);
void OnFlagL(GrepConfig* config);
void OnFlagN(GrepConfig* config);
void OnFlagF(GrepConfig* config, char* argv, int index,
             bool* expect_pattern_file);
void OnFlagH(GrepConfig* config);
void OnFlagO(GrepConfig* config);
void ReadPatternsFromFile(GrepConfig* config, const char* filename);
void AddPattern(GrepConfig* config, char* pattern, bool need_free);
int ContainsEFlag(char* flag_str);

#endif