#include <gui/scene_manager.h>

#pragma once
#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

void file_browser_on_enter(void*);
bool file_browser_on_event(void*, SceneManagerEvent);
void file_browser_on_exit(void*);

#endif // FILE_BROWSER_H