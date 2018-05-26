/*
 * kbd.h -- keyboard simulation
 */


#ifndef _KBD_H_
#define _KBD_H_


#define KEYBOARD_CTRL		0	/* keyboard control register */
#define KEYBOARD_DATA		4	/* keyboard data register */

#define KEYBOARD_RDY		0x01	/* keyboard has a character */
#define KEYBOARD_IEN		0x02	/* enable keyboard interrupt */
#define KEYBOARD_USEC		2000	/* input checking interval */


void keyPressed(unsigned int xKeycode);
void keyReleased(unsigned int xKeycode);

Word keyboardRead(Word addr);
void keyboardWrite(Word addr, Word data);

void keyboardReset(void);
void keyboardInit(void);
void keyboardExit(void);


#endif /* _KBD_H_ */
