#include "gui.h"

#if defined(__has_include)
#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#elif __has_include(<SDL.h>)
#include <SDL.h>
#else
#error "SDL header not found. Install SDL2 development package."
#endif
#else
#include <SDL2/SDL.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef USE_SDL_TTF
#if defined(__has_include)
#if __has_include(<SDL2/SDL_ttf.h>)
#include <SDL2/SDL_ttf.h>
#elif __has_include(<SDL_ttf.h>)
#include <SDL_ttf.h>
#else
#undef USE_SDL_TTF
#endif
#else
#include <SDL2/SDL_ttf.h>
#endif
#endif

static const char **glyph5x7(char ch) {
    switch (toupper((unsigned char) ch)) {
        case 'A': { static const char *g[] = {" ### ","#   #","#   #","#####","#   #","#   #","#   #"}; return g; }
        case 'B': { static const char *g[] = {"#### ","#   #","#   #","#### ","#   #","#   #","#### "}; return g; }
        case 'C': { static const char *g[] = {" ### ","#   #","#    ","#    ","#    ","#   #"," ### "}; return g; }
        case 'D': { static const char *g[] = {"#### ","#   #","#   #","#   #","#   #","#   #","#### "}; return g; }
        case 'E': { static const char *g[] = {"#####","#    ","#    ","#### ","#    ","#    ","#####"}; return g; }
        case 'F': { static const char *g[] = {"#####","#    ","#    ","#### ","#    ","#    ","#    "}; return g; }
        case 'G': { static const char *g[] = {" ### ","#   #","#    ","# ###","#   #","#   #"," ### "}; return g; }
        case 'H': { static const char *g[] = {"#   #","#   #","#   #","#####","#   #","#   #","#   #"}; return g; }
        case 'I': { static const char *g[] = {"#####","  #  ","  #  ","  #  ","  #  ","  #  ","#####"}; return g; }
        case 'N': { static const char *g[] = {"#   #","##  #","# # #","#  ##","#   #","#   #","#   #"}; return g; }
        case 'O': { static const char *g[] = {" ### ","#   #","#   #","#   #","#   #","#   #"," ### "}; return g; }
        case 'R': { static const char *g[] = {"#### ","#   #","#   #","#### ","# #  ","#  # ","#   #"}; return g; }
        case 'S': { static const char *g[] = {" ####","#    ","#    "," ### ","    #","    #","#### "}; return g; }
        case 'Y': { static const char *g[] = {"#   #","#   #"," # # ","  #  ","  #  ","  #  ","  #  "}; return g; }
        case '0': { static const char *g[] = {" ### ","#   #","#  ##","# # #","##  #","#   #"," ### "}; return g; }
        case '1': { static const char *g[] = {"  #  "," ##  ","  #  ","  #  ","  #  ","  #  "," ### "}; return g; }
        case '2': { static const char *g[] = {" ### ","#   #","    #","   # ","  #  "," #   ","#####"}; return g; }
        case '3': { static const char *g[] = {" ### ","#   #","    #"," ### ","    #","#   #"," ### "}; return g; }
        case '4': { static const char *g[] = {"   # ","  ## "," # # ","#  # ","#####","   # ","   # "}; return g; }
        case '5': { static const char *g[] = {"#####","#    ","#    ","#### ","    #","#   #"," ### "}; return g; }
        case '6': { static const char *g[] = {" ### ","#   #","#    ","#### ","#   #","#   #"," ### "}; return g; }
        case '7': { static const char *g[] = {"#####","    #","   # ","  #  "," #   "," #   "," #   "}; return g; }
        case '8': { static const char *g[] = {" ### ","#   #","#   #"," ### ","#   #","#   #"," ### "}; return g; }
        case '9': { static const char *g[] = {" ### ","#   #","#   #"," ####","    #","#   #"," ### "}; return g; }
        case 'T': { static const char *g[] = {"#####","  #  ","  #  ","  #  ","  #  ","  #  ","  #  "}; return g; }
        case 'P': { static const char *g[] = {"#### ","#   #","#   #","#### ","#    ","#    ","#    "}; return g; }
        case 'M': { static const char *g[] = {"#   #","## ##","# # #","#   #","#   #","#   #","#   #"}; return g; }
        case 'Q': { static const char *g[] = {" ### ","#   #","#   #","#   #","# # #","#  # "," ## #"}; return g; }
        case 'K': { static const char *g[] = {"#   #","#  # ","# #  ","##   ","# #  ","#  # ","#   #"}; return g; }
        case 'U': { static const char *g[] = {"#   #","#   #","#   #","#   #","#   #","#   #"," ### "}; return g; }
        case 'L': { static const char *g[] = {"#    ","#    ","#    ","#    ","#    ","#    ","#####"}; return g; }
        case 'W': { static const char *g[] = {"#   #","#   #","#   #","# # #","# # #","## ##","#   #"}; return g; }
        case 'V': { static const char *g[] = {"#   #","#   #","#   #","#   #","#   #"," # # ","  #  "}; return g; }
        case 'J': { static const char *g[] = {"#####","   # ","   # ","   # ","   # ","#  # "," ##  "}; return g; }
        case 'X': { static const char *g[] = {"#   #","#   #"," # # ","  #  "," # # ","#   #","#   #"}; return g; }
        case 'Z': { static const char *g[] = {"#####","    #","   # ","  #  "," #   ","#    ","#####"}; return g; }
        case '-': { static const char *g[] = {"     ","     ","     "," ### ","     ","     ","     "}; return g; }
        case ':': { static const char *g[] = {"     ","  #  ","     ","     ","     ","  #  ","     "}; return g; }
        case '=': { static const char *g[] = {"     "," ### ","     "," ### ","     ","     ","     "}; return g; }
        case '/': { static const char *g[] = {"    #","   # ","   # ","  #  "," #   "," #   ","#    "}; return g; }
        case '.': { static const char *g[] = {"     ","     ","     ","     ","     ","  ## ","  ## "}; return g; }
        default:  { static const char *g[] = {"     ","     ","     ","     ","     ","     ","     "}; return g; }
    }
}

