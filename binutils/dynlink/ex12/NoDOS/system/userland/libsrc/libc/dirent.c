/*
 * dirent.c -- directory access library
 */


#include "dirent.h"
#include "syscalls.h"


DIR *opendir(void) {
  return _opendir();
}


void closedir(DIR *dp) {
  _closedir(dp);
}


int readdir(DirEntry *buf, DIR *dp) {
  return _readdir(buf, dp);
}
