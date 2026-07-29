
TARGET = decode_metadata

CC = gcc

CFLAGS = -Wall -Wextra -O2 -std=c99

SRCS = decode_metadata.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
