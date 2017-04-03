/*
 * sdcard.h -- SD card simulation
 */


#ifndef _SDCARD_H_
#define _SDCARD_H_


#define SDC_SECTOR_SIZE		512

#define SDC_CTRL_SELECT		0x01
#define SDC_CTRL_FASTCLK	0x02
#define SDC_CTRL_CRC16MISO	0x04

#define SDC_STAT_READY		0x01
#define SDC_STAT_WPROT		0x02

#define SDC_ANS_PARAM_ERR	0x40
#define SDC_ANS_ADDR_ERR	0x20
#define SDC_ANS_ERASE_SEQ_ERR	0x10
#define SDC_ANS_COM_CRC_ERR	0x08
#define SDC_ANS_ILLEGAL_CMD	0x04
#define SDC_ANS_ERASE_RESET	0x02
#define SDC_ANS_IN_IDLE_STATE	0x01


Word sdcardRead(Word addr);
void sdcardWrite(Word addr, Word data);

void sdcardReset(void);
void sdcardInit(char *sdcardImageName);
void sdcardExit(void);


#endif /* _SDCARD_H_ */
