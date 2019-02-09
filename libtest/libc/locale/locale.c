/*
 * locale.c -- test locale functions
 */


#include <stdio.h>
#include <locale.h>


int main(void) {
  struct lconv *lc;

  printf("LC_ALL        : %d\n", LC_ALL);
  printf("LC_COLLATE    : %d\n", LC_COLLATE);
  printf("LC_CTYPE      : %d\n", LC_CTYPE);
  printf("LC_MONETARY   : %d\n", LC_MONETARY);
  printf("LC_NUMERIC    : %d\n", LC_NUMERIC);
  printf("LC_TIME       : %d\n", LC_TIME);
  printf("\n");
  lc = localeconv();
  printf("decimal_point      = \"%s\"\n", lc->decimal_point);
  printf("thousands_sep      = \"%s\"\n", lc->thousands_sep);
  printf("grouping           = \"%s\"\n", lc->grouping);
  printf("int_curr_symbol    = \"%s\"\n", lc->int_curr_symbol);
  printf("currency_symbol    = \"%s\"\n", lc->currency_symbol);
  printf("mon_decimal_point  = \"%s\"\n", lc->mon_decimal_point);
  printf("mon_thousands_sep  = \"%s\"\n", lc->mon_thousands_sep);
  printf("mon_grouping       = \"%s\"\n", lc->mon_grouping);
  printf("positive_sign      = \"%s\"\n", lc->positive_sign);
  printf("negative_sign      = \"%s\"\n", lc->negative_sign);
  printf("int_frac_digits    = %d\n", (int) lc->int_frac_digits);
  printf("frac_digits        = %d\n", (int) lc->frac_digits);
  printf("p_cs_precedes      = %d\n", (int) lc->p_cs_precedes);
  printf("p_sep_by_space     = %d\n", (int) lc->p_sep_by_space);
  printf("n_cs_precedes      = %d\n", (int) lc->n_cs_precedes);
  printf("n_sep_by_space     = %d\n", (int) lc->n_sep_by_space);
  printf("p_sign_posn        = %d\n", (int) lc->p_sign_posn);
  printf("n_sign_posn        = %d\n", (int) lc->n_sign_posn);
  return 0;
}
