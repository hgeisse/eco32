/*
 * start.h -- startup code
 */


#ifndef _START_H_
#define _START_H_


typedef unsigned int Word;
typedef unsigned char Byte;
typedef int (*ISR)(int irq);


void enable(void);
void disable(void);
Word getMask(void);
void setMask(Word mask);
ISR getISR(int irq);
void setISR(int irq, ISR isr);


#endif /* _START_H_ */
