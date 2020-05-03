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
#include "fpu.h"
#include "trace.h"
#include "mmu.h"
#include "icache.h"
#include "dcache.h"
#include "ram.h"
#include "rom.h"
#include "timer.h"
#include "dsp.h"
#include "kbd.h"
#include "serial.h"
#include "disk.h"
#include "sdcard.h"
#include "bio.h"
#include "output.h"
#include "shutdown.h"
#include "graph1.h"
#include "graph2.h"
#include "mouse.h"


static void usage(char *myself) {
  fprintf(stderr, "Usage: %s\n", myself);
  fprintf(stderr, "    [-i]           set interactive mode\n");
  fprintf(stderr, "    [-m <n>]       install n MB of RAM (1-%d)\n",
          RAM_SIZE_MAX / M);
  fprintf(stderr, "    [-l <prog>]    set program file name\n");
  fprintf(stderr, "    [-a <addr>]    set program load address\n");
  fprintf(stderr, "    [-r <rom>]     set ROM image file name\n");
  fprintf(stderr, "    [-d <disk>]    set disk image file name\n");
  fprintf(stderr, "    [-d NONE]      no disk, install controller only\n");
  fprintf(stderr, "    [-D <sdcard>]  set SD card image file name\n");
  fprintf(stderr, "    [-D NONE]      no SD card, install controller only\n");
  fprintf(stderr, "    [-s <n>]       install n serial lines (0-%d)\n",
          MAX_NSERIALS);
  fprintf(stderr, "    [-t <k>]       connect terminal to line k (0-%d)\n",
          MAX_NSERIALS - 1);
  fprintf(stderr, "    [-g]           install graphics card 640x480x32\n");
  fprintf(stderr, "    [-G]           install graphics card 1024x768x1\n");
  fprintf(stderr, "    [-c]           install console\n");
  fprintf(stderr, "    [-o <file>]    bind output device to file\n");
  fprintf(stderr, "    [-x]           use simulator with DejaGnu/expect\n");
  fprintf(stderr, "    [-ics <n>]     icache ld size in bytes (2-28)\n");
  fprintf(stderr, "    [-icl <n>]     icache ld line size in bytes (2-10)\n");
  fprintf(stderr, "    [-ica <n>]     icache ld associativity (0-1)\n");
  fprintf(stderr, "    [-dcs <n>]     dcache ld size in bytes (2-28)\n");
  fprintf(stderr, "    [-dcl <n>]     dcache ld line size in bytes (2-10)\n");
  fprintf(stderr, "    [-dca <n>]     dcache ld associativity (0-1)\n");
  fprintf(stderr, "    [-sb <3 hex>]  set board buttons(1)/switches(2)\n");
  fprintf(stderr, "The options -l and -r are mutually exclusive.\n");
  fprintf(stderr, "If both are omitted, interactive mode is assumed.\n");
  fprintf(stderr, "Unconnected serial lines can be accessed by opening\n");
  fprintf(stderr, "their corresponding pseudo terminal (path is shown).\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  int i, j;
  char *argp;
  char *endp;
  Bool interactive;
  int memSize;
  char *progName;
  unsigned int loadAddr;
  char *romName;
  Bool disk;
  char *diskName;
  Bool sdcard;
  char *sdcardName;
  int numSerials;
  Bool connectTerminals[MAX_NSERIALS];
  Bool graphics1;
  Bool graphics2;
  Bool console;
  char *outputName;
  Bool expect;
  int icacheTotalSize;
  int icacheLineSize;
  int icacheAssoc;
  int dcacheTotalSize;
  int dcacheLineSize;
  int dcacheAssoc;
  Word initialSwitches;
  Word initialPC;
  char command[20];
  char *line;

  interactive = false;
  memSize = RAM_SIZE_DFL / M;
  progName = NULL;
  loadAddr = 0;
  romName = NULL;
  disk = false;
  diskName = NULL;
  sdcard = false;
  sdcardName = NULL;
  numSerials = 0;
  for (j = 0; j < MAX_NSERIALS; j++) {
    connectTerminals[j] = false;
  }
  graphics1 = false;
  graphics2 = false;
  console = false;
  outputName = NULL;
  expect = false;
  icacheTotalSize = IC_LD_TOTAL_SIZE;
  icacheLineSize = IC_LD_LINE_SIZE;
  icacheAssoc = IC_LD_ASSOC;
  dcacheTotalSize = DC_LD_TOTAL_SIZE;
  dcacheLineSize = DC_LD_LINE_SIZE;
  dcacheAssoc = DC_LD_ASSOC;
  initialSwitches = 0;
  for (i = 1; i < argc; i++) {
    argp = argv[i];
    if (strcmp(argp, "-i") == 0) {
      interactive = true;
    } else
    if (strcmp(argp, "-m") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      memSize = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          memSize <= 0 ||
          memSize > RAM_SIZE_MAX / M) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-l") == 0) {
      if (i == argc - 1 || progName != NULL || romName != NULL) {
        usage(argv[0]);
      }
      progName = argv[++i];
    } else
    if (strcmp(argp, "-a") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      loadAddr = strtoul(argv[++i], &endp, 0);
      if (*endp != '\0') {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-r") == 0) {
      if (i == argc - 1 || romName != NULL || progName != NULL) {
        usage(argv[0]);
      }
      romName = argv[++i];
    } else
    if (strcmp(argp, "-d") == 0) {
      if (i == argc - 1 || disk) {
        usage(argv[0]);
      }
      disk = true;
      diskName = argv[++i];
      if (strcmp(diskName, "NONE") == 0) {
        diskName = NULL;
      }
    } else
    if (strcmp(argp, "-D") == 0) {
      if (i == argc - 1 || sdcard) {
        usage(argv[0]);
      }
      sdcard = true;
      sdcardName = argv[++i];
      if (strcmp(sdcardName, "NONE") == 0) {
        sdcardName = NULL;
      }
    } else
    if (strcmp(argp, "-s") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      numSerials = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          numSerials < 0 ||
          numSerials > MAX_NSERIALS) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-t") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      j = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          j < 0 ||
          j > MAX_NSERIALS - 1) {
        usage(argv[0]);
      }
      connectTerminals[j] = true;
    } else
    if (strcmp(argp, "-g") == 0) {
      graphics1 = true;
    } else
    if (strcmp(argp, "-G") == 0) {
      graphics2 = true;
    } else
    if (strcmp(argp, "-c") == 0) {
      console = true;
    } else
    if (strcmp(argp, "-o") == 0) {
      if (i == argc - 1 || outputName != NULL) {
        usage(argv[0]);
      }
      outputName = argv[++i];
    } else
    if (strcmp(argp, "-x") == 0) {
      expect = true;
    } else
    if (strcmp(argp, "-ics") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      icacheTotalSize = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          icacheTotalSize < 2 ||
          icacheTotalSize > 28) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-icl") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      icacheLineSize = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          icacheLineSize < 2 ||
          icacheLineSize > 10) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-ica") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      icacheAssoc = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          icacheAssoc < 0 ||
          icacheAssoc > 1) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-dcs") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      dcacheTotalSize = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          dcacheTotalSize < 2 ||
          dcacheTotalSize > 28) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-dcl") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      dcacheLineSize = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          dcacheLineSize < 2 ||
          dcacheLineSize > 10) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-dca") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      dcacheAssoc = strtol(argv[++i], &endp, 10);
      if (*endp != '\0' ||
          dcacheAssoc < 0 ||
          dcacheAssoc > 1) {
        usage(argv[0]);
      }
    } else
    if (strcmp(argp, "-sb") == 0) {
      if (i == argc - 1) {
        usage(argv[0]);
      }
      initialSwitches = strtoul(argv[++i], &endp, 16);
      if (*endp != '\0' ||
          (initialSwitches & ~0xFFF) != 0) {
        usage(argv[0]);
      }
    } else {
      usage(argv[0]);
    }
  }
  cInit(expect);
  cPrintf("ECO32 Simulator started\n");
  if (progName == NULL && romName == NULL && !interactive) {
    cPrintf("Neither a program to load nor a system ROM was\n");
    cPrintf("specified, so interactive mode is assumed.\n");
    interactive = true;
  }
  for (j = MAX_NSERIALS - 1; j >= 0; j--) {
    if (connectTerminals[j] && j >= numSerials) {
      /* user wants a terminal on a line which is not installed */
      numSerials = j + 1;
      cPrintf("Serial lines 0..%d automatically installed.\n", j);
      break;
    }
  }
  initInstrTable();
  timerInit();
  if (console) {
    displayInit();
  }
  if (graphics1) {
    graph1Init();
  }
  if (graphics2) {
    graph2Init();
  }
  if (console || graphics1 || graphics2) {
    keyboardInit();
  }
  if (graphics1 || graphics2) {
    mouseInit();
  }
  serialInit(numSerials, connectTerminals, expect);
  if (disk) {
    diskInit(diskName);
  }
  if (sdcard) {
    sdcardInit(sdcardName);
  }
  bioInit(initialSwitches);
  outputInit(outputName);
  shutdownInit();
  ramInit(memSize * M, progName, loadAddr);
  romInit(romName);
  icacheInit(icacheTotalSize, icacheLineSize, icacheAssoc);
  dcacheInit(dcacheTotalSize, dcacheLineSize, dcacheAssoc);
  mmuInit();
  traceInit();
  fpuInit();
  if (progName != NULL) {
    initialPC = 0xC0000000 | loadAddr;
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
  fpuExit();
  traceExit();
  mmuExit();
  icacheExit();
  dcacheExit();
  ramExit();
  romExit();
  timerExit();
  displayExit();
  graph1Exit();
  graph2Exit();
  keyboardExit();
  mouseExit();
  serialExit();
  diskExit();
  sdcardExit();
  bioExit();
  outputExit();
  shutdownExit();
  cPrintf("ECO32 Simulator finished\n");
  cExit();
  return 0;
}
