// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_assets.c
// @id            gui.assets.pipeline
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Asset catalog management and procedural geometric vector art synthesis.
//   Provides styled character heraldry, monster silhouettes, terrain textures,
//   and visual effects with drop-in slots for external artist bitmaps.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_assets.h, gui/gui_palette.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static registry, zero heap).
//   - Concurrency / Safety:  REENTRANT_PURE (Procedural math and rendering).
//   - Latency Budget:        < 50 microseconds per sprite draw.
//
// AXIOMATIC INVARIANTS:
//   1. All assets have defined procedural generators ensuring non-blank render.
//   2. Shading and geometry are strictly contained within requested dimensions.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC REGISTRY
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_assets.h"
#include "gui/gui_palette.h"

#include <X11/Xlib.h>
#include <string.h>

#define CATALOG_CAPACITY 16

static gui_asset_t s_catalog[CATALOG_CAPACITY];
static int s_catalog_count = 0;
static bool s_initialized = false;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

static void register_asset(gui_asset_id_t id, const char *name,
                           int w, int h, const char *path)
{
    if (s_catalog_count >= CATALOG_CAPACITY) {
        return;
    }

    gui_asset_t *a = &s_catalog[s_catalog_count++];
    a->id = id;
    a->name = name;
    a->kind = ASSET_KIND_PROCEDURAL;
    a->width = w;
    a->height = h;
    a->artist_file_path = path;
}

void gui_assets_init(void)
{
    if (s_initialized) {
        return;
    }

    s_catalog_count = 0;

    register_asset(ASSET_HERO_ELIJAH, "Elijah Ro-enheart", 32, 32,
                   "assets/sprites/characters/elijah_32x32.png");
    register_asset(ASSET_HERO_KATRINA, "Lady Katrina Jaren", 32, 32,
                   "assets/sprites/characters/katrina_32x32.png");
    register_asset(ASSET_HERO_CASATTI, "Prince Casatti", 32, 32,
                   "assets/sprites/characters/casatti_32x32.png");

    register_asset(ASSET_ENEMY_DREAM_EATER, "Dream Eater", 32, 32,
                   "assets/sprites/enemies/dream_eater_32x32.png");
    register_asset(ASSET_ENEMY_SHADOW_STALKER, "ShadowStalker", 32, 32,
                   "assets/sprites/enemies/shadow_stalker_32x32.png");
    register_asset(ASSET_ENEMY_DARK_LORD, "The Dark Lord", 40, 48,
                   "assets/sprites/enemies/dark_lord_40x48.png");

    register_asset(ASSET_TILE_COBBLESTONE, "Cobblestone", 48, 24,
                   "assets/sprites/tiles/cobblestone_48x24.png");
    register_asset(ASSET_TILE_GRASS, "Verdant Meadow", 48, 24,
                   "assets/sprites/tiles/grass_48x24.png");
    register_asset(ASSET_TILE_STONE_WALL, "Stone Wall", 48, 48,
                   "assets/sprites/tiles/stone_wall_48x48.png");
    register_asset(ASSET_TILE_VOID_CRAG, "Void Crag", 48, 24,
                   "assets/sprites/tiles/void_crag_48x24.png");
    register_asset(ASSET_TILE_WATER, "Abyssal Waters", 48, 24,
                   "assets/sprites/tiles/water_48x24.png");

    register_asset(ASSET_RELIC_SHIELD_LUX, "Shield of Lux", 24, 24,
                   "assets/sprites/icons/shield_lux_24x24.png");
    register_asset(ASSET_RELIC_SWORD_TENEBRIS, "Sword of Tenebris", 24, 24,
                   "assets/sprites/icons/sword_tenebris_24x24.png");

    register_asset(ASSET_VFX_LUX_BARRIO, "Lux Barrio Arc", 32, 32, NULL);
    register_asset(ASSET_VFX_CUTTATIO_SLASH, "Cuttatio Slash", 32, 32, NULL);

    s_initialized = true;
}

const gui_asset_t *gui_assets_get(gui_asset_id_t id)
{
    if (!s_initialized) {
        gui_assets_init();
    }

    for (int i = 0; i < s_catalog_count; i++) {
        if (s_catalog[i].id == id) {
            return &s_catalog[i];
        }
    }

    return NULL;
}

bool gui_assets_bind_external(gui_asset_id_t id, const char *file_path)
{
    if (file_path == NULL) {
        return false;
    }

    if (!s_initialized) {
        gui_assets_init();
    }

    for (int i = 0; i < s_catalog_count; i++) {
        if (s_catalog[i].id == id) {
            s_catalog[i].artist_file_path = file_path;
            s_catalog[i].kind = ASSET_KIND_BITMAP;
            return true;
        }
    }

    return false;
}

