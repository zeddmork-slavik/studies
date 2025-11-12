#ifndef SERVICE_FUNCTIONS_H
#define SERVICE_FUNCTIONS_H

#include "config.h"

void* UseMalloc(size_t size, GrepConfig* config);
void* UseRealloc(void* ptr, size_t size, GrepConfig* config);
void FreeMemory(GrepConfig* config);

#endif