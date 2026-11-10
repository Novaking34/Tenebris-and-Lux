/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_screens.c
@id            viewport.reel.screens.alternate
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        5-Screen Presentation View Modes & Reel Navigators
@role          Dedicated full-canvas alternate presentation screens for High Fantasy TRPG:
               Protagonist Hero Dossier, 32-Slot Item & Relic Bag, Provincial Realm
               Topology Map, and Historical Deeds Chronicle Codex, coordinated via an
               ergonomic top tab navigation bar.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All alternate screens strictly honor viewport bounding rectangle.
               4. Screen switching is idempotent with zero state mutation.
               5. Exactly 5 canonical screens registered (F1-F5 hotkeys).
               6. Non-anonymous structs and enums enforced across all declarations.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Maintains the 5 presentation screens and top tab navigation bar:
  1. Tactical Grid Viewport (GUI_SCREEN_TACTICAL)
  2. Hero Dossier & Attributes (GUI_SCREEN_STATUS)
  3. 32-Slot Inventory & Relics (GUI_SCREEN_INVENTORY)
  4. Provincial Realm Map (GUI_SCREEN_WORLD_MAP)
  5. Chronicle & Deeds Codex (GUI_SCREEN_CHRONICLE)

  ### For Game Designers & Non-Programmers:
  Provides the full-page menu screens players switch between using hotkeys F1-F5
  or clicking the tabs at the top of the window: looking at hero stats, browsing
  the backpack and relics, viewing the province map of the realm, and reading
  past story chronicle entries.

  ### For Systems Programmers & Engine Architects:
  - Idempotent screen state machine: switching screens alters viewport display
    mode without mutating game turn state or triggering side effects.
  - Zero dynamic heap allocation: all screen descriptors reside in static memory.
  - Tab hit-testing and hotkey dispatching execute in < 1 microsecond.
  - Clean separation: renders via generic renderer callbacks and read-only state.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & LOCAL DEFINITIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
Mission: Screen navigation tab bar, mouse click dispatching, and full-canvas
         alternate screen presentation renderers.
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
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (X11) --- */
#include <X11/Xlib.h>

/* --- Tier 3: Internal Engine Foundation & Simulation State Bridge Headers --- */
#include "viewport/reel/gui_screens.h"
#include "viewport/hud/gui_widgets.h"
#include "viewport/styling/gui_fonts.h"
#include "viewport/styling/gui_assets.h"
#include "core/manifest.h"
#include "log/log.h"
#include "debug/debug.h"

/*
--------------------------------------------------------------------------------
ARCHITECTURAL PLUG COVENANT & 3-LAYER BOUNDARY SPECIFICATION:
This module sits at the simulation bridge where the Big Machine (engine/)
displays the active state of the Master Turn Gear (game/).
The formal 3-layer architecture comprises:
  1. The Engine (engine/): Generic hardware, viewport, and display apparatus.
  2. The Game The Engine Runs (The Plug: hero/sim_state.[ch] & ending/game.[ch]):
     The adapter harness translating domain state into generic display slots.
  3. The Game Itself (game/): Tactical rules, world lore, and narrative canon.
The simulation bridge headers below (hero/sim_state.h and ending/game.h) are
explicitly quarantined here as the known 'plug' seam. When modernization reaches
the game/ wave, these two file families will receive dedicated adapter elevation
to complete the formal 3-layer decoupling.
--------------------------------------------------------------------------------
*/
#include "hero/sim_state.h"
#include "ending/game.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Local Module Manifest Anchor, Category Tags & Invariant Vectors
--------------------------------------------------------------------------------
*/

#define MODULE_ID           "viewport.reel.screens"
#define LOCAL_LOG_CAT       LOG_CAT_VIEWPORT
#define LOCAL_MODULE_TAG    "GUI_SCREENS"

/*
Invariants:
1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP)
2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic)
3. Idempotent screen state machine: switching screens alters viewport display mode only
4. Tab hit-testing and hotkey dispatching execute in bounded time
5. Renders via generic renderer callbacks and read-only state
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/* None. (Screen structures and enums declared in viewport/reel/gui_screens.h) */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations) & Manifest Anchor
--------------------------------------------------------------------------------
*/

