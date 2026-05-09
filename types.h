#ifndef TYPES_H
#define TYPES_H

#include <time.h>

typedef enum {
    INTERSECTION_F10 = 0,
    INTERSECTION_F11 = 1
} IntersectionId;

typedef enum {
    VEH_AMBULANCE = 0,
    VEH_FIRETRUCK,
    VEH_BUS,
    VEH_CAR,
    VEH_BIKE,
    VEH_TRACTOR,
    VEH_TYPE_COUNT
} VehicleType;

typedef enum {
    TURN_STRAIGHT = 0,
    TURN_LEFT,
    TURN_RIGHT,
    TURN_COUNT
} TurnDirection;

typedef enum {
    PRIORITY_EMERGENCY = 0,
    PRIORITY_MEDIUM = 1,
    PRIORITY_NORMAL = 2
} PriorityLevel;

typedef struct {
    int id;
    VehicleType type;
    IntersectionId origin;
    IntersectionId target;
    TurnDirection turn;
    PriorityLevel priority;
    time_t arrival_time;
    int wants_parking;
} Vehicle;

const char *vehicle_type_str(VehicleType type);
const char *vehicle_type_short(VehicleType type);
const char *intersection_str(IntersectionId id);
const char *turn_str(TurnDirection turn);
const char *turn_short(TurnDirection turn);
const char *priority_str(PriorityLevel p);
const char *priority_short(PriorityLevel p);

#endif
