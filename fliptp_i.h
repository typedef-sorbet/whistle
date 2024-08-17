#include <fliptp.h>

// Include all scenes/*.h here
#include <scenes/mainmenu.h>
#include <scenes/options.h>
#include <scenes/file_browser.h>

#pragma once
#ifndef FLIPTP_I_H
#define FLIPTP_I_H

// These should be ordered by the scene enum
void (*const fliptp_scene_on_enter_handlers[])(
    void*) = {mainmenu_on_enter, options_on_enter, file_browser_on_enter};
bool (*const fliptp_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    mainmenu_on_event,
    options_on_event,
    file_browser_on_event};
void (*const fliptp_scene_on_exit_handlers[])(
    void*) = {mainmenu_on_exit, options_on_exit, file_browser_on_exit};

const SceneManagerHandlers fliptp_scene_manager_handlers = {
    .on_enter_handlers = fliptp_scene_on_enter_handlers,
    .on_event_handlers = fliptp_scene_on_event_handlers,
    .on_exit_handlers = fliptp_scene_on_exit_handlers,
    .scene_num = SCENE_count};

#endif // FLIPTP_I_H
