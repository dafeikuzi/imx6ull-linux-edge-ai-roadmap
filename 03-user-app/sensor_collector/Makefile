CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2

TARGET := sensor_collector
SRCS := sensor_collector.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

