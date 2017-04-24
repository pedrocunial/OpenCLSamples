#include <stdio.h>
#include "image.h"


__kernel void rotate_image(__global pixel_t *srcdata,
                           __global pixel_t *destdata)
{
  const uint i = get_global_id(0);
  const uint j = get_global_id(1);
  const uint ncols = get_global_size(0);
  uint ii = ncols - j - 1;
  destdata[ii * ncols + i] = srcdata[i * ncols + j];
}
