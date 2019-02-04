/*
 * stdarg.h -- variable argument lists
 */


#ifndef _STDARG_H_
#define _STDARG_H_


#ifndef _VA_LIST_DEFINED_
#define _VA_LIST_DEFINED_
typedef char *va_list;
#endif


#define va_start(list,last)	((void) ((list) = ( \
				    sizeof(last) < 4 ? \
				      (char *) ((int *) &(last) + 1) : \
				      (char *) (&(last) + 1) \
				  )))

#define va_arg(list,type)	()

#define va_end(list)		((void) 0)


#endif /* _STDARG_H_ */