static void draw_bitmap_text(SDL_Renderer *r, int x, int y, int scale, SDL_Color color, const char *text) {
    if (!text || !*text) {
        return;
    }
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, 255);
    int cursor_x = x;
    for (const char *p = text; *p; p++) {
        if (*p == ' ') {
            cursor_x += 4 * scale;
            continue;
        }
        const char **glyph = glyph5x7(*p);
        for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 5; col++) {
                if (glyph[row][col] != ' ') {
                    SDL_Rect px = {cursor_x + col * scale, y + row * scale, scale, scale};
                    SDL_RenderFillRect(r, &px);
                }
            }
        }
        cursor_x += 6 * scale;
    }
}

typedef struct {
    int active_count;
    int emergency_preempt;
    int emergency_active;
    int waiting_medium;
    int waiting_normal;
    MovementGroup active_group;
} TrafficSnapshot;

typedef struct {
    int parked_now;
    int waiting_now;
    int total_spots;
    int wait_capacity;
} ParkingSnapshot;

static void snapshot_intersection(
        IntersectionState *traffic,
        ParkingLot *parking,
        TrafficSnapshot *t,
        ParkingSnapshot *p) {
    pthread_mutex_lock(&traffic->lock);
    t->active_count = traffic->active_count;
    t->emergency_preempt = traffic->emergency_preempt;
    t->emergency_active = traffic->emergency_active;
    t->waiting_medium = traffic->waiting_medium;
    t->waiting_normal = traffic->waiting_normal;
    t->active_group = traffic->active_group;
    pthread_mutex_unlock(&traffic->lock);

    pthread_mutex_lock(&parking->lock);
    p->parked_now = parking->parked_now;
    p->waiting_now = parking->waiting_now;
    p->total_spots = parking->total_spots;
    p->wait_capacity = parking->wait_capacity;
    pthread_mutex_unlock(&parking->lock);
}

