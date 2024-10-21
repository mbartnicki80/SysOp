CC=gcc
CFLAGS=-g -Wall -std=c17  
TARGET = printer

.PHONY: all clean

all: clean $(TARGET) user

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -lm -o $(TARGET)

user: user.c
	$(CC) $(CFLAGS) user.c -lm -o user

clean:
	rm -f $(TARGET) user