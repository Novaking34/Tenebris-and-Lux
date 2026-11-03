// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_sim_integrated.c
// @id            test.integrated.sim
// @layer         Layer 4: Verification Suite
// @subsystem     Simulation Cross-Subsystem Harness (tests/integrated)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Cross-subsystem integration verification testing the synchronized
//   pipeline between Combat Orchestration, Item Inventory, Dynamic Map Viewport
//   Projection, Dispatcher Domain Routing, and Interactive Debugger Rewind.
//   Organized across the 4 Ontological Testing Sections.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/core/game.h, sim/dispatch/dispatcher.h,
//                            sim/combat/combat.h, sim/items/items.h,
//                            sim/world/world.h, util/debug/debug.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        < 50 milliseconds for entire suite.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/combat/combat.h"
#include "sim/core/game.h"
#include "sim/core/sim_state.h"
#include "sim/dispatch/dispatcher.h"
#include "sim/items/inventory.h"
#include "sim/items/items.h"
#include "sim/world/world.h"
#include "util/debug/debug.h"
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

static void test_sim_integrated_always_pass(void)
{
    printf("  [PASS-1] Section 1: Always Pass (Nominal Baseline)...\n");

    player_state_t state;
    game_init(&state);

    /* 1.1 Initial game facade state */
    TEST_ASSERT(state.hp == 450, "Facade initializes Elijah with 450 HP");
    TEST_ASSERT(state.location == LOC_THE_DREAMSCAPE,
                "Initial realm is The Dreamscape");
    TEST_ASSERT(state.stance == STANCE_NEUTRAL, "Initial stance is Neutral");

    /* 1.2 Dispatcher routes stance shift to combat */
    char msg[256];
    dispatcher_process_command(&state, "stance warding", msg, sizeof(msg));
    TEST_ASSERT(state.stance == STANCE_WARDING, "Shifted to Barrier Warding");

    /* 1.3 Dispatcher routes defend barrio */
    dispatcher_process_command(&state, "ward", msg, sizeof(msg));
    TEST_ASSERT(state.lux_barrio_turns > 0, "Lux Barrio barrier raised");
    TEST_ASSERT(state.turns_taken == 1, "Turn count incremented to 1");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

static void test_sim_integrated_never_fail(void)
{
    printf("  [FAIL-2] Section 2: Never Fail (Fault-Tolerant Stress)...\n");

    player_state_t state;
    game_init(&state);
    char msg[256];

    /* 2.1 Malformed and empty inputs through pipeline */
    dispatcher_process_command(&state, "   ", msg, sizeof(msg));
    dispatcher_process_command(&state, "invalid_command_xyz", msg, sizeof(msg));
    TEST_ASSERT(state.turns_taken == 0,
                "Malformed commands produced zero turn increments");

    /* 2.2 Boundary violation: using unowned item */
    dispatcher_process_command(&state, "use 999", msg, sizeof(msg));
    TEST_ASSERT(state.turns_taken == 0,
                "Unowned item consumption safely rejected");

    printf("  [FAIL-2] Section 2 passed cleanly.\n");
}

static void test_sim_integrated_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Section 3: Assertion & Guarantee (Invariants)...\n");

    player_state_t state;
    game_init(&state);
    char msg[256];

    /* 3.1 Equip catalyst -> stance shift -> attack damage bonus */
    dispatcher_process_command(&state, "equip medusa", msg, sizeof(msg));
    TEST_ASSERT(state.has_medusas_sword, "Equipped Medusa's Sword");

    dispatcher_process_command(&state, "stance striking", msg, sizeof(msg));
    TEST_ASSERT(state.stance == STANCE_STRIKING, "Shifted to Edge Striking");

    /* Strike threat */
    dispatcher_process_command(&state, "strike", msg, sizeof(msg));
    TEST_ASSERT(state.threat_hp < state.threat_max_hp,
                "Threat damaged with catalyst bonus");

    /* 3.2 Dynamic map viewport updates without altering world simulation */
    world_update_viewport(1920, 1080);
    const map_viewport_t *vp = world_get_viewport();
    TEST_ASSERT(vp->scale_factor > 2.99f && vp->scale_factor < 3.01f,
                "Viewport dynamically scaled to 3.0x for 1080p");
    TEST_ASSERT(state.threat_hp < state.threat_max_hp,
                "Simulation world truth preserved across display resize");

    printf("  [AXIOM-3] Section 3 passed cleanly.\n");
}

static void test_sim_integrated_emergence(void)
{
    printf("  [EMERGE-4] Section 4: Emergence (Campaign & Rewind)...\n");

    player_state_t state;
    game_init(&state);
    char msg[256];

    /* Step 1: Capture initial snapshot in Dreamscape */
    debug_init(NULL);
    game_record_debug_snapshot(&state, "debug", "Initial_Dreamscape");
    size_t snap_initial = debug_get_snapshot_count() - 1;

    /* Step 2: Traverse to Dio Village */
    dispatcher_process_command(&state, "move north", msg, sizeof(msg));
    TEST_ASSERT(state.location == LOC_DIO_VILLAGE, "Arrived at Dio Village");

    /* Step 3: Add potion to inventory and equip catalyst */
    inventory_add(&state.inventory, NULL, 0, 1, 2);
    dispatcher_process_command(&state, "equip ring_of_fire", msg, sizeof(msg));
    TEST_ASSERT(state.has_ring_of_fire, "Equipped Ring of Fire");

    /* Step 4: Traverse North into Whispering Woods */
    dispatcher_process_command(&state, "move north", msg, sizeof(msg));
    TEST_ASSERT(state.location == LOC_WHISPERING_WOODS, "Arrived in Woods");
    TEST_ASSERT(state.threat_hp > 0, "Threat active in Woods");

    /* Step 5: Capture mid-battle snapshot */
    game_record_debug_snapshot(&state, "debug", "Woods_Battle");
    size_t snap_battle = debug_get_snapshot_count() - 1;
    TEST_ASSERT(snap_battle > snap_initial, "Mid-battle snapshot captured");

    /* Step 6: Strike foe, drink potion */
    dispatcher_process_command(&state, "strike", msg, sizeof(msg));
    dispatcher_process_command(&state, "use 1", msg, sizeof(msg));

    /* Step 7: Time-travel rewind to initial state */
    bool rewound = game_rewind_debug_snapshot(snap_initial, &state);
    TEST_ASSERT(rewound, "Rewound back to initial snapshot");
    TEST_ASSERT(state.location == LOC_THE_DREAMSCAPE,
                "Restored location to The Dreamscape");
    TEST_ASSERT(state.turns_taken == 0, "Restored turn count to 0");
    TEST_ASSERT(state.threat_hp == 70, "Restored threat HP to 70");

    debug_shutdown();

    printf("  [EMERGE-4] Section 4 passed cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_sim_integrated_preflight(void)
{
    printf("[AUDIT] Running Simulation Integration Pre-Flight...\n");
    diag_result_t game_res = game_preflight();
    TEST_ASSERT(game_res.passed, "Game facade preflight passed");
    printf("[AUDIT] Integrated preflight passed: %s\n", game_res.details);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TEST RUNNER
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("  Tenebris and Lux - Simulation Integration Verification    \n");
    printf("============================================================\n");

    test_sim_integrated_preflight();
    test_sim_integrated_always_pass();
    test_sim_integrated_never_fail();
    test_sim_integrated_assertions_and_guarantees();
    test_sim_integrated_emergence();

    printf("============================================================\n");
    printf("ALL SIMULATION INTEGRATED TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
