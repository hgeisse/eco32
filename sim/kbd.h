/*
 * kbd.h -- keyboard simulation
 */


#ifndef _KBD_H_
#define _KBD_H_


#define KEYBOARD_CTRL		0	/* keyboard control register */
#define KEYBOARD_DATA		4	/* keyboard data register */

#define KEYBOARD_RCVR_RDY	0x01	/* receiver has a character */
#define KEYBOARD_RCVR_IEN	0x02	/* enable receiver interrupt */
#define KEYBOARD_RCVR_ERR	0x04	/* receiver detected an error */
#define KEYBOARD_RCVR_USEC	2000	/* input checking interval */

#define KEYBOARD_XMTR_RDY	0x10	/* transmitter accepts a character */
#define KEYBOARD_XMTR_IEN	0x20	/* enable transmitter interrupt */
#define KEYBOARD_XMTR_USEC	1042	/* output speed */


void keyPressed(unsigned int xKeycode);
void keyReleased(unsigned int xKeycode);

Word keyboardRead(Word addr);
void keyboardWrite(Word addr, Word data);

void keyboardReset(void);
void keyboardInit(void);
void keyboardExit(void);


#endif /* _KBD_H_ */
