/*
 * fp.h -- floating-point representation
 */


#ifndef _FP_H_
#define _FP_H_


#define _FP_V_FLAG	0x10		/* invalid */
#define _FP_I_FLAG	0x08		/* infinite */
#define _FP_O_FLAG	0x04		/* overflow */
#define _FP_U_FLAG	0x02		/* underflow */
#define _FP_X_FLAG	0x01		/* inexact */


#define _FP_SGN(x)	((x) >> 31)
#define _FP_EXP(x)	(((x) << 1) >> 24)
#define _FP_FRC(x)	(((x) << 9) >> 9)
#define _FP_FLT(s,e,f)	(((s) << 31) | ((e) << 23) | (f))


typedef unsigned int _FP_Word;

typedef union {
  float f;
  _FP_Word w;
} _FP_Union;


#endif /* _FP_H_ */
