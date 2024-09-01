#include <helpers/subghz_worker.h>

static inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

static int32_t subghz_worker_thread_sender(void* _context) {
    FURI_LOG_I(TAG, "SubGhz senderworker start");

    subghz_worker* instance = (subghz_worker*)_context;

    while(instance->worker_running) {
        // file should already be open
        unsigned char readbuf[WHISTLE_PACKET_DATA_MAX_SIZE];
        size_t readlen = stream_read(instance->file_stream, readbuf, sizeof(readbuf));

        if(readlen == 0) {
            instance->worker_running = false;
            continue;
        }

        // whistle_packet packet = subghz_worker_pack(readbuf, readlen, instance->offset);

        // instance->offset += readlen;

        // subghz_worker_write(instance, (uint8_t*)&packet, sizeof(packet));

        furi_delay_ms(10);
    }

    FURI_LOG_I(TAG, "SubGhz sender worker stop");
    return 0;
}

static int32_t subghz_worker_thread_receiver(void* _context) {
    FURI_LOG_I(TAG, "SubGhz receiver worker start");

    subghz_worker* instance = (subghz_worker*)_context;

    while(instance->worker_running) {
        // TODO put messages onto the queue here?
    }

    FURI_LOG_I(TAG, "SubGhz receiver worker stop");
    return 0;
}

subghz_worker_event subghz_worker_get_event(subghz_worker* instance) {
    furi_assert(instance);

    subghz_worker_event event;

    if(furi_message_queue_get(instance->event_queue, &event, FuriWaitForever) != FuriStatusOk) {
        event.event = EVENT_SubGhzWorker_NoEvent;
    }

    return event;
}

// TODO it's probably more idiomatic to have setters for these, but whatever
// TODO should also do error checking here
subghz_worker*
    subghz_worker_alloc(const SubGhzDevice* device, whistle_mode mode, FuriString* path) {
    subghz_worker* instance = malloc(sizeof(subghz_worker));

    instance->mode = mode;
    instance->subghz_device = device;

    instance->path = furi_string_alloc_set(path);
    instance->storage = furi_record_open(RECORD_STORAGE);

    instance->file_stream = file_stream_alloc(instance->storage);

    instance->thread = furi_thread_alloc_ex(
        "SubGhzWorker",
        2048,
        mode == MODE_Sending ? subghz_worker_thread_sender : subghz_worker_thread_receiver,
        instance);
    instance->subghz_txrx = subghz_tx_rx_worker_alloc();
    instance->event_queue = furi_message_queue_alloc(80, sizeof(subghz_worker_event));

    instance->protocol_worker = protocol_worker_alloc((void*)instance);

    return instance;
}

void subghz_worker_free(subghz_worker* instance) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    furi_message_queue_free(instance->event_queue);
    subghz_tx_rx_worker_free(instance->subghz_txrx);
    furi_thread_free(instance->thread);
    furi_string_free(instance->path);

    furi_record_close(RECORD_STORAGE);

    free(instance);
}

static void subghz_worker_handle_have_read(void* context) {
    furi_assert(context);

    subghz_worker* instance = (subghz_worker*)context;
    subghz_worker_event event;

    unsigned char buffer[128];
    memset(buffer, 0x00, sizeof(buffer));

    // size_t len = subghz_worker_read(instance, buffer, sizeof(buffer));
    // size_t copy_len = min(len, sizeof(instance->packet_buffer) - instance->packet_buffer_ptr);

    // // enshrining this dumbass piece of code as a cautionary tale against writing code at 1am
    // //  memcpy(buffer, (instance->packet_buffer + instance->packet_buffer_ptr), copy_len);
    // // what the fuck do you think brackets were invented for idiot
    // memcpy(buffer, &instance->packet_buffer[instance->packet_buffer_ptr], copy_len);

    // // TODO this is probably pretty inefficient to do every read, is there a better way?
    // for(size_t i = 0; i < sizeof(instance->packet_buffer) - 4; ++i) {
    //     uint32_t sentinel_window;
    //     // Is this any faster/slower than indexing into packet_buffer and shifting?
    //     memcpy(&sentinel_window, &instance->packet_buffer[i], sizeof(uint32_t));
    //     if(sentinel_window == WHISTLE_PACKET_SENTINEL) {
    //         // we're done! sound the alarm!
    //         event.event = EVENT_SubGhzWorker_PacketReady;
    //         instance->sentinel_offset = i;
    //         furi_message_queue_put(instance->event_queue, &event, FuriWaitForever);
    //     }
    // }

    // TODO do other handling here, I guess?

    instance->last_time_rx_data = furi_get_tick();
    event.event = EVENT_SubGhzWorker_RxData;
    furi_message_queue_put(instance->event_queue, &event, FuriWaitForever);
}

