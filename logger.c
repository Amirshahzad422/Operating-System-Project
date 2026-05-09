#include "logger.h"

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

static pthread_mutex_t g_log_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_use_color = -1;

static const char *component_color(const char *component) {
    if (!component) {
        return "\x1b[37m";
    }
    if (strcmp(component, "SYSTEM") == 0) {
        return "\x1b[36m";
    }
    if (strcmp(component, "CTRL") == 0) {
        return "\x1b[35m";
    }
    if (strcmp(component, "TRAFFIC") == 0) {
        return "\x1b[33m";
    }
    if (strcmp(component, "PARKING") == 0) {
        return "\x1b[32m";
    }
    if (strcmp(component, "VEHICLE") == 0) {
        return "\x1b[37m";
    }
    if (strcmp(component, "STATE") == 0) {
        return "\x1b[94m";
    }
    return "\x1b[37m";
}

static int should_use_color(void) {
    if (g_use_color == -1) {
        g_use_color = isatty(STDOUT_FILENO) ? 1 : 0;
    }
    return g_use_color;
}

void log_line(const char *component, const char *scope, const char *event, const char *details) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm tm_now;
    localtime_r(&tv.tv_sec, &tm_now);
    int ms = (int) (tv.tv_usec / 1000);

    pthread_mutex_lock(&g_log_lock);
    const char *comp = component ? component : "SYS";
    const char *scp = scope ? scope : "-";
    const char *evt = event ? event : "-";
    const char *det = details ? details : "-";
    pid_t pid = getpid();

    if (should_use_color()) {
        printf("\x1b[90m[p%05d %02d:%02d:%02d.%03d]\x1b[0m %s%-8s\x1b[0m %-3s | %-12s | %s\n",
               (int) pid, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, ms,
               component_color(comp), comp, scp, evt, det);
    } else {
        printf("[p%05d %02d:%02d:%02d.%03d] %-8s %-3s | %-12s | %s\n",
               (int) pid, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, ms,
               comp, scp, evt, det);
    }
    fflush(stdout);
    pthread_mutex_unlock(&g_log_lock);
}

void log_linef(const char *component, const char *scope, const char *event, const char *fmt, ...) {
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    log_line(component, scope, event, buffer);
}