static void draw_bar(SDL_Renderer *r, int x, int y, int w, int h, int value, int max_value, Uint8 fr, Uint8 fg, Uint8 fb) {
    SDL_Rect border = {x, y, w, h};
    SDL_SetRenderDrawColor(r, 100, 100, 118, 255);
    SDL_RenderDrawRect(r, &border);

    if (max_value <= 0) {
        return;
    }
    if (value < 0) {
        value = 0;
    }
    if (value > max_value) {
        value = max_value;
    }
    int filled = (w - 2) * value / max_value;
    SDL_Rect fill = {x + 1, y + 1, filled, h - 2};
    SDL_SetRenderDrawColor(r, fr, fg, fb, 255);
    SDL_RenderFillRect(r, &fill);
}

static void draw_lane_marks(SDL_Renderer *r, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(r, 85, 90, 105, 255);
    SDL_RenderDrawLine(r, x + w / 2, y + 14, x + w / 2, y + h - 14);
    SDL_RenderDrawLine(r, x + 14, y + h / 2, x + w - 14, y + h / 2);

    SDL_SetRenderDrawColor(r, 165, 170, 190, 255);
    int step_x = (w - 44) / 5;
    for (int i = 0; i < 5; i++) {
        int dx = x + 22 + i * step_x;
        SDL_RenderDrawLine(r, dx, y + h / 2 - 2, dx + step_x / 3, y + h / 2 - 2);
        int dy = y + 22 + i * ((h - 44) / 5);
        SDL_RenderDrawLine(r, x + w / 2 - 2, dy, x + w / 2 - 2, dy + (h - 44) / 25);
    }
}

/* Vehicles as road bricks on horizontal + vertical arms (like console activity). */
static void draw_traffic_bricks(SDL_Renderer *r, SDL_Rect area, int active, int emergency) {
    int cx = area.x + area.w / 2;
    int cy = area.y + area.h / 2;
    int n = active;
    if (n < 0) {
        n = 0;
    }
    if (n > 22) {
        n = 22;
    }

    const int brick = 14;
    const int gap = 5;
    int nh = (n + 1) / 2;
    if (nh > 11) {
        nh = 11;
    }
    int nv = n - nh;
    if (nv > 11) {
        nv = 11;
    }

    int span_h = nh > 0 ? (nh - 1) * (brick + gap) : 0;
    int hx0 = cx - span_h / 2;

    Uint8 rf = emergency ? 255 : (Uint8) 240;
    Uint8 gf = emergency ? 70 : (Uint8) 200;
    Uint8 bf = emergency ? 70 : (Uint8) 75;

    for (int i = 0; i < nh; i++) {
        int hx = hx0 + i * (brick + gap);
        SDL_Rect b = {hx, cy - brick / 2, brick, brick};
        SDL_SetRenderDrawColor(r, rf, gf, bf, 255);
        SDL_RenderFillRect(r, &b);
        SDL_SetRenderDrawColor(r, 38, 40, 50, 255);
        SDL_RenderDrawRect(r, &b);
    }

    /* Slightly brighter on vertical arm when not emergency */
    if (!emergency) {
        rf = 230;
        gf = 190;
        bf = 70;
    }
    int span_v = nv > 0 ? (nv - 1) * (brick + gap) : 0;
    int vy0 = cy - span_v / 2;
    for (int j = 0; j < nv; j++) {
        int vy = vy0 + j * (brick + gap);
        SDL_Rect b = {cx - brick / 2, vy, brick, brick};
        SDL_SetRenderDrawColor(r, rf, gf, bf, 255);
        SDL_RenderFillRect(r, &b);
        SDL_SetRenderDrawColor(r, 38, 40, 50, 255);
        SDL_RenderDrawRect(r, &b);
    }
}

