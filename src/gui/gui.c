// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui.c
// @layer         Layer 2: Presentation & Viewport Subsystem
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Native Desktop X11 Double-Buffered GUI Implementation
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Zero-dependency native X11 double buffering with 32-bit alpha safety.
//                2. Every button click or hotkey dispatches through universal action pipeline.
//                3. Diagnostic audit verifies layout bounding boxes and non-overlapping cards.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// ( System Headers, Visual Palette, Layout Geometry & State Tables )
// =============================================================================

#include "gui/gui.h"
#include "sim/game.h"
#include "util/geom_utils.h"
#include "util/math_utils.h"
#include "util/time_utils.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_BUTTONS 9

static const gui_button_t buttons[NUM_BUTTONS] = {
    {  40, 480, 160, 42, "Ward (W)",      "ward",               'w' },
    { 220, 480, 160, 42, "Strike (S)",    "strike",             's' },
    { 400, 480, 160, 42, "Balance (B)",   "balance",            'b' },
    { 580, 480, 160, 42, "Stance (T)",    "stance equilibrium", 't' },
    {  40, 536, 160, 42, "Look (L)",      "look",               'l' },
    { 220, 536, 160, 42, "Status (I)",    "status",             'i' },
    { 400, 536, 160, 42, "Advance (N)",   "move north",         'n' },
    { 580, 536, 160, 42, "Retreat (R)",   "move south",         'r' },
    { 750,  20,  70, 32, "Quit",          "quit",               'q' }
};

