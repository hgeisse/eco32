/*
 * boot.h -- bootstrap from disk
 */


#ifndef _BOOT_H_
#define _BOOT_H_


#define PHYS_BOOT	0x00010000	/* where to load the bootstrap */
#define VIRT_BOOT	0xC0010000	/* where to start the bootstrap */
#define TOS_BOOT	0xC0011000	/* top of stack for bootstrap */


void boot(int dskno);


#endif /* _BOOT_H_ */
