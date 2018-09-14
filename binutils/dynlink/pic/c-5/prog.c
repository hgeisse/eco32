/*
 * program that produces all sorts of relocations
 */

static int stat_dummy_1 = 0x11111111;
static int stat_dummy_3;
static int stat_dummy_4;

static int m = 0x12345678;	/* 1: initialization */
static int k;			/* 2: initialization */
extern int n;			/* 3: initialization */

static int stat_dummy_2 = 0x22222222;
static int stat_dummy_5;
static int stat_dummy_6;

static void f(void) {
}

extern void g(void);

/* 4: .data/.code/W32 */
void (*fptr1)(void) = f;

/* 5: .data/.data/W32 */
int *iptr1 = &m;

/* 6: .data/.bss/W32 */
int *iptr2 = &k;

/* 7: .data/symbol/W32 */
void (*fptr2)(void) = g;

/* 8: .data/symbol/W32 */
int *iptr3 = &n;

int main(void) {
  int i;
  void (*fp)(void);

  /* 9: .code/.code/H16, .code/.code/L16 */
  fp = f;

  /* 10: .code/.code/R16 */
  if (m == k) m++;

  /* 11: .code/.code/R26 */
  f();

  /* 12: .code/symbol/R26 */
  g();

  /* 13: .code/.data/H16, .code/.data/L16 */
  i = m;

  /* 14: .code/.bss/H16, .code/.bss/L16 */
  i = k;

  /* 15: .code/symbol/H16, .code/symbol/L16 */
  i = n;

  /* 16: .code/symbol/H16, .code/symbol/L16 */
  fp = g;

  return 0;
}
