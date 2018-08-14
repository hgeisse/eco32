/*
 * app.c -- an application using a library
 */


#include <stdio.h>
#include "mylib.h"


int main(int argc, char *argv[]) {
  puts("calling a library function");
  myfunc();
  puts("returned from library function");
  return 0;
}
