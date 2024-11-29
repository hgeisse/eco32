/*
 * main.c -- main program
 */


#include "types.h"
#include "stdarg.h"
#include "iolib.h"


#define PXD_BASE	((Word *) 0xF0700000)	/* packet device base */

#define PXD_RCV_CTRL	(PXD_BASE + 0)		/* control register */
#define PXD_RCV_SIZE	(PXD_BASE + 1)		/* size register */
#define PXD_RCV_BUFFER	(PXD_BASE + 0x20000)	/* data buffer start */

#define PXD_RCV_RDY	0x01		/* receiver has a packet */
#define PXD_RCV_OVER	0x02		/* receiver has been overrun */
#define PXD_RCV_RLS	0x10		/* release receiver */
#define PXD_RCV_IEN	0x20		/* enable receiver interrupt */

#define PXD_XMT_CTRL	(PXD_BASE + 4)		/* control register */
#define PXD_XMT_SIZE	(PXD_BASE + 5)		/* size register */
#define PXD_XMT_BUFFER	(PXD_BASE + 0x20400)	/* data buffer start */

#define PXD_XMT_RDY	0x01		/* transmitter accepts a packet */
#define PXD_XMT_UNDR	0x02		/* transmitter has been underrun */
#define PXD_XMT_RLS	0x10		/* release transmitter */
#define PXD_XMT_IEN	0x20		/* enable transmitter interrupt */


/**************************************************************/


Word size;
Word data[1024];


void waitForRcvRdy(void) {
  while ((*PXD_RCV_CTRL & PXD_RCV_RDY) == 0) ;
}


void copyPacketFromRcv(void) {
  int i;
  Word w;

  size = *PXD_RCV_SIZE;
  for (i = 0; i < size; i++) {
    w = PXD_RCV_BUFFER[i];
    data[i] = w;
  }
}


void releaseRcv(void) {
  *PXD_RCV_CTRL = PXD_RCV_RLS;
}


void showRequest(void) {
  int i;
  Word w;

  printf("packet received:\n");
  printf("    size = 0x%08X words\n", size);
  w = data[0];
  printf("    data[0] = %d = %s\n",
         w, w == 42 ? "REQ_ECHO" : w == 43 ? "REP_ECHO" : "<UNKNOWN>");
  for (i = 1; i < size; i++) {
    w = data[i];
    printf("    data[%d] = 0x%08X = '%c%c%c%c'\n",
           i, w,
           (w >> 24) & 0xFF, (w >> 16) & 0xFF,
           (w >>  8) & 0xFF, (w >>  0) & 0xFF);
  }
}


void prepareReply(void) {
  data[0] += 1;
  data[5] &= ~((Word) 0x00010000);
}


void showReply(void) {
  int i;
  Word w;

  printf("packet to be sent:\n");
  printf("    size = 0x%08X words\n", size);
  w = data[0];
  printf("    data[0] = %d = %s\n",
         w, w == 42 ? "REQ_ECHO" : w == 43 ? "REP_ECHO" : "<UNKNOWN>");
  for (i = 1; i < size; i++) {
    w = data[i];
    printf("    data[%d] = 0x%08X = '%c%c%c%c'\n",
           i, w,
           (w >> 24) & 0xFF, (w >> 16) & 0xFF,
           (w >>  8) & 0xFF, (w >>  0) & 0xFF);
  }
}


void waitForXmtRdy(void) {
  while ((*PXD_XMT_CTRL & PXD_XMT_RDY) == 0) ;
}


void copyPacketToXmt(void) {
  int i;
  Word w;

  *PXD_XMT_SIZE = size;
  for (i = 0; i < size; i++) {
    w = data[i];
    PXD_XMT_BUFFER[i] = w;
  }
}


void releaseXmt(void) {
  *PXD_XMT_CTRL = PXD_XMT_RLS;
}


/**************************************************************/


void main(void) {
  printf("\nPacket Exchange Device Test\n\n");
  while (1) {
    waitForRcvRdy();
    copyPacketFromRcv();
    releaseRcv();
    showRequest();
    prepareReply();
    showReply();
    waitForXmtRdy();
    copyPacketToXmt();
    releaseXmt();
  }
}
