// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_view2d.c
// @id            gui.view2d.tactical
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Renders the tactical 2D / 2.5D world viewport. Implements diamond dimetric
//   projection, 3D prism elevation blocks (3-face shaded cubes), depth-sorted
//   Painter's algorithm entity queue, and tactical range overlays.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_view2d.h, gui/gui_assets.h,
//                            gui/gui_palette.h, util/math/math_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static entity queue, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure rendering math).
//   - Latency Budget:        < 5000 microseconds per frame render.
//
// AXIOMATIC INVARIANTS:
//   1. Diamond isometric projection reversibility.
//   2. Entities render strictly back-to-front according to depth key sorting.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_view2d.h"
#include "gui/gui_assets.h"
#include "gui/gui_palette.h"
#include "sim/core/game.h"
#include "util/math/math_utils.h"

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_COLS 8
#define GRID_ROWS 8

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

void gui_iso_project(int gx, int gy, int gz,
                     int origin_x, int origin_y,
                     int tile_w, int tile_h, int elev_scale,
                     int *out_sx, int *out_sy)
{
    if (out_sx == NULL || out_sy == NULL) {
        return;
    }

    int half_w = tile_w / 2;
    int half_h = tile_h / 2;

    *out_sx = origin_x + (gx - gy) * half_w;
    *out_sy = origin_y + (gx + gy) * half_h - (gz * elev_scale);
}

void gui_iso_unproject(int sx, int sy, int gz,
                       int origin_x, int origin_y,
                       int tile_w, int tile_h, int elev_scale,
                       int *out_gx, int *out_gy)
{
    if (out_gx == NULL || out_gy == NULL || tile_w <= 0 || tile_h <= 0) {
        return;
    }

    float fdx = (float)(sx - origin_x);
    float fdy = (float)(sy + (gz * elev_scale) - origin_y);

    float fgx = (fdx / (float)tile_w) + (fdy / (float)tile_h);
    float fgy = (fdy / (float)tile_h) - (fdx / (float)tile_w);

    *out_gx = (int)(fgx >= 0.0f ? (fgx + 0.5f) : (fgx - 0.5f));
    *out_gy = (int)(fgy >= 0.0f ? (fgy + 0.5f) : (fgy - 0.5f));
}

/**
 * Draws a 3D elevation prism (front-left face, front-right face, top diamond).
 */
static void draw_elevation_prism(Display *d, Drawable dr, GC gc,
                                 int sx, int sy, int tile_w, int tile_h,
                                 int gz, int elev_scale,
                                 unsigned long tile_color,
                                 const gui_palette_t *pal)
{
    int half_w = tile_w / 2;
    int half_h = tile_h / 2;
    int drop   = gz * elev_scale;

    /* Front-Left vertical face */
    if (drop > 0) {
        XPoint left_face[4] = {
            { (short)(sx - half_w), (short)(sy + half_h) },
            { (short)sx,            (short)(sy + tile_h) },
            { (short)sx,            (short)(sy + tile_h + drop) },
            { (short)(sx - half_w), (short)(sy + half_h + drop) }
        };
        unsigned long left_col = gui_palette_shade(tile_color, 0.75f);
        XSetForeground(d, gc, left_col);
        XFillPolygon(d, dr, gc, left_face, 4, Convex, CoordModeOrigin);
        XSetForeground(d, gc, pal->border);
        XDrawLines(d, dr, gc, left_face, 4, CoordModeOrigin);

        /* Front-Right vertical face */
        XPoint right_face[4] = {
            { (short)sx,            (short)(sy + tile_h) },
            { (short)(sx + half_w), (short)(sy + half_h) },
            { (short)(sx + half_w), (short)(sy + half_h + drop) },
            { (short)sx,            (short)(sy + tile_h + drop) }
        };
        unsigned long right_col = gui_palette_shade(tile_color, 0.55f);
        XSetForeground(d, gc, right_col);
        XFillPolygon(d, dr, gc, right_face, 4, Convex, CoordModeOrigin);
        XSetForeground(d, gc, pal->border);
        XDrawLines(d, dr, gc, right_face, 4, CoordModeOrigin);
    }

    /* Top diamond face */
    XPoint top_face[4] = {
        { (short)sx,            (short)sy },
        { (short)(sx + half_w), (short)(sy + half_h) },
        { (short)sx,            (short)(sy + tile_h) },
        { (short)(sx - half_w), (short)(sy + half_h) }
    };
    unsigned long top_col = (drop > 0) ?
        gui_palette_shade(tile_color, 1.15f) : tile_color;

    XSetForeground(d, gc, top_col);
    XFillPolygon(d, dr, gc, top_face, 4, Convex, CoordModeOrigin);

    /* Rim grid outline */
    XSetForeground(d, gc, pal->tile_grid);
    XDrawLines(d, dr, gc, top_face, 4, CoordModeOrigin);
    XDrawLine(d, dr, gc, top_face[3].x, top_face[3].y,
              top_face[0].x, top_face[0].y);
}

