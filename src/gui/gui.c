// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui.c
// @id            gui.viewport.x11
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Native desktop X11 double-buffered GUI presentation implementation.
//   Translates X11 key and mouse events into universal simulation actions,
//   rendering the tactical viewport with 32-bit color geometry and typography.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.engine.core, core.util.geom, core.util.time
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static layout, zero heap).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Main thread event loop).
//   - Latency Budget:        < 16,666 microseconds per frame (60 FPS budget).
//
// AXIOMATIC INVARIANTS:
//   1. Zero-dependency native X11 double buffering with 32-bit alpha safety.
//   2. Button clicks and hotkeys dispatch through universal pipeline.
//   3. Diagnostic audit verifies bounding boxes and non-overlapping cards.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: STATIC BUTTON TOPOLOGY & COLOR PALETTE ]
// Mission: Define discrete button bounds, action commands, and UI palette.
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.2: PALETTE ALLOCATION & CHRONICLE HELPERS ]
// Mission: Translate 24-bit RGB values to X11 pixels and manage ring-buffers.
// -----------------------------------------------------------------------------

/**
 * @brief Allocates an X11 pixel color from a 24-bit RGB hexadecimal integer.
 *
 * ### Machine Role & Mission
 * Queries the default X11 colormap to allocate a color cell matching the
 * requested RGB value. On 32-bit composited visuals (ARGB visuals), this
 * ensures the alpha channel is clamped to 255 (0xFF000000) to prevent
 * transparent visual corruption.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Color Palette & Visual Hardware Mapping
 * - Cog: RGB Hex to Hardware Pixel Color Allocator
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic allocations).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Bound to X11 connection handle).
 * - Latency Budget: < 5 microseconds per color allocation.
 *
 * @param[in] d       Connected X11 Display handle.
 * @param[in] screen  Screen index of display.
 * @param[in] rgb_hex 24-bit RGB hexadecimal color value (0xRRGGBB).
 * @return Allocated X11 pixel value with alpha channel enforced.
 */
static unsigned long alloc_color(Display *d, int screen, unsigned int rgb_hex)
{
    /* Step 1: Deconstruct 24-bit RGB hex into 8-bit color channels */
    XColor col;
    unsigned int r = (rgb_hex >> 16) & 0xFF;
    unsigned int g = (rgb_hex >> 8)  & 0xFF;
    unsigned int b = (rgb_hex)       & 0xFF;

    /* Step 2: Scale 8-bit channels [0, 255] to X11 16-bit range [0, 65535] */
    col.red   = (unsigned short)(r * 257);
    col.green = (unsigned short)(g * 257);
    col.blue  = (unsigned short)(b * 257);
    col.flags = DoRed | DoGreen | DoBlue;

    /* Step 3: Allocate color cell from default colormap */
    if (XAllocColor(d, DefaultColormap(d, screen), &col)) {
        /* On 32-bit composited visuals, ensure alpha channel is 255 (opaque) */
        if (DefaultDepth(d, screen) == 32 || (col.pixel & 0xFF000000) == 0) {
            return col.pixel | 0xFF000000;
        }
        return col.pixel;
    }

    /* Fallback: Direct 32-bit ARGB packed integer */
    return 0xFF000000 | rgb_hex;
}

/**
 * @brief Appends a formatted narrative entry to the GUI chronicle ring-buffer.
 *
 * ### Machine Role & Mission
 * Maintains the scrolling combat and exploration narrative log in the GUI
 * viewport. When the log reaches GUI_MAX_LOG_LINES, older entries are shifted
 * upward by one position, discarding the oldest event in FIFO order.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Narrative Chronicle Ring-Buffer
 * - Cog: Bounded Chronicle String Inserter & Shifter
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic allocations, fixed buffer).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Main presentation thread only).
 * - Latency Budget: < 10 microseconds per append operation.
 *
 * @param[in,out] log Target GUI log ring-buffer struct.
 * @param[in]     msg Narrative message string to append.
 */
