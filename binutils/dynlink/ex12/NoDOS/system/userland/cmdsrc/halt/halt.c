/*
 * halt.c -- halt the system
 */


#include "stdio.h"
#include "syscalls.h"


int main(int argc, char *argv[]) {
  printf("NoDOS halted.\n");
  _halt();
  return 0;
}
