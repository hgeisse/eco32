/*
 * mkdisk.c -- build a super simple disk image
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#define BLOCK_SIZE	4096
#define MAX_FILES	(BLOCK_SIZE / sizeof(DirEntry))

#define LINE_SIZE	200
#define MAX_TOKENS	20


/**************************************************************/


typedef struct {
  int blockStart;
  int blockSize;
  int byteSize;
  int dummy;
  char name[64 - 4 * 4];
} DirEntry;


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  fprintf(stderr, "error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}


/**************************************************************/


unsigned int read4FromEco(unsigned char *p) {
  return (unsigned int) p[0] << 24 |
         (unsigned int) p[1] << 16 |
         (unsigned int) p[2] <<  8 |
         (unsigned int) p[3] <<  0;
}


void write4ToEco(unsigned char *p, unsigned int data) {
  p[0] = data >> 24;
  p[1] = data >> 16;
  p[2] = data >>  8;
  p[3] = data >>  0;
}


void conv4FromEcoToNative(unsigned char *p) {
  unsigned int data;

  data = read4FromEco(p);
  * (unsigned int *) p = data;
}


void conv4FromNativeToEco(unsigned char *p) {
  unsigned int data;

  data = * (unsigned int *) p;
  write4ToEco(p, data);
}


/**************************************************************/


void writeBlock(int blkno, void *buf, FILE *imageFile) {
  if (fseek(imageFile, blkno * BLOCK_SIZE, SEEK_SET) < 0) {
    error("cannot seek to block");
  }
  if (fwrite(buf, BLOCK_SIZE, 1, imageFile) != 1) {
    error("cannot write block");
  }
}


/**************************************************************/


DirEntry directory[MAX_FILES];
int nextFilno;
int nextBlkno;


void writePreliminaryDirectory(FILE *imageFile) {
  memset(directory, 0, MAX_FILES * sizeof(DirEntry));
  writeBlock(0, directory, imageFile);
  nextFilno = 0;
  nextBlkno = 1;
}


void writeFinalDirectory(FILE *imageFile) {
  int i;

  for (i = 0; i < MAX_FILES; i++) {
    conv4FromNativeToEco((unsigned char *) &directory[i].blockStart);
    conv4FromNativeToEco((unsigned char *) &directory[i].blockSize);
    conv4FromNativeToEco((unsigned char *) &directory[i].byteSize);
    conv4FromNativeToEco((unsigned char *) &directory[i].dummy);
  }
  writeBlock(0, directory, imageFile);
  for (i = 0; i < MAX_FILES; i++) {
    conv4FromEcoToNative((unsigned char *) &directory[i].blockStart);
    conv4FromEcoToNative((unsigned char *) &directory[i].blockSize);
    conv4FromEcoToNative((unsigned char *) &directory[i].byteSize);
    conv4FromEcoToNative((unsigned char *) &directory[i].dummy);
  }
}


void writeFile(char *hostPath, char *targetPath, FILE *imageFile) {
  FILE *dataFile;
  unsigned char buf[BLOCK_SIZE];
  int n, i;
  int blocks, bytes;

  printf("writing '%s' to '%s'\n", hostPath, targetPath);
  dataFile = fopen(hostPath, "r");
  if (dataFile == NULL) {
    error("cannot open host file '%s'", hostPath);
  }
  blocks = 0;
  bytes = 0;
  while (1) {
    n = fread(buf, 1, BLOCK_SIZE, dataFile);
    if (n == 0) {
      break;
    }
    if (n < BLOCK_SIZE) {
      for (i = n; i < BLOCK_SIZE; i++) {
        buf[i] = '\0';
      }
      writeBlock(nextBlkno + blocks, buf, imageFile);
      blocks++;
      bytes += n;
      break;
    }
    writeBlock(nextBlkno + blocks, buf, imageFile);
    blocks++;
    bytes += BLOCK_SIZE;
  }
  fclose(dataFile);
  printf("  %d blocks (%d bytes) written\n", blocks, bytes);
  strcpy(directory[nextFilno].name, targetPath);
  directory[nextFilno].blockStart = nextBlkno;
  directory[nextFilno].blockSize = blocks;
  directory[nextFilno].byteSize = bytes;
  nextFilno++;
  nextBlkno += blocks;
}


/**************************************************************/


void usage(char *myself) {
  printf("usage: %s <disk layout file> <disk image file>\n", myself);
  exit(1);
}


int tokenize(char *line, char *tokens[], int maxTokens) {
  int n;
  char *p;

  n = 0;
  p = strtok(line, " \t\n");
  while (p != NULL) {
    if (n < maxTokens) {
      tokens[n] = p;
      n++;
    }
    p = strtok(NULL, " \t\n");
  }
  return n;
}


int main(int argc, char *argv[]) {
  char *layoutName;
  char *imageName;
  FILE *layoutFile;
  FILE *imageFile;
  char line[LINE_SIZE];
  char *tokens[MAX_TOKENS];
  int n;

  if (argc != 3) {
    usage(argv[0]);
  }
  layoutName = argv[1];
  layoutFile = fopen(layoutName, "r");
  if (layoutFile == NULL) {
    error("cannot open layout file '%s'", layoutName);
  }
  imageName = argv[2];
  imageFile = fopen(imageName, "w");
  if (imageFile == NULL) {
    error("cannot open image file '%s'", imageName);
  }
  writePreliminaryDirectory(imageFile);
  while (fgets(line, LINE_SIZE, layoutFile) != NULL) {
    n = tokenize(line, tokens, MAX_TOKENS);
    if (n != 2) {
      error("illegal line in layout file");
    }
    writeFile(tokens[0], tokens[1], imageFile);
  }
  writeFinalDirectory(imageFile);
  fclose(layoutFile);
  fclose(imageFile);
  return 0;
}