static void log_append(gui_log_t *log, const char *msg)
{
    /* Step 1: Guard against null or empty input */
    if (msg == NULL || strlen(msg) == 0) {
        return;
    }

    /* Step 2: Append if buffer has capacity */
    if (log->count < GUI_MAX_LOG_LINES) {
        strncpy(log->lines[log->count], msg, GUI_LOG_ENTRY_LEN - 1);
        log->lines[log->count][GUI_LOG_ENTRY_LEN - 1] = '\0';
        log->count++;
    } else {
        /* Step 3: Shift entries upward to make room for newest entry */
        for (int i = 0; i < GUI_MAX_LOG_LINES - 1; i++) {
            strncpy(log->lines[i], log->lines[i + 1], GUI_LOG_ENTRY_LEN);
        }
        strncpy(log->lines[GUI_MAX_LOG_LINES - 1], msg, GUI_LOG_ENTRY_LEN - 1);
        log->lines[GUI_MAX_LOG_LINES - 1][GUI_LOG_ENTRY_LEN - 1] = '\0';
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: DOUBLE-BUFFERED RENDERING & GEOMETRY RASTERIZATION ]
// Mission: Draw background, banner, twin relic meters, region, and buttons.
// -----------------------------------------------------------------------------

/**
 * @brief Renders a complete simulation frame onto an off-screen X11 backbuffer.
 *
 * ### Machine Role & Mission
 * Composes the entire graphical interface for a single frame: header banner,
 * twin relic meters (Shield of Lux and Sword of Tenebris), stance and moral
 * sway indicators, regional status panel, action chronicle log, and interactive
 * command buttons. Rendering directly to an off-screen pixmap guarantees
 * tear-free presentation.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Double-Buffered Graphics Frame Composer
 * - Cog: Frame Rasterization & Viewport Renderer
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic allocations).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Bound to X11 main event loop).
 * - Latency Budget: < 5000 microseconds (well within 16,666 us 60 FPS budget).
 *
 * @param[in] d     Connected X11 Display handle.
 * @param[in] dr    Target Drawable (off-screen Pixmap backbuffer).
 * @param[in] gc    Graphics Context handle.
 * @param[in] state Pointer to active simulation player state.
 * @param[in] p     Pointer to pre-allocated color palette.
 * @param[in] log   Pointer to narrative chronicle ring-buffer.
 */
static void render_frame(Display *d, Drawable dr, GC gc,
    const player_state_t *state, const gui_palette_t *p, const gui_log_t *log)
{
    /* Step 1: Render background canvas */
    XSetForeground(d, gc, p->bg);
    XFillRectangle(d, dr, gc, 0, 0, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT);

    /* Step 2: Render header banner */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 15, GUI_WINDOW_WIDTH - 40, 50);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 15, GUI_WINDOW_WIDTH - 40, 50);

    XSetForeground(d, gc, p->shield_gold);
    XDrawString(d, dr, gc, 40, 42, "TENEBRIS AND LUX  [v0.0.1]", 26);
    XSetForeground(d, gc, p->text_muted);
    XDrawString(d, dr, gc, 260, 42,
        "- The Chronicles of the Shield of Lux and Sword of Tenebris", 59);

    /* Step 3: Render dual relics status panel */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 75, GUI_WINDOW_WIDTH - 40, 85);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 75, GUI_WINDOW_WIDTH - 40, 85);

    /* Left Meter: Shield of Lux */
    char shield_label[64];
    snprintf(shield_label, sizeof(shield_label),
             "Shield of Lux (Aegis): %d / 100", state->shield_power);
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
        XFillRectangle(d, dr, gc, bar_s_x + 1, bar_s_y + 1,
                       fill_s - 2, bar_s_h - 2);
    }

    /* Right Meter: Sword of Tenebris */
    char sword_label[64];
    snprintf(sword_label, sizeof(sword_label),
             "Sword of Tenebris (Edge): %d / 100", state->sword_edge);
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
        XFillRectangle(d, dr, gc, bar_t_x + 1, bar_t_y + 1,
                       fill_t - 2, bar_t_h - 2);
    }

    /* Step 4: Render stance and moral sway */
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

    /* Step 5: Render Kingdom Region viewport */
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
            "[!] COMBAT ENGAGED: Vanguard Threat (Vigor: %d/100)",
            state->threat_hp);
        XSetForeground(d, gc, p->malice_red);
        XDrawString(d, dr, gc, 40, 260, threat_str, strlen(threat_str));
    } else {
        char honor_str[128];
        snprintf(honor_str, sizeof(honor_str),
            "Secured | Foes: %d | Warded: %d | Turns: %d",
            state->foes_vanquished, state->innocents_warded,
            state->turns_taken);
        XSetForeground(d, gc, p->text_muted);
        XDrawString(d, dr, gc, 40, 260, honor_str, strlen(honor_str));
    }

    /* Step 6: Render action log / chronicle viewport */
    XSetForeground(d, gc, p->panel);
    XFillRectangle(d, dr, gc, 20, 295, GUI_WINDOW_WIDTH - 40, 175);
    XSetForeground(d, gc, p->border);
    XDrawRectangle(d, dr, gc, 20, 295, GUI_WINDOW_WIDTH - 40, 175);

    XSetForeground(d, gc, p->text_muted);
    XDrawString(d, dr, gc, 40, 318, "--- Chronicle of Deeds ---", 26);

    for (int i = 0; i < log->count; i++) {
        unsigned long fg = (i == log->count - 1) ?
            p->shield_gold : p->text_main;
        XSetForeground(d, gc, fg);
        XDrawString(d, dr, gc, 40, 342 + (i * 22), log->lines[i],
            strlen(log->lines[i]));
    }

    /* Step 7: Render interactive action buttons */
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: SPATIAL INPUT DETECTION & HOTKEY ROUTING ]
// Mission: Resolve mouse click coordinates and keystrokes into button actions.
// -----------------------------------------------------------------------------

