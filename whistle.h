#pragma once
#ifndef FLIPTP_H
#define FLIPTP_H

#include <furi/core/string.h>
#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/file_browser.h>
#include <gui/modules/loading.h>
#include <subghz/subghz_setting.h>

#include <helpers/subghz_worker.h>

#include <gui/icon_i.h>
#include <enums.h>
#include "whistle_icons.h"

#define TAG               "whistle"
#define TRACE             FURI_LOG_T(TAG, __FUNCTION__)
// Note: using this is probably bad practice
#define CONTEXT_CAST(CTX) whistle_context* context = (whistle_context*)CTX

typedef struct {
    // GUI stuff
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    // View modules
    Menu* menu;
    VariableItemList* option_list;
    FileBrowser* file_browser;
    Loading* loading_screen;

    // Configuration
    whistle_mode mode;
    bool encryption;
    uint32_t frequency;
    FuriString* selected_file;
    SubGhzSetting* subghz_settings;

    // Thread worker(s)
    subghz_worker* subghz_worker;

    // ...
} whistle_context;

#endif // FLIPTP_H
