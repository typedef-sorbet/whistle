#include <stdint.h>

#pragma once
#ifndef PROTOCOL_H
#define PROTOCOL_H

#define WHISTLE_PACKET_MAX_SIZE 64
#define WHISTLE_PACKET_DATA_MAX_SIZE \
    WHISTLE_PACKET_MAX_SIZE - (2 * sizeof(uint32_t)) - sizeof(whistle_packet_type)
#define WHISTLE_PACKET_SENTINEL     0xD0D0CAFE  // This assumes that nothing sent will ever contain this byte sequence.

typedef enum {
    WHISTLE_TYPE_PREAMBLE,
    WHISTLE_TYPE_DATA,
    WHISTLE_TYPE_count
} whistle_packet_type;

typedef struct {
    uint32_t file_size; // in bytes, restricts us to 2^32 bytes or approx. 4.2 GB
    char file_name[WHISTLE_PACKET_DATA_MAX_SIZE - sizeof(uint32_t)];
} whistle_preamble;

typedef struct {
    uint32_t offset;
    uint8_t size;
    unsigned char data[WHISTLE_PACKET_DATA_MAX_SIZE - sizeof(uint32_t)];
} whistle_data;

typedef union {
    whistle_preamble preamble;
    whistle_data data_chunk;
} whistle_packet_inner;

typedef struct {
    uint32_t packet_crc;                                // calculated assuming CRC = 0
    whistle_packet_type packet_type;
    whistle_packet_inner inner;
    uint32_t sentinel;
} whistle_packet;       


#endif // PROTOCOL_H
