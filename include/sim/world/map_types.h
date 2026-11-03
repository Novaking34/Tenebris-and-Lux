// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          map_types.h
// @id            sim.world.map_types
// @layer         Layer 1: Simulation Engine
// @subsystem     World Navigation & Dynamic Map Strata (sim/world)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Ontological type definitions for maps unbound from RPG Maker tile schemas.
//   Formalizes multi-tier classifications (Overworld, Tactical Grids, Town
//   Exteriors, Interior Chambers, Dungeon Labyrinths), cell passability,
//   camera viewports, and dynamic projection boundaries.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational simulation leaf).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure data structures).
//   - Latency Budget:        < 5 microseconds per spatial query.
//
// AXIOMATIC INVARIANTS:
//   1. World coordinates remain invariant and independent of display scaling.
//   2. Cell coordinates are bounded within [0, width) and [0, height).
//   3. Viewport scale factor is strictly positive (> 0.0f).
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_WORLD_MAP_TYPES_H
#define TENEBRIS_LUX_SIM_WORLD_MAP_TYPES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_MAP_PORTALS       8
#define MAX_MAP_ID_LEN        32
#define MAX_MAP_NAME_LEN      48
#define MAX_MAP_DESC_LEN      160
#define MAX_MAP_GRID_WIDTH    64
#define MAX_MAP_GRID_HEIGHT   64

/**
 * @brief High-level taxonomic category of a game map.
 */
typedef enum {
    MAP_TYPE_PROVINCIAL_OVERWORLD = 0,
    MAP_TYPE_TACTICAL_GRID        = 1,
    MAP_TYPE_SETTLEMENT_EXTERIOR  = 2,
    MAP_TYPE_INTERIOR_CHAMBER     = 3,
    MAP_TYPE_DUNGEON_LABYRINTH    = 4,
    MAP_TYPE_COUNT
} map_type_t;

/**
 * @brief Environmental context governing lighting and ambient rules.
 */
typedef enum {
    MAP_ENV_EXTERIOR    = 0,
    MAP_ENV_INTERIOR    = 1,
    MAP_ENV_ASTRAL      = 2,
    MAP_ENV_BATTLE_ZONE = 3,
    MAP_ENV_COUNT
} map_env_t;

/**
 * @brief Terrain classification for individual tactical cells.
 */
typedef enum {
    TERRAIN_PLAIN    = 0,
    TERRAIN_ROAD     = 1,
    TERRAIN_FOREST   = 2,
    TERRAIN_STONE    = 3,
    TERRAIN_WATER    = 4,
    TERRAIN_WALL     = 5,
    TERRAIN_VOID     = 6,
    TERRAIN_ELEVATED = 7,
    TERRAIN_COUNT
} map_terrain_t;

/**
 * @brief Representation of an individual spatial grid cell.
 */
typedef struct {
    int16_t       x;
    int16_t       y;
    map_terrain_t terrain;
    int8_t        elevation;
    bool          passable;
    bool          blocks_los;
    int8_t        movement_cost;
    int8_t        cover_defense_bonus;
} map_cell_t;

/**
 * @brief Invariant physical dimensions of a map in world units.
 */
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t cell_size_px;
} map_dimensions_t;

/**
 * @brief Dynamic presentation camera tracking protagonist in world space.
 */
typedef struct {
    int32_t cam_x;
    int32_t cam_y;
    int32_t target_x;
    int32_t target_y;
} map_camera_t;

/**
 * @brief Dynamic viewport projection parameters adapting to screen size.
 */
typedef struct {
    uint32_t screen_w;
    uint32_t screen_h;
    uint32_t target_view_w;
    uint32_t target_view_h;
    float    scale_factor;
    int32_t  offset_x;
    int32_t  offset_y;
} map_viewport_t;

/**
 * @brief Interactive portal linking maps together.
 */
typedef struct {
    int16_t x;
    int16_t y;
    char    target_map_id[MAX_MAP_ID_LEN];
    char    portal_name[MAX_MAP_NAME_LEN];
} map_portal_t;

/**
 * @brief Complete canonical map definition.
 */
typedef struct {
    char             id[MAX_MAP_ID_LEN];
    char             canonical_name[MAX_MAP_NAME_LEN];
    map_type_t       type;
    map_env_t        environment;
    map_dimensions_t dimensions;
    int16_t          spawn_x;
    int16_t          spawn_y;
    uint32_t         portal_count;
    map_portal_t     portals[MAX_MAP_PORTALS];
    char             description[MAX_MAP_DESC_LEN];
} map_def_t;

#endif // TENEBRIS_LUX_SIM_WORLD_MAP_TYPES_H
