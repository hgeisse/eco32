/*
 * boot.c -- the bootstrap (boot loader)
 */


#include "biolib.h"


/**************************************************************/

/* constants and data types */


#define DFLT_KERNEL	"/boot/eos32.bin"    /* path of default OS kernel */
#define LOAD_ADDR	0xC0010000	     /* where to load the kernel */

#define MAX_LINE	100		/* line buffer size */

#define EXEC_MAGIC	0x3AE82DD4	/* magic number for executables */

#define PSHIFT		12		/* ld of page size */
#define PSIZE		(1 << PSHIFT)	/* page size in bytes */
#define PMASK		(PSIZE - 1)	/* page mask */

#define BSHIFT		12		/* ld of block size */
#define BSIZE		(1 << BSHIFT)	/* disk block size in bytes */
#define BMASK		(BSIZE - 1)	/* block mask */

#define SSIZE		512		/* disk sector size in bytes */
#define SPB		(BSIZE / SSIZE)	/* sectors per block */

#define ROOT_INO	1	/* inode number of root directory */
#define NADDR		8	/* number of block addresses in inode */
#define NDADDR		6	/* number of direct block addresses */
#define DIRSIZ		60	/* max length of path name component */

#define NIPB		(BSIZE / sizeof(Dinode))
#define NDIRENT		(BSIZE / sizeof(Dirent))
#define NINDIR		(BSIZE / sizeof(EOS32_daddr_t))

#define NULL		0


typedef unsigned int EOS32_ino_t;
typedef unsigned int EOS32_daddr_t;
typedef unsigned int EOS32_off_t;
typedef int EOS32_time_t;


#define IFMT		070000	/* type of file */
#define	  IFREG		040000	/* regular file */
#define	  IFDIR		030000	/* directory */
#define	  IFCHR		020000	/* character special */
#define	  IFBLK		010000	/* block special */
#define	  IFFREE	000000	/* reserved (indicates free inode) */
#define ISUID		004000	/* set user id on execution */
#define ISGID		002000	/* set group id on execution */
#define ISVTX		001000	/* save swapped text even after use */
#define IUREAD		000400	/* user's read permission */
#define IUWRITE		000200	/* user's write permission */
#define IUEXEC		000100	/* user's execute permission */
#define IGREAD		000040	/* group's read permission */
#define IGWRITE		000020	/* group's write permission */
#define IGEXEC		000010	/* group's execute permission */
#define IOREAD		000004	/* other's read permission */
#define IOWRITE		000002	/* other's write permission */
#define IOEXEC		000001	/* other's execute permission */

/*
 * The following two macros convert an inode number to the disk
 * block containing the inode and an inode number within the block.
 */
#define itod(i)		(2 + (i) / NIPB)
#define itoo(i)		((i) % NIPB)


typedef struct {
  unsigned int di_mode;		/* type and mode of file */
  unsigned int di_nlink;	/* number of links to file */
  unsigned int di_uid;		/* owner's user id */
  unsigned int di_gid;		/* owner's group id */
  EOS32_time_t di_ctime;	/* time created */
  EOS32_time_t di_mtime;	/* time last modified */
  EOS32_time_t di_atime;	/* time last accessed */
  EOS32_off_t di_size;		/* number of bytes in file */
  EOS32_daddr_t di_addr[NADDR];	/* disk block addresses */
} Dinode;


typedef struct {
  EOS32_ino_t d_ino;		/* directory inode */
  char d_name[DIRSIZ];		/* directory name */
} Dirent;


/**************************************************************/

/* string functions */


int strlen(char *str) {
  int i;

  i = 0;
  while (*str++ != '\0') {
    i++;
  }
  return i;
}


int strcmp(char *str1, char *str2) {
  while (*str1 == *str2) {
    if (*str1 == '\0') {
      return 0;
    }
    str1++;
    str2++;
  }
  return (* (unsigned char *) str1) - (* (unsigned char *) str2);
}


void strcpy(char *dst, char *src) {
  while ((*dst++ = *src++) != '\0') ;
}


/**************************************************************/

/* terminal I/O */


char getchar(void) {
  return getc();
}


void putchar(char c) {
  if (c == '\n') {
    putchar('\r');
  }
  putc(c);
}


void puts(char *s) {
  while (*s != '\0') {
    putchar(*s++);
  }
}


/**************************************************************/

/* get a line from the terminal */


void getLine(char *prompt, char *line, int max) {
  int index;
  char c;

  puts(prompt);
  puts(line);
  index = strlen(line);
  while (1) {
    c = getchar();
    switch (c) {
      case '\r':
        putchar('\n');
        line[index] = '\0';
        return;
      case '\b':
      case 0x7F:
        if (index == 0) {
          break;
        }
        putchar('\b');
        putchar(' ');
        putchar('\b');
        index--;
        break;
      default:
        if (c == '\t') {
          c = ' ';
        }
        if (c < 0x20 || c > 0x7E) {
          break;
        }
        putchar(c);
        line[index++] = c;
        break;
    }
  }
}


/**************************************************************/

/* halt with error message */


void halt(char *msg) {
  puts(msg);
  puts(", machine halted.\n");
  while (1) ;
}


/**************************************************************/

/* disk I/O */


unsigned int bootDisk = 0;	/* gets loaded by previous stage */
unsigned int startSector = 0;	/* gets loaded by previous stage */
unsigned int numSectors = 0;	/* gets loaded by previous stage */


/*
 * read a disk block
 *
 * NOTE: buffer must be word-aligned
 */
