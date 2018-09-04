unsigned int n = 42U;

unsigned int f(void) {
  if (n >= 42U) {
    n = 1U;
  }
  return n;
}
