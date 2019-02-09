/* -*-C-*- elefunt.h */

#define PRECISION	1	/* single precision only */

#include "f2d.h"

/* Header file for Cody & Waite Elementary Function Test Package -- C Version */
/* This file is included by ../{dp,qp,sp}/elefunt.h, each of which set PRECISION first */

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <unistd.h> */

#ifdef MSC
#include <float.h>	/* need for _control87() in machar.c */
#endif

#ifdef __TURBOC__
#include <float.h>	/* need for _control87() in machar.c */
#endif

/* To facilitate code movement between precisions, we use private
   types sp_t, dp_t, and qp_t instead of the usual float, double, and
   long double.  We use uppercase generic elementary function names
   defined near the end of this file. */

#if defined(SP_T)		       /* allow compile-time choice */
typedef SP_T sp_t;
#else
typedef float sp_t;
#endif

#if defined(DP_T)		       /* allow compile-time choice */
typedef DP_T dp_t;
#else
typedef double dp_t;
#endif

#if PRECISION == 4			/* expose long double only when needed */
#if defined(QP_T) /* allow compile-time choice: not long double everywhere */
typedef QP_T qp_t;
#else
typedef long double qp_t;
#endif
#endif

#ifndef INITIALSEED			/* allow compile-time choice */
#define INITIALSEED 0
#endif

#if INITIALSEED < 0L
#define INITIALSEED (-INITIALSEED)
#endif

#if INITIALSEED < 0L			/* must have been LONG_MIN */
#define INITIALSEED LONG_MAX
#endif

#ifndef MAXTEST				/* allow compile-time choice */
#define MAXTEST (2000)
#endif

#if MAXTEST <= 0			/* prevent nonsensical value */
#define MAXTEST 2000			/* ELEFUNT default */
#endif

#if PRECISION == 1
#define ONE 1.0F
#define ZERO 0.0F
#define TWO 2.0F
#define TEN 10.0F
#endif

#if PRECISION == 2
#define ONE 1.0
#define ZERO 0.0
#define TWO 2.0
#define TEN 10.0
#endif

#if PRECISION == 4
#define ONE 1.0L
#define ZERO 0.0L
#define TWO 2.0L
#define TEN 10.0L
#endif

#ifdef VMS

#ifndef EXIT_FAILURE
#define EXIT_FAILURE	((1 << 28) + 2) /* (suppresses %NONAME-E-NOMSG) */
#endif /* EXIT_FAILURE */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	(1)
#endif /* EXIT_SUCCESS */

#else /* NOT VMS */

#ifndef EXIT_FAILURE
#define EXIT_FAILURE	(1)
#endif /* EXIT_FAILURE */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	(0)
#endif /* EXIT_SUCCESS */

#endif /* VMS */

#ifdef PCC_20
typedef int void;
#endif

#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#define	STDC	1
#define ARGS(parenthesized_list) parenthesized_list
#define VOIDP	void*
#define VOID_ARG void

#else  /* NOT STDC */
/* Provide some synonyms for new ANSI C features */

#define	STDC	0
#define	const
#define	volatile
#define VOID_ARG

#ifdef __GNUC__			/* GNU gcc supports ANSI prototypes */
#define ARGS(parenthesized_list) parenthesized_list
#else
#ifdef ardent			/* Ardent cc supports ANSI prototypes */
#define ARGS(parenthesized_list) parenthesized_list
#else
#define ARGS(parenthesized_list) ()
#endif /* ardent */
#endif /* __GNUC__ */
#endif /* __STDC__ */

extern long	initseed ARGS((void));
extern long	maxtest ARGS((void));

extern sp_t	ipowf ARGS((sp_t x, int n));
extern sp_t	sran ARGS((void));
extern sp_t	srandl ARGS((sp_t x));
extern long	sranset ARGS((long));
extern sp_t	storef ARGS((sp_t *p));

