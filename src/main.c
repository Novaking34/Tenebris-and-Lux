// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          main.c
// @id            app.entry.main
// @layer         Layer 3: Application Entry & Presentation Router
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Executable entry point, command-line argument router, pre-flight router,
//   and presentation frontend selector. Coordinates subsystem boot,
//   executing diagnostics, and selecting GUI or CLI presentation.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.engine.core, gui.viewport.x11, core.util.*
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap in router).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Main thread execution).
//   - Latency Budget:        < 5 milliseconds total pre-flight initialization.
//
// AXIOMATIC INVARIANTS:
//   1. Headless fallback guarantees graceful degradation when display absent.
//   2. Zero-warning clean compilation under -std=c11 -Wall -Wextra -Wpedantic.
//   3. Diagnostic self-audit returns 0 only when all invariants pass.
//   4. Loose bolts principle: Pre-flight halt terminates prior to simulation.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 3 (Application Entry) | Machine: Presentation Router
// =============================================================================

#include "sim/game.h"
#include "gui/gui.h"
#include "util/manifest.h"
#include "util/math_utils.h"
#include "util/string_utils.h"
#include "util/geom_utils.h"
#include "util/time_utils.h"

#include <stdio.h>
#include <string.h>

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: CENTRAL MANIFEST REGISTRATION ]
// Mission: Register all engine subsystem manifests for boot-time pre-flight.
// -----------------------------------------------------------------------------

/**
 * @brief Registers all core engine subsystem manifests into central registry.
 *
 * ### Machine Role & Mission
 * Populates the central manifest registry with all Layer 0 utilities (manifest,
 * time, math, geom, string), Layer 1 simulation engine (game), and Layer 2
 * presentation viewport (gui). Enables boot-time loose-bolts invariant checks
 * and dependency graph resolution before any game loop execution begins.
 *
 * ### Ontological Classification
 * - Stratum: Layer 3 (Application Entry & Presentation Router)
 * - Machine: app.entry.main (Executable Entry & Presentation Router)
 * - Assembly: Pre-Flight Manifest Registry Initialization
 * - Cog: Module Manifest Registration Primitive
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Stack-allocated fixed manifest tables).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Called at startup on main thread).
 * - Latency Budget: < 50 microseconds.
 *
 * @param[out] registry Target manifest registry to initialize and populate.
 */
static void init_engine_manifest(manifest_registry_t *registry)
{
    /* Step 1: Zero-initialize registry table */
    manifest_registry_init(registry);

    /* Step 2: Register Layer 0 Core Utilities */
    manifest_register(registry, &manifest_utils_manifest);
    manifest_register(registry, &time_utils_manifest);
    manifest_register(registry, &math_utils_manifest);
    manifest_register(registry, &geom_utils_manifest);
    manifest_register(registry, &string_utils_manifest);

    /* Step 3: Register Layer 1 Simulation Engine */
    manifest_register(registry, &game_manifest);

    /* Step 4: Register Layer 2 Presentation Viewport */
    manifest_register(registry, &gui_manifest);
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 3 (Application Entry) | Machine: Presentation Router
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: CLI INFORMATION & HELP FORMATTERS ]
// Mission: Print command-line options and version provenance.
// -----------------------------------------------------------------------------

/**
 * @brief Prints command-line usage information and supported flags to stdout.
 *
 * ### Machine Role & Mission
 * Documents available CLI flags (`--help`, `--version`, `--audit`, `--cli`,
 * `--gui`) for human operators and script automation harnesses.
 *
 * ### Ontological Classification
 * - Stratum: Layer 3 (Application Entry & Presentation Router)
 * - Machine: app.entry.main (Executable Entry & Presentation Router)
 * - Assembly: CLI Interface & Diagnostics
 * - Cog: Command-Line Usage Formatter & Printer
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic allocations).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Pure console output).
 * - Latency Budget: < 100 microseconds.
 */
static void print_usage(void)
{
    /* Step 1: Emit usage banner and option explanations */
    printf("Usage: tenebris-lux [options]\n\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message and exit\n");
    printf("  -v, --version  Show version information and exit\n");
    printf("  -a, --audit    Run internal invariant audit and exit\n");
    printf("  -c, --cli      Launch game in terminal CLI mode\n");
    printf("  -g, --gui      Launch game in native desktop GUI mode\n");
}

/**
 * @brief Prints the application title, version tag, and narrative description.
 *
 * ### Machine Role & Mission
 * Displays formal provenance and engine version metadata to standard output.
 *
 * ### Ontological Classification
 * - Stratum: Layer 3 (Application Entry & Presentation Router)
 * - Machine: app.entry.main (Executable Entry & Presentation Router)
 * - Assembly: CLI Interface & Diagnostics
 * - Cog: Application Identity & Version Formatter
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero heap allocations).
 * - Concurrency: CONCURRENCY_MAIN_THREAD.
 * - Latency Budget: < 50 microseconds.
 */
static void print_version(void)
{
    /* Step 1: Output application name, version string, and description */
    printf("%s v%s\n", APP_NAME, APP_VERSION);
    printf("%s\n", APP_DESCRIPTION);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 3 (Application Entry) | Machine: Presentation Router
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: COMPREHENSIVE SUBSYSTEM SELF-AUDIT SUITE ]
// Mission: Execute pre-flight manifest checks, sim invariants, and GUI bounds.
// -----------------------------------------------------------------------------

