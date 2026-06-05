CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2

all: led_control

led_control: led_control.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f led_control

