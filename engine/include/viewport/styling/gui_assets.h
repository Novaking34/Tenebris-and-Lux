/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_assets.h
@id            viewport.styling.assets
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Asset Management & Procedural Vector Art Synthesis Pipeline
@role          Asset abstraction layer, procedural 2D/2.5D geometric art synthesis,
               character crests, terrain textures, and artist bitmap slots.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Seamless procedural fallback: missing external artist bitmaps automatically
                  render procedural geometric vector graphics with zero crashes.
               4. Bounded procedural rendering strictly contained within entity bounding rects.
               5. Latency budget: < 50 microseconds per entity sprite draw.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Asset abstraction layer and procedural 2D/2.5D geometric art synthesis engine.
  Generates stylized character crests, terrain textures, isometric blocks, and
  visual effects, while providing clean drop-in binding slots for external artist
  spritesheets and bitmaps defined in engine/config/viewport/assets.json.

  ### For Game Designers & Non-Programmers:
  This module guarantees that the game is 100% playable and looks visually styled
  even if no external PNG image files exist on disk. It can procedurally draw
  character silhouettes, shields, swords, and battlefield tiles using math and
  geometry alone, and seamlessly switches to real art files when they are added.

  ### For Systems Programmers & Engine Architects:
  - Procedural Fallback Floor: ASSET_KIND_PROCEDURAL is always available as baseline.
  - Zero heap allocation: rasterization occurs directly into X11 pixmap surfaces.
  - Tagged asset descriptors (gui_asset_slot_t, gui_asset_registry_t) with
    direct path hints for rapid asset resolution.
  - Reentrant drawing routines with strict coordinate clipping.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_ASSETS_H
#define TENEBRIS_LUX_GUI_ASSETS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Asset descriptors, include ladder, procedural fallback guarantees,
and pre-conditional static validation inlines for visual sprite rendering.
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
Sub-Assembly 2.1: Pre-Conditions & Asset Limits
--------------------------------------------------------------------------------
Compile-time sizing limits ensuring sprite descriptors fit in static memory.
For Designers: Boundaries ensure character and terrain sprites work 100% without
requiring external image files via procedural geometric vector synthesis.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the GUI Assets subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Procedural & Bitmap Asset Manager. */
#define GUI_ASSETS_MODULE_ID           "viewport.assets"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Primary structural ontology defined in viewport/raster/gui_types.h.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an asset identifier is within catalog bounds.
 *
 * @param[in] id Asset enum identifier to check.
 * @return true if id >= 0 and id < ASSET_ID_COUNT.
 */
static inline bool gui_asset_id_is_valid(gui_asset_id_t id) {
    return id >= ASSET_NONE && id <= ASSET_VFX_SLASH;
}

/**
 * @brief Predicate verifying if an asset kind is recognized.
 *
 * @param[in] kind Asset kind enum value to check.
 * @return true if kind is ASSET_KIND_PROCEDURAL or ASSET_KIND_BITMAP.
 */
