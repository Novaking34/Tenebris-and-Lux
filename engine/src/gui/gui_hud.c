// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_hud.c
// @id            gui.hud.display
// @layer         Layer 2: Presentation & Viewport
// @version       1.2.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Heads-Up Display (HUD) presentation routines. Renders heroic title banner,
//   protagonist vitals dossier, combat attributes, twin relics resonance,
//   active wards/buffs, threat telemetry, and multi-line narrative chronicle
//   using High Fantasy typography.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_hud.h, gui/gui_widgets.h, gui/gui_fonts.h,
//                            sim/core/game.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Presentation read-only pipeline).
//   - Latency Budget:        < 2000 microseconds per frame render.
//
// AXIOMATIC INVARIANTS:
//   1. Ring buffer shifts oldest elements upward in FIFO order.
//   2. Meter graphics saturate cleanly at boundaries.
//   3. Chronicle smoothly displays most recent visible narrative events.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_hud.h"
#include "gui/gui_widgets.h"
#include "gui/gui_fonts.h"
#include "sim/core/game.h"
#include "sim/core/sim_state.h"

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
                           const gui_palette_t *pal,
                           const void *fonts)
{
    if (display == NULL || pnl == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    /* Fill background, border, and inner filigree rim */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);

    XSetForeground(d, g, pal->tile_grid);
    XDrawRectangle(d, draw, g, pnl->x + 2, pnl->y + 2,
                   pnl->width - 4, pnl->height - 4);

    /* Title string: Heroic High Fantasy Titling */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_TITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, pnl->x + 20, pnl->y + 32,
                "TENEBRIS AND LUX", 17);

    /* Subtitle string */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->text_muted);
    int sub_offset = (cat != NULL) ?
        gui_fonts_measure_width(cat, GUI_FONT_TITLE, "TENEBRIS AND LUX ") : 220;
    XDrawString(d, draw, g, pnl->x + 20 + sub_offset + 10, pnl->y + 30,
                "— Tactical TRPG & Hero's Journey", 33);

    /* Dynastic cycle era banner */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    XSetForeground(d, g, pal->gold_bright);
    const char *era_str = "[Era VI: Accord of Kings]";
    int era_len = (int)strlen(era_str);
    int era_w = (cat != NULL) ?
        gui_fonts_measure_width(cat, GUI_FONT_UI, era_str) : (era_len * 9);
    int era_x = pnl->x + pnl->width - era_w - 20;
    if (era_x < pnl->x + 550) {
        era_x = pnl->x + 550;
    }
    XDrawString(d, draw, g, era_x, pnl->y + 30, era_str, era_len);
}

