#include <stdio.h>
#include <stdlib.h>

#include "image.h"

image_t * make_image(int nrows, int ncols) {
  image_t *image = (image_t *) malloc(sizeof(image_t));
  if (image == NULL) {
    printf("Error: cannot allocate image_t\n");
    return NULL;
  }

  pixel_t *data = (pixel_t *) calloc(nrows * ncols, sizeof(pixel_t));
  if (data == NULL) {
    free(image);
    printf("Error: cannot allocate pixels\n");
    return NULL;
  }

  image->data = data;
  image->nrows = nrows;
  image->ncols = ncols;

  return image;
}

void delete_image(image_t * image) {
  free(image->data);
  free(image);
}

int compare_image(image_t *img1, image_t *img2) {
  if (img1->nrows != img2->nrows || img1->ncols != img2->ncols) {
    return 0;
  }

  size_t i, n;
  n = img1->nrows * img1->ncols;
  for (i = 0; i < n; i++) {
    if (!compare_pixel(img1->data[i], img2->data[i])) {
      return 0;
    }
  }

  return 1;
}

pixel_t get_pix(image_t *image, int i, int j) {
  return image->data[i * image->ncols + j];
}

void set_pix(image_t *image, int i, int j, pixel_t pix) {
  image->data[i * image->ncols + j] = pix;
}
