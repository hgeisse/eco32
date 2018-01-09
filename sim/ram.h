/*
 * ram.h -- RAM simulation
 */


#ifndef _RAM_H_
#define _RAM_H_


void ramRead(Word pAddr, Word *dst, int nWords);
void ramWrite(Word pAddr, Word *src, int nWords);

void ramReset(void);
void ramInit(unsigned int memorySize,
             char *progImageName,
             unsigned int loadAddr);
void ramExit(void);


#endif /* _RAM_H_ */
