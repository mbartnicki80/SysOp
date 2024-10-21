CC=gcc
CFLAGS=-g -Wall -std=c17  
TARGET = result

.PHONY: all clean

all: clean $(TARGET) calculate

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -lm -o $(TARGET)

calculate: calculate.c
	$(CC) $(CFLAGS) calculate.c -lm -o calculate

clean:
	rm -f $(TARGET) calculate