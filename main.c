#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "controller.h"
#ifdef USE_SDL2
#include "gui.h"
#endif
#include "logger.h"
#include "parking.h"
#include "traffic.h"
#include "types.h"

#define DEFAULT_VEHICLES 15
#define MAX_VEHICLES 200
#define PARKING_SPOTS 10
#define PARKING_WAIT_CAP 5

typedef struct {
    IntersectionState traffic[2];
    ParkingLot parking[2];
    ControllerSystem controllers;
} Simulation;

typedef struct {
    int spawn_min_ms;
    int spawn_jitter_ms;
    int cross_min_ms;
    int cross_jitter_ms;
    int park_min_ms;
    int park_jitter_ms;
} SimTiming;

static volatile sig_atomic_t g_stop = 0;

#ifdef USE_SDL2
/* Counts controllers_notify_emergency (parent → controller processes) for GUI. */
static volatile sig_atomic_t g_gui_emergency_pipe_count = 0;

typedef struct {
    pthread_t *threads;
    int n;
} VehicleReaperArg;

static void *vehicle_reaper_main(void *p) {
    VehicleReaperArg *a = (VehicleReaperArg *) p;
    for (int i = 0; i < a->n; i++) {
        pthread_join(a->threads[i], NULL);
    }
    return NULL;
}

static void gui_sleep_us_chunked(
        GuiContext *gui,
        Simulation *sim,
        volatile sig_atomic_t *stop,
        useconds_t total_us,
        int spawned,
        int target) {
    useconds_t left = total_us;
    while (left > 0 && !*stop) {
        useconds_t step = left > 16000U ? 16000U : left;
        gui_process_events(stop);
        if (*stop) {
            break;
        }
        int ipc = (int) g_gui_emergency_pipe_count;
        gui_render(gui, sim->traffic, sim->parking, spawned, target, ipc);
        usleep(step);
        left -= step;
    }
}

static void gui_run_until_close(GuiContext *gui, Simulation *sim, int spawned, int target) {
    while (!g_stop) {
        gui_process_events(&g_stop);
        if (g_stop) {
            break;
        }
        int ipc = (int) g_gui_emergency_pipe_count;
        gui_render(gui, sim->traffic, sim->parking, spawned, target, ipc);
        usleep(16000);
    }
}
#endif

static void on_sigint(int signo) {
    (void) signo;
    g_stop = 1;
}

static void log_state_snapshot(Simulation *sim, int spawned, int target) {
    int f10_active, f11_active, f10_emg, f11_emg;
    int f10_park, f10_wait, f11_park, f11_wait;

    pthread_mutex_lock(&sim->traffic[INTERSECTION_F10].lock);
    f10_active = sim->traffic[INTERSECTION_F10].active_count;
    f10_emg = sim->traffic[INTERSECTION_F10].emergency_preempt;
    pthread_mutex_unlock(&sim->traffic[INTERSECTION_F10].lock);

    pthread_mutex_lock(&sim->traffic[INTERSECTION_F11].lock);
    f11_active = sim->traffic[INTERSECTION_F11].active_count;
    f11_emg = sim->traffic[INTERSECTION_F11].emergency_preempt;
    pthread_mutex_unlock(&sim->traffic[INTERSECTION_F11].lock);

    pthread_mutex_lock(&sim->parking[INTERSECTION_F10].lock);
    f10_park = sim->parking[INTERSECTION_F10].parked_now;
    f10_wait = sim->parking[INTERSECTION_F10].waiting_now;
    pthread_mutex_unlock(&sim->parking[INTERSECTION_F10].lock);

    pthread_mutex_lock(&sim->parking[INTERSECTION_F11].lock);
    f11_park = sim->parking[INTERSECTION_F11].parked_now;
    f11_wait = sim->parking[INTERSECTION_F11].waiting_now;
    pthread_mutex_unlock(&sim->parking[INTERSECTION_F11].lock);

    log_linef("STATE", "-", "snapshot",
              "prg=%d/%d F10(a%d p%d w%d e%d) F11(a%d p%d w%d e%d)",
              spawned, target, f10_active, f10_park, f10_wait, f10_emg,
              f11_active, f11_park, f11_wait, f11_emg);
}

static PriorityLevel priority_of(VehicleType t) {
    if (t == VEH_AMBULANCE || t == VEH_FIRETRUCK) {
        return PRIORITY_EMERGENCY;
    }
    if (t == VEH_BUS) {
        return PRIORITY_MEDIUM;
    }
    return PRIORITY_NORMAL;
}

static VehicleType random_vehicle_type(void) {
    int x = rand() % VEH_TYPE_COUNT;
    return (VehicleType) x;
}

static TurnDirection random_turn(void) {
    return (TurnDirection) (rand() % TURN_COUNT);
}