/**
 * Draws a tactical range diamond overlay.
 */
static void draw_range_diamond(Display *d, Drawable dr, GC gc,
                               int sx, int sy, int tile_w, int tile_h,
                               unsigned long range_color)
{
    int half_w = tile_w / 2;
    int half_h = tile_h / 2;

    XPoint diamond[4] = {
        { (short)sx,                (short)(sy + 2) },
        { (short)(sx + half_w - 3), (short)(sy + half_h) },
        { (short)sx,                (short)(sy + tile_h - 2) },
        { (short)(sx - half_w + 3), (short)(sy + half_h) }
    };

    XSetForeground(d, gc, range_color);
    XDrawLines(d, dr, gc, diamond, 4, CoordModeOrigin);
    XDrawLine(d, dr, gc, diamond[3].x, diamond[3].y,
              diamond[0].x, diamond[0].y);
}

/**
 * Renders the 2.5D Isometric Tactical Grid and Entities.
 */
static void render_isometric_view(Display *d, Drawable dr, GC gc,
                                  const gui_panel_rect_t *vp,
                                  const player_state_t *state,
                                  const gui_palette_t *pal)
{
    const int tile_w     = GUI_ISO_TILE_W;
    const int tile_h     = GUI_ISO_TILE_H;
    const int elev_scale = GUI_ISO_ELEV_SCALE;
    const int origin_x   = vp->x + (vp->width / 2);
    int origin_y         = vp->y + ((vp->height - 192) / 2);
    if (origin_y < vp->y + 40) {
        origin_y = vp->y + 40;
    }

    int hero_gx = 3;
    int hero_gy = 4;
    int threat_gx = 5;
    int threat_gy = 2;

    /* Step 1: Render base isometric tiles and elevation blocks */
    for (int gy = 0; gy < GRID_ROWS; gy++) {
        for (int gx = 0; gx < GRID_COLS; gx++) {
            int gz = 0;
            if (gx == 0 || gy == 0) {
                gz = 1; /* Perimeter rampart elevation */
            } else if (gx == 3 && gy == 3) {
                gz = 1; /* Central altar dais */
            }

            int sx = 0;
            int sy = 0;
            gui_iso_project(gx, gy, gz, origin_x, origin_y,
                            tile_w, tile_h, elev_scale, &sx, &sy);

            unsigned long tile_col = pal->tile_cobble;
            if ((gx + gy) % 2 == 0) {
                tile_col = pal->panel;
            }
            if (gz > 0) {
                tile_col = pal->tile_wall;
            }

            draw_elevation_prism(d, dr, gc, sx, sy, tile_w, tile_h,
                                 gz, elev_scale, tile_col, pal);

            /* Tactical movement overlay around Hero (distance <= 2) */
            int hero_dist = abs(gx - hero_gx) + abs(gy - hero_gy);
            if (hero_dist <= 2 && gz == 0) {
                draw_range_diamond(d, dr, gc, sx, sy, tile_w, tile_h,
                                   pal->tactical_move);
            }

            /* Tactical attack overlay around threat (distance <= 1) */
            if (state->threat_hp > 0) {
                int threat_dist = abs(gx - threat_gx) + abs(gy - threat_gy);
                if (threat_dist <= 1 && gz == 0) {
                    draw_range_diamond(d, dr, gc, sx, sy, tile_w, tile_h,
                                       pal->tactical_atk);
                }
            }
        }
    }

    /* Step 2: Assemble depth-sorted entity queue (Painter's Algorithm) */
    gui_depth_entity_t queue[GUI_MAX_ENTITIES_DEPTH];
    int q_count = 0;

    /* Enqueue Hero (Elijah) */
    if (q_count < GUI_MAX_ENTITIES_DEPTH) {
        int sx = 0;
        int sy = 0;
        gui_iso_project(hero_gx, hero_gy, 0, origin_x, origin_y,
                        tile_w, tile_h, elev_scale, &sx, &sy);
        queue[q_count].depth_key = (hero_gx + hero_gy) * 1000;
        queue[q_count].screen_x  = sx;
        queue[q_count].screen_y  = sy + (tile_h / 2);
        queue[q_count].asset_id  = ASSET_HERO_ELIJAH;
        queue[q_count].facing    = 0;
        queue[q_count].hp        = state->hp;
        queue[q_count].max_hp    = state->max_hp;
        queue[q_count].is_hero   = true;
        q_count++;
    }

    /* Enqueue Active Threat if alive */
    if (state->threat_hp > 0 && q_count < GUI_MAX_ENTITIES_DEPTH) {
        int sx = 0;
        int sy = 0;
        gui_iso_project(threat_gx, threat_gy, 0, origin_x, origin_y,
                        tile_w, tile_h, elev_scale, &sx, &sy);
        queue[q_count].depth_key = (threat_gx + threat_gy) * 1000;
        queue[q_count].screen_x  = sx;
        queue[q_count].screen_y  = sy + (tile_h / 2);
        queue[q_count].asset_id  = ASSET_ENEMY_DREAM_EATER;
        queue[q_count].facing    = 1;
        queue[q_count].hp        = state->threat_hp;
        queue[q_count].max_hp    = 100;
        queue[q_count].is_hero   = false;
        q_count++;
    }

    /* Insertion sort queue by depth_key */
    for (int i = 1; i < q_count; i++) {
        gui_depth_entity_t key = queue[i];
        int j = i - 1;
        while (j >= 0 && queue[j].depth_key > key.depth_key) {
            queue[j + 1] = queue[j];
            j--;
        }
        queue[j + 1] = key;
    }

    /* Step 3: Draw sorted entities */
    for (int i = 0; i < q_count; i++) {
        gui_assets_draw_sprite(d, dr, gc, queue[i].asset_id,
                               queue[i].screen_x, queue[i].screen_y,
                               32, queue[i].facing, pal);

        /* Overhead health pip bar */
        int bar_w = 24;
        int bar_x = queue[i].screen_x - (bar_w / 2);
        int bar_y = queue[i].screen_y - 28;
        XSetForeground(d, gc, pal->bg);
        XFillRectangle(d, dr, gc, bar_x, bar_y, bar_w, 4);

        int fill_w = math_scale_ratio(queue[i].hp, queue[i].max_hp, bar_w);
        unsigned long hp_col = queue[i].is_hero ?
            pal->virtue_green : pal->malice_red;
        XSetForeground(d, gc, hp_col);
        XFillRectangle(d, dr, gc, bar_x, bar_y, fill_w, 4);
    }
}

