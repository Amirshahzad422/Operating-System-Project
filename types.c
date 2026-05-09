#include "types.h"

const char *vehicle_type_str(VehicleType type) {
    static const char *names[] = {
        "Ambulance", "Firetruck", "Bus", "Car", "Bike", "Tractor"
    };
    if (type < 0 || type >= VEH_TYPE_COUNT) {
        return "UnknownVehicle";
    }
    return names[type];
}

const char *vehicle_type_short(VehicleType type) {
    static const char *names[] = {"AMB", "FIR", "BUS", "CAR", "BIK", "TRC"};
    if (type < 0 || type >= VEH_TYPE_COUNT) {
        return "UNK";
    }
    return names[type];
}

const char *intersection_str(IntersectionId id) {
    return id == INTERSECTION_F10 ? "F10" : "F11";
}

const char *turn_str(TurnDirection turn) {
    static const char *names[] = {"Straight", "Left", "Right"};
    if (turn < 0 || turn >= TURN_COUNT) {
        return "UnknownTurn";
    }
    return names[turn];
}

const char *turn_short(TurnDirection turn) {
    static const char *names[] = {"S", "L", "R"};
    if (turn < 0 || turn >= TURN_COUNT) {
        return "?";
    }
    return names[turn];
}

const char *priority_str(PriorityLevel p) {
    switch (p) {
        case PRIORITY_EMERGENCY:
            return "Emergency";
        case PRIORITY_MEDIUM:
            return "Medium";
        default:
            return "Normal";
    }
}

const char *priority_short(PriorityLevel p) {
    switch (p) {
        case PRIORITY_EMERGENCY:
            return "E";
        case PRIORITY_MEDIUM:
            return "M";
        default:
            return "N";
    }
}
