/*
 * test00.c -- constants, data transfers
 */

float g_f;
double g_d;

double pi = 3.1415926;

int main(int argc, char *argv[]) {
  float f;
  double d;

  f = 1.1223344;
  d = 5.5667788;
  f = d;
  d = f;
  g_f = f;
  g_d = d;
  f = g_d;
  d = g_f;
  return 0;
}
