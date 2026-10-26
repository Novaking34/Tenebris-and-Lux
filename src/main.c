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
//   Executable entry point, command-line argument router, pre-flight orchestrator,
//   and presentation frontend selector. Serves as the gateway coordinating
//   subsystem boot, executing diagnostics, and selecting GUI or CLI presentation.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.engine.core, gui.viewport.x11, core.util.*
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap in startup routing).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Process main thread execution).
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
// ( System Headers & Application Constants )
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

static void init_engine_manifest(manifest_registry_t *registry)
{
    manifest_registry_init(registry);
    manifest_register(registry, &manifest_utils_manifest);
    manifest_register(registry, &time_utils_manifest);
    manifest_register(registry, &math_utils_manifest);
    manifest_register(registry, &geom_utils_manifest);
    manifest_register(registry, &string_utils_manifest);
    manifest_register(registry, &game_manifest);
    manifest_register(registry, &gui_manifest);
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Presentation Routing & CLI Execution Engine )
// =============================================================================

static void print_usage(void)
{
    printf("Usage: tenebris-lux [options]\n\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message and exit\n");
    printf("  -v, --version  Show version information and exit\n");
    printf("  -a, --audit    Run internal invariant audit and exit\n");
    printf("  -c, --cli      Launch game in terminal CLI mode\n");
    printf("  -g, --gui      Launch game in native desktop GUI mode\n");
}

static void print_version(void)
{
    printf("%s v%s\n", APP_NAME, APP_VERSION);
    printf("%s\n", APP_DESCRIPTION);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Subsystem Combined Audit Suite )
// =============================================================================

static int run_full_subsystem_audit(void)
{
    manifest_registry_t registry;
    init_engine_manifest(&registry);
    manifest_run_preflight(&registry);
    manifest_print_report(&registry);

    if (registry.status == PREFLIGHT_STATUS_HALT_FATAL) {
        fprintf(stderr, "[FATAL] Pre-flight failed due to loose bolts!\n");
        return 1;
    }

    printf("\n============================================================\n");
    printf("      Tenebris and Lux - Comprehensive Subsystem Audit      \n");
    printf("============================================================\n");

    /* 1. Run simulation invariants */
    if (game_run_self_audit() != 0) {
        fprintf(stderr, "[FAIL] Core game simulation audit failed.\n");
        return 1;
    }

    /* 2. Run GUI layout bounds audit */
    if (!gui_audit_layout()) {
        fprintf(stderr, "[FAIL] GUI viewport layout bounds exceeded!\n");
        return 1;
    }
    printf("[PASS] Test 5: GUI viewport layout invariants verified.\n");

    printf("============================================================\n");
    printf("Audit Result: ALL INVARIANT CHECKS PASSED (5/5 + Pre-Flight)\n");
    printf("============================================================\n");
    return 0;
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// ( CLI Entry Point & Process Teardown )
// =============================================================================

int main(int argc, char *argv[])
{
    bool force_cli = false;
    bool force_gui = false;

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
        } else if (strcmp(argv[1], "--gui") == 0 || strcmp(argv[1], "-g") == 0) {
            force_gui = true;
        } else {
            fprintf(stderr, "Unknown option: %s\nUse --help for usage.\n", argv[1]);
            return 1;
        }
    }

    /* Pre-Flight Inspection: "Don't start a machine you know has loose bolts" */
    manifest_registry_t registry;
    init_engine_manifest(&registry);
    preflight_status_t preflight = manifest_run_preflight(&registry);

    if (preflight == PREFLIGHT_STATUS_HALT_FATAL) {
        fprintf(stderr, "\n[FATAL ERROR] Engine startup halted due to loose bolts!\n");
        manifest_print_report(&registry);
        return 1;
    }

    player_state_t state;
    game_init(&state);

    if (force_cli) {
        game_loop_cli(&state);
        return 0;
    }

    if (force_gui) {
        if (!gui_is_available()) {
            fprintf(stderr, "Error: No graphical X11 display available for --gui.\n");
            return 1;
        }
        return gui_run(&state);
    }

    /* Graded policy check: If pre-flight detected missing presentation layer, downgrade */
    if (preflight == PREFLIGHT_STATUS_DOWNGRADED || !gui_is_available()) {
        printf("[Notice] Graphical display unavailable. Downgrading to CLI safe mode.\n\n");
        game_loop_cli(&state);
        return 0;
    }

    /* Default launch mode */
    return gui_run(&state);
}
