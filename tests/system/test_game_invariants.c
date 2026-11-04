// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_game_invariants.c
// @id            test.system.invariants
// @layer         Layer 4: Verification Suite
// @subsystem     Game Simulation Invariants Harness (tests/system)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-10-26
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   System-level simulation engine invariant verification harness.
//   Verifies bounded twin relics energy, moral sway continuum bounds,
//   hero name customization, deterministic stance transitions, and topological
//   provincial movements across four ontological verification sections:
//     1. Always Pass (Nominal Simulation State & Awakening Baseline)
//     2. Never Fail (Intentional State Corruption Tripwires & Unknown Stances)
//     3. Assertion & Guarantee (Axiomatic Continuum Bounds & Relic Ceilings)
//     4. Emergence (Bi-Directional Province Topology & Dynamic Sway Shifts)
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.engine.core, core.util.*
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Deterministic test execution).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        Total suite completes in under 1 second.
//
// AXIOMATIC INVARIANTS:
//   1. Relic energy invariants: 0 <= shield <= 100, 0 <= sword <= 100.
//   2. Stance transition state machine is deterministic and complete.
//   3. Topological paths across Kingdom provinces are strictly bounded.
//   4. Diagnostic tripwire reports invalid state immediately upon corruption.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// ( System Headers & Test Assertion Macros )
// =============================================================================

#include "ending/game.h"
#include "world/world.h"
#include "dispatch/dispatcher.h"

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
// BLOCK 3: BODY BLOCK
// ( Four-Tier Ontological Test Suites )
// =============================================================================

// -----------------------------------------------------------------------------
// [ SECTION 1: TESTS THAT SHOULD ALWAYS PASS (NOMINAL SYSTEM BASELINE) ]
// -----------------------------------------------------------------------------

static void test_initial_state_invariants(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal System State & Customization)...\n");
    player_state_t state;
    game_init(&state);

    TEST_ASSERT(state.shield_power == INITIAL_SHIELD_POWER, "Initial shield power match");
    TEST_ASSERT(state.sword_edge == INITIAL_SWORD_EDGE, "Initial sword edge match");
    TEST_ASSERT(state.moral_sway == INITIAL_MORAL_SWAY, "Initial moral sway zero");
    TEST_ASSERT(state.stance == STANCE_NEUTRAL, "Initial stance neutral");
    TEST_ASSERT(state.location == LOC_THE_DREAMSCAPE, "Initial location at The Dreamscape");
    TEST_ASSERT(state.is_running == true, "Game is initially running");
    TEST_ASSERT(state.threat_hp == 70, "Initial trial threat in The Dreamscape");
    TEST_ASSERT(strcmp(state.name, DEFAULT_PROTAGONIST_NAME) == 0, "Default protagonist name Elijah");

    game_audit_report_t rep = game_audit_state(&state);
    TEST_ASSERT(rep.is_valid, "Initial audit report is valid");

    /* Hero name customization */
    game_set_protagonist_name(&state, "Katrina");
    TEST_ASSERT(strcmp(state.name, "Katrina") == 0, "Custom hero name Katrina applied");

    game_process_command(&state, "name Corvus", NULL, 0);
    TEST_ASSERT(strcmp(state.name, "Corvus") == 0, "Command name Corvus applied");

    game_audit_report_t rep2 = game_audit_state(&state);
    TEST_ASSERT(rep2.is_valid, "State remains valid after name customization");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 2: TESTS THAT SHOULD NEVER FAIL (NEGATIVE FAULT TRIPWIRES) ]
// -----------------------------------------------------------------------------

