/*
 * rom.h -- ROM simulation
 */


#ifndef _ROM_H_
#define _ROM_H_


void romRead(Word pAddr, Word *dst, int nWords);
void romWrite(Word pAddr, Word *src, int nWords);

void romReset(void);
void romInit(char *progImageName);
void romExit(void);


#endif /* _ROM_H_ */
