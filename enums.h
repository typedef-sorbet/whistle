#pragma once
#ifndef WHISTLE_ENUMS
#define WHISTLE_ENUMS

#define TAG "whistle"

typedef enum {
    SCENE_MainMenu,
    SCENE_Options,
    SCENE_FileBrowser,
    SCENE_Transfer,
    SCENE_count
} whistle_scene_id;

typedef enum {
    VIEW_MainMenu,
    VIEW_Options,
    VIEW_FileBrowser,
    VIEW_Transfer,
    VIEW_count
} whistle_view_id;

typedef enum {
    MODE_Sending,
    MODE_Receiving,
    MODE_count
} whistle_mode;

#endif // WHISTLE_ENUMS