/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui.c
@id            viewport.raster.gui
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop X11 Double-Buffered Viewport & Presentation Orchestrator
@role          Master presentation facade coordinating X11 display, double-buffered
               pixmaps, palettes, layout, widgets, procedural assets, HUD, and 2D/2.5D rendering.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Presentation follows simulation: GUI is strictly read-only and never mutates sim state.
               4. Double-buffered rendering: off-screen pixmap blits to window, guaranteeing zero tearing.
               5. Non-overlapping button and panel geometry passes strict pairwise AABB audits.
               6. Headless resilience: gracefully falls back to dummy/CLI mode when DISPLAY is absent.
               7. Frame budget: < 16,666 microseconds per frame (60 FPS rendering target).
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Master presentation facade and orchestrator for the desktop X11 double-buffered
  viewport and 2.5D tactical engine. Coordinates color palettes, panel layouts,
  widgets, asset synthesis, HUD, and 2D/2.5D rendering pipelines while translating
  user input events into universal simulation commands.

  ### For Game Designers & Non-Programmers:
  This is the master graphics engine that opens the game window on your screen,
  draws the map, characters, and menus into an off-screen canvas, and then flips
  it to the screen 60 times a second so that there is no screen flickering or tearing.

  ### For Systems Programmers & Engine Architects:
  - Native X11 double-buffered surface management (Pixmap, GC, XSync).
  - Zero heap allocation: all draw commands write to pre-allocated buffers.
  - Decoupled from simulation: receives read-only state pointers and outputs command tokens.
  - Headless-safe fallback allowing automated CI testing without an active X server.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (X11)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* --- Tier 2: External Dependencies --- */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/raster/gui.h"
#include "viewport/styling/gui_fonts.h"
#include "viewport/reel/gui_screens.h"
#include "viewport/reel/reel.h"
#include "hero/sim_state.h"
#include "ending/game.h"
#include "log/log.h"
#include "core/time_utils.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Static Presentation State Registers
--------------------------------------------------------------------------------
Pre-allocated static singletons for display layout, widgets, and fonts.
--------------------------------------------------------------------------------
*/

static gui_layout_t       s_layout;
static gui_button_t       s_buttons[GUI_NUM_BUTTONS];
static gui_palette_t      s_palette;
static gui_font_catalog_t s_fonts;
static gui_log_t          s_log;
static gui_view_mode_t    s_view_mode = GUI_VIEW_25D_ISOMETRIC;
static gui_screen_mode_t  s_screen_mode = GUI_SCREEN_TACTICAL;
static reel_t             s_reel;

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Desktop display availability queries and presentation business logic.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Desktop Presentation Query & Event Routing
--------------------------------------------------------------------------------
Probing display connectivity, screen routing, and event dispatch logic.
--------------------------------------------------------------------------------
*/

/**
 * @brief Toggles EWMH fullscreen state via _NET_WM_STATE_FULLSCREEN client message.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches the game window between windowed mode and borderless fullscreen mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Dispatches ClientMessage event with _NET_WM_STATE_TOGGLE to the root window.
 *
 * @param[in] d X11 Display pointer.
 * @param[in] w X11 Window handle.
 */