static int vehicle_can_park(VehicleType t) {
    return t == VEH_CAR || t == VEH_BIKE || t == VEH_TRACTOR || t == VEH_BUS;
}

typedef struct {
    Simulation *sim;
    Vehicle v;
    const SimTiming *timing;
} VehicleThreadArg;

static void *vehicle_thread_main(void *arg_ptr) {
    VehicleThreadArg *arg = (VehicleThreadArg *) arg_ptr;
    Simulation *sim = arg->sim;
    const SimTiming *timing = arg->timing;
    Vehicle v = arg->v;
    free(arg);

    char details[256];
    snprintf(details, sizeof(details),
             "id=%d type=%s route=%s>%s tr=%s pr=%s pk=%c",
             v.id, vehicle_type_short(v.type), intersection_str(v.origin), intersection_str(v.target),
             turn_short(v.turn), priority_short(v.priority), v.wants_parking ? 'Y' : 'N');
    log_line("VEHICLE", intersection_str(v.origin), "arrive", details);

    if (v.wants_parking) {
        ParkingLot *lot = &sim->parking[v.origin];
        int parked = parking_try_enter(lot, v.id);
        if (!parked) {
            snprintf(details, sizeof(details), "id=%d action=skip reason=park-unavail", v.id);
            log_line("VEHICLE", intersection_str(v.origin), "decision", details);
            return NULL;
        }
        usleep((useconds_t) ((rand() % timing->park_jitter_ms + timing->park_min_ms) * 1000));
        parking_leave(lot, v.id);
    }

    IntersectionState *state = &sim->traffic[v.origin];
    if (v.priority == PRIORITY_EMERGENCY) {
        traffic_set_emergency_preempt(state, 1);
        int from_f10 = (v.origin == INTERSECTION_F10);
        controllers_notify_emergency(&sim->controllers, from_f10, v.id);
#ifdef USE_SDL2
        g_gui_emergency_pipe_count++;
#endif
    }

    traffic_enter(state, &v);
    usleep((useconds_t) ((rand() % timing->cross_jitter_ms + timing->cross_min_ms) * 1000));
    traffic_leave(state, &v);

    if (v.priority == PRIORITY_EMERGENCY) {
        traffic_set_emergency_preempt(state, 0);
    }
    return NULL;
}

static int sim_init(Simulation *sim) {
    if (traffic_init(&sim->traffic[INTERSECTION_F10], "F10") != 0) {
        return -1;
    }
    if (traffic_init(&sim->traffic[INTERSECTION_F11], "F11") != 0) {
        traffic_destroy(&sim->traffic[INTERSECTION_F10]);
        return -1;
    }
    if (parking_init(&sim->parking[INTERSECTION_F10], "F10", PARKING_SPOTS, PARKING_WAIT_CAP) != 0) {
        return -1;
    }
    if (parking_init(&sim->parking[INTERSECTION_F11], "F11", PARKING_SPOTS, PARKING_WAIT_CAP) != 0) {
        parking_destroy(&sim->parking[INTERSECTION_F10]);
        return -1;
    }
    if (controllers_start(&sim->controllers) != 0) {
        parking_destroy(&sim->parking[INTERSECTION_F10]);
        parking_destroy(&sim->parking[INTERSECTION_F11]);
        return -1;
    }
    return 0;
}

static void sim_destroy(Simulation *sim) {
    controllers_stop(&sim->controllers);
    parking_destroy(&sim->parking[INTERSECTION_F10]);
    parking_destroy(&sim->parking[INTERSECTION_F11]);
    traffic_destroy(&sim->traffic[INTERSECTION_F10]);
    traffic_destroy(&sim->traffic[INTERSECTION_F11]);
}

int main(int argc, char **argv) {
    int target_vehicles = DEFAULT_VEHICLES;
    SimTiming timing = {
        .spawn_min_ms = 80,
        .spawn_jitter_ms = 220,
        .cross_min_ms = 250,
        .cross_jitter_ms = 500,
        .park_min_ms = 200,
        .park_jitter_ms = 400
    };
#ifdef USE_SDL2
    int gui_mode = 0;
#endif

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--gui") == 0) {
#ifdef USE_SDL2
            gui_mode = 1;
            continue;
#else
            fprintf(stderr, "This binary was built without SDL2 support.\n");
            return 1;
#endif
        }
        target_vehicles = atoi(argv[i]);
        if (target_vehicles <= 0 || target_vehicles > MAX_VEHICLES) {
            fprintf(stderr, "Invalid argument '%s'. Use vehicle count 1..%d and optional --gui\n",
                    argv[i], MAX_VEHICLES);
            return 1;
        }
    }

    srand((unsigned int) time(NULL));
    signal(SIGINT, on_sigint);

    Simulation sim;
    if (sim_init(&sim) != 0) {
        fprintf(stderr, "Failed to initialize simulation: %s\n", strerror(errno));
        return 1;
    }

    log_line("SYSTEM", "-", "startup", "------------------------------------------------------------");
    {
        char details[192];
        snprintf(details, sizeof(details), "veh=%d park_spots=%d park_wait=%d",
                 target_vehicles, PARKING_SPOTS, PARKING_WAIT_CAP);
        log_line("SYSTEM", "-", "config", details);
    }
    log_line("SYSTEM", "-", "legend", "priority: Emergency > Medium(Bus) > Normal");
    log_line("SYSTEM", "-", "legend", "pr: E/M/N tr: S/L/R");
    log_line("SYSTEM", "-", "legend", "fmt: [time pid] COMP/SCOPE event :: key=val");
    log_state_snapshot(&sim, 0, target_vehicles);

