/*
 * mouse.h -- mouse simulation
 */


#ifndef _MOUSE_H_
#define _MOUSE_H_


Word mouseRead(Word addr);
void mouseWrite(Word addr, Word data);

void mouseReset(void);
void mouseInit(void);
void mouseExit(void);


#endif /* _MOUSE_H_ */
