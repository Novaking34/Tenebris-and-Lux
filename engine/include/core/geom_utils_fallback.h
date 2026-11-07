/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          geom_utils_fallback.h
@id            core.util.geom_fallback
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        2D/2.5D Spatial Geometry Subsystem (core/geom)
@role          Quarantined Monolithic Emergency Default Code Page & Spatial Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined execution path: Never invoked in normal production.
               4. Provenance tracking: loaded_from_data is unconditionally false.
               5. Canonical fallback viewport: 640x360 reference logical frame.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Isolated, monolithic emergency default code page and tripwire fallback for
  the 2D/2.5D spatial geometry subsystem. Quarantined strictly outside the main
  production execution path. Provides compile-time static viewport dimensions,
  isometric tile dimensions, and fallback point-in-rect and AABB intersection
  math routines when external assets or manifests are offline.

  ### For Game Designers & Non-Programmers:
  If display resolution settings fail to load, this code page acts like an
  emergency test-pattern card, locking the window into a classic 640x360
  Game Boy / retro aspect ratio so the game never crashes with a black screen.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Generates immutable, bounds-checked fallback viewports on the stack.
  - Reentrant pure math for point hit-testing and AABB separation.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_GEOM_UTILS_FALLBACK_H
#define TENEBRIS_LUX_CORE_GEOM_UTILS_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & STATIC CONFIGURATION CONSTANTS
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Fallback Geometric Constants
--------------------------------------------------------------------------------
*/

/** Canonical profile identifier for geometric fallback. */
#define GEOM_FALLBACK_PROFILE_NAME    "geom_canonical_fallback"

/** Canonical reference logical screen width in pixels (640). */
#define GEOM_FALLBACK_TARGET_W        640

/** Canonical reference logical screen height in pixels (360). */
#define GEOM_FALLBACK_TARGET_H        360

/** Canonical 2.5D isometric diamond tile width in pixels (32). */
#define GEOM_FALLBACK_TILE_W          32

/** Canonical 2.5D isometric diamond tile height in pixels (16 for 2:1 dimetric). */
#define GEOM_FALLBACK_TILE_H          16

/** Canonical elevation vertical step in pixels (8). */
#define GEOM_FALLBACK_ELEVATION_STEP  8

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Fallback Viewport Data Structures
--------------------------------------------------------------------------------
*/

/**
 * @brief Self-contained fallback viewport structure.
 */
typedef struct geom_fallback_viewport {
    uint32_t screen_w;     /**< Physical screen width in pixels. */
    uint32_t screen_h;     /**< Physical screen height in pixels. */
    uint32_t target_w;     /**< Canonical logical width (640). */
    uint32_t target_h;     /**< Canonical logical height (360). */
    float    scale_factor; /**< Uniform isotropic scale multiplier. */
    int32_t  offset_x;     /**< Pillarbox centering offset. */
    int32_t  offset_y;     /**< Letterbox centering offset. */
} geom_fallback_viewport_t;

/*
================================================================================
BLOCK 3: BODY BLOCK / EXPORTED FALLBACK COGS
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Viewport & Bounding Box Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Computes fallback isotropic viewport scaling and centering offsets.
 *
 * @param[in] screen_w Target physical screen width.
 * @param[in] screen_h Target physical screen height.
 * @return Fully-initialized geom_fallback_viewport_t structure.
 */
geom_fallback_viewport_t geom_fallback_get_viewport(uint32_t screen_w, uint32_t screen_h);

/**
 * @brief Fallback point-in-rectangle test without engine dependencies.
 *
 * @param[in] px Point X.
 * @param[in] py Point Y.
 * @param[in] rx Rectangle origin X.
 * @param[in] ry Rectangle origin Y.
 * @param[in] rw Rectangle width.
 * @param[in] rh Rectangle height.
 * @return True if (px, py) is contained within [rx, rx+rw] x [ry, ry+rh].
 */
bool geom_fallback_point_in_rect(int px, int py, int rx, int ry, int rw, int rh);

/**
 * @brief Fallback AABB intersection evaluator without engine dependencies.
 *
 * @return True if rectangles a and b overlap with non-zero area.
 */
bool geom_fallback_rect_intersects(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating fallback geometric invariants.
 *
 * @return Diagnostic check outcome reporting PASS or FATAL.
 */
diag_result_t geom_utils_fallback_preflight(void);

/**
 * @brief Standalone boolean audit wrapper for regression test harnesses.
 *
 * @return True if all geometric fallback tripwires pass cleanly.
 */
bool geom_fallback_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Include Guard Teardown & Header Close )
================================================================================
*/

#endif /* TENEBRIS_LUX_CORE_GEOM_UTILS_FALLBACK_H */
