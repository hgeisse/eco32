/*
 * icache.h -- instruction cache simulation
 */


#ifndef _ICACHE_H_
#define _ICACHE_H_


Word icacheReadWord(Word pAddr);

void icacheInvalidate(void);

void icacheReset(void);
void icacheInit(void);
void icacheExit(void);


#endif /* _ICACHE_H_ */
