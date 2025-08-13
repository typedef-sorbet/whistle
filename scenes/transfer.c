#include <scenes/transfer.h>
#include <whistle.h>

#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>

static const char *thread_state_str(FuriThreadState state) {
    switch (state) {
        case FuriThreadStateStopped:    return "Stopped";
        case FuriThreadStateRunning:    return "Running";
        case FuriThreadStateStarting:   return "Starting";
        case FuriThreadStateStopping:   return "Stopping";
        default:                        return "Unknown";
    }
}

void transfer_on_enter(void* _context) {
    TRACE;

    furi_assert(_context);

    FURI_LOG_I(TAG, "furi_assert in transfer_on_enter passed");

    whistle_context* context = (whistle_context*)_context;

    FURI_LOG_I(TAG, "whistle_context* cast from _context in transfer_on_enter passed");

    // Allocate/open subghz device
    subghz_devices_init();

    const SubGhzDevice* subghz_device = subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME);

    FURI_LOG_I(TAG, "subghz_devices_get_by_name in transfer_on_enter passed");

    // Allocate subghz worker
    context->subghz_worker =
        subghz_worker_alloc(subghz_device, context->mode, context->selected_file, context, transfer_on_thread_state_change);

    FURI_LOG_I(TAG, "subghz_worker_alloc in transfer_on_enter passed");

    // TODO error checking

    subghz_worker_start(context->subghz_worker, context->frequency);

    FURI_LOG_I(TAG, "subghz_worker_start in transfer_on_enter passed");

    view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_Transfer);
}

bool transfer_on_event(void* _context, SceneManagerEvent event) {
    TRACE;

    furi_assert(_context);

    whistle_context *context = (whistle_context*)_context;

    bool consumed = true;

    // TODO
    (void)_context;
    (void)event;

    scene_manager_next_scene(context->scene_manager, SCENE_Done);

    return consumed;
}

void transfer_on_exit(void* _context) {
    TRACE;

    furi_assert(_context);
}

void transfer_on_thread_state_change(FuriThread *thread, FuriThreadState state, void *_context) {
    TRACE;

    furi_assert(thread); furi_assert(_context);

    whistle_context *context = (whistle_context*)_context;

    FURI_LOG_D(TAG, "SubGhzWorker thread state changed: %s", thread_state_str(state));

    switch (state) {
        case FuriThreadStateStopped:
            FURI_LOG_I(TAG, "SubGhzWorker done, moving to Done scene");

            subghz_worker_thread_stop(context->subghz_worker);
            subghz_worker_free(context->subghz_worker);

            scene_manager_next_scene(context->scene_manager, SCENE_Done);
        break;

        default:
            // no-op
        break;
    }
}
