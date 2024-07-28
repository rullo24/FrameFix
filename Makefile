# Makefile for X11 Application with pkg-config

# Compiler
CC = gcc

# Executable Name
EXEC = FrameFix

# Compiler Flags
CFLAGS = -Wall -g $(shell pkg-config --cflags x11)

# Libraries
LIBS = $(shell pkg-config --libs x11)

# Source Files
SRCS = main.c

# Object Files
OBJS = $(SRCS:.c=.o)

# Targets
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

