/*
 * graph2.h -- graphics controller 2 simulation
 */


#ifndef _GRAPH2_H_
#define _GRAPH2_H_


Word graph2Read(Word addr);
void graph2Write(Word addr, Word data);

void graph2Reset(void);
void graph2Init(void);
void graph2Exit(void);


#endif /* _GRAPH2_H_ */
