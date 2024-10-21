CC=gcc
CFLAGS=-Wall -std=c17
TARGET = sizes

.PHONY: all clean

all: clean sizes

sizes: sizes.c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm -f $(TARGET)