static inline bool gui_asset_kind_is_valid(gui_asset_kind_t kind) {
    return kind == ASSET_KIND_PROCEDURAL || kind == ASSET_KIND_BITMAP;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Asset queries, path bindings, and procedural vector rasterization cogs.
All execution loops and lifecycle initialization are strictly quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Catalog Query & Binding Cogs
--------------------------------------------------------------------------------
Lookup, external file binding, and resolution cogs for the asset registry.
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves asset metadata for a canonical asset identifier.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the visual asset definition for a game element, telling you its
 * friendly name, pixel dimensions, and whether it uses procedural shapes or artist art.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(N) linear probe over static array `s_catalog`. Returns pointer to static
 * `gui_asset_t` or NULL if unregistered.
 *
 * @param[in] id Target asset enum ID.
 * @return Pointer to asset record, or NULL if unregistered.
 */
const gui_asset_t *gui_assets_get(gui_asset_id_t id);

/**
 * @brief Registers an external artist bitmap/spritesheet file path for an asset.
 *
 * ### For Game Designers & Non-Programmers:
 * Assigns an artist PNG or bitmap file to replace the default mathematical
 * geometric shape for a character, enemy, or tile.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets `artist_file_path` and marks kind as `ASSET_KIND_BITMAP`. Safe zero-heap
 * assignment into static catalog slot.
 *
 * @param[in] id        Target asset enum ID.
 * @param[in] file_path Path to external artist image asset.
 * @return `true` if registered; `false` on invalid ID or NULL path.
 */
bool gui_assets_bind_external(gui_asset_id_t id, const char *file_path);

/**
 * @brief Probes and binds all on-disk artist assets whose files are accessible.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks disk to see which artist PNGs exist, automatically switching found art
 * to bitmap mode while keeping missing art as procedural shapes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Iterates static catalog probing file accessibility via `path_finder_resolve_with_hint`.
 * Missing or null paths transparently retain ASSET_KIND_PROCEDURAL.
 *
 * @return Number of assets successfully bound to external bitmaps.
 */
int gui_assets_resolve_available(void);

/**
 * @brief Returns true if the asset is currently bound to an external artist bitmap.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether an asset is currently showing an external artist image.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries asset kind for `ASSET_KIND_BITMAP`.
 *
 * @param[in] id Target asset identifier.
 * @return true if external bitmap is active; false if procedural or unregistered.
 */
bool gui_assets_is_external(gui_asset_id_t id);

/**
 * @brief Returns true if the asset is utilizing built-in procedural synthesis.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether an asset is currently rendering via procedural geometric math.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries asset kind for `ASSET_KIND_PROCEDURAL`.
 *
 * @param[in] id Target asset identifier.
 * @return true if procedural generator is active; false otherwise.
 */
bool gui_assets_is_procedural(gui_asset_id_t id);

/**
 * @brief Returns count of registered assets matching the given kind.
 *
 * ### For Game Designers & Non-Programmers:
 * Counts how many assets are using artist images versus procedural math.
 *
 * ### For Systems Programmers & Engine Architects:
 * Linear scan of static catalog returning count matching specified `gui_asset_kind_t`.
 *
 * @param[in] kind ASSET_KIND_PROCEDURAL or ASSET_KIND_BITMAP.
 * @return Total asset count for that kind.
 */
int gui_assets_count_by_kind(gui_asset_kind_t kind);

/**
 * @brief Resets all assets to ASSET_KIND_PROCEDURAL.
 *
 * ### For Game Designers & Non-Programmers:
 * Forces all visuals back to procedural geometric shapes, useful for low-memory
 * testing or procedural-only mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets all active catalog entries to `ASSET_KIND_PROCEDURAL`.
 */
void gui_assets_reset_to_procedural(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Procedural & Bitmap Rendering Cogs
--------------------------------------------------------------------------------
Rasterization and drawing routines for sprites, tile terrain, and VFX.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders an entity sprite (hero, companion, or monster) at screen position.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws a hero, companion, or enemy at the specified screen coordinates with
 * directional facing and team colors.
 *
 * ### For Systems Programmers & Engine Architects:
 * If no external artist bitmap is loaded, synthesizes a high-fidelity procedural
 * geometric silhouette with heraldic colors and facing indicators. Bounded within
 * size x size box. Zero dynamic heap allocation.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] id      Asset identifier to draw.
 * @param[in] cx      Center screen X coordinate.
 * @param[in] cy      Center screen Y coordinate.
 * @param[in] size    Bounding box size in pixels.
 * @param[in] facing  Facing direction (0: South, 1: West, 2: East, 3: North).
 * @param[in] pal     Color palette.
 */
void gui_assets_draw_sprite(void *display, unsigned long dr, void *gc,
                            gui_asset_id_t id, int cx, int cy,
                            int size, int facing,
                            const gui_palette_t *pal);

/**
 * @brief Renders procedural terrain detailing onto a diamond or rectangular surface.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws decorative surface patterns onto terrain tiles (like cobblestone cracks,
 * grass tufts, or void fissures).
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders sub-tile line primitives directly into the X11 drawable backbuffer
 * using palette foreground GC. Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] id      Terrain asset identifier.
 * @param[in] x       Top-left X coordinate.
 * @param[in] y       Top-left Y coordinate.
 * @param[in] w       Width in pixels.
 * @param[in] h       Height in pixels.
 * @param[in] pal     Color palette.
 */
void gui_assets_draw_tile_detail(void *display, unsigned long dr, void *gc,
                                 gui_asset_id_t id, int x, int y,
                                 int w, int h,
                                 const gui_palette_t *pal);

/**
 * @brief Renders a visual effect (VFX) such as a barrier arc or blade slash.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws combat spell effects like glowing barrier shields or slicing blade arcs
 * centered at a target location.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct X11 arc/line primitive rasterization using theme palette colors.
 * Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] id      VFX asset identifier.
 * @param[in] cx      Center X coordinate.
 * @param[in] cy      Center Y coordinate.
 * @param[in] size    Radius or extent.
 * @param[in] pal     Color palette.
 */
void gui_assets_draw_vfx(void *display, unsigned long dr, void *gc,
                         gui_asset_id_t id, int cx, int cy,
                         int size, const gui_palette_t *pal);

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
 * @brief Audits the asset catalog, verifying ID uniqueness and generator availability.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs a sanity check on all visual art slots to make sure no character or monster
 * is missing or broken.
 *
 * ### For Systems Programmers & Engine Architects:
 * Self-test tripwire. Validates minimum catalog capacity, hero presence,
 * dimension constraints, and external binding rollback. Side-effect free.
 *
 * @return `true` if all asset contracts are verified; else `false`.
 */
bool gui_assets_audit(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating presentation asset registry invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Validates that all hero, monster, and terrain art slots are properly configured before the game starts.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight validation tripwire executing gui_assets_audit(). Returns true on success.
 *
 * @return `true` if all asset invariants pass; else `false`.
 */
bool gui_assets_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: High-Fantasy Asset Catalog
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
 * @brief Formats an ASCII diagnostic dump card representing Asset Catalog subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing registered art assets, procedural floor fallback count,
 * external artist bitmaps, and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 *
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_assets_execute_dump_card(char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the asset catalog, registering procedural generators and paths.
 *
 * ### For Game Designers & Non-Programmers:
 * Prepares the entire art catalog at game startup, registering all characters,
 * monsters, terrain, and icons.
 *
 * ### For Systems Programmers & Engine Architects:
 * Static catalog initializer. Slots baseline procedural generators and path
 * hints into `s_catalog`. Idempotent.
 */
void gui_assets_init(void);

/**
 * @brief Shuts down the asset catalog, resetting registered art slots.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely releases and resets art asset catalog registrations on shutdown.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets catalog entry count and static initialization flag.
 */
void gui_assets_shutdown(void);

#endif /* TENEBRIS_LUX_GUI_ASSETS_H */

