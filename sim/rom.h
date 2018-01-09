/*
 * rom.h -- ROM simulation
 */


#ifndef _ROM_H_
#define _ROM_H_


void romRead(Word pAddr, Word *dst, int nWords);

void romReset(void);
void romInit(char *romImageName);
void romExit(void);


#endif /* _ROM_H_ */