extern dp_t	ipow ARGS((dp_t x, int n));
extern dp_t	ran ARGS((void));
extern dp_t	randl ARGS((dp_t x));
extern long	ranset ARGS((long));
extern dp_t	store ARGS((dp_t *p));

#if PRECISION == 4
extern qp_t	ipowl ARGS((qp_t x, int n));
extern qp_t	qran ARGS((void));
extern qp_t	qrandl ARGS((qp_t x));
extern long	qranset ARGS((long));
extern qp_t	storel ARGS((qp_t *p));
#endif

#if 0
/* This is lacking on most systems */
extern sp_t	cotanf ARGS((sp_t));
extern dp_t	cotan ARGS((dp_t));
#if PRECISION == 4
extern qp_t	cotanl ARGS((qp_t));
#endif
#else
#define cotanf(x) (ONE/(tanf)(x))	/* not defined in most C libraries */
#define cotan(x) (ONE/(tan)(x))		/* not defined in most C libraries */
#define cotanl(x) (ONE/(tanl)(x))	/* not defined in most C libraries */
#endif

extern void	talog ARGS((void));
extern void	tasin ARGS((void));
extern void	tatan ARGS((void));
extern void	texp ARGS((void));
extern void	tpower ARGS((void));
extern void	tsin ARGS((void));
extern void	tsinh ARGS((void));
extern void	tmacha ARGS((void));
extern void	tsqrt ARGS((void));
extern void	ttan ARGS((void));
extern void	ttanh ARGS((void));


#undef ABS
#define ABS(x)  (((x) < 0) ? -(x) : (x))

#undef AINT
#define AINT(x) TO_FP_T((long)(x))

#undef AMAX1
#define AMAX1(x,y) (((x) >= (y)) ? (x) : (y))

#undef AMIN1
#define AMIN1(x,y) (((x) <= (y)) ? (x) : (y))

#undef IABS
#define IABS(x) (((x) < 0) ? -(x) : (x))

#undef INT
#define INT(x) ((int)(x))

#define SIGN(x,y)  (((y) >= 0) ? ABS(x) : -ABS(x))

/* Define generic names for library functions, to ease porting code
   from one precision to another. */

#if PRECISION == 1
#ifdef __sun
#include <sunmath.h>	/* need for float function prototypes on Sun Solaris 2.x */
#endif

extern void	macharf ARGS((int* ibeta, int* it, int* irnd, int* ngrd,
			int* machep, int* negep, int* iexp, int* minexp,
			int* maxexp, sp_t* eps, sp_t* epsneg,
			sp_t* xmin, sp_t* xmax));

#define	ACOS	(acosf)
#define ALOG	(logf)
#define ALOG10	(log10f)
#define	ASIN	(asinf)
#define	ATAN	(atanf)
#define	ATAN2	(atan2f)
#define	COS	(cosf)
#define	COSH	(coshf)
#define	COTAN	cotanf			/* special case */
#define	EXP	(expf)
#define	FLOOR	(floorf)
#define	IPOW	(ipowf)
#define	LDEXP	(ldexpf)
#define	LOG	(logf)
#define	LOG10	(log10f)
#define	MODF	(modff)
#define	POW	(powf)
#define	RAN	(sran)
#define	RANDL	(srandl)
#define	SIN	(sinf)
#define	SINH	(sinhf)
#define	SQRT	(sqrtf)
#define	STORE	(storef)
#define	TAN	(tanf)
#define	TANH	(tanhf)

