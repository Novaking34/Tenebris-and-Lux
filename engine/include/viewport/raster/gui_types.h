/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_types.h
@id            viewport.raster.types
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop Presentation Types, View Modes & Bounding Geometry
@role          Core type definitions, layout bounding geometries, color models,
               view modes, and widget specifications for the Layer 2 presentation engine.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Presentation follows simulation: GUI types are read-only views of sim state.
               4. All layout rects are strictly non-overlapping and bounded within window dimensions.
               5. Seamless view mode support: Flat 2D, 2.5D Isometric, and Fullscreen modes.
               6. Asset abstraction: Procedural rasterizers seamlessly decouple from artist bitmaps.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Defines the primary ontology, layout boundaries, view mode enumerations,
  and widget structures for the desktop X11 presentation engine. Provides
  strongly typed data contracts connecting low-level X11 primitives with
  high-level tactical HUDs and battlefield viewports.

  ### For Game Designers & Non-Programmers:
  This file serves as the blueprint dictionary for everything drawn on screen:
  window dimensions (860x620), screen modes (Flat 2D vs Isometric 2.5D),
  how buttons look when clicked, and how many lines of story text fit on screen.

  ### For Systems Programmers & Engine Architects:
  - Strongly typed, non-anonymous enums (gui_view_mode_t, gui_btn_id_t, gui_font_role_t).
  - Fixed-size layout rectangles (geom_rect_t) with static memory layout.
  - Zero dynamic heap allocation: all widget states and log entries reside in static buffers.
  - Clean separation from game domain state: uses forward declarations and generic pointers.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_TYPES_H
#define TENEBRIS_LUX_GUI_TYPES_H

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
Tier 1: ISO C11 Standard Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Tier 2: External Library Dependencies (None) */

/* Tier 3: Internal Engine Foundation Headers */
#include "core/manifest.h"
#include "core/geom_utils.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing limits, window dimensions, tile dimensions, and buffer
capacities for the desktop X11 presentation engine.

For Designers:
Defines fixed logical desktop window bounds (860x620), isometric grid tile
proportions (48x24 with 8px elevation scale per step), log capacity
(16 lines of 128 characters), and navigation tab counts (5 tabs).

For Programmers:
Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) by establishing
fixed buffer limits for assets, log rings, and depth-sorted entity arrays.
--------------------------------------------------------------------------------
*/

/** Default native desktop presentation window width in pixels. */
#define GUI_WINDOW_WIDTH       860

/** Default native desktop presentation window height in pixels. */
#define GUI_WINDOW_HEIGHT      620

/** Maximum capacity of recent chronicle narrative event lines in the log ring. */
#define GUI_MAX_LOG_LINES      16

/** Maximum character length of a single chronicle narrative log entry. */
#define GUI_LOG_ENTRY_LEN      128

/** Maximum number of interactive buttons in the tactical command dock. */
#define GUI_NUM_BUTTONS        9

/** Number of top navigation screen tabs in the header banner. */
#define GUI_NUM_TABS           5

/** Maximum number of cached presentation assets in the asset registry. */
#define GUI_MAX_ASSETS         32

/** Maximum capacity of depth-sorted entities for Painter's algorithm rendering. */
#define GUI_MAX_ENTITIES_DEPTH 64

/** Width of an isometric grid tile diamond in pixels. */
#define GUI_ISO_TILE_W         48

/** Height of an isometric grid tile diamond in pixels. */
#define GUI_ISO_TILE_H         24

/** Vertical pixel offset scale per discrete elevation unit in isometric view. */
#define GUI_ISO_ELEV_SCALE     8

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Module identity and invariant verification hooks for GUI Presentation Types.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Desktop GUI Types & Descriptors. */
#define GUI_TYPES_MODULE_ID    "viewport.types"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Core types, layout bounding rectangles, color models, view modes, and widget
structures for the desktop presentation engine.
--------------------------------------------------------------------------------
*/

/**
 * @enum gui_screen_mode
 * @brief Presentation screen views and modal navigation states.
 *
 * For Designers:
 * Represents the 5 primary game screens accessible via the top navigation tabs:
 * Tactical Grid, Hero Dossier, Inventory, Provincial Map, and Chronicle Codex.
 *
 * For Programmers:
 * Bounded enumerator indexed [0, 4] matching tab descriptors.
 */
