// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          gui_types.h
// @id            gui.types.shared
// @layer         Layer 2: Presentation & Viewport
// @version       1.0.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core type definitions, layout bounding geometries, color models,
//   view modes, and widget specifications for the Layer 2 desktop GUI
//   presentation layer and 2.5D tactical isometric viewport engine.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/core/sim_types.h, util/math/geom_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Presentation thread).
//   - Latency Budget:        < 16,666 microseconds per frame (60 FPS budget).
//
// AXIOMATIC INVARIANTS:
//   1. Presentation follows simulation: GUI never mutates state directly.
//   2. All layout rects are strictly non-overlapping and bounded within window.
//   3. Supports seamless toggle between Flat 2D and 2.5D Isometric viewports.
//   4. Asset abstraction decouples procedural rasterizers from artist bitmaps.
// =============================================================================

#ifndef TENEBRIS_LUX_GUI_TYPES_H
#define TENEBRIS_LUX_GUI_TYPES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
// =============================================================================

#include "hero/sim_types.h"
#include "core/geom_utils.h"
#include <stdbool.h>
#include <stddef.h>

#define GUI_WINDOW_WIDTH       860
#define GUI_WINDOW_HEIGHT      620
#define GUI_MAX_LOG_LINES      16
#define GUI_LOG_ENTRY_LEN      128
#define GUI_NUM_BUTTONS        9
#define GUI_NUM_TABS           5
#define GUI_MAX_ASSETS         32
#define GUI_MAX_ENTITIES_DEPTH 64
#define GUI_ISO_TILE_W         48
#define GUI_ISO_TILE_H         24
#define GUI_ISO_ELEV_SCALE     8

/**
 * Presentation screen views and modal navigation states.
 */
typedef enum gui_screen_mode {
    GUI_SCREEN_TACTICAL = 0,    /**< 2.5D / 2D Tactical battlefield & combat */
    GUI_SCREEN_STATUS,          /**< Full protagonist dossier & attributes */
    GUI_SCREEN_INVENTORY,       /**< Bag, key items, Grandma's Watch */
    GUI_SCREEN_WORLD_MAP,       /**< Provincial realm topology & travel */
    GUI_SCREEN_CHRONICLE        /**< Complete lore codex & history of deeds */
} gui_screen_mode_t;

/**
 * Top navigation tab descriptor.
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
 * Display view mode for the tactical world viewport.
 */
typedef enum gui_view_mode {
    GUI_VIEW_2D_FLAT = 0,         /**< Classic orthographic top-down grid */
    GUI_VIEW_25D_ISOMETRIC = 1    /**< Tactical dimetric grid with Z-height */
} gui_view_mode_t;

/**
 * Hardware and software color palette representation.
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
 * Named bounding box representing a major desktop presentation zone.
 */
typedef struct gui_panel_rect {
    int x;
    int y;
    int width;
    int height;
    const char *title;
} gui_panel_rect_t;

/**
 * Unified desktop presentation layout geometry.
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
 * Interactive button widget definition in screen coordinates.
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
 * Scrollable ring-buffer storing recent chronicle narrative events.
 */
typedef struct gui_log {
    char lines[GUI_MAX_LOG_LINES][GUI_LOG_ENTRY_LEN];
    int count;
} gui_log_t;

/**
 * Canonical asset identifier for characters, monsters, terrain, and relics.
 */
typedef enum gui_asset_id {
    ASSET_NONE = 0,
    ASSET_HERO_ELIJAH = 1,
    ASSET_HERO_KATRINA = 2,
    ASSET_HERO_CASATTI = 3,
    ASSET_ENEMY_DREAM_EATER = 10,
    ASSET_ENEMY_SHADOW_STALKER = 11,
    ASSET_ENEMY_DARK_LORD = 12,
    ASSET_TILE_COBBLESTONE = 20,
    ASSET_TILE_GRASS = 21,
    ASSET_TILE_STONE_WALL = 22,
    ASSET_TILE_VOID_CRAG = 23,
    ASSET_TILE_WATER = 24,
    ASSET_RELIC_SHIELD_LUX = 30,
    ASSET_RELIC_SWORD_TENEBRIS = 31,
    ASSET_VFX_LUX_BARRIO = 40,
    ASSET_VFX_CUTTATIO_SLASH = 41
} gui_asset_id_t;

/**
 * Asset pipeline origin and representation kind.
 */
typedef enum gui_asset_kind {
    ASSET_KIND_PROCEDURAL = 0,    /**< Synthesized geometric vector art */
    ASSET_KIND_BITMAP = 1         /**< External PNG/BMP artist spritesheet */
} gui_asset_kind_t;

/**
 * Asset specification in the Asset Abstraction Layer.
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
 * Isometric screen point with elevation depth.
 */
typedef struct gui_iso_point {
    int screen_x;
    int screen_y;
    int elevation_z;
} gui_iso_point_t;

/**
 * Discrete grid coordinate.
 */
typedef struct gui_grid_coord {
    int gx;
    int gy;
} gui_grid_coord_t;

/**
 * Depth-sorted entity render packet for the Painter's Algorithm.
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

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Functions declared in domain-specific headers )
// =============================================================================

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Subsystem audits declared in domain-specific headers )
// =============================================================================

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_GUI_TYPES_H */
