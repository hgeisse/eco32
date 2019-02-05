/*
 * remez.c -- linear approximation of sqrt(x)
 *
 * interval: [a,b] = [1,2]
 * function: sqrt(x)
 * degree of polynomial: 1
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define EPS	1.0e-6


void error(char *msg) {
  printf("Error: %s\n", msg);
  exit(1);
}


void show(char *txt, double m[3][3], double y[3]) {
  int i, j;

  printf("%s\n", txt);
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      printf("%14e  ", m[i][j]);
    }
    printf("   ##   %14e", y[i]);
    printf("\n");
  }
  printf("\n");
}


void solve3x3(double m[3][3], double y[3]) {
  int i, j, k;
  double p, r;

  for (i = 0; i < 3; i++) {
    p = m[i][i];
    printf("i=%d, p=%e\n", i, p);
    if (fabs(p) < EPS) {
      error("matrix is singular");
    }
    for (j = i; j < 3; j++) {
      m[i][j] /= p;
    }
    y[i] /= p;
    show("unit", m, y);
    for (j = i + 1; j < 3; j++) {
      p = m[j][i];
      printf("j=%d, p=%e\n", j, p);
      for (k = i; k < 3; k++) {
        m[j][k] -= p * m[i][k];
      }
      y[j] -= p * y[i];
    }
    show("iteration", m, y);
  }
  for (i = 2; i > 0; i--) {
    r = y[i];
    for (j = i - 1; j >= 0; j--) {
      y[j] -= m[j][i] * r;
      m[j][i] = 0.0;
    }
  }
  show("back-insert", m, y);
}


double f(double x) {
  return sqrt(x);
}


double phi;		/* approx. 6.180340e-01 */
double phi_square;	/* approx. 3.819660e-01 */


double x_point(double a, double b) {
  return a + (b - a) * phi_square;
}


double y_point(double a, double b) {
  return a + (b - a) * phi;
}


double search(double b0, double b1) {
  double a, x, y, b;
  double fx, fy;
  double next;

  a = 1.0;
  b = 2.0;
  x = x_point(a, b);
  y = y_point(a, b);
  fx = f(x) - (b0 + x * b1);
  fy = f(y) - (b0 + y * b1);
  while (fabs(b - a) > EPS) {
    printf("search: a=%e, x=%e, y=%e, b=%e\n", a, x, y, b);
    printf("        fx=%e, fy=%e\n", fx, fy);
    if (fx > fy) {
      next = x_point(a, y);
      b = y;
      y = x;
      x = next;
      fy = fx;
      fx = f(x) - (b0 + x * b1);
    } else {
      next = y_point(x, b);
      a = x;
      x = y;
      y = next;
      fx = fy;
      fy = f(y) - (b0 + y * b1);
    }
  }
  return (a + b) / 2.0;
}


int main(void) {
  double m[3][3];
  double y[3];
  double x[3];
  int i, iter;

  phi = (sqrt(5.0) - 1.0) / 2.0;
  phi_square = phi * phi;
#if 0
  m[0][0] =  3.0;
  m[0][1] = -4.0;
  m[0][2] =  1.0;
  m[1][0] =  1.0;
  m[1][1] =  2.0;
  m[1][2] = -1.0;
  m[2][0] = -2.0;
  m[2][1] = -3.0;
  m[2][2] =  3.0;
  y[0] = -2.0;
  y[1] =  2.0;
  y[2] =  1.0;
  show("original", m, y);
  solve3x3(m, y);
#endif
  x[0] = 1.0;
  x[1] = 1.5;
  x[2] = 2.0;
  for (iter = 0; iter < 4; iter++) {
    for (i = 0; i < 3; i++) {
      m[i][0] = 1;
      m[i][1] = x[i];
      m[i][2] = (i & 1) ? -1.0 : +1.0;
      y[i] = f(x[i]);
    }
    solve3x3(m, y);
    x[1] = search(y[0], y[1]);
  }
  printf("y = b0 + b1*x with b0 = %e, b1 = %e\n", y[0], y[1]);
  return 0;
}