/**
 * @brief Locates an interactive button at the specified screen coordinates.
 *
 * ### Machine Role & Mission
 * Iterates through the static button topology table, performing a 2D
 * point-in-rectangle hit test via Layer 0 geometric primitives. Returns the
 * matching button pointer if a hit is detected.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Spatial Button Collision & Input Mapping
 * - Cog: Cursor Point-in-Rectangle Hit Detector
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic allocations, pointer lookup).
 * - Concurrency: REENTRANT_PURE (Pure immutable button table inspection).
 * - Latency Budget: < 1 microsecond.
 *
 * @param[in] x Screen horizontal X coordinate in pixels.
 * @param[in] y Screen vertical Y coordinate in pixels.
 * @return Pointer to matching `gui_button_t`, or NULL if no button hit.
 */
static const gui_button_t *find_clicked_button(int x, int y)
{
    /* Step 1: Iterate through button list and test point intersection */
    for (int i = 0; i < NUM_BUTTONS; i++) {
        geom_rect_t rect = geom_make_rect(buttons[i].x, buttons[i].y,
            buttons[i].width, buttons[i].height);
        if (geom_point_in_rect(x, y, rect)) {
            return &buttons[i];
        }
    }

    /* Step 2: No collision detected */
    return NULL;
}

/**
 * @brief Resolves a keyboard character into its associated button action.
 *
 * ### Machine Role & Mission
 * Performs an exact case-sensitive match against registered button hotkey
 * characters ('w', 's', 'b', 't', 'l', 'i', 'n', 'r', 'q').
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Spatial Button Collision & Input Mapping
 * - Cog: Hotkey Character Action Button Resolver
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic allocations).
 * - Concurrency: REENTRANT_PURE (Immutable table inspection).
 * - Latency Budget: < 1 microsecond.
 *
 * @param[in] key Hotkey character to resolve.
 * @return Pointer to matching `gui_button_t`, or NULL if unregistered.
 */
static const gui_button_t *find_button_by_hotkey(char key)
{
    /* Step 1: Iterate through button list and compare hotkey character */
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (buttons[i].hotkey == key) {
            return &buttons[i];
        }
    }

    /* Step 2: Unmatched hotkey */
    return NULL;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: PUBLIC PRESENTATION INTERFACE & EVENT PUMP ]
// Mission: Probe display availability and execute the X11 double-buffered loop.
// -----------------------------------------------------------------------------

/**
 * @brief Probes whether a graphical X11 display server is reachable.
 *
 * ### Machine Role & Mission
 * Attempts a lightweight connection probe via XOpenDisplay(NULL) to test
 * if an X11 server is accessible. This allows the application entry router
 * to gracefully downgrade to terminal CLI mode in headless environments.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: X11 Server Connection & Headless Probe
 * - Cog: Display Connection Probe Primitive
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero heap allocations).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Opens temporary connection).
 * - Latency Budget: < 2000 microseconds probe time.
 *
 * @return `true` if an X11 server is accessible; `false` in headless contexts.
 */
