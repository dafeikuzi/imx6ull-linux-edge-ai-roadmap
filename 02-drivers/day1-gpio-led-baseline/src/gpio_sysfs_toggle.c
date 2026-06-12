#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio"

static int write_text(const char *path, const char *text)
{
    int fd;
    ssize_t len;
    ssize_t ret;

    fd = open(path, O_WRONLY);
    if (fd < 0)
        return -errno;

    len = (ssize_t)strlen(text);
    ret = write(fd, text, len);
    close(fd);

    if (ret != len)
        return -EIO;

    return 0;
}

static int read_text(const char *path, char *buf, size_t size)
{
    int fd;
    ssize_t ret;

    if (size == 0)
        return -EINVAL;

    fd = open(path, O_RDONLY);
    if (fd < 0)
        return -errno;

    ret = read(fd, buf, size - 1);
    close(fd);

    if (ret < 0)
        return -errno;

    buf[ret] = '\0';
    return 0;
}

static int gpio_export(int gpio)
{
    char path[128];
    char value[32];
    int ret;

    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d", gpio);
    if (access(path, F_OK) == 0)
        return 0;

    snprintf(value, sizeof(value), "%d", gpio);
    ret = write_text(SYSFS_GPIO_DIR "/export", value);
    if (ret == -EBUSY)
        return 0;

    return ret;
}

static int gpio_set_direction(int gpio, const char *direction)
{
    char path[128];

    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
    return write_text(path, direction);
}

static int gpio_set_value(int gpio, int value)
{
    char path[128];

    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    return write_text(path, value ? "1" : "0");
}

static int gpio_get_value(int gpio, char *buf, size_t size)
{
    char path[128];

    snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    return read_text(path, buf, size);
}

static void usage(const char *prog)
{
    fprintf(stderr,
            "Usage:\n"
            "  %s <gpio> <value0> [value1] [delay_ms]\n\n"
            "Example:\n"
            "  %s 131 0 1 500\n"
            "  %s 133 1 0 500\n",
            prog, prog, prog);
}

static void die_step(const char *step, int ret)
{
    fprintf(stderr, "%s failed: %s (%d)\n", step, strerror(-ret), ret);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int gpio;
    int value0;
    int value1;
    int delay_ms;
    int ret;
    char buf[32];

    if (argc < 3 || argc > 5) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    gpio = atoi(argv[1]);
    value0 = atoi(argv[2]) ? 1 : 0;
    value1 = (argc >= 4) ? (atoi(argv[3]) ? 1 : 0) : value0;
    delay_ms = (argc >= 5) ? atoi(argv[4]) : 0;

    printf("gpio_sysfs_toggle: gpio=%d value0=%d value1=%d delay_ms=%d\n",
           gpio, value0, value1, delay_ms);

    ret = gpio_export(gpio);
    if (ret)
        die_step("export", ret);

    ret = gpio_set_direction(gpio, "out");
    if (ret)
        die_step("set direction", ret);

    ret = gpio_set_value(gpio, value0);
    if (ret)
        die_step("write value0", ret);

    ret = gpio_get_value(gpio, buf, sizeof(buf));
    if (ret)
        die_step("read value0", ret);
    printf("after value0: %s", buf);

    if (delay_ms > 0)
        usleep((useconds_t)delay_ms * 1000);

    ret = gpio_set_value(gpio, value1);
    if (ret)
        die_step("write value1", ret);

    ret = gpio_get_value(gpio, buf, sizeof(buf));
    if (ret)
        die_step("read value1", ret);
    printf("after value1: %s", buf);

    return EXIT_SUCCESS;
}
