/*
 * pxdtest.c -- packet exchange device test, client
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERV_ADDR	"127.0.0.1"
#define SERV_PORT	48765


/**************************************************************/


typedef unsigned int Word;

typedef enum { false = 0, true = 1 } Bool;


#define REQ_ECHO	42
#define REP_ECHO	43


typedef struct {
  Word type;		/* type of packet */
  Word size;		/* size of data (up to 1024 words, may be 0) */
  Word data[1024];	/* followed by <size> words of data */
} Packet;


Bool debugPacket = false;


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  exit(1);
}


/**************************************************************/


void showPacket(Packet *p, ssize_t size) {
  unsigned char *q;
  int i;

  printf("Packet (size = %ld bytes):\n", size);
  q = (unsigned char *) p;
  i = 0;
  while (i < size) {
    printf("%02X ", *q);
    q++;
    i++;
    if (i % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}


/**************************************************************/


int sockfd;
struct sockaddr_in servaddr;
socklen_t servlen = sizeof(servaddr);


void requestReply(Packet *req, ssize_t reqSize,
                  Packet *rep, ssize_t *repSize) {
  ssize_t n;

  /* send request */
  if (debugPacket) {
    showPacket(req, reqSize);
  }
  n = sendto(sockfd, req, reqSize, 0,
             (struct sockaddr *) &servaddr, servlen);
  if (n != reqSize) {
    error("cannot send");
  }
  /* receive answer */
  n = recvfrom(sockfd, rep, sizeof(Packet), 0, NULL, NULL);
  if (n < 0) {
    error("cannot receive");
  }
  *repSize = n;
  if (debugPacket) {
    showPacket(rep, *repSize);
  }
}


/**************************************************************/


Word roundToWords(int n) {
  return (n + sizeof(Word) - 1) / sizeof(Word);
}


void echo(char *str) {
  unsigned int n;
  Packet req;
  ssize_t reqSize;
  Packet rep;
  ssize_t repSize;

  n = roundToWords(strlen(str) + 1);
  printf("REQ: '%s' [%u words]\n", str, n);
  req.type = htonl(REQ_ECHO);
  req.size = htonl(n);
  memset(req.data, 0, n * sizeof(Word));
  strcpy((char *) req.data, str);
  reqSize = (2 + n) * sizeof(Word);
  requestReply(&req, reqSize, &rep, &repSize);
  if (ntohl(rep.type) != REP_ECHO) {
    error("unexpected reply type %u", rep.type);
  }
  n = ntohl(rep.size);
  if (repSize != (2 + n) * sizeof(Word)) {
    error("inconsistent reply size %lu / %u", repSize, n);
  }
  printf("REP: '%s' [%u words]\n", (char *) rep.data, n);
}


/**************************************************************/


void client(void) {
  echo("Hello, world! (req 1)");
  sleep(3);
  echo("Hello, world! (req 2)");
  sleep(2);
  echo("Hello, world! (req 3)");
  sleep(1);
  echo("Hello, world! (req 4)");
}


/**************************************************************/


int main() {
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    error("cannot create socket");
  }
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  if (inet_aton(SERV_ADDR, &servaddr.sin_addr) == 0) {
    error("cannot convert '%s' to an inet address", SERV_ADDR);
  }
  servaddr.sin_port = htons(SERV_PORT);
  client();
  return 0;
}