static const char *const TAB_LABELS[GUI_NUM_TABS] = {
    "Tactical [F1]",
    "Status [F2]",
    "Inventory [F3]",
    "World Map [F4]",
    "Chronicle [F5]"
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

/**
 * @brief Fast static inline check for valid screen mode enum value.
 */
static inline bool gui_screens_is_valid_mode(gui_screen_mode_t mode)
{
    return mode >= GUI_SCREEN_TACTICAL && mode <= GUI_SCREEN_CHRONICLE;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Screen navigation tab bar, mouse click dispatching, and full-canvas
alternate screen presentation renderers.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Screen Navigation & Tab Hit-Testing Cogs
--------------------------------------------------------------------------------
Top tab bar rendering, boundary calculations, and mouse pick dispatching.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders the top screen navigation tab bar inside the header banner.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws the 5 clickable menu tabs across the top of the screen (Tactical [F1],
 * Status [F2], Inventory [F3], World Map [F4], Chronicle [F5]), highlighting
 * the active screen in glowing gold.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders tab boxes, border outlines, active highlight bar, and centered labels
 * into the X11 drawable backbuffer using theme palette colors. Zero heap.
 */
void gui_screens_render_tab_bar(void *display, unsigned long dr, void *gc,
                                const gui_panel_rect_t *pnl,
                                const gui_palette_t *pal,
                                gui_screen_mode_t active,
                                const void *fonts)
{
    if (display == NULL || pnl == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    int total_tabs = GUI_NUM_TABS;
    int tab_w = 110;
    int tab_h = 24;
    int tab_gap = 6;
    int start_y = pnl->y + (pnl->height > tab_h ? (pnl->height - tab_h) / 2 : 0);
    int start_x = pnl->x + 240;

    for (int i = 0; i < total_tabs; i++) {
        int tx = start_x + i * (tab_w + tab_gap);
        bool is_active = ((gui_screen_mode_t)i == active);

        unsigned long bg = is_active ? pal->btn_hover : pal->panel;
        unsigned long border = is_active ? pal->gold_bright : pal->border;
        unsigned long fg = is_active ? pal->shield_gold : pal->text_muted;

        XSetForeground(d, g, bg);
        XFillRectangle(d, draw, g, tx, start_y, tab_w, tab_h);

        XSetForeground(d, g, border);
        XDrawRectangle(d, draw, g, tx, start_y, tab_w, tab_h);

        if (is_active) {
            /* Radiant highlight line under active tab */
            XSetForeground(d, g, pal->gold_bright);
            XFillRectangle(d, draw, g, tx + 2, start_y + tab_h - 3,
                           tab_w - 4, 2);
        }

        if (cat != NULL) {
            gui_fonts_apply(d, g, cat, GUI_FONT_UI);
        }
        XSetForeground(d, g, fg);
        int label_len = (int)strlen(TAB_LABELS[i]);
        int tw = (cat != NULL) ?
            gui_fonts_measure_width(cat, GUI_FONT_UI, TAB_LABELS[i]) :
            (label_len * 8);
        int text_x = tx + ((tab_w - tw) / 2);
        if (text_x < tx + 4) text_x = tx + 4;
        XDrawString(d, draw, g, text_x, start_y + (tab_h / 2) + 4,
                    TAB_LABELS[i], label_len);
    }
}

/**
 * @brief Checks whether a click in the header banner hit any navigation tab.
 *
 * ### For Game Designers & Non-Programmers:
 * Detects if the player clicked one of the 5 screen tabs with the mouse and
 * identifies which screen they want to view.
 *
 * ### For Systems Programmers & Engine Architects:
 * Performs linear AABB hit tests across all 5 tab bounds. If hit, writes enum
 * to `out_screen` and returns true; otherwise returns false in O(1) time.
 */
bool gui_screens_handle_tab_click(const gui_panel_rect_t *pnl,
                                  int cx, int cy,
                                  gui_screen_mode_t *out_screen)
{
    if (pnl == NULL || out_screen == NULL) {
        return false;
    }

    int total_tabs = GUI_NUM_TABS;
    int tab_w = 110;
    int tab_h = 24;
    int tab_gap = 6;
    int start_y = pnl->y + (pnl->height > tab_h ? (pnl->height - tab_h) / 2 : 0);
    int start_x = pnl->x + 240;

    if (cy < start_y || cy > start_y + tab_h) {
        return false;
    }

    for (int i = 0; i < total_tabs; i++) {
        int tx = start_x + i * (tab_w + tab_gap);
        if (cx >= tx && cx <= tx + tab_w) {
            *out_screen = (gui_screen_mode_t)i;
            return true;
        }
    }
    return false;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Full-Canvas Alternate Screen Rendering Cogs
--------------------------------------------------------------------------------
Full-page presentation renderers for Status Dossier, Inventory, World Map, and Chronicle.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders the grand full-canvas Protagonist Hero Dossier & Attributes Matrix.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the complete character sheet: large heraldic crest sprite, heroic lineage,
 * HP/MP/TP bars, attributes (ATK/DEF/MAT/MDF), relic power, and campaign achievements.
 *
 * ### For Systems Programmers & Engine Architects:
 * Three-column partition of `area`. Renders hero crest via `gui_assets_draw_sprite`,
 * vitals via `gui_widget_draw_meter`, and formatted attribute strings.
 */
void gui_screen_render_status_dossier(void *display, unsigned long dr, void *gc,
                                      const gui_panel_rect_t *area,
                                      const player_state_t *state,
                                      const gui_palette_t *pal,
                                      const void *fonts)
{
    if (display == NULL || area == NULL || state == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    /* Fill background and frame */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, area->x, area->y, area->width, area->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, area->x, area->y, area->width, area->height);

    /* Three-column balanced partitioning */
    int pad = 24;
    int gap = 20;
    int avail_w = area->width - (2 * pad) - (2 * gap);
    int col_w = avail_w / 3;

    int col1_x = area->x + pad;
    int col2_x = col1_x + col_w + gap;
    int col3_x = col2_x + col_w + gap;
    int top_y  = area->y + 36;

    /* -------------------------------------------------------------------------
     * COLUMN 1: Hero Identity & Heraldry
     * ---------------------------------------------------------------------- */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_TITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, col1_x, top_y, state->name,
                (int)strlen(state->name));

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->text_muted);
    XDrawString(d, draw, g, col1_x, top_y + 30, state->title,
                (int)strlen(state->title));

    /* Large Hero Heraldry Crest Box */
    int crest_box_y = top_y + 48;
    int crest_size = 140;
    XSetForeground(d, g, pal->bg);
    XFillRectangle(d, draw, g, col1_x, crest_box_y, crest_size, crest_size);
    XSetForeground(d, g, pal->gold_bright);
    XDrawRectangle(d, draw, g, col1_x, crest_box_y, crest_size, crest_size);

    /* Draw hero sprite in heraldic crest */
    gui_assets_draw_sprite(d, draw, g, ASSET_HERO_ELIJAH,
                           col1_x + (crest_size / 2),
                           crest_box_y + (crest_size / 2) + 10,
                           96, 0, pal);

    int id_y = crest_box_y + crest_size + 30;
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, col1_x, id_y, "--- Heroic Lineage ---", 23);
    id_y += 28;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char buf[128];
    snprintf(buf, sizeof(buf), "Class: %s",
             sim_state_get_class_name(state->active_class));
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, col1_x, id_y, buf, (int)strlen(buf));
    id_y += 24;

    snprintf(buf, sizeof(buf), "Relic: %s",
             sim_state_get_relic_name(state->relic));
    XDrawString(d, draw, g, col1_x, id_y, buf, (int)strlen(buf));
    id_y += 24;

    snprintf(buf, sizeof(buf), "Origin: %s", state->former_tutelage);
    XSetForeground(d, g, pal->text_muted);
    XDrawString(d, draw, g, col1_x, id_y, buf, (int)strlen(buf));

    /* -------------------------------------------------------------------------
     * COLUMN 2: Vitals & Core Combat Attributes Matrix
     * ---------------------------------------------------------------------- */
    int cur2_y = top_y;
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_TITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, col2_x, cur2_y, "Vitals & Reserves", 17);
    cur2_y += 36;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    /* HP Meter */
    char hp_str[64];
    snprintf(hp_str, sizeof(hp_str), "Health (HP): %d / %d",
             state->hp, state->max_hp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, col2_x, cur2_y, hp_str, (int)strlen(hp_str));
    cur2_y += 10;
    gui_widget_draw_meter(d, draw, g, col2_x, cur2_y, col_w, 20,
                          state->hp, state->max_hp,
                          pal->virtue_green, pal->bg, pal->border);
    cur2_y += 36;

    /* MP Meter */
    char mp_str[64];
    snprintf(mp_str, sizeof(mp_str), "Mana (MP): %d / %d",
             state->mp, state->max_mp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, col2_x, cur2_y, mp_str, (int)strlen(mp_str));
    cur2_y += 10;
    gui_widget_draw_meter(d, draw, g, col2_x, cur2_y, col_w, 20,
                          state->mp, state->max_mp,
                          pal->tactical_move, pal->bg, pal->border);
    cur2_y += 36;

    /* TP Meter */
    char tp_str[64];
    snprintf(tp_str, sizeof(tp_str), "Tactical Points (TP): %d / %d",
             state->tp, state->max_tp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, col2_x, cur2_y, tp_str, (int)strlen(tp_str));
    cur2_y += 10;
    gui_widget_draw_meter(d, draw, g, col2_x, cur2_y, col_w, 20,
                          state->tp, state->max_tp,
                          pal->gold_bright, pal->bg, pal->border);
    cur2_y += 44;

    /* Attributes Subheader */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, col2_x, cur2_y,
                "--- Martial & Arcane Power ---", 30);
    cur2_y += 30;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char stat1[64];
    snprintf(stat1, sizeof(stat1), "Physical Attack (ATK):  %d", state->atk);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, col2_x, cur2_y, stat1, (int)strlen(stat1));
    cur2_y += 24;

    char stat2[64];
    snprintf(stat2, sizeof(stat2), "Physical Defense (DEF): %d", state->def);
    XDrawString(d, draw, g, col2_x, cur2_y, stat2, (int)strlen(stat2));
    cur2_y += 24;

    char stat3[64];
    snprintf(stat3, sizeof(stat3), "Magic Attack (MAT):     %d", state->mat);
    XDrawString(d, draw, g, col2_x, cur2_y, stat3, (int)strlen(stat3));
    cur2_y += 24;

    char stat4[64];
    snprintf(stat4, sizeof(stat4), "Magic Defense (MDF):    %d", state->mdf);
    XDrawString(d, draw, g, col2_x, cur2_y, stat4, (int)strlen(stat4));
    cur2_y += 24;

    char move_str[64];
    snprintf(move_str, sizeof(move_str), "Battlefield Move (MP):  %d Tiles",
             state->movement_points);
    XSetForeground(d, g, pal->gold_bright);
    XDrawString(d, draw, g, col2_x, cur2_y, move_str, (int)strlen(move_str));

    /* -------------------------------------------------------------------------
     * COLUMN 3: Twin Relics Continuum & Campaign Honor
     * ---------------------------------------------------------------------- */
    int cur3_y = top_y;
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_TITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, col3_x, cur3_y, "Twin Relics Resonance", 21);
    cur3_y += 36;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    /* Shield of Lux */
    char s_str[64];
    snprintf(s_str, sizeof(s_str), "Shield of Lux: %d / 100 (Barrier Power)",
             state->shield_power);
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, col3_x, cur3_y, s_str, (int)strlen(s_str));
    cur3_y += 10;
    gui_widget_draw_meter(d, draw, g, col3_x, cur3_y, col_w, 20,
                          state->shield_power, MAX_RESONANCE,
                          pal->shield_gold, pal->bg, pal->border);
    cur3_y += 36;

    /* Sword of Tenebris */
    char t_str[64];
    snprintf(t_str, sizeof(t_str), "Sword of Tenebris: %d / 100 (Edge Mist)",
             state->sword_edge);
    XSetForeground(d, g, pal->sword_violet);
    XDrawString(d, draw, g, col3_x, cur3_y, t_str, (int)strlen(t_str));
    cur3_y += 10;
    gui_widget_draw_meter(d, draw, g, col3_x, cur3_y, col_w, 20,
                          state->sword_edge, MAX_RESONANCE,
                          pal->sword_violet, pal->bg, pal->border);
    cur3_y += 36;

    /* Moral Sway Continuum */
    char sway_desc[64];
    snprintf(sway_desc, sizeof(sway_desc), "Moral Sway: %+d [%s]",
             state->moral_sway, game_get_sway_desc(state->moral_sway));
    unsigned long scol = (state->moral_sway > 0) ? pal->virtue_green :
        ((state->moral_sway < 0) ? pal->malice_red : pal->text_muted);
    XSetForeground(d, g, scol);
    XDrawString(d, draw, g, col3_x, cur3_y, sway_desc, (int)strlen(sway_desc));
    cur3_y += 10;

    int sway_val = state->moral_sway + 100;
    gui_widget_draw_meter(d, draw, g, col3_x, cur3_y, col_w, 14,
                          sway_val, 200, scol, pal->bg, pal->border);
    cur3_y += 44;

    /* Campaign Honor Record */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, col3_x, cur3_y,
                "--- Campaign Honor Record ---", 29);
    cur3_y += 30;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char rec1[64], rec2[64], rec3[64], rec4[64];
    snprintf(rec1, sizeof(rec1), "Foes Vanquished:    %d",
             state->foes_vanquished);
    snprintf(rec2, sizeof(rec2), "Innocents Warded:  %d",
             state->innocents_warded);
    snprintf(rec3, sizeof(rec3), "Tactical Turns:     %d", state->turns_taken);
    snprintf(rec4, sizeof(rec4), "Kingdom Treasury:   %d Gold (G)",
             state->gold);

    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, col3_x, cur3_y, rec1, (int)strlen(rec1));
    cur3_y += 24;
    XDrawString(d, draw, g, col3_x, cur3_y, rec2, (int)strlen(rec2));
    cur3_y += 24;
    XDrawString(d, draw, g, col3_x, cur3_y, rec3, (int)strlen(rec3));
    cur3_y += 24;
    XSetForeground(d, g, pal->gold_bright);
    XDrawString(d, draw, g, col3_x, cur3_y, rec4, (int)strlen(rec4));
}

