/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_screens.h
@id            viewport.reel.screens
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        5-Screen Presentation View Modes & Reel Navigators
@role          Dedicated full-canvas alternate presentation screens for High Fantasy TRPG:
               Protagonist Hero Dossier, 32-Slot Item & Relic Bag, Provincial Realm
               Topology Map, and Historical Deeds Chronicle Codex, coordinated via an
               ergonomic top tab navigation bar aligned with screens.json.
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All alternate screens strictly honor viewport bounding rectangle.
               4. Screen switching is idempotent with zero simulation state mutation.
               5. Exactly 5 canonical screens registered (F1-F5 hotkeys).
               6. Non-anonymous structs and enums enforced across all declarations.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Maintains the 5 presentation screens and top tab navigation bar:
  1. Tactical Grid Viewport (GUI_SCREEN_TACTICAL)
  2. Hero Dossier & Attributes (GUI_SCREEN_STATUS)
  3. 32-Slot Inventory & Relics (GUI_SCREEN_INVENTORY)
  4. Provincial Realm Map (GUI_SCREEN_WORLD_MAP)
  5. Chronicle & Deeds Codex (GUI_SCREEN_CHRONICLE)

  ### For Game Designers & Non-Programmers:
  Provides the full-page menu screens player switch between using hotkeys F1-F5
  or clicking the tabs at the top of the window: looking at hero stats, browsing
  the backpack and relics, viewing the province map of the realm, and reading
  past story chronicle entries.

  ### For Systems Programmers & Engine Architects:
  - Idempotent screen state machine: switching screens alters viewport display
    mode without mutating game turn state or triggering side effects.
  - Zero dynamic heap allocation: all screen descriptors reside in static memory.
  - Tab hit-testing and hotkey dispatching execute in < 1 microsecond.
  - Clean separation: renders via generic renderer callbacks and read-only state.
================================================================================
*/

#ifndef TENEBRIS_LUX_GUI_SCREENS_H
#define TENEBRIS_LUX_GUI_SCREENS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Screen modes, include ladder, navigation tab state, and pre-conditional
static validation inlines for full-canvas screen transitions.
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
#include "viewport/reel/reel.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Screen Mode Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for full-screen presentation modes and navigation tabs.
For Designers: Boundaries ensure switching between Tactical Grid, Hero Dossier,
Inventory, Continental Map, and Story Chronicle executes instantaneously without lag.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the GUI Screens subsystem.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Screen Navigation & Frames subsystem. */
#define GUI_SCREENS_MODULE_ID          "viewport.screens"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Opaque simulation state hooks and screen mode ontology.
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
 * @brief Predicate verifying if a screen presentation mode is recognized.
 *
 * @param[in] mode Screen mode enum value to check.
 * @return true if mode >= GUI_SCREEN_TACTICAL and mode <= GUI_SCREEN_CHRONICLE.
 */
static inline bool gui_screen_mode_is_valid(gui_screen_mode_t mode) {
    return mode >= GUI_SCREEN_TACTICAL && mode <= GUI_SCREEN_CHRONICLE;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop X11 Viewport
================================================================================
Mission: Screen navigation tab bar, mouse click dispatching, and full-canvas
alternate screen presentation renderers.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Screen Navigation & Tab Hit-Testing Cogs
--------------------------------------------------------------------------------
Top tab bar rendering, boundary calculations, and mouse pick dispatching.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders the top screen navigation tab bar inside the header banner.
 *
 * ### For Game Designers & Non-Programmers:
 * Draws the 5 clickable menu tabs across the top of the screen (Tactical [F1],
 * Status [F2], Inventory [F3], World Map [F4], Chronicle [F5]), highlighting
 * the active screen in glowing gold.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders tab boxes, border outlines, active highlight bar, and centered labels
 * into the X11 drawable backbuffer using theme palette colors. Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] pnl     Header panel rectangle.
 * @param[in] pal     Color palette.
 * @param[in] active  Active screen mode.
 * @param[in] fonts   High Fantasy font catalog.
 */
void gui_screens_render_tab_bar(void *display, unsigned long dr, void *gc,
                                const gui_panel_rect_t *pnl,
                                const gui_palette_t *pal,
                                gui_screen_mode_t active,
                                const void *fonts);

/**
 * @brief Checks whether a click in the header banner hit any navigation tab.
 *
 * ### For Game Designers & Non-Programmers:
 * Detects if the player clicked one of the 5 screen tabs with the mouse and
 * identifies which screen they want to view.
 *
 * ### For Systems Programmers & Engine Architects:
 * Performs linear AABB hit tests across all 5 tab bounds. If hit, writes enum
 * to `out_screen` and returns true; otherwise returns false in O(1) time.
 *
 * @param[in]  pnl        Header panel rectangle.
 * @param[in]  cx         Click X coordinate.
 * @param[in]  cy         Click Y coordinate.
 * @param[out] out_screen Selected screen mode if hit.
 * @return `true` if a tab was clicked; else `false`.
 */
