/*
 * randlinux.c -- this is from "man 3 rand"
 */


static unsigned long next = 1;


/* RAND_MAX assumed to be 32767 */
int rand(void) {
  next = next * 1103515245 + 12345;
  return((unsigned)(next/65536) % 32768);
}


void srand(unsigned int seed) {
  next = seed;
}
