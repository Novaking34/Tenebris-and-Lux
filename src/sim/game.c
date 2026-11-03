// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          game.c
// @id            sim.engine.core
// @layer         Layer 1: Simulation Engine
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Cohesive simulation engine facade orchestrator. Unites the modular
//   sub-assemblies (sim_state, combat, world, dispatcher) providing a single
//   clean interface for presentation viewports and test harnesses.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.engine.state, sim.engine.combat,
//                            sim.engine.world, sim.engine.dispatcher
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions)
//   - Latency Budget:        < 50 microseconds per simulation tick
//
// AXIOMATIC INVARIANTS:
//   1. Cohesive facade delegates purely to modular sub-assemblies.
//   2. Shield of Lux power bounded: 0 <= shield_power <= MAX_RESONANCE.
//   3. Sword of Tenebris edge bounded: 0 <= sword_edge <= MAX_RESONANCE.
//   4. Moral sway bounded continuum: -100 <= moral_sway <= +100.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

#include "sim/game.h"
#include "sim/sim_state.h"
#include "sim/combat.h"
#include "sim/world.h"
#include "sim/dispatcher.h"
#include "util/debug/debug.h"
#include "util/string/string_utils.h"

#include <stdio.h>
#include <string.h>

static const char *const s_game_invariants[4] = {
    "Shield of Lux resonance bounded: [0, MAX_RESONANCE]",
    "Sword of Tenebris edge bounded: [0, MAX_RESONANCE]",
    "Moral sway continuum bounded: [-100, +100]",
    "Deterministic stance transitions & topology"
};

