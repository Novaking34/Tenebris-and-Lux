/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          geom_utils.h
@id            core.util.geom
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        2D/2.5D Spatial Geometry Subsystem (core/geom)
@role          2D Bounding Boxes, Viewport Scaling, Camera Projections & Isometric Math
@invariants    1. Non-negative dimensions: geom_make_rect clamps negative w or h to 0.
               2. Screen-space coordinates: Origin (0,0) at top-left.
               3. Scale factor S > 0.0f strictly guaranteed.
               4. Screen-to-world and world-to-screen transforms are exact inverses.
               5. Camera clamp strictly bounds camera inside [0, max_world - view_size].
               6. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack value structures).
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  2D geometry structures, bounding box overlap (AABB), hit-testing primitives,
  elevated isotropic viewport scalers, camera coordinate transformations,
  letterbox/pillarbox computation, and 2.5D classic isometric/dimetric grid
  projections. Serves as universal spatial apparatus for all viewports and games.

  ### For Game Designers & Non-Programmers:
  Provides the spatial layout and math for buttons, HUD meters, tactical grid
  diamonds, and camera tracking. Ensures the game always centers properly on
  any display with classic black bars (letterboxing) instead of stretching.

  ### For Systems Programmers & Engine Architects:
  - 100% Stack Allocation: Zero heap usage (ALLOC_ZERO_HEAP).
  - Exact roundtrip coordinate reversibility: `screen_to_world(world_to_screen(p)) == p`.
  - Dimetric 2:1 isometric projection matrix for classic tactical grid rendering.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_GEOM_UTILS_H
#define TENEBRIS_LUX_CORE_GEOM_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: 2D/2.5D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy (Downward Dependency Ladder)
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
#include "core/geom_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Manifest Anchor
--------------------------------------------------------------------------------
*/

/**
 * Manifest record for the 2D Spatial Geometry Engine.
 */
extern const module_metadata_t geom_utils_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: 2D Spatial Bounding Box & Point Records
--------------------------------------------------------------------------------
*/

/**
 * Axis-Aligned Bounding Box (AABB) in 2D discrete integer screen space.
 */
typedef struct {
    int x; /**< Top-left horizontal coordinate in screen space. */
    int y; /**< Top-left vertical coordinate in screen space. */
    int w; /**< Bounding box width (horizontal extent, w >= 0). */
    int h; /**< Bounding box height (vertical extent, h >= 0). */
} geom_rect_t;

/**
 * Discrete 2D point coordinate.
 */
typedef struct {
    int32_t x; /**< Horizontal coordinate. */
    int32_t y; /**< Vertical coordinate. */
} geom_point2d_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Universal Dynamic Viewport Data Structure
--------------------------------------------------------------------------------
*/

/**
 * Universal dynamic viewport projection parameters adapting to physical screens.
 */
typedef struct geom_viewport {
    uint32_t screen_w;     /**< Physical screen width in pixels. */
    uint32_t screen_h;     /**< Physical screen height in pixels. */
    uint32_t target_w;     /**< Logical unscaled reference width (e.g. 640). */
    uint32_t target_h;     /**< Logical unscaled reference height (e.g. 360). */
    float    scale_factor; /**< Uniform isotropic scaling multiplier (> 0.0f). */
    int32_t  offset_x;     /**< Horizontal pillarbox centering offset in pixels. */
    int32_t  offset_y;     /**< Vertical letterbox centering offset in pixels. */
} geom_viewport_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: 2.5D Tactical Grid & Isometric Projection Records
--------------------------------------------------------------------------------
*/

/**
 * Discrete 2.5D grid coordinate for tactical tile maps.
 */
typedef struct geom_grid_coord {
    int32_t tile_x;      /**< Horizontal grid cell index. */
    int32_t tile_y;      /**< Vertical grid cell index. */
    int32_t elevation_z; /**< Vertical elevation / height layer step. */
} geom_grid_coord_t;

/**
 * Configuration for 2.5D dimetric/isometric grid projection.
 */
