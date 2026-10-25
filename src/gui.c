// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui.c
// @layer         Presentation & Viewport Subsystem
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Native Desktop X11 Double-Buffered GUI Implementation
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Zero-dependency native X11 double buffering.
//                2. Every button click or hotkey dispatches through core engine.
//                3. Diagnostic audit verifies layout bounding boxes at startup.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// ( System Headers, Visual Palette, Layout Geometry & State Tables )
// =============================================================================

#define _POSIX_C_SOURCE 199309L

#include "gui.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_BUTTONS 9

static const gui_button_t buttons[NUM_BUTTONS] = {
    {  40, 480, 160, 42, "Look (L)",      "look",       'l' },
    { 220, 480, 160, 42, "Explore (E)",   "explore",    'e' },
    { 400, 480, 160, 42, "Kindle (K)",    "kindle",     'k' },
    { 580, 480, 160, 42, "Status (T)",    "status",     't' },
    {  40, 536, 160, 42, "North (N)",     "move north", 'n' },
    { 220, 536, 160, 42, "South (S)",     "move south", 's' },
    { 400, 536, 160, 42, "East (R)",      "move east",  'r' },
    { 580, 536, 160, 42, "West (W)",      "move west",  'w' },
    { 750,  20,  70, 32, "Quit",          "quit",       'q' }
};

typedef struct {
    unsigned long bg;
    unsigned long panel;
    unsigned long border;
    unsigned long text_main;
    unsigned long text_muted;
    unsigned long lux_high;
    unsigned long lux_med;
    unsigned long lux_low;
    unsigned long btn_bg;
    unsigned long btn_border;
} gui_palette_t;

static unsigned long alloc_color(Display *d, int screen, unsigned int rgb_hex)
{
    XColor col;
    unsigned int r = (rgb_hex >> 16) & 0xFF;
    unsigned int g = (rgb_hex >> 8)  & 0xFF;
    unsigned int b = (rgb_hex)       & 0xFF;
    col.red   = (unsigned short)(r * 257);
    col.green = (unsigned short)(g * 257);
    col.blue  = (unsigned short)(b * 257);
    col.flags = DoRed | DoGreen | DoBlue;
    if (XAllocColor(d, DefaultColormap(d, screen), &col)) {
        /* On 32-bit composited visuals, ensure alpha channel is 255 (opaque) */
        if (DefaultDepth(d, screen) == 32 || (col.pixel & 0xFF000000) == 0) {
            return col.pixel | 0xFF000000;
        }
        return col.pixel;
    }
    return 0xFF000000 | rgb_hex;
}