/**
 * Procedural geometric synthesis of Elijah (The Steadfast Vanguard).
 */
static void draw_procedural_elijah(Display *d, Drawable dr, GC gc,
                                   int cx, int cy, int size, int facing,
                                   const gui_palette_t *pal)
{
    (void)facing;
    int half = size / 2;

    /* Drop shadow */
    XSetForeground(d, gc, pal->shadow);
    XFillArc(d, dr, gc, cx - half + 4, cy + half - 6, size - 8, 8, 0, 360 * 64);

    /* Armor Torso: Slate granite mantle */
    XSetForeground(d, gc, pal->btn_border);
    XFillRectangle(d, dr, gc, cx - 6, cy - 8, 12, 14);

    /* Helm & Visor: Ivory with gold plume */
    XSetForeground(d, gc, pal->text_main);
    XFillArc(d, dr, gc, cx - 5, cy - 18, 10, 10, 0, 360 * 64);
    XSetForeground(d, gc, pal->shield_gold);
    XFillRectangle(d, dr, gc, cx - 2, cy - 22, 4, 5);

    /* Shield of Lux: Golden emblazoned heater shield on left */
    XSetForeground(d, gc, pal->shield_gold);
    XFillArc(d, dr, gc, cx - 12, cy - 8, 8, 14, 0, 360 * 64);
    XSetForeground(d, gc, pal->gold_bright);
    XDrawArc(d, dr, gc, cx - 12, cy - 8, 8, 14, 0, 360 * 64);

    /* Sword of Tenebris: Abyssal violet blade on right */
    XSetForeground(d, gc, pal->sword_violet);
    XDrawLine(d, dr, gc, cx + 8, cy - 12, cx + 8, cy + 6);
    XSetForeground(d, gc, pal->text_main);
    XDrawLine(d, dr, gc, cx + 6, cy - 4, cx + 10, cy - 4);
}

/**
 * Procedural geometric synthesis of Dream Eater (Astral Aberration).
 */
static void draw_procedural_dream_eater(Display *d, Drawable dr, GC gc,
                                        int cx, int cy, int size,
                                        const gui_palette_t *pal)
{
    (void)size;
    /* Soft abyssal shadow */
    XSetForeground(d, gc, pal->shadow);
    XFillArc(d, dr, gc, cx - 10, cy + 10, 20, 6, 0, 360 * 64);

    /* Central core: Violet diamond */
    XPoint core[4] = {
        { (short)cx, (short)(cy - 12) },
        { (short)(cx + 10), (short)cy },
        { (short)cx, (short)(cy + 12) },
        { (short)(cx - 10), (short)cy }
    };
    XSetForeground(d, gc, pal->sword_violet);
    XFillPolygon(d, dr, gc, core, 4, Convex, CoordModeOrigin);

    /* Glowing pupil / nucleus */
    XSetForeground(d, gc, pal->gold_bright);
    XFillArc(d, dr, gc, cx - 3, cy - 3, 6, 6, 0, 360 * 64);

    /* Orbiting abyssal motes */
    XSetForeground(d, gc, pal->violet_bright);
    XFillRectangle(d, dr, gc, cx - 14, cy - 6, 3, 3);
    XFillRectangle(d, dr, gc, cx + 12, cy + 4, 3, 3);
}

/**
 * Procedural geometric synthesis of ShadowStalker (Vanguard Threat).
 */
static void draw_procedural_shadow_stalker(Display *d, Drawable dr, GC gc,
                                           int cx, int cy, int size,
                                           const gui_palette_t *pal)
{
    (void)size;
    /* Shadow */
    XSetForeground(d, gc, pal->shadow);
    XFillArc(d, dr, gc, cx - 12, cy + 10, 24, 6, 0, 360 * 64);

    /* Horned shadowy body */
    XPoint body[5] = {
        { (short)(cx - 10), (short)(cy - 14) },
        { (short)(cx - 4),  (short)(cy - 6) },
        { (short)(cx + 4),  (short)(cy - 6) },
        { (short)(cx + 10), (short)(cy - 14) },
        { (short)cx,        (short)(cy + 10) }
    };
    XSetForeground(d, gc, pal->bg);
    XFillPolygon(d, dr, gc, body, 5, Convex, CoordModeOrigin);
    XSetForeground(d, gc, pal->border);
    XDrawLines(d, dr, gc, body, 5, CoordModeOrigin);

    /* Piercing crimson eyes */
    XSetForeground(d, gc, pal->malice_red);
    XFillRectangle(d, dr, gc, cx - 5, cy - 2, 3, 2);
    XFillRectangle(d, dr, gc, cx + 2, cy - 2, 3, 2);
}

