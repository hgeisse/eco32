static int n = 0x3456789A;

int main(void) {
  int *p;

  p = &n;
  return *p - n;
}
