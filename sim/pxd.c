/*
 * pxd.c -- packet exchange device
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"
#include "console.h"
#include "error.h"
#include "except.h"
#include "timer.h"
#include "pxd.h"
#include "cpu.h"


static Bool debug = false;

static Bool installed = false;

static Word rcvCtrl;
static Word rcvSize;
static Word rcvBuf[PXD_MAX_SIZE];

static Word xmtCtrl;
static Word xmtSize;
static Word xmtBuf[PXD_MAX_SIZE];

static int sockfd;
static struct sockaddr_in cliaddr;
static socklen_t clilen = sizeof(cliaddr);


/**************************************************************/


static Word readWord(Byte *p) {
  Word data;

  data = ((Word) *(p + 0)) << 24 |
         ((Word) *(p + 1)) << 16 |
         ((Word) *(p + 2)) <<  8 |
         ((Word) *(p + 3)) <<  0;
  return data;
}


static void writeWord(Byte *p, Word data) {
  *(p + 0) = (Byte) (data >> 24);
  *(p + 1) = (Byte) (data >> 16);
  *(p + 2) = (Byte) (data >>  8);
  *(p + 3) = (Byte) (data >>  0);
}


/**************************************************************/


static void rcvCallback(int dummy) {
  socklen_t len;
  Packet packet;
  ssize_t n;

  /* restart callback timer */
  timerStart(PXD_RCV_USEC, rcvCallback, dummy);
  /* try to receive a packet */
  len = clilen;
  n = recvfrom(sockfd, &packet, sizeof(Packet), 0,
               (struct sockaddr *) &cliaddr, &len);
  if (n < 0) {
    /* cannot receive */
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      /* because nothing did come in */
      return;
    }
    /* here, we have a real error */
    error("packet exchange device receive error");
  }
  /* a packet arrived */
  if (debug) {
    cPrintf("%ld bytes received\n", n);
  }
  if ((n & 3) != 0) {
    /* size of packet is not an integral multiple of words */
    error("packet exchange device illegal packet size");
  }
  if (rcvCtrl & PXD_RCV_RDY) {
    /* we have a packet in the buffer already */
    /* raise overrun flag, but don't overwrite buffer */
    rcvCtrl |= PXD_RCV_OVER;
  } else {
    /* copy data to buffer and set size */
    memcpy(&rcvBuf[0], &packet.data[0], n);
    rcvSize = n >> 2;
    /* set ready flag */
    rcvCtrl |= PXD_RCV_RDY;
    if (rcvCtrl & PXD_RCV_IEN) {
      /* raise pxd rcv interrupt */
      cpuSetInterrupt(IRQ_PXD_RCV);
    }
  }
}


static void xmtCallback(int dummy) {
  Packet packet;
  ssize_t n;
  ssize_t k;

  /* get size and copy data from buffer */
  n = xmtSize << 2;
  memcpy(&packet.data[0], &xmtBuf[0], n);
  /* try to transmit packet */
  k = sendto(sockfd, &packet, n, 0,
             (struct sockaddr *) &cliaddr, clilen);
  if (k != n) {
    /* this is a real error */
    error("packet exchange device transmit error");
  }
  /* set ready flag */
  xmtCtrl |= PXD_XMT_UNDR | PXD_XMT_RDY;
  if (xmtCtrl & PXD_XMT_IEN) {
    /* raise pxd xmt interrupt */
    cpuSetInterrupt(IRQ_PXD_XMT);
  }
}


/**************************************************************/


Word pxdRead(Word addr) {
  Word data;

  if (debug) {
    cPrintf("\n**** PXD READ from 0x%08X", addr);
  }
  if (!installed) {
    /* package exchange device controller not installed */
    throwException(EXC_BUS_TIMEOUT);
  }
  if (addr == PXD_RCV_CTRL) {
    /* read receive control */
    data = rcvCtrl;
  } else
  if (addr == PXD_RCV_SIZE) {
    /* read receive size */
    data = rcvSize;
  } else
  if ((addr & 0xFF000) == PXD_RCV_BUFFER) {
    /* read receive buffer */
    data = readWord((Byte *) &rcvBuf[(addr & 0x0FFF) >> 2]);
  } else
  if (addr == PXD_XMT_CTRL) {
    /* read transmit control */
    data = xmtCtrl;
  } else
  if (addr == PXD_XMT_SIZE) {
    /* read transmit size */
    throwException(EXC_BUS_TIMEOUT);
  } else
  if ((addr & 0xFF000) == PXD_XMT_BUFFER) {
    /* read transmit buffer */
    throwException(EXC_BUS_TIMEOUT);
  } else {
    /* read illegal register */
    throwException(EXC_BUS_TIMEOUT);
  }
  if (debug) {
    cPrintf(", data = 0x%08X ****\n", data);
  }
  return data;
}


