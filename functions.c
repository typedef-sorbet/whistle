#include "core/log.h"
#include <functions.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void hexdump(const void *data, size_t len) {
    char hexbuf[(4 * len) + 1];
    memset(hexbuf, 0x00, sizeof(hexbuf));

    const char *datachars = (const char *)data;

    for(size_t i = 0; i < len; ++i) {
        snprintf(&hexbuf[2 * i], 4, "%02x ", (uint8_t)datachars[i]);
    }

    FURI_LOG_D(TAG, "0x%s", len, hexbuf);
}