typedef enum gui_screen_mode {
    GUI_SCREEN_TACTICAL = 0,    /**< 2.5D / 2D Tactical battlefield & combat */
    GUI_SCREEN_STATUS,          /**< Full protagonist dossier & attributes */
    GUI_SCREEN_INVENTORY,       /**< Bag, key items, Grandma's Watch */
    GUI_SCREEN_WORLD_MAP,       /**< Provincial realm topology & travel */
    GUI_SCREEN_CHRONICLE        /**< Complete lore codex & history of deeds */
} gui_screen_mode_t;

/**
 * @struct gui_tab
 * @brief Top navigation tab descriptor.
 *
 * For Designers:
 * A clickable header tab bar button linking directly to a primary screen.
 *
 * For Programmers:
 * Bounded rectangular button descriptor with single hotkey accelerator.
 */
typedef struct gui_tab {
    int x;
    int y;
    int width;
    int height;
    const char *label;
    gui_screen_mode_t screen;
    char hotkey;
} gui_tab_t;

/**
 * @enum gui_view_mode
 * @brief Display view mode for the tactical world viewport.
 *
 * For Designers:
 * Switches between classic top-down orthographic grid and dimetric isometric 2.5D.
 *
 * For Programmers:
 * Pure discrete rendering mode toggle.
 */
typedef enum gui_view_mode {
    GUI_VIEW_2D_FLAT = 0,         /**< Classic orthographic top-down grid */
    GUI_VIEW_25D_ISOMETRIC = 1    /**< Tactical dimetric grid with Z-height */
} gui_view_mode_t;

/**
 * @struct gui_palette
 * @brief Hardware and software color palette representation.
 *
 * For Designers:
 * The complete thematic color palette defining background, panel, text,
 * relic, morale, and tactical tile colors.
 *
 * For Programmers:
 * Pixel color values packed for X11 TrueColor (0xRRGGBB).
 */
typedef struct gui_palette {
    unsigned long bg;             /**< Obsidian canvas */
    unsigned long panel;          /**< Granite panel background */
    unsigned long border;         /**< Carved stone rim */
    unsigned long text_main;      /**< Ivory text */
    unsigned long text_muted;     /**< Silver secondary text */
    unsigned long shield_gold;    /**< Radiant Lux gold */
    unsigned long gold_bright;    /**< Lux edge highlight */
    unsigned long sword_violet;   /**< Tenebrous violet */
    unsigned long violet_bright;  /**< Tenebrous edge highlight */
    unsigned long virtue_green;   /**< Emerald virtue */
    unsigned long malice_red;     /**< Crimson malice */
    unsigned long btn_bg;         /**< Button slate */
    unsigned long btn_border;     /**< Button rim */
    unsigned long btn_hover;      /**< Button hover highlight */
    unsigned long tile_grid;      /**< Tactical grid lines */
    unsigned long tile_cobble;    /**< Cobblestone fill */
    unsigned long tile_grass;     /**< Meadow grass fill */
    unsigned long tile_water;     /**< Abyssal water fill */
    unsigned long tile_wall;      /**< Rampart stone wall */
    unsigned long tactical_move;  /**< Move range highlight */
    unsigned long tactical_atk;   /**< Attack range highlight */
    unsigned long shadow;         /**< Drop shadow tint */
} gui_palette_t;

/**
 * @struct gui_panel_rect
 * @brief Named bounding box representing a major desktop presentation zone.
 *
 * For Designers:
 * A positioned rectangular section of the screen with a label title.
 *
 * For Programmers:
 * Simple bounding rectangle with non-owning title string pointer.
 */
typedef struct gui_panel_rect {
    int x;
    int y;
    int width;
    int height;
    const char *title;
} gui_panel_rect_t;

/**
 * @struct gui_layout
 * @brief Unified desktop presentation layout geometry.
 *
 * For Designers:
 * Specifies the arrangement of header, 2D viewport, hero status, chronicle log,
 * and command dock across the window.
 *
 * For Programmers:
 * Fixed non-overlapping panel partitions filling window dimensions.
 */
typedef struct gui_layout {
    int window_width;
    int window_height;
    gui_panel_rect_t header;       /**< Top title banner and cycle indicator */
    gui_panel_rect_t viewport_2d;  /**< 2D / 2.5D tactical map viewport */
    gui_panel_rect_t status;       /**< Right hero vitals & relics panel */
    gui_panel_rect_t chronicle;    /**< Bottom left narrative log */
    gui_panel_rect_t command_dock; /**< Bottom right tactical action dock */
} gui_layout_t;

/**
 * @struct gui_button
 * @brief Interactive button widget definition in screen coordinates.
 *
 * For Designers:
 * A button in the tactical command dock with label, hotkey, and active/hover states.
 *
 * For Programmers:
 * Screen-coordinate widget descriptor with hover and active state flags.
 */
