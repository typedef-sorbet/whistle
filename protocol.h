#pragma once
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <sys/types.h>

#define WHISTLE_PACKET_MAX_SIZE 64
#define WHISTLE_PACKET_DATA_MAX_SIZE \
    WHISTLE_PACKET_MAX_SIZE - sizeof(whistle_packet_header) - sizeof(uint32_t)
#define WHISTLE_PACKET_SENTINEL 0xD0D0CAFE

typedef enum {
    PACKET_TYPE_SYN,
    PACKET_TYPE_DATA,
    PACKET_TYPE_ACK,
    PACKET_TYPE_FIN
} whistle_packet_type;

typedef enum {
    RECEIVER_STATE_IDLE,
    RECEIVER_STATE_RECEIVING,
    RECEIVER_STATE_DONE
} whistle_protocol_state_receiver;

typedef enum {
    SENDER_STATE_IDLE,
    SENDER_STATE_SYN_SENT,
    SENDER_STATE_SENDING,
    SENDER_STATE_FIN_SENT,
    SENDER_STATE_DONE
} whistle_protocol_state_sender;

typedef union {
    whistle_protocol_state_receiver receiver;
    whistle_protocol_state_sender sender;
} whistle_protocol_state;

typedef struct {
    whistle_packet_type type;
    uint32_t crc;
    uint8_t size;
} whistle_packet_header;

typedef struct {
    uint64_t total_size;
} whistle_packet_syn;

typedef struct {
    uint16_t sequence_num;
} whistle_packet_ack;

typedef struct {
    uint16_t sequence_num;
    unsigned char data[WHISTLE_PACKET_DATA_MAX_SIZE];
} whistle_packet_data;

typedef union {
    whistle_packet_ack ack;
    whistle_packet_data data;
    whistle_packet_syn syn;
} whistle_packet_payload;

typedef struct {
    whistle_packet_header header;
    whistle_packet_payload payload;
    uint32_t sentinel;
} whistle_packet;

typedef struct {
    unsigned char packet_buffer[2 * WHISTLE_PACKET_MAX_SIZE];
    size_t packet_buffer_ptr;
    size_t sentinel_offset;
    whistle_protocol_state state;
    uint64_t total_size;
    uint16_t sequence_num;
    void* subghz_worker_parent;
} whistle_protocol_worker;

whistle_protocol_worker* protocol_worker_alloc(void* parent);

void protocol_pop_packet(whistle_protocol_worker* instance, whistle_packet* packet_ptr);
void protocol_process_packet(whistle_protocol_worker* instance, whistle_packet* packet);
whistle_packet
    protocol_pack(whistle_packet_type type, whistle_packet_payload payload, size_t size);

#endif // PROTOCOL_H
