/*
 * icache.h -- instruction cache simulation
 */


#ifndef _ICACHE_H_
#define _ICACHE_H_


#define IC_LD_TOTAL_SIZE	12	/* ld(total size in bytes) */
#define IC_LD_LINE_SIZE		4	/* ld(line size in bytes) */
#define IC_TWO_WAY_ASSOC	false	/* false: direct mapped */
					/* true: two way associative */


Word icacheReadWord(Word pAddr);

void icacheInvalidate(void);

void icacheReset(void);
void icacheInit(void);
void icacheExit(void);


#endif /* _ICACHE_H_ */