/**
 * @brief Renders the 32-Slot Inventory, Consumables & Key Relic Inspection screen.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the player's 32-slot inventory bag arranged in a 4x8 grid on the left,
 * with a detailed inspect window and lore card for selected relics on the right.
 *
 * ### For Systems Programmers & Engine Architects:
 * Grid rasterization drawing 32 item cells with indices and names, paired with
 * an artifact detail card displaying category, origin, and purse gold. Zero heap.
 */
void gui_screen_render_inventory(void *display, unsigned long dr, void *gc,
                                 const gui_panel_rect_t *area,
                                 const player_state_t *state,
                                 const gui_palette_t *pal,
                                 const void *fonts)
{
    if (display == NULL || area == NULL || state == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    /* Fill background */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, area->x, area->y, area->width, area->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, area->x, area->y, area->width, area->height);

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_TITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, area->x + 24, area->y + 36,
                "INVENTORY & REPLICATED ARTIFACTS", 32);

    /* Left side: 32-slot inventory bag grid */
    int grid_x = area->x + 24;
    int grid_y = area->y + 60;
    int cols = 4;
    int rows = 8;
    int cell_w = 120;
    int cell_h = 44;

    const char *sample_items[3] = {
        "Grandma's Watch",
        "Sacyran Herb",
        "Phial of Light"
    };

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }

    int idx = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int cx = grid_x + (c * (cell_w + 10));
            int cy = grid_y + (r * (cell_h + 8));

            XSetForeground(d, g, (idx < 3) ? pal->btn_hover : pal->bg);
            XFillRectangle(d, draw, g, cx, cy, cell_w, cell_h);
            XSetForeground(d, g, (idx < 3) ? pal->gold_bright : pal->border);
            XDrawRectangle(d, draw, g, cx, cy, cell_w, cell_h);

            char num_str[16];
            snprintf(num_str, sizeof(num_str), "#%02d", idx + 1);
            XSetForeground(d, g, pal->text_muted);
            XDrawString(d, draw, g, cx + 6, cy + 16, num_str,
                        (int)strlen(num_str));

            if (idx < 3) {
                XSetForeground(d, g, pal->text_main);
                XDrawString(d, draw, g, cx + 6, cy + 34, sample_items[idx],
                            (int)strlen(sample_items[idx]));
            }
            idx++;
        }
    }

    /* Right side: Detailed Artifact Dossier */
    int card_x = grid_x + (cols * (cell_w + 10)) + 30;
    int card_w = area->width - card_x + area->x - 24;
    if (card_w > 120) {
        XSetForeground(d, g, pal->bg);
        XFillRectangle(d, draw, g, card_x, grid_y, card_w, 420);
        XSetForeground(d, g, pal->gold_bright);
        XDrawRectangle(d, draw, g, card_x, grid_y, card_w, 420);

        if (cat != NULL) {
            gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
        }
        XSetForeground(d, g, pal->shield_gold);
        XDrawString(d, draw, g, card_x + 20, grid_y + 36,
                    "Artifact: Grandma's Watch", 25);

        if (cat != NULL) {
            gui_fonts_apply(d, g, cat, GUI_FONT_STORY);
        }
        XSetForeground(d, g, pal->text_main);
        XDrawString(d, draw, g, card_x + 20, grid_y + 70,
                    "Category: Sovereign Key Quest Relic", 34);
        XDrawString(d, draw, g, card_x + 20, grid_y + 100,
                    "Origin:   Inherited in Dio Village.", 35);
        XDrawString(d, draw, g, card_x + 20, grid_y + 130,
                    "Canon:    Conceals calibrated Dream Compass.", 45);
        XDrawString(d, draw, g, card_x + 20, grid_y + 160,
                    "Power:    Pierces planar illusion of Capital.", 45);

        /* Gold & Capacity bar */
        char g_str[64];
        snprintf(g_str, sizeof(g_str), "Purse: %d G  |  Slots: 3/32",
                 state->gold);
        XSetForeground(d, g, pal->gold_bright);
        XDrawString(d, draw, g, card_x + 20, grid_y + 360,
                    g_str, (int)strlen(g_str));
    }
}

