CC=gcc
CFLAGS=-g -Wall -std=c17  
TARGET = demonstrate

.PHONY: all clean

all: clean $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	rm -f $(TARGET)
