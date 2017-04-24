#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "image_proc.h"
#include "perflab.h"

#define IMG_SIZE (1 << 9)
char filename_orig[] = "house.ppm";
char filename_dest[] = "house_rotated.ppm";

#define NRUNS_ROTATE 1000
#define NRUNS_AVG 1000

#define TEST_PRECISION 1e-1

void reset_image(image_t *img) {
  int i, npix;
  pixel_t pix;

  pix.c[0] = 0;
  pix.c[1] = 0;
  pix.c[2] = 0;

  npix = img->nrows * img->ncols;
  for (i = 0; i < npix; i++) {
    img->data[i] = pix;
  }
}

void test_rotate(void) {
  printf("\n\n");
  printf("=================\n");
  printf("Test rotate image\n");
  printf("=================\n\n");

  /* Make the original image. */
  /*image_t *orig_image = make_test_image(IMG_SIZE, IMG_SIZE);*/
  image_t *orig_image = read_image(filename_orig);

  /* Apply the rotate_image function to save the benchmark image. */
  image_t *correct_image = make_image(IMG_SIZE, IMG_SIZE);
  rotate_image(orig_image, correct_image);

  save_image(correct_image, filename_dest);

  /* Make the destination image */
  image_t *dest_image = make_image(IMG_SIZE, IMG_SIZE);

  int i;
  tictoc_t clock;

  printf("Init baseline timing.\n");

  /* Measure performance of rotate_image. */
  reset(&clock);
  tic(&clock);
  for (i = 0; i < NRUNS_ROTATE; i++) {
    rotate_image(orig_image, dest_image);
  }
  toc(&clock);
  printf("rotate_image: %lf usec.\n", get_time_usec(&clock) / NRUNS_ROTATE);

  /* Run better_rotate_image once to warm-up the cache */
  reset_image(dest_image);
  better_rotate_image(orig_image, dest_image);

  printf("Init improved rotate timing.\n");

  /* Measure performance of better_rotate_image. */
  reset(&clock);
  tic(&clock);
  for (i = 0; i < NRUNS_ROTATE; i++) {
    better_rotate_image(orig_image, dest_image);
  }
  toc(&clock);
  printf("better_rotate_image: %lf usec.\n", get_time_usec(&clock) / NRUNS_ROTATE);

  /* Check if better_rotate_image matches rotate_image. */
  if (compare_image(correct_image, dest_image) == 0) {
    printf("Erro na rotação de imagem\n");
  }

  /* Clean-up. */
  delete_image(dest_image);
  delete_image(correct_image);
  delete_image(orig_image);  
}

void test_color(void) {
  printf("\n\n");
  printf("==========\n");
  printf("Test color\n");
  printf("==========\n\n");

  /* Make the original image. */
  /*image_t *orig_image = make_test_image(IMG_SIZE, IMG_SIZE);*/
  image_t *orig_image = read_image("house.ppm");

  /* Apply the avg_color function to save the benchmark color. */
  float correct_avg_luma = avg_luma(orig_image);
  printf("Average luma: %f\n", correct_avg_luma);

  /* Measure performance of avg_color. */
  int i;
  tictoc_t clock;

  printf("Init baseline timing.\n");

  /* Run avg_color once to warm-up the cache */
  float dest_luma = avg_luma(orig_image);

  reset(&clock);
  tic(&clock);
  for (i = 0; i < NRUNS_AVG; i++) {
    dest_luma = avg_luma(orig_image);
  }
  toc(&clock);
  printf("avg_luma: %lf usec.\n", get_time_usec(&clock) / NRUNS_AVG);

  printf("Init better_avg_luma timing.\n");

  /* Run better_avg_color once to warm-up the cache */
  dest_luma = better_avg_luma(orig_image);
  printf("Average luma from better_avg_luma: %f\n", dest_luma);

  reset(&clock);
  tic(&clock);
  for (i = 0; i < NRUNS_AVG; i++) {
    dest_luma = better_avg_luma(orig_image);
  }
  toc(&clock);
  printf("better_avg_luma: %lf usec.\n", get_time_usec(&clock) / NRUNS_AVG);

  /* Check if better_rotate_image matches rotate_image. */
  if (fabsf(correct_avg_luma - dest_luma) > TEST_PRECISION) {
    printf("Erro na media de luma.\n");
  }

  /* Clean-up. */
  delete_image(orig_image);  
}


int main(int argc, char *argv[]) {
  printf("Clock resolution: %lf\n", get_res());
  test_rotate();
  test_color();

  return 0;
}