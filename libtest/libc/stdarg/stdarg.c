/*
 * stdarg.c -- test variable argument lists
 */


#include <stdio.h>
#include <stdarg.h>


/**************************************************************/


/* types */

typedef char T1;
typedef short T2;
typedef int T3;
typedef struct {
  int i1;
  int i2;
  int i3;
} T4;
typedef struct {
  char a[131];
} T5;


int equT4(T4 *p, T4 *q) {
  if (p->i1 != q->i1) {
    return 0;
  }
  if (p->i2 != q->i2) {
    return 0;
  }
  if (p->i3 != q->i3) {
    return 0;
  }
  return 1;
}


int equT5(T5 *p, T5 *q) {
  int i;

  for (i = 0; i < 131; i++) {
    if (p->a[i] != q->a[i]) {
      return 0;
    }
  }
  return 1;
}


/* values */

T1 t1_v = 'a';
T2 t2_v = 1234;
T3 t3_v = 33554432;
T4 t4_v = {
  11, 22, 33
};
T5 t5_v = {
  "******************************************\n"
  "* These are exactly 130+1 characters!    **\n"
  "******************************************\n"
};


/* dummies */

T1 t1_d = 'z';
T2 t2_d = -1234;
T3 t3_d = -33554432;
T4 t4_d = {
  99, 88, 77
};
T5 t5_d = {
  "******************************************\n"
  "* These are exactly 130+1 dummy chars!   **\n"
  "******************************************\n"
};


/* read 1 byte */