void pxdWrite(Word addr, Word data) {
  if (debug) {
    cPrintf("\n**** PXD WRITE to 0x%08X, data = 0x%08X ****\n",
            addr, data);
  }
  if (!installed) {
    /* package exchange device controller not installed */
    throwException(EXC_BUS_TIMEOUT);
  }
  if (addr == PXD_RCV_CTRL) {
    /* write receive control */
    /* only bits RLS and IEN */
    if (data & PXD_RCV_RLS) {
      rcvCtrl &= ~(PXD_RCV_OVER | PXD_RCV_RDY);
    }
    if (data & PXD_RCV_IEN) {
      rcvCtrl |= PXD_RCV_IEN;
    } else {
      rcvCtrl &= ~PXD_RCV_IEN;
    }
    if ((rcvCtrl & PXD_RCV_IEN) != 0 &&
        (rcvCtrl & PXD_RCV_RDY) != 0) {
      /* raise pxd rcv interrupt */
      cpuSetInterrupt(IRQ_PXD_RCV);
    } else {
      /* lower pxd rcv interrupt */
      cpuResetInterrupt(IRQ_PXD_RCV);
    }
  } else
  if (addr == PXD_RCV_SIZE) {
    /* write receive size */
    throwException(EXC_BUS_TIMEOUT);
  } else
  if ((addr & 0xFF000) == PXD_RCV_BUFFER) {
    /* write receive buffer */
    throwException(EXC_BUS_TIMEOUT);
  } else
  if (addr == PXD_XMT_CTRL) {
    /* write transmit control */
    /* only bits RLS and IEN */
    if (data & PXD_XMT_RLS) {
      xmtCtrl &= ~(PXD_XMT_UNDR | PXD_XMT_RDY);
      timerStart(PXD_XMT_USEC, xmtCallback, 0);
    }
    if (data & PXD_XMT_IEN) {
      xmtCtrl |= PXD_XMT_IEN;
    } else {
      xmtCtrl &= ~PXD_XMT_IEN;
    }
    if ((xmtCtrl & PXD_XMT_IEN) != 0 &&
        (xmtCtrl & PXD_XMT_RDY) != 0) {
      /* raise pxd xmt interrupt */
      cpuSetInterrupt(IRQ_PXD_XMT);
    } else {
      /* lower pxd xmt interrupt */
      cpuResetInterrupt(IRQ_PXD_XMT);
    }
  } else
  if (addr == PXD_XMT_SIZE) {
    /* write transmit size */
    xmtSize = data;
  } else
  if ((addr & 0xFF000) == PXD_XMT_BUFFER) {
    /* write transmit buffer */
    writeWord((Byte *) &xmtBuf[(addr & 0x0FFF) >> 2], data);
  } else {
    /* write illegal register */
    throwException(EXC_BUS_TIMEOUT);
  }
}


/**************************************************************/


void pxdReset(void) {
  if (!installed) {
    /* packet exchange device not installed */
    return;
  }
  cPrintf("Resetting Packet Exchange Device...\n");
  rcvCtrl = 0;
  timerStart(PXD_RCV_USEC, rcvCallback, 0);
  xmtCtrl = PXD_XMT_UNDR | PXD_XMT_RDY;
}


void pxdInit(void) {
  struct sockaddr_in servaddr;
  int flags;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    error("cannot create socket for packet exchange device");
  }
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PXD_SERVER_PORT);
  if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    error("cannot bind packet exchange device to server address");
  }
  flags = fcntl(sockfd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(sockfd, F_SETFL, flags);
  installed = true;
  pxdReset();
}


void pxdExit(void) {
  if (!installed) {
    /* packet exchange device not installed */
    return;
  }
}
