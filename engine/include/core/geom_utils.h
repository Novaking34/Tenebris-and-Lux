/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          geom_utils.h
@id            core.util.geom
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
================================================================================
Mission: Viewport math, 2.5D isometric projections, bounding boxes, include ladder,
manifest hooks, structural ontology, and pre-conditional static validation inlines.
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
#include "core/geom_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Geometric Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for spatial viewports, rectangles, and tile steps.
For Designers: Boundaries ensure retro 640x360 coordinates and 32x16 isometric
diamond tiles scale smoothly to modern 1080p and 4K displays without blurring.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the 2D Spatial Geometry Engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the 2D Spatial Geometry Engine. */
#define GEOM_UTILS_MODULE_ID           "core.geom_utils"

/**
 * Manifest record for the 2D Spatial Geometry Engine.
 */
extern const module_metadata_t geom_utils_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for 2D bounding boxes, points, viewports, and isometric grids.
--------------------------------------------------------------------------------
*/

/**
 * @struct geom_rect
 * @brief Axis-Aligned Bounding Box (AABB) in 2D discrete integer screen space.
 *
 * For Designers: A rectangular boundary box used for UI buttons, character hitboxes,
 * and menu panels.
 *
 * For Programmers: Explicitly non-negative integer dimensions in screen space.
 */
typedef struct geom_rect {
    int x; /**< Top-left horizontal coordinate in screen space. */
    int y; /**< Top-left vertical coordinate in screen space. */
    int w; /**< Bounding box width (horizontal extent, w >= 0). */
    int h; /**< Bounding box height (vertical extent, h >= 0). */
} geom_rect_t;

/**
 * @struct geom_point2d
 * @brief Discrete 2D point coordinate.
 *
 * For Designers: A single pixel location on the screen.
 *
 * For Programmers: Integer coordinates (x, y) representing screen or grid points.
 */
typedef struct geom_point2d {
    int32_t x; /**< Horizontal coordinate. */
    int32_t y; /**< Vertical coordinate. */
} geom_point2d_t;

/**
 * @struct geom_viewport
 * @brief Universal dynamic viewport projection parameters adapting to physical screens.
 *
 * For Designers: The mathematical camera that scales our retro 640x360 game window
 * to fit full-screen monitors while adding black letterbox/pillarbox borders.
 *
 * For Programmers: Reentrant viewport transform context with uniform scale and centering.
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

/**
 * @struct geom_grid_coord
 * @brief Discrete 2.5D grid coordinate for tactical tile maps.
 *
 * For Designers: The board game coordinates of a character on the battlefield
 * (column, row, and height elevation step).
 *
 * For Programmers: Pure Layer 1 integer grid unit coordinates.
 */
typedef struct geom_grid_coord {
    int32_t tile_x;      /**< Horizontal grid cell index. */
    int32_t tile_y;      /**< Vertical grid cell index. */
    int32_t elevation_z; /**< Vertical elevation / height layer step. */
} geom_grid_coord_t;

/**
 * @struct geom_iso_config
 * @brief Configuration for 2.5D dimetric/isometric grid projection.
 *
 * For Designers: The visual proportions of battlefield tiles (e.g. 32px wide by 16px tall).
 *
 * For Programmers: Loaded from engine/config/viewport/layout.json.
 */
typedef struct geom_iso_config {
    int32_t tile_width;     /**< Logical diamond width in pixels (e.g. 32). */
    int32_t tile_height;    /**< Logical diamond height in pixels (e.g. 16 for 2:1). */
    int32_t elevation_step; /**< Vertical pixel offset per elevation unit (e.g. 8). */
} geom_iso_config_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a rectangle has valid non-negative dimensions.
 *
 * @param[in] rect Pointer to rectangle record to test.
 * @return true if rect is non-NULL and w >= 0 and h >= 0.
 */
static inline bool geom_rect_is_valid(const geom_rect_t *rect) {
    return (rect != NULL) && (rect->w >= 0) && (rect->h >= 0);
}

/**
 * @brief Predicate verifying if a viewport descriptor has valid screen dimensions and scale.
 *
 * @param[in] vp Pointer to viewport record to test.
 * @return true if vp is non-NULL, screen dimensions are non-zero, and scale > 0.0f.
 */
static inline bool geom_viewport_is_valid(const geom_viewport_t *vp) {
    return (vp != NULL) && (vp->screen_w > 0) && (vp->screen_h > 0) && (vp->scale_factor > 0.0f);
}

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
Mission: Rectangular bounds, point containment, and collision detection.
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs a validated 2D rectangle with non-negative dimensions.
 *
 * ### For Game Designers & Non-Programmers:
 * Defines a box on screen for UI windows, buttons, or unit hitboxes, ensuring width
 * and height can never be negative.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamps w and h to 0 if negative, returning a normalized geom_rect_t structure by value.
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
 * ### For Game Designers & Non-Programmers:
 * Checks whether the mouse cursor or tap position is currently inside a menu button or panel.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates half-open inclusion: (px >= r.x && px < r.x + r.w && py >= r.y && py < r.y + r.h).
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
 * ### For Game Designers & Non-Programmers:
 * Determines if two characters or spell effects overlap on screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Standard 2D Separating Axis Test (SAT) for axis-aligned bounding boxes.
 *
 * @param[in] a First rectangle.
 * @param[in] b Second rectangle.
 * @return True if a and b share non-zero area; false otherwise.
 */
bool geom_rect_intersects(geom_rect_t a, geom_rect_t b);

