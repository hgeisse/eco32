/*
 * fp.h -- floating-point representation
 */


#ifndef _FP_H_
#define _FP_H_


#define _FP_V_FLAG	0x10
#define _FP_I_FLAG	0x08
#define _FP_O_FLAG	0x04
#define _FP_U_FLAG	0x02
#define _FP_X_FLAG	0x01


#define _FP_SGN(x)	((x) & 0x80000000)
#define _FP_EXP(x)	(((x) << 1) >> 24)
#define _FP_FRC(x)	((x) & 0x7FFFFF)
#define _FP_FLT(s,e,f)	(s | (e << 23) | f)

#define _FP_ABS(x)	((x) < 0 ? -(x) : (x))
#define _FP_DELTA(Z,R)	((int) (Z.w & 0x7FFFFFFF) - (int) (R.w & 0x7FFFFFFF))


typedef unsigned int _FP_Word;

typedef union {
  float f;
  _FP_Word w;
} _FP_Union;


#endif /* _FP_H_ */
