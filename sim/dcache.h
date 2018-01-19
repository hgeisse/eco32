/*
 * dcache.h -- data cache simulation
 */


#ifndef _DCACHE_H_
#define _DCACHE_H_


#define DC_LD_TOTAL_SIZE	12	/* ld(total size in bytes) */
#define DC_LD_LINE_SIZE		4	/* ld(line size in bytes) */
#define DC_TWO_WAY_ASSOC	false	/* false: direct mapped */
					/* true: two way associative */


Word dcacheReadWord(Word pAddr);
Half dcacheReadHalf(Word pAddr);
Byte dcacheReadByte(Word pAddr);
void dcacheWriteWord(Word pAddr, Word data);
void dcacheWriteHalf(Word pAddr, Half data);
void dcacheWriteByte(Word pAddr, Byte data);

void dcacheInvalidate(void);
void dcacheFlush(void);

void dcacheReset(void);
void dcacheInit(void);
void dcacheExit(void);


#endif /* _DCACHE_H_ */
