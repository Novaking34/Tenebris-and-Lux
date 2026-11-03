// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_dispatch_unit.c
// @id            test.unit.dispatch
// @layer         Layer 4: Verification Suite
// @subsystem     Simulation Dispatch Unit Verification (tests/unit)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Comprehensive unit verification for the simulation command dispatcher:
//   tokenization, alias resolution, domain classification, and orchestrated
//   delegation across Combat, World, Items, and System domains.
//   Organized across the 4 Ontological Testing Sections.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/dispatch/dispatcher.h,
//                            sim/dispatch/dispatch_combat.h,
//                            sim/dispatch/dispatch_world.h,
//                            sim/dispatch/dispatch_items.h,
//                            sim/dispatch/dispatch_system.h,
//                            sim/dispatch/dispatch_loader.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        < 20 milliseconds for entire suite.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/core/sim_state.h"
#include "sim/dispatch/dispatch_combat.h"
#include "sim/dispatch/dispatch_items.h"
#include "sim/dispatch/dispatch_loader.h"
#include "sim/dispatch/dispatch_system.h"
#include "sim/dispatch/dispatch_types.h"
#include "sim/dispatch/dispatch_world.h"
#include "sim/dispatch/dispatcher.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

// =============================================================================
// BLOCK 3: BODY BLOCK / 4-SECTION TEST SUITES
// =============================================================================

static void test_dispatch_always_pass(void)
{
    printf("  [PASS-1] Section 1: Always Pass (Nominal Baseline)...\n");

    /* 1.1 Domain classification */
    TEST_ASSERT(dispatcher_classify_verb("attack") == DOMAIN_COMBAT,
                "Verb 'attack' classified as DOMAIN_COMBAT");
    TEST_ASSERT(dispatcher_classify_verb("move") == DOMAIN_WORLD,
                "Verb 'move' classified as DOMAIN_WORLD");
    TEST_ASSERT(dispatcher_classify_verb("items") == DOMAIN_ITEMS,
                "Verb 'items' classified as DOMAIN_ITEMS");
    TEST_ASSERT(dispatcher_classify_verb("status") == DOMAIN_SYSTEM,
                "Verb 'status' classified as DOMAIN_SYSTEM");

    /* 1.2 Alias resolution */
    char canonical[32] = {0};
    char def_arg[64] = {0};
    bool res1 = dispatcher_resolve_alias("strike", canonical, sizeof(canonical),
                                         def_arg, sizeof(def_arg));
    TEST_ASSERT(res1 && strcmp(canonical, "attack") == 0,
                "Alias 'strike' resolves to canonical 'attack'");
    TEST_ASSERT(strcmp(def_arg, "cuttatio") == 0,
                "Alias 'strike' provides default argument 'cuttatio'");

    bool res2 = dispatcher_resolve_alias("ward", canonical, sizeof(canonical),
                                         def_arg, sizeof(def_arg));
    TEST_ASSERT(res2 && strcmp(canonical, "defend") == 0,
                "Alias 'ward' resolves to canonical 'defend'");
    TEST_ASSERT(strcmp(def_arg, "barrio") == 0,
                "Alias 'ward' provides default argument 'barrio'");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

static void test_dispatch_never_fail(void)
{
    printf("  [FAIL-2] Section 2: Never Fail (Fault-Tolerant Hardening)...\n");

    player_state_t state;
    sim_state_init(&state);

    /* 2.1 NULL input handling */
    dispatcher_process_command(&state, NULL, NULL, 0);
    dispatcher_process_command(NULL, "strike", NULL, 0);
    TEST_ASSERT(state.turns_taken == 0, "NULL inputs produced zero mutations");

    /* 2.2 Whitespace-only input */
    dispatcher_process_command(&state, "    ", NULL, 0);
    TEST_ASSERT(state.turns_taken == 0, "Whitespace input produced zero turns");

    /* 2.3 Unknown verb handling */
    char out[128];
    dispatcher_process_command(&state, "unknown_gibberish_verb_xyz",
                               out, sizeof(out));
    TEST_ASSERT(state.turns_taken == 0, "Unknown verb produced zero turns");
    TEST_ASSERT(strstr(out, "Unknown tactical command") != NULL,
                "Informative fallback message emitted on unknown verb");

    printf("  [FAIL-2] Section 2 passed cleanly.\n");
}

static void test_dispatch_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Section 3: Assertion & Guarantee (Invariants)...\n");

    player_state_t state;
    sim_state_init(&state);
    char msg[256];

    /* 3.1 Combat action dispatch increments turn */
    dispatcher_process_command(&state, "strike", msg, sizeof(msg));
    TEST_ASSERT(state.turns_taken == 1, "'strike' advanced turn to 1");
    TEST_ASSERT(state.threat_hp < state.threat_max_hp,
                "'strike' inflicted damage on threat");

    /* 3.2 Barrier ward dispatch */
    dispatcher_process_command(&state, "ward", msg, sizeof(msg));
    TEST_ASSERT(state.turns_taken == 2, "'ward' advanced turn to 2");
    TEST_ASSERT(state.lux_barrio_turns > 0, "Lux Barrio active barrier raised");

    /* 3.3 Rename hero dispatch */
    dispatcher_process_command(&state, "name Roland", msg, sizeof(msg));
    TEST_ASSERT(strcmp(state.name, "Roland") == 0, "Hero designated as Roland");

    /* 3.4 Quit session dispatch */
    dispatcher_process_command(&state, "quit", msg, sizeof(msg));
    TEST_ASSERT(!state.is_running, "Session terminated cleanly on quit");

    printf("  [AXIOM-3] Section 3 passed cleanly.\n");
}

static void test_dispatch_emergence(void)
{
    printf("  [EMERGE-4] Section 4: Emergence (Command Chaining)...\n");

    player_state_t state;
    sim_state_init(&state);
    char msg[256];

    /* Multi-verb script through the master orchestrator */
    const char *script[] = {
        "stance striking",
        "name Casatti",
        "move north",
        "items",
        "status",
        "quit"
    };

    for (size_t i = 0; i < sizeof(script) / sizeof(script[0]); i++) {
        dispatcher_process_command(&state, script[i], msg, sizeof(msg));
    }

    TEST_ASSERT(state.stance == STANCE_STRIKING, "Stance is Striking");
    TEST_ASSERT(strcmp(state.name, "Casatti") == 0, "Name is Casatti");
    TEST_ASSERT(!state.is_running, "Execution loop completed cleanly");

    printf("  [EMERGE-4] Section 4 passed cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_dispatch_preflight(void)
{
    printf("[AUDIT] Running Dispatch Subsystem Pre-Flight Verification...\n");
    diag_result_t res = dispatcher_preflight();
    TEST_ASSERT(res.passed, "Dispatcher preflight passed");
    printf("[AUDIT] Dispatcher preflight passed: %s\n", res.details);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TEST RUNNER
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - Dispatch Unit Verification Suite    \n");
    printf("============================================================\n");

    test_dispatch_preflight();
    test_dispatch_always_pass();
    test_dispatch_never_fail();
    test_dispatch_assertions_and_guarantees();
    test_dispatch_emergence();

    printf("============================================================\n");
    printf("ALL DISPATCH UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
