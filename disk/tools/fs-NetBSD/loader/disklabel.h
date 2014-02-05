/*
 * disklabel.h -- structure of the disklabel
 */


#ifndef _DISKLABEL_H_
#define _DISKLABEL_H_


#define DSKMAGIC	0x82564557
#define NTYPENAME	16
#define NPACKNAME	16
#define NDDATA		5
#define NSPARE		5
#define MAXPARTS	16
#define PADSIZE		108


typedef struct {
  uint32_t p_size;		/* number of sectors in partition */
  uint32_t p_offset;		/* starting sector */
  uint32_t p_fsize;		/* filesystem basic fragment size */
  uint8_t p_fstype;		/* filesystem type */
  uint8_t p_frag;		/* filesystem fragments per block */
  uint16_t p_cpg;		/* filesystem cylinders per group */
} Partition;

typedef struct {
  uint32_t d_magic;		/* magic number */
  uint16_t d_type;		/* drive type */
  uint16_t d_subtype;		/* controller, d_type specific */
  char d_typename[NTYPENAME];	/* type name */
  char d_packname[NPACKNAME];	/* pack identifier */
  uint32_t d_secsize;		/* bytes per sector */
  uint32_t d_nsectors;		/* data sectors per track */
  uint32_t d_ntracks;		/* tracks per cylinder */
  uint32_t d_ncylinders;	/* data cylinders per unit */
  uint32_t d_secpercyl;		/* data sectors per cylinder */
  uint32_t d_secperunit;	/* data sectors per unit */
  uint16_t d_sparespertrack;	/* spare sectors per track */
  uint16_t d_sparespercyl;	/* spare sectors per cylinder */
  uint32_t d_acylinders;	/* alternative cylinders per unit */
  uint16_t d_rpm;		/* rotational speed */
  uint16_t d_interleave;	/* hardware sector interleave */
  uint16_t d_trackskew;		/* sector 0 skew, per track */
  uint16_t d_cylskew;		/* sector 0 skew, per cylinder */
  uint32_t d_headswitch;	/* head switch time, usec */
  uint32_t d_trkseek;		/* track-to-track seek, usec */
  uint32_t d_flags;		/* generic flags */
  uint32_t d_drivedata[NDDATA];	/* drive-type specific information */
  uint32_t d_spare[NSPARE];	/* reserved for future use */
  uint32_t d_magic2;		/* the magic number again */
  uint16_t d_checksum;		/* xor of data incl partitions */
  uint16_t d_npartitions;	/* number of partitions */
  uint32_t d_bbsize;		/* size of boot area at sector 0, bytes */
  uint32_t d_sbsize;		/* size of filesystem superblock, bytes */
  Partition d_parts[MAXPARTS];	/* the partition table */
  uint8_t d_pad[PADSIZE];	/* pad up to sector size */
} DiskLabel;


#endif /* _DISKLABEL_H_ */
