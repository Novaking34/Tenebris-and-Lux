/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_palette.h
@id            viewport.styling.palette
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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

#ifndef TENEBRIS_LUX_GUI_PALETTE_H
#define TENEBRIS_LUX_GUI_PALETTE_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Color profile specifications, theme catalogs, biome terrain palettes,
include ladder, and pre-conditional static validation inlines for GUI styling.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "viewport/raster/gui_types.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Palette Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for theme and biome catalogs.
For Designers: Boundaries ensure the game can register up to 8 visual themes
(e.g., Solar Radiance, Abyssal Shadow) and 8 biome palettes (plains, forest, stone).
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/** Maximum selectable visual themes in the master catalog. */
#define GUI_MAX_THEMES        8

/** Maximum character length for theme identifier strings. */
#define GUI_THEME_ID_LEN      32

/** Maximum character length for descriptive theme names. */
#define GUI_THEME_NAME_LEN    64

/** Maximum registered environmental biome terrain palettes. */
#define GUI_MAX_BIOMES        8

/** Maximum character length for biome identifier strings. */
#define GUI_BIOME_ID_LEN      32

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the GUI Palette subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Color Palette & Theming subsystem. */
#define GUI_PALETTE_MODULE_ID          "viewport.palette"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for color profiles, themes, and biome terrain palettes.
--------------------------------------------------------------------------------
*/

/**
 * @struct gui_color_profile
 * @brief 24-bit RGB / 32-bit ARGB Color Profile specification.
 *
 * For Designers: The master color scheme for windows, panels, buttons, and text.
 * Reflects High Fantasy Realism with radiant hope and grounded tactile depth.
 *
 * For Programmers: Explicitly packed 32-bit color values formatted 0xAARRGGBB.
 */
typedef struct gui_color_profile {
    uint32_t bg_primary;       /**< Deep canvas slate / obsidian. */
    uint32_t bg_panel;         /**< Granite panel backdrop. */
    uint32_t border_outer;     /**< Heavy carved stone outer rim. */
    uint32_t border_inner;     /**< Sun-warmed inner bevel border. */
    uint32_t text_header;      /**< High-contrast ivory text. */
    uint32_t text_body;        /**< Warm silver parchment body text. */
    uint32_t text_muted;       /**< Secondary muted slate text. */
    uint32_t btn_default;      /**< Unfocused button slate fill. */
    uint32_t btn_hover;        /**< Radiant button hover state. */
    uint32_t btn_active;       /**< Depressed button active state. */
    uint32_t accent_lux;       /**< Solar Gold (Radiant Hope / Shield of Lux). */
    uint32_t accent_tenebris;  /**< Twilight Amethyst (Solemn Gravity / Sword of Tenebris). */
    uint32_t accent_virtue;    /**< Emerald Verdant (Living Growth & Renewal). */
    uint32_t accent_malice;    /**< Crimson Malice (Usuru's Shadow & Dire Threat). */
} gui_color_profile_t;

/**
 * @struct gui_theme
 * @brief Individual theme definition within the theme catalog.
 *
 * For Designers: A named skin for the user interface loaded from data files.
 *
 * For Programmers: Loaded from engine/config/viewport/themes.json.
 */
typedef struct gui_theme {
    char                id[GUI_THEME_ID_LEN];     /**< Unique theme identifier string. */
    char                name[GUI_THEME_NAME_LEN]; /**< Human-readable theme name. */
    gui_color_profile_t colors;                   /**< Concrete 32-bit color profile. */
} gui_theme_t;

/**
 * @struct gui_theme_catalog
 * @brief Master theme catalog holding registered presentation themes.
 *
 * For Designers: The menu collection of all available UI themes and the active choice.
 *
 * For Programmers: Stack/static catalog populated by gui_palette_load_themes().
 */
