#include <gui/scene_manager.h>
#include <helpers/subghz_worker.h>

#pragma once
#ifndef TRANSFER_H
#define TRANSFER_H

void transfer_on_enter(void*);
bool transfer_on_event(void*, SceneManagerEvent);
void transfer_on_exit(void*);

void transfer_on_thread_state_change(FuriThread *thread, FuriThreadState state, void *context);

#endif // TRANSFER_H
