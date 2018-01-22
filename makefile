CFLAGS = -Wall -pg -O2
OBJS = main.o filter.o
CC = gcc

all : denoise

denoise: $(OBJS)
	$(CC) -o $@ $?

clean:
	rm $(OBJS) denoise
