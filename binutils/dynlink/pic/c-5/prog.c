/*
 * program that produces all sorts of relocations
 */

static int stat_dummy_1 = 0x11111111;
static int stat_dummy_3;
static int stat_dummy_4;

static int m = 0x12345678;
static int k;
extern int n;

static int stat_dummy_2 = 0x22222222;
static int stat_dummy_5;
static int stat_dummy_6;

static void f(void) {
}

extern void g(void);

/* .data/.code/W32 */
void (*fptr1)(void) = f;

/* .data/.data/W32 */
int *iptr1 = &m;

/* .data/.bss/W32 */
int *iptr2 = &k;

/* .data/symbol/W32 */
void (*fptr2)(void) = g;

/* .data/symbol/W32 */
int *iptr3 = &n;

int main(void) {
  int i;
  void (*fp)(void);

  /* .code/.code/H16, .code/.code/L16 */
  fp = f;

  /* .code/.code/R16 */
  if (m == k) {
    m++;
  }

  /* .code/.code/R26 */
  f();

  /* .code/.data/H16, .code/.data/L16 */
  i = m;

  /* .code/.bss/H16, .code/.bss/L16 */
  i = k;

  /* .code/symbol/H16, .code/symbol/L16 */
  i = n;

  /* .code/symbol/H16, .code/symbol/L16 */
  fp = g;

  /* .code/symbol/R26 */
  g();

  return 0;
}
