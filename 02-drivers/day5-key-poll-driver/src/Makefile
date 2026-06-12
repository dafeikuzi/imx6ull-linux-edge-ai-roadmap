obj-m += imx6ull_key_poll.o

KDIR ?= /path/to/linux-4.9.88
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
APP := key_poll_test
APP_CFLAGS ?= -Wall -Wextra -O2

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
	$(CROSS_COMPILE)gcc $(APP_CFLAGS) key_poll_test.c -o $(APP)

app:
	$(CROSS_COMPILE)gcc $(APP_CFLAGS) key_poll_test.c -o $(APP)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(APP)
