/*
 * mouse.h -- mouse simulation
 */


#ifndef _MOUSE_H_
#define _MOUSE_H_


#define MOUSE_CTRL		0	/* mouse control register */
#define MOUSE_DATA		4	/* mouse data register */

#define MOUSE_RCVR_RDY		0x01	/* receiver has a byte */
#define MOUSE_RCVR_IEN		0x02	/* enable receiver interrupt */
#define MOUSE_RCVR_ERR		0x04	/* receiver detected an error */
#define MOUSE_RCVR_USEC		2000	/* input checking interval */

#define MOUSE_XMTR_RDY		0x10	/* transmitter accepts a byte */
#define MOUSE_XMTR_IEN		0x20	/* enable transmitter interrupt */
#define MOUSE_XMTR_USEC		1042	/* output speed */


void mouseMoved(int xMotionX, int xMotionY);
void mouseButtonPressed(unsigned int xButton);
void mouseButtonReleased(unsigned int xButton);

Word mouseRead(Word addr);
void mouseWrite(Word addr, Word data);

void mouseReset(void);
void mouseInit(void);
void mouseExit(void);


#endif /* _MOUSE_H_ */
