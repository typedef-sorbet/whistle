#include "core/log.h"
#include <functions.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TAG "whistle"

void hexdump(const void *data, size_t len, int log_level) {
    char hexbuf[(4 * len) + 1];
    memset(hexbuf, 0x00, sizeof(hexbuf));

    const char *datachars = (const char *)data;

    for(size_t i = 0; i < len; ++i) {
        snprintf(&hexbuf[2 * i], 4, "%02x ", (uint8_t)datachars[i]);
    }

    switch (log_level) {
        case LOG_TRACE:
            FURI_LOG_T(TAG, "0x%s", hexbuf);
            break;
        case LOG_DEBUG:
            FURI_LOG_D(TAG, "0x%s", hexbuf);
            break;
        case LOG_INFO:
            FURI_LOG_I(TAG, "0x%s", hexbuf);
            break;
        case LOG_WARNING:
            FURI_LOG_W(TAG, "0x%s", hexbuf);
            break;
        case LOG_ERROR:
            FURI_LOG_E(TAG, "0x%s", hexbuf);
            break;
    }
}