static void draw_slots(
        SDL_Renderer *r,
        int x, int y,
        int total, int filled,
        int cols,
        SDL_Color on_color,
        SDL_Color off_color) {
    if (total <= 0) {
        return;
    }
    if (filled < 0) {
        filled = 0;
    }
    if (filled > total) {
        filled = total;
    }

    const int cell_w = 16;
    const int cell_h = 10;
    const int gap = 3;
    for (int i = 0; i < total; i++) {
        int row = i / cols;
        int col = i % cols;
        SDL_Rect cell = {x + col * (cell_w + gap), y + row * (cell_h + gap), cell_w, cell_h};
        SDL_Color c = i < filled ? on_color : off_color;
        SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
        SDL_RenderFillRect(r, &cell);
        SDL_SetRenderDrawColor(r, 35, 38, 48, 255);
        SDL_RenderDrawRect(r, &cell);
    }
}

#ifdef USE_SDL_TTF
static TTF_Font *try_open_font(int ptsize) {
    const char *paths[] = {
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/SFNS.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    };
    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        TTF_Font *f = TTF_OpenFont(paths[i], ptsize);
        if (f) {
            return f;
        }
    }
    return NULL;
}

static void draw_text(SDL_Renderer *r, TTF_Font *font, int x, int y, SDL_Color color, const char *text) {
    if (!font || !text || !*text) {
        return;
    }
    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text, color);
    if (!surf) {
        return;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
    if (!tex) {
        SDL_FreeSurface(surf);
        return;
    }
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(r, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
}
#endif

static void draw_label(
        GuiContext *ctx, SDL_Renderer *r, int x, int y, int small,
        SDL_Color color, const char *text) {
#ifndef USE_SDL_TTF
    (void) ctx;
#endif
#ifdef USE_SDL_TTF
    TTF_Font *font = small ? (TTF_Font *) ctx->font_small : (TTF_Font *) ctx->font_regular;
    if (font) {
        draw_text(r, font, x, y, color, text);
        return;
    }
#endif
    draw_bitmap_text(r, x, y, small ? 2 : 3, color, text);
}

int gui_init(GuiContext *ctx) {
    ctx->initialized = 0;
    ctx->window = NULL;
    ctx->renderer = NULL;
    ctx->font_regular = NULL;
    ctx->font_small = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *w = SDL_CreateWindow(
            "OS Project - F10/F11",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            960, 700, SDL_WINDOW_SHOWN);
    if (!w) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(w);
        SDL_Quit();
        return -1;
    }

    ctx->window = w;
    ctx->renderer = r;
#ifdef USE_SDL_TTF
    if (TTF_Init() == 0) {
        ctx->font_regular = try_open_font(17);
        ctx->font_small = try_open_font(13);
    }
#endif
    ctx->initialized = 1;
    return 0;
}

int gui_process_events(volatile sig_atomic_t *stop_flag) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            *stop_flag = 1;
            return 1;
        }
    }
    return 0;
}

static const char *group_tag(MovementGroup g) {
    switch (g) {
        case GROUP_NS_FLOW:
            return "NS";
        case GROUP_EW_FLOW:
            return "EW";
        case GROUP_LEFT_ONLY:
            return "Lf";
        default:
            return "-";
    }
}

