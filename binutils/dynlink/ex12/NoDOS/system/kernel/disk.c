/*
 * disk.c -- simulate a super simple disk
 */


#include "stdarg.h"
#include "iolib.h"
#include "disk.h"


/*
 * blocksize: 4k
 * structure: directory (1 block = max. 64 files)
 *            data (N blocks)
 */


unsigned char diskImage[] = {
  #include "../../mkdisk/disk.img.txt"
};


#define BLOCK_SIZE	4096
#define MAX_FILES	(BLOCK_SIZE / sizeof(DirEntry))

#define MAX_OPEN_DIRS	20
#define MAX_OPEN_FILES	20


/**************************************************************/


static void readBlock(int blkno, void *buf) {
  memcpy(buf, diskImage + blkno * BLOCK_SIZE, BLOCK_SIZE);
}


/**************************************************************/


DirEntry directory[MAX_FILES];


void mountDisk(void) {
  readBlock(0, directory);
}


/**************************************************************/


DIR openDirs[MAX_OPEN_DIRS];


DIR *opendir(void) {
  int i;

  for (i = 0; i < MAX_OPEN_DIRS; i++) {
    if (!openDirs[i].slotInUse) {
      break;
    }
  }
  if (i == MAX_OPEN_DIRS) {
    return NULL;
  }
  openDirs[i].slotInUse = 1;
  openDirs[i].index = -1;
  return &openDirs[i];
}


void closedir(DIR *dp) {
  dp->slotInUse = 0;
}


int readdir(DirEntry *buf, DIR *dp) {
  DirEntry *de;

  do {
    dp->index++;
    if (dp->index == MAX_FILES) {
      return -1;
    }
    de = &directory[dp->index];
  } while (de->blockStart == 0) ;
  memcpy((unsigned char *) buf, (unsigned char *) de, sizeof(DirEntry));
  return 0;
}


/**************************************************************/


FILE openFiles[MAX_OPEN_FILES];


FILE *fopen(char *path) {
  DirEntry *de;
  int i;

  de = &directory[0];
  do {
    if (de->blockStart != 0 && strcmp(de->name, path) == 0) {
      break;
    }
    de++;
  } while (de != &directory[MAX_FILES]);
  if (de == &directory[MAX_FILES]) {
    return NULL;
  }
  for (i = 0; i < MAX_OPEN_FILES; i++) {
    if (!openFiles[i].slotInUse) {
      break;
    }
  }
  if (i == MAX_OPEN_FILES) {
    return NULL;
  }
  openFiles[i].slotInUse = 1;
  openFiles[i].blockStart = de->blockStart;
  openFiles[i].byteSize = de->byteSize;
  openFiles[i].filePtr = 0;
  return &openFiles[i];
}


void fclose(FILE *fp) {
  fp->slotInUse = 0;
}


int fread(void *buf, int size, int num, FILE *fp) {
  return 0;
}
