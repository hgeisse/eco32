/*
 * test04.c -- function definitions and calls
 */

float twice(float arg) {
  return 2 * arg;
}

int main(int argc, char *argv[]) {
  float f1, f2;

  f1 = twice(0.3333333);
  f2 = twice(twice(0.3333333));
  return 0;
}
