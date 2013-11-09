CC=gcc
LIBS=-lncurses -lcurl
CFLAGS=-std=c99
C_FILES=$(wildcard *.c)

main: $(C_FILES)
	$(CC) $(CFLAGS) $< $(LIBS)