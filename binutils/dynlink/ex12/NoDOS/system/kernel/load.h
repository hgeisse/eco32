/*
 * load.h -- program loader
 */


#ifndef _LOAD_H_
#define _LOAD_H_


#define LDERR_NONE	0	/* no error */
#define LDERR_FND	1	/* not found */
#define LDERR_HDR	2	/* cannot read header */
#define LDERR_MGC	3	/* wrong magic number */
#define LDERR_STR	4	/* cannot read strings */
#define LDERR_SEG	5	/* cannot read segments */
#define LDERR_DAT	6	/* cannot read data */


int loadExecutable(char *execPath, unsigned int *startAddr);
void unload(void);


#endif /* _LOAD_H_ */
