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

#include "ending/game.h"
#include "hero/sim_state.h"
#include "combat/combat.h"
#include "relics/items.h"
#include "world/world.h"
#include "dispatch/dispatcher.h"
#include "viewport/raster/gui.h"
#include "viewport/reel/reel.h"
#include "audio/audio.h"
#include "core/manifest.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/path_finder.h"
#include "io/master_loader.h"
#include "core/math_utils.h"
#include "core/geom_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "core/clock_pacer.h"
#include "core/formula_eval.h"
#include "log/log.h"
#include "debug/debug.h"
#include "audio/audio_bus.h"

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
 * fs, json, time, math, geom, string, log, debug), Layer 1 simulation engine (state,
 * combat, world, dispatcher, game), and Layer 2 presentation viewport (gui).
 * Enables boot-time loose-bolts invariant checks and dependency graph resolution
 * before any game loop execution begins.
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
    manifest_register(registry, &fs_utils_manifest);
    manifest_register(registry, &json_utils_manifest);
    manifest_register(registry, &path_finder_manifest);
    manifest_register(registry, &master_loader_manifest);
    manifest_register(registry, &time_utils_manifest);
    manifest_register(registry, &math_utils_manifest);
    manifest_register(registry, &geom_utils_manifest);
    manifest_register(registry, &string_utils_manifest);
    manifest_register(registry, &clock_pacer_manifest);
    manifest_register(registry, &formula_eval_manifest);
    manifest_register(registry, &log_manifest);
    manifest_register(registry, &debug_manifest);

    /* Step 3: Register Layer 1 Simulation Engine Subsystems */
    manifest_register(registry, &sim_state_manifest);
    manifest_register(registry, &combat_manifest);
    manifest_register(registry, &items_manifest);
    manifest_register(registry, &world_manifest);
    manifest_register(registry, &dispatcher_manifest);
    manifest_register(registry, &game_manifest);

    /* Step 4: Register Layer 2 Presentation & Audio Viewport */
    manifest_register(registry, &audio_bus_manifest);
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
    printf("  -h, --help            Show this help message and exit\n");
    printf("  -v, --version         Show version information and exit\n");
    printf("  -a, --audit           Run internal invariant audit and exit\n");
    printf("  -d, --debug           Launch game with interactive debugger enabled\n");
    printf("  -c, --cli             Launch game in terminal CLI mode\n");
    printf("  -g, --gui             Launch game in native desktop GUI mode\n");
    printf("  -r, --data-root <dir> Specify active game data mount directory\n");
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
    log_init(NULL);
    LOG_INFO(LOG_CAT_LOG, "Audit", 0.0f, "Starting comprehensive diagnostic self-audit");

    /* Step 1: Initialize central manifest and run pre-flight diagnostics */
    manifest_registry_t registry;
    init_engine_manifest(&registry);
    manifest_run_preflight(&registry);
    manifest_print_report(&registry);

    /* Step 2: Evaluate pre-flight outcome for fatal loose-bolts */
    if (registry.status == PREFLIGHT_STATUS_HALT_FATAL) {
        LOG_FATAL(LOG_CAT_LOG, "Audit", -1.0f, "Pre-flight failed due to loose bolts!");
        fprintf(stderr, "[FATAL] Pre-flight failed due to loose bolts!\n");
        log_shutdown();
        return 1;
    }

    /* Step 2b: Run telemetry subsystem diagnostic audit */
    diag_result_t log_diag = log_audit(false);
    if (!log_diag.passed) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "Core telemetry audit failed: %s", log_diag.details);
        fprintf(stderr, "[FAIL] Core telemetry subsystem audit failed: %s\n", log_diag.details);
        log_shutdown();
        return 1;
    }
    printf("[PASS] Test 0a: Telemetry subsystem & grade invariants verified.\n");

    /* Step 2c: Run debugger subsystem diagnostic audit */
    diag_result_t debug_diag = debug_audit(false);
    if (!debug_diag.passed) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "Core debugger audit failed: %s", debug_diag.details);
        fprintf(stderr, "[FAIL] Core debugger subsystem audit failed: %s\n", debug_diag.details);
        debug_shutdown();
        log_shutdown();
        return 1;
    }
    printf("[PASS] Test 0b: Debugger subsystem & state rewind invariants verified.\n");

    /* Step 2d: Run frame pacing orchestrator diagnostic audit */
    if (!clock_pacer_audit()) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "Clock pacer orchestrator audit failed");
        fprintf(stderr, "[FAIL] Clock pacer orchestrator audit failed.\n");
        debug_shutdown();
        log_shutdown();
        return 1;
    }
    printf("[PASS] Test 0c: Frame pacing & accumulator invariants verified.\n");

    /* Step 2e: Run battle formula orchestrator diagnostic audit */
    if (!formula_eval_audit()) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "Formula evaluator orchestrator audit failed");
        fprintf(stderr, "[FAIL] Formula evaluator orchestrator audit failed.\n");
        debug_shutdown();
        log_shutdown();
        return 1;
    }
    printf("[PASS] Test 0d: Battle formulas & affinity invariants verified.\n");

    /* Step 2f: Run audio bus orchestrator diagnostic audit */
    if (!audio_bus_preflight_audit()) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "Audio event bus audit failed");
        fprintf(stderr, "[FAIL] Audio event bus orchestrator audit failed.\n");
        debug_shutdown();
        log_shutdown();
        return 1;
    }
    printf("[PASS] Test 0e: Audio event bus & multi-channel mixer invariants verified.\n");

    /* Step 2g: Run Movie Reel presentation orchestrator diagnostic audit */
    if (!reel_audit()) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "Movie Reel orchestrator audit failed");
        fprintf(stderr, "[FAIL] Movie Reel orchestrator audit failed.\n");
        debug_shutdown();
        log_shutdown();
        return 1;
    }
    printf("[PASS] Test 0f: Movie Reel scene strip & depth kinematics invariants verified.\n");

    /* Step 3: Run simulation engine invariant audit */
    if (game_run_self_audit() != 0) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "Core game simulation audit failed");
        fprintf(stderr, "[FAIL] Core game simulation audit failed.\n");
        debug_shutdown();
        log_shutdown();
        return 1;
    }

    /* Step 4: Run GUI layout bounds and collision audit */
    if (!gui_audit_layout()) {
        LOG_CRITICAL(LOG_CAT_LOG, "Audit", -0.8f, "GUI viewport layout bounds exceeded");
        fprintf(stderr, "[FAIL] GUI viewport layout bounds exceeded!\n");
        debug_shutdown();
        log_shutdown();
        return 1;
    }
    printf("[PASS] Test 5: GUI viewport layout invariants verified.\n");

    /* Step 5: Report comprehensive pass summary */
    printf("============================================================\n");
    printf("Audit Result: ALL INVARIANT CHECKS PASSED (9/9 + Pre-Flight)\n");
    printf("============================================================\n");

    LOG_INFO(LOG_CAT_LOG, "Audit", 1.0f, "All invariant checks passed successfully (9/9 + Pre-Flight)");
    debug_shutdown();
    log_shutdown();
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
    bool force_debug = false;

    /* Step 2: Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        }
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            print_version();
            return 0;
        }
        if (strcmp(argv[i], "--audit") == 0 || strcmp(argv[i], "-a") == 0) {
            return run_full_subsystem_audit();
        }
        if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
            force_debug = true;
            force_cli = true;
        } else if (strcmp(argv[i], "--cli") == 0 || strcmp(argv[i], "-c") == 0) {
            force_cli = true;
        } else if (strcmp(argv[i], "--gui") == 0 ||
                   strcmp(argv[i], "-g") == 0) {
            force_gui = true;
        } else if (strcmp(argv[i], "--data-root") == 0 || strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc) {
                path_finder_set_data_root(argv[++i]);
            } else {
                fprintf(stderr, "Error: --data-root requires a directory path.\n");
                return 1;
            }
        } else if (strncmp(argv[i], "--data-root=", 12) == 0) {
            path_finder_set_data_root(argv[i] + 12);
        } else {
            fprintf(stderr, "Unknown option: %s\nUse --help for usage.\n",
                    argv[i]);
            return 1;
        }
    }

    /* Step 3: Initialize forensic logging & debugging */
    log_init(NULL);
    debug_init(NULL);
    LOG_INFO(LOG_CAT_LOG, "Engine", 0.0f, "Tenebris and Lux boot initialized (mode: %s%s)",
             force_cli ? "CLI" : (force_gui ? "GUI" : "Auto"),
             force_debug ? " [DEBUG-ACTIVE]" : "");

    /* Step 4: Run Pre-Flight ("Don't start machine with loose bolts") */
    manifest_registry_t registry;
    init_engine_manifest(&registry);
    preflight_status_t preflight = manifest_run_preflight(&registry);

    if (preflight == PREFLIGHT_STATUS_HALT_FATAL) {
        LOG_FATAL(LOG_CAT_LOG, "Engine", -1.0f, "Engine startup halted due to loose bolts!");
        fprintf(stderr,
                "\n[FATAL ERROR] Engine startup halted due to loose bolts!\n");
        manifest_print_report(&registry);
        debug_shutdown();
        log_shutdown();
        return 1;
    }

    /* Step 5: Initialize simulation player state */
    player_state_t state;
    game_init(&state);

    /* Record initial genesis snapshot */
    game_record_debug_snapshot(&state, "init", "genesis");

    /* Step 6: Route explicit Debug / CLI launch request */
    if (force_debug) {
        printf("============================================================\n");
        printf("   TENEBRIS AND LUX - INTERACTIVE DEBUGGER ACTIVE\n");
        printf("   Type 'debug help' or 'inspect' during gameplay\n");
        printf("============================================================\n\n");
        LOG_INFO(LOG_CAT_DEBUG, "Engine", 0.0f, "Entering interactive debug mode in CLI");
        game_loop_cli(&state);
        debug_shutdown();
        log_shutdown();
        return 0;
    }

    if (force_cli) {
        LOG_INFO(LOG_CAT_LOG, "Engine", 0.0f, "Entering terminal CLI game loop");
        game_loop_cli(&state);
        debug_shutdown();
        log_shutdown();
        return 0;
    }

    /* Step 7: Route explicit GUI launch request */
    if (force_gui) {
        if (!gui_is_available()) {
            LOG_ERROR(LOG_CAT_LOG, "Engine", -0.5f, "No graphical X11 display available for --gui");
            fprintf(stderr,
                    "Error: No graphical X11 display available for --gui.\n");
            debug_shutdown();
            log_shutdown();
            return 1;
        }
        LOG_INFO(LOG_CAT_LOG, "Engine", 0.0f, "Launching native X11 GUI");
        int rc = gui_run(&state);
        debug_shutdown();
        log_shutdown();
        return rc;
    }

    /* Step 8: Graded policy fallback: downgrade to CLI if display missing */
    if (preflight == PREFLIGHT_STATUS_DOWNGRADED || !gui_is_available()) {
        LOG_WARN(LOG_CAT_LOG, "Engine", -0.2f, "Graphical display unavailable, downgrading to CLI");
        printf("[Notice] Graphical display unavailable. "
               "Downgrading to CLI safe mode.\n\n");
        game_loop_cli(&state);
        debug_shutdown();
        log_shutdown();
        return 0;
    }

    /* Step 9: Default presentation: launch native GUI */
    LOG_INFO(LOG_CAT_LOG, "Engine", 0.0f, "Launching native X11 GUI default presentation");
    int rc = gui_run(&state);
    debug_shutdown();
    log_shutdown();
    return rc;
}

// End of main.c