/**
 * @brief Renders the Provincial Topographical Realm Map of Sacyra and Libira.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the world map showing major kingdom locations connected by cobblestone
 * trade roads, with glowing beacon rings marking the player's current location.
 *
 * ### For Systems Programmers & Engine Architects:
 * Draws connected provincial graph nodes and road lines across Sacyra, Libira,
 * and the Capital. Renders beacon rings for `state->location`. Zero heap.
 */
void gui_screen_render_world_map(void *display, unsigned long dr, void *gc,
                                 const gui_panel_rect_t *area,
                                 const player_state_t *state,
                                 const gui_palette_t *pal,
                                 const void *fonts)
{
    if (display == NULL || area == NULL || state == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    /* Fill background */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, area->x, area->y, area->width, area->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, area->x, area->y, area->width, area->height);

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_TITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, area->x + 24, area->y + 36,
                "PROVINCIAL REALM TOPOLOGY", 25);

    /* 6 Provincial Nodes coordinates */
    struct {
        const char *name;
        int x;
        int y;
        location_id_t loc;
    } nodes[6] = {
        { "The Dreamscape",    area->x + 100, area->y + 120,
          LOC_THE_DREAMSCAPE },
        { "Dio Village",       area->x + 240, area->y + 240,
          LOC_DIO_VILLAGE },
        { "Whispering Woods",  area->x + 420, area->y + 180,
          LOC_WHISPERING_WOODS },
        { "Sacyra Marches",    area->x + 580, area->y + 260,
          LOC_SACYRA_MARCHES },
        { "The Royal Capital", area->x + 760, area->y + 160,
          LOC_ROYAL_CAPITAL },
        { "Kingdom of Libira", area->x + 940, area->y + 280,
          LOC_KINGDOM_OF_LIBIRA }
    };

    /* Draw trade roads between consecutive provincial nodes */
    XSetForeground(d, g, pal->tile_cobble);
    for (int i = 0; i < 5; i++) {
        XDrawLine(d, draw, g, nodes[i].x + 20, nodes[i].y + 20,
                  nodes[i + 1].x + 20, nodes[i + 1].y + 20);
    }

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }

    /* Draw province nodes */
    for (int i = 0; i < 6; i++) {
        bool is_current = (state->location == nodes[i].loc);

        XSetForeground(d, g, is_current ? pal->gold_bright : pal->btn_bg);
        XFillArc(d, draw, g, nodes[i].x, nodes[i].y, 36, 36, 0, 360 * 64);
        XSetForeground(d, g, is_current ? pal->shield_gold : pal->border);
        XDrawArc(d, draw, g, nodes[i].x, nodes[i].y, 36, 36, 0, 360 * 64);

        if (is_current) {
            /* Radiant beacon rings */
            XDrawArc(d, draw, g, nodes[i].x - 6, nodes[i].y - 6,
                     48, 48, 0, 360 * 64);
        }

        XSetForeground(d, g, is_current ? pal->gold_bright : pal->text_main);
        XDrawString(d, draw, g, nodes[i].x - 20, nodes[i].y + 54,
                    nodes[i].name, (int)strlen(nodes[i].name));
    }
}

