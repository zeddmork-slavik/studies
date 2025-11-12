#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "output.h"
#include "parsing_grep_argv.h"
#include "service_functions.h"
#include "task.h"

int main(int argc, char* argv[]) {
  GrepConfig config;
  InitGrepConfig(&config);

  ParsingGrepArgv(argc, argv, &config);

  if (!config.error) {
    CreateRegex(&config);
  }

  if (!config.error) {
    ProcessAllFiles(&config);
  }
  FreeRegex(&config);

  if (!config.error) {
    PrintResults(&config);
  }
  if (config.error) {
    PrintError(&config);
  }
  FreeMemory(&config);

  return 0;
}