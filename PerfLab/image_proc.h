#ifndef __IMAGE_PROC_H__
#define __IMAGE_PROC_H__

#include "image.h"

void rotate_image(image_t *src, image_t *dest);
void better_rotate_image(image_t *src, image_t *dest);

float avg_luma(image_t *img);
float better_avg_luma(image_t *img);

#endif  /* __IMAGE_PROC_H__ */