bool gui_is_available(void)
{
    /* Step 1: Attempt connection to X11 display from $DISPLAY */
    Display *d = XOpenDisplay(NULL);
    if (d == NULL) {
        return false;
    }

    /* Step 2: Close test connection and report accessibility */
    XCloseDisplay(d);
    return true;
}

/**
 * @brief Launches the native X11 double-buffered presentation loop.
 *
 * ### Machine Role & Mission
 * Serves as the primary graphical desktop interface for Tenebris and Lux.
 * Creates an X11 top-level window, off-screen backbuffer pixmap, initializes
 * the dark fantasy color palette, and pumps user input events (clicks, keys)
 * at 60 FPS while dispatching actions to the universal simulation engine.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Presentation Lifecycle & Event Dispatch
 * - Cog: Native X11 Double-Buffered Event Loop
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Static structures, zero engine heap).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Bound to X11 event loop).
 * - Latency Budget: < 16,666 microseconds per frame (60 FPS budget).
 *
 * @param[in,out] state Active simulation player state.
 * @return Exit status code (0 on clean exit, non-zero on failure).
 */
int gui_run(player_state_t *state)
{
    /* Step 1: Open X11 display connection */
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "GUI Error: Failed to open X11 display.\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    /* Step 2: Configure window attributes and event masks */
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
                     StructureNotifyMask;
    swa.background_pixel = BlackPixel(display, screen);

    Window window = XCreateWindow(display, root,
        100, 100, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWEventMask | CWBackPixel, &swa);

    XStoreName(display, window, "Tenebris and Lux [v0.0.1]");

    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);

    XMapWindow(display, window);

    /* Step 3: Allocate off-screen double-buffer pixmap and graphics context */
    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    int window_depth = wa.depth;

    Pixmap backbuffer = XCreatePixmap(display, window,
        GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, window_depth);
    GC gc = XCreateGC(display, backbuffer, 0, NULL);

    /* Step 4: Allocate color palette */
    gui_palette_t pal;
    pal.bg           = alloc_color(display, screen, 0x0D0E12); // Obsidian Void
    pal.panel        = alloc_color(display, screen, 0x1A1D24); // Dark Granite
    pal.border       = alloc_color(display, screen, 0x2E3440); // Carved Stone
    pal.text_main    = alloc_color(display, screen, 0xECEFF4); // Ivory
    pal.text_muted   = alloc_color(display, screen, 0x8892B0); // Silver
    pal.shield_gold  = alloc_color(display, screen, 0xF0CE78); // Lux Gold
    pal.sword_violet = alloc_color(display, screen, 0x8A78C8); // Violet
    pal.virtue_green = alloc_color(display, screen, 0x52B788); // Emerald
    pal.malice_red   = alloc_color(display, screen, 0xE63946); // Crimson
    pal.btn_bg       = alloc_color(display, screen, 0x1E222B); // Button Slate
    pal.btn_border   = alloc_color(display, screen, 0x3E4452); // Button Rim

    /* Step 5: Initialize chronicle narrative log */
    gui_log_t log;
    log.count = 0;
    log_append(&log, "Awakened at The Garrison Altar in Sacyra.");
    log_append(&log, "You bear the Shield of Lux and the Sword of Tenebris.");
    log_append(&log, "The Dark Lord Malakor stirs in the deep abyss.");

    bool running = true;

    /* Step 6: Execute 60 FPS double-buffered event pump */
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
                const gui_button_t *btn =
                    find_clicked_button(ev.xbutton.x, ev.xbutton.y);
                if (btn != NULL) {
                    char out_msg[GUI_LOG_ENTRY_LEN];
                    out_msg[0] = '\0';
                    game_process_command(state, btn->command, out_msg,
                                         sizeof(out_msg));
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
                        game_process_command(state, btn->command, out_msg,
                                             sizeof(out_msg));
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

    /* Step 7: Teardown display resources and backbuffer */
    XFreePixmap(display, backbuffer);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: INVARIANT MANIFEST & PRE-FLIGHT VERIFIER ]
// Mission: Provide verifiable metadata and boot-time layout bounds checks.
// -----------------------------------------------------------------------------

