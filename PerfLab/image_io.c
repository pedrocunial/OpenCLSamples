#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "image_io.h"

#define LINE_SIZE 256

static void save_image_header_to_file(image_t *image, FILE *file) {
  fprintf(file, "P3\n");
  fprintf(file, "# Created by: perflab image library.\n");
  fprintf(file, "%d %d\n", image->nrows, image->ncols);
  fprintf(file, "%d\n", max_pix_value());  
}

static void save_image_pixels_to_file(image_t *image, FILE *file) {
  pixel_t *pix = image->data;
  pixel_t *end = image->data + image->nrows * image->ncols;

  while (pix != end) {
    fprintf(file, "%d %d %d\n", pix->c[0], pix->c[1], pix->c[2]);
    pix++;
  }
}

static void save_image_to_file(image_t *image, FILE *file) {
  save_image_header_to_file(image, file);
  save_image_pixels_to_file(image, file);
} 

int save_image(image_t *image, char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Cannot open file %s for writing.\n", filename);
    return -1;
  }
  save_image_to_file(image, file);
  fclose(file);
  return 0;
}

static int read_line(char buf[], size_t size_buf, FILE *file) {
  if (fgets(buf, LINE_SIZE, file) == NULL) {
    return -1;
  }

  if (buf[0] == '#') {
    return 0;
  }

  return 1;
}

static image_t *read_image_from_file(FILE *file) {
  char buf[LINE_SIZE];
  int width, height, max_pixel;

  int state = 0;
  while (state < 3) {
    /* Skip comment lines. */
    int res = 0;
    while (res == 0) {
      res = read_line(buf, LINE_SIZE, file);
    }

    if (res == -1) {
      printf("Error reading line.\n");
      return NULL;
    }

    switch(state) {
      case 0:
        buf[2] = '\0';
        if (strcmp(buf, "P3") != 0) {
          printf("Error reading P3 tag.\n");
          return NULL;
        }
        state = 1;
        break;
      case 1:
        if (sscanf(buf, "%d %d", &width, &height) != 2) {
          printf("Error readind width and height.\n");
          return NULL;
        }
        state = 2;
        break;
      case 2:
        if (sscanf(buf, "%d", &max_pixel) != 1) {
          printf("Error reading max pixel value.\n");
          return NULL;
        }
        state = 3;
        break;
    }
  }

  image_t *img = make_image(height, width);
  int num_pix = width * height;

  int i;
  pixel_t pix;
  for (i = 0; i < num_pix; i++) {
    if (fscanf(file, "%hhu %hhu %hhu", &(pix.c[0]), &(pix.c[1]), &(pix.c[2])) != 3) {
      printf("Error reading pixel value.\n");
      delete_image(img);
      return NULL;
    }
    img->data[i] = pix;
  }
  return img;
}

image_t *read_image(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Cannot open %s for reading.\n", filename);
    return NULL;
  }
  image_t *img = read_image_from_file(file);
  fclose(file);

  if (img == NULL) {
    printf("Error while reading %s.\n", filename);
    return NULL;
  }
  return img;
}