static void test_diagnostic_tripwire_manifests(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (State Corruption Tripwires)...\n");
    player_state_t state;
    game_init(&state);

    /* Case A: Corrupted Shield Power caught by audit */
    state.shield_power = -5;
    game_audit_report_t r_shield = game_audit_state(&state);
    TEST_ASSERT(!r_shield.is_valid, "Tripwire caught negative shield power");

    /* Case B: Corrupted Sword Edge caught by audit */
    state.shield_power = 80;
    state.sword_edge = 150;
    game_audit_report_t r_sword = game_audit_state(&state);
    TEST_ASSERT(!r_sword.is_valid, "Tripwire caught excessive sword edge");

    /* Case C: Corrupted Stance caught by audit */
    state.sword_edge = 80;
    state.stance = (combat_stance_t)99;
    game_audit_report_t r_stance = game_audit_state(&state);
    TEST_ASSERT(!r_stance.is_valid, "Tripwire caught invalid stance");

    /* Case D: Unknown stance command input preserves current stance */
    state.stance = STANCE_NEUTRAL;
    game_process_command(&state, "stance gibberish_unknown", NULL, 0);
    TEST_ASSERT(state.stance == STANCE_NEUTRAL, "Invalid stance command rejects safely without corruption");

    printf("  [FAIL-2] Section 2 passed: All diagnostic corruption tripwires caught.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 3: TESTS THAT ARE AN ASSERTION AND GUARANTEE (BOUNDS AXIOMS) ]
// -----------------------------------------------------------------------------

static void test_relic_and_moral_axioms(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Continuum Bounds Axioms)...\n");
    player_state_t state;
    game_init(&state);

    /* 3.1 Relic energy boundaries: 0 <= shield <= 100, 0 <= sword <= 100 */
    for (int i = 0; i < 20; i++) {
        game_process_command(&state, "defend", NULL, 0);
        TEST_ASSERT(state.shield_power <= MAX_RESONANCE, "Shield cannot exceed MAX_RESONANCE");
        TEST_ASSERT(state.shield_power >= 0, "Shield cannot be negative");
    }
    TEST_ASSERT(state.shield_power == MAX_RESONANCE, "Shield power clamped at MAX_RESONANCE");

    state.threat_hp = 0; // vanquish threat to hone out-of-combat edge
    for (int i = 0; i < 20; i++) {
        game_process_command(&state, "strike", NULL, 0);
        TEST_ASSERT(state.sword_edge <= MAX_RESONANCE, "Sword edge cannot exceed MAX_RESONANCE");
        TEST_ASSERT(state.sword_edge >= 0, "Sword edge cannot be negative");
    }
    TEST_ASSERT(state.sword_edge == MAX_RESONANCE, "Sword edge clamped at MAX_RESONANCE");

    /* 3.2 Moral sway boundaries [-100, +100] */
    state.moral_sway = 98;
    game_process_command(&state, "ward", NULL, 0);
    game_process_command(&state, "ward", NULL, 0);
    TEST_ASSERT(state.moral_sway <= 100, "Moral sway cannot exceed +100");

    state.moral_sway = -99;
    game_process_command(&state, "balance", NULL, 0);
    TEST_ASSERT(state.moral_sway == -96, "Balance nudges negative sway toward equilibrium");

    /* 3.3 Stance state machine completeness */
    game_process_command(&state, "stance warding", NULL, 0);
    TEST_ASSERT(state.stance == STANCE_WARDING, "Transition to STANCE_WARDING");
    game_process_command(&state, "stance striking", NULL, 0);
    TEST_ASSERT(state.stance == STANCE_STRIKING, "Transition to STANCE_STRIKING");
    game_process_command(&state, "stance equilibrium", NULL, 0);
    TEST_ASSERT(state.stance == STANCE_EQUILIBRIUM, "Transition to STANCE_EQUILIBRIUM");
    game_process_command(&state, "stance neutral", NULL, 0);
    TEST_ASSERT(state.stance == STANCE_NEUTRAL, "Transition to STANCE_NEUTRAL");

    printf("  [AXIOM-3] Section 3 passed: All continuum and state machine invariants guaranteed.\n");
}

// -----------------------------------------------------------------------------
// [ SECTION 4: TESTS THAT ACCOUNT FOR EMERGENCE (SPATIAL TOPOLOGY) ]
// -----------------------------------------------------------------------------

static void test_spatial_topology_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Spatial Topology & Province Transitions)...\n");
    player_state_t state;
    game_init(&state);

    /* In Dreamscape: invalid movements safely rejected */
    game_process_command(&state, "move west", NULL, 0);
    TEST_ASSERT(state.location == LOC_THE_DREAMSCAPE, "Cannot move west from The Dreamscape");
    game_process_command(&state, "move east", NULL, 0);
    TEST_ASSERT(state.location == LOC_THE_DREAMSCAPE, "Cannot move east from The Dreamscape");

    /* Move North into Dio Village */
    game_process_command(&state, "move north", NULL, 0);
    TEST_ASSERT(state.location == LOC_DIO_VILLAGE, "Moved North into Dio Village");

    /* Move South back to The Dreamscape */
    game_process_command(&state, "move south", NULL, 0);
    TEST_ASSERT(state.location == LOC_THE_DREAMSCAPE, "Returned South to The Dreamscape");

    game_audit_report_t rep = game_audit_state(&state);
    TEST_ASSERT(rep.is_valid, "State remains valid across bi-directional province navigation");

    printf("  [EMERGE-4] Section 4 passed: Province transitions cleanly emerged without drift.\n");
}

