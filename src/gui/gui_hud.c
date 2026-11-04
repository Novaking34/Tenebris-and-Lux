// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_hud.c
// @id            gui.hud.display
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implements Heads-Up Display (HUD) rendering, hero vitals inspection,
//   dual relic meters, moral sway continuum visualizer, and narrative log.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_hud.h, gui/gui_widgets.h, sim/core/game.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Presentation read-only pipeline).
//   - Latency Budget:        < 2000 microseconds per frame render.
//
// AXIOMATIC INVARIANTS:
//   1. Ring buffer shifts oldest elements upward in FIFO order.
//   2. Meter graphics saturate cleanly at boundaries.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_hud.h"
#include "gui/gui_widgets.h"
#include "sim/core/game.h"

#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

void gui_hud_log_append(gui_log_t *log, const char *msg)
{
    if (log == NULL || msg == NULL || strlen(msg) == 0) {
        return;
    }

    if (log->count < GUI_MAX_LOG_LINES) {
        strncpy(log->lines[log->count], msg, GUI_LOG_ENTRY_LEN - 1);
        log->lines[log->count][GUI_LOG_ENTRY_LEN - 1] = '\0';
        log->count++;
    } else {
        /* Shift older entries upward */
        for (int i = 0; i < GUI_MAX_LOG_LINES - 1; i++) {
            strncpy(log->lines[i], log->lines[i + 1], GUI_LOG_ENTRY_LEN);
        }
        strncpy(log->lines[GUI_MAX_LOG_LINES - 1], msg,
                GUI_LOG_ENTRY_LEN - 1);
        log->lines[GUI_MAX_LOG_LINES - 1][GUI_LOG_ENTRY_LEN - 1] = '\0';
    }
}

void gui_hud_render_header(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const gui_palette_t *pal)
{
    if (display == NULL || pnl == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    /* Fill background and border */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);

    /* Title string */
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, pnl->x + 20, pnl->y + 28,
                "TENEBRIS AND LUX  [v0.0.1]", 26);

    /* Subtitle string */
    XSetForeground(d, g, pal->text_muted);
    XDrawString(d, draw, g, pnl->x + 240, pnl->y + 28,
                "- Tactical TRPG & 2.5D Presentation Viewport", 44);

    /* Dynastic cycle tag */
    XSetForeground(d, g, pal->gold_bright);
    XDrawString(d, draw, g, pnl->x + pnl->width - 150, pnl->y + 28,
                "Era VI: Accord", 14);
}

