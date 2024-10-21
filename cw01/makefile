CC=gcc
CFLAGS=-Wall -g -std=c17
TARGET = countdown

.PHONY: all clean

all: clean countdown

countdown: countdown.c
	$(CC) $(CFLAGS) countdown.c -o $(TARGET)

clean:
	rm -f $(TARGET)

