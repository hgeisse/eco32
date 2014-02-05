/*
 * boot.c -- the bootstrap (boot loader)
 */


#include "types.h"
#include "disklabel.h"
#include "fs.h"
#include "dinode.h"
#include "dir.h"
#include "stdarg.h"
#include "biolib.h"


/**************************************************************/

/* constants and data types */


#define LOADER_NAME	"loader"	/* name of loader in root dir */
#define LOADER_ADDR	0xC0300000	/* where to load the loader */

#define SSHIFT		9		/* ld of sector size */
#define SSIZE		(1 << SSHIFT)	/* disk sector size in bytes */
#define SMASK		(SSIZE - 1)	/* sector mask */

#define BSHIFT		13		/* ld of block size */
#define BSIZE		(1 << BSHIFT)	/* disk block size in bytes */
#define BMASK		(BSIZE - 1)	/* block mask */


typedef struct ufs1_dinode Dinode;
typedef struct direct Dirent;
typedef unsigned int daddr_t;
typedef unsigned int ino_t;


#define NIPB		(BSIZE / sizeof(Dinode))

#define NULL		0


/**************************************************************/

/* debugging */


Bool debugReadSectors = FALSE;
Bool debugReadDiskLabel = FALSE;
Bool debugReadSuperBlock = FALSE;
Bool debugGetInode = FALSE;
Bool debugSearchDir = FALSE;
Bool debugLoadLoader = FALSE;


/**************************************************************/

/* string support */


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


/**************************************************************/

/* scaled-down version of printf */


/*
 * Output a character on the console.
 * Replace LF by CR/LF.
 */
void putchar(char c) {
  if (c == '\n') {
    putchar('\r');
  }
  putc(c);
}


/*
 * Count the number of characters needed to represent
 * a given number in base 10.
 */
int countPrintn(long n) {
  long a;
  int res;

  res = 0;
  if (n < 0) {
    res++;
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    res += countPrintn(a);
  }
  return res + 1;
}


/*
 * Output a number in base 10.
 */
void printn(long n) {
  long a;

  if (n < 0) {
    putchar('-');
    n = -n;
  }
  a = n / 10;
  if (a != 0) {
    printn(a);
  }
  putchar(n % 10 + '0');
}


/*
 * Count the number of characters needed to represent
 * a given number in a given base.
 */
int countPrintu(unsigned long n, unsigned long b) {
  unsigned long a;
  int res;

  res = 0;
  a = n / b;
  if (a != 0) {
    res += countPrintu(a, b);
  }
  return res + 1;
}


/*
 * Output a number in a given base.
 */
void printu(unsigned long n, unsigned long b, Bool upperCase) {
  unsigned long a;

  a = n / b;
  if (a != 0) {
    printu(a, b, upperCase);
  }
  if (upperCase) {
    putchar("0123456789ABCDEF"[n % b]);
  } else {
    putchar("0123456789abcdef"[n % b]);
  }
}


/*
 * Output a number of filler characters.
 */
void fill(int numFillers, char filler) {
  while (numFillers-- > 0) {
    putchar(filler);
  }
}


/*
 * Formatted output with a variable argument list.
 */
void vprintf(char *fmt, va_list ap) {
  char c;
  int n;
  long ln;
  unsigned int u;
  unsigned long lu;
  char *s;
  Bool negFlag;
  char filler;
  int width, count;

  while (1) {
    while ((c = *fmt++) != '%') {
      if (c == '\0') {
        return;
      }
      putchar(c);
    }
    c = *fmt++;
    if (c == '-') {
      negFlag = TRUE;
      c = *fmt++;
    } else {
      negFlag = FALSE;
    }
    if (c == '0') {
      filler = '0';
      c = *fmt++;
    } else {
      filler = ' ';
    }
    width = 0;
    while (c >= '0' && c <= '9') {
      width *= 10;
      width += c - '0';
      c = *fmt++;
    }
    if (c == 'd') {
      n = va_arg(ap, int);
      count = countPrintn(n);
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      printn(n);
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
      u = va_arg(ap, int);
      count = countPrintu(u,
                c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      printu(u,
             c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
             c == 'X');
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'l') {
      c = *fmt++;
      if (c == 'd') {
        ln = va_arg(ap, long);
        count = countPrintn(ln);
        if (width > 0 && !negFlag) {
          fill(width - count, filler);
        }
        printn(ln);
        if (width > 0 && negFlag) {
          fill(width - count, filler);
        }
      } else
      if (c == 'u' || c == 'o' || c == 'x' || c == 'X') {
        lu = va_arg(ap, long);
        count = countPrintu(lu,
                  c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10));
        if (width > 0 && !negFlag) {
          fill(width - count, filler);
        }
        printu(lu,
               c == 'o' ? 8 : ((c == 'x' || c == 'X') ? 16 : 10),
               c == 'X');
        if (width > 0 && negFlag) {
          fill(width - count, filler);
        }
      } else {
        putchar('l');
        putchar(c);
      }
    } else
    if (c == 's') {
      s = va_arg(ap, char *);
      count = strlen(s);
      if (width > 0 && !negFlag) {
        fill(width - count, filler);
      }
      while ((c = *s++) != '\0') {
        putchar(c);
      }
      if (width > 0 && negFlag) {
        fill(width - count, filler);
      }
    } else
    if (c == 'c') {
      c = va_arg(ap, char);
      putchar(c);
    } else {
      putchar(c);
    }
  }
}