void gui_hud_render_status(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const player_state_t *state,
                           const gui_palette_t *pal)
{
    if (display == NULL || pnl == NULL || state == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    /* Panel background and rim */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);

    /* Section 1: Hero Identity */
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 22,
                "--- Protagonist Vitals ---", 26);

    char name_buf[64];
    snprintf(name_buf, sizeof(name_buf), "Hero: %s", state->name);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 44,
                name_buf, strlen(name_buf));

    /* Health (HP) meter */
    char hp_str[64];
    snprintf(hp_str, sizeof(hp_str), "HP: %d / %d", state->hp, state->max_hp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 64, hp_str, strlen(hp_str));

    gui_widget_draw_meter(d, dr, g, pnl->x + 14, pnl->y + 70,
                          pnl->width - 28, 12,
                          state->hp, state->max_hp,
                          pal->virtue_green, pal->bg, pal->border);

    /* Mana (MP) meter */
    char mp_str[64];
    snprintf(mp_str, sizeof(mp_str), "MP: %d / %d", state->mp, state->max_mp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 100, mp_str, strlen(mp_str));

    gui_widget_draw_meter(d, dr, g, pnl->x + 14, pnl->y + 106,
                          pnl->width - 28, 12,
                          state->mp, state->max_mp,
                          pal->tactical_move, pal->bg, pal->border);

    /* Section 2: Twin Relics Continuum */
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 138,
                "--- Twin Relics Resonance ---", 29);

    /* Shield of Lux */
    char s_label[64];
    snprintf(s_label, sizeof(s_label), "Shield Lux: %d/100",
             state->shield_power);
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 158,
                s_label, strlen(s_label));

    gui_widget_draw_meter(d, dr, g, pnl->x + 14, pnl->y + 164,
                          pnl->width - 28, 12,
                          state->shield_power, MAX_RESONANCE,
                          pal->shield_gold, pal->bg, pal->border);

    /* Sword of Tenebris */
    char t_label[64];
    snprintf(t_label, sizeof(t_label), "Sword Tenebris: %d/100",
             state->sword_edge);
    XSetForeground(d, g, pal->sword_violet);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 194,
                t_label, strlen(t_label));

    gui_widget_draw_meter(d, dr, g, pnl->x + 14, pnl->y + 200,
                          pnl->width - 28, 12,
                          state->sword_edge, MAX_RESONANCE,
                          pal->sword_violet, pal->bg, pal->border);

    /* Section 3: Stance & Moral Sway */
    char stance_str[64];
    snprintf(stance_str, sizeof(stance_str), "Stance: %s",
             game_get_stance_name(state->stance));
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 232,
                stance_str, strlen(stance_str));

    char sway_str[64];
    snprintf(sway_str, sizeof(sway_str), "Moral Sway: %+d [%s]",
             state->moral_sway, game_get_sway_desc(state->moral_sway));
    unsigned long sway_col = pal->text_muted;
    if (state->moral_sway > 0) {
        sway_col = pal->virtue_green;
    } else if (state->moral_sway < 0) {
        sway_col = pal->malice_red;
    }
    XSetForeground(d, g, sway_col);
    XDrawString(d, draw, g, pnl->x + 14, pnl->y + 252,
                sway_str, strlen(sway_str));

    /* Section 4: Threat status */
    if (state->threat_hp > 0) {
        char threat_str[64];
        snprintf(threat_str, sizeof(threat_str),
                 "[!] Vanguard Threat: %d HP", state->threat_hp);
        XSetForeground(d, g, pal->malice_red);
        XDrawString(d, draw, g, pnl->x + 14, pnl->y + 284,
                    threat_str, strlen(threat_str));

        gui_widget_draw_meter(d, dr, g, pnl->x + 14, pnl->y + 292,
                              pnl->width - 28, 10,
                              state->threat_hp, 100,
                              pal->malice_red, pal->bg, pal->border);
    } else {
        char honor_str[64];
        snprintf(honor_str, sizeof(honor_str),
                 "Secured | Turns: %d", state->turns_taken);
        XSetForeground(d, g, pal->text_muted);
        XDrawString(d, draw, g, pnl->x + 14, pnl->y + 284,
                    honor_str, strlen(honor_str));
    }
}

void gui_hud_render_chronicle(void *display, unsigned long dr, void *gc,
                              const gui_panel_rect_t *pnl,
                              const gui_log_t *log,
                              const gui_palette_t *pal)
{
    if (display == NULL || pnl == NULL || log == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    /* Fill background and border */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);

    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, pnl->x + 16, pnl->y + 22,
                "--- Chronicle of Deeds ---", 26);

    for (int i = 0; i < log->count; i++) {
        unsigned long fg = (i == log->count - 1) ?
            pal->gold_bright : pal->text_main;
        XSetForeground(d, g, fg);
        XDrawString(d, draw, g, pnl->x + 16, pnl->y + 46 + (i * 20),
                    log->lines[i], strlen(log->lines[i]));
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

bool gui_hud_audit(void)
{
    gui_log_t log;
    log.count = 0;

    /* Append up to capacity */
    for (int i = 0; i < GUI_MAX_LOG_LINES; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Test Event #%d", i);
        gui_hud_log_append(&log, buf);
    }

    if (log.count != GUI_MAX_LOG_LINES) {
        return false;
    }
    if (strcmp(log.lines[0], "Test Event #0") != 0) {
        return false;
    }

    /* Overflow append: FIFO shift invariance */
    gui_hud_log_append(&log, "Overflow Event #6");
    if (log.count != GUI_MAX_LOG_LINES) {
        return false;
    }
    if (strcmp(log.lines[0], "Test Event #1") != 0) {
        return false;
    }
    if (strcmp(log.lines[GUI_MAX_LOG_LINES - 1], "Overflow Event #6") != 0) {
        return false;
    }

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

