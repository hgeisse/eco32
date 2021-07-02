/*
 * disk.c -- simulate a super simple disk
 */


#include "stdarg.h"
#include "iolib.h"
#include "disk.h"


/**************************************************************/


/*
 * blocksize: 4k
 * structure: directory (1 block = max. 64 files)
 *            data (N blocks)
 */


unsigned char diskImage[] = {
  #include "../disk/disk.img.txt"
};


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
  } while (de->blockStart == 0);
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
  openFiles[i].filePos = 0;
  openFiles[i].bufBlkno = -1;
  return &openFiles[i];
}


void fclose(FILE *fp) {
  fp->slotInUse = 0;
}


int fseek(FILE *fp, int offset, int whence) {
  int newPos;

  switch (whence) {
    case SEEK_SET:
      newPos = offset;
      break;
    case SEEK_CUR:
      newPos = fp->filePos + offset;
      break;
    case SEEK_END:
      newPos = fp->byteSize + offset;
      break;
    default:
      return -1;
  }
  if (newPos < 0 || newPos >= fp->byteSize) {
    return -1;
  }
  fp->filePos = newPos;
  return 0;
}


int ftell(FILE *fp) {
  return fp->filePos;
}


int fread(void *buf, int size, int num, FILE *fp) {
  unsigned char *bp;
  int nbytes, bytes;
  int blkno, index;
  int n;

  bp = buf;
  nbytes = size * num;
  if (fp->filePos + nbytes > fp->byteSize) {
    nbytes = fp->byteSize - fp->filePos;
  }
  if (nbytes <= 0) {
    return 0;
  }
  bytes = nbytes;
  do {
    blkno = fp->filePos / BLOCK_SIZE;
    if (fp->bufBlkno != blkno) {
      readBlock(fp->blockStart + blkno, fp->buf);
      fp->bufBlkno = blkno;
    }
    index = fp->filePos % BLOCK_SIZE;
    if (bytes > BLOCK_SIZE - index) {
      n = BLOCK_SIZE - index;
    } else {
      n = bytes;
    }
    memcpy(bp, &fp->buf[index], n);
    bp += n;
    fp->filePos += n;
    bytes -= n;
  } while (bytes != 0);
  return nbytes / size;
}
