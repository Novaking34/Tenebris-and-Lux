/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_hud.h
@id            viewport.hud.display
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop Heads-Up Display (HUD) & Chronicle Presentation Engine
@role          Renders title banner, vitals dossier, combat stance indicators,
               twin relics resonance meters, threat telemetry, and narrative chronicle.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Presentation follows simulation: HUD is strictly read-only and never mutates sim state.
               4. Chronicle ring buffer renders oldest elements upward in FIFO order.
               5. Meter graphics saturate cleanly at 0% and 100% boundaries without underflow/overflow.
               6. Bounded latency budget: < 2000 microseconds per frame HUD render.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Heads-Up Display (HUD) presentation and telemetry display routines. Renders the
  heroic title banner, protagonist vitals dossier (Health, Mana, Stamina), combat
  stance gauges, twin relics resonance (Shield of Lux vs Sword of Tenebris), active
  wards/buffs, threat telemetry, and multi-line narrative chronicle using High
  Fantasy typography and data-driven layout geometry.

  ### For Game Designers & Non-Programmers:
  This module draws the visible dashboard the player watches during play: the hero's
  health bar, magic gauge, active battle stance, and the story text chronicle box
  at the bottom of the screen. It translates internal game simulation numbers into
  beautiful, readable fantasy graphics and bars.

  ### For Systems Programmers & Engine Architects:
  - 100% Read-Only Presentation Pipeline: Never writes to or mutates game simulation memory.
  - Zero heap allocation: Uses bounded stack draw structures and statically allocated buffers.
  - Bound by layout rectangles defined in engine/config/viewport/layout.json.
  - Non-anonymous types and clean DWARF symbol generation for debugging.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_HUD_H
#define TENEBRIS_LUX_GUI_HUD_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Heads-Up Display panels, include ladder, opaque simulation anchors,
and pre-conditional static validation inlines for desktop HUD rendering.
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
Sub-Assembly 2.1: Pre-Conditions & HUD Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for HUD gauge bars, labels, and text buffers.
For Designers: Boundaries ensure character vitals (HP, Light, Void), relic gauges,
and scrolling chronicle text render neatly inside their carved stone panels.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the GUI HUD subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Heads-Up Display subsystem. */
#define GUI_HUD_MODULE_ID              "viewport.hud"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Opaque simulation state hooks and HUD structural ontology.
--------------------------------------------------------------------------------
*/

/**
 * @struct player_state_s
 * @brief Forward declaration of opaque simulation state descriptor.
 *
 * For Designers:
 * An opaque representation of the active protagonist's simulation status
 * (health, energy, moral alignment) rendered into the HUD gauges.
 *
 * For Programmers:
 * Incomplete type forward-declaration decouples Layer 2 viewport headers from
 * Layer 1 game simulation headers, preserving standalone engine boundaries.
 */
typedef struct player_state_s player_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a HUD panel has valid visible dimensions.
 *
 * @param[in] pnl Pointer to panel rect to test.
 * @return true if pnl is non-NULL and width > 0 and height > 0.
 */
static inline bool gui_hud_panel_is_visible(const gui_panel_rect_t *pnl) {
    return (pnl != NULL) && (pnl->width > 0) && (pnl->height > 0);
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Title banner, hero vitals dossier, twin relics resonance gauges,
and narrative chronicle presentation cogs.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Chronicle Ring-Buffer Cogs
--------------------------------------------------------------------------------
Narrative event appending and FIFO ring-buffer rotation cogs.
--------------------------------------------------------------------------------
*/

/**
 * @brief Appends a narrative event to the chronicle log ring-buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Adds a new story or combat message to the scrolling event log box at the
 * bottom-left of the screen, pushing out the oldest entry when full.
 *
 * ### For Systems Programmers & Engine Architects:
 * Appends `msg` to `gui_log_t`. Once `GUI_MAX_LOG_LINES` is reached, shifts older
 * strings upward in FIFO order and places `msg` in the final line slot.
 *
 * @param[in,out] log Target log structure.
 * @param[in]     msg Narrative message string.
 */
void gui_hud_log_append(gui_log_t *log, const char *msg);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: HUD Banner & Dossier Presentation Cogs
--------------------------------------------------------------------------------
Header banner, protagonist vitals dossier, and scrolling chronicle rendering.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders the top title banner and realm cycle status.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws the elegant decorative header ribbon at the very top of the window,
 * showing the game's title, subtitle, and current story era.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders header background, border, title, and era strings using high-fantasy
 * typography into the X11 drawable backbuffer. Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Header panel rectangle.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog (or NULL).
 */
void gui_hud_render_header(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const gui_palette_t *pal,
                           const void *fonts);

/**
 * @brief Renders the hero vitals dossier, relic meters, attributes, and threat panel.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws the character status card on the right side of the screen, showing health,
 * mana, Shield of Lux / Sword of Tenebris resonance bars, and active monster threats.
 *
 * ### For Systems Programmers & Engine Architects:
 * Inspects `player_state_t` read-only. Chooses expanded or compact layout depending
 * on panel height. Draws gauges, labels, and attributes using theme palette.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Status panel rectangle.
 * @param[in] state   Active simulation player state.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog (or NULL).
 */
void gui_hud_render_status(void *display, unsigned long dr, void *gc,
                           const gui_panel_rect_t *pnl,
                           const player_state_t *state,
                           const gui_palette_t *pal,
                           const void *fonts);

/**
 * @brief Renders the scrolling narrative chronicle log.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the recent lines of story and battle dialogue inside the bottom-left
 * scroll box with the latest message highlighted in radiant gold.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders up to `max_visible` lines of narrative text from `gui_log_t`, highlighting
 * the most recent entry with `pal->gold_bright`. Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Chronicle panel rectangle.
 * @param[in] log     Narrative log ring-buffer.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog (or NULL).
 */
void gui_hud_render_chronicle(void *display, unsigned long dr, void *gc,
                              const gui_panel_rect_t *pnl,
                              const gui_log_t *log,
                              const gui_palette_t *pal,
                              const void *fonts);

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
 * @brief Audits chronicle ring-buffer FIFO capacity and bounds invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the event log to confirm it never drops or scrambles recent messages
 * when new ones arrive.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates FIFO capacity fill, string boundary safety, and shift-on-overflow invariants.
 *
 * @return `true` if all HUD invariants hold; else `false`.
 */
bool gui_hud_audit(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating Heads-Up Display and chronicle log invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Validates that header banners, character status dossier gauges, and event log buffers operate reliably.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight validation tripwire executing gui_hud_audit(). Returns true on success.
 *
 * @return `true` if all HUD invariants pass; else `false`.
 */
bool gui_hud_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: HUD Layout & Renderers
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
 * @brief Formats an ASCII diagnostic dump card representing Heads-Up Display subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing HUD rendering metrics, log capacity,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 *
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_hud_execute_dump_card(char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_GUI_HUD_H */