bool gui_screens_handle_tab_click(const gui_panel_rect_t *pnl,
                                  int cx, int cy,
                                  gui_screen_mode_t *out_screen);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Full-Canvas Alternate Screen Rendering Cogs
--------------------------------------------------------------------------------
Full-page presentation renderers for Status Dossier, Inventory, World Map, and Chronicle.
--------------------------------------------------------------------------------
*/

/**
 * @brief Renders the grand full-canvas Protagonist Hero Dossier & Attributes Matrix.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the complete character sheet: large heraldic crest sprite, heroic lineage,
 * HP/MP/TP bars, attributes (ATK/DEF/MAT/MDF), relic power, and campaign achievements.
 *
 * ### For Systems Programmers & Engine Architects:
 * Three-column partition of `area`. Renders hero crest via `gui_assets_draw_sprite`,
 * vitals via `gui_widget_draw_meter`, and formatted attribute strings.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] area    Full canvas drawing area.
 * @param[in] state   Active simulation player state.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog.
 */
void gui_screen_render_status_dossier(void *display, unsigned long dr, void *gc,
                                      const gui_panel_rect_t *area,
                                      const player_state_t *state,
                                      const gui_palette_t *pal,
                                      const void *fonts);

/**
 * @brief Renders the 32-Slot Inventory, Consumables & Key Relic Inspection screen.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the player's 32-slot inventory bag arranged in a 4x8 grid on the left,
 * with a detailed inspect window and lore card for selected relics on the right.
 *
 * ### For Systems Programmers & Engine Architects:
 * Grid rasterization drawing 32 item cells with indices and names, paired with
 * an artifact detail card displaying category, origin, and purse gold. Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] area    Full canvas drawing area.
 * @param[in] state   Active simulation player state.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog.
 */
void gui_screen_render_inventory(void *display, unsigned long dr, void *gc,
                                 const gui_panel_rect_t *area,
                                 const player_state_t *state,
                                 const gui_palette_t *pal,
                                 const void *fonts);

/**
 * @brief Renders the Provincial Topographical Realm Map of Sacyra and Libira.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the world map showing major kingdom locations connected by cobblestone
 * trade roads, with glowing beacon rings marking the player's current location.
 *
 * ### For Systems Programmers & Engine Architects:
 * Draws connected provincial graph nodes and road lines across Sacyra, Libira,
 * and the Capital. Renders beacon rings for `state->location`. Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] area    Full canvas drawing area.
 * @param[in] state   Active simulation player state.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog.
 */
void gui_screen_render_world_map(void *display, unsigned long dr, void *gc,
                                 const gui_panel_rect_t *area,
                                 const player_state_t *state,
                                 const gui_palette_t *pal,
                                 const void *fonts);

/**
 * @brief Renders the full-canvas Historical Deeds Chronicle & Accord Lore Codex.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays the entire campaign narrative log across a wide page, allowing the
 * player to review past storyline events, dialogues, and battle triumphs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders up to `max_visible` chronicle lines from `gui_log_t` with custom line
 * spacing and radiant highlight on the newest entry. Zero heap.
 *
 * @param[in] display Opaque Display pointer.
 * @param[in] dr      Target Drawable backbuffer.
 * @param[in] gc      Opaque GC graphics context.
 * @param[in] area    Full canvas drawing area.
 * @param[in] log     Narrative log ring-buffer.
 * @param[in] pal     Color palette.
 * @param[in] fonts   High Fantasy font catalog.
 */
void gui_screen_render_chronicle_codex(void *display, unsigned long dr,
                                       void *gc,
                                       const gui_panel_rect_t *area,
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
 * @brief Audits alternate screen tab geometry and hit-test boundaries.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the screen tab bar to verify that clicks outside the tabs are ignored,
 * and clicks on Tab 0 reliably switch to the Tactical screen.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies tab click hit-testing bounds and mode resolution invariants.
 *
 * @return `true` if all screen invariants pass; else `false`.
 */
bool gui_screens_audit(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Preflight verification for alternate screens and navigation tab bar.
 *
 * ### For Game Designers & Non-Programmers:
 * Validates that all 5 screen navigation tabs (F1 through F5) respond accurately.
 *
 * ### For Systems Programmers & Engine Architects:
 * Preflight test cycling through all 5 tabs verifying center point hitboxes.
 *
 * @return `true` if all screen hitboxes and reel mode mappings pass.
 */
bool gui_screens_preflight(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Alternate Screen Renderers
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
 * @brief Formats an ASCII diagnostic dump card representing Alternate Screens subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing alternate screens status, navigation tab count,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 *
 * @param[out] buffer   Output buffer for the diagnostic card.
 * @param[in]  capacity Maximum capacity of the destination buffer.
 * @return Number of characters written, or negative on buffer overflow/error.
 */
int gui_screens_execute_dump_card(char *buffer, size_t capacity);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_GUI_SCREENS_H */
