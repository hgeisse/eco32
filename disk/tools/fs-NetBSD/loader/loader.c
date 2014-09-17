/*
 * loader.c -- the NetBSD system loader
 */


#include "types.h"
#include "disklabel.h"
#include "fs.h"
#include "dinode.h"
#include "dir.h"
#include "stdarg.h"
#include "auxlib.h"
#include "biolib.h"
#include "elf.h"


/**************************************************************/

/* constants and data types */


#define DFLT_KERNEL	"/netbsd"	/* default path to kernel */

#define MAX_LINE	100		/* line buffer size */

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
Bool debugLoadElf = FALSE;


/**************************************************************/

/* halt with error message */


void halt(char *msg) {
  printf("%s, machine halted.\n", msg);
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

/* file operations */


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
    /* double or triple indirect blocks */
    halt("Cannot do double or triple indirect blocks");
  }
  readSectors(diskFrag * spf, spb, addr);
}


/*
 * read a number of bytes from a file
 */
void readFile(Dinode *ip, unsigned int offset,
              unsigned int size, void *buffer) {
  unsigned int dskblk[8192 / sizeof(unsigned int)];
  unsigned char *bufp;
  unsigned char *blkp;
  unsigned int blkno;
  unsigned int index;
  unsigned int aux;
  unsigned int numBytes;
  int i;

  bufp = (unsigned char *) buffer;
  blkp = (unsigned char *) dskblk;
  blkno = offset / sbp->fs_bsize;
  index = offset % sbp->fs_bsize;
  aux = sbp->fs_bsize - index;
  numBytes = (size <= aux) ? size : aux;
  readFileBlock(ip, blkno, blkp);
  for (i = 0; i < numBytes; i++) {
    *bufp++ = blkp[index++];
  }
  blkno++;
  size -= numBytes;
  while (size >= sbp->fs_bsize) {
    index = 0;
    numBytes = sbp->fs_bsize;
    readFileBlock(ip, blkno, blkp);
    for (i = 0; i < numBytes; i++) {
      *bufp++ = blkp[index++];
    }
    blkno++;
    size -= numBytes;
  }
  if (size > 0) {
    index = 0;
    numBytes = size;
    readFileBlock(ip, blkno, blkp);
    for (i = 0; i < numBytes; i++) {
      *bufp++ = blkp[index++];
    }
  }
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

/* convert path to inode */


unsigned int directoryBlock[BSIZE / sizeof(unsigned int)];
Dirent *dbp = (Dirent *) &directoryBlock[0];


Dinode *pathToInode(char *path) {
  Dinode *ip;
  char c;
  char dirBuffer[FFS_MAXNAMLEN];
  char *cp;
  unsigned int offset;
  Dirent *dp;
  unsigned int ino;
  unsigned int len;

  /* get root inode */
  ip = getInode(ROOTINO);
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
      if (cp < &dirBuffer[FFS_MAXNAMLEN]) {
        *cp++ = c;
      }
      c = *path++;
    }
    *cp++ = '\0';
    /* ip must be a directory */
    if ((ip->di_mode & IFMT) != IFDIR) {
      return NULL;
    }
    if (debugSearchDir) {
      printf("DEBUG: this is a directory, ok\n");
      printf("DEBUG: path component to search for = '%s'\n", dirBuffer);
    }
    /* search directory */
    offset = 0;
    while (1) {
      if (offset >= ip->di_size[1]) {
        /* component not found */
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
        if (strcmp(dirBuffer, dp->d_name) == 0) {
          /* component found */
          break;
        }
      }
      /* no match, try next entry */
      dp = (Dirent *) ((unsigned char *) dp + len);
      offset += len;
    }
    /* get corresponding inode */
    ip = getInode(ino);
    /* look for more components */
  }
  return ip;
}


/**************************************************************/

/* load an ELF executable */


#define MAX_PHDR_SIZE	100


