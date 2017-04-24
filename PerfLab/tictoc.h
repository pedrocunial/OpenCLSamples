#ifndef __TICTOC_H__
#define __TICTOC_H__

#include <stdint.h>
#include <time.h>

typedef struct {
  struct timespec start;
  struct timespec total;
} tictoc_t;

static inline void tic(tictoc_t *clock) {
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &(clock->start));
}

static inline void toc(tictoc_t *clock) {
  struct timespec end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
  (clock->total).tv_sec += end.tv_sec - (clock->start).tv_sec;
  (clock->total).tv_nsec += end.tv_nsec - (clock->start).tv_nsec;
}

void reset(tictoc_t *clock);

uint64_t get_time_nsec(tictoc_t *clock);

double get_time_usec(tictoc_t *clock);

double get_time_msec(tictoc_t *clock);

double get_time_sec(tictoc_t *clock);

double get_res(void);

#endif  /* __TICTOC_H__ */
