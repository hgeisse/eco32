/*
 * fpu.h -- floating-point unit
 */


#ifndef _FPU_H_
#define _FPU_H_


#define FP_CMP_LT	0		/* less than */
#define FP_CMP_EQ	1		/* equal */
#define FP_CMP_GT	2		/* greater than */
#define FP_CMP_UO	3		/* unordered */

#define FP_V_FLAG	0x10		/* invalid */
#define FP_I_FLAG	0x08		/* infinite */
#define FP_O_FLAG	0x04		/* overflow */
#define FP_U_FLAG	0x02		/* underflow */
#define FP_X_FLAG	0x01		/* inexact */


Word fpAdd(Word x, Word y);
Word fpSub(Word x, Word y);
Word fpMul(Word x, Word y);
Word fpDiv(Word x, Word y);

Word fpCnvF2I(Word x);
Word fpCnvI2F(Word x);

int fpCmp(Word x, Word y, Bool invalidIfUnordered);


#endif /* _FPU_H_ */
