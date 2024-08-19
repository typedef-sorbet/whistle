#include <helpers/subghz_worker.h>

static const uint32_t crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535,
    0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd,
    0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d,
    0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
    0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac,
    0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab,
    0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
    0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb,
    0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea,
    0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce,
    0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
    0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409,
    0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739,
    0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268,
    0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0,
    0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8,
    0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
    0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703,
    0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7,
    0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae,
    0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6,
    0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d,
    0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5,
    0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
    0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

static inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

static uint32_t crc32(unsigned char* data, size_t size) {
    const uint8_t* p = data;
    uint32_t crc;

    crc = ~0U;
    while(size--) {
        crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    }

    return crc ^ ~0U;
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

        whistle_packet packet = subghz_worker_pack(readbuf, readlen, instance->offset);

        instance->offset += readlen;

        subghz_worker_write(instance, (uint8_t*)&packet, sizeof(packet));

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

    size_t len = subghz_worker_read(instance, buffer, sizeof(buffer));
    size_t copy_len = min(len, sizeof(instance->packet_buffer) - instance->packet_buffer_ptr);

    // enshrining this dumbass piece of code as a cautionary tale against writing code at 1am
    //  memcpy(buffer, (instance->packet_buffer + instance->packet_buffer_ptr), copy_len);
    // what the fuck do you think brackets were invented for idiot
    memcpy(buffer, &instance->packet_buffer[instance->packet_buffer_ptr], copy_len);

    // TODO this is probably pretty inefficient to do every read, is there a better way?
    for(size_t i = 0; i < sizeof(instance->packet_buffer) - 4; ++i) {
        uint32_t sentinel_window;
        // Is this any faster/slower than indexing into packet_buffer and shifting?
        memcpy(&sentinel_window, &instance->packet_buffer[i], sizeof(uint32_t));
        if(sentinel_window == WHISTLE_PACKET_SENTINEL) {
            // we're done! sound the alarm!
            event.event = EVENT_SubGhzWorker_PacketReady;
            instance->sentinel_offset = i;
            furi_message_queue_put(instance->event_queue, &event, FuriWaitForever);
        }
    }

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
        instance->packet_buffer_ptr = 0;

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

whistle_packet subghz_worker_pack(unsigned char* data, size_t size, uint32_t offset) {
    furi_assert(data);
    furi_assert(size < WHISTLE_PACKET_MAX_SIZE);

    whistle_packet packet;

    memset(&packet, 0x00, sizeof(packet));

    memcpy(packet.data, data, size);
    packet.packet_size =
        size + sizeof(packet.packet_crc) + sizeof(packet.packet_size) + sizeof(packet.sentinel);
    packet.file_offset = offset;
    packet.sentinel = WHISTLE_PACKET_SENTINEL;

    packet.packet_crc = crc32((unsigned char*)&packet, sizeof(packet));

    return packet;
}

void subghz_worker_pop_packet(subghz_worker* instance, whistle_packet* packet_ptr) {
    furi_assert(instance);
    furi_assert(packet_ptr);

    unsigned char* packet_start = &instance->packet_buffer
                                       [instance->sentinel_offset - sizeof(whistle_packet) +
                                        sizeof(instance->sentinel_offset)];

    memcpy(packet_ptr, packet_start, sizeof(whistle_packet));

    // roll the buffer
    unsigned char* temp_buf[2 * WHISTLE_PACKET_MAX_SIZE] = {0};

    memcpy(
        temp_buf,
        &instance->packet_buffer[instance->sentinel_offset + sizeof(instance->sentinel_offset)],
        sizeof(temp_buf));

    memset(instance->packet_buffer, 0x00, sizeof(instance->packet_buffer));

    memcpy(instance->packet_buffer, temp_buf, sizeof(temp_buf));
}