static void render_status_expanded(Display *d, Drawable draw, GC g,
                                   const gui_panel_rect_t *pnl,
                                   const player_state_t *state,
                                   const gui_palette_t *pal,
                                   const gui_font_catalog_t *cat)
{
    int px = pnl->x + 16;
    int cur_y = pnl->y + 24;
    int bar_w = pnl->width - 32;

    /* Section 1: Hero Identity */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, "--- Protagonist Dossier ---", 27);
    cur_y += 24;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char name_buf[96];
    snprintf(name_buf, sizeof(name_buf), "%s  |  %s",
             state->name, state->title);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, name_buf, (int)strlen(name_buf));
    cur_y += 20;

    char class_buf[96];
    snprintf(class_buf, sizeof(class_buf), "Class: %s  |  Relic: %s",
             sim_state_get_class_name(state->active_class),
             sim_state_get_relic_name(state->relic));
    XSetForeground(d, g, pal->text_muted);
    XDrawString(d, draw, g, px, cur_y, class_buf, (int)strlen(class_buf));
    cur_y += 26;

    /* Section 2: Vitals & Energy Pools */
    char hp_str[64];
    snprintf(hp_str, sizeof(hp_str), "Health (HP): %d / %d",
             state->hp, state->max_hp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, hp_str, (int)strlen(hp_str));
    cur_y += 8;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->hp, state->max_hp,
                          pal->virtue_green, pal->bg, pal->border);
    cur_y += 22;

    char mp_str[64];
    snprintf(mp_str, sizeof(mp_str), "Mana (MP): %d / %d",
             state->mp, state->max_mp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, mp_str, (int)strlen(mp_str));
    cur_y += 8;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->mp, state->max_mp,
                          pal->tactical_move, pal->bg, pal->border);
    cur_y += 22;

    char tp_str[64];
    snprintf(tp_str, sizeof(tp_str), "Tactical (TP): %d / %d",
             state->tp, state->max_tp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, tp_str, (int)strlen(tp_str));
    cur_y += 8;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->tp, state->max_tp,
                          pal->gold_bright, pal->bg, pal->border);
    cur_y += 26;

    /* Section 3: Combat Attributes */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, "--- Combat Attributes ---", 25);
    cur_y += 22;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char stat_buf[96];
    snprintf(stat_buf, sizeof(stat_buf),
             "ATK: %d   DEF: %d   MAT: %d   MDF: %d",
             state->atk, state->def, state->mat, state->mdf);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, stat_buf, (int)strlen(stat_buf));
    cur_y += 18;

    char move_buf[64];
    snprintf(move_buf, sizeof(move_buf), "Tactical Move: %d Tiles (MP)",
             state->movement_points);
    XSetForeground(d, g, pal->text_muted);
    XDrawString(d, draw, g, px, cur_y, move_buf, (int)strlen(move_buf));
    cur_y += 26;

    /* Section 4: Twin Relics Continuum */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, "--- Twin Relics Resonance ---", 29);
    cur_y += 22;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char s_label[64];
    snprintf(s_label, sizeof(s_label), "Shield of Lux: %d/100 (Barrier)",
             state->shield_power);
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, s_label, (int)strlen(s_label));
    cur_y += 8;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->shield_power, MAX_RESONANCE,
                          pal->shield_gold, pal->bg, pal->border);
    cur_y += 22;

    char t_label[64];
    snprintf(t_label, sizeof(t_label), "Sword of Tenebris: %d/100 (Edge)",
             state->sword_edge);
    XSetForeground(d, g, pal->sword_violet);
    XDrawString(d, draw, g, px, cur_y, t_label, (int)strlen(t_label));
    cur_y += 8;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->sword_edge, MAX_RESONANCE,
                          pal->sword_violet, pal->bg, pal->border);
    cur_y += 26;

    /* Section 5: Stance & Moral Sway */
    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, "--- Stance & Cosmic Balance ---", 31);
    cur_y += 22;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char stance_str[64];
    snprintf(stance_str, sizeof(stance_str), "Stance: %s",
             game_get_stance_name(state->stance));
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, stance_str, (int)strlen(stance_str));
    cur_y += 18;

    char sway_str[64];
    snprintf(sway_str, sizeof(sway_str), "Moral Sway: %+d [%s]",
             state->moral_sway, game_get_sway_desc(state->moral_sway));
    unsigned long sway_col = (state->moral_sway > 0) ? pal->virtue_green :
        ((state->moral_sway < 0) ? pal->malice_red : pal->text_muted);
    XSetForeground(d, g, sway_col);
    XDrawString(d, draw, g, px, cur_y, sway_str, (int)strlen(sway_str));
    cur_y += 26;

    /* Section 6: Active Buffs & Wards */
    if (cur_y < pnl->y + pnl->height - 110) {
        if (cat != NULL) {
            gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
        }
        XSetForeground(d, g, pal->shield_gold);
        XDrawString(d, draw, g, px, cur_y, "--- Wards & Active Buffs ---", 28);
        cur_y += 20;

        if (cat != NULL) {
            gui_fonts_apply(d, g, cat, GUI_FONT_UI);
        }
        char buff_buf[96];
        if (state->lux_barrio_turns > 0) {
            snprintf(buff_buf, sizeof(buff_buf),
                     "Lux Barrio: %d Turns Active (-25%% Dmg)",
                     state->lux_barrio_turns);
            XSetForeground(d, g, pal->gold_bright);
        } else {
            snprintf(buff_buf, sizeof(buff_buf), "Lux Barrio: Inactive");
            XSetForeground(d, g, pal->text_muted);
        }
        XDrawString(d, draw, g, px, cur_y, buff_buf, (int)strlen(buff_buf));
        cur_y += 24;
    }

    /* Section 7: Threat Status */
    if (cur_y < pnl->y + pnl->height - 60) {
        if (cat != NULL) {
            gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
        }
        XSetForeground(d, g, pal->shield_gold);
        XDrawString(d, draw, g, px, cur_y, "--- Threat Encounter ---", 24);
        cur_y += 22;

        if (cat != NULL) {
            gui_fonts_apply(d, g, cat, GUI_FONT_UI);
        }
        if (state->threat_hp > 0) {
            char threat_str[64];
            snprintf(threat_str, sizeof(threat_str),
                     "[!] %s: %d / %d HP",
                     state->threat_name, state->threat_hp,
                     state->threat_max_hp);
            XSetForeground(d, g, pal->malice_red);
            XDrawString(d, draw, g, px, cur_y, threat_str,
                        (int)strlen(threat_str));
            cur_y += 8;
            gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 10,
                                  state->threat_hp, 100,
                                  pal->malice_red, pal->bg, pal->border);
        } else {
            char honor_str[96];
            snprintf(honor_str, sizeof(honor_str),
                     "Secured  |  Foes: %d  |  Turns: %d",
                     state->foes_vanquished, state->turns_taken);
            XSetForeground(d, g, pal->text_muted);
            XDrawString(d, draw, g, px, cur_y, honor_str,
                        (int)strlen(honor_str));
        }
    }
}

