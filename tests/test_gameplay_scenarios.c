// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_gameplay_scenarios.c
// @layer         Verification & Game Dev Test Suite
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Game Development Gameplay Scenarios, Replay & Performance Suite
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Deterministic replay invariance: identical inputs yield identical state.
//                2. Softlock immunity: no sequence of commands causes deadlock.
//                3. 60 FPS budget: action simulation latency < 50 microseconds per turn.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// ( System Headers & High-Resolution Timing )
// =============================================================================

#define _POSIX_C_SOURCE 199309L

#include "sim/game.h"
#include "util/time_utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
// ( Scenario Replay & Gameplay Flow Tests )
// =============================================================================

static void test_scenario_righteous_warden(void)
{
    printf("[RUN] Testing Scenario: The Righteous Warden Path...\n");
    player_state_t state;
    game_init(&state);

    /* 1. At Garrison Altar, adopt Aegis Warding stance */
    game_process_command(&state, "stance warding", NULL, 0);
    TEST_ASSERT(state.stance == STANCE_WARDING, "Stance is warding");

    /* 2. Advance North to Citadel Gates */
    game_process_command(&state, "move north", NULL, 0);
    TEST_ASSERT(state.location == LOC_CITADEL_GATES, "Reached Citadel Gates");
    TEST_ASSERT(state.threat_hp == 50, "Encountered Citadel vanguard (50 HP)");

    /* 3. Raise Shield of Lux against the enemy vanguard twice */
    game_process_command(&state, "ward", NULL, 0); // Stagger 25 -> 25 HP left
    TEST_ASSERT(state.threat_hp == 25, "First ward staggered foe to 25 HP");
    TEST_ASSERT(state.moral_sway > 0, "Warding increased moral virtue");

    game_process_command(&state, "ward", NULL, 0); // Stagger 25 -> 0 HP left (routed)
    TEST_ASSERT(state.threat_hp == 0, "Second ward routed enemy vanguard");
    TEST_ASSERT(state.foes_vanquished == 1, "Vanquished 1 foe via radiant ward");
    TEST_ASSERT(state.innocents_warded == 2, "Innocents warded count updated");

    game_audit_report_t rep = game_audit_state(&state);
    TEST_ASSERT(rep.is_valid, "Righteous Warden state remains valid");
    printf("[PASS] Righteous Warden scenario verified.\n");
}

static void test_scenario_umbral_striker(void)
{
    printf("[RUN] Testing Scenario: The Umbral Striker Path...\n");
    player_state_t state;
    game_init(&state);

    /* 1. Move North to Citadel Gates and shift to Edge Striking stance */
    game_process_command(&state, "move north", NULL, 0);
    game_process_command(&state, "stance striking", NULL, 0);
    TEST_ASSERT(state.stance == STANCE_STRIKING, "Stance is striking");
    TEST_ASSERT(state.threat_hp == 50, "Encountered Citadel vanguard");

    /* 2. Cleave with the Sword of Tenebris */
    game_process_command(&state, "strike", NULL, 0); // 45 damage -> 5 HP left
    TEST_ASSERT(state.threat_hp == 5, "Striking stance dealt 45 severance damage");
    TEST_ASSERT(state.sword_edge < INITIAL_SWORD_EDGE, "Sword edge spent on strike");

    /* 3. Deliver finishing blow */
    game_process_command(&state, "strike", NULL, 0);
    TEST_ASSERT(state.threat_hp == 0, "Finishing strike eliminated vanguard");
    TEST_ASSERT(state.foes_vanquished == 1, "Foe recorded as vanquished");

    game_audit_report_t rep = game_audit_state(&state);
    TEST_ASSERT(rep.is_valid, "Umbral Striker state remains valid");
    printf("[PASS] Umbral Striker scenario verified.\n");
}

