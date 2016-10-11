#!/usr/bin/make -f

# name of the binary
TARGET      = predprey

CC          = gcc

CPPFLAGS    = -MD
CFLAGS      = -std=c11 -O3
LDFLAGS     =
LDLIBS      = -lpng

SRCS = lattice.c predprey.c output.c xorshift.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

-include $(DEPS)

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

.PHONY: all clean