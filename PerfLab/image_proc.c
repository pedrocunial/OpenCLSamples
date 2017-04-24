#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "image.h"
#include "image_proc.h"

#define LUM0 0.299
#define LUM1 0.587
#define LUM2 0.114

void rotate_image(image_t *src, image_t *dest) {
  int i, j;
  pixel_t pix;
  for (i = 0; i < src->nrows; i++) {
    for (j = 0; j < src->ncols; j++) {
      pix = get_pix(src, i, j);
      set_pix(dest, dest->nrows - j - 1, i, pix);
    }
  }
}

void better_rotate_image(image_t *src, image_t *dest) {
  // init opencl
  size_t srcsize, nrows, worksize, global[2];
  pixel_t *srcdata = src->data;
  pixel_t *destdata = dest->data;
  cl_int err;
  cl_platform_id platform;
  cl_device_id device;
  cl_uint platforms, devices;
  nrows = src->nrows;
  global[0] = nrows;
  global[1] = nrows;
  worksize = nrows * nrows * sizeof(pixel_t);

  err = clGetPlatformIDs(1, &platform, &platforms);
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &devices);

  cl_context_properties properties[] = {
    CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
    0};
  // Note that nVidia's OpenCL requires the platform property
  // NOTE: It just shows a warning, soo it's fine :^)
  cl_context context = clCreateContext(properties, 1, &device, NULL, NULL,
                                       &err);
  cl_command_queue cq = clCreateCommandQueue(context, device, 0, &err);

  char srcfile[8192];
  FILE *fp = fopen("image_proc.cl", "r");
  srcsize = fread(srcfile, sizeof(srcfile), 1, fp);
  fclose(fp);

  const char *srcptr[] = { srcfile };
  // Submit the source code of the function kernel to OpenCL
  cl_program prog = clCreateProgramWithSource(context, 1, srcptr,
                                              &srcsize, &err);
  // and compile it (after this we could extract the compiled version)
  err = clBuildProgram(prog, 0, NULL, "", NULL, NULL);

  // Allocate memory for the kernel to work with
  cl_mem mem1, mem2;
  mem1 = clCreateBuffer(context, CL_MEM_READ_ONLY,
                        worksize,
                        NULL, &err);
  mem2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                        worksize,
                        NULL, &err);

  cl_kernel k_rot = clCreateKernel(prog, "rotate_image", &err);
  clSetKernelArg(k_rot, 0, sizeof(mem1), &mem1);
  clSetKernelArg(k_rot, 1, sizeof(mem2), &mem2);

  err = clEnqueueWriteBuffer(cq, mem1, CL_FALSE, 0, worksize,
                             srcdata, 0, NULL, NULL);
  err = clEnqueueNDRangeKernel(cq, k_rot, 2, NULL, &global,
                                 NULL, 0, NULL, NULL);
  err = clEnqueueReadBuffer(cq, mem2, CL_FALSE, 0, worksize,
                              destdata, 0, NULL, NULL);
  err = clFinish(cq);

  /* rotate_image(src, dest); */
}

float avg_luma(image_t *img) {
  int i, j;
  double luma;
  pixel_t pix;

  luma = 0.0;
  for (i = 0; i < img->nrows; i++) {
    for (j = 0; j < img->ncols; j++) {
      pix = get_pix(img, i, j);
      luma += 0.299 * ((double) pix.c[0]) +
        0.587 * ((double) pix.c[1]) +
        0.114 * ((double) pix.c[2]);
    }
  }
  luma /= (double) (img->nrows * img->ncols);

  return (float) luma;
}

float better_avg_luma(image_t *img) {
  return avg_luma(img);
}
