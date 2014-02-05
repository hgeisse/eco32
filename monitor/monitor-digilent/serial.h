/*
 * serial.s -- the serial line interface
 */


#ifndef _SERIAL_H_
#define _SERIAL_H_


int ser0inchk(void);
int ser0in(void);
int ser0outchk(void);
void ser0out(int c);

int ser1inchk(void);
int ser1in(void);
int ser1outchk(void);
void ser1out(int c);


#endif /* _SERIAL_H_ */