static void log_append(gui_log_t *log, const char *msg)
{
    if (msg == NULL || strlen(msg) == 0) {
        return;
    }
    if (log->count < GUI_MAX_LOG_LINES) {
        strncpy(log->lines[log->count], msg, GUI_LOG_ENTRY_LEN - 1);
        log->lines[log->count][GUI_LOG_ENTRY_LEN - 1] = '\0';
        log->count++;
    } else {
        for (int i = 0; i < GUI_MAX_LOG_LINES - 1; i++) {
            strncpy(log->lines[i], log->lines[i + 1], GUI_LOG_ENTRY_LEN);
        }
        strncpy(log->lines[GUI_MAX_LOG_LINES - 1], msg, GUI_LOG_ENTRY_LEN - 1);
        log->lines[GUI_MAX_LOG_LINES - 1][GUI_LOG_ENTRY_LEN - 1] = '\0';
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Native Double-Buffered Rendering Routines & Event Dispatcher )
// =============================================================================

static void render_frame(Display *d, Drawable dr, GC gc,
    const player_state_t *state, const gui_palette_t *p, const gui_log_t *log)
{
    /* Background Canvas */
    XSetForeground(d, gc, p->bg);
    XFillRectangle(d, dr, gc, 0, 0, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT);

    /* --- Header Banner --- */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 15, GUI_WINDOW_WIDTH - 40, 50);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 15, GUI_WINDOW_WIDTH - 40, 50);

    XSetForeground(d, gc, p->lux_high);
    XDrawString(d, dr, gc, 40, 42, "TENEBRIS AND LUX  [v0.0.1]", 26);
    XSetForeground(d, gc, p->text_muted);
    XDrawString(d, dr, gc, 260, 42,
        "- An Ontological Journey Between Light and Shadow", 49);

    /* --- Lux Energy Gauge Panel --- */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 75, GUI_WINDOW_WIDTH - 40, 65);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 75, GUI_WINDOW_WIDTH - 40, 65);

    char lux_label[64];
    snprintf(lux_label, sizeof(lux_label), "Lux Level: %d / %d",
        state->lux, MAX_LUX);
    XSetForeground(d, gc, p->text_main);
    XDrawString(d, dr, gc, 40, 98, lux_label, strlen(lux_label));

    char stat_str[128];
    snprintf(stat_str, sizeof(stat_str),
        "Lumina Shards: %d    |    Turns Survived: %d",
        state->lumina_shards, state->turns_survived);
    XSetForeground(d, gc, p->text_muted);
    XDrawString(d, dr, gc, 400, 98, stat_str, strlen(stat_str));

    /* Visual Meter Bar */
    int bar_x = 40;
    int bar_y = 108;
    int bar_w = GUI_WINDOW_WIDTH - 80;
    int bar_h = 18;

    XSetForeground(d, gc, p->bg);
    XFillRectangle(d, dr, gc, bar_x, bar_y, bar_w, bar_h);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, bar_x, bar_y, bar_w, bar_h);

    int fill_w = (state->lux * bar_w) / MAX_LUX;
    if (fill_w > 0) {
        if (state->lux > 50) {
            XSetForeground(d, gc, p->lux_high);
        } else if (state->lux > 20) {
            XSetForeground(d, gc, p->lux_med);
        } else {
            XSetForeground(d, gc, p->lux_low);
        }
        XFillRectangle(d, dr, gc, bar_x + 1, bar_y + 1, fill_w - 2, bar_h - 2);
    }

    /* --- Room Chamber Viewport --- */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 150, GUI_WINDOW_WIDTH - 40, 130);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 150, GUI_WINDOW_WIDTH - 40, 130);

    const char *room_name = game_get_room_name(state->current_room);
    const char *room_desc = game_get_room_desc(state->current_room);

    char room_hdr[128];
    snprintf(room_hdr, sizeof(room_hdr), "[ Chamber: %s ]", room_name);
    XSetForeground(d, gc, p->lux_high);
    XDrawString(d, dr, gc, 40, 180, room_hdr, strlen(room_hdr));

    XSetForeground(d, gc, p->text_main);
    XDrawString(d, dr, gc, 40, 215, room_desc, strlen(room_desc));

    if (state->lux <= LOW_LUX_THRESHOLD && state->is_running) {
        XSetForeground(d, gc, p->lux_low);
        XDrawString(d, dr, gc, 40, 255,
            "[!] WARNING: Your Lux is critically low! Kindle a Lumina shard immediately.", 75);
    } else if (!state->is_running) {
        XSetForeground(d, gc, p->lux_low);
        XDrawString(d, dr, gc, 40, 255,
            "[!] Darkness has claimed your essence. The run has ended.", 57);
    }

    /* --- Chronicle / Action Log Viewport --- */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 290, GUI_WINDOW_WIDTH - 40, 175);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 290, GUI_WINDOW_WIDTH - 40, 175);

    XSetForeground(d, gc, p->text_muted);
    XDrawString(d, dr, gc, 40, 315, "--- Chronicle of Events ---", 27);

    for (int i = 0; i < log->count; i++) {
        XSetForeground(d, gc, (i == log->count - 1) ? p->lux_high : p->text_main);
        XDrawString(d, dr, gc, 40, 340 + (i * 22), log->lines[i],
            strlen(log->lines[i]));
    }

    /* --- Interactive Action Buttons --- */
    for (int i = 0; i < NUM_BUTTONS; i++) {
        const gui_button_t *b = &buttons[i];

        XSetForeground(d, gc, p->btn_bg);
        XFillRectangle(d, dr, gc, b->x, b->y, b->width, b->height);

        XSetForeground(d, gc, p->btn_border);
        XDrawRectangle(d, dr, gc, b->x, b->y, b->width, b->height);

        XSetForeground(d, gc, (i == NUM_BUTTONS - 1) ? p->lux_low : p->lux_high);
        int text_x = b->x + 16;
        int text_y = b->y + 25;
        XDrawString(d, dr, gc, text_x, text_y, b->label, strlen(b->label));
    }
}

static void trigger_command(player_state_t *state, const char *cmd,
    gui_log_t *log)
{
    char msg[GUI_LOG_ENTRY_LEN];
    msg[0] = '\0';
    game_process_command(state, cmd, msg, sizeof(msg));
    if (strlen(msg) > 0) {
        log_append(log, msg);
    }
}

