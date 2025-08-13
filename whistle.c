#include <whistle.h>
#include <whistle_i.h>

#include <scenes/done.h>

bool whistle_custom_event_cb(void* _context, uint32_t event) {
    TRACE;

    whistle_context* context = (whistle_context*)_context;
    return scene_manager_handle_custom_event(context->scene_manager, event);
}

bool whistle_custom_navigation_event_callback(void* _context) {
    TRACE;

    whistle_context* context = (whistle_context*)_context;

    return scene_manager_handle_back_event(context->scene_manager);
}

whistle_context* context_alloc() {
    TRACE;

    whistle_context* context = malloc(sizeof(whistle_context));

    if(NULL == context) {
        goto err_context;
    }

    context->scene_manager = scene_manager_alloc(&whistle_scene_manager_handlers, context);
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

    context->text_box = text_box_alloc();
    if (NULL == context->text_box) {
        goto err_textbox_alloc;
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

    context->loading_screen = loading_alloc();
    if(NULL == context->loading_screen) {
        goto err_loading_screen;
    }

    // Gui setup
    context->gui = furi_record_open(RECORD_GUI);
    if(NULL == context->gui) {
        goto err_gui;
    }

    // Timer setup
    context->timer = furi_timer_alloc(done_on_timer_complete, FuriTimerTypeOnce, context);
    if (NULL == context->timer) {
        goto err_timer;
    }

    // ViewDispatcher setup
    view_dispatcher_enable_queue(context->view_dispatcher);

    view_dispatcher_set_event_callback_context(context->view_dispatcher, context);

    view_dispatcher_set_custom_event_callback(context->view_dispatcher, whistle_custom_event_cb);

    view_dispatcher_set_navigation_event_callback(
        context->view_dispatcher, whistle_custom_navigation_event_callback);

    view_dispatcher_attach_to_gui(
        context->view_dispatcher, context->gui, ViewDispatcherTypeFullscreen);

    // Add views
    view_dispatcher_add_view(
        context->view_dispatcher, VIEW_MainMenu, menu_get_view(context->menu));
    view_dispatcher_add_view(
        context->view_dispatcher, VIEW_Options, variable_item_list_get_view(context->option_list));
    view_dispatcher_add_view(
        context->view_dispatcher, VIEW_FileBrowser, file_browser_get_view(context->file_browser));
    view_dispatcher_add_view(
        context->view_dispatcher, VIEW_Transfer, loading_get_view(context->loading_screen));
    view_dispatcher_add_view(context->view_dispatcher, VIEW_Done, text_box_get_view(context->text_box));

    // Set sane defaults for options
    context->encryption = false;
    context->frequency = 432000000;
    furi_string_reset(context->selected_file);

    subghz_setting_load(context->subghz_settings, EXT_PATH("subghz/assets/setting_user"));

    return context;

err_timer:
    furi_record_close(RECORD_GUI);
    free(context->gui);

err_gui:
    loading_free(context->loading_screen);

err_loading_screen:
    file_browser_free(context->file_browser);

err_file_browser:
    furi_string_free(context->selected_file);

err_string:
    subghz_setting_free(context->subghz_settings);

err_subghz:
    variable_item_list_free(context->option_list);

err_option_list:
    text_box_free(context->text_box);

err_textbox_alloc:
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

void context_free(whistle_context* context) {
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

int32_t whistle_main() {
    TRACE;

    whistle_context* context = context_alloc();

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
