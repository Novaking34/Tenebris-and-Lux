// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_palette.c
// @id            gui.palette.manager
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implements hardware X11 colormap allocation and software ARGB shading math
//   for 2D / 2.5D visual rendering.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: gui/gui_palette.h, util/math/math_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure color arithmetic).
//   - Latency Budget:        < 5 microseconds per color allocation.
//
// AXIOMATIC INVARIANTS:
//   1. Clamps alpha to 0xFF000000 on 32-bit composited visuals.
//   2. Shading factors never wrap around 8-bit channel bounds.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC CONSTANTS
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "gui/gui_palette.h"
#include "util/math/math_utils.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define HEX_BG_OBSIDIAN    0x0F131E
#define HEX_PANEL_GRANITE  0x181E2E
#define HEX_BORDER_STONE   0x3A4460
#define HEX_TEXT_IVORY     0xF8F9FC
#define HEX_TEXT_SILVER    0x9BA5C2
#define HEX_SHIELD_GOLD    0xE8B84B
#define HEX_GOLD_BRIGHT    0xFFE082
#define HEX_SWORD_VIOLET   0x9D65E5
#define HEX_VIOLET_BRIGHT  0xC79BFF
#define HEX_VIRTUE_GREEN   0x2ECC71
#define HEX_MALICE_RED     0xE74C3C
#define HEX_BTN_SLATE      0x20273A
#define HEX_BTN_RIM        0x4A567A
#define HEX_BTN_HOVER      0x323D5A
#define HEX_TILE_GRID      0x2A3348
#define HEX_TILE_COBBLE    0x54607B
#define HEX_TILE_GRASS     0x2E6F40
#define HEX_TILE_WATER     0x214E80
#define HEX_TILE_WALL      0x5E6B88
#define HEX_TACTICAL_MOVE  0x3898EC
#define HEX_TACTICAL_ATK   0xF35B7A
#define HEX_SHADOW         0x07090F

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

/**
 * Allocates a hardware X11 pixel from a 24-bit RGB integer.
 */
static unsigned long alloc_x11_color(Display *d, int screen,
                                     unsigned int rgb_hex)
{
    if (d == NULL) {
        return 0xFF000000 | rgb_hex;
    }

    XColor col;
    unsigned int r = (rgb_hex >> 16) & 0xFF;
    unsigned int g = (rgb_hex >> 8) & 0xFF;
    unsigned int b = (rgb_hex) & 0xFF;

    col.red   = (unsigned short)(r * 257);
    col.green = (unsigned short)(g * 257);
    col.blue  = (unsigned short)(b * 257);
    col.flags = DoRed | DoGreen | DoBlue;

    if (XAllocColor(d, DefaultColormap(d, screen), &col)) {
        if (DefaultDepth(d, screen) == 32 || (col.pixel & 0xFF000000) == 0) {
            return col.pixel | 0xFF000000;
        }
        return col.pixel;
    }

    return 0xFF000000 | rgb_hex;
}

void gui_palette_init_default(gui_palette_t *p)
{
    if (p == NULL) {
        return;
    }

    p->bg            = 0xFF000000 | HEX_BG_OBSIDIAN;
    p->panel         = 0xFF000000 | HEX_PANEL_GRANITE;
    p->border        = 0xFF000000 | HEX_BORDER_STONE;
    p->text_main     = 0xFF000000 | HEX_TEXT_IVORY;
    p->text_muted    = 0xFF000000 | HEX_TEXT_SILVER;
    p->shield_gold   = 0xFF000000 | HEX_SHIELD_GOLD;
    p->gold_bright   = 0xFF000000 | HEX_GOLD_BRIGHT;
    p->sword_violet  = 0xFF000000 | HEX_SWORD_VIOLET;
    p->violet_bright = 0xFF000000 | HEX_VIOLET_BRIGHT;
    p->virtue_green  = 0xFF000000 | HEX_VIRTUE_GREEN;
    p->malice_red    = 0xFF000000 | HEX_MALICE_RED;
    p->btn_bg        = 0xFF000000 | HEX_BTN_SLATE;
    p->btn_border    = 0xFF000000 | HEX_BTN_RIM;
    p->btn_hover     = 0xFF000000 | HEX_BTN_HOVER;
    p->tile_grid     = 0xFF000000 | HEX_TILE_GRID;
    p->tile_cobble   = 0xFF000000 | HEX_TILE_COBBLE;
    p->tile_grass    = 0xFF000000 | HEX_TILE_GRASS;
    p->tile_water    = 0xFF000000 | HEX_TILE_WATER;
    p->tile_wall     = 0xFF000000 | HEX_TILE_WALL;
    p->tactical_move = 0xFF000000 | HEX_TACTICAL_MOVE;
    p->tactical_atk  = 0xFF000000 | HEX_TACTICAL_ATK;
    p->shadow        = 0xFF000000 | HEX_SHADOW;
}

