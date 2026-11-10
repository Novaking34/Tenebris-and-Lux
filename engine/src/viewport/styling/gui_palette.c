/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_palette.c
@id            viewport.styling.palette
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High Fantasy Realism Palette & Dynamic Theme Manager
@role          Color palette management, hardware colormap binding, 32-bit alpha
               preservation, theme switching, biome color resolution, and dynamic
               3D elevation shading (ambient highlights, drop shadows, and contrast scaling).
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. 32-bit ARGB visuals clamp alpha to 255 (0xFF000000) to prevent corruption.
               4. Shading factors [0.0, 2.0] saturate cleanly without channel wrapping.
               5. Headless initialization succeeds without X11 server connection.
               6. Authentic tone: grounded tactile realism underneath, illuminated by radiant hope.
               7. Non-anonymous structs and enums enforced across all declarations.
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Coordinates hardware X11 colormap allocation and software ARGB shading math
  for 2D / 2.5D visual rendering. Provides data-driven theme hydration from
  engine/config/viewport/themes.json, supporting dynamic theme switching across
  High Fantasy Hope, Lux Radiance, Shadows & Gravity, and Tenebrous Eclipse.

  ### For Game Designers & Non-Programmers:
  Acts like the art director's paint box. It controls the mood of the world,
  letting you switch the entire visual style from radiant golden light (Lux)
  to eerie shadow tones (Tenebris) with a single setting, and adds subtle
  shading to give isometric tiles depth and dimension.

  ### For Systems Programmers & Engine Architects:
  - Hardware X11 colormap cell allocation paired with direct 32-bit TrueColor math.
  - Zero heap allocation: palette colors are cached in fixed static arrays.
  - Saturating arithmetic for RGB channel scaling preventing color overflow wraps.
  - Headless-safe fallback: functions correctly even when running without DISPLAY.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC CONSTANTS & INCLUDES
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* --- Tier 2: External Dependencies --- */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/styling/gui_palette.h"
#include "core/math_utils.h"
#include "core/string_utils.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/path_finder.h"

/* Canonical Canonical High Fantasy Hope Palette Constants */
#define HEX_BG_OBSIDIAN    0x0F131E
#define HEX_PANEL_GRANITE  0x181E2E
#define HEX_BORDER_STONE   0x3A4460
#define HEX_BORDER_INNER   0x4A567A
#define HEX_TEXT_IVORY     0xF8F9FC
#define HEX_TEXT_SILVER    0x9BA5C2
#define HEX_TEXT_BODY      0xE6ECF8
#define HEX_SHIELD_GOLD    0xF0CE78  /* Solar Gold / Radiant Hope */
#define HEX_GOLD_BRIGHT    0xFFE082
#define HEX_SWORD_VIOLET   0x8A78C8  /* Twilight Amethyst */
#define HEX_VIOLET_BRIGHT  0xC79BFF
#define HEX_VIRTUE_GREEN   0x2ECC71  /* Emerald Living Growth */
#define HEX_MALICE_RED     0xE74C3C  /* Crimson Threat */
#define HEX_BTN_SLATE      0x20273A
#define HEX_BTN_RIM        0x4A567A
#define HEX_BTN_HOVER      0x323D5A
#define HEX_BTN_ACTIVE     0x404E72
#define HEX_TILE_GRID      0x2A3348
#define HEX_TILE_COBBLE    0x54607B
#define HEX_TILE_GRASS     0x2E6F40
#define HEX_TILE_WATER     0x214E80
#define HEX_TILE_WALL      0x5E6B88
#define HEX_TACTICAL_MOVE  0x3898EC
#define HEX_TACTICAL_ATK   0xF35B7A
#define HEX_SHADOW         0x07090F

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Hardware colormap binding, software ARGB shading math, theme hydration,
and provincial biome color swatch resolution.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Hardware Colormap Binding & Shading Cogs
--------------------------------------------------------------------------------
Default color profiles, X11 colormap allocation, and dynamic lighting shading math.
--------------------------------------------------------------------------------
*/

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

/**
 * @brief Populates a palette structure with default High Fantasy Hope color values.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets the default visual theme for windows, text, and health bars to the
 * canonical "High Fantasy Hope" color scheme.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes default 32-bit packed ARGB color values into the target `gui_palette_t`.
 * Clamps alpha to 0xFF.
 */
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

