/*
 * common.h -- common types, etc.
 */


#ifndef _COMMON_H_
#define _COMMON_H_


#include <errno.h>
#include <err.h>


#ifdef __linux__

#include <byteswap.h>

#define bswap16(a)	bswap_16(a)
#define bswap32(a)	bswap_32(a)
#define bswap64(a)	bswap_64(a)

#endif


typedef unsigned long long uintmax_t;

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;


#define MAXPHYS		(64 * 1024)     /* max raw I/O transfer size */
#define MAXQUOTAS	2


#endif /* _COMMON_H_ */
