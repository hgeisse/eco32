/*
 * core_portme.c
 */


#include "coremark.h"
#include "core_portme.h"


#if VALIDATION_RUN
  volatile ee_s32 seed1_volatile = 0x3415;
  volatile ee_s32 seed2_volatile = 0x3415;
  volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
  volatile ee_s32 seed1_volatile = 0x0;
  volatile ee_s32 seed2_volatile = 0x0;
  volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
  volatile ee_s32 seed1_volatile = 0x8;
  volatile ee_s32 seed2_volatile = 0x8;
  volatile ee_s32 seed3_volatile = 0x8;
#endif
  volatile ee_s32 seed4_volatile = ITERATIONS;
  volatile ee_s32 seed5_volatile = 0;


ee_u32 default_num_contexts = 1;


void portable_init(core_portable *p, int *argc, char *argv[]) {
  ee_printf("CoreMark started...\n\n");
  if (sizeof(ee_ptr_int) != sizeof(ee_u8 *)) {
    ee_printf("ERROR! Please define ee_ptr_int "
              "to a type that holds a pointer!\n");
  }
  if (sizeof(ee_u32) != 4) {
    ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
  }
  p->portable_id = 1;
}


void portable_fini(core_portable *p) {
  p->portable_id = 0;
  ee_printf("\nCoreMark finished.\n");
}


static unsigned int start_ticks;
static unsigned int stop_ticks;


void start_time(void) {
  volatile unsigned int *base;
  unsigned int val;

  base = (unsigned int *) 0xF0000000;
  val = *(base + 0);
  *(base + 0) = 0x00000000;
  *(base + 1) = 0xFFFFFFFF;
  start_ticks = *(base + 2);
}


void stop_time(void) {
  volatile unsigned int *base;

  base = (unsigned int *) 0xF0000000;
  stop_ticks = *(base + 2);
}


CORE_TICKS get_time(void) {
  return start_ticks - stop_ticks;
}


secs_ret time_in_secs(CORE_TICKS ticks) {
  return (secs_ret) ticks / (secs_ret) EE_TICKS_PER_SEC;
}