void gui_palette_bind_x11(gui_palette_t *p, void *display, int screen)
{
    if (p == NULL || display == NULL) {
        return;
    }

    Display *d = (Display *)display;
    p->bg            = alloc_x11_color(d, screen, HEX_BG_OBSIDIAN);
    p->panel         = alloc_x11_color(d, screen, HEX_PANEL_GRANITE);
    p->border        = alloc_x11_color(d, screen, HEX_BORDER_STONE);
    p->text_main     = alloc_x11_color(d, screen, HEX_TEXT_IVORY);
    p->text_muted    = alloc_x11_color(d, screen, HEX_TEXT_SILVER);
    p->shield_gold   = alloc_x11_color(d, screen, HEX_SHIELD_GOLD);
    p->gold_bright   = alloc_x11_color(d, screen, HEX_GOLD_BRIGHT);
    p->sword_violet  = alloc_x11_color(d, screen, HEX_SWORD_VIOLET);
    p->violet_bright = alloc_x11_color(d, screen, HEX_VIOLET_BRIGHT);
    p->virtue_green  = alloc_x11_color(d, screen, HEX_VIRTUE_GREEN);
    p->malice_red    = alloc_x11_color(d, screen, HEX_MALICE_RED);
    p->btn_bg        = alloc_x11_color(d, screen, HEX_BTN_SLATE);
    p->btn_border    = alloc_x11_color(d, screen, HEX_BTN_RIM);
    p->btn_hover     = alloc_x11_color(d, screen, HEX_BTN_HOVER);
    p->tile_grid     = alloc_x11_color(d, screen, HEX_TILE_GRID);
    p->tile_cobble   = alloc_x11_color(d, screen, HEX_TILE_COBBLE);
    p->tile_grass    = alloc_x11_color(d, screen, HEX_TILE_GRASS);
    p->tile_water    = alloc_x11_color(d, screen, HEX_TILE_WATER);
    p->tile_wall     = alloc_x11_color(d, screen, HEX_TILE_WALL);
    p->tactical_move = alloc_x11_color(d, screen, HEX_TACTICAL_MOVE);
    p->tactical_atk  = alloc_x11_color(d, screen, HEX_TACTICAL_ATK);
    p->shadow        = alloc_x11_color(d, screen, HEX_SHADOW);
}

unsigned long gui_palette_shade(unsigned long color, float factor)
{
    if (factor < 0.0f) {
        factor = 0.0f;
    }

    int r = (int)(((color >> 16) & 0xFF) * factor);
    int g = (int)(((color >> 8) & 0xFF) * factor);
    int b = (int)(((color) & 0xFF) * factor);

    r = math_clamp_int(r, 0, 255);
    g = math_clamp_int(g, 0, 255);
    b = math_clamp_int(b, 0, 255);

    return 0xFF000000 | ((unsigned long)r << 16) |
           ((unsigned long)g << 8) | (unsigned long)b;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

bool gui_palette_audit(void)
{
    gui_palette_t p;
    gui_palette_init_default(&p);

    if ((p.bg & 0xFF000000) != 0xFF000000) {
        return false;
    }
    if ((p.shield_gold & 0xFF000000) != 0xFF000000) {
        return false;
    }

    /* Test shading attenuation and saturation */
    unsigned long shaded_dark = gui_palette_shade(p.tile_wall, 0.5f);
    unsigned long shaded_light = gui_palette_shade(p.tile_wall, 1.5f);

    int r_orig  = (int)((p.tile_wall >> 16) & 0xFF);
    int r_dark  = (int)((shaded_dark >> 16) & 0xFF);
    int r_light = (int)((shaded_light >> 16) & 0xFF);

    if (r_dark >= r_orig || r_light <= r_orig) {
        return false;
    }

    /* Negative factor check */
    unsigned long clamped_zero = gui_palette_shade(p.tile_wall, -1.0f);
    if ((clamped_zero & 0x00FFFFFF) != 0) {
        return false;
    }

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

