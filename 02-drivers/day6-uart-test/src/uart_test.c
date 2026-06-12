#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static speed_t parse_baud(int baud)
{
	switch (baud) {
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	default:
		return 0;
	}
}

static int setup_uart(int fd, int baud)
{
	struct termios tio;
	speed_t speed = parse_baud(baud);

	if (!speed) {
		fprintf(stderr, "unsupported baud: %d\n", baud);
		return -1;
	}

	if (tcgetattr(fd, &tio) < 0) {
		perror("tcgetattr");
		return -1;
	}

	cfmakeraw(&tio);
	cfsetispeed(&tio, speed);
	cfsetospeed(&tio, speed);
	tio.c_cflag |= CLOCAL | CREAD;
	tio.c_cflag &= ~CSTOPB;
	tio.c_cflag &= ~PARENB;
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |= CS8;
	tio.c_cc[VMIN] = 0;
	tio.c_cc[VTIME] = 10;

	if (tcsetattr(fd, TCSANOW, &tio) < 0) {
		perror("tcsetattr");
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	char tx[128];
	char rx[128];
	int fd;
	int baud;
	ssize_t n;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s /dev/ttymxcX 115200\n", argv[0]);
		return 1;
	}

	baud = atoi(argv[2]);
	fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	if (setup_uart(fd, baud) < 0) {
		close(fd);
		return 1;
	}

	printf("input text: ");
	fflush(stdout);
	if (!fgets(tx, sizeof(tx), stdin)) {
		close(fd);
		return 1;
	}

	if (write(fd, tx, strlen(tx)) < 0) {
		perror("write");
		close(fd);
		return 1;
	}

	n = read(fd, rx, sizeof(rx) - 1);
	if (n < 0) {
		perror("read");
		close(fd);
		return 1;
	}

	rx[n] = '\0';
	printf("rx: %s\n", rx);
	close(fd);
	return 0;
}