/*
 * Formatted output.
 * This is a scaled-down version of the C library's
 * printf. Used to print diagnostic information on
 * the console (and optionally to a logfile).
 */
void printf(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}


/**************************************************************/

/* halt with error message */


void halt(char *msg) {
  printf(msg);
  printf(", machine halted.\n");
  while (1) ;
}


/**************************************************************/

/* disk I/O */


unsigned int bootDisk = 0;	/* gets loaded by previous stage */
				/* never changed */
unsigned int startSector = 0;	/* gets loaded by previous stage */
				/* initially holds start of slice */
				/* disklabel read --> start of partition */
unsigned int numSectors = 0;	/* gets loaded by previous stage */
				/* initially holds size of slice */
				/* disklabel read --> size of partition */

unsigned int spf;		/* superblock read --> sectors per fragment */
unsigned int spb;		/* superblock read --> sectors per block */


/*
 * read disk sectors
 *
 * NOTE: buffer must be word-aligned
 */
void readSectors(unsigned int start,
                 unsigned int count,
                 void *buffer) {
  unsigned int sector;
  int result;

  if (start + count > numSectors) {
    halt("Sector number exceeds slice or partition size");
  }
  sector = startSector + start;
  if (debugReadSectors) {
    printf("DEBUG: reading sector 0x%08X, %d sectors\n", sector, count);
  }
  result = rwscts(bootDisk, 'r', sector,
                  (unsigned int) buffer & 0x3FFFFFFF, count);
  if (result != 0) {
    halt("Load error");
  }
}


/**************************************************************/

/* disklabel handling */


unsigned int diskLabel[SSIZE / sizeof(unsigned int)];
DiskLabel *dlp = (DiskLabel *) &diskLabel[0];

unsigned int superStart;	/* partition-relative offset, in sectors */
unsigned int superSize;		/* size, in sectors */


void readDiskLabel(void) {
  readSectors(1, 1, dlp);
  if (dlp->d_magic != DSKMAGIC ||
      dlp->d_magic2 != DSKMAGIC) {
    halt("Disklabel corrupted");
  }

  startSector = dlp->d_parts[0].p_offset;
  numSectors = dlp->d_parts[0].p_size;

  superStart = dlp->d_bbsize;
  if ((superStart & SMASK) != 0) {
    halt("Superblock offset is not multiple of sector size");
  }
  superStart >>= SSHIFT;

  superSize = dlp->d_sbsize;
  if ((superSize & SMASK) != 0) {
    halt("Superblock size is not multiple of sector size");
  }
  if (superSize > BSIZE) {
    halt("Superblock too big");
  }
  superSize >>= SSHIFT;

  if (debugReadDiskLabel) {
    printf("DEBUG: super start = %d sectors, super size = %d sectors\n",
           superStart, superSize);
  }
}


/**************************************************************/

/* superblock handling */


unsigned int superBlock[BSIZE / sizeof(unsigned int)];
struct fs *sbp = (struct fs *) &superBlock[0];


void readSuperBlock(void) {
  readSectors(superStart, superSize, sbp);
  if (sbp->fs_magic != FS_UFS1_MAGIC) {
    halt("Wrong magic number in superblock");
  }
  spf = sbp->fs_fsize / SSIZE;
  spb = sbp->fs_bsize / SSIZE;
  if (sbp->fs_inopb != NIPB) {
    halt("Wrong number of inodes per block");
  }
  if (debugReadSuperBlock) {
    printf("DEBUG: %d sectors per fragment\n", spf);
    printf("DEBUG: %d sectors per block\n", spb);
    printf("DEBUG: %d inodes per block\n", NIPB);
  }
}


/**************************************************************/


unsigned int sibno = 0;
unsigned int sib[8192 / sizeof(unsigned int)];
unsigned int *sibp = (unsigned int *) &sib[0];


/*
 * read a block of a file
 *
 * NOTE: addr must be word-aligned
 */
