#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <pthread.h>

#include "types.h"

typedef enum {
    GROUP_NONE = -1,
    GROUP_NS_FLOW = 0,
    GROUP_EW_FLOW = 1,
    GROUP_LEFT_ONLY = 2
} MovementGroup;

typedef struct {
    char name[8];
    pthread_mutex_t lock;
    pthread_cond_t cv;
    int active_count;
    MovementGroup active_group;
    int emergency_preempt;
    int emergency_active;
    int waiting_medium;
    int waiting_normal;
} IntersectionState;

int traffic_init(IntersectionState *s, const char *name);
void traffic_destroy(IntersectionState *s);
MovementGroup movement_group_for_turn(TurnDirection turn, int vehicle_id);
void traffic_enter(IntersectionState *s, const Vehicle *v);
void traffic_leave(IntersectionState *s, const Vehicle *v);
void traffic_set_emergency_preempt(IntersectionState *s, int enabled);

#endif
