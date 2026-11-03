// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          map_scaler.h
// @id            sim.world.map_scaler
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
//   Dynamic map viewport scaler and camera coordinate projection engine.
//   Decouples fixed invariant world space from arbitrary screen resolutions,
//   computing isotropic aspect-ratio scaling, letterboxing/pillarboxing,
//   and camera boundary clamping with strict zero dynamic heap allocation.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/world/map_types.h, util/manifest.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure mathematical transforms).
//   - Latency Budget:        < 5 microseconds per coordinate transformation.
//
// AXIOMATIC INVARIANTS:
//   1. Scale factor S > 0.0f strictly guaranteed.
//   2. Screen-to-world and world-to-screen transforms are exact inverses.
//   3. Clamped camera coordinates never breach [0, world_max - view_size].
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_WORLD_MAP_SCALER_H
#define TENEBRIS_LUX_SIM_WORLD_MAP_SCALER_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/world/map_types.h"
#include "util/manifest.h"
#include <stdbool.h>
#include <stdint.h>

extern const module_metadata_t map_scaler_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// =============================================================================

/**
 * @brief Computes viewport isotropic scale factor and centering offsets.
 *
 * @param[out] vp            Pointer to viewport structure to initialize.
 * @param[in]  screen_w      Current physical screen width in pixels.
 * @param[in]  screen_h      Current physical screen height in pixels.
 * @param[in]  target_view_w Target unscaled reference viewport width.
 * @param[in]  target_view_h Target unscaled reference viewport height.
 */
void map_scaler_init_viewport(map_viewport_t *vp,
                              uint32_t screen_w, uint32_t screen_h,
                              uint32_t target_view_w, uint32_t target_view_h);

/**
 * @brief Projects a 2D world coordinate into 2D screen pixel space.
 *
 * @param[in]  world_x     World horizontal coordinate.
 * @param[in]  world_y     World vertical coordinate.
 * @param[in]  cam         Active presentation camera.
 * @param[in]  vp          Active dynamic viewport.
 * @param[out] out_screen_x Transformed screen horizontal pixel.
 * @param[out] out_screen_y Transformed screen vertical pixel.
 * @return True if transformed successfully, false if parameters invalid.
 */
bool map_scaler_world_to_screen(int32_t world_x, int32_t world_y,
                                const map_camera_t *cam,
                                const map_viewport_t *vp,
                                int32_t *out_screen_x,
                                int32_t *out_screen_y);

/**
 * @brief Projects screen pixel coordinates back into invariant world space.
 *
 * @param[in]  screen_x    Screen pixel horizontal coordinate.
 * @param[in]  screen_y    Screen pixel vertical coordinate.
 * @param[in]  cam         Active presentation camera.
 * @param[in]  vp          Active dynamic viewport.
 * @param[out] out_world_x  Inverted world horizontal coordinate.
 * @param[out] out_world_y  Inverted world vertical coordinate.
 * @return True if inverted successfully, false if clicked outside viewport.
 */
bool map_scaler_screen_to_world(int32_t screen_x, int32_t screen_y,
                                const map_camera_t *cam,
                                const map_viewport_t *vp,
                                int32_t *out_world_x,
                                int32_t *out_world_y);

/**
 * @brief Clamps camera origin to invariant world boundaries.
 *
 * @param[in,out] cam Active presentation camera.
 * @param[in]     dim Dimensions of current active map.
 * @param[in]     vp  Active dynamic viewport.
 */
void map_scaler_clamp_camera(map_camera_t *cam,
                             const map_dimensions_t *dim,
                             const map_viewport_t *vp);

/**
 * @brief Checks if a world coordinate is currently inside visible viewport.
 *
 * @param[in] world_x Horizontal world position.
 * @param[in] world_y Vertical world position.
 * @param[in] cam     Active camera.
 * @param[in] vp      Active viewport.
 * @return True if point is inside viewport boundaries, false otherwise.
 */
bool map_scaler_is_visible(int32_t world_x, int32_t world_y,
                           const map_camera_t *cam,
                           const map_viewport_t *vp);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Pre-flight self-audit verifying scaler mathematical invariants.
 *
 * @return Diagnostic check outcome.
 */
diag_result_t map_scaler_preflight(void);

#endif // TENEBRIS_LUX_SIM_WORLD_MAP_SCALER_H
