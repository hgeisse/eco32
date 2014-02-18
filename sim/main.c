/*
 * main.c -- ECO32 simulator
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "command.h"
#include "instr.h"
#include "cpu.h"
#include "mmu.h"
#include "memory.h"
#include "timer.h"
#include "dspkbd.h"
#include "term.h"
#include "disk.h"
#include "output.h"
#include "shutdown.h"
#include "graph.h"


static void usage(char *myself) {
  fprintf(stderr, "Usage: %s\n", myself);
  fprintf(stderr, "         [-i]             set interactive mode\n");
  fprintf(stderr, "         [-m <n>]         install n MB of RAM (1-%d)\n",
          RAM_SIZE_MAX / M);
  fprintf(stderr, "         [-l <prog>]      set program file name\n");
  fprintf(stderr, "         [-r <rom>]       set ROM image file name\n");
  fprintf(stderr, "         [-d <disk>]      set disk image file name\n");
  fprintf(stderr, "         [-t <n>]         connect n terminals (0-%d)\n",
          MAX_NTERMS);
  fprintf(stderr, "         [-g]             install graphics controller\n");
  fprintf(stderr, "         [-c]             install console\n");
  fprintf(stderr, "         [-o <file>]      bind output device to file\n");
  fprintf(stderr, "The options -l and -r are mutually exclusive.\n");
  fprintf(stderr, "If both are omitted, interactive mode is assumed.\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i;
  char *argp;
  char *endp;
  Bool interactive;
  int memSize;
  char *progName;
  char *romName;
  char *diskName;
  int numTerms;
  Bool graphics;
  Bool console;
  char *outputName;
  Word initialPC;
  char command[20];
  char *line;

  interactive = false;
  memSize = RAM_SIZE_DFL / M;
  progName = NULL;
  romName = NULL;
  diskName = NULL;
  numTerms = 0;
  graphics = false;
  console = false;
  outputName = NULL;
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (*argp != '-') {
      usage(argv[0]);
    }
    argp++;
    switch (*argp) {
      case 'i':
        interactive = true;
        break;
      case 'm':
        if (i == argc - 1) {
          usage(argv[0]);
        }
        memSize = strtol(argv[++i], &endp, 10);
        if (*endp != '\0' ||
            memSize <= 0 ||
            memSize > RAM_SIZE_MAX / M) {
          usage(argv[0]);
        }
        break;
      case 'l':
        if (i == argc - 1 || progName != NULL || romName != NULL) {
          usage(argv[0]);
        }
        progName = argv[++i];
        break;
      case 'r':
        if (i == argc - 1 || romName != NULL || progName != NULL) {
          usage(argv[0]);
        }
        romName = argv[++i];
        break;
      case 'd':
        if (i == argc - 1 || diskName != NULL) {
          usage(argv[0]);
        }
        diskName = argv[++i];
        break;
      case 't':
        if (i == argc - 1) {
          usage(argv[0]);
        }
        numTerms = strtol(argv[++i], &endp, 10);
        if (*endp != '\0' ||
            numTerms < 0 ||
            numTerms > MAX_NTERMS) {
          usage(argv[0]);
        }
        break;
      case 'g':
        graphics = true;
        break;
      case 'c':
        console = true;
        break;
      case 'o':
        if (i == argc - 1 || outputName != NULL) {
          usage(argv[0]);
        }
        outputName = argv[++i];
        break;
      default:
        usage(argv[0]);
    }
  }
  cInit();
  cPrintf("ECO32 Simulator started\n");
  if (progName == NULL && romName == NULL && !interactive) {
    cPrintf("Neither a program to load nor a system ROM was\n");
    cPrintf("specified, so interactive mode is assumed.\n");
    interactive = true;
  }
  initInstrTable();
  timerInit();
  if (console) {
    displayInit();
    keyboardInit();
  }
  termInit(numTerms);
  diskInit(diskName);
  outputInit(outputName);
  shutdownInit();
  if (graphics) {
    graphInit();
  }
  memoryInit(memSize * M, progName, romName);
  mmuInit();
  if (progName != NULL) {
    initialPC = 0xC0000000;
  } else {
    initialPC = 0xC0000000 | ROM_BASE;
  }
  cpuInit(initialPC);
  if (!interactive) {
    cPrintf("Start executing...\n");
    strcpy(command, "c\n");
    execCommand(command);
  } else {
    while (1) {
      line = cGetLine("ECO32 > ");
      if (*line == '\0') {
        break;
      }
      cAddHist(line);
      if (execCommand(line)) {
        break;
      }
    }
  }
  cpuExit();
  mmuExit();
  memoryExit();
  timerExit();
  displayExit();
  keyboardExit();
  termExit();
  diskExit();
  outputExit();
  shutdownExit();
  graphExit();
  cPrintf("ECO32 Simulator finished\n");
  cExit();
  return 0;
}
