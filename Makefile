CC=gcc
LIBS=-lncurses -lcurl -lm -ljansson
LIB_DIRS:=-L$(CURDIR)/jansson/src/.libs/
CFLAGS=-std=c99 -Wall -pedantic -g
C_HEADERS:=$(wildcard *.h)
C_FILES:=$(wildcard *.c)
C_ALL:=$(C_HEADERS) $(C_FILES)

MKDIR=mkdir -p

BIN_DIR=bin

main: prepareDir
	$(CC) $(CFLAGS) $(C_FILES) $(LIBS) $(LIB_DIRS) -o $(BIN_DIR)/$@

test:
	$(CC) $(CFLAGS) List.c StringReplace.c tests/StringReplaceTest.c $(LIBS) -o $(BIN_DIR)/$@

prepareDir:
	$(MKDIR) $(BIN_DIR)