typedef struct geom_iso_config {
    int32_t tile_width;     /**< Logical diamond width in pixels (e.g. 32). */
    int32_t tile_height;    /**< Logical diamond height in pixels (e.g. 16 for 2:1). */
    int32_t elevation_step; /**< Vertical pixel offset per elevation unit (e.g. 8). */
} geom_iso_config_t;

/*
================================================================================
BLOCK 3: BODY BLOCK / EXPORTED ENGINE API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Machine: 2D/2.5D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: 2D Bounding Box & Hit-Testing Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs a validated 2D rectangle with non-negative dimensions.
 *
 * @param[in] x Top-left X.
 * @param[in] y Top-left Y.
 * @param[in] w Width (negative clamped to 0).
 * @param[in] h Height (negative clamped to 0).
 * @return Normalized geom_rect_t structure.
 */
geom_rect_t geom_make_rect(int x, int y, int w, int h);

/**
 * @brief Tests whether a 2D screen-space point lies within a bounding box.
 *
 * @param[in] px Point X.
 * @param[in] py Point Y.
 * @param[in] r  Target rectangle.
 * @return True if (px, py) is inside r; false otherwise.
 */
bool geom_point_in_rect(int px, int py, geom_rect_t r);

/**
 * @brief Tests whether two 2D Axis-Aligned Bounding Boxes (AABBs) intersect.
 *
 * @param[in] a First rectangle.
 * @param[in] b Second rectangle.
 * @return True if a and b share non-zero area; false otherwise.
 */
bool geom_rect_intersects(geom_rect_t a, geom_rect_t b);

/**
 * @brief Verifies that a bounding box resides entirely within viewport limits.
 *
 * @param[in] r     Target rectangle.
 * @param[in] max_w Viewport maximum width.
 * @param[in] max_h Viewport maximum height.
 * @return True if r is within [0, max_w] x [0, max_h].
 */
bool geom_rect_within_bounds(geom_rect_t r, int max_w, int max_h);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Universal Viewport Scaler & Camera Cogs (Elevated)
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes a viewport structure computing uniform scale and centering offsets.
 *
 * @param[out] vp       Pointer to viewport structure to initialize.
 * @param[in]  screen_w Physical screen width in pixels.
 * @param[in]  screen_h Physical screen height in pixels.
 * @param[in]  target_w Canonical logical reference width.
 * @param[in]  target_h Canonical logical reference height.
 */
void geom_viewport_init(geom_viewport_t *vp,
                        uint32_t screen_w, uint32_t screen_h,
                        uint32_t target_w, uint32_t target_h);

/**
 * @brief Projects a 2D world coordinate into 2D screen pixel space.
 *
 * @param[in]  world_x      World horizontal coordinate.
 * @param[in]  world_y      World vertical coordinate.
 * @param[in]  cam_x        Camera horizontal position.
 * @param[in]  cam_y        Camera vertical position.
 * @param[in]  vp           Active dynamic viewport parameters.
 * @param[out] out_screen_x Destination for screen horizontal pixel.
 * @param[out] out_screen_y Destination for screen vertical pixel.
 * @return True if transformed successfully; false if parameters invalid.
 */
bool geom_world_to_screen(int32_t world_x, int32_t world_y,
                          int32_t cam_x, int32_t cam_y,
                          const geom_viewport_t *vp,
                          int32_t *out_screen_x, int32_t *out_screen_y);

/**
 * @brief Projects screen pixel coordinates back into invariant world space.
 *
 * @param[in]  screen_x    Screen pixel horizontal coordinate.
 * @param[in]  screen_y    Screen pixel vertical coordinate.
 * @param[in]  cam_x       Camera horizontal position.
 * @param[in]  cam_y       Camera vertical position.
 * @param[in]  vp          Active dynamic viewport parameters.
 * @param[out] out_world_x Destination for world horizontal coordinate.
 * @param[out] out_world_y Destination for world vertical coordinate.
 * @return True if inverted successfully; false if clicked outside viewport.
 */
bool geom_screen_to_world(int32_t screen_x, int32_t screen_y,
                          int32_t cam_x, int32_t cam_y,
                          const geom_viewport_t *vp,
                          int32_t *out_world_x, int32_t *out_world_y);

