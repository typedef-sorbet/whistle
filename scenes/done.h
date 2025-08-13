#include <gui/scene_manager.h>

#pragma once
#ifndef DONE_H
#define DONE_H

void done_on_enter(void*);
bool done_on_event(void*, SceneManagerEvent);
void done_on_exit(void*);

void done_on_timer_complete(void*);

#endif