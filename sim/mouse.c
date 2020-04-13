/*
 * mouse.c -- mouse simulation
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <pthread.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "cpu.h"
#include "timer.h"
#include "mouse.h"


#define COMMAND_RESET		0xFF
#define COMMAND_DISABLE_DATA	0xF5
#define COMMAND_ENABLE_DATA	0xF4

#define ANSWER_ACK		0xFA
#define ANSWER_BATOK		0xAA

#define DEVICE_ID		0x00


static Bool debug = false;
static Bool debugMove = false;
static Bool debugButtons = false;
static Bool installed = false;
static Bool mouseDataReporting = false;

static pthread_mutex_t lock;


/**************************************************************/


#define MOUSE_BUF_MAX		3000
#define MOUSE_BUF_MIN_FREE	(3 * 4)
#define MOUSE_BUF_NEXT(p)	(((p) + 1) % MOUSE_BUF_MAX)


static Byte mouseBuf[MOUSE_BUF_MAX];
static int mouseBufWritePtr;
static int mouseBufReadPtr;

static int xPos;
static int yPos;
static Byte buttons;


static void mouseBufInit(void) {
  mouseBufWritePtr = 0;
  mouseBufReadPtr = 0;
  xPos = 0;
  yPos = 0;
  buttons = 0;
}


static int mouseBufFree(void) {
  if (mouseBufReadPtr <= mouseBufWritePtr) {
    return MOUSE_BUF_MAX - (mouseBufWritePtr - mouseBufReadPtr) - 1;
  } else {
    return (mouseBufReadPtr - mouseBufWritePtr) - 1;
  }
}


static void mouseBufWrite(Byte *p, int n) {
  int i;

  pthread_mutex_lock(&lock);
  if (mouseBufFree() < MOUSE_BUF_MIN_FREE) {
    /* buffer full */
    pthread_mutex_unlock(&lock);
    return;
  }
  for (i = 0; i < n; i++) {
    mouseBuf[mouseBufWritePtr] = p[i];
    mouseBufWritePtr = MOUSE_BUF_NEXT(mouseBufWritePtr);
  }
  pthread_mutex_unlock(&lock);
}


void mouseMoved(int xMotionX, int xMotionY) {
  int dx, dy;
  Byte sx, sy;
  Byte ox, oy;
  Byte mouseEvent[3];

  if (debugMove) {
    cPrintf("**** MOUSE MOVED: 0x%08X 0x%08X ****\n", xMotionX, xMotionY);
  }
  if (!mouseDataReporting) {
    /* do not send any movement packets */
    return;
  }
  dx = xMotionX - xPos;
  xPos = xMotionX;
  sx = (dx < 0) ? 0x10 : 0x00;
  ox = (dx < -255 || dx > 255) ? 0x40 : 0x00;
  dy = -(xMotionY - yPos);
  yPos = xMotionY;
  sy = (dy < 0) ? 0x20 : 0x00;
  oy = (dy < -255 || dy > 255) ? 0x80 : 0x00;
  mouseEvent[0] = oy | ox | sy | sx | 0x08 | buttons;
  mouseEvent[1] = dx & 0xFF;
  mouseEvent[2] = dy & 0xFF;
  mouseBufWrite(mouseEvent, 3);
}


void mouseButtonPressed(unsigned int xButton) {
  Byte mouseEvent[3];

  if (debugButtons) {
    cPrintf("**** MOUSE BUTTON PRESSED: 0x%08X ****\n", xButton);
  }
  if (!mouseDataReporting) {
    /* do not send any movement packets */
    return;
  }
  switch (xButton) {
    case 1:
      /* left */
      buttons |= 0x01;
      break;
    case 2:
      /* middle */
      buttons |= 0x04;
      break;
    case 3:
      /* right */
      buttons |= 0x02;
      break;
  }
  mouseEvent[0] = 0x08 | buttons;
  mouseEvent[1] = 0;
  mouseEvent[2] = 0;
  mouseBufWrite(mouseEvent, 3);
}


void mouseButtonReleased(unsigned int xButton) {
  Byte mouseEvent[3];

  if (debugButtons) {
    cPrintf("**** MOUSE BUTTON RELEASED: 0x%08X ****\n", xButton);
  }
  if (!mouseDataReporting) {
    /* do not send any movement packets */
    return;
  }
  switch (xButton) {
    case 1:
      /* left */
      buttons &= ~0x01;
      break;
    case 2:
      /* middle */
      buttons &= ~0x04;
      break;
    case 3:
      /* right */
      buttons &= ~0x02;
      break;
  }
  mouseEvent[0] = 0x08 | buttons;
  mouseEvent[1] = 0;
  mouseEvent[2] = 0;
  mouseBufWrite(mouseEvent, 3);
}


/**************************************************************/


static Word mouseCtrl;
static Word mouseRcvrData;
static Word mouseXmtrData;


static void mouseRcvrCallback(int dev) {
  if (debug) {
    cPrintf("\n**** MOUSE RCVR CALLBACK ****\n");
  }
  timerStart(MOUSE_RCVR_USEC, mouseRcvrCallback, dev);
  if (mouseBufWritePtr == mouseBufReadPtr || (mouseCtrl & MOUSE_RCVR_RDY)) {
    /* no byte ready, or last byte not yet read */
    return;
  }
  /* any byte received */
  mouseRcvrData = mouseBuf[mouseBufReadPtr];
  mouseBufReadPtr = MOUSE_BUF_NEXT(mouseBufReadPtr);
  mouseCtrl |= MOUSE_RCVR_RDY;
  if (mouseCtrl & MOUSE_RCVR_IEN) {
    /* raise mouse interrupt */
    cpuSetInterrupt(IRQ_MOUSE);
  }
}