void gui_render(
        GuiContext *ctx,
        IntersectionState traffic[2],
        ParkingLot parking[2],
        int spawned,
        int target,
        int ipc_emergency_notifies) {
    if (!ctx->initialized) {
        return;
    }
    SDL_Renderer *r = (SDL_Renderer *) ctx->renderer;
    SDL_Window *w = (SDL_Window *) ctx->window;

    TrafficSnapshot t0, t1;
    ParkingSnapshot p0, p1;
    snapshot_intersection(&traffic[0], &parking[0], &t0, &p0);
    snapshot_intersection(&traffic[1], &parking[1], &t1, &p1);

    SDL_SetRenderDrawColor(r, 26, 28, 36, 255);
    SDL_RenderClear(r);

    /* Progress */
    int progress_w = 880;
    int px0 = 40;
    int py0 = 44;
    int progress = target > 0 ? (progress_w * spawned / target) : 0;
    SDL_SetRenderDrawColor(r, 55, 58, 72, 255);
    SDL_Rect pb = {px0, py0, progress_w, 16};
    SDL_RenderFillRect(r, &pb);
    SDL_SetRenderDrawColor(r, 72, 152, 230, 255);
    SDL_Rect fill = {px0, py0, progress, 16};
    SDL_RenderFillRect(r, &fill);

    /* Intersection panels */
    SDL_Rect f10 = {40, 78, 430, 340};
    SDL_Rect f11 = {490, 78, 430, 340};

    SDL_SetRenderDrawColor(r, 48, 52, 64, 255);
    SDL_RenderFillRect(r, &f10);
    SDL_RenderFillRect(r, &f11);
    SDL_SetRenderDrawColor(r, 118, 125, 150, 255);
    SDL_RenderDrawRect(r, &f10);
    SDL_RenderDrawRect(r, &f11);

    draw_lane_marks(r, f10.x + 12, f10.y + 12, f10.w - 24, f10.h - 24);
    draw_lane_marks(r, f11.x + 12, f11.y + 12, f11.w - 24, f11.h - 24);

    /* Signal lamps (professional: color only, no long text inside panel) */
    SDL_Rect lap10 = {f10.x + 16, f10.y + 12, 18, 18};
    SDL_Rect lap11 = {f11.x + 16, f11.y + 12, 18, 18};
    SDL_SetRenderDrawColor(r,
            t0.emergency_preempt ? 255 : 55,
            t0.emergency_preempt ? 65 : 200,
            t0.emergency_preempt ? 65 : 95,
            255);
    SDL_RenderFillRect(r, &lap10);
    SDL_SetRenderDrawColor(r,
            t1.emergency_preempt ? 255 : 55,
            t1.emergency_preempt ? 65 : 200,
            t1.emergency_preempt ? 65 : 95,
            255);
    SDL_RenderFillRect(r, &lap11);

    /* Traffic = bricks on roads */
    SDL_Rect inner10 = {f10.x + 12, f10.y + 12, f10.w - 24, f10.h - 24};
    SDL_Rect inner11 = {f11.x + 12, f11.y + 12, f11.w - 24, f11.h - 24};
    draw_traffic_bricks(r, inner10, t0.active_count, t0.emergency_preempt);
    draw_traffic_bricks(r, inner11, t1.active_count, t1.emergency_preempt);

    /* Parking / queue (visual only under each panel — no crowding with text) */
    int metrics_y = 432;
    draw_bar(r, 56, metrics_y, 380, 14, p0.parked_now, p0.total_spots, 65, 185, 115);
    draw_bar(r, 56, metrics_y + 22, 380, 14, p0.waiting_now, p0.wait_capacity, 95, 165, 235);
    draw_bar(r, 506, metrics_y, 380, 14, p1.parked_now, p1.total_spots, 65, 185, 115);
    draw_bar(r, 506, metrics_y + 22, 380, 14, p1.waiting_now, p1.wait_capacity, 95, 165, 235);

    SDL_Color slot_on = {65, 185, 115, 255};
    SDL_Color slot_off = {52, 58, 70, 255};
    SDL_Color wait_on = {95, 165, 235, 255};
    SDL_Color wait_off = {50, 56, 68, 255};
    draw_slots(r, 56, metrics_y + 44, p0.total_spots, p0.parked_now, 10, slot_on, slot_off);
    draw_slots(r, 506, metrics_y + 44, p1.total_spots, p1.parked_now, 10, slot_on, slot_off);
    draw_slots(r, 56, metrics_y + 62, p0.wait_capacity, p0.waiting_now, 5, wait_on, wait_off);
    draw_slots(r, 506, metrics_y + 62, p1.wait_capacity, p1.waiting_now, 5, wait_on, wait_off);

    /* Minimal labels: only intersection IDs on panel */
    SDL_Color label = {220, 224, 238, 255};
    draw_label(ctx, r, f10.x + 42, f10.y + 10, 0, label, "F10");
    draw_label(ctx, r, f11.x + 42, f11.y + 10, 0, label, "F11");

    /* Bottom strip: legend left, status right — no overlap */
    int foot_y = 640;
    SDL_SetRenderDrawColor(r, 255, 80, 80, 255);
    SDL_Rect l1 = {40, foot_y, 14, 14};
    SDL_RenderFillRect(r, &l1);
    SDL_SetRenderDrawColor(r, 240, 200, 85, 255);
    SDL_Rect l2 = {62, foot_y, 14, 14};
    SDL_RenderFillRect(r, &l2);
    SDL_SetRenderDrawColor(r, 65, 185, 115, 255);
    SDL_Rect l3 = {84, foot_y, 14, 14};
    SDL_RenderFillRect(r, &l3);
    SDL_SetRenderDrawColor(r, 95, 165, 235, 255);
    SDL_Rect l4 = {106, foot_y, 14, 14};
    SDL_RenderFillRect(r, &l4);

    SDL_Color muted = {155, 162, 185, 255};
    draw_label(ctx, r, 128, foot_y - 2, 1, muted, "R EMERG  Y ROAD  G PARK  B QUEUE");

    int complete = (spawned >= target && t0.active_count == 0 && t1.active_count == 0);
    SDL_Color st = complete ? (SDL_Color){100, 220, 130, 255} : (SDL_Color){200, 205, 220, 255};
    draw_label(ctx, r, 780, foot_y - 4, 0, st, complete ? "DONE" : "RUN");

    SDL_Color foot_accent = {130, 200, 255, 255};
    char ipc_stub[40];
    snprintf(ipc_stub, sizeof(ipc_stub), "PIPE=%d", ipc_emergency_notifies < 0 ? 0 : ipc_emergency_notifies);
    draw_label(ctx, r, 400, foot_y - 4, 1, foot_accent, ipc_stub);

    char title[320];
    snprintf(title, sizeof(title),
             "spawned %d/%d | emerg_pipe_parent=%d | F10 a=%d M=%d N=%d %s p=%d q=%d | "
             "F11 a=%d M=%d N=%d %s p=%d q=%d%s",
             spawned, target, ipc_emergency_notifies,
             t0.active_count, t0.waiting_medium, t0.waiting_normal, group_tag(t0.active_group),
             p0.parked_now, p0.waiting_now,
             t1.active_count, t1.waiting_medium, t1.waiting_normal, group_tag(t1.active_group),
             p1.parked_now, p1.waiting_now,
             complete ? " | close window" : "");
    SDL_SetWindowTitle(w, title);

    SDL_RenderPresent(r);
}

void gui_shutdown(GuiContext *ctx) {
    if (!ctx->initialized) {
        return;
    }
#ifdef USE_SDL_TTF
    if (ctx->font_regular) {
        TTF_CloseFont((TTF_Font *) ctx->font_regular);
    }
    if (ctx->font_small) {
        TTF_CloseFont((TTF_Font *) ctx->font_small);
    }
    if (TTF_WasInit()) {
        TTF_Quit();
    }
#endif
    SDL_DestroyRenderer((SDL_Renderer *) ctx->renderer);
    SDL_DestroyWindow((SDL_Window *) ctx->window);
    SDL_Quit();
    ctx->renderer = NULL;
    ctx->window = NULL;
    ctx->font_regular = NULL;
    ctx->font_small = NULL;
    ctx->initialized = 0;
}
