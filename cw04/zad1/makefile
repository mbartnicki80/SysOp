CC=gcc
CFLAGS=-Wall -std=c17
TARGET = process

.PHONY: all clean

all: clean $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm -f $(TARGET)
