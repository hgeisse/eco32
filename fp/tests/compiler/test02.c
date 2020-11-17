/*
 * test02.c -- conversions
 */

int main(int argc, char *argv[]) {
  float f1, f2;
  int i1, i2;
  float r;

  f1 = 5.55555;
  i1 = f1;
  f1 += 0.5;
  i2 = 42;
  f2 = i2;
  i2 += 2;
  r = f1 + i1 + f2 + i2;
  return 0;
}
