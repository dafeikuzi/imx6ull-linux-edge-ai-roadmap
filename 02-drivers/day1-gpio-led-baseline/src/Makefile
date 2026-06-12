CC ?= arm-linux-gnueabihf-gcc
CFLAGS ?= -Wall -Wextra -O2

TARGET := gpio_sysfs_toggle
SRCS := gpio_sysfs_toggle.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
