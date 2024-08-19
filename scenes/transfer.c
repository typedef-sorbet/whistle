#include <scenes/transfer.h>
#include <whistle.h>

#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>

void transfer_on_enter(void* _context) {
    // TODO the app seems to hang somewhere around here...
    // It's probably a race condition with the spawned thread.
    TRACE;

    furi_assert(_context);

    whistle_context* context = (whistle_context*)_context;

    // Allocate/open subghz device

    const SubGhzDevice* subghz_device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);

    // Allocate subghz worker
    context->subghz_worker =
        subghz_worker_alloc(subghz_device, context->mode, context->selected_file);

    // TODO error checking

    subghz_worker_start(context->subghz_worker, context->frequency);

    view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_Transfer);
}

bool transfer_on_event(void* _context, SceneManagerEvent event) {
    TRACE;

    furi_assert(_context);

    bool consumed = false;

    // TODO
    (void)_context;
    (void)event;

    return consumed;
}

void transfer_on_exit(void* _context) {
    TRACE;

    furi_assert(_context);

    // TODO
    (void)_context;
}