/**
 * @brief Renders the full-canvas Historical Deeds Chronicle & Accord Lore Codex.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the entire campaign narrative log across a wide page, allowing the
 * player to review past storyline events, dialogues, and battle triumphs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders up to `max_visible` chronicle lines from `gui_log_t` with custom line
 * spacing and radiant highlight on the newest entry. Zero heap.
 */
void gui_screen_render_chronicle_codex(void *display, unsigned long dr,
                                       void *gc,
                                       const gui_panel_rect_t *area,
                                       const gui_log_t *log,
                                       const gui_palette_t *pal,
                                       const void *fonts)
{
    if (display == NULL || area == NULL || log == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    /* Fill background */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, area->x, area->y, area->width, area->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, area->x, area->y, area->width, area->height);

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_TITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, area->x + 24, area->y + 36,
                "CHRONICLES OF SACYRA & DEEDS CODEX", 34);

    const int line_h = 28;
    int max_visible = (area->height - 70) / line_h;
    if (max_visible < 1) max_visible = 1;

    int start_idx = log->count - max_visible;
    if (start_idx < 0) start_idx = 0;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_STORY);
    }

    int rendered = 0;
    for (int i = start_idx; i < log->count; i++) {
        unsigned long fg = (i == log->count - 1) ?
            pal->gold_bright : pal->text_main;
        XSetForeground(d, g, fg);
        int dy = area->y + 70 + (rendered * line_h);
        XDrawString(d, draw, g, area->x + 24, dy,
                    log->lines[i], (int)strlen(log->lines[i]));
        rendered++;
    }
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
 * @brief Audits alternate screen tab geometry and hit-test boundaries.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the screen tab bar to verify that clicks outside the tabs are ignored,
 * and clicks on Tab 0 reliably switch to the Tactical screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies tab click hit-testing bounds and mode resolution invariants.
 */
