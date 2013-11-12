CC=gcc
LIBS=-lncurses -lcurl -lm
CFLAGS=-std=c99 -Wall -pedantic -g
C_HEADERS:=$(wildcard *.h)
C_FILES:=$(wildcard *.c)
C_ALL:=$(C_HEADERS) $(C_FILES)

main:
	$(CC) $(CFLAGS) $(C_FILES) $(LIBS)

test:
	$(CC) $(CFLAGS) List.c StringReplace.c tests/StringReplaceTest.c $(LIBS)