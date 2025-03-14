#!/usr/bin/make -f

# name of the binary
TARGET      = predprey

CC          = gcc

MAGICK_CPPFLAGS	= -I/usr/include/ImageMagick-7 -fopenmp \
	-DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16 \
	-DMAGICKCORE_CHANNEL_MASK_DEPTH=32
MAGICK_CFLAGS	=
MAGICK_LDFLAGS	=
MAGICK_LDLIBS	= -lMagickCore-7.Q16


CPPFLAGS    = $(MAGICK_CPPFLAGS) -MD
CFLAGS      = $(MAGICK_CFLAGS) -std=c11 -O3
LDFLAGS     = $(MAGICK_LDFLAGS)
LDLIBS      = $(MAGICK_LDLIBS)

SRCS = lattice.c predprey.c output.c xorshift.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

.PHONY: all clean

-include $(DEPS)
