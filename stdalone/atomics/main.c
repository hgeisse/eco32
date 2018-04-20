/*
 * main.c -- the main test program
 */


#include "stdarg.h"
#include "start.h"


/*
 * Please choose a mutex method:
 *     0    non-atomic test-and-set, i.e. wrong
 *     1    disable interrupts
 *     2    using ldlw/stcw
 */
#define MUTEX_METHOD	2


#define TRIES		100000


/**************************************************************/


void putchar(char c) {
  unsigned int *base;

  if (c == '\n') {
    putchar('\r');
  }
  base = (unsigned int *) 0xF0300000;
  while ((*(base + 2) & 1) == 0) ;
  *(base + 3) = c;
}


void puts(char *s) {
  char c;

  while ((c = *s++) != '\0') {
    putchar(c);
  }
}


void printn(int n) {
  int a;

  if (n < 0) {
    putchar('-');
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    printn(a);
  }
  putchar(n % 10 + '0');
}


void printu(unsigned int n, unsigned int b) {
  unsigned int a;

  a = n / b;
  if (a != 0) {
    printu(a, b);
  }
  putchar("0123456789ABCDEF"[n % b]);
}


void printf(char *fmt, ...) {
  va_list ap;
  char c;
  int n;
  unsigned int u;
  char *s;

  va_start(ap, fmt);
  while (1) {
    while ((c = *fmt++) != '%') {
      if (c == '\0') {
        va_end(ap);
        return;
      }
      putchar(c);
    }
    c = *fmt++;
    if (c == 'd') {
      n = va_arg(ap, int);
      printn(n);
    } else
    if (c == 'u' || c == 'o' || c == 'x') {
      u = va_arg(ap, int);
      printu(u, c == 'o' ? 8 : (c == 'x' ? 16 : 10));
    } else
    if (c == 's') {
      s = va_arg(ap, char *);
      puts(s);
    } else {
      putchar(c);
    }
  }
}


/**************************************************************/


static char *exceptionCause[32] = {
  /* 00 */  "terminal 0 transmitter interrupt",
  /* 01 */  "terminal 0 receiver interrupt",
  /* 02 */  "terminal 1 transmitter interrupt",
  /* 03 */  "terminal 1 receiver interrupt",
  /* 04 */  "keyboard interrupt",
  /* 05 */  "unknown interrupt",
  /* 06 */  "unknown interrupt",
  /* 07 */  "unknown interrupt",
  /* 08 */  "disk interrupt",
  /* 09 */  "unknown interrupt",
  /* 10 */  "unknown interrupt",
  /* 11 */  "unknown interrupt",
  /* 12 */  "unknown interrupt",
  /* 13 */  "unknown interrupt",
  /* 14 */  "timer 0 interrupt",
  /* 15 */  "timer 1 interrupt",
  /* 16 */  "bus timeout exception",
  /* 17 */  "illegal instruction exception",
  /* 18 */  "privileged instruction exception",
  /* 19 */  "divide instruction exception",
  /* 20 */  "trap instruction exception",
  /* 21 */  "TLB miss exception",
  /* 22 */  "TLB write exception",
  /* 23 */  "TLB invalid exception",
  /* 24 */  "illegal address exception",
  /* 25 */  "privileged address exception",
  /* 26 */  "unknown exception",
  /* 27 */  "unknown exception",
  /* 28 */  "unknown exception",
  /* 29 */  "unknown exception",
  /* 30 */  "unknown exception",
  /* 31 */  "unknown exception"
};


void defaultISR(int irq, unsigned int *registers) {
  printf("\n%s\n", exceptionCause[irq]);
  /* do not skip any instruction */
}


void initInterrupts(void) {
  int i;

  for (i = 0; i < 32; i++) {
    setISR(i, defaultISR);
  }
}


/**************************************************************/


typedef struct thread {
  int slotUsed;			/* is this slot in use? */
  unsigned int stack[1024];	/* the thread's stack */
  unsigned int registers[32];	/* the thread's saved registers */
} Thread;