static void render_status_compact(Display *d, Drawable draw, GC g,
                                  const gui_panel_rect_t *pnl,
                                  const player_state_t *state,
                                  const gui_palette_t *pal,
                                  const gui_font_catalog_t *cat)
{
    int px = pnl->x + 14;
    int cur_y = pnl->y + 22;
    int bar_w = pnl->width - 28;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, "--- Protagonist Vitals ---", 26);
    cur_y += 22;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char name_buf[64];
    snprintf(name_buf, sizeof(name_buf), "Hero: %s", state->name);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, name_buf, (int)strlen(name_buf));
    cur_y += 20;

    char hp_str[64];
    snprintf(hp_str, sizeof(hp_str), "HP: %d / %d", state->hp, state->max_hp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, hp_str, (int)strlen(hp_str));
    cur_y += 6;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->hp, state->max_hp,
                          pal->virtue_green, pal->bg, pal->border);
    cur_y += 24;

    char mp_str[64];
    snprintf(mp_str, sizeof(mp_str), "MP: %d / %d", state->mp, state->max_mp);
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, mp_str, (int)strlen(mp_str));
    cur_y += 6;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->mp, state->max_mp,
                          pal->tactical_move, pal->bg, pal->border);
    cur_y += 28;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, "--- Twin Relics Resonance ---", 29);
    cur_y += 20;

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_UI);
    }
    char s_label[64];
    snprintf(s_label, sizeof(s_label), "Shield Lux: %d/100",
             state->shield_power);
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, px, cur_y, s_label, (int)strlen(s_label));
    cur_y += 6;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->shield_power, MAX_RESONANCE,
                          pal->shield_gold, pal->bg, pal->border);
    cur_y += 24;

    char t_label[64];
    snprintf(t_label, sizeof(t_label), "Sword Tenebris: %d/100",
             state->sword_edge);
    XSetForeground(d, g, pal->sword_violet);
    XDrawString(d, draw, g, px, cur_y, t_label, (int)strlen(t_label));
    cur_y += 6;
    gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 12,
                          state->sword_edge, MAX_RESONANCE,
                          pal->sword_violet, pal->bg, pal->border);
    cur_y += 24;

    char stance_str[64];
    snprintf(stance_str, sizeof(stance_str), "Stance: %s",
             game_get_stance_name(state->stance));
    XSetForeground(d, g, pal->text_main);
    XDrawString(d, draw, g, px, cur_y, stance_str, (int)strlen(stance_str));
    cur_y += 20;

    char sway_str[64];
    snprintf(sway_str, sizeof(sway_str), "Moral Sway: %+d [%s]",
             state->moral_sway, game_get_sway_desc(state->moral_sway));
    unsigned long sway_col = (state->moral_sway > 0) ? pal->virtue_green :
        ((state->moral_sway < 0) ? pal->malice_red : pal->text_muted);
    XSetForeground(d, g, sway_col);
    XDrawString(d, draw, g, px, cur_y, sway_str, (int)strlen(sway_str));
    cur_y += 24;

    if (state->threat_hp > 0) {
        char threat_str[64];
        snprintf(threat_str, sizeof(threat_str),
                 "[!] Threat: %d HP", state->threat_hp);
        XSetForeground(d, g, pal->malice_red);
        XDrawString(d, draw, g, px, cur_y, threat_str,
                    (int)strlen(threat_str));
        cur_y += 6;
        gui_widget_draw_meter(d, draw, g, px, cur_y, bar_w, 10,
                              state->threat_hp, 100,
                              pal->malice_red, pal->bg, pal->border);
    }
}

void gui_hud_render_status(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const player_state_t *state,
                           const gui_palette_t *pal,
                           const void *fonts)
{
    if (display == NULL || pnl == NULL || state == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    /* Panel background and rim */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);

    if (pnl->height >= 500) {
        render_status_expanded(d, draw, g, pnl, state, pal, cat);
    } else {
        render_status_compact(d, draw, g, pnl, state, pal, cat);
    }
}

void gui_hud_render_chronicle(void *display, unsigned long dr, void *gc,
                              const gui_panel_rect_t *pnl,
                              const gui_log_t *log,
                              const gui_palette_t *pal,
                              const void *fonts)
{
    if (display == NULL || pnl == NULL || log == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;
    const gui_font_catalog_t *cat = (const gui_font_catalog_t *)fonts;

    /* Fill background and border */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, pnl->x, pnl->y, pnl->width, pnl->height);

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_SUBTITLE);
    }
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, pnl->x + 16, pnl->y + 24,
                "--- Chronicle of Deeds ---", 26);

    const int line_h = 22;
    int max_visible = (pnl->height - 42) / line_h;
    if (max_visible < 1) {
        max_visible = 1;
    }

    int start_idx = log->count - max_visible;
    if (start_idx < 0) {
        start_idx = 0;
    }

    if (cat != NULL) {
        gui_fonts_apply(d, g, cat, GUI_FONT_STORY);
    }

    int rendered = 0;
    for (int i = start_idx; i < log->count; i++) {
        unsigned long fg = (i == log->count - 1) ?
            pal->gold_bright : pal->text_main;
        XSetForeground(d, g, fg);
        int dy = pnl->y + 48 + (rendered * line_h);
        XDrawString(d, draw, g, pnl->x + 16, dy,
                    log->lines[i], (int)strlen(log->lines[i]));
        rendered++;
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
