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
static Packet rcvBuf;

static Word xmtCtrl;
static Packet xmtBuf;

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


static void rcvrCallback(int dummy) {
  socklen_t len;
  Packet packet;
  ssize_t n;

  /* restart callback timer */
  timerStart(PXD_RCV_USEC, rcvrCallback, dummy);
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
    cPrintf("%ld bytes received, size = %u words\n", n, packet.size);
  }
  if (rcvCtrl & PXD_RCV_RDY) {
    /* we have a packet in the buffer already */
    /* raise overrun flag, but don't overwrite buffer */
    rcvCtrl |= PXD_RCV_OVR;
  } else {
    /* copy packet to buffer and set ready flag */
    memcpy(&rcvBuf, &packet, n);
    rcvCtrl |= PXD_RCV_RDY;
  }
  if (rcvCtrl & PXD_RCV_IEN) {
    /* raise pxd rcv interrupt */
    cpuSetInterrupt(IRQ_PXD_RCVR);
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
  if (addr == PXD_RCV_TYPE) {
    /* read receive type */
    data = rcvBuf.type;
  } else
  if (addr == PXD_RCV_SIZE) {
    /* read receive size */
    data = rcvBuf.size;
  } else
  if ((addr & 0xFF000) == PXD_RCV_BUFFER) {
    /* read receive buffer */
    data = readWord((Byte *) &rcvBuf.data[(addr & 0x0FFF) >> 2]);
  } else
  if (addr == PXD_XMT_CTRL) {
    /* read transmit control */
    data = xmtCtrl;
  } else
  if (addr == PXD_XMT_TYPE) {
    /* read transmit type */
    throwException(EXC_BUS_TIMEOUT);
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
    /* only bits CLR and IEN */
    if (data & PXD_RCV_CLR) {
      rcvCtrl &= ~(PXD_RCV_OVR | PXD_RCV_RDY);
    }
    if (data & PXD_RCV_IEN) {
      rcvCtrl |= PXD_RCV_IEN;
    } else {
      rcvCtrl &= ~PXD_RCV_IEN;
    }
    if ((rcvCtrl & PXD_RCV_IEN) != 0 &&
        (rcvCtrl & PXD_RCV_RDY) != 0) {
      /* raise pxd rcvr interrupt */
      cpuSetInterrupt(IRQ_PXD_RCVR);
    } else {
      /* lower pxd rcvr interrupt */
      cpuResetInterrupt(IRQ_PXD_RCVR);
    }
  } else
  if (addr == PXD_RCV_TYPE) {
    /* write receive type */
    throwException(EXC_BUS_TIMEOUT);
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
    /* !!!!! */
  } else
  if (addr == PXD_XMT_TYPE) {
    /* write transmit type */
    xmtBuf.type = data;
  } else
  if (addr == PXD_XMT_SIZE) {
    /* write transmit size */
    xmtBuf.size = data;
  } else
  if ((addr & 0xFF000) == PXD_XMT_BUFFER) {
    /* write transmit buffer */
    writeWord((Byte *) &xmtBuf.data[(addr & 0x0FFF) >> 2], data);
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
  timerStart(PXD_RCV_USEC, rcvrCallback, 0);
  xmtCtrl = PXD_XMT_RDY;
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
