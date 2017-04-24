#ifndef __PIXEL_H__
#define __PIXEL_H__

#define PIX_TYPE unsigned char

#define PIX_TYPE_FMT "%hhu"

typedef struct {
  PIX_TYPE c[3];
} pixel_t;

typedef struct {
  float c[3];
} float_pixel_t;

#define SET_PIX(pix, c0, c1, c2) { \
  (pix).c[0] = (c0); \
  (pix).c[1] = (c1); \
  (pix).c[2] = (c2); \
}

/* Return 1 if identical, 0 otherwise. */
static inline int compare_pixel(pixel_t p1, pixel_t p2) { \
  return p1.c[0] == p2.c[0] && p1.c[1] == p2.c[1] && p1.c[2] == p2.c[2]; \
}

static inline int compare_pixel_float(float_pixel_t p1, float_pixel_t p2) { \
  return p1.c[0] == p2.c[0] && p1.c[1] == p2.c[1] && p1.c[2] == p2.c[2]; \
}

static inline int max_pix_value(void) {
  return (1 << (sizeof(PIX_TYPE) << 3)) - 1;
}

void fill_random_pix(pixel_t *pix, int max_pix);

#endif  /* __PIXEL_H__ */
