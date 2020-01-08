/*
 * graph1.h -- graphics controller 1 simulation
 */


#ifndef _GRAPH1_H_
#define _GRAPH1_H_


Word graph1Read(Word addr);
void graph1Write(Word addr, Word data);

void graph1Reset(void);
void graph1Init(void);
void graph1Exit(void);


#endif /* _GRAPH1_H_ */
