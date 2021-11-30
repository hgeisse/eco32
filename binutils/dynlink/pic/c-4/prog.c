static int s1;
static int s2;
static int a;
static int s3;
static int s4;

extern int b;

static int func1(void) {
  return 1;
}

int func2(void) {
  return 2;
}

static void func(void) {
  /* func1 will be called by a simple 'jal' */
  a += func1();
  /* func2 will be called by loading $1, followed by 'jalr' */
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
