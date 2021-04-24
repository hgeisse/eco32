/*
 * program that produces all sorts of relocations
 */

#ifdef all_relocs
/* relocations in .code */
#define _code_code_H16L16
#define _code_code_R16
#define _code_code_R26
#define _code_data_H16L16
#define _code_bss_H16L16
#define _code_symbol_H16L16_1
#define _code_symbol_H16L16_2
#define _code_symbol_R26
/* relocations in .data */
#define _data_code_W32
#define _data_data_W32
#define _data_bss_W32
#define _data_symbol_W32_1
#define _data_symbol_W32_2
#endif

static int m = 42;
static int k;
extern int n;

static void f(void) {
}

extern void g(void);

/* .data/.code/W32 */
#ifdef _data_code_W32
void (*fptr1)(void) = f;
#endif

/* .data/.data/W32 */
#ifdef _data_data_W32
int *iptr1 = &m;
#endif

/* .data/.bss/W32 */
#ifdef _data_bss_W32
int *iptr2 = &k;
#endif

/* .data/symbol/W32 */
#ifdef _data_symbol_W32_1
void (*fptr2)(void) = g;
#endif

/* .data/symbol/W32 */
#ifdef _data_symbol_W32_2
int *iptr3 = &n;
#endif

int main(void) {
  int i;
  void (*fp)(void);

  /* .code/.code/H16, .code/.code/L16 */
#ifdef _code_code_H16L16
  fp = f;
#endif

  /* .code/.code/R16 */
#ifdef _code_code_R16
  if (i % 5 == i % 3) {
    i++;
  }
#endif

  /* .code/.code/R26 */
#ifdef _code_code_R26
  f();
#endif

  /* .code/.data/H16, .code/.data/L16 */
#ifdef _code_data_H16L16
  i = m;
#endif

  /* .code/.bss/H16, .code/.bss/L16 */
#ifdef _code_bss_H16L16
  i = k;
#endif

  /* .code/symbol/H16, .code/symbol/L16 */
#ifdef _code_symbol_H16L16_1
  i = n;
#endif

  /* .code/symbol/H16, .code/symbol/L16 */
#ifdef _code_symbol_H16L16_2
  fp = g;
#endif

  /* .code/symbol/R26 */
#ifdef _code_symbol_R26
  g();
#endif

  return 0;
}
