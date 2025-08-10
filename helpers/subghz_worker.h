#include <furi/core/thread.h>
#include <lib/subghz/subghz_tx_rx_worker.h>
#include <lib/toolbox/stream/file_stream.h>
#include <enums.h>
#include <protocol.h>

#pragma once
#ifndef SUBGHZ_WORKER_H
#define SUBGHZ_WORKER_H

typedef enum {
    EVENT_SubGhzWorker_NoEvent,
    EVENT_SubGhzWorker_UserEnter,
    EVENT_SubGhzWorker_UserExit,
    EVENT_SubGhzWorker_InputData,
    EVENT_SubGhzWorker_RxData,
    EVENT_SubGhzWorker_PacketReady,
    EVENT_SubGhzWorker_count
} subghz_worker_event_type;

typedef struct {
    subghz_worker_event_type event;
    char c;
} subghz_worker_event;

typedef struct {
    FuriThread* thread;
    SubGhzTxRxWorker* subghz_txrx;

    volatile bool worker_running;
    volatile bool worker_stopping;
    whistle_mode mode;

    FuriMessageQueue* event_queue;
    uint32_t last_time_rx_data;

    // TODO rightsize
    unsigned char packet_buffer[2 * WHISTLE_PACKET_MAX_SIZE];
    size_t packet_buffer_ptr;
    size_t sentinel_offset;

    Storage* storage;
    FuriString* path;
    Stream* file_stream;
    size_t offset;

    const SubGhzDevice* subghz_device;

    // shouldn't need Cli pointer, right?
} subghz_worker;

subghz_worker_event subghz_worker_get_event(subghz_worker* instance);

subghz_worker*
    subghz_worker_alloc(const SubGhzDevice* device, whistle_mode mode, FuriString* path);

void subghz_worker_free(subghz_worker* instance);

bool subghz_worker_start(subghz_worker* instance, uint32_t frequency);

void subghz_worker_thread_stop(subghz_worker* instance);

bool subghz_worker_is_running(subghz_worker* instance);

void subghz_worker_put_event(subghz_worker* instance, subghz_worker_event* event);

size_t subghz_worker_available(subghz_worker* instance);

size_t subghz_worker_read(subghz_worker* instance, uint8_t* data, size_t size);

void subghz_worker_pop_packet(subghz_worker* instance, whistle_packet* packet_ptr);

whistle_packet subghz_worker_pack_data(unsigned char* data, size_t size, uint32_t offset);

whistle_packet subghz_worker_pack_preamble(uint32_t file_size, char *file_name, size_t file_name_size);

whistle_packet subghz_worker_pack_postamble();

bool subghz_worker_write(subghz_worker* instance, uint8_t* data, size_t size);

#endif // SUBGHZ_WORKER_H
