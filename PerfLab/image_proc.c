#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include "CL/cl.h"
#endif

#include "image.h"
#include "image_proc.h"

#define LUM0 0.299
#define LUM1 0.587
#define LUM2 0.114

char * get_error_string(cl_int error)
{
  char str[8192];
  switch (error) {
        case CL_SUCCESS:
          return "Success!";
        case CL_DEVICE_NOT_FOUND:
          return "Device not found.";
        case CL_DEVICE_NOT_AVAILABLE:
          return "Device not available";
        case CL_COMPILER_NOT_AVAILABLE:
          return "Compiler not available";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
          return "Memory object allocation failure";
        case CL_OUT_OF_RESOURCES:
          return "Out of resources";
        case CL_OUT_OF_HOST_MEMORY:
          return "Out of host memory";
        case CL_PROFILING_INFO_NOT_AVAILABLE:
          return "Profiling information not available";
        case CL_MEM_COPY_OVERLAP:
          return "Memory copy overlap";
        case CL_IMAGE_FORMAT_MISMATCH:
          return "Image format mismatch";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
          return "Image format not supported";
        case CL_BUILD_PROGRAM_FAILURE:
          return "Program build failure";
        case CL_MAP_FAILURE:
          return "Map failure";
        case CL_INVALID_VALUE:
          return "Invalid value";
        case CL_INVALID_DEVICE_TYPE:
          return "Invalid device type";
        case CL_INVALID_PLATFORM:
          return "Invalid platform";
        case CL_INVALID_DEVICE:
          return "Invalid device";
        case CL_INVALID_CONTEXT:
          return "Invalid context";
        case CL_INVALID_QUEUE_PROPERTIES:
          return "Invalid queue properties";
        case CL_INVALID_COMMAND_QUEUE:
          return "Invalid command queue";
        case CL_INVALID_HOST_PTR:
          return "Invalid host pointer";
        case CL_INVALID_MEM_OBJECT:
          return "Invalid memory object";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
          return "Invalid image format descriptor";
        case CL_INVALID_IMAGE_SIZE:
          return "Invalid image size";
        case CL_INVALID_SAMPLER:
          return "Invalid sampler";
        case CL_INVALID_BINARY:
          return "Invalid binary";
        case CL_INVALID_BUILD_OPTIONS:
          return "Invalid build options";
        case CL_INVALID_PROGRAM:
          return "Invalid program";
        case CL_INVALID_PROGRAM_EXECUTABLE:
          return "Invalid program executable";
        case CL_INVALID_KERNEL_NAME:
          return "Invalid kernel name";
        case CL_INVALID_KERNEL_DEFINITION:
          return "Invalid kernel definition";
        case CL_INVALID_KERNEL:
          return "Invalid kernel";
        case CL_INVALID_ARG_INDEX:
          return "Invalid argument index";
        case CL_INVALID_ARG_VALUE:
          return "Invalid argument value";
        case CL_INVALID_ARG_SIZE:
          return "Invalid argument size";
        case CL_INVALID_KERNEL_ARGS:
          return "Invalid kernel arguments";
        case CL_INVALID_WORK_DIMENSION:
          return "Invalid work dimension";
        case CL_INVALID_WORK_GROUP_SIZE:
          return "Invalid work group size";
        case CL_INVALID_WORK_ITEM_SIZE:
          return "Invalid work item size";
        case CL_INVALID_GLOBAL_OFFSET:
          return "Invalid global offset";
        case CL_INVALID_EVENT_WAIT_LIST:
          return "Invalid event wait list";
        case CL_INVALID_EVENT:
          return "Invalid event";
        case CL_INVALID_OPERATION:
          return "Invalid operation";
        case CL_INVALID_GL_OBJECT:
          return "Invalid OpenGL object";
        case CL_INVALID_BUFFER_SIZE:
          return "Invalid buffer size";
        case CL_INVALID_MIP_LEVEL:
          return "Invalid mip-map level";
        default:
          return "Unknown";
    }
}

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
  cl_platform_id platform = NULL;
  cl_device_id device = NULL;
  cl_uint platforms = 0, devices = 0;
  nrows = src->nrows;
  global[0] = nrows;
  global[1] = nrows;
  worksize = nrows * nrows * sizeof(pixel_t);

  err = clGetPlatformIDs(1, &platform, &platforms);
  if (err) {
    puts("ERROR: platform ids");
    exit(1);
  }
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &devices);
  if (err) {
    puts("ERROR: device ids");
    exit(1);
  }

  cl_context_properties properties[] = {
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0
  };
  // Note that nVidia's OpenCL requires the platform property
  cl_context context = clCreateContext(properties, 1, &device,
                                       NULL, NULL, &err);
  if (err) {
    puts("ERROR: context");
    puts(get_error_string(err));
    exit(1);
  }
  cl_command_queue cq = clCreateCommandQueue(context, device, 0, &err);
  if (err) {
    puts("ERROR: command queue");
    exit(1);
  }

  char srcfile[8192];
  FILE *fp = fopen("image_proc.cl", "r");
  srcsize = fread(srcfile, sizeof(srcfile), 1, fp);
  fclose(fp);

  const char *srcptr[] = { srcfile };
  // Submit the source code of the function kernel to OpenCL
  cl_program prog = clCreateProgramWithSource(context, 1, srcptr,
                                              &srcsize, &err);
  if (err) {
    puts("ERROR: create program");
    exit(1);
  }
  // and compile it (after this we could extract the compiled version)
  err = clBuildProgram(prog, 0, NULL, "-I ./", NULL, NULL);
  if (err) {
    /* cl_build_status bstatus; */
    char log[8192];
    /* size_t len; */
    puts("ERROR: build program");
    err = clGetProgramBuildInfo(prog, device,
                                CL_PROGRAM_BUILD_LOG,
                                8192,
                                log, NULL);
    puts(log);
    exit(1);
  }

  // Allocate memory for the kernel to work with
  cl_mem mem1, mem2;
  mem1 = clCreateBuffer(context, CL_MEM_READ_ONLY,
                        worksize,
                        NULL, &err);
  if (err) {
    puts("ERROR: mem1");
    exit(1);
  }
  mem2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                        worksize,
                        NULL, &err);
  if (err) {
    puts("ERROR: mem2");
    exit(1);
  }

  cl_kernel k_rot = clCreateKernel(prog, "rotate_image", &err);
  if (err) {
    puts("ERROR: create kernel");
    exit(1);
  }
  clSetKernelArg(k_rot, 0, sizeof(mem1), &mem1);
  clSetKernelArg(k_rot, 1, sizeof(mem2), &mem2);

  err = clEnqueueWriteBuffer(cq, mem1, CL_FALSE, 0, worksize,
                             srcdata, 0, NULL, NULL);
  if (err) {
    puts("ERROR: write buffer");
    exit(1);
  }

  // NOTE: Maybe I need to use &global
  err = clEnqueueNDRangeKernel(cq, k_rot, 2, NULL, global,
                               NULL, 0, NULL, NULL);
  if (err) {
    puts("ERROR: range kernel");
    exit(1);
  }
  err = clEnqueueReadBuffer(cq, mem2, CL_FALSE, 0, worksize,
                            destdata, 0, NULL, NULL);
  if (err) {
    puts("ERROR: read buffer");
    exit(1);
  }
  err = clFinish(cq);
  if (err) {
    puts("ERROR: finish");
    exit(1);
  }

  dest->data = destdata;

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
