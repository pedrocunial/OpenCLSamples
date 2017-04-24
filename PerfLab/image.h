#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "pix.h"

typedef struct {
  pixel_t *data;
  int nrows;
  int ncols;
} image_t;

image_t * make_image(int nrows, int ncols);

void delete_image(image_t * image);

int compare_image(image_t *img1, image_t *img2);

pixel_t get_pix(image_t *image, int i, int j);

void set_pix(image_t *image, int i, int j, pixel_t pix);

#endif  /* __IMAGE_H__ */