typedef struct gui_button {
    int x;
    int y;
    int width;
    int height;
    const char *label;
    const char *command;
    char hotkey;
    bool is_hovered;
    bool is_active;
} gui_button_t;

/**
 * @struct gui_log
 * @brief Scrollable ring-buffer storing recent chronicle narrative events.
 *
 * For Designers:
 * Stores the running history of recent battle logs and story narration.
 *
 * For Programmers:
 * Fixed 2D character matrix with count index, zero dynamic heap.
 */
typedef struct gui_log {
    char lines[GUI_MAX_LOG_LINES][GUI_LOG_ENTRY_LEN];
    int count;
} gui_log_t;

/**
 * @enum gui_asset_id
 * @brief Canonical asset identifier for characters, monsters, terrain, and relics.
 *
 * For Designers:
 * Defines generic engine visual slots (hero, minion, boss, tile, relic, vfx)
 * mapped cleanly to game domain narrative aliases (Elijah, Dream Eater, Shield of Lux).
 *
 * For Programmers:
 * Tagged enumeration representing integer asset slot keys in the asset registry.
 */
typedef enum gui_asset_id {
    ASSET_NONE = 0,
    ASSET_ENTITY_PROTAGONIST_1 = 1,  /**< Primary hero entity slot */
    ASSET_ENTITY_PROTAGONIST_2 = 2,  /**< Secondary companion entity slot */
    ASSET_ENTITY_PROTAGONIST_3 = 3,  /**< Tertiary companion entity slot */
    ASSET_ENTITY_ENEMY_MINION_1 = 10, /**< Standard enemy minion slot 1 */
    ASSET_ENTITY_ENEMY_MINION_2 = 11, /**< Standard enemy minion slot 2 */
    ASSET_ENTITY_BOSS_1        = 12, /**< Major boss entity slot */
    ASSET_TILE_PATH            = 20, /**< Standard walkable terrain tile */
    ASSET_TILE_PLAINS          = 21, /**< Vegetative / meadow terrain tile */
    ASSET_TILE_STRUCTURE       = 22, /**< Solid wall / obstacle terrain tile */
    ASSET_TILE_HAZARD          = 23, /**< Void / chasm terrain tile */
    ASSET_TILE_LIQUID          = 24, /**< Water / fluid terrain tile */
    ASSET_RELIC_DEFENSIVE      = 30, /**< Primary defensive ward / shield icon */
    ASSET_RELIC_OFFENSIVE      = 31, /**< Primary offensive blade icon */
    ASSET_VFX_BARRIER          = 40, /**< Radiant defensive arc visual effect */
    ASSET_VFX_SLASH            = 41, /**< Striking slash visual effect */

    /* Game Domain Aliases for Backward Compatibility during v0.1.1 iteration */
    ASSET_HERO_ELIJAH          = ASSET_ENTITY_PROTAGONIST_1,
    ASSET_HERO_KATRINA         = ASSET_ENTITY_PROTAGONIST_2,
    ASSET_HERO_CASATTI         = ASSET_ENTITY_PROTAGONIST_3,
    ASSET_ENEMY_DREAM_EATER    = ASSET_ENTITY_ENEMY_MINION_1,
    ASSET_ENEMY_SHADOW_STALKER = ASSET_ENTITY_ENEMY_MINION_2,
    ASSET_ENEMY_DARK_LORD      = ASSET_ENTITY_BOSS_1,
    ASSET_TILE_COBBLESTONE     = ASSET_TILE_PATH,
    ASSET_TILE_GRASS           = ASSET_TILE_PLAINS,
    ASSET_TILE_STONE_WALL      = ASSET_TILE_STRUCTURE,
    ASSET_TILE_VOID_CRAG       = ASSET_TILE_HAZARD,
    ASSET_TILE_WATER           = ASSET_TILE_LIQUID,
    ASSET_RELIC_SHIELD_LUX     = ASSET_RELIC_DEFENSIVE,
    ASSET_RELIC_SWORD_TENEBRIS = ASSET_RELIC_OFFENSIVE,
    ASSET_VFX_LUX_BARRIO       = ASSET_VFX_BARRIER,
    ASSET_VFX_CUTTATIO_SLASH   = ASSET_VFX_SLASH
} gui_asset_id_t;

/**
 * @enum gui_asset_kind
 * @brief Asset pipeline origin and representation kind.
 *
 * For Designers:
 * Distinguishes procedural vector art drawn directly by code from artist bitmaps
 * loaded from disk, ensuring the game is 100% playable with zero external art.
 *
 * For Programmers:
 * Discrete rendering mode indicator governing raster dispatch.
 */