void f_1_1(char *anchor, ...) {
  va_list ap;
  T1 d;
  T1 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T1);
  v = va_arg(ap, T1);
  if (v == t1_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_2_1(char *anchor, ...) {
  va_list ap;
  T2 d;
  T1 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T2);
  v = va_arg(ap, T1);
  if (v == t1_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_3_1(char *anchor, ...) {
  va_list ap;
  T3 d;
  T1 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T3);
  v = va_arg(ap, T1);
  if (v == t1_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_4_1(char *anchor, ...) {
  va_list ap;
  T4 d;
  T1 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T4);
  v = va_arg(ap, T1);
  if (v == t1_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_5_1(char *anchor, ...) {
  va_list ap;
  T5 d;
  T1 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T5);
  v = va_arg(ap, T1);
  if (v == t1_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


/* read 2 bytes */


void f_1_2(char *anchor, ...) {
  va_list ap;
  T1 d;
  T2 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T1);
  v = va_arg(ap, T2);
  if (v == t2_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_2_2(char *anchor, ...) {
  va_list ap;
  T2 d;
  T2 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T2);
  v = va_arg(ap, T2);
  if (v == t2_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_3_2(char *anchor, ...) {
  va_list ap;
  T3 d;
  T2 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T3);
  v = va_arg(ap, T2);
  if (v == t2_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_4_2(char *anchor, ...) {
  va_list ap;
  T4 d;
  T2 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T4);
  v = va_arg(ap, T2);
  if (v == t2_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_5_2(char *anchor, ...) {
  va_list ap;
  T5 d;
  T2 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T5);
  v = va_arg(ap, T2);
  if (v == t2_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


/* read 4 bytes */


void f_1_3(char *anchor, ...) {
  va_list ap;
  T1 d;
  T3 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T1);
  v = va_arg(ap, T3);
  if (v == t3_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_2_3(char *anchor, ...) {
  va_list ap;
  T2 d;
  T3 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T2);
  v = va_arg(ap, T3);
  if (v == t3_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_3_3(char *anchor, ...) {
  va_list ap;
  T3 d;
  T3 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T3);
  v = va_arg(ap, T3);
  if (v == t3_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_4_3(char *anchor, ...) {
  va_list ap;
  T4 d;
  T3 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T4);
  v = va_arg(ap, T3);
  if (v == t3_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_5_3(char *anchor, ...) {
  va_list ap;
  T5 d;
  T3 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T5);
  v = va_arg(ap, T3);
  if (v == t3_v) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


/* read 12 bytes */


void f_1_4(char *anchor, ...) {
  va_list ap;
  T1 d;
  T4 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T1);
  v = va_arg(ap, T4);
  if (equT4(&v, &t4_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_2_4(char *anchor, ...) {
  va_list ap;
  T2 d;
  T4 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T2);
  v = va_arg(ap, T4);
  if (equT4(&v, &t4_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_3_4(char *anchor, ...) {
  va_list ap;
  T3 d;
  T4 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T3);
  v = va_arg(ap, T4);
  if (equT4(&v, &t4_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_4_4(char *anchor, ...) {
  va_list ap;
  T4 d;
  T4 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T4);
  v = va_arg(ap, T4);
  if (equT4(&v, &t4_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_5_4(char *anchor, ...) {
  va_list ap;
  T5 d;
  T4 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T5);
  v = va_arg(ap, T4);
  if (equT4(&v, &t4_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


/* read 131 bytes */


void f_1_5(char *anchor, ...) {
  va_list ap;
  T1 d;
  T5 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T1);
  v = va_arg(ap, T5);
  if (equT5(&v, &t5_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_2_5(char *anchor, ...) {
  va_list ap;
  T2 d;
  T5 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T2);
  v = va_arg(ap, T5);
  if (equT5(&v, &t5_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_3_5(char *anchor, ...) {
  va_list ap;
  T3 d;
  T5 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T3);
  v = va_arg(ap, T5);
  if (equT5(&v, &t5_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_4_5(char *anchor, ...) {
  va_list ap;
  T4 d;
  T5 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T4);
  v = va_arg(ap, T5);
  if (equT5(&v, &t5_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


void f_5_5(char *anchor, ...) {
  va_list ap;
  T5 d;
  T5 v;

  va_start(ap, anchor);
  printf("%s : ", anchor);
  d = va_arg(ap, T5);
  v = va_arg(ap, T5);
  if (equT5(&v, &t5_v)) {
    printf("ok");
  } else {
    printf("error");
  }
  printf("\n");
  va_end(ap);
}


/* driver */


void test(void) {
  int n;

  n = sizeof(T1);
  printf("size of T1 : %d\n", n);
  n = sizeof(T2);
  printf("size of T2 : %d\n", n);
  n = sizeof(T3);
  printf("size of T3 : %d\n", n);
  n = sizeof(T4);
  printf("size of T4 : %d\n", n);
  n = sizeof(T5);
  printf("size of T5 : %d\n", n);
  printf("/* ---- */\n");
  f_1_1("f_1_1", t1_d, t1_v);
  f_2_1("f_2_1", t2_d, t1_v);
  f_3_1("f_3_1", t3_d, t1_v);
  f_4_1("f_4_1", t4_d, t1_v);
  f_5_1("f_5_1", t5_d, t1_v);
  printf("/* ---- */\n");
  f_1_2("f_1_2", t1_d, t2_v);
  f_2_2("f_2_2", t2_d, t2_v);
  f_3_2("f_3_2", t3_d, t2_v);
  f_4_2("f_4_2", t4_d, t2_v);
  f_5_2("f_5_2", t5_d, t2_v);
  printf("/* ---- */\n");
  f_1_3("f_1_3", t1_d, t3_v);
  f_2_3("f_2_3", t2_d, t3_v);
  f_3_3("f_3_3", t3_d, t3_v);
  f_4_3("f_4_3", t4_d, t3_v);
  f_5_3("f_5_3", t5_d, t3_v);
  printf("/* ---- */\n");
  f_1_4("f_1_4", t1_d, t4_v);
  f_2_4("f_2_4", t2_d, t4_v);
  f_3_4("f_3_4", t3_d, t4_v);
  f_4_4("f_4_4", t4_d, t4_v);
  f_5_4("f_5_4", t5_d, t4_v);
  printf("/* ---- */\n");
  f_1_5("f_1_5", t1_d, t5_v);
  f_2_5("f_2_5", t2_d, t5_v);
  f_3_5("f_3_5", t3_d, t5_v);
  f_4_5("f_4_5", t4_d, t5_v);
  f_5_5("f_5_5", t5_d, t5_v);
}


/**************************************************************/


int main(void) {
  test();
  return 0;
}
