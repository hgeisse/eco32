/*
 * shdspout.c -- show display output in readable format
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curses.h>


#define LINE_SIZE	200
#define MAX_TOKENS	20


int splitLine(char *line, char *tokens[], int maxTokens) {
  int n;
  char *p;

  n = 0;
  p = strtok(line, ", ");
  while (p != NULL) {
    if (n < maxTokens) {
      tokens[n] = p;
      n++;
    }
    p = strtok(NULL, ", ");
  }
  return n;
}


int main(int argc, char *argv[]) {
  FILE *inFile;
  char line[LINE_SIZE];
  char *tokens[MAX_TOKENS];
  int n;
  int x, y;
  unsigned char c;
  struct timespec delay;

  if (argc != 2) {
    printf("usage: %s <display output file>\n", argv[0]);
    exit(1);
  }
  inFile = fopen(argv[1], "r");
  if (inFile == NULL) {
    printf("error: cannot open input file '%s'\n", argv[1]);
    exit(1);
  }
  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  while (fgets(line, LINE_SIZE, inFile) != NULL) {
    n = splitLine(line, tokens, MAX_TOKENS);
    if (n != 12) {
      continue;
    }
    y = strtoul(tokens[2], NULL, 0);
    x = strtoul(tokens[5], NULL, 0);
    c = strtoul(tokens[11], NULL, 0);
    mvaddch(y, x, c);
    refresh();
    delay.tv_sec = 0;
    delay.tv_nsec = 100000000;
    nanosleep(&delay, NULL);
  }
  do {
    n = getch();
  } while (n == ERR);
  endwin();
  fclose(inFile);
  return 0;
}
