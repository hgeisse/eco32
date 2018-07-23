static int a;
extern int b;

void func(void) {
  a = b;
  b = a;
}
