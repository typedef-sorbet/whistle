#pragma once
#ifndef TRANSFER_H
#define TRANSFER_H

#include <gui/scene_manager.h>
#include <helpers/subghz_worker.h>

void transfer_on_enter(void*);
bool transfer_on_event(void*, SceneManagerEvent);
void transfer_on_exit(void*);

#endif // TRANSFER_H
