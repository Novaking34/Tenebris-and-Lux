// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          main.c
// @layer         Application Entry & CLI Coordinator
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Executable Entry Point, Argument Parser & Presentation Router
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Headless fallback guarantees graceful degradation when display absent.
//                2. Zero-warning clean compilation under -std=c11 -Wall -Wextra.
//                3. Diagnostic self-audit returns 0 only when all invariants pass.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// ( System Headers & Application Constants )
// =============================================================================

#include "main.h"
#include "gui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    printf("============================================================\n");
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
    printf("Audit Result: ALL INVARIANT CHECKS PASSED (5/5)\n");
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

    /* Default behavior: Launch native GUI if display available, otherwise CLI */
    if (gui_is_available()) {
        return gui_run(&state);
    }

    printf("[Notice] No graphical display detected. Launching in CLI mode.\n\n");
    game_loop_cli(&state);
    return 0;
}