static void toggle_fullscreen(Display *d, Window w)
{
    Atom wm_state = XInternAtom(d, "_NET_WM_STATE", False);
    Atom wm_fullscreen = XInternAtom(d, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = w;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 2; /* _NET_WM_STATE_TOGGLE */
    xev.xclient.data.l[1] = (long)wm_fullscreen;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 1; /* Normal application source */
    xev.xclient.data.l[4] = 0;

    int screen = DefaultScreen(d);
    XSendEvent(d, RootWindow(d, screen), False,
               SubstructureNotifyMask | SubstructureRedirectMask, &xev);
}

/**
 * @brief Probes whether a graphical X11 display server is reachable.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if a graphical screen is available to open a window on; returns false
 * if running headless in a terminal or automated test.
 *
 * ### For Systems Programmers & Engine Architects:
 * Attempts XOpenDisplay(NULL) and closes immediately. Returns true if non-null,
 * false if connection fails or DISPLAY environment variable is unset.
 *
 * @return true if an X11 server is accessible; false in headless contexts.
 */
bool gui_is_available(void)
{
    Display *d = XOpenDisplay(NULL);
    if (d == NULL) {
        return false;
    }
    XCloseDisplay(d);
    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
*/

/**
 * @brief Audits all interactive button rectangles and panels for AABB collisions.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures that no buttons overlap each other and all panels stay inside the window boundaries.
 *
 * ### For Systems Programmers & Engine Architects:
 * Exhaustive pairwise AABB collision audit across layout panels and command dock buttons.
 *
 * @return true if all panels and buttons reside inside viewport and do not overlap; else false.
 */
bool gui_audit_layout(void)
{
    gui_layout_t layout;
    gui_layout_get_default(&layout);

    /* Step 1: Panel non-overlap and containment */
    if (!gui_layout_audit(&layout)) {
        return false;
    }

    /* Step 2: Widget button bounds and non-overlap */
    gui_button_t test_btns[GUI_NUM_BUTTONS];
    gui_widgets_init_buttons(&layout, test_btns, GUI_NUM_BUTTONS);
    if (!gui_widgets_audit(test_btns, GUI_NUM_BUTTONS,
                           GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT)) {
        return false;
    }

    /* Step 3: Color palette arithmetic */
    if (!gui_palette_audit()) {
        return false;
    }

    /* Step 4: Asset catalog integrity */
    if (!gui_assets_audit()) {
        return false;
    }

    /* Step 5: 2D / 2.5D Viewport projection reciprocity */
    if (!gui_view2d_audit()) {
        return false;
    }

    /* Step 6: HUD chronicle ring buffer */
    if (!gui_hud_audit()) {
        return false;
    }

    /* Step 7: Typography catalog fallback audit */
    if (!gui_fonts_audit()) {
        return false;
    }

    /* Step 8: Alternate screen navigation audit */
    if (!gui_screens_audit()) {
        return false;
    }

    /* Step 9: Movie Reel & scene depth audit */
    if (!reel_audit()) {
        return false;
    }

    return true;
}

/**
 * @brief Standalone boolean audit wrapper executing layout verification and display checks.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated check verifying that the graphics engine and screen layout are ready to draw.
 *
 * ### For Systems Programmers & Engine Architects:
 * Primary audit entry point for viewport rasterizer. Delegates directly to gui_audit_layout().
 *
 * @return true if all layout, widget, palette, asset, and view invariants pass; else false.
 */
bool gui_audit(void)
{
    return gui_audit_layout();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

static const char *const gui_invariants[] = {
    "Double-buffered native X11 rendering with zero-tear guarantees",
    "Universal action dispatch without direct simulation mutation",
    "Non-overlapping panel and button geometry bounded within viewport",
    "Dynamic layout recalibration adapts smoothly to full-screen/resizing",
    "Headless fallback cleanly downgrades to CLI in DISPLAY absence"
};

const module_metadata_t gui_manifest = {
    "gui.viewport.x11",
    "gui.h",
    LAYER_2_PRESENTATION,
    { 1, 2, 0 },
    2,                          /* interface_rev */
    "2026-11-03",               /* last_updated */
    "Desktop presentation viewport, HUD, and 2.5D tactical isometric engine",
    4,                          /* dependency_count */
    { "sim.engine.core", "core.util.geom", "core.util.time", "core.util.log" },
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_MAIN_THREAD,    /* concurrency */
    16666,                      /* max_budget_us */
    5,
    {
        gui_invariants[0],
        gui_invariants[1],
        gui_invariants[2],
        gui_invariants[3],
        gui_invariants[4]
    },
    gui_preflight
};

/**
 * @brief Pre-flight diagnostic callback probing display accessibility and layout.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs startup diagnostics to verify that all interface elements fit on screen without overlap.
 *
 * ### For Systems Programmers & Engine Architects:
 * Manifest preflight hook running `gui_audit_layout()` and checking `gui_is_available()`.
 * Returns PASS or CRITICAL.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or CRITICAL.
 */
diag_result_t gui_preflight(void)
{
    diag_result_t res;
    res.subsystem = gui_manifest.module_id;
    res.check_name = "Layout Bounds, 2.5D Viewport & Display Accessibility";

    if (!gui_audit_layout()) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details =
            "Fatal: Viewport bounds exceeded, button overlap, or math error.";
        return res;
    }

    if (!gui_is_available()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details =
            "X11 display not detected ($DISPLAY empty); safe CLI fallback.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details =
        "X11 display connected; 2.5D viewport & panel geometry verified.";
    return res;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Native X11 Viewport Pump
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Launches the native X11 double-buffered presentation loop.
 *
 * ### For Game Designers & Non-Programmers:
 * Opens the main game window, starts the 60 FPS animation loop, listens for keyboard
 * and mouse clicks, and displays the game until the player closes the window.
 *
 * ### For Systems Programmers & Engine Architects:
 * Main presentation pump. Creates X11 window, off-screen Pixmap, and GC. Polls
 * XPending event queue, dispatches user input, updates Movie Reel kinematics,
 * composes backbuffer frame, and blits via XCopyArea at 60 FPS.
 *
 * @param[in,out] state Active simulation player state.
 * @return Exit status code (0 on clean exit).
 */
int gui_run(player_state_t *state)
{
    if (state == NULL) {
        return 1;
    }

    /* Step 1: Open X11 display connection */
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        LOG_ERROR(LOG_CAT_LOG, "GUIViewport", -0.5f,
                  "Failed to open X11 display connection.");
        fprintf(stderr, "GUI Error: Failed to open X11 display.\n");
        return 1;
    }

    LOG_INFO(LOG_CAT_LOG, "GUIViewport", 0.0f,
             "Initialized X11 desktop viewport (%dx%d double-buffered).",
             GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT);

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    /* Step 2: Configure top-level window attributes */
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
                     StructureNotifyMask;
    swa.background_pixel = BlackPixel(display, screen);

    int cur_win_w = GUI_WINDOW_WIDTH;
    int cur_win_h = GUI_WINDOW_HEIGHT;

    Window window = XCreateWindow(display, root,
        100, 100, (unsigned int)cur_win_w, (unsigned int)cur_win_h, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWEventMask | CWBackPixel, &swa);

    XStoreName(display, window, "Tenebris and Lux [v0.0.1]");

    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);

    XMapWindow(display, window);

    /* Step 3: Allocate off-screen double-buffer Pixmap */
    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    int window_depth = wa.depth;

    Pixmap backbuffer = XCreatePixmap(display, window,
        (unsigned int)cur_win_w, (unsigned int)cur_win_h,
        (unsigned int)window_depth);
    GC gc = XCreateGC(display, backbuffer, 0, NULL);

    /* Step 4: Initialize sub-assemblies */
    gui_palette_bind_x11(&s_palette, display, screen);
    gui_fonts_init(&s_fonts, display);
    gui_fonts_apply(display, gc, &s_fonts, GUI_FONT_UI);
    gui_layout_update(&s_layout, cur_win_w, cur_win_h);
    gui_widgets_init_buttons(&s_layout, s_buttons, GUI_NUM_BUTTONS);
    gui_assets_init();
    int resolved_assets = gui_assets_resolve_available();
    LOG_INFO(LOG_CAT_LOG, "GUI", 0.0f, "GUI assets initialized: %d external bitmaps resolved", resolved_assets);
    reel_init(&s_reel, (int32_t)GUI_SCREEN_TACTICAL, GUI_NUM_TABS);

    /* Step 5: Initialize chronicle narrative log */
    s_log.count = 0;
    gui_hud_log_append(&s_log,
        "Awakened at The Garrison Altar in Sacyra.");
    gui_hud_log_append(&s_log,
        "Bearing the Shield of Lux and Sword of Tenebris.");
    gui_hud_log_append(&s_log,
        "Tactical 2.5D active. [M] toggle 2D/2.5D, [F11] fullscreen.");

    bool running = true;

    /* Step 6: Execute 60 FPS presentation pump */
    while (running && state->is_running) {
        while (XPending(display)) {
            XEvent ev;
            XNextEvent(display, &ev);

            if (ev.type == ClientMessage) {
                if ((Atom)ev.xclient.data.l[0] == wm_delete) {
                    LOG_INFO(LOG_CAT_LOG, "GUIViewport", 0.0f,
                        "Window close received via WM_DELETE_WINDOW.");
                    running = false;
                    state->is_running = false;
                    break;
                }
            } else if (ev.type == ConfigureNotify) {
                int new_w = ev.xconfigure.width;
                int new_h = ev.xconfigure.height;
                if ((new_w != cur_win_w || new_h != cur_win_h) &&
                    new_w > 100 && new_h > 100) {
                    cur_win_w = new_w;
                    cur_win_h = new_h;
                    if (backbuffer != None) {
                        XFreePixmap(display, backbuffer);
                    }
                    backbuffer = XCreatePixmap(display, window,
                        (unsigned int)cur_win_w,
                        (unsigned int)cur_win_h,
                        (unsigned int)window_depth);
                    gui_layout_update(&s_layout, cur_win_w, cur_win_h);
                    gui_widgets_init_buttons(&s_layout, s_buttons,
                                             GUI_NUM_BUTTONS);
                    LOG_DEBUG(LOG_CAT_DEBUG, "GUIViewport", 0.0f,
                              "Window resized to %dx%d.",
                              cur_win_w, cur_win_h);
                }
            } else if (ev.type == ButtonPress) {
                int bx = ev.xbutton.x;
                int by = ev.xbutton.y;

                gui_screen_mode_t clicked_screen;
                if (gui_screens_handle_tab_click(&s_layout.header,
                                                 bx, by, &clicked_screen)) {
                    reel_navigate(&s_reel, (int32_t)clicked_screen, 0.20f);
                    s_screen_mode = clicked_screen;
                    static const char *const tab_names[GUI_NUM_TABS] = {
                        "Tactical Battle",
                        "Hero Dossier",
                        "Inventory Bag",
                        "World Map",
                        "Chronicle Codex"
                    };
                    char tab_msg[GUI_LOG_ENTRY_LEN];
                    snprintf(tab_msg, sizeof(tab_msg), "Switched screen: %s",
                             tab_names[clicked_screen]);
                    gui_hud_log_append(&s_log, tab_msg);
                } else if (s_screen_mode == GUI_SCREEN_TACTICAL) {
                    /* Check command dock buttons */
                    const gui_button_t *btn =
                        gui_button_find_clicked(s_buttons, GUI_NUM_BUTTONS,
                                                bx, by);
                    if (btn != NULL) {
                        LOG_DEBUG(LOG_CAT_DEBUG, "GUIViewport", 0.0f,
                            "Button click '%s' -> '%s'.",
                            btn->label, btn->command);
                        char out_msg[GUI_LOG_ENTRY_LEN];
                        out_msg[0] = '\0';
                        game_process_command(state, btn->command, out_msg,
                                             sizeof(out_msg));
                        if (strlen(out_msg) > 0) {
                            gui_hud_log_append(&s_log, out_msg);
                        }
                        if (strcmp(btn->command, "quit") == 0) {
                            running = false;
                        }
                    } else if (geom_point_in_rect(bx, by,
                        gui_layout_panel_to_rect(&s_layout.viewport_2d))) {
                        /* Click in 2D / 2.5D Viewport: unproject to grid */
                        int gx = 0;
                        int gy = 0;
                        int scale = gui_view2d_calc_scale(
                            &s_layout.viewport_2d);
                        if (s_view_mode == GUI_VIEW_25D_ISOMETRIC) {
                            int origin_x = 0;
                            int origin_y = 0;
                            gui_view2d_get_iso_origin(&s_layout.viewport_2d,
                                                      scale, &origin_x,
                                                      &origin_y);
                            gui_iso_unproject(bx, by, 0, origin_x, origin_y,
                                              GUI_ISO_TILE_W * scale,
                                              GUI_ISO_TILE_H * scale,
                                              GUI_ISO_ELEV_SCALE * scale,
                                              &gx, &gy);
                        } else {
                            int cell_size = 38 * scale;
                            int origin_x = 0;
                            int origin_y = 0;
                            gui_view2d_get_flat_origin(&s_layout.viewport_2d,
                                                       scale, &origin_x,
                                                       &origin_y);
                            gx = (bx - origin_x) / cell_size;
                            gy = (by - origin_y) / cell_size;
                        }

                        if (gx >= 0 && gx < 8 && gy >= 0 && gy < 8) {
                            char click_msg[GUI_LOG_ENTRY_LEN];
                            snprintf(click_msg, sizeof(click_msg),
                                     "Tactical target selected: Grid (%d, %d)",
                                     gx, gy);
                            gui_hud_log_append(&s_log, click_msg);
                        }
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

                if (ks == XK_Escape) {
                    reel_navigate(&s_reel, (int32_t)GUI_SCREEN_TACTICAL, 0.20f);
                    s_screen_mode = GUI_SCREEN_TACTICAL;
                    gui_hud_log_append(&s_log,
                        "Returned to Tactical Battle View.");
                } else if (ks == XK_1 || key == '1') {
                    reel_navigate(&s_reel, (int32_t)GUI_SCREEN_TACTICAL, 0.20f);
                    s_screen_mode = GUI_SCREEN_TACTICAL;
                    gui_hud_log_append(&s_log,
                        "Switched screen: Tactical Battle View.");
                } else if (ks == XK_2 || key == '2' || key == 'i') {
                    reel_navigate(&s_reel, (int32_t)GUI_SCREEN_STATUS, 0.20f);
                    s_screen_mode = GUI_SCREEN_STATUS;
                    gui_hud_log_append(&s_log,
                        "Switched screen: Protagonist Hero Dossier.");
                } else if (ks == XK_3 || key == '3' || key == 'u') {
                    reel_navigate(&s_reel, (int32_t)GUI_SCREEN_INVENTORY, 0.20f);
                    s_screen_mode = GUI_SCREEN_INVENTORY;
                    gui_hud_log_append(&s_log,
                        "Switched screen: 32-Slot Bag & Key Relics.");
                } else if (ks == XK_4 || key == '4' || key == 'o') {
                    reel_navigate(&s_reel, (int32_t)GUI_SCREEN_WORLD_MAP, 0.20f);
                    s_screen_mode = GUI_SCREEN_WORLD_MAP;
                    gui_hud_log_append(&s_log,
                        "Switched screen: Provincial Realm Topology.");
                } else if (ks == XK_5 || key == '5' || key == 'c') {
                    reel_navigate(&s_reel, (int32_t)GUI_SCREEN_CHRONICLE, 0.20f);
                    s_screen_mode = GUI_SCREEN_CHRONICLE;
                    gui_hud_log_append(&s_log,
                        "Switched screen: Deeds & Lore Codex.");
                } else if (ks == XK_F11 || key == 'f') {
                    toggle_fullscreen(display, window);
                    gui_hud_log_append(&s_log,
                        "Fullscreen mode toggled (F11 / F).");
                } else if (key == 'm') {
                    /* Toggle between 2.5D Isometric and 2D Top-Down */
                    s_view_mode = (s_view_mode == GUI_VIEW_25D_ISOMETRIC) ?
                        GUI_VIEW_2D_FLAT : GUI_VIEW_25D_ISOMETRIC;
                    gui_hud_log_append(&s_log,
                        (s_view_mode == GUI_VIEW_25D_ISOMETRIC) ?
                        "Viewport switched to 2.5D Isometric Mode." :
                        "Viewport switched to 2D Top-Down Mode.");
                } else if (key != 0 && s_screen_mode == GUI_SCREEN_TACTICAL) {
                    const gui_button_t *btn =
                        gui_button_find_by_hotkey(s_buttons, GUI_NUM_BUTTONS,
                                                  key);
                    if (btn != NULL) {
                        LOG_DEBUG(LOG_CAT_DEBUG, "GUIViewport", 0.0f,
                            "Hotkey '%c' triggered action '%s'.",
                            key, btn->command);
                        char out_msg[GUI_LOG_ENTRY_LEN];
                        out_msg[0] = '\0';
                        game_process_command(state, btn->command, out_msg,
                                             sizeof(out_msg));
                        if (strlen(out_msg) > 0) {
                            gui_hud_log_append(&s_log, out_msg);
                        }
                        if (strcmp(btn->command, "quit") == 0) {
                            running = false;
                        }
                    }
                }
            }
        }

        /* Step 7: Compose complete frame onto backbuffer */
        reel_update(&s_reel, 0.0166f, cur_win_w);
        s_screen_mode = (gui_screen_mode_t)reel_get_current_screen(&s_reel);

        XSetForeground(display, gc, s_palette.bg);
        XFillRectangle(display, backbuffer, gc, 0, 0,
                       (unsigned int)cur_win_w, (unsigned int)cur_win_h);

        gui_hud_render_header(display, backbuffer, gc,
                              &s_layout.header, &s_palette, &s_fonts);
        gui_screens_render_tab_bar(display, backbuffer, gc,
                                   &s_layout.header, &s_palette,
                                   s_screen_mode, &s_fonts);

        if (s_screen_mode == GUI_SCREEN_TACTICAL) {
            gui_view2d_render(display, backbuffer, gc,
                              &s_layout.viewport_2d, state,
                              &s_palette, s_view_mode, &s_fonts);
            gui_hud_render_status(display, backbuffer, gc,
                                  &s_layout.status, state,
                                  &s_palette, &s_fonts);
            gui_hud_render_chronicle(display, backbuffer, gc,
                                     &s_layout.chronicle, &s_log,
                                     &s_palette, &s_fonts);

            /* Render command dock frame and buttons */
            XSetForeground(display, gc, s_palette.panel);
            XFillRectangle(display, backbuffer, gc,
                           s_layout.command_dock.x, s_layout.command_dock.y,
                           (unsigned int)s_layout.command_dock.width,
                           (unsigned int)s_layout.command_dock.height);
            XSetForeground(display, gc, s_palette.border);
            XDrawRectangle(display, backbuffer, gc,
                           s_layout.command_dock.x, s_layout.command_dock.y,
                           (unsigned int)s_layout.command_dock.width,
                           (unsigned int)s_layout.command_dock.height);

            gui_fonts_apply(display, gc, &s_fonts, GUI_FONT_UI);
            for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
                gui_widget_draw_button(display, backbuffer, gc,
                                       &s_buttons[i], &s_palette);
            }
        } else {
            gui_panel_rect_t full_content;
            full_content.x = s_layout.viewport_2d.x;
            full_content.y = s_layout.viewport_2d.y;
            full_content.width = cur_win_w - (2 * full_content.x);
            full_content.height = cur_win_h - full_content.y - 10;
            full_content.title = "Screen View";

            if (s_screen_mode == GUI_SCREEN_STATUS) {
                gui_screen_render_status_dossier(display, backbuffer, gc,
                                                 &full_content, state,
                                                 &s_palette, &s_fonts);
            } else if (s_screen_mode == GUI_SCREEN_INVENTORY) {
                gui_screen_render_inventory(display, backbuffer, gc,
                                            &full_content, state,
                                            &s_palette, &s_fonts);
            } else if (s_screen_mode == GUI_SCREEN_WORLD_MAP) {
                gui_screen_render_world_map(display, backbuffer, gc,
                                            &full_content, state,
                                            &s_palette, &s_fonts);
            } else if (s_screen_mode == GUI_SCREEN_CHRONICLE) {
                gui_screen_render_chronicle_codex(display, backbuffer, gc,
                                                  &full_content, &s_log,
                                                  &s_palette, &s_fonts);
            }
        }

        /* Step 8: Blit offscreen backbuffer to window (zero-tear) */
        XCopyArea(display, backbuffer, window, gc, 0, 0,
                  (unsigned int)cur_win_w, (unsigned int)cur_win_h, 0, 0);
        XFlush(display);

        time_sleep_ms(16.66);
    }

    /* Step 9: Teardown display resources */
    LOG_INFO(LOG_CAT_LOG, "GUIViewport", 0.0f,
             "Tearing down presentation resources.");
    gui_fonts_teardown(&s_fonts, display);
    XFreePixmap(display, backbuffer);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}

/**
 * @brief Formats an ASCII diagnostic dump card representing Native X11 Viewport subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing display server availability, target dimensions,
 * frame rate, and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 */
int gui_execute_dump_card(char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    bool avail = gui_is_available();
    diag_result_t diag = gui_preflight();

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT RASTER SUBSYSTEM DIAGNOSTIC CARD                 \n"
        "============================================================\n"
        " Subsystem       : Native X11 Double-Buffered Viewport      \n"
        " Display Server  : %-41s\n"
        " Target Dim      : %-41s\n"
        " Target FPS      : %-41d\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        avail ? "AVAILABLE" : "HEADLESS / UNAVAILABLE",
        "1280x720 (Logical 640x360)",
        60,
        diag.severity == SEVERITY_PASS ? "PASSED" : "FAILED");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Pure procedural loop; lifecycle allocations scoped to gui_run frame).
--------------------------------------------------------------------------------
*/
