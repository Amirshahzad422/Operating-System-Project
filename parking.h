#ifndef PARKING_H
#define PARKING_H

#include <pthread.h>
#ifndef __APPLE__
#include <semaphore.h>
#endif

#ifdef __APPLE__
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cv;
    int value;
} SimSemaphore;
#else
typedef sem_t SimSemaphore;
#endif

typedef struct {
    char name[8];
    SimSemaphore spots_sem;
    SimSemaphore wait_sem;
    int total_spots;
    int wait_capacity;
    pthread_mutex_t lock;
    int parked_now;
    int waiting_now;
} ParkingLot;

int parking_init(ParkingLot *lot, const char *name, int spots, int wait_capacity);
void parking_destroy(ParkingLot *lot);
int parking_try_enter(ParkingLot *lot, int vehicle_id);
void parking_leave(ParkingLot *lot, int vehicle_id);

#endif
