/*
 * ar.h -- structure of static library files
 */


#ifndef _AR_H_
#define _AR_H_


#define ARCH_MAGIC	0x0412CF03


typedef struct {
  unsigned int magic;		/* must be ARCH_MAGIC */
  unsigned int omods;		/* offset of module table in file */
  unsigned int nmods;		/* number of module table entries */
  unsigned int odata;		/* offset of data space in file */
  unsigned int sdata;		/* size of data space in file */
  unsigned int ostrs;		/* offset of string space in file */
  unsigned int sstrs;		/* size of string space in file */
} ArchHeader;


typedef struct {
  unsigned int name;		/* offset of name in string space */
  unsigned int offs;		/* offset of module in data space */
  unsigned int size;		/* size of module in bytes */
  unsigned int fsym;		/* first symbol name in string space */
  unsigned int nsym;		/* number of symbol names */
} ModuleRecord;


#endif /* _AR_H_ */