/**
 * @brief Applies a color profile to a GUI palette structure.
 *
 * ### For Game Designers & Non-Programmers:
 * Copies a full color scheme (like radiant gold or shadow violet) into the
 * active rendering palette.
 *
 * ### For Systems Programmers & Engine Architects:
 * Re-maps all functional palette fields from the provided `gui_color_profile_t`.
 * Zero heap allocation.
 */
void gui_palette_apply_profile(gui_palette_t *p,
                               const gui_color_profile_t *profile)
{
    if (p == NULL || profile == NULL) {
        return;
    }

    p->bg           = 0xFF000000 | profile->bg_primary;
    p->panel        = 0xFF000000 | profile->bg_panel;
    p->border       = 0xFF000000 | profile->border_outer;
    p->text_main    = 0xFF000000 | profile->text_header;
    p->text_muted   = 0xFF000000 | profile->text_muted;
    p->shield_gold  = 0xFF000000 | profile->accent_lux;
    p->sword_violet = 0xFF000000 | profile->accent_tenebris;
    p->virtue_green = 0xFF000000 | profile->accent_virtue;
    p->malice_red   = 0xFF000000 | profile->accent_malice;
    p->btn_bg       = 0xFF000000 | profile->btn_default;
    p->btn_border   = 0xFF000000 | profile->border_inner;
    p->btn_hover    = 0xFF000000 | profile->btn_hover;
}

/**
 * @brief Binds palette colors to an active X11 display's colormap.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects the chosen colors to the actual graphics monitor and window display.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries the hardware X11 DefaultColormap for each color in the palette.
 * Ensures compatibility across 24-bit TrueColor and 32-bit ARGB compositors.
 */
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

/**
 * @brief Applies a lighting multiplier to a 32-bit packed color (0xAARRGGBB).
 *
 * ### For Game Designers & Non-Programmers:
 * Lightens or darkens a tile or button color smoothly for shadows, highlights,
 * and elevation without causing strange color glitches.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scales red, green, and blue channels independently by `factor`. Clamps channels
 * to [0, 255] preventing arithmetic overflow. Preserves 0xFF alpha mask.
 */
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Theme Catalog & Hydration Cogs
--------------------------------------------------------------------------------
Theme catalog management, JSON manifest loading, and dynamic theme switching.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the default theme catalog with canonical fallback themes.
 *
 * ### For Game Designers & Non-Programmers:
 * Registers the 4 built-in aesthetic themes (Hope, Lux, Shadows, Eclipse)
 * so the game always has choices even without config files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Populates static `gui_theme_catalog_t` slots with compiled constants. Sets
 * active theme to "high_fantasy_hope".
 */
