/*
 * disk.h -- simulate a super simple disk
 */


#ifndef _DISK_H_
#define _DISK_H_


#define BLOCK_SIZE		4096
#define MAX_FILES		(BLOCK_SIZE / sizeof(DirEntry))

#define MAX_OPEN_DIRS		20
#define MAX_OPEN_FILES		20

#define SEEK_SET		0
#define SEEK_CUR		1
#define SEEK_END		2


typedef struct {
  int blockStart;
  int byteSize;
  char name[64 - 2 * 4];
} DirEntry;

typedef struct {
  int slotInUse;
  int index;
} DIR;

typedef struct {
  int slotInUse;
  int blockStart;
  int byteSize;
  int filePos;
  int bufBlkno;
  unsigned char buf[BLOCK_SIZE];
} FILE;


void mountDisk(void);

DIR *opendir(void);
void closedir(DIR *dp);
int readdir(DirEntry *buf, DIR *dp);

FILE *fopen(char *path);
void fclose(FILE *fp);
int fseek(FILE *fp, int offset, int whence);
int ftell(FILE *fp);
int fread(void *buf, int size, int num, FILE *fp);


#endif /* _DISK_H_ */
