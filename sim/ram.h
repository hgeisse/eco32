/*
 * ram.h -- RAM simulation
 */


#ifndef _RAM_H_
#define _RAM_H_


void ramRead(Word pAddr, Word *data, int nWords);
void ramWrite(Word pAddr, Word *data, int nWords);

void ramReset(void);
void ramInit(void);
void ramExit(void);


#endif /* _RAM_H_ */
