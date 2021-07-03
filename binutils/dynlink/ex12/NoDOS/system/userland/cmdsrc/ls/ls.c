/*
 * ls.c -- list directory contents to standard output
 */


#include "stdio.h"
#include "dirent.h"


int main(int argc, char *argv[]) {
  DIR *dp;
  DirEntry dirEntry;

  dp = opendir();
  while (1) {
    if (readdir(&dirEntry, dp) < 0) {
      break;
    }
    printf("%8d  %s\n", dirEntry.byteSize, dirEntry.name);
  }
  closedir(dp);
  return 0;
}