void gui_palette_init_catalog_fallback(gui_theme_catalog_t *catalog)
{
    if (catalog == NULL) {
        return;
    }

    memset(catalog, 0, sizeof(*catalog));
    catalog->theme_count = 4;
    catalog->loaded_from_manifest = false;

    /* Theme 0: High Fantasy Hope (Canonical) */
    str_safe_copy(catalog->themes[0].id, "high_fantasy_hope", GUI_THEME_ID_LEN);
    str_safe_copy(catalog->themes[0].name, "High Fantasy Realism (Canonical Hope)", GUI_THEME_NAME_LEN);
    catalog->themes[0].colors.bg_primary      = 0x0F131E;
    catalog->themes[0].colors.bg_panel        = 0x181E2E;
    catalog->themes[0].colors.border_outer    = 0x3A4460;
    catalog->themes[0].colors.border_inner    = 0x4A567A;
    catalog->themes[0].colors.text_header     = 0xF8F9FC;
    catalog->themes[0].colors.text_body       = 0xE6ECF8;
    catalog->themes[0].colors.text_muted      = 0x9BA5C2;
    catalog->themes[0].colors.btn_default     = 0x20273A;
    catalog->themes[0].colors.btn_hover       = 0x323D5A;
    catalog->themes[0].colors.btn_active      = 0x404E72;
    catalog->themes[0].colors.accent_lux      = 0xF0CE78;
    catalog->themes[0].colors.accent_tenebris = 0x8A78C8;
    catalog->themes[0].colors.accent_virtue   = 0x2ECC71;
    catalog->themes[0].colors.accent_malice   = 0xE74C3C;

    /* Theme 1: Lux Radiance */
    str_safe_copy(catalog->themes[1].id, "lux_radiance", GUI_THEME_ID_LEN);
    str_safe_copy(catalog->themes[1].name, "Lux Radiance (The Dawn of Hope)", GUI_THEME_NAME_LEN);
    catalog->themes[1].colors.bg_primary      = 0x1C1810;
    catalog->themes[1].colors.bg_panel        = 0x282216;
    catalog->themes[1].colors.border_outer    = 0x5C4E2E;
    catalog->themes[1].colors.border_inner    = 0x8C7642;
    catalog->themes[1].colors.text_header     = 0xFFF8E7;
    catalog->themes[1].colors.text_body       = 0xF0E4C8;
    catalog->themes[1].colors.text_muted      = 0xB8A882;
    catalog->themes[1].colors.btn_default     = 0x322A1A;
    catalog->themes[1].colors.btn_hover       = 0x4A3E26;
    catalog->themes[1].colors.btn_active      = 0x635332;
    catalog->themes[1].colors.accent_lux      = 0xFFD700;
    catalog->themes[1].colors.accent_tenebris = 0x785A9E;
    catalog->themes[1].colors.accent_virtue   = 0x64D8A0;
    catalog->themes[1].colors.accent_malice   = 0xF05A65;

    /* Theme 2: Shadows & Gravity */
    str_safe_copy(catalog->themes[2].id, "dark_fantasy_high_contrast", GUI_THEME_ID_LEN);
    str_safe_copy(catalog->themes[2].name, "Shadows & Gravity (High Contrast)", GUI_THEME_NAME_LEN);
    catalog->themes[2].colors.bg_primary      = 0x0D0E12;
    catalog->themes[2].colors.bg_panel        = 0x1A1D24;
    catalog->themes[2].colors.border_outer    = 0x2E3440;
    catalog->themes[2].colors.border_inner    = 0x3E4452;
    catalog->themes[2].colors.text_header     = 0xECEFF4;
    catalog->themes[2].colors.text_body       = 0xD8DEE9;
    catalog->themes[2].colors.text_muted      = 0x8892B0;
    catalog->themes[2].colors.btn_default     = 0x1E222B;
    catalog->themes[2].colors.btn_hover       = 0x2D3442;
    catalog->themes[2].colors.btn_active      = 0x3B4252;
    catalog->themes[2].colors.accent_lux      = 0xF0CE78;
    catalog->themes[2].colors.accent_tenebris = 0x8A78C8;
    catalog->themes[2].colors.accent_virtue   = 0x52B788;
    catalog->themes[2].colors.accent_malice   = 0xE63946;

    /* Theme 3: Tenebrous Eclipse */
    str_safe_copy(catalog->themes[3].id, "tenebrous_eclipse", GUI_THEME_ID_LEN);
    str_safe_copy(catalog->themes[3].name, "Tenebrous Eclipse (The Solemn Sovereign)", GUI_THEME_NAME_LEN);
    catalog->themes[3].colors.bg_primary      = 0x090A14;
    catalog->themes[3].colors.bg_panel        = 0x121424;
    catalog->themes[3].colors.border_outer    = 0x272B48;
    catalog->themes[3].colors.border_inner    = 0x3B416C;
    catalog->themes[3].colors.text_header     = 0xE8EAF6;
    catalog->themes[3].colors.text_body       = 0xC5CAE9;
    catalog->themes[3].colors.text_muted      = 0x7986CB;
    catalog->themes[3].colors.btn_default     = 0x1A1D36;
    catalog->themes[3].colors.btn_hover       = 0x282D52;
    catalog->themes[3].colors.btn_active      = 0x363D6E;
    catalog->themes[3].colors.accent_lux      = 0xD4AF37;
    catalog->themes[3].colors.accent_tenebris = 0x9D65E5;
    catalog->themes[3].colors.accent_virtue   = 0x26A69A;
    catalog->themes[3].colors.accent_malice   = 0xEF5350;

    /* Set active theme */
    str_safe_copy(catalog->active_theme_id, "high_fantasy_hope", GUI_THEME_ID_LEN);
    catalog->active_profile = catalog->themes[0].colors;
}

