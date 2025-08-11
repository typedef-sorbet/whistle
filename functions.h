#include <strings.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define TAG               "whistle"

#define LOG_TRACE         0
#define LOG_DEBUG         1
#define LOG_INFO          2
#define LOG_WARNING       3
#define LOG_ERROR         4

void hexdump(const void *data, size_t len, int log_level);

#endif