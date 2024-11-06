/*
 * pxd.h -- packet exchange device
 */


#ifndef _PXD_H_
#define _PXD_H_


/*
 * internal interface
 */


#define PXD_RCV_CTRL		0x00	/* receiver control register */
#define PXD_RCV_TYPE		0x04	/* receiver packet type register */
#define PXD_RCV_SIZE		0x08	/* receiver data size register */
#define PXD_RCV_BUFFER		0x80000	/* receiver data buffer start */

#define PXD_RCV_RDY		0x01	/* receiver has a packet */
#define PXD_RCV_OVER		0x02	/* receiver has been overrun */
#define PXD_RCV_RLS		0x10	/* release receiver */
#define PXD_RCV_IEN		0x20	/* enable receiver interrupt */

#define PXD_RCV_USEC		100	/* input checking interval */

#define PXD_XMT_CTRL		0x10	/* transmitter control register */
#define PXD_XMT_TYPE		0x14	/* transmitter packet type register */
#define PXD_XMT_SIZE		0x18	/* transmitter data size register */
#define PXD_XMT_BUFFER		0x81000	/* transmitter data buffer start */

#define PXD_XMT_RDY		0x01	/* transmitter accepts a packet */
#define PXD_XMT_UNDR		0x02	/* transmitter has been underrun */
#define PXD_XMT_RLS		0x10	/* release transmitter */
#define PXD_XMT_IEN		0x20	/* enable transmitter interrupt */

#define PXD_XMT_USEC		100	/* output speed */


Word pxdRead(Word addr);
void pxdWrite(Word addr, Word data);

void pxdReset(void);
void pxdInit(void);
void pxdExit(void);


/*
 * external interface
 */


#define PXD_SERVER_PORT		48765

#define PXD_MAX_SIZE		1024


typedef struct {
  Word type;			/* type of packet */
  Word size;			/* size of data */
				/* up to PXD_MAX_SIZE words, may be 0 */
  Word data[PXD_MAX_SIZE];	/* followed by <size> words of data */
} Packet;


#endif /* _PXD_H_ */
