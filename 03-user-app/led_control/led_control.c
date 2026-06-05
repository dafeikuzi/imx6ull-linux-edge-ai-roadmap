#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path;
    const char *state;
    FILE *fp;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <led_path> <0|1>\n", argv[0]);
        return 1;
    }

    path = argv[1];
    state = argv[2];
    if (strcmp(state, "0") != 0 && strcmp(state, "1") != 0) {
        fprintf(stderr, "state must be 0 or 1\n");
        return 1;
    }

    fp = fopen(path, "w");
    if (!fp) {
        fprintf(stderr, "open %s failed: %s\n", path, strerror(errno));
        return 1;
    }

    fprintf(fp, "%s\n", state);
    fclose(fp);
    return 0;
}

