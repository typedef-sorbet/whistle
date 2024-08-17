#include <fliptp.h>
#include <fliptp_i.h>

bool fliptp_custom_event_cb(void* _context, uint32_t event) {
    TRACE;

    fliptp_context* context = (fliptp_context*)_context;
    return scene_manager_handle_custom_event(context->scene_manager, event);
}

bool fliptp_custom_navigation_event_callback(void* _context) {
    TRACE;

    fliptp_context* context = (fliptp_context*)_context;

    return scene_manager_handle_back_event(context->scene_manager);
}

fliptp_context* context_alloc() {
    TRACE;

    fliptp_context* context = malloc(sizeof(fliptp_context));

    if(NULL == context) {
        goto err_context;
    }

    context->scene_manager = scene_manager_alloc(&fliptp_scene_manager_handlers, context);
    if(NULL == context->scene_manager) {
        goto err_scene_manager;
    }

    context->view_dispatcher = view_dispatcher_alloc();
    if(NULL == context->view_dispatcher) {
        goto err_view_dispatcher;
    }

    // Menu setup
    context->menu = menu_alloc();
    if(NULL == context->menu) {
        goto err_menu_alloc;
    }

    // VariableItemList setup
    context->option_list = variable_item_list_alloc();
    if(NULL == context->option_list) {
        goto err_option_list;
    }

    // SubGhzSetting setup
    context->subghz_settings = subghz_setting_alloc();
    if(NULL == context->subghz_settings) {
        goto err_subghz;
    }

    // File string setup
    context->selected_file = furi_string_alloc();
    if(NULL == context->selected_file) {
        goto err_string;
    }

    // File browser setup
    context->file_browser = file_browser_alloc(context->selected_file);
    if(NULL == context->file_browser) {
        goto err_file_browser;
    }

    // Gui setup
    context->gui = furi_record_open(RECORD_GUI);
    if(NULL == context->gui) {
        goto err_gui;
    }

    // ViewDispatcher setup
    view_dispatcher_enable_queue(context->view_dispatcher);

    view_dispatcher_set_event_callback_context(context->view_dispatcher, context);

    view_dispatcher_set_custom_event_callback(context->view_dispatcher, fliptp_custom_event_cb);

    view_dispatcher_set_navigation_event_callback(
        context->view_dispatcher, fliptp_custom_navigation_event_callback);

    view_dispatcher_attach_to_gui(
        context->view_dispatcher, context->gui, ViewDispatcherTypeFullscreen);

    // Add views
    view_dispatcher_add_view(
        context->view_dispatcher, VIEW_MainMenu, menu_get_view(context->menu));
    view_dispatcher_add_view(
        context->view_dispatcher, VIEW_Options, variable_item_list_get_view(context->option_list));
    view_dispatcher_add_view(
        context->view_dispatcher, VIEW_FileBrowser, file_browser_get_view(context->file_browser));

    // Set sane defaults for options
    context->encryption = false;
    context->frequency = 432000000;
    furi_string_reset(context->selected_file);

    return context;

err_gui:
    file_browser_free(context->file_browser);

err_file_browser:
    furi_string_free(context->selected_file);

err_string:
    subghz_setting_free(context->subghz_settings);

err_subghz:
    variable_item_list_free(context->option_list);

err_option_list:
    menu_free(context->menu);

err_menu_alloc:
    view_dispatcher_free(context->view_dispatcher);

err_view_dispatcher:
    scene_manager_free(context->scene_manager);

err_scene_manager:
    free(context);

err_context:
    return NULL;
}

void context_free(fliptp_context* context) {
    TRACE;

    scene_manager_free(context->scene_manager);

    // Remove all views
    for(int i = 0; i < VIEW_count; ++i) {
        view_dispatcher_remove_view(context->view_dispatcher, i);
    }

    view_dispatcher_free(context->view_dispatcher);

    menu_free(context->menu);

    free(context);
}

int32_t fliptp_main() {
    TRACE;

    fliptp_context* context = context_alloc();

    if(NULL == context) {
        FURI_LOG_E(TAG, "context_alloc failed!\n");
        return 1;
    }

    FURI_LOG_I(TAG, "context_alloc succeeded!\n");

    scene_manager_next_scene(context->scene_manager, SCENE_MainMenu);

    FURI_LOG_I(TAG, "Starting ViewDispatcher...\n");

    view_dispatcher_run(context->view_dispatcher);

    // Once we get here, we're cleaning up.
    FURI_LOG_I(TAG, "Cleaning up...\n");
    furi_record_close(RECORD_GUI);
    context_free(context);

    return 0;
}
