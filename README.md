# OS Project Spring 2026 - Traffic + Parking Simulation

This implementation follows the project requirements:

- Two intersections (`F10`, `F11`)
- Vehicle concurrency with `pthread`
- Two traffic controllers as separate processes (`fork`)
- Controller-to-controller coordination via pipes
- Emergency priority preemption
- Parking lot module per intersection with dual semaphores
- Graceful shutdown with configurable vehicle count and `SIGINT`

## Build

```bash
make
```

### Build with SDL2 visualization

Install SDL2 first:

- Ubuntu:
  ```bash
  sudo apt update
  sudo apt install libsdl2-dev
  ```
- macOS:
  ```bash
  brew install sdl2
  ```

Then build GUI binary:

```bash
make os_project_sim_gui
```

## Run

```bash
./os_project_sim 15
```

You can replace `15` with any value in range `1..200`.

### Run GUI mode

```bash
./os_project_sim_gui 15 --gui
```

Or:

```bash
make run-gui
```

## Core Design

- `main.c`
  - Simulation initialization, vehicle spawning, joining, cleanup
  - SIGINT handling for graceful stop
- `controller.c/.h`
  - Spawns `F10` and `F11` controller processes
  - Parent sends emergency notifications
  - Controllers notify each other through pipes
- `traffic.c/.h`
  - Non-conflicting movement groups
  - Intersection entry/leave synchronization
  - Emergency preemption for normal traffic
- `parking.c/.h`
  - Parking spots semaphore (capacity = 10)
  - Bounded waiting queue semaphore
  - Vehicles never block intersection while waiting
- `types.c/.h`
  - Vehicle metadata and display helpers
- `gui.c/.h` (optional)
  - SDL2-based visualizer for intersections and parking load
  - Keeps core simulation logic unchanged

## Requirement Mapping

1. **Multi-process architecture**: `fork()` creates two controller processes.
2. **IPC**: controllers exchange emergency alerts via pipes.
3. **Concurrency**: each vehicle is a separate `pthread`.
4. **Metadata**: every vehicle includes id/type/origin/target/direction/priority/arrival_time.
5. **Parking logic**:
   - 10 parking spots (`sem_t spots_sem`)
   - bounded wait queue (`sem_t wait_sem`)
   - parking vehicles skip crossing if parking is unavailable (no intersection blocking)
6. **Priority**:
   - Emergency: ambulance, firetruck
   - Medium: bus
   - Normal: car, bike, tractor
7. **Safety**: only compatible movement groups are allowed concurrently.
8. **Graceful shutdown**:
   - stop on configured vehicle limit or `Ctrl+C`
   - join all threads
   - stop controllers
   - destroy semaphores/mutexes/condition variables and close pipes

## Demo Tips (for viva)

- Run with `./os_project_sim 25` to show richer concurrency.
- Explain:
  - Why process-level controllers are separate from vehicle threads
  - How emergency preemption is enforced
  - How dual semaphores prevent unbounded parking waits
  - Why vehicles avoid blocking intersection when parking is unavailable
