/*
 * rep.c -- test the representation of float numbers
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fp.h"


/**************************************************************/


void dump(_FP_Word w) {
  printf("0x%08X = [%c%02X.%06X]",
         w, _FP_SGN(w) ? '-' : '+', _FP_EXP(w), _FP_FRC(w));
}


void showClass(_FP_Word w) {
  _FP_Word e;
  _FP_Word f;

  e = _FP_EXP(w);
  f = _FP_FRC(w);
  printf("<%s", _FP_SGN(w) ? "-" : "+");
  switch (e) {
    case 0:
      if (f == 0) {
        printf("Zero");
      } else {
        printf("Subn");
      }
      break;
    case 255:
      if (f == 0) {
        printf("Infn");
      } else {
        if (f & 0x00400000) {
          printf("qNaN");
        } else {
          printf("sNaN");
        }
      }
      break;
    default:
      printf("Nrml");
      break;
  }
  printf(">");
}


void showValue(_FP_Word w) {
  _FP_Union X;

  X.w = w;
  printf("%e", X.f);
}


/**************************************************************/


void test_single(_FP_Word w) {
  printf("w = ");
  dump(w);
  printf(" = ");
  showClass(w);
  printf(" = ");
  showValue(w);
  printf("\n");
}


/**************************************************************/


int main(int argc, char *argv[]) {
  printf("------------------------------------------------\n");
  test_single(0x00000000);
  printf("------------------------------------------------\n");
  test_single(0x80000000);
  printf("------------------------------------------------\n");
  test_single(0x7F800000);
  printf("------------------------------------------------\n");
  test_single(0xFF800000);
  printf("------------------------------------------------\n");
  test_single(0x7F800005);
  printf("------------------------------------------------\n");
  test_single(0xFF800005);
  printf("------------------------------------------------\n");
  test_single(0x7FC00000);
  printf("------------------------------------------------\n");
  test_single(0xFFC00000);
  printf("------------------------------------------------\n");
  test_single(0x7FC00005);
  printf("------------------------------------------------\n");
  test_single(0xFFC00005);
  printf("------------------------------------------------\n");
  test_single(0x3F800000);
  printf("------------------------------------------------\n");
  test_single(0xBF800000);
  printf("------------------------------------------------\n");
  test_single(0x40000000);
  printf("------------------------------------------------\n");
  test_single(0xC0000000);
  printf("------------------------------------------------\n");
  test_single(0x3F000000);
  printf("------------------------------------------------\n");
  test_single(0xBF000000);
  printf("------------------------------------------------\n");
  test_single(0x3F600000);
  printf("------------------------------------------------\n");
  test_single(0xBF600000);
  printf("------------------------------------------------\n");
  test_single(0x42F6E979);
  printf("------------------------------------------------\n");
  test_single(0xC2F6E979);
  printf("------------------------------------------------\n");
  test_single(0x579CE349);
  printf("------------------------------------------------\n");
  test_single(0xD79CE349);
  printf("------------------------------------------------\n");
  test_single(0x2FBDAA6F);
  printf("------------------------------------------------\n");
  test_single(0xAFBDAA6F);
  printf("------------------------------------------------\n");
  test_single(0x00300000);
  printf("------------------------------------------------\n");
  test_single(0x80300000);
  printf("------------------------------------------------\n");
  test_single(0x00000001);
  printf("------------------------------------------------\n");
  test_single(0x80000001);
  printf("------------------------------------------------\n");
  test_single(0x00000002);
  printf("------------------------------------------------\n");
  test_single(0x80000002);
  printf("------------------------------------------------\n");
  return 0;
}
