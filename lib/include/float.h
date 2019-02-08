/*
 * float.h -- characteristic of floating-point types
 */


#ifndef _FLOAT_H_
#define _FLOAT_H_


/*
 * Definitions:
 *
 * FLT_RADIX
 *   Radix of exponent representation.
 *       b
 *
 * FLT_MANT_DIG
 *   Number of base-FLT_RADIX digits in the significand.
 *       p
 *
 * FLT_DIG
 *   Number of decimal digits, q, such that any floating-point number
 *   with q decimal digits can be rounded into a floating-point number
 *   with p radix b digits and back again without change to the
 *   q decimal digits.
 *       p * log10(b)                    if b is a power of 10
 *       floor((p - 1) * log10(b))       otherwise
 *
 * FLT_MIN_EXP
 *   Minimum int x such that FLT_RADIX**(x-1) is a normalized float.
 *       emin
 *
 * FLT_MIN_10_EXP
 *   Minimum negative integer such that 10 raised to that power is in
 *   the range of normalized floating-point numbers.
 *       ceil(log10(b) * (emin - 1))
 *
 * FLT_MAX_EXP
 *   Maximum int x such that FLT_RADIX**(x-1) is a representable float.
 *       emax
 *
 * FLT_MAX_10_EXP
 *   Maximum integer such that 10 raised to that power is in the range
 *   of representable finite floating-point numbers.
 *       floor(log10((1 - b**-p) * b**emax))
 *
 * FLT_MAX
 *   Maximum representable finite floating-point number.
 *       (1 - b**-p) * b**emax
 *
 * FLT_EPSILON
 *   The difference between 1 and the least value greater than 1 that
 *   is representable in the given floating point type.
 *       b**(1-p)
 *
 * FLT_MIN
 *   Minimum normalized positive floating-point number.
 *       b**(emin - 1)
 *
 * FLT_ROUNDS
 *   Addition rounds to
 *       -1: unknown, 0: zero, 1: nearest, 2: +inf, 3: -inf
 */


#define FLT_RADIX	2

#define FLT_MANT_DIG	24
#define FLT_DIG		6
#define FLT_MIN_EXP	-125
#define FLT_MIN_10_EXP	-37
#define FLT_MAX_EXP	128
#define FLT_MAX_10_EXP	38
#define FLT_MAX		3.402823466385e+38
#define FLT_EPSILON	1.192092895508e-7
#define FLT_MIN		1.175494350822e-38

#define DBL_MANT_DIG	FLT_MANT_DIG
#define DBL_DIG		FLT_DIG
#define DBL_MIN_EXP	FLT_MIN_EXP
#define DBL_MIN_10_EXP	FLT_MIN_10_EXP
#define DBL_MAX_EXP	FLT_MAX_EXP
#define DBL_MAX_10_EXP	FLT_MAX_10_EXP
#define DBL_MAX		FLT_MAX
#define DBL_EPSILON	FLT_EPSILON
#define DBL_MIN		FLT_MIN

#define FLT_ROUNDS	1


#endif /* _FLOAT_H_ */
