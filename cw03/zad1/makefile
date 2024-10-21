CC=gcc
CFLAGS=-Wall -std=c17
TARGET = reverse

.PHONY: all clean

all: clean reverse

reverse: reverse.c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm -f $(TARGET) output.txt
