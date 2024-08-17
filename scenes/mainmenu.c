#include <scenes/mainmenu.h>

#include <whistle.h>

void mainmenu_selection_callback(void* _context, uint32_t choice) {
    TRACE;

    FURI_LOG_T(TAG, __FUNCTION__);

    whistle_context* context = (whistle_context*)_context;

    switch(choice) {
    case OPTION_MainMenu_SendFile:
        FURI_LOG_I(TAG, "Selected Send File\n");
        scene_manager_handle_custom_event(context->scene_manager, EVENT_MainMenu_SendFileSelected);
        break;

    case OPTION_MainMenu_ReceiveFile:
        FURI_LOG_I(TAG, "Selected Receive File\n");
        scene_manager_handle_custom_event(
            context->scene_manager, EVENT_MainMenu_ReceiveFileSelected);
        break;
    }
}

void mainmenu_on_enter(void* _context) {
    TRACE;

    whistle_context* context = (whistle_context*)_context;

    menu_reset(context->menu);

    menu_add_item(
        context->menu,
        "Send File",
        NULL,
        OPTION_MainMenu_SendFile,
        mainmenu_selection_callback,
        context);

    menu_add_item(
        context->menu,
        "Receive File",
        NULL,
        OPTION_MainMenu_ReceiveFile,
        mainmenu_selection_callback,
        context);

    view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_MainMenu);
}

bool mainmenu_on_event(void* _context, SceneManagerEvent event) {
    TRACE;

    // (void)_context;
    // (void)event;

    whistle_context* context = (whistle_context*)_context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom: {
        switch(event.event) {
        case EVENT_MainMenu_SendFileSelected:
            FURI_LOG_I(TAG, "Got event for Send File selected\n");
            context->mode = MODE_Sending;
            scene_manager_next_scene(context->scene_manager, SCENE_Options);
            consumed = true;
            break;

        case EVENT_MainMenu_ReceiveFileSelected:
            FURI_LOG_I(TAG, "Got event for Receive File selected\n");
            context->mode = MODE_Receiving;
            scene_manager_next_scene(context->scene_manager, SCENE_Options);
            consumed = true;
            break;
        }
    } break;

    default:
        consumed = false;
        break;
    }

    return consumed;
}

void mainmenu_on_exit(void* _context) {
    TRACE;

    whistle_context* context = (whistle_context*)_context;
    menu_reset(context->menu);
}
