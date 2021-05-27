/*
 * eof.h -- ECO32 Object Format
 *          define structure of linkable object and executable files
 */


#ifndef _EOF_H_
#define _EOF_H_


#define EOF_R_MAGIC	0xE7B79BFE	/* relocatable object */
#define EOF_X_MAGIC	0x3AECCFDF	/* executable object */
#define EOF_D_MAGIC	0xFB23146C	/* dynamic library object */

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
#define RELOC_GA_H16	5	/* PIC: GOT addr, PC relative, high part */
#define RELOC_GA_L16	6	/* PIC: GOT addr, PC relative, low part */
#define RELOC_GR_H16	7	/* PIC: GOT relative, high part */
#define RELOC_GR_L16	8	/* PIC: GOT relative, low part */
#define RELOC_GP_L16	9	/* PIC: GOT pointer, offset within GOT */
#define RELOC_ER_W32	10	/* PIC: executable relative, 32 bit word */
#define RELOC_SYM	0x100	/* symbol flag, may be added to any RELOC */


typedef struct {
  unsigned int magic;		/* must be one of EOF_<type>_MAGIC */
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
} EofHeader;


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
				/* if -1: none, loc is virtual address */
  int typ;			/* relocation type: one of RELOC_xxx */
				/* symbol flag RELOC_SYM may be set */
  int ref;			/* what is referenced */
				/* if -1: nothing */
				/* if symbol flag = 0: segment number */
				/* if symbol flag = 1: symbol number */
  int add;			/* additive part of value */
} RelocRecord;


#endif /* _EOF_H_ */