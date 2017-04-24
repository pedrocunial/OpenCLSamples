#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "pix.h"

static int randrange(int min, int max) {
  int range = max - min;
  return min + rand() % range;
}

void fill_random_pix(pixel_t *pix, int max_pix) {
  pix->c[0] = (PIX_TYPE) randrange(0, max_pix);
  pix->c[1] = (PIX_TYPE) randrange(0, max_pix);
  pix->c[2] = (PIX_TYPE) randrange(0, max_pix);
}
