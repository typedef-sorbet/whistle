#include <gui/scene_manager.h>
#include <helpers/subghz_worker.h>

#pragma once
#ifndef TRANSFER_H
#define TRANSFER_H

void transfer_on_enter(void*);
bool transfer_on_event(void*, SceneManagerEvent);
void transfer_on_exit(void*);

#endif // TRANSFER_H
