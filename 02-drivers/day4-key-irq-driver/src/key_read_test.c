#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(const char *prog)
{
	fprintf(stderr,
		"Usage:\n"
		"  %s /dev/imx6ull_key [count]\n\n"
		"Example:\n"
		"  %s /dev/imx6ull_key 4\n",
		prog, prog);
}

int main(int argc, char **argv)
{
	char buf[32];
	int fd;
	int count = 0;
	int seen = 0;
	ssize_t n;

	if (argc < 2 || argc > 3) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if (argc == 3) {
		count = atoi(argv[2]);
		if (count <= 0) {
			fprintf(stderr, "count must be positive\n");
			return EXIT_FAILURE;
		}
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	printf("waiting for key events on %s%s\n", argv[1],
	       count ? "..." : " forever...");
	fflush(stdout);

	while (count == 0 || seen < count) {
		n = read(fd, buf, sizeof(buf) - 1);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			perror("read");
			close(fd);
			return EXIT_FAILURE;
		}
		buf[n] = '\0';
		printf("event[%d]: %s", seen + 1, buf);
		fflush(stdout);
		seen++;
	}

	close(fd);
	return EXIT_SUCCESS;
}
