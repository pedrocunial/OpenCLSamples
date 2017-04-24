#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "tictoc.h"

void reset(tictoc_t *clock) {
  (clock->total).tv_sec = 0;
  (clock->total).tv_nsec = 0;
}

uint64_t get_time_nsec(tictoc_t *clock) {
  return 1000000000 * (clock->total).tv_sec + (clock->total).tv_nsec;
}

double get_time_usec(tictoc_t *clock) {
  return 1e-3 * get_time_nsec(clock);
}

double get_time_msec(tictoc_t *clock) {
  return 1e-6 * get_time_nsec(clock);
}

double get_time_sec(tictoc_t *clock) {
  return 1e-9 * get_time_nsec(clock);
}

double get_res(void) {
  struct timespec res;
  clock_getres(CLOCK_PROCESS_CPUTIME_ID, &res);
  return (double) (1000000000 * res.tv_sec + res.tv_nsec);
}
