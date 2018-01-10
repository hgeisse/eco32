/*
 * dcache.h -- data cache simulation
 */


#ifndef _DCACHE_H_
#define _DCACHE_H_


Word dcacheReadWord(Word pAddr);
Half dcacheReadHalf(Word pAddr);
Byte dcacheReadByte(Word pAddr);
void dcacheWriteWord(Word pAddr, Word data);
void dcacheWriteHalf(Word pAddr, Half data);
void dcacheWriteByte(Word pAddr, Byte data);

void dcacheReset(void);
void dcacheInit(void);
void dcacheExit(void);


#endif /* _DCACHE_H_ */