#if STDC
#define acosf(x)	acosf_used_illegally_without_wrapper(x)
#define logf(x)		logf_used_illegally_without_wrapper(x)
#define log10f(x)	log10f_used_illegally_without_wrapper(x)
#define asinf(x)	asinf_used_illegally_without_wrapper(x)
#define atanf(x)	atanf_used_illegally_without_wrapper(x)
#define atan2f(x,y)	atan2f_used_illegally_without_wrapper(x,y)
#define cosf(x)		cosf_used_illegally_without_wrapper(x)
#define coshf(x)	coshf_used_illegally_without_wrapper(x)
#define expf(x)		expf_used_illegally_without_wrapper(x)
#define floorf(x)	floorf_used_illegally_without_wrapper(x)
#define ipowf(x,y)	ipowf_used_illegally_without_wrapper(x,y)
#define ldexpf(x,y)	ldexpf_used_illegally_without_wrapper(x,y)
#define logf(x)		logf_used_illegally_without_wrapper(x)
#define log10f(x)	log10f_used_illegally_without_wrapper(x)
#define modff(x,y)	modff_used_illegally_without_wrapper(x,y)
#define powf(x,y)	powf_used_illegally_without_wrapper(x,y)
#define sran()		sran_used_illegally_without_wrapper()
#define srandl()	srandl_used_illegally_without_wrapper()
#define sinf(x)		sinf_used_illegally_without_wrapper(x)
#define sinhf(x)	sinhf_used_illegally_without_wrapper(x)
#define sqrtf(x)	sqrtf_used_illegally_without_wrapper(x)
#define storef(x)	storef_used_illegally_without_wrapper(x)
#define tanf(x)		tanf_used_illegally_without_wrapper(x)
#define tanhf(x)	tanhf_used_illegally_without_wrapper(x)
#endif

#if !defined(HUGE_VALF)
#define HUGE_VALF (3.40282347e+38F)
#endif
#define TO_FP_T(x) ((sp_t)(x))

#endif /* PRECISION == 1 */

#if PRECISION == 2
extern void	machar ARGS((int* ibeta, int* it, int* irnd, int* ngrd,
			int* machep, int* negep, int* iexp, int* minexp,
			int* maxexp, dp_t* eps, dp_t* epsneg,
			dp_t* xmin, dp_t* xmax));

#define	ACOS	(acos)
#define ALOG	(log)
#define ALOG10	(log10)
#define	ASIN	(asin)
#define	ATAN	(atan)
#define	ATAN2	(atan2)
#define	COS	(cos)
#define	COSH	(cosh)
#define	COTAN	cotan			/* special case */
#define	EXP	(exp)
#define	FLOOR	(floor)
#define	IPOW	(ipow)
#define	LDEXP	(ldexp)
#define	LOG	(log)
#define	LOG10	(log10)
#define	MODF	(modf)
#define	POW	(pow)
#define	RAN	(ran)
#define	RANDL	(randl)
#define	SIN	(sin)
#define	SINH	(sinh)
#define	SQRT	(sqrt)
#define	STORE	(store)
#define	TAN	(tan)
#define	TANH	(tanh)

#if STDC
#define	acos(x)		acos_used_illegally_without_wrapper(x)
#define log(x)		log_used_illegally_without_wrapper(x)
#define log10(x)	log10_used_illegally_without_wrapper(x)
#define	asin(x)		asin_used_illegally_without_wrapper(x)
#define	atan(x)		atan_used_illegally_without_wrapper(x)
#define	atan2(x,y)	atan2_used_illegally_without_wrapper(x,y)
#define	cos(x)		cos_used_illegally_without_wrapper(x)
#define	cosh(x)		cosh_used_illegally_without_wrapper(x)
#define	exp(x)		exp_used_illegally_without_wrapper(x)
#define	floor(x)	floor_used_illegally_without_wrapper(x)
#define	ipow(x,y)	ipow_used_illegally_without_wrapper(x,y)
#define	ldexp(x,y)	ldexp_used_illegally_without_wrapper(x,y)
#define	log(x)		log_used_illegally_without_wrapper(x)
#define	log10(x)	log10_used_illegally_without_wrapper(x)
#define	modf(x,y)	modf_used_illegally_without_wrapper(x,y)
#define	pow(x,y)	pow_used_illegally_without_wrapper(x,y)
#define	ran()		ran_used_illegally_without_wrapper()
#define	randl()		randl_used_illegally_without_wrapper()
#define	sin(x)		sin_used_illegally_without_wrapper(x)
#define	sinh(x)		sinh_used_illegally_without_wrapper(x)
#define	sqrt(x)		sqrt_used_illegally_without_wrapper(x)
#define	store(x)	store_used_illegally_without_wrapper(x)
#define	tan(x)		tan_used_illegally_without_wrapper(x)
#define	tanh(x)		tanh_used_illegally_without_wrapper(x)
#endif

