static int s1;
static int s2;
static int a;
static int s3;
static int s4;

extern int b;

int func1(void) {
  return 1;
}

int func2(void) {
  return 2;
}

void func(void) {
  a += func1();
  b += func2();
  /* read access to variable 'b': GOT-indirect */
  /* write access to varisble 'a': GOT-relative */
  a = b;
  /* read access to variable 'a': GOT-relative */
  /* write access to varisble 'b': GOT-indirect */
  b = a;
}

int main() {
  func();
  return a * b;
}
