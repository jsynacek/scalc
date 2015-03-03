CC=gcc
CFLAGS=-Wall
LDFLAGS=-lm

all:
	$(CC) $(CFLAGS) $(LDFLAGS) scalc.c -o scalc

clean:
	rm -f scalc