typedef struct {
    unsigned long bg;
    unsigned long panel;
    unsigned long border;
    unsigned long text_main;
    unsigned long text_muted;
    unsigned long shield_gold;
    unsigned long sword_violet;
    unsigned long virtue_green;
    unsigned long malice_red;
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

    XSetForeground(d, gc, p->shield_gold);
    XDrawString(d, dr, gc, 40, 42, "TENEBRIS AND LUX  [v0.0.1]", 26);
    XSetForeground(d, gc, p->text_muted);
    XDrawString(d, dr, gc, 260, 42,
        "- The Chronicles of the Shield of Lux and Sword of Tenebris", 59);

    /* --- Dual Relics Status Panel --- */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 75, GUI_WINDOW_WIDTH - 40, 85);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 75, GUI_WINDOW_WIDTH - 40, 85);

    /* Left Meter: Shield of Lux */
    char shield_label[64];
    snprintf(shield_label, sizeof(shield_label), "Shield of Lux (Aegis): %d / 100",
        state->shield_power);
    XSetForeground(d, gc, p->shield_gold);
    XDrawString(d, dr, gc, 40, 96, shield_label, strlen(shield_label));

    int bar_s_x = 40;
    int bar_s_y = 104;
    int bar_s_w = 370;
    int bar_s_h = 16;
    XSetForeground(d, gc, p->bg);
    XFillRectangle(d, dr, gc, bar_s_x, bar_s_y, bar_s_w, bar_s_h);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, bar_s_x, bar_s_y, bar_s_w, bar_s_h);

    int fill_s = math_scale_ratio(state->shield_power, MAX_RESONANCE, bar_s_w);
    if (fill_s > 0) {
        XSetForeground(d, gc, p->shield_gold);
        XFillRectangle(d, dr, gc, bar_s_x + 1, bar_s_y + 1, fill_s - 2, bar_s_h - 2);
    }

    /* Right Meter: Sword of Tenebris */
    char sword_label[64];
    snprintf(sword_label, sizeof(sword_label), "Sword of Tenebris (Edge): %d / 100",
        state->sword_edge);
    XSetForeground(d, gc, p->sword_violet);
    XDrawString(d, dr, gc, 450, 96, sword_label, strlen(sword_label));

    int bar_t_x = 450;
    int bar_t_y = 104;
    int bar_t_w = 370;
    int bar_t_h = 16;
    XSetForeground(d, gc, p->bg);
    XFillRectangle(d, dr, gc, bar_t_x, bar_t_y, bar_t_w, bar_t_h);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, bar_t_x, bar_t_y, bar_t_w, bar_t_h);

    int fill_t = math_scale_ratio(state->sword_edge, MAX_RESONANCE, bar_t_w);
    if (fill_t > 0) {
        XSetForeground(d, gc, p->sword_violet);
        XFillRectangle(d, dr, gc, bar_t_x + 1, bar_t_y + 1, fill_t - 2, bar_t_h - 2);
    }

    /* Sub-bar line: Stance and Moral Sway */
    char stance_str[96];
    snprintf(stance_str, sizeof(stance_str), "Stance: %s",
        game_get_stance_name(state->stance));
    XSetForeground(d, gc, p->text_main);
    XDrawString(d, dr, gc, 40, 142, stance_str, strlen(stance_str));

    char sway_str[128];
    snprintf(sway_str, sizeof(sway_str), "Moral Sway: %+d [%s]",
        state->moral_sway, game_get_sway_desc(state->moral_sway));
    if (state->moral_sway > 0) {
        XSetForeground(d, gc, p->virtue_green);
    } else if (state->moral_sway < 0) {
        XSetForeground(d, gc, p->malice_red);
    } else {
        XSetForeground(d, gc, p->text_muted);
    }
    XDrawString(d, dr, gc, 450, 142, sway_str, strlen(sway_str));

    /* --- Kingdom Region Viewport --- */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 170, GUI_WINDOW_WIDTH - 40, 115);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 170, GUI_WINDOW_WIDTH - 40, 115);

    const char *loc_name = game_get_location_name(state->location);
    const char *loc_desc = game_get_location_desc(state->location);

    char loc_hdr[128];
    snprintf(loc_hdr, sizeof(loc_hdr), "[ Province: %s ]", loc_name);
    XSetForeground(d, gc, p->shield_gold);
    XDrawString(d, dr, gc, 40, 196, loc_hdr, strlen(loc_hdr));

    XSetForeground(d, gc, p->text_main);
    XDrawString(d, dr, gc, 40, 226, loc_desc, strlen(loc_desc));

    if (state->threat_hp > 0) {
        char threat_str[128];
        snprintf(threat_str, sizeof(threat_str),
            "[!] COMBAT ENGAGED: Dark Lord's Vanguard Presence (Threat Vigor: %d/100)",
            state->threat_hp);
        XSetForeground(d, gc, p->malice_red);
        XDrawString(d, dr, gc, 40, 260, threat_str, strlen(threat_str));
    } else {
        char honor_str[128];
        snprintf(honor_str, sizeof(honor_str),
            "Territory Secured | Foes Vanquished: %d | Innocents Warded: %d | Turns: %d",
            state->foes_vanquished, state->innocents_warded, state->turns_taken);
        XSetForeground(d, gc, p->text_muted);
        XDrawString(d, dr, gc, 40, 260, honor_str, strlen(honor_str));
    }

    /* --- Chronicle / Action Log Viewport --- */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 295, GUI_WINDOW_WIDTH - 40, 175);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 295, GUI_WINDOW_WIDTH - 40, 175);

    XSetForeground(d, gc, p->text_muted);
    XDrawString(d, dr, gc, 40, 318, "--- Chronicle of Deeds ---", 26);

    for (int i = 0; i < log->count; i++) {
        XSetForeground(d, gc, (i == log->count - 1) ? p->shield_gold : p->text_main);
        XDrawString(d, dr, gc, 40, 342 + (i * 22), log->lines[i],
            strlen(log->lines[i]));
    }

    /* --- Interactive Action Buttons --- */
    for (int i = 0; i < NUM_BUTTONS; i++) {
        const gui_button_t *b = &buttons[i];

        XSetForeground(d, gc, p->btn_bg);
        XFillRectangle(d, dr, gc, b->x, b->y, b->width, b->height);

        XSetForeground(d, gc, p->btn_border);
        XDrawRectangle(d, dr, gc, b->x, b->y, b->width, b->height);

        if (i == NUM_BUTTONS - 1) {
            XSetForeground(d, gc, p->malice_red);
        } else if (i == 0) {
            XSetForeground(d, gc, p->shield_gold);
        } else if (i == 1) {
            XSetForeground(d, gc, p->sword_violet);
        } else {
            XSetForeground(d, gc, p->text_main);
        }

        int text_x = b->x + 16;
        int text_y = b->y + 25;
        XDrawString(d, dr, gc, text_x, text_y, b->label, strlen(b->label));
    }
}

static const gui_button_t *find_clicked_button(int x, int y)
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        geom_rect_t rect = geom_make_rect(buttons[i].x, buttons[i].y,
            buttons[i].width, buttons[i].height);
        if (geom_point_in_rect(x, y, rect)) {
            return &buttons[i];
        }
    }
    return NULL;
}

static const gui_button_t *find_button_by_hotkey(char key)
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].hotkey == key) {
            return &buttons[i];
        }
    }
    return NULL;
}

bool gui_is_available(void)
{
    Display *d = XOpenDisplay(NULL);
    if (d == NULL) {
        return false;
    }
    XCloseDisplay(d);
    return true;
}

