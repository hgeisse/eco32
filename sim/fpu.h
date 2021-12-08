/*
 * fpu.h -- floating-point unit
 */


#ifndef _FPU_H_
#define _FPU_H_


#define FPU_COND	0x80000000	/* result of last comparison */

#define FPU_FLAG_V	0x0100		/* invalid */
#define FPU_FLAG_I	0x0080		/* infinite */
#define FPU_FLAG_O	0x0040		/* overflow */
#define FPU_FLAG_U	0x0020		/* underflow */
#define FPU_FLAG_X	0x0010		/* inexact */

#define FPU_RND_NEAR	0x0000		/* to nearest, ties to even */
#define FPU_RND_ZERO	0x0001		/* toward zero */
#define FPU_RND_DOWN	0x0002		/* toward negative infinity */
#define FPU_RND_UP	0x0003		/* toward positive infinity */
#define FPU_RND_MASK	0x0003		/* mask for rounding mode */

#define FPU_PRED_EQ	0		/* equal */
#define FPU_PRED_NE	1		/* not equal */
#define FPU_PRED_LE	2		/* less or equal */
#define FPU_PRED_LT	3		/* less than */
#define FPU_PRED_ULE	4		/* unordered or less or equal */
#define FPU_PRED_ULT	5		/* unordered or less than */


Word fpuAdd(Word x, Word y);
Word fpuSub(Word x, Word y);
Word fpuMul(Word x, Word y);
Word fpuDiv(Word x, Word y);
Word fpuSqrt(Word x);

Word fpuCnvI2F(Word x);
Word fpuCnvF2I(Word x, int rnd);

void fpuCmp(Word x, Word y, int pred);

Word fpuGetFPC(void);
void fpuSetFPC(Word value);

void fpuReset(void);
void fpuInit(void);
void fpuExit(void);


#endif /* _FPU_H_ */