#ifdef USE_SDL2
    GuiContext gui;
    if (gui_mode && gui_init(&gui) != 0) {
        sim_destroy(&sim);
        return 1;
    }
    if (gui_mode) {
        /* Long GUI run: easy to follow (full traffic + parking + emergencies). */
        timing.spawn_min_ms = 1400;
        timing.spawn_jitter_ms = 900;
        timing.cross_min_ms = 2200;
        timing.cross_jitter_ms = 1400;
        timing.park_min_ms = 1400;
        timing.park_jitter_ms = 1100;
    }
#endif

    pthread_t threads[MAX_VEHICLES];
    int spawned = 0;

#ifdef USE_SDL2
    if (gui_mode) {
        gui_process_events(&g_stop);
        gui_render(&gui, sim.traffic, sim.parking, 0, target_vehicles, (int) g_gui_emergency_pipe_count);
    }
#endif

    for (int i = 0; i < target_vehicles && !g_stop; i++) {
        VehicleThreadArg *arg = (VehicleThreadArg *) malloc(sizeof(*arg));
        if (!arg) {
            g_stop = 1;
            break;
        }
        Vehicle v;
        v.id = i + 1;
        v.type = random_vehicle_type();
        v.origin = (rand() % 2 == 0) ? INTERSECTION_F10 : INTERSECTION_F11;
        v.target = (v.origin == INTERSECTION_F10) ? INTERSECTION_F11 : INTERSECTION_F10;
        v.turn = random_turn();
        v.priority = priority_of(v.type);
        v.arrival_time = time(NULL);
        v.wants_parking = vehicle_can_park(v.type) ? (rand() % 2) : 0;

        arg->sim = &sim;
        arg->v = v;
        arg->timing = &timing;

        if (pthread_create(&threads[spawned], NULL, vehicle_thread_main, arg) != 0) {
            free(arg);
            g_stop = 1;
            break;
        }
        spawned++;
        if (spawned % 5 == 0 || spawned == target_vehicles) {
            log_state_snapshot(&sim, spawned, target_vehicles);
        }
#ifdef USE_SDL2
        if (gui_mode) {
            useconds_t delay_us =
                    (useconds_t) ((rand() % timing.spawn_jitter_ms + timing.spawn_min_ms) * 1000);
            gui_sleep_us_chunked(&gui, &sim, &g_stop, delay_us, spawned, target_vehicles);
            continue;
        }
#endif
        usleep((useconds_t) ((rand() % timing.spawn_jitter_ms + timing.spawn_min_ms) * 1000));
    }

#ifdef USE_SDL2
    pthread_t reaper_tid = 0;
    int reaper_ok = 0;
    VehicleReaperArg reap = {.threads = threads, .n = spawned};
#endif

#ifdef USE_SDL2
    if (gui_mode && spawned > 0) {
        reaper_ok = (pthread_create(&reaper_tid, NULL, vehicle_reaper_main, &reap) == 0);
    }
    if (!(gui_mode && reaper_ok)) {
#else
    {
#endif
        for (int i = 0; i < spawned; i++) {
            pthread_join(threads[i], NULL);
        }
    }

#ifdef USE_SDL2
    if (gui_mode) {
        gui_run_until_close(&gui, &sim, spawned, target_vehicles);
        if (reaper_ok) {
            pthread_join(reaper_tid, NULL);
        }
        gui_shutdown(&gui);
    }
#endif

    {
        char details[128];
        snprintf(details, sizeof(details), "spawned=%d stop=%d", spawned, g_stop ? 1 : 0);
        log_line("SYSTEM", "-", "completed", details);
        log_state_snapshot(&sim, spawned, target_vehicles);
        log_line("SYSTEM", "-", "shutdown", "------------------------------------------------------------");
    }

    sim_destroy(&sim);
    return 0;
}