int gui_run(player_state_t *state)
{
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "GUI Error: Failed to open X11 display.\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
    swa.background_pixel = BlackPixel(display, screen);

    Window window = XCreateWindow(display, root,
        100, 100, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWEventMask | CWBackPixel, &swa);

    XStoreName(display, window, "Tenebris and Lux [v0.0.1]");

    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);

    XMapWindow(display, window);

    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    int window_depth = wa.depth;

    Pixmap backbuffer = XCreatePixmap(display, window,
        GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, window_depth);
    GC gc = XCreateGC(display, backbuffer, 0, NULL);

    gui_palette_t pal;
    pal.bg           = alloc_color(display, screen, 0x0D0E12); // Obsidian Void
    pal.panel        = alloc_color(display, screen, 0x1A1D24); // Dark Granite
    pal.border       = alloc_color(display, screen, 0x2E3440); // Carved Stone Border
    pal.text_main    = alloc_color(display, screen, 0xECEFF4); // Radiant Ivory
    pal.text_muted   = alloc_color(display, screen, 0x8892B0); // Twilight Silver
    pal.shield_gold  = alloc_color(display, screen, 0xF0CE78); // Shield of Lux Gold
    pal.sword_violet = alloc_color(display, screen, 0x8A78C8); // Sword of Tenebris Violet
    pal.virtue_green = alloc_color(display, screen, 0x52B788); // Virtue / Mercy Emerald
    pal.malice_red   = alloc_color(display, screen, 0xE63946); // Malice / Tyranny Crimson
    pal.btn_bg       = alloc_color(display, screen, 0x1E222B); // Button Slate
    pal.btn_border   = alloc_color(display, screen, 0x3E4452); // Button Rim

    gui_log_t log;
    log.count = 0;
    log_append(&log, "Awakened at The Garrison Altar in the Kingdom of Sol-Tenebrae.");
    log_append(&log, "You bear the Shield of Lux and the Sword of Tenebris.");
    log_append(&log, "The Dark Lord Malakor stirs in the deep abyss. Choose your stance.");

    bool running = true;

    while (running && state->is_running) {
        while (XPending(display)) {
            XEvent ev;
            XNextEvent(display, &ev);

            if (ev.type == ClientMessage) {
                if ((Atom)ev.xclient.data.l[0] == wm_delete) {
                    running = false;
                    state->is_running = false;
                    break;
                }
            } else if (ev.type == ButtonPress) {
                const gui_button_t *btn = find_clicked_button(ev.xbutton.x, ev.xbutton.y);
                if (btn != NULL) {
                    char out_msg[GUI_LOG_ENTRY_LEN];
                    out_msg[0] = '\0';
                    game_process_command(state, btn->command, out_msg, sizeof(out_msg));
                    if (strlen(out_msg) > 0) {
                        log_append(&log, out_msg);
                    }
                    if (strcmp(btn->command, "quit") == 0) {
                        running = false;
                    }
                }
            } else if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                char key = 0;
                if (ks >= XK_a && ks <= XK_z) {
                    key = (char)('a' + (ks - XK_a));
                } else if (ks >= XK_A && ks <= XK_Z) {
                    key = (char)('a' + (ks - XK_A));
                }

                if (key != 0) {
                    const gui_button_t *btn = find_button_by_hotkey(key);
                    if (btn != NULL) {
                        char out_msg[GUI_LOG_ENTRY_LEN];
                        out_msg[0] = '\0';
                        game_process_command(state, btn->command, out_msg, sizeof(out_msg));
                        if (strlen(out_msg) > 0) {
                            log_append(&log, out_msg);
                        }
                        if (strcmp(btn->command, "quit") == 0) {
                            running = false;
                        }
                    }
                }
            }
        }

        render_frame(display, backbuffer, gc, state, &pal, &log);
        XCopyArea(display, backbuffer, window, gc, 0, 0,
            GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, 0, 0);
        XFlush(display);

        time_sleep_ms(16.66);
    }

    XFreePixmap(display, backbuffer);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Viewport Integrity Checks & Surface Tripwires )
// =============================================================================

bool gui_audit_layout(void)
{
    for (int i = 0; i < NUM_BUTTONS; i++) {
        const gui_button_t *b = &buttons[i];
        geom_rect_t r1 = geom_make_rect(b->x, b->y, b->width, b->height);

        if (!geom_rect_within_bounds(r1, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT)) {
            fprintf(stderr, "[GUI AUDIT] Button '%s' exceeds window bounds: x=%d y=%d w=%d h=%d\n",
                b->label, b->x, b->y, b->width, b->height);
            return false;
        }

        /* Check for accidental button overlap with other buttons */
        for (int j = i + 1; j < NUM_BUTTONS; j++) {
            const gui_button_t *b2 = &buttons[j];
            geom_rect_t r2 = geom_make_rect(b2->x, b2->y, b2->width, b2->height);
            if (geom_rect_intersects(r1, r2)) {
                fprintf(stderr, "[GUI AUDIT] Overlap detected between '%s' and '%s'\n",
                    b->label, b2->label);
                return false;
            }
        }
    }

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of gui.c
