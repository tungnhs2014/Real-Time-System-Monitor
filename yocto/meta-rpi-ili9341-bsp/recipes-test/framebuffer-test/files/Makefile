CC = gcc
CFLAGS = -Wall -O2
TARGET = fb_test

all: $(TARGET)

$(TARGET): fb_test.c
	$(CC) $(CFLAGS) -o $(TARGET) fb_test.c

clean:
	rm -f $(TARGET)

.PHONY: all clean
