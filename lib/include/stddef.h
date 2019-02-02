/*
 * stddef.h -- common definitions
 */


#ifndef _STDDEF_H_
#define _STDDEF_H_


#ifndef NULL
#define NULL		((void *) 0)
#endif

#ifndef offsetof
#define offsetof(t,m)	((size_t) ((char *) &((t *) 0)->m - (char *) 0))
#endif


#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef unsigned long size_t;
#endif

#ifndef _PTRDIFF_T_DEFINED_
#define _PTRDIFF_T_DEFINED_
typedef long ptrdiff_t;
#endif


#endif /* _STDDEF_H_ */
