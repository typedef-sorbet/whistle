#include <stdint.h>

#pragma once
#ifndef PROTOCOL_H
#define PROTOCOL_H

#define WHISTLE_PACKET_MAX_SIZE 64
#define WHISTLE_PACKET_DATA_MAX_SIZE \
    WHISTLE_PACKET_MAX_SIZE - (3 * sizeof(uint32_t)) - sizeof(uint8_t)
#define WHISTLE_PACKET_SENTINEL 0xD0D0CAFE

typedef struct {
    uint32_t packet_crc;
    uint8_t packet_size;
    uint32_t file_offset;
    unsigned char data[WHISTLE_PACKET_DATA_MAX_SIZE];
    uint32_t sentinel;
} whistle_packet;

#endif // PROTOCOL_H
