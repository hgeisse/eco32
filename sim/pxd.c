/*
 * pxd.c -- packet exchange device
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "pxd.h"


static Bool debug = true;

static Bool installed = false;


/**************************************************************/


Word pxdRead(Word addr) {
  Word data;

  if (debug) {
    cPrintf("\n**** DISK READ from 0x%08X", addr);
  }
  if (!installed) {
    /* package exchange device controller not installed */
    throwException(EXC_BUS_TIMEOUT);
  }
  data = 0;  /* !!!!! to be implemented */
  if (debug) {
    cPrintf(", data = 0x%08X ****\n", data);
  }
  return data;
}


void pxdWrite(Word addr, Word data) {
  if (debug) {
    cPrintf("\n**** DISK WRITE to 0x%08X, data = 0x%08X ****\n",
            addr, data);
  }
  if (!installed) {
    /* package exchange device controller not installed */
    throwException(EXC_BUS_TIMEOUT);
  }
  /* !!!!! to be implemented */
}


void pxdReset(void) {
  if (!installed) {
    /* packet exchange device not installed */
    return;
  }
  cPrintf("Resetting Packet Exchange Device...\n");
}


void pxdInit(void) {
  installed = true;
  pxdReset();
}


void pxdExit(void) {
  if (!installed) {
    /* packet exchange device not installed */
    return;
  }
}