void readFileBlock(Dinode *ip, int blkno, void *addr) {
  unsigned int diskFrag;

  if (blkno < NDADDR) {
    /* direct block */
    diskFrag = ip->di_db[blkno];
  } else
  if (blkno - NDADDR < sbp->fs_bsize / sizeof(int32_t)) {
    /* single indirect block */
    if (sibno != ip->di_ib[0]) {
      sibno = ip->di_ib[0];
      readSectors(sibno * spf, spb, sibp);
    }
    diskFrag = sibp[blkno - NDADDR];
  } else {
    /* double or triple indirect block */
    halt("Cannot do double or triple indirect blocks");
  }
  readSectors(diskFrag * spf, spb, addr);
}


/**************************************************************/

/* inode handling */


unsigned int inodeBlock[BSIZE / sizeof(unsigned int)];
Dinode *ibp = (Dinode *) &inodeBlock[0];


Dinode *getInode(unsigned int inode) {
  unsigned int fnum;
  unsigned int inum;
  Dinode *ip;

  fnum = ino_to_fsba(sbp, inode);
  inum = ino_to_fsbo(sbp, inode);
  if (debugGetInode) {
    printf("DEBUG: getting inode 0x%08X\n", inode);
    printf("DEBUG: this is in fragment %d, inode %d\n", fnum, inum);
  }
  readSectors(fnum * spf, spb, ibp);
  ip = ibp + inum;
  if (ip->di_size[0] != 0) {
    halt("File too big");
  }
  if (debugGetInode) {
    printf("DEBUG: mode of file = 0x%04X\n", ip->di_mode);
    printf("DEBUG: size of file = 0x%08X\n", ip->di_size[1]);
  }
  return ip;
}


/**************************************************************/

/* locate inode of loader in root dir */


unsigned int directoryBlock[BSIZE / sizeof(unsigned int)];
Dirent *dbp = (Dirent *) &directoryBlock[0];


Dinode *getLoader(char *loaderName) {
  Dinode *ip;
  unsigned int offset;
  Dirent *dp;
  unsigned int ino;
  unsigned int len;

  ip = getInode(ROOTINO);
  if ((ip->di_mode & IFMT) != IFDIR) {
    halt("No root directory");
  }
  offset = 0;
  while (1) {
    if (offset >= ip->di_size[1]) {
      /* not found */
      return NULL;
    }
    if ((offset & BMASK) == 0) {
      /* read the next directory block */
      readFileBlock(ip, offset >> BSHIFT, dbp);
      dp = dbp;
    }
    ino = dp->d_fileno;
    len = dp->d_reclen;
    if (debugSearchDir) {
      printf("DEBUG: ino = %4d, len = %4d, ", ino, len);
      printf("dirsiz = %4d, name = %s\n", DIRSIZ(0, dp, 0), dp->d_name);
    }
    if (ino != 0) {
      if (strcmp(loaderName, dp->d_name) == 0) {
        /* found */
        break;
      }
    }
    /* no match, try next entry */
    dp = (Dirent *) ((unsigned char *) dp + len);
    offset += len;
  }
  /* get corresponding inode */
  ip = getInode(ino);
  if ((ip->di_mode & IFMT) != IFREG) {
    printf("/%s ", loaderName);
    halt(" is not a regular file");
  }
  return ip;
}


/**************************************************************/

/* load the loader */


void loadLoader(Dinode *ip) {
  unsigned int numBlocks;
  unsigned int nextBlock;
  unsigned char *nextAddr;

  numBlocks = (ip->di_size[1] + (BSIZE - 1)) / BSIZE;
  nextBlock = 0;
  nextAddr = (unsigned char *) LOADER_ADDR;
  while (numBlocks--) {
    if (debugLoadLoader) {
      printf("DEBUG: loading block %d at 0x%08X\n", nextBlock, nextAddr);
    }
    readFileBlock(ip, nextBlock, nextAddr);
    nextBlock++;
    nextAddr += BSIZE;
  }
}


/**************************************************************/

/* main program */


unsigned int entryPoint;	/* where to continue from main() */


int main(void) {
  Dinode *ip;
  unsigned int saveBootDisk;
  unsigned int saveStartSector;
  unsigned int saveNumSectors;

  /* save environment, see below */
  saveBootDisk = bootDisk;
  saveStartSector = startSector;
  saveNumSectors = numSectors;
  printf("Bootstrap loader executing...\n");
  readDiskLabel();
  readSuperBlock();
  ip = getLoader(LOADER_NAME);
  if (ip == NULL) {
    printf("'/%s' ", LOADER_NAME);
    halt("not found");
  }
  printf("Loading '/%s'...\n", LOADER_NAME);
  loadLoader(ip);
  printf("Starting '/%s'...\n", LOADER_NAME);
  entryPoint = LOADER_ADDR;
  /* restore environment: the loader expects exactly
     the same entry conditions as the boot loader */
  bootDisk = saveBootDisk;
  startSector = saveStartSector;
  numSectors = saveNumSectors;
  return 0;
}
