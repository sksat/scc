#ifndef UTIL_H_
#define UTIL_H_

#include <stdarg.h>

void info(const char *fmt, ...);
void error(const char *fmt, ...) __attribute__((noreturn));

#endif
