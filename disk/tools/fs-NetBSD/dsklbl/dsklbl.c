/*
 * dsklbl.c -- show or write a disklabel onto a NetBSD partition
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "types.h"
#include "disklabel.h"


/**************************************************************/


#define SSIZE		512	/* disk sector size in bytes */
#define BSIZE		8192	/* disk block size in bytes */


/**************************************************************/


void error(char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  printf("Error: ");
  vprintf(fmt, ap);
  printf("\n");
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


unsigned short read2FromEco(unsigned char *p) {
  return (unsigned short) p[0] << 8 |
         (unsigned short) p[1] << 0;
}


void write2ToEco(unsigned char *p, unsigned short data) {
  p[0] = data >> 8;
  p[1] = data >> 0;
}


void conv4FromEcoToX86(unsigned char *p) {
  unsigned int data;

  data = read4FromEco(p);
  * (unsigned int *) p = data;
}


void conv4FromX86ToEco(unsigned char *p) {
  unsigned int data;

  data = * (unsigned int *) p;
  write4ToEco(p, data);
}


void conv2FromEcoToX86(unsigned char *p) {
  unsigned short data;

  data = read2FromEco(p);
  * (unsigned short *) p = data;
}


void conv2FromX86ToEco(unsigned char *p) {
  unsigned short data;

  data = * (unsigned short *) p;
  write2ToEco(p, data);
}


/**************************************************************/


uint16_t chksum(DiskLabel *lp, int numParts) {
  uint16_t *start;
  uint16_t *end;
  uint16_t sum;

  start = (uint16_t *) lp;
  end = (uint16_t *) &lp->d_parts[numParts];
  sum = 0;
  while (start < end) {
    sum ^= *start++;
  }
  return sum;
}


/**************************************************************/


void dlFromEcoToX86(DiskLabel *lp) {
  int i;
  int numParts;

  conv4FromEcoToX86((unsigned char *) &lp->d_magic);
  conv2FromEcoToX86((unsigned char *) &lp->d_type);
  conv2FromEcoToX86((unsigned char *) &lp->d_subtype);
  conv4FromEcoToX86((unsigned char *) &lp->d_secsize);
  conv4FromEcoToX86((unsigned char *) &lp->d_nsectors);
  conv4FromEcoToX86((unsigned char *) &lp->d_ntracks);
  conv4FromEcoToX86((unsigned char *) &lp->d_ncylinders);
  conv4FromEcoToX86((unsigned char *) &lp->d_secpercyl);
  conv4FromEcoToX86((unsigned char *) &lp->d_secperunit);
  conv2FromEcoToX86((unsigned char *) &lp->d_sparespertrack);
  conv2FromEcoToX86((unsigned char *) &lp->d_sparespercyl);
  conv4FromEcoToX86((unsigned char *) &lp->d_acylinders);
  conv2FromEcoToX86((unsigned char *) &lp->d_rpm);
  conv2FromEcoToX86((unsigned char *) &lp->d_interleave);
  conv2FromEcoToX86((unsigned char *) &lp->d_trackskew);
  conv2FromEcoToX86((unsigned char *) &lp->d_cylskew);
  conv4FromEcoToX86((unsigned char *) &lp->d_headswitch);
  conv4FromEcoToX86((unsigned char *) &lp->d_trkseek);
  conv4FromEcoToX86((unsigned char *) &lp->d_flags);
  for (i = 0; i < NDDATA; i++) {
    conv4FromEcoToX86((unsigned char *) &lp->d_drivedata[i]);
  }
  for (i = 0; i < NSPARE; i++) {
    conv4FromEcoToX86((unsigned char *) &lp->d_spare[i]);
  }
  conv4FromEcoToX86((unsigned char *) &lp->d_magic2);
  conv2FromEcoToX86((unsigned char *) &lp->d_checksum);
  conv2FromEcoToX86((unsigned char *) &lp->d_npartitions);
  numParts = lp->d_npartitions;
  if (numParts < 0 || numParts > MAXPARTS) {
    printf("Warning: number of partitions out of range!\n");
    numParts = MAXPARTS;
  }
  conv4FromEcoToX86((unsigned char *) &lp->d_bbsize);
  conv4FromEcoToX86((unsigned char *) &lp->d_sbsize);
  for (i = 0; i < MAXPARTS; i++) {
    conv4FromEcoToX86((unsigned char *) &lp->d_parts[i].p_size);
    conv4FromEcoToX86((unsigned char *) &lp->d_parts[i].p_offset);
    conv4FromEcoToX86((unsigned char *) &lp->d_parts[i].p_fsize);
    conv2FromEcoToX86((unsigned char *) &lp->d_parts[i].p_cpg);
  }
  /* after byte-swapping, the checksum must be calculated again */
  lp->d_checksum = 0;
  lp->d_checksum = chksum(lp, numParts);
  /* Attention: the new checksum must not be converted! */
}


void dlFromX86ToEco(DiskLabel *lp) {
  int i;
  int numParts;

  conv4FromX86ToEco((unsigned char *) &lp->d_magic);
  conv2FromX86ToEco((unsigned char *) &lp->d_type);
  conv2FromX86ToEco((unsigned char *) &lp->d_subtype);
  conv4FromX86ToEco((unsigned char *) &lp->d_secsize);
  conv4FromX86ToEco((unsigned char *) &lp->d_nsectors);
  conv4FromX86ToEco((unsigned char *) &lp->d_ntracks);
  conv4FromX86ToEco((unsigned char *) &lp->d_ncylinders);
  conv4FromX86ToEco((unsigned char *) &lp->d_secpercyl);
  conv4FromX86ToEco((unsigned char *) &lp->d_secperunit);
  conv2FromX86ToEco((unsigned char *) &lp->d_sparespertrack);
  conv2FromX86ToEco((unsigned char *) &lp->d_sparespercyl);
  conv4FromX86ToEco((unsigned char *) &lp->d_acylinders);
  conv2FromX86ToEco((unsigned char *) &lp->d_rpm);
  conv2FromX86ToEco((unsigned char *) &lp->d_interleave);
  conv2FromX86ToEco((unsigned char *) &lp->d_trackskew);
  conv2FromX86ToEco((unsigned char *) &lp->d_cylskew);
  conv4FromX86ToEco((unsigned char *) &lp->d_headswitch);
  conv4FromX86ToEco((unsigned char *) &lp->d_trkseek);
  conv4FromX86ToEco((unsigned char *) &lp->d_flags);
  for (i = 0; i < NDDATA; i++) {
    conv4FromX86ToEco((unsigned char *) &lp->d_drivedata[i]);
  }
  for (i = 0; i < NSPARE; i++) {
    conv4FromX86ToEco((unsigned char *) &lp->d_spare[i]);
  }
  conv4FromX86ToEco((unsigned char *) &lp->d_magic2);
  conv2FromX86ToEco((unsigned char *) &lp->d_checksum);
  numParts = lp->d_npartitions;
  if (numParts < 0 || numParts > MAXPARTS) {
    printf("Warning: number of partitions out of range!\n");
    numParts = MAXPARTS;
  }
  conv2FromX86ToEco((unsigned char *) &lp->d_npartitions);
  conv4FromX86ToEco((unsigned char *) &lp->d_bbsize);
  conv4FromX86ToEco((unsigned char *) &lp->d_sbsize);
  for (i = 0; i < MAXPARTS; i++) {
    conv4FromX86ToEco((unsigned char *) &lp->d_parts[i].p_size);
    conv4FromX86ToEco((unsigned char *) &lp->d_parts[i].p_offset);
    conv4FromX86ToEco((unsigned char *) &lp->d_parts[i].p_fsize);
    conv2FromX86ToEco((unsigned char *) &lp->d_parts[i].p_cpg);
  }
  /* after byte-swapping, the checksum must be calculated again */
  lp->d_checksum = 0;
  lp->d_checksum = chksum(lp, numParts);
  /* Attention: the new checksum must not be converted! */
}


/**************************************************************/


FILE *diskFile;
unsigned int sliceStart;
unsigned int sliceSize;


void readSector(unsigned int sno, unsigned char *buf) {
  int n;

  if (sno >= sliceSize) {
    error("illegal sector number %u in readSector()", sno);
  }
  fseek(diskFile, (sliceStart + sno) * SSIZE, SEEK_SET);
  n = fread(buf, 1, SSIZE, diskFile);
  if (n != SSIZE) {
    error("read error on sector %u", sno);
  }
}


void writeSector(unsigned int sno, unsigned char *buf) {
  int n;

  if (sno >= sliceSize) {
    error("illegal sector number %u in writeSector()", sno);
  }
  fseek(diskFile, (sliceStart + sno) * SSIZE, SEEK_SET);
  n = fwrite(buf, 1, SSIZE, diskFile);
  if (n != SSIZE) {
    error("write error on sector %u", sno);
  }
}


/**************************************************************/


void showDiskLabel(DiskLabel *lp) {
  int i;
  int c;
  int numParts;

  printf("magic number    : 0x%08X\n", lp->d_magic);
  if (lp->d_magic != DSKMAGIC) {
    printf("Warning: wrong magic number!\n");
  }
  printf("type            : 0x%04X\n", lp->d_type);
  printf("subtype         : 0x%04X\n", lp->d_subtype);
  printf("typename        : '");
  for (i = 0; i < NTYPENAME; i++) {
    c = lp->d_typename[i];
    if (c == 0) {
      break;
    }
    if (c >= 0x20 && c < 0x7F) {
      printf("%c", c);
    } else {
      printf(".");
    }
  }
  printf("'\n");
  printf("packname        : '");
  for (i = 0; i < NPACKNAME; i++) {
    c = lp->d_packname[i];
    if (c == 0) {
      break;
    }
    if (c >= 0x20 && c < 0x7F) {
      printf("%c", c);
    } else {
      printf(".");
    }
  }
  printf("'\n");
  printf("secsize         : 0x%08X\n", lp->d_secsize);
  printf("nsectors        : 0x%08X\n", lp->d_nsectors);
  printf("ntracks         : 0x%08X\n", lp->d_ntracks);
  printf("ncylinders      : 0x%08X\n", lp->d_ncylinders);
  printf("secpercyl       : 0x%08X\n", lp->d_secpercyl);
  printf("secperunit      : 0x%08X\n", lp->d_secperunit);
  printf("sparespertrack  : 0x%04X\n", lp->d_sparespertrack);
  printf("sparespercyl    : 0x%04X\n", lp->d_sparespercyl);
  printf("acylinders      : 0x%08X\n", lp->d_acylinders);
  printf("rpm             : 0x%04X\n", lp->d_rpm);
  printf("interleave      : 0x%04X\n", lp->d_interleave);
  printf("trackskew       : 0x%04X\n", lp->d_trackskew);
  printf("cylskew         : 0x%04X\n", lp->d_cylskew);
  printf("headswitch      : 0x%08X\n", lp->d_headswitch);
  printf("trkseek         : 0x%08X\n", lp->d_trkseek);
  printf("flags           : 0x%08X\n", lp->d_flags);
  printf("magic2          : 0x%08X\n", lp->d_magic2);
  if (lp->d_magic2 != DSKMAGIC) {
    printf("Warning: wrong magic number!\n");
  }
  printf("checksum        : 0x%04X\n", lp->d_checksum);
  printf("NOTE: The checksum may have been re-computed because of\n");
  printf("endianness conversion, and thus may differ from the disk!\n");
  printf("bbsize          : 0x%08X\n", lp->d_bbsize);
  printf("sbsize          : 0x%08X\n", lp->d_sbsize);
  printf("npartitions     : 0x%04X\n", lp->d_npartitions);
  numParts = lp->d_npartitions;
  if (numParts < 0 || numParts > MAXPARTS) {
    printf("Warning: number of partitions out of range!\n");
    numParts = MAXPARTS;
  }
  for (i = 0; i < numParts; i++) {
    if (lp->d_parts[i].p_size == 0) {
      continue;
    }
    printf("  partition %c:\n", 'a' + i);
    printf("    size        : 0x%08X\n", lp->d_parts[i].p_size);
    printf("    offset      : 0x%08X\n", lp->d_parts[i].p_offset);
    printf("    fsize       : 0x%08X\n", lp->d_parts[i].p_fsize);
    printf("    fstype      : 0x%02X\n", lp->d_parts[i].p_fstype);
    printf("    frag        : 0x%02X\n", lp->d_parts[i].p_frag);
    printf("    cpg         : 0x%04X\n", lp->d_parts[i].p_cpg);
  }
}


/**************************************************************/


void initDiskLabel(DiskLabel *lp) {
  int i;

  lp->d_magic = 0;
  lp->d_type = 0;
  lp->d_subtype = 0;
  for (i = 0; i < NTYPENAME; i++) {
    lp->d_typename[i] = 0;
  }
  for (i = 0; i < NPACKNAME; i++) {
    lp->d_packname[i] = 0;
  }
  lp->d_secsize = 0;
  lp->d_nsectors = 0;
  lp->d_ntracks = 0;
  lp->d_ncylinders = 0;
  lp->d_secpercyl = 0;
  lp->d_secperunit = 0;
  lp->d_sparespertrack = 0;
  lp->d_sparespercyl = 0;
  lp->d_acylinders = 0;
  lp->d_rpm = 0;
  lp->d_interleave = 0;
  lp->d_trackskew = 0;
  lp->d_cylskew = 0;
  lp->d_headswitch = 0;
  lp->d_trkseek = 0;
  lp->d_flags = 0;
  for (i = 0; i < NDDATA; i++) {
    lp->d_drivedata[i] = 0;
  }
  for (i = 0; i < NSPARE; i++) {
    lp->d_spare[i] = 0;
  }
  lp->d_magic2 = 0;
  lp->d_checksum = 0;
  lp->d_npartitions = 0;
  lp->d_bbsize = 0;
  lp->d_sbsize = 0;
  for (i = 0; i < MAXPARTS; i++) {
    lp->d_parts[i].p_size = 0;
    lp->d_parts[i].p_offset = 0;
    lp->d_parts[i].p_fsize = 0;
    lp->d_parts[i].p_fstype = 0;
    lp->d_parts[i].p_frag = 0;
    lp->d_parts[i].p_cpg = 0;
  }
  for (i = 0; i < PADSIZE; i++) {
    lp->d_pad[i] = 0;
  }
}


void fillDiskLabelDefault1(DiskLabel *lp) {
  /* the default disklabel for a non-partitioned disk */
  lp->d_magic = DSKMAGIC;
  strcpy(lp->d_typename, "ECO32 SD-100M");
  lp->d_secsize = 512;
  lp->d_nsectors = 128;
  lp->d_ntracks = 16;
  lp->d_ncylinders = 100;
  lp->d_secpercyl = 2048;
  lp->d_secperunit = 204800;
  lp->d_sparespertrack = 0;
  lp->d_sparespercyl = 0;
  lp->d_acylinders = 0;
  lp->d_rpm = 3600;
  lp->d_interleave = 1;
  lp->d_trackskew = 0;
  lp->d_cylskew = 0;
  lp->d_headswitch = 0;
  lp->d_trkseek = 0;
  lp->d_flags = 0;
  lp->d_magic2 = DSKMAGIC;
  lp->d_checksum = 0;
  lp->d_npartitions = 16;
  lp->d_bbsize = 8192;
  lp->d_sbsize = 8192;
  /*-------------------------------*/
  lp->d_parts[0].p_size = 172032;
  lp->d_parts[0].p_offset = 0;
  lp->d_parts[0].p_fsize = 1024;
  lp->d_parts[0].p_fstype = 7;
  lp->d_parts[0].p_frag = 8;
  /*-------------------------------*/
  lp->d_parts[1].p_size = 32768;
  lp->d_parts[1].p_offset = 172032;
  lp->d_parts[1].p_fsize = 0;
  lp->d_parts[1].p_fstype = 1;
  lp->d_parts[1].p_frag = 0;
  /*-------------------------------*/
  lp->d_parts[2].p_size = 204800;
  lp->d_parts[2].p_offset = 0;
  lp->d_parts[2].p_fsize = 0;
  lp->d_parts[2].p_fstype = 0;
  lp->d_parts[2].p_frag = 0;
  /*-------------------------------*/
  lp->d_checksum = chksum(lp, lp->d_npartitions);
}


void fillDiskLabelDefault2(DiskLabel *lp) {
  /* the default disklabel for a partitioned disk */
  lp->d_magic = DSKMAGIC;
  strcpy(lp->d_typename, "ECO32 SD-100M");
  lp->d_secsize = 512;
  lp->d_nsectors = 128;
  lp->d_ntracks = 16;
  lp->d_ncylinders = 100;
  lp->d_secpercyl = 2048;
  lp->d_secperunit = 204800;
  lp->d_sparespertrack = 0;
  lp->d_sparespercyl = 0;
  lp->d_acylinders = 0;
  lp->d_rpm = 3600;
  lp->d_interleave = 1;
  lp->d_trackskew = 0;
  lp->d_cylskew = 0;
  lp->d_headswitch = 0;
  lp->d_trkseek = 0;
  lp->d_flags = 0;
  lp->d_magic2 = DSKMAGIC;
  lp->d_checksum = 0;
  lp->d_npartitions = 16;
  lp->d_bbsize = 8192;
  lp->d_sbsize = 8192;
  /*-------------------------------*/
  lp->d_parts[0].p_size = 106496;
  lp->d_parts[0].p_offset = 81920;
  lp->d_parts[0].p_fsize = 1024;
  lp->d_parts[0].p_fstype = 7;
  lp->d_parts[0].p_frag = 8;
  /*-------------------------------*/
  lp->d_parts[1].p_size = 16384;
  lp->d_parts[1].p_offset = 188416;
  lp->d_parts[1].p_fsize = 0;
  lp->d_parts[1].p_fstype = 1;
  lp->d_parts[1].p_frag = 0;
  /*-------------------------------*/
  lp->d_parts[2].p_size = 122880;
  lp->d_parts[2].p_offset = 81920;
  lp->d_parts[2].p_fsize = 0;
  lp->d_parts[2].p_fstype = 0;
  lp->d_parts[2].p_frag = 0;
  /*-------------------------------*/
  lp->d_parts[3].p_size = 204800;
  lp->d_parts[3].p_offset = 0;
  lp->d_parts[3].p_fsize = 0;
  lp->d_parts[3].p_fstype = 0;
  lp->d_parts[3].p_frag = 0;
  /*-------------------------------*/
  lp->d_checksum = chksum(lp, lp->d_npartitions);
}


void fillDiskLabelFromFile(DiskLabel *lp, char *fileName) {
  error("configuration file not supported yet");
}


/**************************************************************/


int checkEndian(DiskLabel *lp) {
  int endian;
  uint32_t magic;
  uint16_t numParts;
  uint16_t sum;

  magic = lp->d_magic;
  if (magic != lp->d_magic2) {
    printf("Warning: magic numbers differ!\n");
    return -1;
  }
  if (magic == DSKMAGIC) {
    /* little endian */
    endian = 1;
  } else {
    /* assume big endian */
    endian = 0;
    /* and check */
    conv4FromEcoToX86((unsigned char *) &magic);
    if (magic != DSKMAGIC) {
      /* neither little endian nor big endian */
      printf("Warning: wrong magic number!\n");
      return -2;
    }
  }
  numParts = lp->d_npartitions;
  if (endian == 0) {
    conv2FromEcoToX86((unsigned char *) &numParts);
  }
  sum = chksum(lp, (int) numParts);
  if (sum != 0) {
    printf("Warning: wrong checksum!\n");
    return -3;
  }
  return endian;
}


/**************************************************************/


void usage(char *myself) {
  printf("Usage:\n");
  printf("    %s -c  <disk> <partition or '*'>\n", myself);
  printf("    %s -t  <disk> <partition or '*'>\n", myself);
  printf("    %s -rb <disk> <partition or '*'>\n", myself);
  printf("    %s -rl <disk> <partition or '*'>\n", myself);
  printf("    %s -wb <disk> <partition or '*'> [<config file>]\n", myself);
  printf("    %s -wl <disk> <partition or '*'> [<config file>]\n", myself);
  printf("c: check, t: toggle endianness\n");
  printf("r: read, w: write disklabel\n");
  printf("b: big, l: little endian\n");
}


int main(int argc, char *argv[]) {
  int optionCheck;
  int optionToggle;
  int optionRead;
  int optionBig;
  char *diskName;
  int part;
  char *endptr;
  unsigned char partTable[SSIZE];
  unsigned char *ptptr;
  unsigned int partType;
  DiskLabel dl;
  int endian;

  if (argc != 4 && argc != 5) {
    usage(argv[0]);
    exit(1);
  }
  if (strcmp(argv[1], "-c") != 0 &&
      strcmp(argv[1], "-t") != 0 &&
      strcmp(argv[1], "-rb") != 0 &&
      strcmp(argv[1], "-rl") != 0 &&
      strcmp(argv[1], "-wb") != 0 &&
      strcmp(argv[1], "-wl") != 0) {
    usage(argv[0]);
    exit(1);
  }
  if (strcmp(argv[1], "-c") == 0) {
    optionCheck = 1;
    optionToggle = 0;
    optionRead = 0;
    optionBig = 0;
  } else
  if (strcmp(argv[1], "-t") == 0) {
    optionCheck = 1;
    optionToggle = 1;
    optionRead = 0;
    optionBig = 0;
  } else {
    optionCheck = 0;
    optionToggle = 0;
    optionRead = (argv[1][1] == 'r');
    optionBig = (argv[1][2] == 'b');
  }
  diskName = argv[2];
  diskFile = fopen(diskName, "r+b");
  if (diskFile == NULL) {
    error("cannot open disk image '%s'", diskName);
  }
  if (strcmp(argv[3], "*") == 0) {
    /* whole disk contains one single slice */
    sliceStart = 0;
    fseek(diskFile, 0, SEEK_END);
    sliceSize = ftell(diskFile) / SSIZE;
  } else {
    /* argv[3] is partition number of NetBSD slice */
    part = strtoul(argv[3], &endptr, 10);
    if (*endptr != '\0' || part < 0 || part > 15) {
      error("illegal partition number '%s'", argv[3]);
    }
    fseek(diskFile, 1 * SSIZE, SEEK_SET);
    if (fread(partTable, 1, SSIZE, diskFile) != SSIZE) {
      error("cannot read partition table of disk '%s'", diskName);
    }
    ptptr = partTable + part * 32;
    partType = read4FromEco(ptptr + 0);
    if ((partType & 0x7FFFFFFF) != 0x000000A9) {
      error("partition %d of disk '%s' is not a NetBSD slice",
            part, diskName);
    }
    sliceStart = read4FromEco(ptptr + 4);
    sliceSize = read4FromEco(ptptr + 8);
  }
  printf("Slice size is %u (0x%X) sectors of %d bytes each.\n",
         sliceSize, sliceSize, SSIZE);
  if (optionCheck) {
    /* check endianness */
    readSector(1, (unsigned char *) &dl);
    endian = checkEndian(&dl);
    if (endian < 0) {
      error("the endiannes of the disklabel could not be determined");
    }
    printf("The disklabel appears to be %s-endian.\n",
           endian == 0 ? "big" : "little");
    if (optionToggle) {
      /* toggle endianness */
      if (endian == 0) {
        /* big -> little */
        dlFromEcoToX86(&dl);
      } else {
        /* little -> big */
        dlFromX86ToEco(&dl);
      }
      endian = checkEndian(&dl);
      if (endian < 0) {
        error("this should never happen");
      }
      printf("The disklabel has been converted to %s-endian.\n",
             endian == 0 ? "big" : "little");
      writeSector(1, (unsigned char *) &dl);
    }
  } else
  if (optionRead) {
    /* read and show disklabel */
    readSector(1, (unsigned char *) &dl);
    if (optionBig) {
      dlFromEcoToX86(&dl);
    }
    showDiskLabel(&dl);
  } else {
    /* write disklabel */
    initDiskLabel(&dl);
    if (argc == 4) {
      fillDiskLabelDefault2(&dl);
    } else {
      fillDiskLabelFromFile(&dl, argv[4]);
    }
    if (optionBig) {
      dlFromX86ToEco(&dl);
    }
    writeSector(1, (unsigned char *) &dl);
  }
  fclose(diskFile);
  return 0;
}