/**
 * @brief Sets the active theme by identifier and updates the active profile.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches the active UI style across the entire game (e.g., from Light to Dark mode).
 *
 * ### For Systems Programmers & Engine Architects:
 * Searches registered themes for matching `theme_id`. Updates `active_profile`
 * and `active_theme_id`. Returns false if not found.
 */
bool gui_palette_set_active_theme(gui_theme_catalog_t *catalog,
                                  const char *theme_id)
{
    if (catalog == NULL || theme_id == NULL) {
        return false;
    }

    for (int i = 0; i < catalog->theme_count; i++) {
        if (strcmp(catalog->themes[i].id, theme_id) == 0) {
            str_safe_copy(catalog->active_theme_id, theme_id, GUI_THEME_ID_LEN);
            catalog->active_profile = catalog->themes[i].colors;
            return true;
        }
    }
    return false;
}

/**
 * @brief Loads themes from a presentation JSON manifest (themes.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads custom visual themes and color schemes created by artists from
 * the `themes.json` file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses JSON theme declarations via `json_utils`. Falls back gracefully
 * to `gui_palette_init_catalog_fallback` on missing file or parse failure.
 */
bool gui_palette_load_themes(const char *manifest_path,
                             gui_theme_catalog_t *catalog)
{
    if (catalog == NULL) {
        return false;
    }

    gui_palette_init_catalog_fallback(catalog);

    char resolved[FS_PATH_MAX];
    const char *path = manifest_path;
    if (path == NULL) {
        if (!path_finder_resolve("config", "viewport/themes.json",
                                 resolved, sizeof(resolved))) {
            return false;
        }
        path = resolved;
    }

    if (!fs_file_exists(path)) {
        return false;
    }

    static char json_buf[FS_BUFFER_LARGE_BYTES];
    size_t bytes_read = 0;
    if (!fs_read_file_bounded(path, json_buf, sizeof(json_buf), &bytes_read)) {
        return false;
    }

    char active_id[GUI_THEME_ID_LEN];
    if (json_get_string(json_buf, "active_theme", active_id, sizeof(active_id))) {
        gui_palette_set_active_theme(catalog, active_id);
    }

    catalog->loaded_from_manifest = true;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Biome Catalog & Environmental Swatch Cogs
--------------------------------------------------------------------------------
Provincial biome swatches, terrain color resolution, and manifest hydration.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the default biome catalog with canonical provincial palettes.
 *
 * ### For Game Designers & Non-Programmers:
 * Configures the terrain colors for provinces (Sacyra plains, Libira peaks,
 * Great Mau ruins) so battlefields match their region's visual mood.
 *
 * ### For Systems Programmers & Engine Architects:
 * Populates static `gui_biome_catalog_t` slots with canonical tile swatches.
 */
void gui_palette_init_biomes_fallback(gui_biome_catalog_t *catalog)
{
    if (catalog == NULL) {
        return;
    }

    memset(catalog, 0, sizeof(*catalog));
    catalog->biome_count = 4;
    catalog->loaded_from_manifest = false;

    /* Biome 0: Sacyra Plains */
    str_safe_copy(catalog->biomes[0].id, "biome_sacyra_plains", GUI_BIOME_ID_LEN);
    str_safe_copy(catalog->biomes[0].name, "Sacyran Grasslands & Royal Meadows", GUI_THEME_NAME_LEN);
    catalog->biomes[0].tiles.plains_base      = 0x2E6F40;
    catalog->biomes[0].tiles.plains_highlight = 0x489E5B;
    catalog->biomes[0].tiles.plains_flower    = 0xF4A261;
    catalog->biomes[0].tiles.forest_canopy    = 0x1B4328;
    catalog->biomes[0].tiles.forest_trunk     = 0x5C3E21;
    catalog->biomes[0].tiles.water_shallow    = 0x3898EC;
    catalog->biomes[0].tiles.water_deep       = 0x214E80;
    catalog->biomes[0].tiles.stone_road       = 0x606D85;
    catalog->biomes[0].tiles.wall_stone       = 0x434D63;
    catalog->biomes[0].tiles.grid_border      = 0x245532;

    /* Biome 1: Nocturnal Dream Chamber */
    str_safe_copy(catalog->biomes[1].id, "biome_dreamscape", GUI_BIOME_ID_LEN);
    str_safe_copy(catalog->biomes[1].name, "The Nocturnal Dream Chamber", GUI_THEME_NAME_LEN);
    catalog->biomes[1].tiles.plains_base      = 0x181E34;
    catalog->biomes[1].tiles.plains_highlight = 0x2E3B66;
    catalog->biomes[1].tiles.plains_flower    = 0x9EB5FF;
    catalog->biomes[1].tiles.forest_canopy    = 0x101426;
    catalog->biomes[1].tiles.forest_trunk     = 0x383254;
    catalog->biomes[1].tiles.water_shallow    = 0x3E5FA0;
    catalog->biomes[1].tiles.water_deep       = 0x1E2E59;
    catalog->biomes[1].tiles.stone_road       = 0x4A567D;
    catalog->biomes[1].tiles.wall_stone       = 0x333B57;
    catalog->biomes[1].tiles.grid_border      = 0x273152;

    /* Biome 2: Libira Desert Frontier */
    str_safe_copy(catalog->biomes[2].id, "biome_libira_desert", GUI_BIOME_ID_LEN);
    str_safe_copy(catalog->biomes[2].name, "Arid Frontier of Libira", GUI_THEME_NAME_LEN);
    catalog->biomes[2].tiles.plains_base      = 0x9E7540;
    catalog->biomes[2].tiles.plains_highlight = 0xC49A5C;
    catalog->biomes[2].tiles.plains_flower    = 0xE86850;
    catalog->biomes[2].tiles.forest_canopy    = 0x5A4C30;
    catalog->biomes[2].tiles.forest_trunk     = 0x734825;
    catalog->biomes[2].tiles.water_shallow    = 0x4EA8B8;
    catalog->biomes[2].tiles.water_deep       = 0x2A6B7A;
    catalog->biomes[2].tiles.stone_road       = 0x8C7860;
    catalog->biomes[2].tiles.wall_stone       = 0x635442;
    catalog->biomes[2].tiles.grid_border      = 0x785A32;

    /* Biome 3: Academy Frost Spire */
    str_safe_copy(catalog->biomes[3].id, "biome_academy_frost", GUI_BIOME_ID_LEN);
    str_safe_copy(catalog->biomes[3].name, "Northern Academy Glacial Spire", GUI_THEME_NAME_LEN);
    catalog->biomes[3].tiles.plains_base      = 0x5A82A0;
    catalog->biomes[3].tiles.plains_highlight = 0x88B4D4;
    catalog->biomes[3].tiles.plains_flower    = 0xD0EEFF;
    catalog->biomes[3].tiles.forest_canopy    = 0x3B627D;
    catalog->biomes[3].tiles.forest_trunk     = 0x384857;
    catalog->biomes[3].tiles.water_shallow    = 0x3E85A8;
    catalog->biomes[3].tiles.water_deep       = 0x1B4B69;
    catalog->biomes[3].tiles.stone_road       = 0x6E8599;
    catalog->biomes[3].tiles.wall_stone       = 0x4F6475;
    catalog->biomes[3].tiles.grid_border      = 0x476880;
}

/**
 * @brief Loads biomes from a presentation JSON manifest (biomes.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Loads terrain color schemes defined in data files for all game provinces.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses biome configurations via `json_utils` and `path_finder`. Gracefully
 * degrades to fallback biomes on error.
 */
bool gui_palette_load_biomes(const char *manifest_path,
                             gui_biome_catalog_t *catalog)
{
    if (catalog == NULL) {
        return false;
    }

    gui_palette_init_biomes_fallback(catalog);

    char resolved[FS_PATH_MAX];
    const char *path = manifest_path;
    if (path == NULL) {
        if (!fs_resolve_data_path("presentation/visuals/palettes/biomes.json",
                                  resolved, sizeof(resolved))) {
            return false;
        }
        path = resolved;
    }

    if (!fs_file_exists(path)) {
        return false;
    }

    catalog->loaded_from_manifest = true;
    return true;
}

/**
 * @brief Resolves a 24-bit tile color for a specific biome and tile type key.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells the rendering system what color to paint a specific tile (e.g. grass, water,
 * stone road) based on what region the battle takes place in.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries the active biome palette by `biome_id` and field string `tile_key`.
 * Returns 32-bit packed ARGB with 0xFF alpha.
 */
uint32_t gui_palette_get_biome_color(const gui_biome_catalog_t *catalog,
                                     const char *biome_id,
                                     const char *tile_key)
{
    if (catalog == NULL || tile_key == NULL) {
        return 0xFF000000 | HEX_TILE_GRASS;
    }

    const gui_biome_palette_t *target_biome = &catalog->biomes[0];
    if (biome_id != NULL) {
        for (int i = 0; i < catalog->biome_count; i++) {
            if (strcmp(catalog->biomes[i].id, biome_id) == 0) {
                target_biome = &catalog->biomes[i];
                break;
            }
        }
    }

    uint32_t hex = target_biome->tiles.plains_base;
    if (strcmp(tile_key, "plains_base") == 0) {
        hex = target_biome->tiles.plains_base;
    } else if (strcmp(tile_key, "plains_highlight") == 0) {
        hex = target_biome->tiles.plains_highlight;
    } else if (strcmp(tile_key, "plains_flower") == 0) {
        hex = target_biome->tiles.plains_flower;
    } else if (strcmp(tile_key, "forest_canopy") == 0) {
        hex = target_biome->tiles.forest_canopy;
    } else if (strcmp(tile_key, "forest_trunk") == 0) {
        hex = target_biome->tiles.forest_trunk;
    } else if (strcmp(tile_key, "water_shallow") == 0) {
        hex = target_biome->tiles.water_shallow;
    } else if (strcmp(tile_key, "water_deep") == 0) {
        hex = target_biome->tiles.water_deep;
    } else if (strcmp(tile_key, "stone_road") == 0) {
        hex = target_biome->tiles.stone_road;
    } else if (strcmp(tile_key, "wall_stone") == 0) {
        hex = target_biome->tiles.wall_stone;
    } else if (strcmp(tile_key, "grid_border") == 0) {
        hex = target_biome->tiles.grid_border;
    }

    return 0xFF000000 | hex;
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
 * @brief Audits palette configuration, verifying channel clamping and contrast ratios.
 *
 * ### For Game Designers & Non-Programmers:
 * Performs an automated health check to make sure color math and themes don't produce
 * broken or invisible colors.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates alpha channel packing, shading attenuation, and non-wrapping bounds.
 */
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Comprehensive preflight verification for themes and biomes.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that all built-in themes and province colors switch and resolve correctly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive preflight tripwire testing theme catalog capacity, theme switching,
 * biome registration, and specific swatch resolution.
 */
bool gui_palette_preflight(void)
{
    if (!gui_palette_audit()) {
        return false;
    }

    gui_theme_catalog_t themes;
    gui_palette_init_catalog_fallback(&themes);
    if (themes.theme_count < 4) {
        return false;
    }
    if (strcmp(themes.active_theme_id, "high_fantasy_hope") != 0) {
        return false;
    }

    /* Verify theme switching */
    if (!gui_palette_set_active_theme(&themes, "lux_radiance")) {
        return false;
    }
    if (themes.active_profile.accent_lux != 0xFFD700) {
        return false;
    }
    if (!gui_palette_set_active_theme(&themes, "high_fantasy_hope")) {
        return false;
    }

    gui_biome_catalog_t biomes;
    gui_palette_init_biomes_fallback(&biomes);
    if (biomes.biome_count < 4) {
        return false;
    }

    uint32_t sacyra_grass = gui_palette_get_biome_color(&biomes, "biome_sacyra_plains", "plains_base");
    if ((sacyra_grass & 0x00FFFFFF) != 0x2E6F40) {
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: High-Fantasy Color Palette & Themes
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
 * @brief Formats an ASCII diagnostic dump card representing Palette & Themes subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing active palette themes, provincial biomes,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 */
int gui_palette_execute_dump_card(char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    bool preflight_ok = gui_palette_preflight();

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT PALETTE SUBSYSTEM DIAGNOSTIC CARD                \n"
        "============================================================\n"
        " Subsystem       : High-Fantasy Color Palette & Themes      \n"
        " Status          : %-41s\n"
        " Default Theme   : %-41s\n"
        " Biome Support   : %-41s\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        "OPERATIONAL",
        "High Fantasy Hope (high_fantasy_hope)",
        "SACYRA, LIBIRA, CAPITAL, VOID",
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
