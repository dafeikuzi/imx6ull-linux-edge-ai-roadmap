CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

all: uart_test

uart_test: uart_test.c
	$(CC) -Wall -O2 $< -o $@

clean:
	rm -f uart_test

