#ifndef COMMON_H
#define COMMON_H

enum { kIsGrep = 1, kIsCat = 0 };

void PrintErrorInvalidFlag(int is_grep, char flag);
void PrintErrorFileNotFound(int is_grep, char *filename);
#endif