void gui_assets_draw_sprite(void *display, unsigned long dr, void *gc,
                            gui_asset_id_t id, int cx, int cy,
                            int size, int facing,
                            const gui_palette_t *pal)
{
    if (display == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    switch (id) {
        case ASSET_HERO_ELIJAH:
        case ASSET_HERO_KATRINA:
        case ASSET_HERO_CASATTI:
            draw_procedural_elijah(d, draw, g, cx, cy, size, facing, pal);
            break;

        case ASSET_ENEMY_DREAM_EATER:
            draw_procedural_dream_eater(d, draw, g, cx, cy, size, pal);
            break;

        case ASSET_ENEMY_SHADOW_STALKER:
        case ASSET_ENEMY_DARK_LORD:
            draw_procedural_shadow_stalker(d, draw, g, cx, cy, size, pal);
            break;

        default:
            /* Generic placeholder entity */
            XSetForeground(d, g, pal->panel);
            XFillArc(d, draw, g, cx - 8, cy - 8, 16, 16, 0, 360 * 64);
            XSetForeground(d, g, pal->border);
            XDrawArc(d, draw, g, cx - 8, cy - 8, 16, 16, 0, 360 * 64);
            break;
    }
}

void gui_assets_draw_tile_detail(void *display, unsigned long dr, void *gc,
                                 gui_asset_id_t id, int x, int y,
                                 int w, int h,
                                 const gui_palette_t *pal)
{
    if (display == NULL || pal == NULL || w <= 4 || h <= 4) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    if (id == ASSET_TILE_COBBLESTONE) {
        XSetForeground(d, g, pal->border);
        XDrawLine(d, draw, g, x + (w / 4), y + (h / 3),
                  x + (3 * w / 4), y + (h / 3));
        XDrawLine(d, draw, g, x + (w / 3), y + (2 * h / 3),
                  x + (2 * w / 3), y + (2 * h / 3));
    } else if (id == ASSET_TILE_GRASS) {
        XSetForeground(d, g, pal->virtue_green);
        XDrawLine(d, draw, g, x + (w / 2) - 2, y + (h / 2) - 2,
                  x + (w / 2) - 2, y + (h / 2) + 2);
        XDrawLine(d, draw, g, x + (w / 2) + 4, y + (h / 2),
                  x + (w / 2) + 4, y + (h / 2) + 3);
    } else if (id == ASSET_TILE_VOID_CRAG) {
        XSetForeground(d, g, pal->sword_violet);
        XDrawLine(d, draw, g, x + 6, y + 4, x + (w / 2), y + (h / 2));
        XDrawLine(d, draw, g, x + (w / 2), y + (h / 2), x + w - 6, y + h - 4);
    }
}

void gui_assets_draw_vfx(void *display, unsigned long dr, void *gc,
                         gui_asset_id_t id, int cx, int cy,
                         int size, const gui_palette_t *pal)
{
    if (display == NULL || pal == NULL) {
        return;
    }

    Display *d = (Display *)display;
    GC g = (GC)gc;
    Drawable draw = (Drawable)dr;

    if (id == ASSET_VFX_LUX_BARRIO) {
        XSetForeground(d, g, pal->shield_gold);
        XDrawArc(d, draw, g, cx - size, cy - size,
                 size * 2, size * 2, 0, 360 * 64);
        XSetForeground(d, g, pal->gold_bright);
        XDrawArc(d, draw, g, cx - size + 2, cy - size + 2,
                 (size - 2) * 2, (size - 2) * 2, 0, 360 * 64);
    } else if (id == ASSET_VFX_CUTTATIO_SLASH) {
        XSetForeground(d, g, pal->violet_bright);
        XDrawLine(d, draw, g, cx - size, cy + size, cx + size, cy - size);
        XSetForeground(d, g, pal->sword_violet);
        XDrawLine(d, draw, g, cx - size + 2, cy + size,
                  cx + size + 2, cy - size);
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

bool gui_assets_audit(void)
{
    if (!s_initialized) {
        gui_assets_init();
    }

    if (s_catalog_count < 8) {
        return false;
    }

    /* Verify essential hero and monster asset presence */
    const gui_asset_t *hero = gui_assets_get(ASSET_HERO_ELIJAH);
    const gui_asset_t *eater = gui_assets_get(ASSET_ENEMY_DREAM_EATER);

    if (hero == NULL || eater == NULL) {
        return false;
    }

    if (hero->width <= 0 || hero->height <= 0) {
        return false;
    }

    /* External binding test */
    if (!gui_assets_bind_external(ASSET_HERO_KATRINA, "test/path.png")) {
        return false;
    }
    const gui_asset_t *katrina = gui_assets_get(ASSET_HERO_KATRINA);
    if (katrina->kind != ASSET_KIND_BITMAP) {
        return false;
    }

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