const module_metadata_t game_manifest = {
    "sim.engine.core",
    "game.h",
    LAYER_1_SIMULATION,
    { 1, 1, 0 },
    2,                          // interface_rev
    "2026-10-30",               // last_updated
    "Simulation engine facade orchestrating state, combat, world & dispatcher",
    4,                          // dependency_count
    {
        "sim.engine.state",
        "sim.engine.combat",
        "sim.engine.world",
        "sim.engine.dispatcher"
    },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    50,                         // max_budget_us
    4,
    {
        s_game_invariants[0],
        s_game_invariants[1],
        s_game_invariants[2],
        s_game_invariants[3]
    },
    game_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / UNIFIED FACADE DELEGATION
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

void game_init(player_state_t *state)
{
    sim_state_init(state);
}

void game_set_protagonist_name(player_state_t *state, const char *name)
{
    sim_state_set_name(state, name);
}

const char *game_get_location_name(location_id_t loc)
{
    return world_get_location_name(loc);
}

const char *game_get_location_desc(location_id_t loc)
{
    return world_get_location_desc(loc);
}

const char *game_get_stance_name(combat_stance_t stance)
{
    return combat_get_stance_name(stance);
}

const char *game_get_sway_desc(int sway)
{
    return sim_state_get_sway_desc(sway);
}

void game_print_intro(void)
{
    dispatcher_print_intro();
}

void game_print_status(const player_state_t *state)
{
    dispatcher_print_status(state);
}

void game_print_location(const player_state_t *state)
{
    dispatcher_print_location(state);
}

void game_print_help(void)
{
    dispatcher_print_help();
}

void game_process_command(player_state_t *state, const char *raw_input,
                          char *out_msg, size_t msg_len)
{
    dispatcher_process_command(state, raw_input, out_msg, msg_len);
}

void game_loop_cli(player_state_t *state)
{
    dispatcher_loop_cli(state);
}

void game_state_to_debug_fields(const player_state_t *state, void *out_fields)
{
    if (state == NULL || out_fields == NULL) return;
    debug_state_fields_t *f = (debug_state_fields_t *)out_fields;
    memset(f, 0, sizeof(debug_state_fields_t));
    f->turn = state->turns_taken;
    str_safe_copy(f->hero_name, state->name, sizeof(f->hero_name));
    f->hp = state->hp;
    f->max_hp = state->max_hp;
    f->mp = state->mp;
    f->tp = state->tp;
    f->shield_power = state->shield_power;
    f->sword_edge = state->sword_edge;
    f->stance = (int)state->stance;
    f->moral_sway = state->moral_sway;
    f->location = (int)state->location;
    f->lux_barrio_turns = state->lux_barrio_turns;
    f->charm_turns = state->charm_turns;
    f->threat_hp = state->threat_hp;
    f->foes_vanquished = state->foes_vanquished;
}

void game_state_from_debug_fields(const void *fields, player_state_t *out_state)
{
    if (fields == NULL || out_state == NULL) return;
    const debug_state_fields_t *f = (const debug_state_fields_t *)fields;
    out_state->turns_taken = f->turn;
    str_safe_copy(out_state->name, f->hero_name, sizeof(out_state->name));
    out_state->hp = f->hp;
    out_state->max_hp = f->max_hp;
    out_state->mp = f->mp;
    out_state->tp = f->tp;
    out_state->shield_power = f->shield_power;
    out_state->sword_edge = f->sword_edge;
    out_state->stance = (combat_stance_t)f->stance;
    out_state->moral_sway = f->moral_sway;
    out_state->location = (location_id_t)f->location;
    out_state->lux_barrio_turns = f->lux_barrio_turns;
    out_state->charm_turns = f->charm_turns;
    out_state->threat_hp = f->threat_hp;
    out_state->foes_vanquished = f->foes_vanquished;
}

uint64_t game_record_debug_snapshot(const player_state_t *state, const char *cmd, const char *arg)
{
    if (state == NULL || !debug_is_active()) return 0;
    debug_state_fields_t f;
    game_state_to_debug_fields(state, &f);
    return debug_record_snapshot(&f, cmd, arg);
}

bool game_rewind_debug_snapshot(size_t index, player_state_t *out_state)
{
    if (out_state == NULL || !debug_is_active()) return false;
    debug_state_fields_t f;
    if (!debug_rewind_to_snapshot(index, &f)) return false;
    game_state_from_debug_fields(&f, out_state);
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Simulation Core
// =============================================================================

game_audit_report_t game_audit_state(const player_state_t *state)
{
    return sim_state_audit(state);
}

diag_result_t game_preflight(void)
{
    diag_result_t res;
    res.subsystem = game_manifest.module_id;
    res.check_name = "Modular Simulation Subsystem Integrity";

    // Sub-Assembly 1: State Preflight
    diag_result_t state_pre = sim_state_preflight();
    if (!state_pre.passed) {
        return state_pre;
    }

    // Sub-Assembly 2: Combat Preflight
    diag_result_t combat_pre = combat_preflight();
    if (!combat_pre.passed) {
        return combat_pre;
    }

    // Sub-Assembly 3: World Preflight
    diag_result_t world_pre = world_preflight();
    if (!world_pre.passed) {
        return world_pre;
    }

    // Sub-Assembly 4: Dispatcher Preflight
    diag_result_t disp_pre = dispatcher_preflight();
    if (!disp_pre.passed) {
        return disp_pre;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "All modular simulation sub-assemblies satisfied pre-flight.";
    return res;
}

int game_run_self_audit(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - Relic & Stance Invariant Self-Audit   \n");
    printf("============================================================\n");

    player_state_t state;
    game_init(&state);

    if (state.shield_power != INITIAL_SHIELD_POWER ||
        state.sword_edge != INITIAL_SWORD_EDGE ||
        state.moral_sway != INITIAL_MORAL_SWAY) {
        fprintf(stderr, "[FAIL] Test 1: Initial twin relic state corrupt!\n");
        return 1;
    }
    printf("[PASS] Test 1: Initial twin relic state verified.\n");

    game_process_command(&state, "barrio", NULL, 0);
    if (state.shield_power != MAX_RESONANCE) {
        fprintf(stderr, "[FAIL] Test 2: Shield did not reach ceiling! (%d)\n",
            state.shield_power);
        return 2;
    }
    printf("[PASS] Test 2: Lux Barrio & Shield ceiling invariant verified.\n");

    game_process_command(&state, "stance striking", NULL, 0);
    if (state.stance != STANCE_STRIKING) {
        fprintf(stderr, "[FAIL] Test 3: Stance did not transition to striking!\n");
        return 3;
    }

    game_process_command(&state, "stance equilibrium", NULL, 0);
    if (state.stance != STANCE_EQUILIBRIUM) {
        fprintf(stderr,
            "[FAIL] Test 3: Stance did not transition to equilibrium!\n");
        return 3;
    }
    printf("[PASS] Test 3: Stance state machine transitions verified.\n");

    state.shield_power = 999;
    game_audit_report_t rep = game_audit_state(&state);
    if (rep.is_valid || rep.error_count == 0) {
        fprintf(stderr,
            "[FAIL] Test 4: Diagnostic tripwire failed to catch corruption!\n");
        return 4;
    }
    printf("[PASS] Test 4: Diagnostic tripwire caught corrupt state.\n");

    return 0;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