typedef enum gui_asset_kind {
    ASSET_KIND_PROCEDURAL = 0,    /**< Synthesized geometric vector art */
    ASSET_KIND_BITMAP = 1         /**< External PNG/BMP artist spritesheet */
} gui_asset_kind_t;

/**
 * @struct gui_asset
 * @brief Asset specification in the Asset Abstraction Layer.
 *
 * For Designers:
 * Describes an individual graphic asset: its slot ID, display name, kind (procedural vs bitmap),
 * pixel dimensions, and external artist file path.
 *
 * For Programmers:
 * Lightweight asset metadata record with non-owning path pointers.
 */
typedef struct gui_asset {
    gui_asset_id_t id;
    const char *name;
    gui_asset_kind_t kind;
    int width;
    int height;
    const char *artist_file_path; /**< Future artist spritesheet path */
} gui_asset_t;

/**
 * @struct gui_iso_point
 * @brief Isometric screen point with elevation depth.
 *
 * For Designers:
 * Represents a point in 2.5D space projected onto the screen with vertical elevation.
 *
 * For Programmers:
 * Triplet of screen coordinates (screen_x, screen_y) and discrete elevation_z.
 */
typedef struct gui_iso_point {
    int screen_x;
    int screen_y;
    int elevation_z;
} gui_iso_point_t;

/**
 * @struct gui_grid_coord
 * @brief Discrete grid coordinate.
 *
 * For Designers:
 * Specifies a specific square tile on the battlefield grid (gx, gy).
 *
 * For Programmers:
 * Integer grid pair with zero floating point inaccuracy.
 */
typedef struct gui_grid_coord {
    int gx;
    int gy;
} gui_grid_coord_t;

/**
 * @struct gui_depth_entity
 * @brief Depth-sorted entity render packet for the Painter's Algorithm.
 *
 * For Designers:
 * An entity (hero, monster, or boss) queued for drawing with proper depth sorting
 * so entities in front overlap entities behind them correctly.
 *
 * For Programmers:
 * Lightweight packet containing calculated depth sorting key, screen coordinates,
 * asset identifier, facing direction, and HP indicators.
 */
typedef struct gui_depth_entity {
    int depth_key;                /**< (gx + gy) * 1000 + gz * 10 */
    int screen_x;
    int screen_y;
    gui_asset_id_t asset_id;
    int facing;                   /**< 0: South, 1: West, 2: East, 3: North */
    int hp;
    int max_hp;
    bool is_hero;
} gui_depth_entity_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate validating if a panel rectangle has non-negative dimensions.
 *
 * @param[in] rect Pointer to panel rectangle.
 * @return true if non-null and width/height >= 0, false otherwise.
 */
static inline bool gui_panel_rect_is_valid(const gui_panel_rect_t *rect) {
    return rect != NULL && rect->width >= 0 && rect->height >= 0;
}

/**
 * @brief Predicate validating non-null safety for an isometric screen point.
 *
 * @param[in] pt Pointer to isometric point.
 * @return true if non-null, false otherwise.
 */
static inline bool gui_iso_point_is_valid(const gui_iso_point_t *pt) {
    return pt != NULL;
}

/**
 * @brief Predicate validating non-null safety for a discrete grid coordinate.
 *
 * @param[in] coord Pointer to grid coordinate.
 * @return true if non-null, false otherwise.
 */
static inline bool gui_grid_coord_is_valid(const gui_grid_coord_t *coord) {
    return coord != NULL;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Viewport Core Types
================================================================================
(Desktop GUI types define purely static structures, enums, color swatches,
coordinate primitives, and bounds-checking inline predicates. Operational
rasterization, projection, and layout APIs are declared in viewport/raster/gui.h,
viewport/raster/gui_view2d.h, and viewport/raster/gui_fallback.h.)
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Viewport Core Types
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
(Desktop GUI types define purely static structures, enums, and bounds-checking
inlines. Functional audits, layout tripwires, and pre-flight routines are
declared in viewport/raster/gui.h, viewport/raster/gui_fallback.h, and
viewport/raster/gui_view2d.h.)
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Viewport Core Types
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
(Pure type definition header: purely static structures, enums, color swatches,
coordinate primitives, and inline predicates. Operational rasterization,
subsystem lifecycle initialization, and diagnostic dump cards are declared
in viewport/raster/gui.h.)
================================================================================
*/

#endif /* TENEBRIS_LUX_GUI_TYPES_H */
