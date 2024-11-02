/*
 * pxd.h -- packet exchange device
 */


#ifndef _PXD_H_
#define _PXD_H_


Word pxdRead(Word addr);
void pxdWrite(Word addr, Word data);

void pxdReset(void);
void pxdInit(void);
void pxdExit(void);


#endif /* _PXD_H_ */
