/* Simple Hello World for OpenCL, written in C.
 * For real code, check for errors. The error code is stored in all calls here,
 * but no checking is done, which is certainly bad. It'll likely simply crash
 * right after a failing call.
 *
 * On GNU/Linux with nVidia OpenCL, program builds with -lOpenCL.
 * Not sure about other platforms.
 */

#include <stdio.h>
#include <string.h>

#include <CL/cl.h>

const char rot13_cl[] = "				\
__kernel void rot13					\
    (   __global    const   char*    in			\
    ,   __global            char*    out		\
    )							\
{							\
    const uint index = get_global_id(0);		\
              \
    char c=in[index];					\
    if (c<'A' || c>'z' || (c>'Z' && c<'a')) {		\
        out[index] = in[index];				\
    } else {						\
        if (c>'m' || (c>'M' && c<'a')) {		\
      out[index] = in[index]-13;			\
  } else {					\
      out[index] = in[index]+13;			\
  }						\
    }							\
}							\
";

void rot13 (char *buf)
{
  int index=0;
  char c=buf[index];
  while (c!=0) {
    if (c<'A' || c>'z' || (c>'Z' && c<'a')) {
      buf[index] = buf[index];
    } else {
      if (c>'m' || (c>'M' && c<'a')) {
        buf[index] = buf[index]-13;
      } else {
        buf[index] = buf[index]+13;
      }
    }
    c=buf[++index];
  }
}

int main() {
  char buf[]="Hello, World!";
  size_t srcsize, worksize = strlen(buf);

  cl_int error;
  cl_platform_id platform;
  cl_device_id device;
  cl_uint platforms, devices;

  // Fetch the Platform and Device IDs; we only want one.
  error=clGetPlatformIDs(1, &platform, &platforms);
  error=clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &devices);
  cl_context_properties properties[] = {
    CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
    0};
  // Note that nVidia's OpenCL requires the platform property
  // NOTE: It just shows a warning, soo it's fine :^)
  cl_context context = clCreateContext(properties, 1, &device, NULL, NULL, &error)
    ;
  cl_command_queue cq = clCreateCommandQueue(context, device, 0, &error);

  rot13(buf);	// scramble using the CPU
  puts(buf);	// Just to demonstrate the plaintext is destroyed

  char src[8192];
  FILE *fil = fopen("rot13.cl","r");
  srcsize=fread(src, sizeof(src), 1, fil);
  fclose(fil);

  /* const char *src=rot13_cl; */
  /* srcsize=strlen(rot13_cl); */

  const char *srcptr[] = {src};
  // Submit the source code of the rot13 kernel to OpenCL
  cl_program prog = clCreateProgramWithSource(context,
                                            1, srcptr, &srcsize, &error);
  // and compile it (after this we could extract the compiled version)
  error = clBuildProgram(prog, 0, NULL, "", NULL, NULL);

  // Allocate memory for the kernel to work with
  cl_mem mem1, mem2;
  mem1 = clCreateBuffer(context, CL_MEM_READ_ONLY, worksize, NULL, &error);
  mem2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, worksize, NULL, &error);

  // get a handle and map parameters for the kernel
  cl_kernel k_rot13 = clCreateKernel(prog, "rot13", &error);
  // NOTE: mem1 and mem2 are both buffers for the string
  clSetKernelArg(k_rot13, 0, sizeof(mem1), &mem1);
  clSetKernelArg(k_rot13, 1, sizeof(mem2), &mem2);

  // Target buffer just so we show we got the data from OpenCL
  char buf2[sizeof buf];
  buf2[0] = '?';
  buf2[worksize] = 0;

  // Send input data to OpenCL (async, don't alter the buffer!)
  // https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueWriteBuffer.html
  /* cl_int clEnqueueWriteBuffer(
       cl_command_queue command_queue, => duh?
       cl_mem buffer,                  => where we're saving
       cl_bool blocking_write,         => magic
       size_t offset,                  => offset in bytes
       size_t cb,                      => size (bytes) of data being written
       const void *ptr,                => ptr to the original data buffer
       cl_uint num_events_in_wait_list,
       const cl_event *event_wait_list,
       cl_event *event
     ); */
  error = clEnqueueWriteBuffer(cq, mem1, CL_FALSE, 0, worksize,
                               buf, 0, NULL, NULL);
  // Perform the operation
  // https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueNDRangeKernel.html
  /* cl_int clEnqueueNDRangeKernel(
       cl_command_queue command_queue,  => duh?
       cl_kernel kernel,                => needs to have the same context of cq
       cl_uint work_dim,                => # of dims working
       const size_t *global_work_offset,
       const size_t *global_work_size,  => arr of work_dim u_val of # to work
       const size_t *local_work_size,   => same
       cl_uint num_events_in_wait_list,
       const cl_event *event_wait_list,
       cl_event *event
     );*/
  error = clEnqueueNDRangeKernel(cq, k_rot13, 1, NULL, &worksize,
                                 &worksize, 0, NULL, NULL);
  // Read the result back into buf2
  // Pretty much the same signature as the write method
  error = clEnqueueReadBuffer(cq, mem2, CL_FALSE, 0, worksize,
                              buf2, 0, NULL, NULL);
  // Await completion of all the above
  error = clFinish(cq);

  // Finally, output out happy message.
  puts(buf2);
}
