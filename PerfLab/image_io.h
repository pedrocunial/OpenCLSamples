#ifndef __IMAGE_IO_H__
#define __IMAGE_IO_H__

#include "image.h"

/* Return -1 if not successful, 0 if OK. */
int save_image(image_t *image, char *filename);

/* Return NULL if not successful, valid pointer if OK. */
image_t *read_image(char *filename);

#endif  /* __IMAGE_IO_H__ */
