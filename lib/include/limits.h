/*
 * limits.h -- limits of integral types
 */


#ifndef _LIMITS_H_
#define _LIMITS_H_


#define CHAR_BIT	8

#define UCHAR_MAX	255
#define SCHAR_MAX	127
#define SCHAR_MIN	(-SCHAR_MAX - 1)
#define CHAR_MAX	SCHAR_MAX
#define CHAR_MIN	SCHAR_MIN

#define USHRT_MAX	65535
#define SHRT_MAX	32767
#define SHRT_MIN	(-SHRT_MAX - 1)

#define UINT_MAX	4294967295U
#define INT_MAX		2147483647
#define INT_MIN		(-INT_MAX - 1)

#define ULONG_MAX	4294967295UL
#define LONG_MAX	2147483647L
#define LONG_MIN	(-LONG_MAX - 1)


#endif /* _LIMITS_H_ */
