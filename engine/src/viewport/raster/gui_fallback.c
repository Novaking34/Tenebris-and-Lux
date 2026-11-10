/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          gui_fallback.c
@id            viewport.raster.fallback
@layer         Layer 2: Presentation & Viewport
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Desktop X11 Double-Buffered Viewport & Presentation (viewport/raster)
@role          Implementation of Quarantined Monolithic Emergency Default Code Page & Headless GUI Fallback
@invariants    1. Zero dynamic heap allocations in Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined execution path: Never invoked in normal production with active DISPLAY.
               4. Provenance tracking: loaded_from_data is unconditionally false.
               5. Canonical fallback baseline: 860x620 desktop window, 640x360 logical reference.
               6. Headless resilience: Safe execution in environments lacking an X11 display server.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Isolated, monolithic emergency default code page and tripwire fallback for
  the desktop X11 presentation orchestrator and viewport rasterizer. Quarantined
  strictly outside the main production rendering pipeline. Provides compile-time
  static presentation dimensions, fallback layout geometries, default obsidian
  color palette synthesis, and headless verification when display servers or
  viewport configuration manifests are unavailable.

  ### For Game Designers & Non-Programmers:
  If the game is launched without an active monitor or screen (such as during
  automated cloud testing), or if the layout configuration files are missing,
  this code page provides an emergency visual blueprint. It ensures all button
  positions, text logs, and color themes default to safe, readable factory values.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Safe headless fallback: Generates valid layout rectangles and palettes on stack.
  - Pure ISO C11 reentrancy with zero global side-effects.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Viewport Rasterizer
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: ISO C11 Standard Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/* Tier 2: External Library Dependencies (None) */

/* Tier 3: Internal Engine Foundation Headers */
#include "core/manifest.h"
#include "viewport/raster/gui_fallback.h"

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Viewport Rasterizer
================================================================================
Mission: Quarantined emergency fallback record hydration, palette synthesis,
layout calculations, and headless environment queries.
All execution loops and lifecycle hooks are quarantined in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Quarantined Fallback Configuration & Synthesis Cogs
--------------------------------------------------------------------------------
Static configuration generation, default palette synthesis, and layout calculation.
--------------------------------------------------------------------------------
*/

/**
 * @brief Populates a caller-supplied struct with immutable GUI fallback configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Fills in default presentation values like standard 860x620 resolution, 60 FPS,
 * and 5 screen tabs when settings cannot be loaded from disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-based copy of immutable static metadata into destination struct. Sets
 * loaded_from_data strictly to false.
 *
 * @param[out] out_config Destination configuration structure.
 */
void gui_fallback_get_config(gui_fallback_config_t *out_config)
{
    if (out_config == NULL) {
        return;
    }
    memset(out_config, 0, sizeof(*out_config));
    strncpy(out_config->id, GUI_FALLBACK_PROFILE_NAME, sizeof(out_config->id) - 1);
    strncpy(out_config->display_name, "Canonical Emergency GUI Fallback", sizeof(out_config->display_name) - 1);
    out_config->window_width = GUI_FALLBACK_WINDOW_WIDTH;
    out_config->window_height = GUI_FALLBACK_WINDOW_HEIGHT;
    out_config->logical_width = GUI_FALLBACK_LOGICAL_WIDTH;
    out_config->logical_height = GUI_FALLBACK_LOGICAL_HEIGHT;
    out_config->target_fps = GUI_FALLBACK_TARGET_FPS;
    out_config->headless_safe = true;
    out_config->loaded_from_data = false;
}

/**
 * @brief Synthesizes default obsidian and carved granite palette on stack.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets up the standard dark fantasy color scheme (gold for Lux, purple for Tenebris,
 * dark slate backgrounds) if theme files are missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Directly initializes 24-bit RGB values for backgrounds, panels, borders, fonts,
 * and HUD gauges on a stack-allocated palette structure.
 *
 * @param[out] out_palette Destination palette structure.
 */
