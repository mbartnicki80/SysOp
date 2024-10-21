CC=gcc
CFLAGS=-g -Wall -std=c17  
TARGET = server

.PHONY: all clean

all: clean $(TARGET) client

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -lm -o $(TARGET)

client: client.c
	$(CC) $(CFLAGS) client.c -lm -o client

clean:
	rm -f $(TARGET) client