static void mouseXmtrCallback(int dev) {
  Byte b;

  if (debug) {
    cPrintf("\n**** MOUSE XMTR CALLBACK ****\n");
  }
  switch (mouseXmtrData) {
    case COMMAND_RESET:
      mouseDataReporting = false;
      mouseBufInit();
      b = ANSWER_ACK;
      mouseBufWrite(&b, 1);
      b = ANSWER_BATOK;
      mouseBufWrite(&b, 1);
      b = DEVICE_ID;
      mouseBufWrite(&b, 1);
      break;
    case COMMAND_DISABLE_DATA:
      mouseDataReporting = false;
      b = ANSWER_ACK;
      mouseBufWrite(&b, 1);
      break;
    case COMMAND_ENABLE_DATA:
      mouseDataReporting = true;
      b = ANSWER_ACK;
      mouseBufWrite(&b, 1);
      break;
    default:
      /*
       * Note: Byte <mouseXmtrData> has been sent to the
       *       mouse and is acknowledged (but ignored).
       */
      b = ANSWER_ACK;
      mouseBufWrite(&b, 1);
      break;
  }
  mouseCtrl |= MOUSE_XMTR_RDY;
  if (mouseCtrl & MOUSE_XMTR_IEN) {
    /* raise mouse interrupt */
    cpuSetInterrupt(IRQ_MOUSE);
  }
}


Word mouseRead(Word addr) {
  Word data;

  if (debug) {
    cPrintf("\n**** MOUSE READ from 0x%08X", addr);
  }
  if (!installed) {
    throwException(EXC_BUS_TIMEOUT);
  }
  if (addr == MOUSE_CTRL) {
    data = mouseCtrl;
  } else
  if (addr == MOUSE_DATA) {
    mouseCtrl &= ~MOUSE_RCVR_RDY;
    if (mouseCtrl & MOUSE_RCVR_IEN) {
      /* lower mouse interrupt */
      cpuResetInterrupt(IRQ_MOUSE);
    }
    data = mouseRcvrData;
  } else {
    /* illegal register */
    throwException(EXC_BUS_TIMEOUT);
  }
  if (debug) {
    cPrintf(", data = 0x%08X ****\n", data);
  }
  return data;
}


void mouseWrite(Word addr, Word data) {
  if (debug) {
    cPrintf("\n**** MOUSE WRITE to 0x%08X, data = 0x%08X ****\n",
            addr, data);
  }
  if (!installed) {
    throwException(EXC_BUS_TIMEOUT);
  }
  if (addr == MOUSE_CTRL) {
    if (data & MOUSE_RCVR_IEN) {
      mouseCtrl |= MOUSE_RCVR_IEN;
    } else {
      mouseCtrl &= ~MOUSE_RCVR_IEN;
    }
    if (data & MOUSE_XMTR_IEN) {
      mouseCtrl |= MOUSE_XMTR_IEN;
    } else {
      mouseCtrl &= ~MOUSE_XMTR_IEN;
    }
    if (((mouseCtrl & MOUSE_RCVR_IEN) != 0 &&
         (mouseCtrl & MOUSE_RCVR_RDY) != 0) ||
        ((mouseCtrl & MOUSE_XMTR_IEN) != 0 &&
         (mouseCtrl & MOUSE_XMTR_RDY) != 0)) {
      /* raise mouse interrupt */
      cpuSetInterrupt(IRQ_MOUSE);
    } else {
      /* lower mouse interrupt */
      cpuResetInterrupt(IRQ_MOUSE);
    }
  } else
  if (addr == MOUSE_DATA) {
    mouseXmtrData = data & 0xFF;
    mouseCtrl &= ~MOUSE_XMTR_RDY;
    if (mouseCtrl & MOUSE_XMTR_IEN) {
      /* lower mouse interrupt */
      cpuResetInterrupt(IRQ_MOUSE);
    }
    timerStart(MOUSE_XMTR_USEC, mouseXmtrCallback, 0);
  } else {
    /* illegal register */
    throwException(EXC_BUS_TIMEOUT);
  }
}


void mouseReset(void) {
  Byte b;

  if (!installed) {
    return;
  }
  cPrintf("Resetting Mouse...\n");
  mouseDataReporting = false;
  mouseBufInit();
  b = ANSWER_BATOK;
  mouseBufWrite(&b, 1);
  b = DEVICE_ID;
  mouseBufWrite(&b, 1);
  mouseCtrl = MOUSE_XMTR_RDY;
  mouseRcvrData = 0;
  mouseXmtrData = 0;
  timerStart(MOUSE_RCVR_USEC, mouseRcvrCallback, 0);
}


void mouseInit(void) {
  if (pthread_mutex_init(&lock, NULL) != 0) {
    error("could not initialize mouse mutex");
  }
  installed = true;
  mouseReset();
}


void mouseExit(void) {
  if (!installed) {
    return;
  }
  pthread_mutex_destroy(&lock);
}
