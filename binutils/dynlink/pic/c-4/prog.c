static int a;
extern int b;

int func2(void) {
  return 42;
}

void func(void) {
  a = func2();
  b = func2();
  a = b;
  b = a;
}

int main() {
  func();
  return 0;
}
