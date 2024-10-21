CC=gcc
CFLAGS=-g -Wall -std=c17

.PHONY: all clean

all: clean catcher sender

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher

clean:
	rm -f sender catcher