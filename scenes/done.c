#include <scenes/done.h>

#include <whistle.h>

void done_on_enter(void *_context) {
    TRACE;

    whistle_context *context = (whistle_context*)_context;

    text_box_reset(context->text_box);
    text_box_set_text(context->text_box, "Done!");

    furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);
    furi_timer_start(context->timer, 2500 /*ms*/);

    // ...
    view_dispatcher_switch_to_view(context->view_dispatcher, VIEW_Done);
}


bool done_on_event(void *_context, SceneManagerEvent event) {
    TRACE;
    (void)_context;
    (void)event;
    return true;
}

void done_on_exit(void *_context) {
    (void)_context;
}

void done_on_timer_complete(void *_context) {
    whistle_context *context = (whistle_context*)_context;
    scene_manager_next_scene(context->scene_manager, SCENE_MainMenu);
}