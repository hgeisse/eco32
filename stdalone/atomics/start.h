/*
 * start.h -- startup code
 */


#ifndef _START_H_
#define _START_H_


typedef void (*ISR)(int irq, unsigned int *registers);


void spinLock(int *mutex);
void enable(void);
void disable(void);
void orMask(unsigned int mask);
void andMask(unsigned int mask);
ISR getISR(int irq);
void setISR(int irq, ISR isr);


#endif /* _START_H_ */