/**
 * Renders the 2D Flat Orthographic Grid.
 */
static void render_flat_view(Display *d, Drawable dr, GC gc,
                             const gui_panel_rect_t *vp,
                             const player_state_t *state,
                             const gui_palette_t *pal)
{
    const int cell_size = 38;
    int grid_w = GRID_COLS * cell_size;
    int grid_h = GRID_ROWS * cell_size;
    int origin_x = vp->x + ((vp->width - grid_w) / 2);
    int origin_y = vp->y + ((vp->height - grid_h) / 2);
    if (origin_x < vp->x + 20) {
        origin_x = vp->x + 20;
    }
    if (origin_y < vp->y + 35) {
        origin_y = vp->y + 35;
    }

    int hero_gx = 3;
    int hero_gy = 4;
    int threat_gx = 5;
    int threat_gy = 2;

    for (int gy = 0; gy < GRID_ROWS; gy++) {
        for (int gx = 0; gx < GRID_COLS; gx++) {
            int cx = origin_x + (gx * cell_size);
            int cy = origin_y + (gy * cell_size);

            unsigned long fill_col = ((gx + gy) % 2 == 0) ?
                pal->panel : pal->btn_bg;
            XSetForeground(d, gc, fill_col);
            XFillRectangle(d, dr, gc, cx, cy, cell_size - 1, cell_size - 1);

            XSetForeground(d, gc, pal->tile_grid);
            XDrawRectangle(d, dr, gc, cx, cy, cell_size - 1, cell_size - 1);

            /* Movement overlay */
            int hero_dist = abs(gx - hero_gx) + abs(gy - hero_gy);
            if (hero_dist <= 2) {
                XSetForeground(d, gc, pal->tactical_move);
                XDrawRectangle(d, dr, gc, cx + 2, cy + 2,
                               cell_size - 5, cell_size - 5);
            }
        }
    }

    /* Draw Hero and Monster */
    int hx = origin_x + (hero_gx * cell_size) + (cell_size / 2);
    int hy = origin_y + (hero_gy * cell_size) + (cell_size / 2);
    gui_assets_draw_sprite(d, dr, gc, ASSET_HERO_ELIJAH, hx, hy, 28, 0, pal);

    if (state->threat_hp > 0) {
        int tx = origin_x + (threat_gx * cell_size) + (cell_size / 2);
        int ty = origin_y + (threat_gy * cell_size) + (cell_size / 2);
        gui_assets_draw_sprite(d, dr, gc, ASSET_ENEMY_DREAM_EATER,
                               tx, ty, 28, 1, pal);
    }
}

