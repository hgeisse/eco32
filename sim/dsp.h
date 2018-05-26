/*
 * dsp.h -- display simulation
 */


#ifndef _DSP_H_
#define _DSP_H_


Word displayRead(Word addr);
void displayWrite(Word addr, Word data);

void displayReset(void);
void displayInit(void);
void displayExit(void);


#endif /* _DSP_H_ */
