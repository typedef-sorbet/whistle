#include <scenes/file_browser.h>
#include <fliptp.h>

void file_browser_browser_callback(void* _context) {
    TRACE;
    (void)_context;
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

    fliptp_context* context = (fliptp_context*)_context;

    // TODO is this what I'm supposed to do? or does returning true automatically do this?
    context->selected_file = path;

    // file_browser_on_exit(context);

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

    scene_manager_next_scene(context->scene_manager, SCENE_Options);
}
