/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          geom_utils_fallback.h
@id            core.util.geom_fallback
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
================================================================================
Mission: Quarantined emergency fallback limits, include ladder, and static
helper predicates for geometric operations when viewport configs are offline.
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Fallback Geometric Constants
--------------------------------------------------------------------------------
Compile-time static defaults for emergency boot and headless geometry tests.
For Designers: Bounds ensure the tactical battlefield grid defaults to 32x16
diamond tiles at 640x360 resolution even if screen settings are missing.
For Programmers: Immutable constants residing in .rodata with zero heap allocation.
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
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the fallback code page.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Geometry Utilities Fallback subsystem. */
#define GEOM_UTILS_FALLBACK_MODULE_ID  "core.geom_utils.fallback"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Fallback Object Definitions
--------------------------------------------------------------------------------
Self-contained fallback profile descriptors and configuration structures.
--------------------------------------------------------------------------------
*/

/**
 * @struct geom_fallback_viewport
 * @brief Self-contained fallback viewport structure.
 *
 * For Designers: An emergency record defining screen centering and letterbox
 * black bars so retro games scale correctly on modern wide monitors.
 *
 * For Programmers: Invariant loaded_from_data is strictly false.
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
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline inspection predicates for the fallback code page.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if screen dimensions are non-zero.
 *
 * @param[in] w Width in pixels to check.
 * @param[in] h Height in pixels to check.
 * @return true if w > 0 and h > 0.
 */
static inline bool geom_fallback_dim_is_valid(uint32_t w, uint32_t h) {
    return w > 0 && h > 0;
}

/**
 * @brief Predicate verifying if an inspected profile name matches the emergency fallback.
 *
 * @param[in] name Profile name string to compare.
 * @return true if name matches GEOM_FALLBACK_PROFILE_NAME; false otherwise.
 */
static inline bool geom_fallback_is_profile_name(const char *name) {
    if (name == NULL) {
        return false;
    }
    const char *expected = GEOM_FALLBACK_PROFILE_NAME;
    while (*name && *expected) {
        if (*name != *expected) {
            return false;
        }
        name++;
        expected++;
    }
    return *name == *expected;
}

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
Mission: Emergency fallback viewport scaling, hit-testing, and box overlap.
--------------------------------------------------------------------------------
*/

/**
 * @brief Computes fallback isotropic viewport scaling and centering offsets.
 *
 * ### For Game Designers & Non-Programmers:
 * Calculates aspect-ratio-preserving screen scaling for emergency boot and headless testing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant viewport computation with 640x360 reference resolution and zero heap allocation.
 *
 * @param[in] screen_w Target physical screen width.
 * @param[in] screen_h Target physical screen height.
 * @return Fully-initialized geom_fallback_viewport_t structure.
 */
geom_fallback_viewport_t geom_fallback_get_viewport(uint32_t screen_w, uint32_t screen_h);

/**
 * @brief Fallback point-in-rectangle test without engine dependencies.
 *
 * ### For Game Designers & Non-Programmers:
 * Simple boundary check testing if a pixel coordinate is inside a rectangular button.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates half-open 2D coordinate inclusion without external math dependencies.
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
 * ### For Game Designers & Non-Programmers:
 * Determines if two rectangular bounding boxes touch or overlap on screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Separating Axis Test (SAT) for 2D rectangles in pure ISO C11.
 *
 * @param[in] ax First box X.
 * @param[in] ay First box Y.
 * @param[in] aw First box width.
 * @param[in] ah First box height.
 * @param[in] bx Second box X.
 * @param[in] by Second box Y.
 * @param[in] bw Second box width.
 * @param[in] bh Second box height.
 * @return True if rectangles a and b overlap with non-zero area.
 */
bool geom_fallback_rect_intersects(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
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
(Fallback 640x360 reference dimensions, point-in-rect bounds, and AABB overlap
invariants are verified via pre-flight diagnostics in Sub-Assembly 4.2.)
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
 * @brief Pre-flight diagnostic check evaluating fallback geometric invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the emergency coordinate backup settings to make sure screen scaling
 * and button hit-testing work properly before use.
 *
 * ### For Systems Programmers & Engine Architects:
 * Asserts fallback viewport letterboxing, rectangle intersection tests,
 * loaded_from_data == false, and profile identifier matching.
 *
 * @return diag_result_t reporting categorized diagnostic outcome.
 */
diag_result_t geom_utils_fallback_preflight(void);

/**
 * @brief Standalone boolean audit wrapper for regression test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated check ensuring emergency geometry backup code is 100% operational.
 *
 * ### For Systems Programmers & Engine Architects:
 * High-level boolean validation audit asserting that all geometric fallback tripwires pass.
 *
 * @return true if all geometric fallback tripwires pass cleanly; false otherwise.
 */
bool geom_fallback_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: 2D Spatial Geometry Engine
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
 * @brief Formats an ASCII diagnostic inspection card for fallback geometry state.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing emergency 640x360 reference resolution and letterboxing offsets.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats a 60-column terminal-friendly status card into out_buf without dynamic heap allocation.
 *
 * @param[in]  vp       Optional pointer to fallback viewport, or NULL to compute from 640x360.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return Number of characters written (excluding null terminator), or 0 on error.
 */
size_t geom_fallback_execute_dump_card(const geom_fallback_viewport_t *vp,
                                       char *out_buf,
                                       size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_GEOM_UTILS_FALLBACK_H */