typedef struct gui_theme_catalog {
    char                active_theme_id[GUI_THEME_ID_LEN]; /**< Identifier of active theme. */
    gui_theme_t         themes[GUI_MAX_THEMES];            /**< Registered theme slots. */
    int                 theme_count;                       /**< Number of active registered themes. */
    gui_color_profile_t active_profile;                    /**< Currently active color profile. */
    bool                loaded_from_manifest;              /**< true if loaded from JSON. */
} gui_theme_catalog_t;

/**
 * @struct gui_biome_tile_colors
 * @brief Terrain tile color set for environmental biomes.
 *
 * For Designers: Distinct colors for grass, flowers, water, and stone roads in a region.
 *
 * For Programmers: 32-bit ARGB color swatches for procedural tile rasterization.
 */
typedef struct gui_biome_tile_colors {
    uint32_t plains_base;      /**< Base meadow grass color. */
    uint32_t plains_highlight; /**< Sunny meadow grass highlight. */
    uint32_t plains_flower;    /**< Wildflower petal accent. */
    uint32_t forest_canopy;    /**< Deep ancient canopy greens. */
    uint32_t forest_trunk;     /**< Weathered wood brown. */
    uint32_t water_shallow;    /**< Clear turquoise shallows. */
    uint32_t water_deep;       /**< Sunken sapphire depths. */
    uint32_t stone_road;       /**< Cobblestone highway grey. */
    uint32_t wall_stone;       /**< Fortified rampart masonry. */
    uint32_t grid_border;      /**< Tactical tile boundary gridlines. */
} gui_biome_tile_colors_t;

/**
 * @struct gui_biome_palette
 * @brief Environmental biome definition.
 *
 * For Designers: Visual styling for a province (e.g. Sacyra plains, Libira peaks).
 *
 * For Programmers: Named collection of tile colors.
 */
typedef struct gui_biome_palette {
    char                    id[GUI_BIOME_ID_LEN];     /**< Biome identifier string. */
    char                    name[GUI_THEME_NAME_LEN]; /**< Human-readable biome title. */
    gui_biome_tile_colors_t tiles;                    /**< Terrain color swatch. */
} gui_biome_palette_t;

/**
 * @struct gui_biome_catalog
 * @brief Master biome catalog holding provincial terrain palettes.
 *
 * For Designers: The world catalog storing visual biomes across the continent.
 *
 * For Programmers: Stack/static catalog populated from world manifests.
 */
typedef struct gui_biome_catalog {
    gui_biome_palette_t biomes[GUI_MAX_BIOMES]; /**< Registered biome slots. */
    int                 biome_count;            /**< Number of active biomes. */
    bool                loaded_from_manifest;   /**< true if loaded from JSON. */
} gui_biome_catalog_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a theme count is within catalog capacity.
 *
 * @param[in] count Theme count to check.
 * @return true if count >= 0 and count <= GUI_MAX_THEMES.
 */
static inline bool gui_palette_theme_count_is_valid(int count) {
    return count >= 0 && count <= GUI_MAX_THEMES;
}

/**
 * @brief Predicate verifying if a biome count is within catalog capacity.
 *
 * @param[in] count Biome count to check.
 * @return true if count >= 0 and count <= GUI_MAX_BIOMES.
 */
static inline bool gui_palette_biome_count_is_valid(int count) {
    return count >= 0 && count <= GUI_MAX_BIOMES;
}

/**
 * @brief Predicate verifying if a theme catalog has been loaded from manifest data.
 *
 * @param[in] cat Pointer to theme catalog to inspect.
 * @return true if cat is non-NULL and loaded_from_manifest is true.
 */
