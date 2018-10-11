/*
 * icache.h -- instruction cache simulation
 */


#ifndef _ICACHE_H_
#define _ICACHE_H_


#define IC_LD_TOTAL_SIZE	12	/* ld(total size in bytes) */
#define IC_LD_LINE_SIZE		4	/* ld(line size in bytes) */
#define IC_LD_ASSOC		0	/* 0: direct-mapped */
					/* 1: two-way associative */


Word icacheReadWord(Word pAddr);

void icacheInvalidate(void);

long icacheGetReadAccesses(void);
long icacheGetReadMisses(void);

Bool icacheProbe(Word pAddr, Word *data);

void icacheReset(void);
void icacheInit(int ldTotal, int ldLine, int ldAss);
void icacheExit(void);


#endif /* _ICACHE_H_ */
