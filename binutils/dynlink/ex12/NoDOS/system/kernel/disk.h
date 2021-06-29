/*
 * disk.h -- disk interface
 */


#ifndef _DISK_H_
#define _DISK_H_


typedef struct {
  int blockStart;
  int blockSize;
  int byteSize;
  int dummy;
  char name[64 - 4 * 4];
} DirEntry;

typedef struct {
  int slotInUse;
  int index;
} DIR;

typedef struct {
  int slotInUse;
  int blockStart;
  int byteSize;
  int filePtr;
} FILE;


void mountDisk(void);

DIR *opendir(void);
void closedir(DIR *dp);
int readdir(DirEntry *buf, DIR *dp);

FILE *fopen(char *path);
void fclose(FILE *fp);
int fread(void *buf, int size, int num, FILE *fp);


#endif /* _DISK_H_ */