bool subghz_worker_start(subghz_worker* instance, uint32_t frequency) {
    furi_assert(instance);
    furi_assert(!instance->worker_running);

    bool res = false;

    if(subghz_tx_rx_worker_start(instance->subghz_txrx, instance->subghz_device, frequency)) {
        furi_message_queue_reset(instance->event_queue);

        if(instance->mode == MODE_Receiving) {
            subghz_tx_rx_worker_set_callback_have_read(
                instance->subghz_txrx, subghz_worker_handle_have_read, instance);
        }

        instance->worker_running = true;
        instance->last_time_rx_data = 0;
        instance->offset = 0;

        if(file_stream_open(
               instance->file_stream,
               furi_string_get_cstr(instance->path),
               instance->mode == MODE_Sending ? FSAM_READ : FSAM_WRITE,
               instance->mode == MODE_Sending ? FSOM_OPEN_EXISTING : FSOM_CREATE_ALWAYS)) {
            furi_thread_start(instance->thread);

            res = true;
        } else {
            FURI_LOG_E(
                TAG,
                "Unable to open file %s for %s",
                furi_string_get_cstr(instance->path),
                instance->mode == MODE_Sending ? "reading" : "writing");
        }

    } else {
        FURI_LOG_E(TAG, "Unable to start subghz_tx_rx_worker");
    }

    return res;
}

void subghz_worker_thread_stop(subghz_worker* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_running);

    if(subghz_tx_rx_worker_is_running(instance->subghz_txrx)) {
        subghz_tx_rx_worker_stop(instance->subghz_txrx);
    }

    file_stream_close(instance->file_stream);

    instance->worker_running = false;

    furi_thread_join(instance->thread);
}

bool subghz_worker_is_running(subghz_worker* instance) {
    furi_assert(instance);
    return instance->worker_running;
}

void subghz_worker_put_event(subghz_worker* instance, subghz_worker_event* event) {
    furi_assert(instance);
    furi_message_queue_put(instance->event_queue, event, FuriWaitForever);
}

size_t subghz_worker_available(subghz_worker* instance) {
    furi_assert(instance);
    return subghz_tx_rx_worker_available(instance->subghz_txrx);
}

size_t subghz_worker_read(subghz_worker* instance, uint8_t* data, size_t size) {
    furi_assert(instance);
    return subghz_tx_rx_worker_read(instance->subghz_txrx, data, size);
}

bool subghz_worker_write(subghz_worker* instance, uint8_t* data, size_t size) {
    furi_assert(instance);

    bool res = subghz_tx_rx_worker_write(instance->subghz_txrx, data, size);

    if(res) {
        char hexbuf[(4 * size) + 1];
        memset(hexbuf, 0x00, sizeof(hexbuf));

        for(size_t i = 0; i < size; ++i) {
            snprintf(&hexbuf[2 * i], 4, "%02x ", (uint8_t)data[i]);
        }

        FURI_LOG_D(TAG, "Wrote %d bytes: 0x%s", size, hexbuf);
    } else {
        FURI_LOG_E(TAG, "Failed to write %d bytes", size);
    }

    return res;
}
