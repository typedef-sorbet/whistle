#include <scenes/options.h>
#include <fliptp.h>

static const char* const yesno_text[2] = {"No", "Yes"};

// I have no fucking idea what this does.
uint8_t options_next_frequency(const uint32_t value, void* _context) {
    TRACE;

    furi_assert(_context);

    fliptp_context* context = (fliptp_context*)_context;
    uint8_t index = 0;

    for(uint8_t i = 0; i < subghz_setting_get_frequency_count(context->subghz_settings); ++i) {
        if(value == subghz_setting_get_frequency(context->subghz_settings, i)) {
            index = i;
            break;
        } else {
            index = subghz_setting_get_frequency_default_index(context->subghz_settings);
        }
    }

    return index;
}

void options_change_frequency_callback(VariableItem* item) {
    fliptp_context* context = (fliptp_context*)variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    char text_buf[10] = {0};
    snprintf(
        text_buf,
        sizeof(text_buf),
        "%lu.%02lu",
        subghz_setting_get_frequency(context->subghz_settings, index) / 1000000,
        (subghz_setting_get_frequency(context->subghz_settings, index) % 1000000) / 10000);
    variable_item_set_current_value_text(item, text_buf);
    context->frequency = subghz_setting_get_frequency(context->subghz_settings, index);
}

void options_change_encryption_callback(VariableItem* item) {
    fliptp_context* context = (fliptp_context*)variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, yesno_text[index]);

    context->encryption = (index == 1);
}

void options_list_enter_callback(void* _context, uint32_t idx) {
    TRACE;
    furi_assert(_context);

    FURI_LOG_D(TAG, "options_list_enter_callback with index %ld", idx);

    fliptp_context* context = (fliptp_context*)_context;

    if(idx == INDEX_Options_File) {
        scene_manager_handle_custom_event(
            context->scene_manager, EVENT_Options_ChooseFileSelected);
    }
}

void options_on_enter(void* _context) {
    TRACE;

    fliptp_context* context = (fliptp_context*)_context;

    // TODO need to restore settings on entry

    // Stolen mostly from the weather station app
    VariableItem* item = NULL;
    uint8_t value_index;

    // Frequency

    item = variable_item_list_add(
        context->option_list,
        "Frequency",
        subghz_setting_get_frequency_count(context->subghz_settings),
        options_change_frequency_callback,
        context);
    value_index = options_next_frequency(context->frequency, context);
    // ???
    scene_manager_set_scene_state(context->scene_manager, SCENE_Options, (uint32_t)item);
    variable_item_set_current_value_index(item, value_index);
    char text_buf[10] = {0};
    snprintf(
        text_buf,
        sizeof(text_buf),
        "%lu.%02lu",
        subghz_setting_get_frequency(context->subghz_settings, value_index) / 1000000,
        (subghz_setting_get_frequency(context->subghz_settings, value_index) % 1000000) / 10000);
    variable_item_set_current_value_text(item, text_buf);

    context->frequency = subghz_setting_get_frequency(context->subghz_settings, value_index);

    // Encryption
    item = variable_item_list_add(
        context->option_list, "Use encryption?", 2, options_change_encryption_callback, context);
    variable_item_set_current_value_index(item, context->encryption ? 1 : 0);
    variable_item_set_current_value_text(item, context->encryption ? "Yes" : "No");

    // Choose file

    if(context->mode == MODE_Sending) {
        item = variable_item_list_add(context->option_list, "File", 1, NULL, context);
        variable_item_set_current_value_text(item, furi_string_get_cstr(context->selected_file));
        variable_item_list_set_enter_callback(
            context->option_list, options_list_enter_callback, context);
    }

    view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_Options);
}

bool options_on_event(void* _context, SceneManagerEvent event) {
    TRACE;

    fliptp_context* context = (fliptp_context*)_context;

    (void)context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom: {
        switch(event.event) {
        case EVENT_Options_ChooseFileSelected:
            scene_manager_next_scene(context->scene_manager, SCENE_FileBrowser);
            consumed = true;
            break;
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

void options_on_exit(void* _context) {
    TRACE;

    fliptp_context* context = (fliptp_context*)_context;

    variable_item_list_reset(context->option_list);
}
