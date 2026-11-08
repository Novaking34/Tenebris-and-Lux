/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_palette.h
@layer         Layer 2: Presentation & Viewport
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High Fantasy Realism Palette & Dynamic Theme Manager
@role          Color palette management, hardware colormap binding, 32-bit alpha
               preservation, theme switching, biome color resolution, and dynamic
               3D elevation shading (ambient highlights, drop shadows, and contrast scaling).
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. 32-bit ARGB visuals clamp alpha to 255 (0xFF000000) to prevent corruption.
               3. Shading factors [0.0, 2.0] saturate cleanly without channel wrapping.
               4. Headless initialization succeeds without X11 server connection.
               5. Authentic tone: grounded tactile realism underneath, illuminated by radiant hope.
               6. Non-anonymous structs and enums enforced across all declarations.

PURPOSE & ARCHITECTURAL ROLE:
  Coordinates hardware X11 colormap allocation and software ARGB shading math
  for 2D / 2.5D visual rendering. Provides data-driven theme hydration from
  themes.json and biomes.json, supporting dynamic theme switching across High
  Fantasy Hope, Lux Radiance, Shadows & Gravity, and Tenebrous Eclipse.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_PALETTE_H
#define TENEBRIS_LUX_GUI_PALETTE_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "viewport/raster/gui_types.h"

#define GUI_MAX_THEMES        8
#define GUI_THEME_ID_LEN      32
#define GUI_THEME_NAME_LEN    64
#define GUI_MAX_BIOMES        8
#define GUI_BIOME_ID_LEN      32

/**
 * 24-bit RGB / 32-bit ARGB Color Profile specification.
 * Reflects High Fantasy Realism with radiant hope and grounded tactile depth.
 */
typedef struct gui_color_profile {
    uint32_t bg_primary;       /**< Deep canvas slate / obsidian */
    uint32_t bg_panel;         /**< Granite panel backdrop */
    uint32_t border_outer;     /**< Heavy carved stone outer rim */
    uint32_t border_inner;     /**< Sun-warmed inner bevel border */
    uint32_t text_header;      /**< High-contrast ivory text */
    uint32_t text_body;        /**< Warm silver parchment body text */
    uint32_t text_muted;       /**< Secondary muted slate text */
    uint32_t btn_default;      /**< Unfocused button slate fill */
    uint32_t btn_hover;        /**< Radiant button hover state */
    uint32_t btn_active;       /**< Depressed button active state */
    uint32_t accent_lux;       /**< Solar Gold (Radiant Hope / Shield of Lux) */
    uint32_t accent_tenebris;  /**< Twilight Amethyst (Solemn Gravity / Sword of Tenebris) */
    uint32_t accent_virtue;    /**< Emerald Verdant (Living Growth & Renewal) */
    uint32_t accent_malice;    /**< Crimson Malice (Usuru's Shadow & Dire Threat) */
} gui_color_profile_t;

/**
 * Individual theme definition within the theme catalog.
 */
typedef struct gui_theme {
    char id[GUI_THEME_ID_LEN];
    char name[GUI_THEME_NAME_LEN];
    gui_color_profile_t colors;
} gui_theme_t;

/**
 * Master theme catalog holding registered presentation themes.
 */
typedef struct gui_theme_catalog {
    char active_theme_id[GUI_THEME_ID_LEN];
    gui_theme_t themes[GUI_MAX_THEMES];
    int theme_count;
    gui_color_profile_t active_profile;
    bool loaded_from_manifest;
} gui_theme_catalog_t;

/**
 * Terrain tile color set for environmental biomes.
 */
typedef struct gui_biome_tile_colors {
    uint32_t plains_base;
    uint32_t plains_highlight;
    uint32_t plains_flower;
    uint32_t forest_canopy;
    uint32_t forest_trunk;
    uint32_t water_shallow;
    uint32_t water_deep;
    uint32_t stone_road;
    uint32_t wall_stone;
    uint32_t grid_border;
} gui_biome_tile_colors_t;

/**
 * Environmental biome definition.
 */
typedef struct gui_biome_palette {
    char id[GUI_BIOME_ID_LEN];
    char name[GUI_THEME_NAME_LEN];
    gui_biome_tile_colors_t tiles;
} gui_biome_palette_t;

