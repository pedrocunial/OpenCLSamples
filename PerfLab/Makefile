CC = gcc
CFLAGS = -Wall -O3 -funroll-loops
LIBS =  -L. -lperflab -lm -lrt -lOpenCL
TARGETS = libperflab.a driver

LIBPERFLAB_OBJS = pix.o image.o image_io.o tictoc.o
LIBPERFLAB_HDRS = pix.h image.h image_io.h tictoc.h

all: $(TARGETS)


pix.o: pix.c pix.h
	$(CC) $(CFLAGS) -c pix.c

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c

image_io.o: image_io.c image_io.h image.h
	$(CC) $(CFLAGS) -c image_io.c

tictoc.o: tictoc.c tictoc.h
	$(CC) $(CFLAGS) -c tictoc.c

libperflab.a: $(LIBPERFLAB_OBJS) $(LIBPERFLAB_HDRS)
	ar rcs libperflab.a $(LIBPERFLAB_OBJS)


image_proc.o: image_proc.c $(LIBPERFLAB_HDRS) image_proc.cl
	$(CC) $(CFLAGS) -c image_proc.c


driver.o: driver.c image_proc.h $(LIBPERFLAB_HDRS)
	$(CC) $(CFLAGS) -c driver.c

driver: driver.o image_proc.o libperflab.a
	$(CC) $(CFLAGS) -o driver driver.o image_proc.o $(LIBS)


clean:
	rm *.o $(TARGETS) house_rotated.ppm
