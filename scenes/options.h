#include <gui/scene_manager.h>
#include <subghz/subghz_setting.h>

#pragma once
#ifndef OPTIONS_H
#define OPTIONS_H

// Variable option list index
typedef enum {
    INDEX_Options_Frequency,
    INDEX_Options_Encryption,
    INDEX_Options_File,
    INDEX_Options_Start,
    INDEX_Options_count
} options_list_index;

// Event enums
typedef enum {
    EVENT_Options_ChooseFileSelected,
    EVENT_Options_Start,
    EVENT_Options_count
} options_list_event;

void options_on_enter(void*);
bool options_on_event(void*, SceneManagerEvent);
void options_on_exit(void*);

#endif // OPTIONS_H
