#include "traffic.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int traffic_init(IntersectionState *s, const char *name) {
    memset(s, 0, sizeof(*s));
    snprintf(s->name, sizeof(s->name), "%s", name);
    s->active_group = GROUP_NONE;
    if (pthread_mutex_init(&s->lock, NULL) != 0) {
        return -1;
    }
    if (pthread_cond_init(&s->cv, NULL) != 0) {
        pthread_mutex_destroy(&s->lock);
        return -1;
    }
    return 0;
}

void traffic_destroy(IntersectionState *s) {
    pthread_cond_destroy(&s->cv);
    pthread_mutex_destroy(&s->lock);
}

MovementGroup movement_group_for_turn(TurnDirection turn, int vehicle_id) {
    if (turn == TURN_LEFT) {
        return GROUP_LEFT_ONLY;
    }
    return (vehicle_id % 2 == 0) ? GROUP_NS_FLOW : GROUP_EW_FLOW;
}

void traffic_enter(IntersectionState *s, const Vehicle *v) {
    MovementGroup g = movement_group_for_turn(v->turn, v->id);

    pthread_mutex_lock(&s->lock);
    if (v->priority == PRIORITY_MEDIUM) {
        s->waiting_medium++;
    } else if (v->priority == PRIORITY_NORMAL) {
        s->waiting_normal++;
    }

    while (1) {
        int emergency_block = s->emergency_active > 0 && v->priority != PRIORITY_EMERGENCY;
        int medium_block = (v->priority == PRIORITY_NORMAL && s->waiting_medium > 0);
        int group_block = (s->active_group != GROUP_NONE && s->active_group != g);
        if (!emergency_block && !medium_block && !group_block) {
            break;
        }
        pthread_cond_wait(&s->cv, &s->lock);
    }

    if (v->priority == PRIORITY_MEDIUM && s->waiting_medium > 0) {
        s->waiting_medium--;
    } else if (v->priority == PRIORITY_NORMAL && s->waiting_normal > 0) {
        s->waiting_normal--;
    }

    if (s->active_group == GROUP_NONE) {
        s->active_group = g;
    }
    s->active_count++;
    char details[192];
    snprintf(details, sizeof(details),
             "id=%d type=%s tr=%s pr=%s active=%d",
             v->id, vehicle_type_short(v->type), turn_short(v->turn), priority_short(v->priority), s->active_count);
    log_line("TRAFFIC", s->name, "enter", details);
    pthread_mutex_unlock(&s->lock);
}

void traffic_leave(IntersectionState *s, const Vehicle *v) {
    pthread_mutex_lock(&s->lock);
    if (s->active_count > 0) {
        s->active_count--;
    }
    if (s->active_count == 0) {
        s->active_group = GROUP_NONE;
    }
    char details[192];
    snprintf(details, sizeof(details), "id=%d type=%s active=%d",
             v->id, vehicle_type_short(v->type), s->active_count);
    log_line("TRAFFIC", s->name, "leave", details);
    pthread_cond_broadcast(&s->cv);
    pthread_mutex_unlock(&s->lock);
}

void traffic_set_emergency_preempt(IntersectionState *s, int enabled) {
    pthread_mutex_lock(&s->lock);
    if (enabled) {
        s->emergency_active++;
    } else if (s->emergency_active > 0) {
        s->emergency_active--;
    }
    s->emergency_preempt = (s->emergency_active > 0);
    pthread_cond_broadcast(&s->cv);
    pthread_mutex_unlock(&s->lock);
}