void gui_view2d_render(void *display, unsigned long dr, void *gc,
                       const gui_panel_rect_t *vp,
                       const player_state_t *state,
                       const gui_palette_t *pal,
                       gui_view_mode_t view_mode)
{
    if (display == NULL || vp == NULL || state == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    /* Render panel frame */
    XSetForeground(d, g, pal->panel);
    XFillRectangle(d, draw, g, vp->x, vp->y, vp->width, vp->height);
    XSetForeground(d, g, pal->border);
    XDrawRectangle(d, draw, g, vp->x, vp->y, vp->width, vp->height);

    /* View mode badge and toggle prompt */
    const char *badge = (view_mode == GUI_VIEW_25D_ISOMETRIC) ?
        "[2.5D ISOMETRIC] (Press 'M' to toggle 2D)" :
        "[2D TOP-DOWN] (Press 'M' to toggle 2.5D)";
    XSetForeground(d, g, pal->shield_gold);
    XDrawString(d, draw, g, vp->x + 14, vp->y + 22, badge, strlen(badge));

    /* Active location text */
    const char *loc = game_get_location_name(state->location);
    char loc_str[64];
    snprintf(loc_str, sizeof(loc_str), "Field: %s", loc);
    XSetForeground(d, g, pal->text_muted);
    XDrawString(d, draw, g, vp->x + vp->width - 180, vp->y + 22,
                loc_str, strlen(loc_str));

    /* Render chosen projection mode */
    if (view_mode == GUI_VIEW_25D_ISOMETRIC) {
        render_isometric_view(d, draw, g, vp, state, pal);
    } else {
        render_flat_view(d, draw, g, vp, state, pal);
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

bool gui_view2d_audit(void)
{
    int origin_x = 400;
    int origin_y = 200;
    int tile_w = GUI_ISO_TILE_W;
    int tile_h = GUI_ISO_TILE_H;
    int elev_scale = GUI_ISO_ELEV_SCALE;

    /* Audit 1: Test projection reciprocity at grid points (0,0), (3,4) */
    int test_points[3][2] = { {0, 0}, {3, 4}, {7, 7} };

    for (int i = 0; i < 3; i++) {
        int gx = test_points[i][0];
        int gy = test_points[i][1];
        int sx = 0;
        int sy = 0;
        gui_iso_project(gx, gy, 0, origin_x, origin_y,
                        tile_w, tile_h, elev_scale, &sx, &sy);

        int rec_gx = -1;
        int rec_gy = -1;
        gui_iso_unproject(sx, sy, 0, origin_x, origin_y,
                          tile_w, tile_h, elev_scale, &rec_gx, &rec_gy);

        if (rec_gx != gx || rec_gy != gy) {
            fprintf(stderr,
                "[GUI VIEW AUDIT] Unproject mismatch at (%d,%d): got (%d,%d)\n",
                gx, gy, rec_gx, rec_gy);
            return false;
        }
    }

    /* Audit 2: Elevation elevation offset shift */
    int sx_z0 = 0, sy_z0 = 0;
    int sx_z2 = 0, sy_z2 = 0;
    gui_iso_project(2, 2, 0, origin_x, origin_y,
                    tile_w, tile_h, elev_scale, &sx_z0, &sy_z0);
    gui_iso_project(2, 2, 2, origin_x, origin_y,
                    tile_w, tile_h, elev_scale, &sx_z2, &sy_z2);

    if (sx_z0 != sx_z2 || sy_z2 != sy_z0 - (2 * elev_scale)) {
        return false;
    }

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