bool gui_screens_audit(void)
{
    gui_panel_rect_t pnl = { 10, 10, 840, 32, "Header" };
    gui_screen_mode_t mode;

    /* Click outside tabs returns false */
    if (gui_screens_handle_tab_click(&pnl, 5, 5, &mode)) {
        return false;
    }

    /* Click inside Tab 0 (Tactical [F1]) returns true: start_x=250, y=20 */
    if (!gui_screens_handle_tab_click(&pnl, 260, 20, &mode) ||
        mode != GUI_SCREEN_TACTICAL) {
        return false;
    }

    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Preflight verification for alternate screens and navigation tab bar.
 *
 * ### For Game Designers & Non-Programmers:
 * Validates that all 5 screen navigation tabs (F1 through F5) respond accurately.
 *
 * ### For Systems Programmers & Engine Architects:
 * Preflight test cycling through all 5 tabs verifying center point hitboxes.
 */
bool gui_screens_preflight(void)
{
    if (!gui_screens_audit()) {
        return false;
    }

    gui_panel_rect_t pnl = { 10, 10, 840, 32, "Header" };
    gui_screen_mode_t mode;

    /* Test all 5 tabs */
    int tab_w = 110;
    int tab_gap = 6;
    int start_x = pnl.x + 240;
    for (int i = 0; i < GUI_NUM_TABS; i++) {
        int cx = start_x + (i * (tab_w + tab_gap)) + (tab_w / 2);
        int cy = pnl.y + (pnl.height / 2);
        if (!gui_screens_handle_tab_click(&pnl, cx, cy, &mode)) {
            return false;
        }
        if ((int)mode != i) {
            return false;
        }
    }

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Alternate Screen Renderers
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
 * @brief Formats an ASCII diagnostic dump card representing Alternate Screens subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing alternate screens status, navigation tab count,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 */
int gui_screens_execute_dump_card(char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    bool preflight_ok = gui_screens_preflight();

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT SCREENS SUBSYSTEM DIAGNOSTIC CARD                \n"
        "============================================================\n"
        " Subsystem       : Alternate Screen Presentation Engine     \n"
        " Status          : %-41s\n"
        " Primary Screens : %-41d\n"
        " Screens Included: %-41s\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        "OPERATIONAL",
        GUI_NUM_TABS,
        "TACTICAL, STATUS, BAG, MAP, CODEX",
        preflight_ok ? "PASSED" : "FAILED");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/
