/*
 * mouse.h -- mouse simulation
 */


#ifndef _MOUSE_H_
#define _MOUSE_H_


#define MOUSE_CTRL		0	/* mouse control register */
#define MOUSE_DATA		4	/* mouse data register */

#define MOUSE_RDY		0x01	/* mouse has a byte */
#define MOUSE_IEN		0x02	/* enable mouse interrupt */
#define MOUSE_USEC		2000	/* input checking interval */


void mouseMoved(int xMotionX, int xMotionY);
void mouseButtonPressed(unsigned int xButton);
void mouseButtonReleased(unsigned int xButton);

Word mouseRead(Word addr);
void mouseWrite(Word addr, Word data);

void mouseReset(void);
void mouseInit(void);
void mouseExit(void);


#endif /* _MOUSE_H_ */