#define TO_FP_T(x) ((dp_t)(x))

#endif /* PRECISION == 2 */

#if PRECISION == 4
#ifdef __sun
#include <sunmath.h>	/* need for long double function prototypes on Sun Solaris 2.x */
#endif

extern void	macharl ARGS((int* ibeta, int* it, int* irnd, int* ngrd,
			int* machep, int* negep, int* iexp, int* minexp,
			int* maxexp, qp_t* eps, qp_t* epsneg,
			qp_t* xmin, qp_t* xmax));

#define ALOG	(logl)
#define ALOG10	(log10l)
#define	ACOS	(acosl)
#define	ASIN	(asinl)
#define	ATAN	(atanl)
#define	ATAN2	(atan2l)
#define	COS	(cosl)
#define	COSH	(coshl)
#define	COTAN	cotanl			/* special case */
#define	EXP	(expl)
#define	FLOOR	(floorl)
#define	IPOW	(ipowl)
#define	LDEXP	(ldexpl)
#define	LOG	(logl)
#define	LOG10	(log10l)
#define	MODF	(modfl)
#define	POW	(powl)
#define	RAN	(qran)
#define	RANDL	(qrandl)
#define	SIN	(sinl)
#define	SINH	(sinhl)
#define	SQRT	(sqrtl)
#define	STORE	(storel)
#define	TAN	(tanl)
#define	TANH	(tanhl)

#if STDC
#define logl(x)		logl_used_illegally_without_wrapper(x)
#define log10l(x)	log10l_used_illegally_without_wrapper(x)
#define	acosl(x)	acosl_used_illegally_without_wrapper(x)
#define	asinl(x)	asinl_used_illegally_without_wrapper(x)
#define	atanl(x)	atanl_used_illegally_without_wrapper(x)
#define	atan2l(x,y)	atan2l_used_illegally_without_wrapper(x,y)
#define	cosl(x)		cosl_used_illegally_without_wrapper(x)
#define	coshl(x)	coshl_used_illegally_without_wrapper(x)
#define	expl(x)		expl_used_illegally_without_wrapper(x)
#define	floorl(x)	floorl_used_illegally_without_wrapper(x)
#define	ipowl(x,y)	ipowl_used_illegally_without_wrapper(x,y)
#define	ldexpl(x,y)	ldexpl_used_illegally_without_wrapper(x,y)
#define	logl(x)		logl_used_illegally_without_wrapper(x)
#define	log10l(x)	log10l_used_illegally_without_wrapper(x)
#define	modfl(x,y)	modfl_used_illegally_without_wrapper(x,y)
#define	powl(x,y)	powl_used_illegally_without_wrapper(x,y)
#define	qran()		qran_used_illegally_without_wrapper()
#define	qrandl()	qrandl_used_illegally_without_wrapper()
#define	sinl(x)		sinl_used_illegally_without_wrapper(x)
#define	sinhl(x)	sinhl_used_illegally_without_wrapper(x)
#define	sqrtl(x)	sqrtl_used_illegally_without_wrapper(x)
#define	storel(x)	storel_used_illegally_without_wrapper(x)
#define	tanl(x)		tanl_used_illegally_without_wrapper(x)
#define	tanhl(x)	tanhl_used_illegally_without_wrapper(x)
#endif

#define TO_FP_T(x) ((qp_t)(x))
#if !defined(HUGE_VALL)
#define HUGE_VALL (1.18973149535723176508575932662800702e+4932L)
#endif

#endif /* PRECISION == 4 */
