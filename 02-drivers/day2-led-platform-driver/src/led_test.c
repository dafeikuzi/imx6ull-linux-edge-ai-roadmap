#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(const char *prog)
{
	fprintf(stderr,
		"Usage:\n"
		"  %s /dev/imx6ull_led on|off|1|0\n"
		"  %s /dev/imx6ull_led read|status\n"
		"  %s /dev/imx6ull_led toggle [count] [delay_ms]\n\n"
		"Examples:\n"
		"  %s /dev/imx6ull_led on\n"
		"  %s /dev/imx6ull_led off\n"
		"  %s /dev/imx6ull_led read\n"
		"  %s /dev/imx6ull_led toggle 6 300\n",
		prog, prog, prog, prog, prog, prog, prog);
}

static int write_all(int fd, const char *buf, size_t len)
{
	size_t done = 0;

	while (done < len) {
		ssize_t ret = write(fd, buf + done, len - done);

		if (ret < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		if (ret == 0) {
			errno = EIO;
			return -1;
		}
		done += (size_t)ret;
	}

	return 0;
}

static int led_write_value(int fd, int on)
{
	const char *value = on ? "1" : "0";

	if (write_all(fd, value, 1) < 0) {
		perror("write");
		return -1;
	}

	printf("led %s\n", on ? "on" : "off");
	return 0;
}

static int led_read_value(int fd)
{
	char buf[16];
	ssize_t ret;

	ret = pread(fd, buf, sizeof(buf) - 1, 0);
	if (ret < 0) {
		perror("read");
		return -1;
	}

	buf[ret] = '\0';
	printf("led state: %s", buf);
	return (buf[0] == '0') ? 0 : 1;
}

static int parse_count(const char *text, int fallback)
{
	int value;

	if (!text)
		return fallback;

	value = atoi(text);
	return value > 0 ? value : fallback;
}

int main(int argc, char **argv)
{
	const char *dev;
	const char *cmd;
	int fd;
	int ret = 0;

	if (argc < 3 || argc > 5) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	dev = argv[1];
	cmd = argv[2];

	fd = open(dev, O_RDWR);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	if (strcmp(cmd, "1") == 0 || strcmp(cmd, "on") == 0) {
		ret = led_write_value(fd, 1);
	} else if (strcmp(cmd, "0") == 0 || strcmp(cmd, "off") == 0) {
		ret = led_write_value(fd, 0);
	} else if (strcmp(cmd, "read") == 0 || strcmp(cmd, "status") == 0) {
		ret = led_read_value(fd) < 0 ? -1 : 0;
	} else if (strcmp(cmd, "toggle") == 0) {
		int count = parse_count(argc >= 4 ? argv[3] : NULL, 4);
		int delay_ms = parse_count(argc >= 5 ? argv[4] : NULL, 300);
		int i;

		for (i = 0; i < count; i++) {
			ret = led_write_value(fd, i % 2 == 0);
			if (ret)
				break;
			usleep((useconds_t)delay_ms * 1000);
		}
	} else {
		usage(argv[0]);
		ret = -1;
	}

	close(fd);
	return ret ? EXIT_FAILURE : EXIT_SUCCESS;
}
