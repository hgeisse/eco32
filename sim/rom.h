/*
 * rom.h -- ROM simulation
 */


#ifndef _ROM_H_
#define _ROM_H_


void romRead(Word pAddr, Word *data, int nWords);
void romWrite(Word pAddr, Word *data, int nWords);

void romReset(void);
void romInit(void);
void romExit(void);


#endif /* _ROM_H_ */