Bool loadElf(Dinode *ip, unsigned int *entryPointPtr) {
  Elf32_Ehdr fileHeader;
  Elf32_Addr entry;
  Elf32_Off phoff;
  Elf32_Half phentsize;
  Elf32_Half phnum;
  int i;
  unsigned int phdrBuf[MAX_PHDR_SIZE / sizeof(unsigned int)];
  Elf32_Phdr *phdrPtr;
  Elf32_Word ptype;
  Elf32_Off offset;
  Elf32_Addr address;
  Elf32_Word filesize;
  Elf32_Word memsize;
  Elf32_Word flags;
  Elf32_Word align;
  unsigned char *memPtr;
  int j;

  readFile(ip, 0, sizeof(Elf32_Ehdr), &fileHeader);
  if (fileHeader.e_ident[EI_MAG0] != ELFMAG0 ||
      fileHeader.e_ident[EI_MAG1] != ELFMAG1 ||
      fileHeader.e_ident[EI_MAG2] != ELFMAG2 ||
      fileHeader.e_ident[EI_MAG3] != ELFMAG3 ||
      fileHeader.e_ident[EI_CLASS] != ELFCLASS32 ||
      fileHeader.e_ident[EI_DATA] != ELFDATA2MSB ||
      fileHeader.e_type != ET_EXEC ||
      fileHeader.e_machine != EM_ECO32) {
    /* file is not in ECO32-ELF executable format */
    return FALSE;
  }
  entry = fileHeader.e_entry;
  phoff = fileHeader.e_phoff;
  phentsize = fileHeader.e_phentsize;
  phnum = fileHeader.e_phnum;
  if (debugLoadElf) {
    printf("DEBUG: entry point (virtual addr) : 0x%08X\n", entry);
    printf("DEBUG: program header table at    : %d\n", phoff);
    printf("DEBUG: prog hdr tbl entry size    : %d\n", phentsize);
    printf("DEBUG: num prog hdr tbl entries   : %d\n", phnum);
  }
  for (i = 0 ; i < phnum; i++) {
    printf("Processing program header %d\n", i);
    readFile(ip, phoff + i * phentsize, phentsize, phdrBuf);
    phdrPtr = (Elf32_Phdr *) phdrBuf;
    ptype = phdrPtr->p_type;
    if (ptype != PT_LOAD) {
      printf("    type is %u, ignored\n", ptype);
      continue;
    }
    offset = phdrPtr->p_offset;
    address = phdrPtr->p_vaddr;
    filesize = phdrPtr->p_filesz;
    memsize = phdrPtr->p_memsz;
    flags = phdrPtr->p_flags;
    align = phdrPtr->p_align;
    if (debugLoadElf) {
      printf("DEBUG: offset   : 0x%08X bytes\n", offset);
      printf("DEBUG: address  : 0x%08X\n", address);
      printf("DEBUG: filesize : 0x%08X bytes\n", filesize);
      printf("DEBUG: memsize  : 0x%08X bytes\n", memsize);
      printf("DEBUG: flags    : 0x%08X\n", flags);
      printf("DEBUG: align    : 0x%08X\n", align);
    }
    memPtr = (unsigned char *) address;
    readFile(ip, offset, filesize, memPtr);
    for (j = filesize; j < memsize; j++) {
      memPtr[j] = 0;
    }
    printf("    segment of %u bytes read ", filesize);
    printf("(+ %u bytes zeroed)\n", memsize - filesize);
    printf("    start vaddr = 0x%08X, ", address);
    printf("end vaddr = 0x%08X\n", address + memsize);
  }
  *entryPointPtr = entry;
  return TRUE;
}


/**************************************************************/

/* main program */


unsigned int entryPoint;	/* where to continue from main() */


int main(void) {
  char line[MAX_LINE];
  Dinode *ip;

  printf("\n");
  printf("NetBSD system loader\n");
  readDiskLabel();
  readSuperBlock();
  strcpy(line, DFLT_KERNEL);
  while (1) {
    getLine("\nPlease enter path to kernel: ", line, MAX_LINE);
    ip = pathToInode(line);
    if (ip == NULL) {
      printf("'%s' not found\n", line);
      continue;
    }
    if ((ip->di_mode & IFMT) != IFREG) {
      printf("'%s' is not a regular file\n", line);
      continue;
    }
    printf("Loading '%s'...\n", line);
    if (!loadElf(ip, &entryPoint)) {
      printf("'%s' cannot be loaded\n", line);
      continue;
    }
    /* kernel successfully loaded */
    break;
  }
  printf("Starting '%s' at address 0x%08X...\n", line, entryPoint);
  return 0;
}
