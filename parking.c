#include "parking.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __APPLE__
static int sim_sem_init(SimSemaphore *s, int initial) {
    s->value = initial;
    if (pthread_mutex_init(&s->lock, NULL) != 0) {
        return -1;
    }
    if (pthread_cond_init(&s->cv, NULL) != 0) {
        pthread_mutex_destroy(&s->lock);
        return -1;
    }
    return 0;
}

static void sim_sem_destroy(SimSemaphore *s) {
    pthread_cond_destroy(&s->cv);
    pthread_mutex_destroy(&s->lock);
}

static int sim_sem_trywait(SimSemaphore *s) {
    int ok = 0;
    pthread_mutex_lock(&s->lock);
    if (s->value > 0) {
        s->value--;
        ok = 1;
    }
    pthread_mutex_unlock(&s->lock);
    return ok;
}

static void sim_sem_post(SimSemaphore *s) {
    pthread_mutex_lock(&s->lock);
    s->value++;
    pthread_cond_signal(&s->cv);
    pthread_mutex_unlock(&s->lock);
}
#else
static int sim_sem_init(SimSemaphore *s, int initial) {
    return sem_init(s, 0, (unsigned int) initial);
}

static void sim_sem_destroy(SimSemaphore *s) {
    sem_destroy(s);
}

static int sim_sem_trywait(SimSemaphore *s) {
    return sem_trywait(s) == 0;
}

static void sim_sem_post(SimSemaphore *s) {
    sem_post(s);
}
#endif

int parking_init(ParkingLot *lot, const char *name, int spots, int wait_capacity) {
    memset(lot, 0, sizeof(*lot));
    snprintf(lot->name, sizeof(lot->name), "%s", name);
    lot->total_spots = spots;
    lot->wait_capacity = wait_capacity;

    if (sim_sem_init(&lot->spots_sem, spots) != 0) {
        return -1;
    }
    if (sim_sem_init(&lot->wait_sem, wait_capacity) != 0) {
        sim_sem_destroy(&lot->spots_sem);
        return -1;
    }
    if (pthread_mutex_init(&lot->lock, NULL) != 0) {
        sim_sem_destroy(&lot->spots_sem);
        sim_sem_destroy(&lot->wait_sem);
        return -1;
    }
    return 0;
}

void parking_destroy(ParkingLot *lot) {
    pthread_mutex_destroy(&lot->lock);
    sim_sem_destroy(&lot->spots_sem);
    sim_sem_destroy(&lot->wait_sem);
}

int parking_try_enter(ParkingLot *lot, int vehicle_id) {
    if (!sim_sem_trywait(&lot->wait_sem)) {
        char details[160];
        snprintf(details, sizeof(details), "id=%d res=reject why=queue-full", vehicle_id);
        log_line("PARKING", lot->name, "queue-check", details);
        return 0;
    }

    pthread_mutex_lock(&lot->lock);
    lot->waiting_now++;
    pthread_mutex_unlock(&lot->lock);

    int got_spot = 0;
    for (int i = 0; i < 10; i++) {
        if (sim_sem_trywait(&lot->spots_sem)) {
            got_spot = 1;
            break;
        }
        usleep(70000);
    }

    pthread_mutex_lock(&lot->lock);
    lot->waiting_now--;
    if (got_spot) {
        lot->parked_now++;
        char details[160];
        snprintf(details, sizeof(details), "id=%d res=parked p=%d w=%d",
                 vehicle_id, lot->parked_now, lot->waiting_now);
        log_line("PARKING", lot->name, "enter", details);
    } else {
        char details[160];
        snprintf(details, sizeof(details), "id=%d res=left why=no-spot", vehicle_id);
        log_line("PARKING", lot->name, "enter", details);
    }
    pthread_mutex_unlock(&lot->lock);

    sim_sem_post(&lot->wait_sem);
    return got_spot;
}

void parking_leave(ParkingLot *lot, int vehicle_id) {
    pthread_mutex_lock(&lot->lock);
    if (lot->parked_now > 0) {
        lot->parked_now--;
    }
    char details[160];
    snprintf(details, sizeof(details), "id=%d res=exit p=%d w=%d",
             vehicle_id, lot->parked_now, lot->waiting_now);
    log_line("PARKING", lot->name, "leave", details);
    pthread_mutex_unlock(&lot->lock);

    sim_sem_post(&lot->spots_sem);
}
