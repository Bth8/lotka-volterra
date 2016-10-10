#!/usr/bin/make -f

# name of the binary
TARGET      = predprey

CC          = gcc

CPPFLAGS    = -MD
CFLAGS      = -std=c11
LDFLAGS     =
LDLIBS      =

SRCS = lattice.c predprey.c output.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

.PHONY: all clean

-include $(DEPS)