/**
 * @brief Verifies that a bounding box resides entirely within viewport limits.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures popups and tooltips do not bleed outside the player's screen boundaries.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies (r.x >= 0 && r.y >= 0 && r.x + r.w <= max_w && r.y + r.h <= max_h).
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
Mission: Aspect ratio preservation, camera projection, and screen-to-world conversion.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes a viewport structure computing uniform scale and centering offsets.
 *
 * ### For Game Designers & Non-Programmers:
 * Scales our 640x360 game to fit any monitor resolution without stretching, adding
 * black bars on top/bottom (letterbox) or sides (pillarbox) as needed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes uniform isotropic scale multiplier and integer centering pixel offsets.
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
 * ### For Game Designers & Non-Programmers:
 * Calculates where an in-game hero or enemy should appear on the player's physical monitor.
 *
 * ### For Systems Programmers & Engine Architects:
 * Applies camera offset, viewport isotropic scaling, and letterbox centering.
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
 * ### For Game Designers & Non-Programmers:
 * Converts mouse clicks on the physical monitor into exact coordinates in the game world.
 *
 * ### For Systems Programmers & Engine Architects:
 * Subtracts centering offset, inverts viewport scale factor, and adds camera translation.
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
 * ### For Game Designers & Non-Programmers:
 * Prevents the game camera from scrolling off the edges of the battlefield map into empty space.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamps cam_x in [0, max_world_w - view_w] and cam_y in [0, max_world_h - view_h].
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
 * ### For Game Designers & Non-Programmers:
 * Checks whether an enemy or treasure chest is currently visible to the player or offscreen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fast 2D bounding frustum culling against (cam_x, cam_y, view_w, view_h).
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
Mission: Dimetric 2:1 tile projections, inverse grid math, and tactical distance metrics.
--------------------------------------------------------------------------------
*/

/**
 * @brief Projects a 2.5D tactical grid coordinate to 2D screen pixels.
 *
 * ### For Game Designers & Non-Programmers:
 * Places characters and tactical terrain diamonds on the classic isometric battlefield.
 *
 * ### For Systems Programmers & Engine Architects:
 * Dimetric 2:1 projection: screen_x = (x - y) * (W/2), screen_y = (x + y) * (H/2) - (Z * step).
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
 * ### For Game Designers & Non-Programmers:
 * Tells you which diamond tile on the battlefield grid the player clicked on.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reversible inverse dimetric matrix transformation with floor rounding.
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
 *
 * ### For Game Designers & Non-Programmers:
 * Standard tactical movement range: movement across 4 orthogonal directions (no diagonals).
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes taxicab L1 norm |dx| + |dy| using 32-bit integer arithmetic.
 *
 * @param[in] x1 Source tile X.
 * @param[in] y1 Source tile Y.
 * @param[in] x2 Target tile X.
 * @param[in] y2 Target tile Y.
 * @return Manhattan distance in grid steps.
 */
int32_t geom_distance_manhattan(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * @brief Calculates Chebyshev distance (max(|x1 - x2|, |y1 - y2|)) between two grid cells.
 *
 * ### For Game Designers & Non-Programmers:
 * Measures 8-direction movement range where diagonal steps cost the same as orthogonal steps.
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes L-infinity norm max(|dx|, |dy|).
 *
 * @param[in] x1 Source tile X.
 * @param[in] y1 Source tile Y.
 * @param[in] x2 Target tile X.
 * @param[in] y2 Target tile Y.
 * @return Chebyshev distance in grid steps.
 */
int32_t geom_distance_chebyshev(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * @brief Calculates squared Euclidean distance ((x1 - x2)^2 + (y1 - y2)^2).
 *
 * ### For Game Designers & Non-Programmers:
 * High-speed straight-line distance check without expensive square-root operations.
 *
 * ### For Systems Programmers & Engine Architects:
 * 64-bit integer computation avoiding overflow and avoiding floating-point sqrtf().
 *
 * @param[in] x1 Source point X.
 * @param[in] y1 Source point Y.
 * @param[in] x2 Target point X.
 * @param[in] y2 Target point Y.
 * @return Squared Euclidean distance.
 */
int64_t geom_distance_euclidean_sq(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D/2.5D Spatial Geometry Engine
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
(Viewport scaling factors, letterbox bounds, dimetric 2:1 tile ratio invariants,
and distance metric precision are verified via pre-flight checks and unit suites.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on geometric invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the game's spatial mathematics engine to ensure battlefield coordinates,
 * mouse clicking on tiles, and letterbox scaling calculations are 100% accurate.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates aspect ratio preservation, coordinate inversion reversibility,
 * dimetric isometric 2:1 projection symmetry, distance calculations, and fallback integrity.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t geom_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated pass/fail health check verifying coordinate system stability.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation harness exercising all geometric invariant tripwires.
 *
 * @return true if all geometric invariant tripwires pass cleanly; false otherwise.
 */
bool geom_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D/2.5D Spatial Geometry Engine
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
 * @brief Formats an ASCII diagnostic inspection card for viewport and geometry state.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing screen resolution, game scale, letterboxing, and tile projection parameters.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats 60-column terminal-friendly status card of geom_viewport_t into out_buf without dynamic heap allocation.
 *
 * @param[in]  vp       Active viewport state.
 * @param[out] out_buf  Destination buffer (minimum 300 bytes).
 * @param[in]  buf_size Size of destination buffer.
 * @return True if formatted cleanly; false on error.
 */
bool geom_execute_dump_card(const geom_viewport_t *vp, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_GEOM_UTILS_H */
