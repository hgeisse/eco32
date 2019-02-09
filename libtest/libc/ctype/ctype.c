/*
 * ctypetest.c -- test character class test functions
 */


#include <stdio.h>
#include <ctype.h>


int main(void) {
  int i, j;
  int c;

  printf("\nisalnum\n");
  printf("%c\n", isalnum(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isalnum(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nisalpha\n");
  printf("%c\n", isalpha(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isalpha(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\niscntrl\n");
  printf("%c\n", iscntrl(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", iscntrl(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nisdigit\n");
  printf("%c\n", isdigit(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isdigit(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nisgraph\n");
  printf("%c\n", isgraph(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isgraph(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nislower\n");
  printf("%c\n", islower(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", islower(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nisprint\n");
  printf("%c\n", isprint(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isprint(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nispunct\n");
  printf("%c\n", ispunct(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", ispunct(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nisspace\n");
  printf("%c\n", isspace(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isspace(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nisupper\n");
  printf("%c\n", isupper(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isupper(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\nisxdigit\n");
  printf("%c\n", isxdigit(-1) ? '#' : 'O');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      printf("%c ", isxdigit(16 * i + j) ? '#' : 'O');
    }
    printf("\n");
  }

  printf("\ntolower\n");
  c = tolower(-1);
  printf("%c\n", isprint(c) ? c : ' ');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      c = tolower(16 * i + j);
      printf("%c ", isprint(c) ? c : ' ');
    }
    printf("\n");
  }

  printf("\ntoupper\n");
  c = toupper(-1);
  printf("%c\n", isprint(c) ? c : ' ');
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      c = toupper(16 * i + j);
      printf("%c ", isprint(c) ? c : ' ');
    }
    printf("\n");
  }

  return 0;
}
