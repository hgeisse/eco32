/*
 * test03.c -- comparisons
 */

int main(int argc, char *argv[]) {
  float f1, f2;

  f1 = 5.55555;
  f2 = 6.66666;
  if (f1 == f2) {
    return 1;
  }
  if (f1 != f2) {
    return 2;
  }
  if (f1 <= f2) {
    return 3;
  }
  if (f1 < f2) {
    return 4;
  }
  if (f1 >= f2) {
    return 5;
  }
  if (f1 > f2) {
    return 6;
  }
  return 0;
}
