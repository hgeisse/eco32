/*
 * fpu.h -- floating-point unit
 */


#ifndef _FPU_H_
#define _FPU_H_


#define FPU_CMP_LT	0		/* less than */
#define FPU_CMP_EQ	1		/* equal */
#define FPU_CMP_GT	2		/* greater than */
#define FPU_CMP_UO	3		/* unordered */

#define FPU_V_FLAG	0x10		/* invalid */
#define FPU_I_FLAG	0x08		/* infinite */
#define FPU_O_FLAG	0x04		/* overflow */
#define FPU_U_FLAG	0x02		/* underflow */
#define FPU_X_FLAG	0x01		/* inexact */


Word fpuAdd(Word x, Word y);
Word fpuSub(Word x, Word y);
Word fpuMul(Word x, Word y);
Word fpuDiv(Word x, Word y);

Word fpuCnvF2I(Word x);
Word fpuCnvI2F(Word x);

int fpuCmp(Word x, Word y, Bool invalidIfUnordered);

void fpuReset(void);
void fpuInit(void);
void fpuExit(void);


#endif /* _FPU_H_ */
