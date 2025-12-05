# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -O2

# Common source file
COMMON_SRCS = strfunc.c

# Target executable files
TARGETS = ../i2c_vread ../i2c_vwrite ../i2c_vread_d

# Default target
all: $(TARGETS)

# Build rule for i2c_vread
../i2c_vread: i2c_vread.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Build rule for i2c_vwrite
../i2c_vwrite: i2c_vwrite.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Build rule for i2c_vread_d
../i2c_vread_d: i2c_vread_d.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) -o $@ $^
# Clean target
clean:
	rm -f ../i2c_vread ../i2c_vwrite ../i2c_vread_d 
