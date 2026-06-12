#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(const char *prog)
{
	fprintf(stderr,
		"Usage:\n"
		"  %s /dev/imx6ull_key [count] [timeout_ms]\n\n"
		"Example:\n"
		"  %s /dev/imx6ull_key 2 10000\n",
		prog, prog);
}

static int parse_positive(const char *text, int fallback)
{
	int value;

	if (!text)
		return fallback;

	value = atoi(text);
	return value > 0 ? value : fallback;
}

int main(int argc, char **argv)
{
	struct pollfd pfd;
	char buf[32];
	int fd;
	int count = 0;
	int timeout_ms = -1;
	int seen = 0;
	ssize_t n;

	if (argc < 2 || argc > 4) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if (argc >= 3)
		count = parse_positive(argv[2], 0);
	if (argc >= 4)
		timeout_ms = parse_positive(argv[3], -1);

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	pfd.fd = fd;
	pfd.events = POLLIN;

	printf("poll waiting on %s, count=%s, timeout_ms=%d\n",
	       argv[1], count ? argv[2] : "forever", timeout_ms);
	fflush(stdout);

	while (count == 0 || seen < count) {
		int ret;

		pfd.revents = 0;
		ret = poll(&pfd, 1, timeout_ms);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			perror("poll");
			close(fd);
			return EXIT_FAILURE;
		}
		if (ret == 0) {
			printf("poll timeout\n");
			close(fd);
			return EXIT_FAILURE;
		}

		if (pfd.revents & (POLLIN | POLLRDNORM)) {
			n = read(fd, buf, sizeof(buf) - 1);
			if (n < 0) {
				perror("read");
				close(fd);
				return EXIT_FAILURE;
			}
			buf[n] = '\0';
			printf("poll event[%d]: %s", seen + 1, buf);
			fflush(stdout);
			seen++;
		} else {
			printf("unexpected revents=0x%x\n", pfd.revents);
		}
	}

	close(fd);
	return EXIT_SUCCESS;
}
