/*
 * dirent.h -- directory access library
 */


#ifndef _DIRENT_H_
#define _DIRENT_H_


#ifndef NULL
#define NULL	((void *) 0)
#endif


typedef struct {
  int slotInUse;
  int index;
} DIR;


typedef struct {
  int blockStart;
  int byteSize;
  char name[64 - 2 * 4];
} DirEntry;


DIR *opendir(void);
void closedir(DIR *dp);
int readdir(DirEntry *buf, DIR *dp);


#endif /* _DIRENT_H_ */