/**
 * Master biome catalog holding provincial terrain palettes.
 */
typedef struct gui_biome_catalog {
    gui_biome_palette_t biomes[GUI_MAX_BIOMES];
    int biome_count;
    bool loaded_from_manifest;
} gui_biome_catalog_t;

/*
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/**
 * Populates a palette structure with default High Fantasy Hope color values.
 *
 * @param[out] p Pointer to palette struct to populate.
 */
void gui_palette_init_default(gui_palette_t *p);

/**
 * Initializes the default theme catalog with canonical fallback themes.
 *
 * @param[out] catalog Target theme catalog to populate.
 */
void gui_palette_init_catalog_fallback(gui_theme_catalog_t *catalog);

/**
 * Loads themes from a presentation JSON manifest (themes.json).
 *
 * @param[in]  manifest_path Path to themes.json (or NULL to auto-resolve).
 * @param[out] catalog       Target theme catalog.
 * @return `true` if loaded successfully; `false` on fallback degradation.
 */
bool gui_palette_load_themes(const char *manifest_path,
                             gui_theme_catalog_t *catalog);

/**
 * Sets the active theme by identifier and updates the active profile.
 *
 * @param[in,out] catalog  Target theme catalog.
 * @param[in]     theme_id Desired theme identifier (e.g. "high_fantasy_hope").
 * @return `true` if found and applied; `false` if not found (retains active).
 */
bool gui_palette_set_active_theme(gui_theme_catalog_t *catalog,
                                  const char *theme_id);

/**
 * Applies a color profile to a GUI palette structure.
 *
 * @param[out] p       Target palette struct.
 * @param[in]  profile Source color profile.
 */
void gui_palette_apply_profile(gui_palette_t *p,
                               const gui_color_profile_t *profile);

/**
 * Initializes the default biome catalog with canonical provincial palettes.
 *
 * @param[out] catalog Target biome catalog to populate.
 */
void gui_palette_init_biomes_fallback(gui_biome_catalog_t *catalog);

/**
 * Loads biomes from a presentation JSON manifest (biomes.json).
 *
 * @param[in]  manifest_path Path to biomes.json (or NULL to auto-resolve).
 * @param[out] catalog       Target biome catalog.
 * @return `true` if loaded successfully; `false` on fallback degradation.
 */
bool gui_palette_load_biomes(const char *manifest_path,
                             gui_biome_catalog_t *catalog);

/**
 * Resolves a 24-bit tile color for a specific biome and tile type key.
 *
 * @param[in] catalog  Target biome catalog.
 * @param[in] biome_id Biome identifier (e.g. "biome_sacyra_plains").
 * @param[in] tile_key Tile key (e.g. "plains_base", "water_shallow").
 * @return 32-bit packed ARGB color with alpha set to 0xFF.
 */
uint32_t gui_palette_get_biome_color(const gui_biome_catalog_t *catalog,
                                     const char *biome_id,
                                     const char *tile_key);

/**
 * Binds palette colors to an active X11 display's colormap.
 *
 * @param[in,out] p       Palette struct to bind.
 * @param[in]     display Opaque pointer to X11 Display handle.
 * @param[in]     screen  Target screen integer.
 */
void gui_palette_bind_x11(gui_palette_t *p, void *display, int screen);

/**
 * Applies a lighting multiplier to a 32-bit packed color (0xAARRGGBB).
 *
 * @param[in] color  Base color value.
 * @param[in] factor Scaling factor (e.g. 0.6 for shadow, 1.3 for highlight).
 * @return Scaled color clamped to [0, 255] per channel with preserved alpha.
 */
unsigned long gui_palette_shade(unsigned long color, float factor);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
*/

/**
 * Audits palette configuration, verifying channel clamping and contrast ratios.
 *
 * @return `true` if palette invariants are satisfied; `false` on failure.
 */
bool gui_palette_audit(void);

/**
 * Comprehensive preflight verification for themes and biomes.
 *
 * @return `true` if all palettes, themes, and biomes pass validation.
 */
bool gui_palette_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

#endif /* TENEBRIS_LUX_GUI_PALETTE_H */
