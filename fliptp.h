#include <furi/core/string.h>
#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/file_browser.h>
#include <subghz/subghz_setting.h>

#pragma once
#ifndef FLIPTP_H
#define FLIPTP_H

#define TAG               "fliptp"
#define TRACE             FURI_LOG_T(TAG, __FUNCTION__)
// Note: using this is probably bad practice
#define CONTEXT_CAST(CTX) fliptp_context* context = (fliptp_context*)CTX

typedef enum {
    SCENE_MainMenu,
    SCENE_Options,
    SCENE_FileBrowser,
    SCENE_count
} fliptp_scene_id;

typedef enum {
    VIEW_MainMenu,
    VIEW_Options,
    VIEW_FileBrowser,
    VIEW_count
} fliptp_view_id;

typedef enum {
    MODE_Sending,
    MODE_Receiving,
    MODE_count
} fliptp_mode;

typedef struct {
    // GUI stuff
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Menu* menu;
    VariableItemList* option_list;
    FileBrowser* file_browser;

    // Configuration
    fliptp_mode mode;
    bool encryption;
    uint32_t frequency;
    FuriString* selected_file;
    SubGhzSetting* subghz_settings;

    // ...
} fliptp_context;

#endif // FLIPTP_H