static void test_scenario_deterministic_golden_replay(void)
{
    printf("[RUN] Testing Scenario: Deterministic Golden Replay...\n");

    const char *script[] = {
        "look",
        "stance warding",
        "ward",
        "move north",
        "ward",
        "strike",
        "balance",
        "stance equilibrium",
        "move north",
        "look",
        "status",
        "balance",
        "move east",
        "ward",
        "strike",
        "balance",
        "status"
    };
    size_t script_len = sizeof(script) / sizeof(script[0]);

    player_state_t run1;
    game_init(&run1);
    for (size_t i = 0; i < script_len; i++) {
        game_process_command(&run1, script[i], NULL, 0);
    }

    player_state_t run2;
    game_init(&run2);
    for (size_t i = 0; i < script_len; i++) {
        game_process_command(&run2, script[i], NULL, 0);
    }

    /* Assert 100% deterministic bitwise parity across runs */
    TEST_ASSERT(run1.shield_power == run2.shield_power, "Deterministic shield power");
    TEST_ASSERT(run1.sword_edge == run2.sword_edge, "Deterministic sword edge");
    TEST_ASSERT(run1.moral_sway == run2.moral_sway, "Deterministic moral sway");
    TEST_ASSERT(run1.stance == run2.stance, "Deterministic stance");
    TEST_ASSERT(run1.location == run2.location, "Deterministic location");
    TEST_ASSERT(run1.threat_hp == run2.threat_hp, "Deterministic threat HP");
    TEST_ASSERT(run1.foes_vanquished == run2.foes_vanquished, "Deterministic foes vanquished");
    TEST_ASSERT(run1.turns_taken == run2.turns_taken, "Deterministic turns count");

    printf("[PASS] Golden replay matches perfectly across independent runs.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Softlock Stress & Performance Headroom Benchmarks )
// =============================================================================

static void test_softlock_and_stress_invariance(void)
{
    printf("[RUN] Testing softlock immunity under 1,000 rapid inputs...\n");
    player_state_t state;
    game_init(&state);

    const char *chaotic_inputs[] = {
        "ward", "strike", "balance", "look", "status",
        "stance warding", "stance striking", "stance equilibrium", "stance neutral",
        "move north", "move south", "move east", "move west",
        "", "   ", "unknown_verb", "move", "stance", "ward now", "help"
    };
    size_t num_inputs = sizeof(chaotic_inputs) / sizeof(chaotic_inputs[0]);

    for (int i = 0; i < 1000; i++) {
        const char *cmd = chaotic_inputs[i % num_inputs];
        game_process_command(&state, cmd, NULL, 0);

        /* Invariant audit tripwire after each command */
        game_audit_report_t rep = game_audit_state(&state);
        TEST_ASSERT(rep.is_valid, "State must remain valid under continuous input stress");
    }

    TEST_ASSERT(state.turns_taken > 0, "Turns progressed normally");
    printf("[PASS] 1,000 chaotic commands executed with zero state corruption or softlocks.\n");
}

static void test_simulation_latency_and_headroom(void)
{
    printf("[RUN] Benchmarking simulation tick latency (10,000 turns)...\n");
    player_state_t state;
    game_init(&state);

    double start_us = time_get_monotonic_us();

    for (int i = 0; i < 10000; i++) {
        game_process_command(&state, "balance", NULL, 0);
    }

    double end_us = time_get_monotonic_us();
    double elapsed_ms = (end_us - start_us) / 1000.0;
    double us_per_turn = (end_us - start_us) / 10000.0;

    printf("[BENCHMARK] 10,000 turns executed in %.2f ms (%.3f us / turn)\n",
        elapsed_ms, us_per_turn);

    /* Assert that per-turn latency is under 50 microseconds (leaving full 60 FPS headroom) */
    TEST_ASSERT(us_per_turn < 50.0, "Simulation turn latency < 50 us");
    printf("[PASS] Simulation latency easily meets 60 FPS frame-budget requirements.\n");
}

// =============================================================================
// BLOCK 5: CLOSING / TEST & CLI BLOCK
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("    Tenebris and Lux - Game Dev Scenario & Replay Suite     \n");
    printf("============================================================\n");

    test_scenario_righteous_warden();
    test_scenario_umbral_striker();
    test_scenario_deterministic_golden_replay();
    test_softlock_and_stress_invariance();
    test_simulation_latency_and_headroom();

    printf("============================================================\n");
    printf("ALL GAMEPLAY SCENARIO TESTS PASSED (5/5 Suites Succeeded)\n");
    printf("============================================================\n");
    return 0;
}
