/*
 * a.out.h -- structure of linkable object and executable files
 */


#ifndef _A_OUT_H_
#define _A_OUT_H_


#define EXEC_MAGIC	0x8F0B45C0

#define SEG_ATTR_X	0x01	/* executable */
#define SEG_ATTR_W	0x02	/* writable */
#define SEG_ATTR_P	0x04	/* present in object file */
#define SEG_ATTR_A	0x08	/* allocatable in memory */

#define SYM_ATTR_U	0x01	/* undefined */

#define RELOC_H16	0	/* write 16 bits with high part of value */
#define RELOC_L16	1	/* write 16 bits with low part of value */
#define RELOC_R16	2	/* write 16 bits with value relative to PC */
#define RELOC_R26	3	/* write 26 bits with value relative to PC */
#define RELOC_W32	4	/* write full 32 bit word with value */
#define RELOC_SYM	0x100	/* symbol flag, may be added to any RELOC */


typedef struct {
  unsigned int magic;		/* must be EXEC_MAGIC */
  unsigned int osegs;		/* offset of segment table in file */
  unsigned int nsegs;		/* number of segment table entries */
  unsigned int osyms;		/* offset of symbol table in file */
  unsigned int nsyms;		/* number of symbol table entries */
  unsigned int orels;		/* offset of relocation table in file */
  unsigned int nrels;		/* number of relocation table entries */
  unsigned int odata;		/* offset of segment data in file */
  unsigned int sdata;		/* size of segment data in file */
  unsigned int ostrs;		/* offset of string space in file */
  unsigned int sstrs;		/* size of string space in file */
  unsigned int entry;		/* entry point (if executable) */
} ExecHeader;


typedef struct {
  unsigned int name;		/* offset in string space */
  unsigned int offs;		/* offset in segment data */
  unsigned int addr;		/* virtual start address */
  unsigned int size;		/* size of segment in bytes */
  unsigned int attr;		/* segment attributes */
} SegmentRecord;


typedef struct {
  unsigned int name;		/* offset in string space */
  int val;			/* the symbol's value */
  int seg;			/* the symbol's segment, -1: absolute */
  unsigned int attr;		/* symbol attributes */
} SymbolRecord;


typedef struct {
  unsigned int loc;		/* where to relocate */
  int seg;			/* in which segment */
  int typ;			/* relocation type: one of RELOC_xxx */
				/* symbol flag RELOC_SYM may be set */
  int ref;			/* what is referenced */
				/* if symbol flag = 0: segment number */
				/* if symbol flag = 1: symbol number */
  int add;			/* additive part of value */
} RelocRecord;


#endif /* _A_OUT_H_ */
