#include <scenes/file_browser.h>
#include <fliptp.h>

void file_browser_browser_callback(void* _context) {
    TRACE;

    fliptp_context* context = (fliptp_context*)_context;

    FURI_LOG_D(
        TAG,
        "file_browser_browser_callback, context->selected_file = %s",
        furi_string_get_cstr(context->selected_file));

    // NOTE not using next_scene bc there's some jank around how that affects the scene stack;
    // the behavior caused by simulating a back event makes more sense IMO
    scene_manager_handle_back_event(context->scene_manager);
}

bool file_browser_browser_item_callback(
    FuriString* path,
    void* _context,
    uint8_t** icon,
    FuriString* item_name) {
    TRACE;
    furi_assert(_context);

    (void)icon;
    (void)item_name;
    (void)path;
    (void)_context;

    return true;
}

void file_browser_on_enter(void* _context) {
    TRACE;
    furi_assert(_context);

    fliptp_context* context = (fliptp_context*)_context;

    file_browser_configure(
        context->file_browser,
        NULL, // ???
        "/",
        true,
        false,
        NULL, // ???
        false);

    file_browser_set_callback(context->file_browser, file_browser_browser_callback, context);
    file_browser_set_item_callback(
        context->file_browser, file_browser_browser_item_callback, context);

    // ???
    file_browser_start(context->file_browser, context->selected_file);

    view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_FileBrowser);
}

bool file_browser_on_event(void* _context, SceneManagerEvent event) {
    TRACE;

    (void)_context;
    // (void)event;

    // fliptp_context* context = (fliptp_context*)_context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom: {
        switch(event.event) {
        default:
            consumed = false;
            break;
        }
    } break;

    default:
        consumed = false;
        break;
    }

    return consumed;
}

void file_browser_on_exit(void* _context) {
    TRACE;
    furi_assert(_context);

    fliptp_context* context = (fliptp_context*)_context;

    file_browser_stop(context->file_browser);
}
