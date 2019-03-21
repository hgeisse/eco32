/*
 * fpu.h -- floating-point unit
 */


#ifndef _FPU_H_
#define _FPU_H_


#define FP_CMP_LT	0		/* less than */
#define FP_CMP_EQ	1		/* equal */
#define FP_CMP_GT	2		/* greater than */
#define FP_CMP_UO	3		/* unordered */


Word fpAdd(Word x, Word y);
Word fpSub(Word x, Word y);
Word fpMul(Word x, Word y);
Word fpDiv(Word x, Word y);

int fpCmp(Word x, Word y);


#endif /* _FPU_H_ */
