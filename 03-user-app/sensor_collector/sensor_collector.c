#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_LINE 256

struct config {
    char device[64];
    int interval_ms;
    char temperature_path[128];
    char humidity_path[128];
    char key_path[128];
    char led_path[128];
    char log_path[128];
};

static void trim_newline(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

static void set_defaults(struct config *cfg)
{
    snprintf(cfg->device, sizeof(cfg->device), "imx6ull-pro");
    cfg->interval_ms = 1000;
    snprintf(cfg->temperature_path, sizeof(cfg->temperature_path), "/tmp/imx6ull_temperature");
    snprintf(cfg->humidity_path, sizeof(cfg->humidity_path), "/tmp/imx6ull_humidity");
    snprintf(cfg->key_path, sizeof(cfg->key_path), "/tmp/imx6ull_key_state");
    snprintf(cfg->led_path, sizeof(cfg->led_path), "/tmp/imx6ull_led_state");
    snprintf(cfg->log_path, sizeof(cfg->log_path), "../logs/sensor_collector.log");
}

static void copy_value(char *dst, size_t dst_size, const char *value)
{
    snprintf(dst, dst_size, "%s", value);
}

static int load_config(const char *path, struct config *cfg)
{
    FILE *fp = fopen(path, "r");
    char line[MAX_LINE];

    if (!fp) {
        fprintf(stderr, "config open failed: %s: %s\n", path, strerror(errno));
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        char *eq;
        trim_newline(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        eq = strchr(line, '=');
        if (!eq) {
            continue;
        }
        *eq = '\0';
        const char *key = line;
        const char *value = eq + 1;

        if (strcmp(key, "device") == 0) copy_value(cfg->device, sizeof(cfg->device), value);
        else if (strcmp(key, "interval_ms") == 0) cfg->interval_ms = atoi(value);
        else if (strcmp(key, "temperature_path") == 0) copy_value(cfg->temperature_path, sizeof(cfg->temperature_path), value);
        else if (strcmp(key, "humidity_path") == 0) copy_value(cfg->humidity_path, sizeof(cfg->humidity_path), value);
        else if (strcmp(key, "key_path") == 0) copy_value(cfg->key_path, sizeof(cfg->key_path), value);
        else if (strcmp(key, "led_path") == 0) copy_value(cfg->led_path, sizeof(cfg->led_path), value);
        else if (strcmp(key, "log_path") == 0) copy_value(cfg->log_path, sizeof(cfg->log_path), value);
    }

    fclose(fp);
    return 0;
}

static double read_double_or_default(const char *path, double fallback)
{
    FILE *fp = fopen(path, "r");
    double value = fallback;
    if (!fp) {
        return fallback;
    }
    if (fscanf(fp, "%lf", &value) != 1) {
        value = fallback;
    }
    fclose(fp);
    return value;
}

static int read_int_or_default(const char *path, int fallback)
{
    FILE *fp = fopen(path, "r");
    int value = fallback;
    if (!fp) {
        return fallback;
    }
    if (fscanf(fp, "%d", &value) != 1) {
        value = fallback;
    }
    fclose(fp);
    return value;
}

static void append_log(const char *path, const char *json)
{
    FILE *fp = fopen(path, "a");
    if (!fp) {
        return;
    }
    fprintf(fp, "%s\n", json);
    fclose(fp);
}

int main(int argc, char **argv)
{
    struct config cfg;
    const char *config_path = argc > 1 ? argv[1] : "../config/collector.conf";

    set_defaults(&cfg);
    load_config(config_path, &cfg);

    for (;;) {
        char json[512];
        long now = (long)time(NULL);
        double temperature = read_double_or_default(cfg.temperature_path, 26.5);
        double humidity = read_double_or_default(cfg.humidity_path, 58.2);
        int key_state = read_int_or_default(cfg.key_path, 0);
        int led_state = read_int_or_default(cfg.led_path, 0);

        snprintf(json, sizeof(json),
                 "{\"device\":\"%s\",\"timestamp\":%ld,\"temperature\":%.2f,"
                 "\"humidity\":%.2f,\"key_state\":%d,\"led_state\":%d}",
                 cfg.device, now, temperature, humidity, key_state, led_state);

        puts(json);
        fflush(stdout);
        append_log(cfg.log_path, json);
        usleep((useconds_t)cfg.interval_ms * 1000);
    }

    return 0;
}

