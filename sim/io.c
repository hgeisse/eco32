/*
 * io.c -- I/O decoder
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "io.h"
#include "timer.h"
#include "dsp.h"
#include "kbd.h"
#include "serial.h"
#include "disk.h"
#include "sdcard.h"
#include "output.h"
#include "shutdown.h"
#include "graph1.h"
#include "graph2.h"
#include "mouse.h"


Word ioReadWord(Word pAddr) {
  Word data;
  int dev;

  if ((pAddr & IO_DEV_MASK) == TIMER_BASE) {
    data = timerRead(pAddr & IO_REG_MASK);
    return data;
  }
  if ((pAddr & IO_DEV_MASK) == DISPLAY_BASE) {
    data = displayRead(pAddr & IO_REG_MASK);
    return data;
  }
  if ((pAddr & IO_DEV_MASK) == PS2DEV_BASE) {
    dev = (pAddr & IO_REG_MASK) >> 12;
    if (dev == 0) {
      data = keyboardRead(pAddr & 0x0FFF);
      return data;
    }
    if (dev == 1) {
      data = mouseRead(pAddr & 0x0FFF);
      return data;
    }
    /* throw bus timeout exception */
    throwException(EXC_BUS_TIMEOUT);
    /* not reached */
  }
  if ((pAddr & IO_DEV_MASK) == SERIAL_BASE) {
    data = serialRead(pAddr & IO_REG_MASK);
    return data;
  }
  if ((pAddr & IO_DEV_MASK) == DISK_BASE) {
    data = diskRead(pAddr & IO_REG_MASK);
    return data;
  }
  if ((pAddr & IO_DEV_MASK) == SDCARD_BASE) {
    data = sdcardRead(pAddr & IO_REG_MASK);
    return data;
  }
  if ((pAddr & IO_GRDEV_MASK) == GRAPH1_BASE) {
    data = graph1Read(pAddr & IO_GRBUF_MASK);
    return data;
  }
  if ((pAddr & IO_GRDEV_MASK) == GRAPH2_BASE) {
    data = graph2Read(pAddr & IO_GRBUF_MASK);
    return data;
  }
  if ((pAddr & IO_DEV_MASK) == OUTPUT_BASE) {
    data = outputRead(pAddr & IO_REG_MASK);
    return data;
  }
  if ((pAddr & IO_DEV_MASK) == SHUTDOWN_BASE) {
    data = shutdownRead(pAddr & IO_REG_MASK);
    return data;
  }
  /* throw bus timeout exception */
  throwException(EXC_BUS_TIMEOUT);
  /* not reached */
  data = 0;
  return data;
}


void ioWriteWord(Word pAddr, Word data) {
  int dev;

  if ((pAddr & IO_DEV_MASK) == TIMER_BASE) {
    timerWrite(pAddr & IO_REG_MASK, data);
    return;
  }
  if ((pAddr & IO_DEV_MASK) == DISPLAY_BASE) {
    displayWrite(pAddr & IO_REG_MASK, data);
    return;
  }
  if ((pAddr & IO_DEV_MASK) == PS2DEV_BASE) {
    dev = (pAddr & IO_REG_MASK) >> 12;
    if (dev == 0) {
      keyboardWrite(pAddr & 0x0FFF, data);
      return;
    }
    if (dev == 1) {
      mouseWrite(pAddr & 0x0FFF, data);
      return;
    }
    /* throw bus timeout exception */
    throwException(EXC_BUS_TIMEOUT);
    /* not reached */
  }
  if ((pAddr & IO_DEV_MASK) == SERIAL_BASE) {
    serialWrite(pAddr & IO_REG_MASK, data);
    return;
  }
  if ((pAddr & IO_DEV_MASK) == DISK_BASE) {
    diskWrite(pAddr & IO_REG_MASK, data);
    return;
  }
  if ((pAddr & IO_DEV_MASK) == SDCARD_BASE) {
    sdcardWrite(pAddr & IO_REG_MASK, data);
    return;
  }
  if ((pAddr & IO_GRDEV_MASK) == GRAPH1_BASE) {
    graph1Write(pAddr & IO_GRBUF_MASK, data);
    return;
  }
  if ((pAddr & IO_GRDEV_MASK) == GRAPH2_BASE) {
    graph2Write(pAddr & IO_GRBUF_MASK, data);
    return;
  }
  if ((pAddr & IO_DEV_MASK) == OUTPUT_BASE) {
    outputWrite(pAddr & IO_REG_MASK, data);
    return;
  }
  if ((pAddr & IO_DEV_MASK) == SHUTDOWN_BASE) {
    shutdownWrite(pAddr & IO_REG_MASK, data);
    return;
  }
  /* throw bus timeout exception */
  throwException(EXC_BUS_TIMEOUT);
  /* not reached */
}