static const char *const gui_invariants[] = {
    "Double-buffered native X11 rendering",
    "Universal action dispatch without direct mutation",
    "Non-overlapping button geometry and bounded layout",
    "Headless fallback when X11 display is unavailable"
};

const module_metadata_t gui_manifest = {
    "gui.viewport.x11",
    "gui.h",
    LAYER_2_PRESENTATION,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-26",               // last_updated
    "Native X11 double-buffered presentation viewport and event dispatcher",
    3,                          // dependency_count
    { "sim.engine.core", "core.util.geom", "core.util.time", NULL },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_MAIN_THREAD,    // concurrency
    16666,                      // max_budget_us
    4,
    {
        gui_invariants[0],
        gui_invariants[1],
        gui_invariants[2],
        gui_invariants[3]
    },
    gui_preflight
};

/**
 * @brief Pre-flight diagnostic callback probing display accessibility.
 *
 * ### Machine Role & Mission
 * Audits GUI prerequisites prior to game loop initiation. Performs two tests:
 * 1. Fatal Check: Layout bounds and button non-overlap audit.
 * 2. Critical Check: Display connection probe. If missing, reports
 *    SEVERITY_CRITICAL enabling graceful downgrade to CLI mode.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Viewport Bounds Audits & Pre-Flight
 * - Cog: GUI Pre-Flight Diagnostic Verifier
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic allocations).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Probes system environment).
 * - Latency Budget: < 2000 microseconds execution time.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or CRITICAL.
 */
diag_result_t gui_preflight(void)
{
    /* Step 1: Initialize diagnostic result record */
    diag_result_t res;
    res.subsystem = gui_manifest.module_id;
    res.check_name = "Layout Bounds & Display Availability";

    /* Step 2: Fatal Check - Layout bounds & non-overlapping buttons */
    if (!gui_audit_layout()) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Viewport bounds exceeded or button overlap detected.";
        return res;
    }

    /* Step 3: Critical Check - Native display availability */
    if (!gui_is_available()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details =
            "X11 display not detected ($DISPLAY empty); safe CLI fallback.";
        return res;
    }

    /* Step 4: Verification passed */
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details =
        "X11 display connected; viewport bounds & button geometry verified.";
    return res;
}

/**
 * @brief Audits all interactive button rectangles for collisions and bounds.
 *
 * ### Machine Role & Mission
 * Iterates through all registered button definitions, asserting that every
 * button rectangle resides strictly within [0, GUI_WINDOW_WIDTH] x [0,
 * GUI_WINDOW_HEIGHT] and does not intersect any other button's bounding box.
 *
 * ### Ontological Classification
 * - Stratum: Layer 2 (Presentation & Viewport)
 * - Machine: gui.viewport.x11 (Native Desktop X11 Viewport)
 * - Assembly: Viewport Bounds Audits & Pre-Flight
 * - Cog: Bounding Box Bounds & Non-Overlap Invariant Auditor
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
 * - Concurrency: REENTRANT_PURE (Immutable table inspection).
 * - Latency Budget: < 50 microseconds.
 *
 * @return `true` if all buttons reside inside viewport and do not overlap.
 */
bool gui_audit_layout(void)
{
    /* Step 1: Verify each button is contained entirely within the window */
    for (int i = 0; i < NUM_BUTTONS; i++) {
        const gui_button_t *b = &buttons[i];
        geom_rect_t r1 = geom_make_rect(b->x, b->y, b->width, b->height);

        if (!geom_rect_within_bounds(r1, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT)) {
            fprintf(stderr,
                "[GUI AUDIT] Button '%s' exceeds window: x=%d y=%d w=%d h=%d\n",
                b->label, b->x, b->y, b->width, b->height);
            return false;
        }

        /* Step 2: Check for accidental overlap with subsequent buttons */
        for (int j = i + 1; j < NUM_BUTTONS; j++) {
            const gui_button_t *b2 = &buttons[j];
            geom_rect_t r2 =
                geom_make_rect(b2->x, b2->y, b2->width, b2->height);
            if (geom_rect_intersects(r1, r2)) {
                fprintf(stderr,
                    "[GUI AUDIT] Overlap detected between '%s' and '%s'\n",
                    b->label, b2->label);
                return false;
            }
        }
    }

    /* Step 3: All buttons strictly conform to layout geometry */
    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of gui.c