/**
 * @brief Clamps camera coordinates within world boundary limits.
 *
 * @param[in,out] cam_x       Pointer to camera horizontal coordinate.
 * @param[in,out] cam_y       Pointer to camera vertical coordinate.
 * @param[in]     max_world_w Total world width in pixels.
 * @param[in]     max_world_h Total world height in pixels.
 * @param[in]     view_w      Logical viewport width in pixels.
 * @param[in]     view_h      Logical viewport height in pixels.
 */
void geom_clamp_camera(int32_t *cam_x, int32_t *cam_y,
                       int32_t max_world_w, int32_t max_world_h,
                       int32_t view_w, int32_t view_h);

/**
 * @brief Tests whether a world point is currently inside the active camera view.
 *
 * @param[in] world_x World X.
 * @param[in] world_y World Y.
 * @param[in] cam_x   Camera X.
 * @param[in] cam_y   Camera Y.
 * @param[in] view_w  Viewport logical width.
 * @param[in] view_h  Viewport logical height.
 * @return True if point is within visible camera bounds.
 */
bool geom_is_in_viewport(int32_t world_x, int32_t world_y,
                         int32_t cam_x, int32_t cam_y,
                         int32_t view_w, int32_t view_h);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: 2D/2.5D Isometric & Tactical Distance Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Projects a 2.5D tactical grid coordinate to 2D screen pixels.
 *
 * @param[in]  grid         Grid cell index and elevation.
 * @param[in]  cfg          Isometric tile configuration.
 * @param[in]  origin_x     Screen X origin of tile (0,0).
 * @param[in]  origin_y     Screen Y origin of tile (0,0).
 * @param[out] out_screen_x Transformed screen X.
 * @param[out] out_screen_y Transformed screen Y.
 * @return True if transformed cleanly; false if configuration is invalid.
 */
bool geom_grid_to_screen_iso(geom_grid_coord_t grid, geom_iso_config_t cfg,
                             int32_t origin_x, int32_t origin_y,
                             int32_t *out_screen_x, int32_t *out_screen_y);

/**
 * @brief Inverts 2D screen pixel coordinates back to flat tactical grid cell coordinates (z=0).
 *
 * @param[in]  screen_x   Screen pixel X.
 * @param[in]  screen_y   Screen pixel Y.
 * @param[in]  cfg        Isometric tile configuration.
 * @param[in]  origin_x   Screen X origin of tile (0,0).
 * @param[in]  origin_y   Screen Y origin of tile (0,0).
 * @param[out] out_tile_x Inverted grid cell X.
 * @param[out] out_tile_y Inverted grid cell Y.
 * @return True if inverted cleanly; false on invalid configuration.
 */
bool geom_screen_to_grid_iso(int32_t screen_x, int32_t screen_y,
                             geom_iso_config_t cfg,
                             int32_t origin_x, int32_t origin_y,
                             int32_t *out_tile_x, int32_t *out_tile_y);

/**
 * @brief Calculates Manhattan distance (|x1 - x2| + |y1 - y2|) between two grid cells.
 */
int32_t geom_distance_manhattan(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * @brief Calculates Chebyshev distance (max(|x1 - x2|, |y1 - y2|)) between two grid cells.
 */
int32_t geom_distance_chebyshev(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * @brief Calculates squared Euclidean distance ((x1 - x2)^2 + (y1 - y2)^2).
 */
int64_t geom_distance_euclidean_sq(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: 2D/2.5D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on geometric invariants.
 *
 * @return Diagnostic check outcome reporting PASS or FATAL.
 */
diag_result_t geom_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return True if all geometric invariant tripwires pass cleanly.
 */
bool geom_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / DIAGNOSTIC CARD DUMP & TEARDOWN
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Diagnostic ASCII Status Card Generator
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ASCII diagnostic inspection card for viewport and geometry state.
 *
 * @param[in]  vp       Active viewport state.
 * @param[out] out_buf  Destination buffer (minimum 300 bytes).
 * @param[in]  buf_size Size of destination buffer.
 * @return True if formatted cleanly; false on error.
 */
bool geom_execute_dump_card(const geom_viewport_t *vp, char *out_buf, size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_GEOM_UTILS_H */
