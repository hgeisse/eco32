/*
 * bio.h -- board I/O
 */


#ifndef _BIO_H_
#define _BIO_H_


Word bioRead(Word addr);
void bioWrite(Word addr, Word data);

void bioShowBoard(void);
void bioSetSwitches(Word data);

void bioReset(void);
void bioInit(Word initialSwitches);
void bioExit(void);


#endif /* _BIO_H_ */