/**
 * @brief Executes the comprehensive engine self-audit suite and manifests.
 *
 * ### Machine Role & Mission
 * Serves as the `--audit` diagnostic tripwire. Executes:
 * 1. Pre-flight verification across all registered modules with DAG checks.
 * 2. Layer 1 simulation invariant audit (`game_run_self_audit()`).
 * 3. Layer 2 GUI viewport layout and collision audit (`gui_audit_layout()`).
 * Returns non-zero if any invariant check or pre-flight test fails.
 *
 * ### Ontological Classification
 * - Stratum: Layer 3 (Application Entry & Presentation Router)
 * - Machine: app.entry.main (Executable Entry & Presentation Router)
 * - Assembly: Diagnostic Self-Audit & Pre-Flight Verification
 * - Cog: Full Engine Diagnostics & Invariant Self-Audit Suite
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Stack allocation only).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Synchronous diagnostic sweep).
 * - Latency Budget: < 5000 microseconds execution time.
 *
 * @return 0 if all invariant checks pass; non-zero if any check fails.
 */
static int run_full_subsystem_audit(void)
{
    /* Step 1: Initialize central manifest and run pre-flight diagnostics */
    manifest_registry_t registry;
    init_engine_manifest(&registry);
    manifest_run_preflight(&registry);
    manifest_print_report(&registry);

    /* Step 2: Evaluate pre-flight outcome for fatal loose-bolts */
    if (registry.status == PREFLIGHT_STATUS_HALT_FATAL) {
        fprintf(stderr, "[FATAL] Pre-flight failed due to loose bolts!\n");
        return 1;
    }

    printf("\n============================================================\n");
    printf("      Tenebris and Lux - Comprehensive Subsystem Audit      \n");
    printf("============================================================\n");

    /* Step 3: Run simulation engine invariant audit */
    if (game_run_self_audit() != 0) {
        fprintf(stderr, "[FAIL] Core game simulation audit failed.\n");
        return 1;
    }

    /* Step 4: Run GUI layout bounds and collision audit */
    if (!gui_audit_layout()) {
        fprintf(stderr, "[FAIL] GUI viewport layout bounds exceeded!\n");
        return 1;
    }
    printf("[PASS] Test 5: GUI viewport layout invariants verified.\n");

    /* Step 5: Report comprehensive pass summary */
    printf("============================================================\n");
    printf("Audit Result: ALL INVARIANT CHECKS PASSED (5/5 + Pre-Flight)\n");
    printf("============================================================\n");
    return 0;
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// Stratum: Layer 3 (Application Entry) | Machine: Presentation Router
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: PROCESS ENTRY POINT & FRONTEND ROUTER ]
// Mission: Parse CLI options, execute pre-flight, and launch GUI or CLI.
// -----------------------------------------------------------------------------

/**
 * @brief Application entry point and presentation frontend router.
 *
 * ### Machine Role & Mission
 * Serves as the operating system process entry point. Parses command-line
 * arguments, executes pre-flight loose-bolts machine checks, initializes the
 * player simulation state, and dynamically routes to either native X11 GUI
 * or terminal CLI mode based on user flags and hardware display availability.
 *
 * ### Ontological Classification
 * - Stratum: Layer 3 (Application Entry & Presentation Router)
 * - Machine: app.entry.main (Executable Entry & Presentation Router)
 * - Assembly: Application Lifecycle & Frontend Router
 * - Cog: Process Entry Point & Presentation Mode Dispatcher
 *
 * ### Execution & Resource Contract
 * - Memory Model: ALLOC_ZERO_HEAP (Zero dynamic heap allocations in router).
 * - Concurrency: CONCURRENCY_MAIN_THREAD (Main thread process lifecycle).
 * - Latency Budget: Lifespan of game process.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Array of command-line argument strings.
 * @return Process exit code (0 on success, non-zero on failure).
 */
int main(int argc, char *argv[])
{
    /* Step 1: Initialize presentation mode flags */
    bool force_cli = false;
    bool force_gui = false;

    /* Step 2: Parse command-line arguments */
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_usage();
            return 0;
        }
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            print_version();
            return 0;
        }
        if (strcmp(argv[1], "--audit") == 0 || strcmp(argv[1], "-a") == 0) {
            return run_full_subsystem_audit();
        }
        if (strcmp(argv[1], "--cli") == 0 || strcmp(argv[1], "-c") == 0) {
            force_cli = true;
        } else if (strcmp(argv[1], "--gui") == 0 ||
                   strcmp(argv[1], "-g") == 0) {
            force_gui = true;
        } else {
            fprintf(stderr, "Unknown option: %s\nUse --help for usage.\n",
                    argv[1]);
            return 1;
        }
    }

    /* Step 3: Run Pre-Flight ("Don't start machine with loose bolts") */
    manifest_registry_t registry;
    init_engine_manifest(&registry);
    preflight_status_t preflight = manifest_run_preflight(&registry);

    if (preflight == PREFLIGHT_STATUS_HALT_FATAL) {
        fprintf(stderr,
                "\n[FATAL ERROR] Engine startup halted due to loose bolts!\n");
        manifest_print_report(&registry);
        return 1;
    }

    /* Step 4: Initialize simulation player state */
    player_state_t state;
    game_init(&state);

    /* Step 5: Route explicit CLI launch request */
    if (force_cli) {
        game_loop_cli(&state);
        return 0;
    }

    /* Step 6: Route explicit GUI launch request */
    if (force_gui) {
        if (!gui_is_available()) {
            fprintf(stderr,
                    "Error: No graphical X11 display available for --gui.\n");
            return 1;
        }
        return gui_run(&state);
    }

    /* Step 7: Graded policy fallback: downgrade to CLI if display missing */
    if (preflight == PREFLIGHT_STATUS_DOWNGRADED || !gui_is_available()) {
        printf("[Notice] Graphical display unavailable. "
               "Downgrading to CLI safe mode.\n\n");
        game_loop_cli(&state);
        return 0;
    }

    /* Step 8: Default presentation: launch native GUI */
    return gui_run(&state);
}

// End of main.c
