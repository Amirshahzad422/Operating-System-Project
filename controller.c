#include "controller.h"
#include "logger.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

static int send_msg(int fd, ControllerMessage m) {
    ssize_t n = write(fd, &m, sizeof(m));
    return n == (ssize_t) sizeof(m) ? 0 : -1;
}

static int recv_msg(int fd, ControllerMessage *m) {
    ssize_t n = read(fd, m, sizeof(*m));
    if (n == 0) {
        return 0;
    }
    if (n != (ssize_t) sizeof(*m)) {
        return -1;
    }
    return 1;
}

static void controller_loop(
        const char *name,
        int parent_cmd_fd,
        int incoming_fd,
        int outgoing_fd) {
    log_line("CTRL", name, "started", "controller process online");

    for (;;) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(parent_cmd_fd, &rfds);
        FD_SET(incoming_fd, &rfds);
        int maxfd = parent_cmd_fd > incoming_fd ? parent_cmd_fd : incoming_fd;

        int s = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (s < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select");
            break;
        }

        if (FD_ISSET(parent_cmd_fd, &rfds)) {
            ControllerMessage m;
            int r = recv_msg(parent_cmd_fd, &m);
            if (r <= 0) {
                break;
            }
            if (m.type == CTRL_CMD_STOP) {
                log_line("CTRL", name, "stopping", "received stop command");
                break;
            }
            if (m.type == CTRL_CMD_EMERGENCY_TO_OTHER) {
                char details[128];
                snprintf(details, sizeof(details), "id=%d act=notify-neighbor", m.vehicle_id);
                log_line("CTRL", name, "emergency-out", details);
                ControllerMessage fwd = {.type = CTRL_CMD_EMERGENCY_TO_OTHER, .vehicle_id = m.vehicle_id};
                send_msg(outgoing_fd, fwd);
            }
        }

        if (FD_ISSET(incoming_fd, &rfds)) {
            ControllerMessage m;
            int r = recv_msg(incoming_fd, &m);
            if (r <= 0) {
                break;
            }
            if (m.type == CTRL_CMD_EMERGENCY_TO_OTHER) {
                char details[128];
                snprintf(details, sizeof(details), "id=%d act=clear-path", m.vehicle_id);
                log_line("CTRL", name, "emergency-in", details);
            }
        }
    }

    close(parent_cmd_fd);
    close(incoming_fd);
    close(outgoing_fd);
    _exit(0);
}

int controllers_start(ControllerSystem *sys) {
    memset(sys, 0, sizeof(*sys));

    int parent_to_f10[2];
    int parent_to_f11[2];
    int f10_to_f11[2];
    int f11_to_f10[2];

    if (pipe(parent_to_f10) != 0 || pipe(parent_to_f11) != 0 ||
        pipe(f10_to_f11) != 0 || pipe(f11_to_f10) != 0) {
        perror("pipe");
        return -1;
    }

    pid_t p10 = fork();
    if (p10 < 0) {
        perror("fork F10");
        return -1;
    }
    if (p10 == 0) {
        close(parent_to_f10[1]);
        close(parent_to_f11[0]);
        close(parent_to_f11[1]);
        close(f10_to_f11[0]);
        close(f11_to_f10[1]);
        controller_loop("F10", parent_to_f10[0], f11_to_f10[0], f10_to_f11[1]);
    }

    pid_t p11 = fork();
    if (p11 < 0) {
        perror("fork F11");
        return -1;
    }
    if (p11 == 0) {
        close(parent_to_f11[1]);
        close(parent_to_f10[0]);
        close(parent_to_f10[1]);
        close(f11_to_f10[0]);
        close(f10_to_f11[1]);
        controller_loop("F11", parent_to_f11[0], f10_to_f11[0], f11_to_f10[1]);
    }

    close(parent_to_f10[0]);
    close(parent_to_f11[0]);
    close(f10_to_f11[0]);
    close(f10_to_f11[1]);
    close(f11_to_f10[0]);
    close(f11_to_f10[1]);

    sys->pid_f10 = p10;
    sys->pid_f11 = p11;
    sys->parent_cmd_f10 = parent_to_f10[1];
    sys->parent_cmd_f11 = parent_to_f11[1];
    return 0;
}

void controllers_notify_emergency(ControllerSystem *sys, int from_f10, int vehicle_id) {
    ControllerMessage m = {.type = CTRL_CMD_EMERGENCY_TO_OTHER, .vehicle_id = vehicle_id};
    int fd = from_f10 ? sys->parent_cmd_f10 : sys->parent_cmd_f11;
    if (send_msg(fd, m) != 0) {
        perror("write emergency command");
    }
}

void controllers_stop(ControllerSystem *sys) {
    ControllerMessage stop = {.type = CTRL_CMD_STOP, .vehicle_id = -1};
    send_msg(sys->parent_cmd_f10, stop);
    send_msg(sys->parent_cmd_f11, stop);

    close(sys->parent_cmd_f10);
    close(sys->parent_cmd_f11);

    if (sys->pid_f10 > 0) {
        waitpid(sys->pid_f10, NULL, 0);
    }
    if (sys->pid_f11 > 0) {
        waitpid(sys->pid_f11, NULL, 0);
    }
}