void gui_fallback_init_palette(gui_palette_t *out_palette)
{
    if (out_palette == NULL) {
        return;
    }
    memset(out_palette, 0, sizeof(*out_palette));
    out_palette->bg = 0x0A0B0E;
    out_palette->panel = 0x14171E;
    out_palette->border = 0x2A2E3D;
    out_palette->text_main = 0xE6E8EE;
    out_palette->text_muted = 0x8A90A0;
    out_palette->shield_gold = 0xD4A017;
    out_palette->gold_bright = 0xF1C40F;
    out_palette->sword_violet = 0x6C3483;
    out_palette->violet_bright = 0x9B59B6;
    out_palette->virtue_green = 0x27AE60;
    out_palette->malice_red = 0xC0392B;
    out_palette->btn_bg = 0x1E222D;
    out_palette->btn_border = 0x3D4355;
    out_palette->btn_hover = 0x2C3242;
    out_palette->tile_grid = 0x222634;
    out_palette->tile_cobble = 0x3A3F50;
    out_palette->tile_grass = 0x2E4A34;
    out_palette->tile_water = 0x1B3B5A;
    out_palette->tile_wall = 0x4A4E5D;
    out_palette->tactical_move = 0x2980B9;
    out_palette->tactical_atk = 0xA93226;
    out_palette->shadow = 0x050508;
}

/**
 * @brief Synthesizes default non-overlapping panel layout rectangles on stack.
 *
 * ### For Game Designers & Non-Programmers:
 * Calculates default positions and sizes for the header, battlefield, dossier,
 * chronicle, and command dock panels to fit the window cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets up 5 standard panel bounding boxes (`header`, `viewport_2d`, `status`,
 * `chronicle`, `command_dock`) guaranteed to satisfy AABB non-overlap audits.
 *
 * @param[out] out_layout Destination layout structure.
 * @param[in]  width      Target window width in pixels.
 * @param[in]  height     Target window height in pixels.
 */
void gui_fallback_init_layout(gui_layout_t *out_layout, int width, int height)
{
    if (out_layout == NULL) {
        return;
    }
    if (width <= 0) width = GUI_FALLBACK_WINDOW_WIDTH;
    if (height <= 0) height = GUI_FALLBACK_WINDOW_HEIGHT;

    out_layout->window_width = width;
    out_layout->window_height = height;

    /* Header Panel: Top banner */
    out_layout->header.x = 10;
    out_layout->header.y = 10;
    out_layout->header.width = width - 20;
    out_layout->header.height = 40;
    out_layout->header.title = "Header";

    /* Tactical Viewport 2D/2.5D: Left center */
    out_layout->viewport_2d.x = 10;
    out_layout->viewport_2d.y = 60;
    out_layout->viewport_2d.width = 540;
    out_layout->viewport_2d.height = 360;
    out_layout->viewport_2d.title = "Battlefield";

    /* Status Panel: Right column */
    out_layout->status.x = 560;
    out_layout->status.y = 60;
    out_layout->status.width = width - 570;
    out_layout->status.height = 360;
    out_layout->status.title = "Dossier";

    /* Chronicle Panel: Bottom left */
    out_layout->chronicle.x = 10;
    out_layout->chronicle.y = 430;
    out_layout->chronicle.width = 540;
    out_layout->chronicle.height = height - 440;
    out_layout->chronicle.title = "Chronicle";

    /* Command Dock: Bottom right */
    out_layout->command_dock.x = 560;
    out_layout->command_dock.y = 430;
    out_layout->command_dock.width = width - 570;
    out_layout->command_dock.height = height - 440;
    out_layout->command_dock.title = "Commands";
}

/**
 * @brief Checks whether the current runtime environment is operating without an X11 server.
 *
 * ### For Game Designers & Non-Programmers:
 * Detects if the game is running on a machine without a display monitor, allowing
 * it to switch gracefully to text mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks the `$DISPLAY` environment variable. Returns true if unset or empty.
 *
 * @return true if $DISPLAY is unset or empty; false otherwise.
 */
