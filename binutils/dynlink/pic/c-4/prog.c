static int a;
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
  a = b;
  b = a;
}

int main() {
  func();
  return a * b;
}
