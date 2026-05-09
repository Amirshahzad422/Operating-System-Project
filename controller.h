#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <sys/types.h>

typedef enum {
    CTRL_CMD_NONE = 0,
    CTRL_CMD_EMERGENCY_TO_OTHER = 1,
    CTRL_CMD_STOP = 9
} ControllerCommandType;

typedef struct {
    int type;
    int vehicle_id;
} ControllerMessage;

typedef struct {
    pid_t pid_f10;
    pid_t pid_f11;
    int parent_cmd_f10;
    int parent_cmd_f11;
} ControllerSystem;

int controllers_start(ControllerSystem *sys);
void controllers_notify_emergency(ControllerSystem *sys, int from_f10, int vehicle_id);
void controllers_stop(ControllerSystem *sys);

#endif
