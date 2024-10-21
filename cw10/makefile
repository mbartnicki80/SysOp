CC=gcc
CFLAGS=-g -Wall -std=c17  
TARGET = santa

.PHONY: all clean

all: clean $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -lm -o $(TARGET)

clean:
	rm -f $(TARGET) user