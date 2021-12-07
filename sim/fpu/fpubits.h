/*
 * fpubits.h -- FPU internal definitions
 */


#ifndef _FPUBITS_H_
#define _FPUBITS_H_


/* !!!!! to be deleted !!!!! */
typedef union {
  float f;
  Word w;
} _FP_Union;


extern Word fpc;	/* floating-point control word */


#endif /* _FPUBITS_H_ */
