#include <gui/scene_manager.h>

#pragma once
#ifndef MAINMENU_H
#define MAINMENU_H

void mainmenu_on_enter(void*);
bool mainmenu_on_event(void*, SceneManagerEvent);
void mainmenu_on_exit(void*);

typedef enum {
    OPTION_MainMenu_SendFile,
    OPTION_MainMenu_ReceiveFile,
    OPTION_MainMenu_count
} mainmenu_option;

typedef enum {
    EVENT_MainMenu_SendFileSelected,
    EVENT_MainMenu_ReceiveFileSelected,
    EVENT_MainMenu_count
} mainmenu_event;

#endif // MAINMENU_H