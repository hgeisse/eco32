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

#define _va_arg_size(type)	((sizeof(type) + 3) & ~3U)
#define _va_arg_1(list,type)	(* (type *) (&(list += 4)[-1]))
#define _va_arg_2(list,type)	(* (type *) (&(list += 4)[-2]))
#define _va_arg_n(list,type)	(* (type *) (&(list += _va_arg_size(type)) \
				    [- (int) _va_arg_size(type)]))

#define va_arg(list,type)	(sizeof(type) == 1 ? \
				    _va_arg_1(list, type) : \
				 sizeof(type) == 2 ? \
				    _va_arg_2(list, type) : \
				    _va_arg_n(list,type))

#define va_end(list)		((void) 0)


#endif /* _STDARG_H_ */
