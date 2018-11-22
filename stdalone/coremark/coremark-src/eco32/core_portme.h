/*
 * core_portme.h
 */


#ifndef CORE_PORTME_H
#define CORE_PORTME_H


#define COMPILER_VERSION	"lcc 4.2"
#define COMPILER_FLAGS		"-Ieco32 -I. -Wo-nostdinc -Wo-nostdlib"

#define NULL				((void *) 0)
#define COMPILER_REQUIRES_SORT_RETURN	1
#define MULTITHREAD			1
#define MEM_LOCATION			"STATIC"
#define MAIN_HAS_NOARGC			1

#define EE_TICKS_PER_SEC		50000000


#define align_mem(x)	((void *)(4U + (((ee_ptr_int)(x) - 1U) & ~3U)))


typedef unsigned int ee_u32;
typedef unsigned short ee_u16;
typedef unsigned char ee_u8;
typedef signed int ee_s32;
typedef signed short ee_s16;
typedef signed char ee_s8;

typedef ee_u32 CORE_TICKS;
typedef ee_u32 ee_size_t;
typedef ee_u32 ee_ptr_int;

typedef struct CORE_PORTABLE_S {
  ee_u8 portable_id;
} core_portable;


extern ee_u32 default_num_contexts;


void portable_init(core_portable *p, int *argc, char *argv[]);
void portable_fini(core_portable *p);
void start_time(void);
void stop_time(void);
CORE_TICKS get_time(void);

int ee_printf(const char *fmt, ...);


#endif /* CORE_PORTME_H */
