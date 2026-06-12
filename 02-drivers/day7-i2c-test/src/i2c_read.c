#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int parse_num(const char *s)
{
	return (int)strtol(s, NULL, 0);
}

int main(int argc, char **argv)
{
	unsigned char reg;
	unsigned char value;
	int fd;
	int addr;
	int ret;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s /dev/i2c-X addr reg\n", argv[0]);
		return 1;
	}

	addr = parse_num(argv[2]);
	reg = (unsigned char)parse_num(argv[3]);

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	if (ioctl(fd, I2C_SLAVE, addr) < 0) {
		perror("ioctl I2C_SLAVE");
		close(fd);
		return 1;
	}

	ret = write(fd, &reg, 1);
	if (ret != 1) {
		perror("write register");
		close(fd);
		return 1;
	}

	ret = read(fd, &value, 1);
	if (ret != 1) {
		perror("read value");
		close(fd);
		return 1;
	}

	printf("i2c %s addr 0x%02x reg 0x%02x = 0x%02x\n",
	       argv[1], addr, reg, value);

	close(fd);
	return 0;
}