static void test_dynamic_map_viewport_and_dispatch_invariants(void)
{
    printf("  [AXIOM-5] Running Section 5: Dynamic Viewport & Dispatch Invariants...\n");
    player_state_t state;
    game_init(&state);

    /* 5.1 Dynamic Viewport Scale and Offset Invariants across resolutions */
    world_update_viewport(800, 600);
    const map_viewport_t *vp_800 = world_get_viewport();
    TEST_ASSERT(vp_800->scale_factor > 1.24f && vp_800->scale_factor < 1.26f,
                "Viewport scale factor for 800x600 is 1.25");
    TEST_ASSERT(vp_800->offset_y > 0, "Vertical letterbox offset is positive");

    world_update_viewport(1920, 1080);
    const map_viewport_t *vp_1080 = world_get_viewport();
    TEST_ASSERT(vp_1080->scale_factor > 2.99f && vp_1080->scale_factor < 3.01f,
                "Viewport scale factor for 1920x1080 is 3.0");
    TEST_ASSERT(vp_1080->offset_x == 0 && vp_1080->offset_y == 0,
                "1080p exact fit produces zero letterbox offsets");

    /* 5.2 Dispatcher alias resolution determinism */
    char canonical[32] = {0};
    char def_arg[64] = {0};
    dispatcher_resolve_alias("strike", canonical, sizeof(canonical),
                             def_arg, sizeof(def_arg));
    TEST_ASSERT(strcmp(canonical, "attack") == 0,
                "Alias 'strike' resolved to canonical 'attack'");
    TEST_ASSERT(strcmp(def_arg, "cuttatio") == 0,
                "Alias 'strike' assigned default argument 'cuttatio'");

    dispatcher_resolve_alias("ward", canonical, sizeof(canonical),
                             def_arg, sizeof(def_arg));
    TEST_ASSERT(strcmp(canonical, "defend") == 0,
                "Alias 'ward' resolved to canonical 'defend'");
    TEST_ASSERT(strcmp(def_arg, "barrio") == 0,
                "Alias 'ward' assigned default argument 'barrio'");

    /* 5.3 Dispatcher domain classification bounds */
    TEST_ASSERT(dispatcher_classify_verb("attack") == DOMAIN_COMBAT,
                "attack is DOMAIN_COMBAT");
    TEST_ASSERT(dispatcher_classify_verb("move") == DOMAIN_WORLD,
                "move is DOMAIN_WORLD");
    TEST_ASSERT(dispatcher_classify_verb("items") == DOMAIN_ITEMS,
                "items is DOMAIN_ITEMS");
    TEST_ASSERT(dispatcher_classify_verb("status") == DOMAIN_SYSTEM,
                "status is DOMAIN_SYSTEM");
    TEST_ASSERT(dispatcher_classify_verb("non_existent_verb") == DOMAIN_UNKNOWN,
                "non_existent_verb is DOMAIN_UNKNOWN");

    printf("  [AXIOM-5] Section 5 passed: Viewport scaling & dispatch routing invariants verified.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

// Dedicated tripwire audits exercised in Section 2

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("     Tenebris and Lux - System State Invariants Harness     \n");
    printf("============================================================\n");

    test_initial_state_invariants();
    test_diagnostic_tripwire_manifests();
    test_relic_and_moral_axioms();
    test_spatial_topology_emergence();
    test_dynamic_map_viewport_and_dispatch_invariants();

    printf("============================================================\n");
    printf("ALL SYSTEM INVARIANT TESTS PASSED (5/5 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
