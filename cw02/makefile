CC = gcc
CFLAGS = -Wall -g -std=c17
TARGET = collatz_conjecture
CURRENT_DIR := $(shell pwd)

.PHONY: all clean

all: clean static shared dynamic

static: $(TARGET).o client.c
	$(CC) -c collatz_conjecture.c $(CFLAGS)
	ar rcs lib$(TARGET).a $(TARGET).o
	$(CC) -c client.c $(CFLAGS)
	$(CC) client.o lib$(TARGET).a -o $(TARGET)_static

shared: $(TARGET).o client.c
	$(CC) $(CFLAGS) -fPIC -shared -o lib$(TARGET).so $^
	$(CC) -o $(TARGET)_shared $(TARGET).o -L$(CURRENT_DIR) -l$(TARGET)

dynamic: $(TARGET).o client.c
	$(CC) $(CFLAGS) -fPIC -shared -o lib$(TARGET).so $^
	$(CC) -D_DL_ $(CFLAGS) -o $(TARGET)_dynamic client.c -L$(CURRENT_DIR) -l$(TARGET)

clean:
	rm -f $(TARGET)_static $(TARGET)_dynamic $(TARGET)_shared *.o *.so *.a