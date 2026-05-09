#ifndef GUI_H
#define GUI_H

#include <signal.h>

#include "parking.h"
#include "traffic.h"

typedef struct {
    int initialized;
    void *window;
    void *renderer;
    void *font_regular;
    void *font_small;
} GuiContext;

int gui_init(GuiContext *ctx);
void gui_render(
        GuiContext *ctx,
        IntersectionState traffic[2],
        ParkingLot parking[2],
        int spawned,
        int target,
        int ipc_emergency_notifies);
int gui_process_events(volatile sig_atomic_t *stop_flag);
void gui_shutdown(GuiContext *ctx);

#endif
