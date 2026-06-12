CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

all: i2c_read

i2c_read: i2c_read.c
	$(CC) -Wall -O2 $< -o $@

clean:
	rm -f i2c_read

