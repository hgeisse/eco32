static int a;
int b;
extern int c;

static int d = 0x11;
int e = 0x22;

void func(void) {
  a = 1;
  b = 2;
  c = 3;
  d = 4;
  e = 5;
}