int gui_run(player_state_t *state)
{
    Display *d = XOpenDisplay(NULL);
    if (!d) {
        fprintf(stderr, "GUI Error: Cannot open X11 display.\n");
        return 1;
    }

    int screen = DefaultScreen(d);
    Window root = RootWindow(d, screen);

    gui_palette_t p;
    p.bg         = alloc_color(d, screen, 0x13101C);
    p.panel      = alloc_color(d, screen, 0x1F1A2C);
    p.border     = alloc_color(d, screen, 0x3C324E);
    p.text_main  = alloc_color(d, screen, 0xE8E2F2);
    p.text_muted = alloc_color(d, screen, 0x8C829E);
    p.lux_high   = alloc_color(d, screen, 0xF0CE78);
    p.lux_med    = alloc_color(d, screen, 0xD49442);
    p.lux_low    = alloc_color(d, screen, 0xC84444);
    p.btn_bg     = alloc_color(d, screen, 0x2A2338);
    p.btn_border = alloc_color(d, screen, 0x564870);

    Window win = XCreateSimpleWindow(d, root, 100, 100,
        GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, 1, p.border, p.bg);

    XStoreName(d, win, "Tenebris and Lux [v0.0.1]");

    Atom wm_delete = XInternAtom(d, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(d, win, &wm_delete, 1);

    XSelectInput(d, win, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(d, win);

    XWindowAttributes wa;
    XGetWindowAttributes(d, win, &wa);

    GC gc = XCreateGC(d, win, 0, NULL);
    Pixmap backbuffer = XCreatePixmap(d, win,
        GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, wa.depth);

    gui_log_t log;
    log.count = 0;
    log_append(&log, "You awaken in the silence of The Fading Sanctuary.");
    log_append(&log, "Creeping Tenebris surrounds you. Manage your Lux carefully.");

    bool running = true;
    while (running && state->is_running) {
        while (XPending(d)) {
            XEvent ev;
            XNextEvent(d, &ev);

            if (ev.type == ClientMessage) {
                if ((Atom)ev.xclient.data.l[0] == wm_delete) {
                    running = false;
                }
            } else if (ev.type == KeyPress) {
                KeySym key = XLookupKeysym(&ev.xkey, 0);
                if (key == XK_q || key == XK_Escape) {
                    running = false;
                } else if (key == XK_e) {
                    trigger_command(state, "explore", &log);
                } else if (key == XK_k) {
                    trigger_command(state, "kindle", &log);
                } else if (key == XK_l) {
                    trigger_command(state, "look", &log);
                } else if (key == XK_t) {
                    trigger_command(state, "status", &log);
                } else if (key == XK_n) {
                    trigger_command(state, "move north", &log);
                } else if (key == XK_s) {
                    trigger_command(state, "move south", &log);
                } else if (key == XK_r) {
                    trigger_command(state, "move east", &log);
                } else if (key == XK_w) {
                    trigger_command(state, "move west", &log);
                }
            } else if (ev.type == ButtonPress) {
                int bx = ev.xbutton.x;
                int by = ev.xbutton.y;
                for (int i = 0; i < NUM_BUTTONS; i++) {
                    const gui_button_t *b = &buttons[i];
                    if (bx >= b->x && bx <= b->x + b->width &&
                        by >= b->y && by <= b->y + b->height) {
                        if (strcmp(b->command, "quit") == 0) {
                            running = false;
                        } else {
                            trigger_command(state, b->command, &log);
                        }
                        break;
                    }
                }
            }
        }

        render_frame(d, backbuffer, gc, state, &p, &log);
        XCopyArea(d, backbuffer, win, gc, 0, 0,
            GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, 0, 0);
        XFlush(d);

        struct timespec ts = {0, 16000000}; /* ~60 FPS */
        nanosleep(&ts, NULL);
    }

    XFreePixmap(d, backbuffer);
    XFreeGC(d, gc);
    XDestroyWindow(d, win);
    XCloseDisplay(d);
    return 0;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Viewport Integrity Checks & Surface Tripwires )
// =============================================================================

bool gui_audit_layout(void)
{
    /* Invariant: All buttons must be within window dimensions */
    for (int i = 0; i < NUM_BUTTONS; i++) {
        const gui_button_t *b = &buttons[i];
        if (b->x < 0 || b->y < 0) {
            return false;
        }
        if (b->x + b->width > GUI_WINDOW_WIDTH ||
            b->y + b->height > GUI_WINDOW_HEIGHT) {
            return false;
        }
    }
    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Headless Display Probe & Subsystem Teardown )
// =============================================================================

bool gui_is_available(void)
{
    Display *d = XOpenDisplay(NULL);
    if (d) {
        XCloseDisplay(d);
        return true;
    }
    return false;
}
