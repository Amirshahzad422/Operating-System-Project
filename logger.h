#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

void log_line(const char *component, const char *scope, const char *event, const char *details);
void log_linef(const char *component, const char *scope, const char *event, const char *fmt, ...);

#endif