static inline bool gui_theme_catalog_is_loaded(const gui_theme_catalog_t *cat) {
    return (cat != NULL) && cat->loaded_from_manifest;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Color profile application, theme switching, biome swatches, and shading math.
All execution loops and lifecycle hooks are quarantined in Block 5.
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
 * @brief Populates a palette structure with default High Fantasy Hope color values.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets the default visual theme for windows, text, and health bars to the
 * canonical "High Fantasy Hope" color scheme.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes default 32-bit packed ARGB color values into the target `gui_palette_t`.
 * Clamps alpha to 0xFF.
 *
 * @param[out] p Pointer to palette struct to populate.
 */
void gui_palette_init_default(gui_palette_t *p);

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
 *
 * @param[out] p       Target palette struct.
 * @param[in]  profile Source color profile.
 */
void gui_palette_apply_profile(gui_palette_t *p,
                               const gui_color_profile_t *profile);

/**
 * @brief Binds palette colors to an active X11 display's colormap.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects the chosen colors to the actual graphics monitor and window display.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries the hardware X11 DefaultColormap for each color in the palette.
 * Ensures compatibility across 24-bit TrueColor and 32-bit ARGB compositors.
 *
 * @param[in,out] p       Palette struct to bind.
 * @param[in]     display Opaque pointer to X11 Display handle.
 * @param[in]     screen  Target screen integer.
 */
void gui_palette_bind_x11(gui_palette_t *p, void *display, int screen);

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
 *
 * @param[in] color  Base color value.
 * @param[in] factor Scaling factor (e.g. 0.6 for shadow, 1.3 for highlight).
 * @return Scaled color clamped to [0, 255] per channel with preserved alpha.
 */
unsigned long gui_palette_shade(unsigned long color, float factor);

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
 *
 * @param[out] catalog Target theme catalog to populate.
 */
void gui_palette_init_catalog_fallback(gui_theme_catalog_t *catalog);

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
 *
 * @param[in]  manifest_path Path to themes.json (or NULL to auto-resolve).
 * @param[out] catalog       Target theme catalog.
 * @return `true` if loaded successfully; `false` on fallback degradation.
 */
bool gui_palette_load_themes(const char *manifest_path,
                             gui_theme_catalog_t *catalog);

/**
 * @brief Sets the active theme by identifier and updates the active profile.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches the active UI style across the entire game (e.g., from Light to Dark mode).
 *
 * ### For Systems Programmers & Engine Architects:
 * Searches registered themes for matching `theme_id`. Updates `active_profile`
 * and `active_theme_id`. Returns false if not found.
 *
 * @param[in,out] catalog  Target theme catalog.
 * @param[in]     theme_id Desired theme identifier (e.g. "high_fantasy_hope").
 * @return `true` if found and applied; `false` if not found (retains active).
 */
bool gui_palette_set_active_theme(gui_theme_catalog_t *catalog,
                                  const char *theme_id);

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
 *
 * @param[out] catalog Target biome catalog to populate.
 */
void gui_palette_init_biomes_fallback(gui_biome_catalog_t *catalog);

/**
 * @brief Loads biomes from a presentation JSON manifest (biomes.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Loads terrain color schemes defined in data files for all game provinces.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses biome configurations via `json_utils` and `path_finder`. Gracefully
 * degrades to fallback biomes on error.
 *
 * @param[in]  manifest_path Path to biomes.json (or NULL to auto-resolve).
 * @param[out] catalog       Target biome catalog.
 * @return `true` if loaded successfully; `false` on fallback degradation.
 */
bool gui_palette_load_biomes(const char *manifest_path,
                             gui_biome_catalog_t *catalog);

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
 *
 * @param[in] catalog  Target biome catalog.
 * @param[in] biome_id Biome identifier (e.g. "biome_sacyra_plains").
 * @param[in] tile_key Tile key (e.g. "plains_base", "water_shallow").
 * @return 32-bit packed ARGB color with alpha set to 0xFF.
 */
uint32_t gui_palette_get_biome_color(const gui_biome_catalog_t *catalog,
                                     const char *biome_id,
                                     const char *tile_key);

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
 *
 * @return `true` if palette invariants are satisfied; `false` on failure.
 */
bool gui_palette_audit(void);

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
 *
 * @return `true` if all palettes, themes, and biomes pass validation.
 */
bool gui_palette_preflight(void);

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
 *
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_palette_execute_dump_card(char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_GUI_PALETTE_H */