bool gui_fallback_is_headless(void)
{
    const char *display = getenv("DISPLAY");
    return (display == NULL || display[0] == '\0');
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Presentation & Viewport) | Machine: Desktop Viewport Rasterizer
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Audit & Self-Test Verification Cogs
--------------------------------------------------------------------------------
Diagnostic self-test tripwires and validation routines for GUI presentation fallback.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating GUI fallback invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that the emergency fallback layout and colors initialize cleanly without errors.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pre-flight diagnostic verifying provenance, window dimensions, target FPS,
 * layout initialization, and non-zero palette values.
 *
 * @return Diagnostic check outcome reporting PASS or FATAL.
 */
diag_result_t gui_fallback_preflight(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = "viewport.raster.fallback";
    res.check_name = "gui_fallback_invariants";
    res.details = "GUI fallback layout and palette parameters verified.";

    gui_fallback_config_t cfg;
    gui_fallback_get_config(&cfg);

    if (cfg.window_width <= 0 || cfg.window_height <= 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Invalid fallback window dimensions.";
        return res;
    }
    if (cfg.logical_width <= 0 || cfg.logical_height <= 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Invalid fallback logical dimensions.";
        return res;
    }
    if (cfg.target_fps <= 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Invalid fallback target FPS.";
        return res;
    }
    if (cfg.loaded_from_data) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fallback loaded_from_data violation.";
        return res;
    }

    gui_layout_t layout;
    gui_fallback_init_layout(&layout, cfg.window_width, cfg.window_height);
    if (layout.window_width != cfg.window_width || layout.window_height != cfg.window_height) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fallback layout dimension mismatch.";
        return res;
    }

    gui_palette_t pal;
    gui_fallback_init_palette(&pal);
    if (pal.bg == 0 || pal.panel == 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fallback palette colors uninitialized.";
        return res;
    }

    return res;
}

/**
 * @brief Standalone boolean audit wrapper for regression test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that the emergency fallback graphics system passes all validation tests.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes `gui_fallback_preflight` and verifies PASS severity status.
 *
 * @return true if all GUI fallback tripwires pass cleanly; false otherwise.
 */
bool gui_fallback_audit(void)
{
    diag_result_t res = gui_fallback_preflight();
    return diag_result_is_pass(&res);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Presentation & Viewport) | Subsystem: Quarantined Viewport Fallback
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
 * @brief Formats an ASCII diagnostic dump card representing Viewport Fallback subsystem state.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates a clean text card summarizing emergency viewport fallbacks, target window metrics,
 * and pre-flight audit status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes a formatted 60-column diagnostic summary into `buffer` within `capacity` bytes.
 */
int gui_fallback_execute_dump_card(char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0) {
        return -1;
    }

    gui_fallback_config_t cfg;
    gui_fallback_get_config(&cfg);
    bool audit_ok = gui_fallback_audit();

    return snprintf(buffer, capacity,
        "============================================================\n"
        " VIEWPORT FALLBACK SUBSYSTEM DIAGNOSTIC CARD               \n"
        "============================================================\n"
        " Subsystem       : Quarantined Viewport Emergency Fallback  \n"
        " Profile Name    : %-41s\n"
        " Fallback Window : %dx%d (Logical %dx%d)                   \n"
        " Target FPS      : %-41d\n"
        " Loaded From Data: %-41s\n"
        " Pre-Flight Audit: %-41s\n"
        "============================================================\n",
        cfg.display_name,
        cfg.window_width, cfg.window_height,
        cfg.logical_width, cfg.logical_height,
        cfg.target_fps,
        cfg.loaded_from_data ? "YES (BREACH)" : "NO (INVARIANT PASS)",
        audit_ok ? "PASSED" : "FAILED");
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/