void readBlock(unsigned int blkno, void *buffer) {
  unsigned int sector;
  int result;

  sector = blkno * SPB;
  if (sector + SPB > numSectors) {
    halt("Sector number exceeds disk or partition size");
  }
  result = rwscts(bootDisk, 'r', sector + startSector,
                  (unsigned int) buffer & 0x3FFFFFFF, SPB);
  if (result != 0) {
    halt("Load error");
  }
}


/**************************************************************/

/* file operations*/


EOS32_daddr_t sibn = (EOS32_daddr_t) -1;
EOS32_daddr_t singleIndirectBlock[NINDIR];
EOS32_daddr_t dibn = (EOS32_daddr_t) -1;
EOS32_daddr_t doubleIndirectBlock[NINDIR];


/*
 * read a block of a file
 *
 * NOTE: addr must be word-aligned
 */
void readFileBlock(Dinode *ip, int blkno, void *addr) {
  EOS32_daddr_t diskBlock;

  if (blkno < NDADDR) {
    /* direct block */
    diskBlock = ip->di_addr[blkno];
  } else
  if (blkno < NDADDR + NINDIR) {
    /* single indirect block */
    diskBlock = ip->di_addr[NDADDR];
    if (sibn != diskBlock) {
      readBlock(diskBlock, singleIndirectBlock);
      sibn = diskBlock;
    }
    diskBlock = singleIndirectBlock[blkno - NDADDR];
  } else {
    /* double indirect block */
    diskBlock = ip->di_addr[NDADDR + 1];
    if (dibn != diskBlock) {
      readBlock(diskBlock, doubleIndirectBlock);
      dibn = diskBlock;
    }
    diskBlock = doubleIndirectBlock[(blkno - NDADDR - NINDIR) / NINDIR];
    if (sibn != diskBlock) {
      readBlock(diskBlock, singleIndirectBlock);
      sibn = diskBlock;
    }
    diskBlock = singleIndirectBlock[(blkno - NDADDR - NINDIR) % NINDIR];
  }
  readBlock(diskBlock, addr);
}


/**************************************************************/

/* inode handling */


Dinode inodeBlock[NIPB];


Dinode *getInode(unsigned int inode) {
  unsigned int bnum;
  unsigned int inum;

  bnum = itod(inode);
  inum = itoo(inode);
  readBlock(bnum, inodeBlock);
  return &inodeBlock[inum];
}


/**************************************************************/

/* convert path to inode */


Dirent directoryBlock[NDIRENT];


Dinode *pathToInode(char *path) {
  Dinode *ip;
  char c;
  char dirBuffer[DIRSIZ];
  char *cp;
  EOS32_off_t offset;
  Dirent *dp;
  unsigned int ino;

  /* get root inode */
  ip = getInode(ROOT_INO);
  /* traverse file system tree while matching path components */
  c = *path++;
  while (1) {
    /* skip leading slashes in path component */
    while (c == '/') {
      c = *path++;
    }
    if (c == '\0') {
      /* no more path components */
      break;
    }
    /* if there is another component, gather up its name */
    cp = dirBuffer;
    while (c != '/' && c != '\0') {
      if (cp < &dirBuffer[DIRSIZ]) {
        *cp++ = c;
      }
      c = *path++;
    }
    *cp++ = '\0';
    /* ip must be a directory */
    if ((ip->di_mode & IFMT) != IFDIR) {
      return NULL;
    }
    /* search directory */
    offset = 0;
    while (1) {
      if (offset >= ip->di_size) {
        /* component not found */
        return NULL;
      }
      if ((offset & BMASK) == 0) {
        /* read the next directory block */
        readFileBlock(ip, offset >> BSHIFT, directoryBlock);
        dp = directoryBlock;
      }
      ino = dp->d_ino;
      if (ino != 0) {
        if (strcmp(dirBuffer, dp->d_name) == 0) {
          /* component found */
          break;
        }
      }
      /* no match, try next entry */
      dp++;
      offset += sizeof(Dirent);
    }
    /* get corresponding inode */
    ip = getInode(ino);
    /* look for more components */
  }
  return ip;
}


/**************************************************************/

/* load the kernel */


void loadKernel(Dinode *ip) {
  unsigned int numBlocks;
  unsigned int nextBlock;
  unsigned char *nextAddr;

  numBlocks = (ip->di_size + (BSIZE - 1)) / BSIZE;
  nextBlock = 0;
  nextAddr = (unsigned char *) LOAD_ADDR;
  while (numBlocks--) {
    readFileBlock(ip, nextBlock, nextAddr);
    nextBlock++;
    nextAddr += BSIZE;
  }
}


/**************************************************************/

/* main program */


unsigned int entryPoint;	/* where to continue from main() */


int main(void) {
  char line[MAX_LINE];
  Dinode *ip;

  puts("Bootstrap loader executing...\n");
  strcpy(line, DFLT_KERNEL);
  while (1) {
    getLine("\nPlease enter path to kernel: ", line, MAX_LINE);
    ip = pathToInode(line);
    if (ip == NULL) {
      puts("'");
      puts(line);
      puts("' not found\n");
      continue;
    }
    if ((ip->di_mode & IFMT) != IFREG) {
      puts("'");
      puts(line);
      puts("' is not a regular file\n");
      continue;
    }
    break;
  }
  puts("Loading '");
  puts(line);
  puts("'...\n");
  loadKernel(ip);
  puts("Starting '");
  puts(line);
  puts("'...\n");
  entryPoint = LOAD_ADDR;
  return 0;
}