Thread threads[2];
int current;


void timerISR(int irq, unsigned int *registers) {
  unsigned int *timerBase;
  unsigned int dummy;
  int i;

  /* reset timer IRQ */
  timerBase = (unsigned int *) 0xF0000000;
  dummy = *timerBase;
  /* switch threads */
  if (current == 0) {
    /* try to switch to thread 1 */
    if (threads[1].slotUsed) {
      /* switch from thread 0 to thread 1 */
      for (i = 0; i < 32; i++) {
        threads[0].registers[i] = registers[i];
      }
      for (i = 0; i < 32; i++) {
        registers[i] = threads[1].registers[i];
      }
      current = 1;
    }
  } else {
    /* try to switch to thread 0 */
    if (threads[0].slotUsed) {
      /* switch from thread 1 to thread 0 */
      for (i = 0; i < 32; i++) {
        threads[1].registers[i] = registers[i];
      }
      for (i = 0; i < 32; i++) {
        registers[i] = threads[0].registers[i];
      }
      current = 0;
    }
  }
}


void initTimer(void) {
  unsigned int *timerBase;

  timerBase = (unsigned int *) 0xF0000000;
  *(timerBase + 1) = 50000;
  *timerBase = 2;
  orMask(1 << 14);
}


void initThreads(void) {
  int i;

  for (i = 0; i < 2; i++) {
    threads[i].slotUsed = 0;
  }
  threads[0].slotUsed = 1;
  current = 0;
  setISR(14, timerISR);
  initTimer();
  enable();
}


void startThread(void (*fp)(void)) {
  int i;

  disable();
  threads[1].slotUsed = 1;
  for (i = 0; i < 32; i++) {
    threads[1].registers[i] = 0;
  }
  threads[1].registers[29] = (unsigned int) &threads[1].stack[1024];
  threads[1].registers[30] = (unsigned int) fp;
  enable();
}


/**************************************************************/


int mutex = 0;


#if MUTEX_METHOD == 0
/*
 * non-atomic test-and-set, i.e. wrong
 */
void lock(void) {
  while (mutex != 0) ;
  mutex = 1;
}
#endif


#if MUTEX_METHOD == 1
/*
 * disable interrupts
 */
void lock(void) {
again:
  while (mutex != 0) ;
  disable();
  if (mutex != 0) {
    enable();
    goto again;
  }
  mutex = 1;
  enable();
}
#endif


#if MUTEX_METHOD == 2
/*
 * using ldlw/stcw
 */
void lock(void) {
  spinLock(&mutex);
}
#endif


void unlock(void) {
  mutex = 0;
}


/**************************************************************/


int f_ended;
int f_critical;


void f(void) {
  int i, j;

  for (i = 0; i < TRIES; i++) {
    lock();
    f_critical = 1;
    f_critical = 0;
    unlock();
  }
  f_ended = 1;
  while (1) ;
}


/**************************************************************/


int g(void) {
  int i, j;
  int violations;

  violations = 0;
  for (i = 0; i < TRIES; i++) {
    lock();
    if (f_critical) {
      violations++;
    }
    unlock();
  }
  return violations;
}


/**************************************************************/


void main(void) {
  int violations;

  initInterrupts();
  printf("\nTesting mutual exclusion\n");
#if MUTEX_METHOD == 0
  printf("Method 0: non-atomic test-and-set");
#endif
#if MUTEX_METHOD == 1
  printf("Method 1: disable interrupts");
#endif
#if MUTEX_METHOD == 2
  printf("Method 2: load-link/store-conditional");
#endif
  printf("\n\n");
  initThreads();
  printf("Starting second thread on function f...\n");
  f_ended = 0;
  f_critical = 0;
  startThread(f);
  printf("Running main thread on function g...\n");
  violations = g();
  while (!f_ended) ;
  if (violations == 0) {
    printf("\nMutual exclusion working all the time.\n");
  } else {
    printf("\nMutual exclusion violated %d times.\n", violations);
  }
  printf("\nHalting...\n");
}
