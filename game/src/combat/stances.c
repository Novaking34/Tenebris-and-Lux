// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          stances.c
// @id            sim.combat.stances
// @layer         Layer 1: Simulation Engine
// @subsystem     Combat Mechanics Subsystem (sim/combat)
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Deterministic 4-stance state machine engine implementation.
//   Manages transitions between Neutral, Aegis Warding, Edge Striking,
//   and Equilibrium stances with telemetry logging and narrative feedback.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/combat/stances.h
//       util/log/log.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure deterministic state shifts)
//
// AXIOMATIC INVARIANTS:
//   1. Determinism: Stance graph forms a strongly connected transition graph.
//   2. Bound limits: Stance enum strictly in [0, STANCE_COUNT).
//   3. Zero heap: No dynamic allocations during transitions.
// =============================================================================

#include "combat/stances.h"
#include "log/log.h"
#include "core/string_utils.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Stance State Machine
// =============================================================================

static const char *const s_stances_invariants[2] = {
    "Stance state transitions form a deterministic complete graph",
    "Stance indices bounded strictly by [0, STANCE_COUNT)"
};

const module_metadata_t stances_manifest = {
    "sim.combat.stances",
    "stances.h",
    LAYER_1_SIMULATION,
    { 1, 3, 0 },
    4,
    "2026-11-03",
    "Deterministic 4-stance state machine engine",
    2,
    { "core.util.log", "core.util.string", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    20,
    2,
    { s_stances_invariants[0], s_stances_invariants[1] },
    stances_preflight
};

static stance_info_t s_stances[COMBAT_MAX_STANCES] = {
    { 0, "neutral", "Neutral (At Ease)", "Balanced recovery.",
      0, 0, 5, false },
    { 1, "warding", "Aegis Warding (Shield High Guard)",
      "Kinetic ward active.", 40, -10, 5, false },
    { 2, "striking", "Edge Striking (Sword Drawn)",
      "Dark severance primed.", -10, 30, 5, false },
    { 3, "equilibrium", "Equilibrium (Twin Harmonized)",
      "Twin relics harmonized.", 20, 15, 0, true }
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC
// Stratum: Layer 1 (Simulation Engine) | Machine: Stance State Machine
// =============================================================================

void stances_init(const stance_info_t *stances, size_t count)
{
    if (!stances || count == 0) {
        return;
    }
    size_t copy_count = (count > COMBAT_MAX_STANCES) ? COMBAT_MAX_STANCES : count;
    for (size_t i = 0; i < copy_count; i++) {
        s_stances[i] = stances[i];
    }
}

const char *stances_get_name(combat_stance_t stance)
{
    if (stance >= 0 && stance < STANCE_COUNT) {
        return s_stances[stance].name;
    }
    return "Unknown Stance";
}

const stance_info_t *stances_get_info(combat_stance_t stance)
{
    if (stance >= 0 && stance < STANCE_COUNT) {
        return &s_stances[stance];
    }
    return &s_stances[0];
}

bool stances_can_transition(combat_stance_t from, combat_stance_t to)
{
    if (from < 0 || from >= STANCE_COUNT || to < 0 || to >= STANCE_COUNT) {
        return false;
    }
    /* All stances form a complete strongly connected graph */
    return true;
}

bool stances_apply_transition(player_state_t *state,
                              const char *token,
                              char *out_msg,
                              size_t msg_len)
{
    if (!state || !token) {
        return false;
    }

    float grade = log_percent_to_grade(state->moral_sway);

    if (strcmp(token, "neutral") == 0 || strcmp(token, "0") == 0) {
        state->stance = STANCE_NEUTRAL;
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Stance shifted: Neutral (At Ease). Balanced recovery.");
        }
        printf("You relax your guard into Neutral stance. Breath steady.\n");
        LOG_CHAR_EVENT(state->name, grade, "Stance shift -> Neutral.");
        return true;
    } else if (strcmp(token, "warding") == 0 || strcmp(token, "1") == 0) {
        state->stance = STANCE_WARDING;
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Stance shifted: Aegis Warding. Shield raised high.");
        }
        printf("You raise the Shield of Lux high into Aegis Warding stance!\n");
        LOG_CHAR_EVENT(state->name, grade, "Stance shift -> Aegis Warding.");
        return true;
    } else if (strcmp(token, "striking") == 0 || strcmp(token, "2") == 0) {
        state->stance = STANCE_STRIKING;
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Stance shifted: Edge Striking. Sword drawn.");
        }
        printf("You lower your center into Edge Striking stance. "
               "The Sword of Tenebris glints.\n");
        LOG_CHAR_EVENT(state->name, grade, "Stance shift -> Edge Striking.");
        return true;
    } else if (strcmp(token, "equilibrium") == 0 || strcmp(token, "3") == 0) {
        state->stance = STANCE_EQUILIBRIUM;
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Stance shifted: Equilibrium. Twin relics harmonized.");
        }
        printf("You align Shield and Sword in Equilibrium stance. "
               "Gold and shadow entwine!\n");
        LOG_CHAR_EVENT(state->name, grade, "Stance shift -> Equilibrium.");
        return true;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len,
                 "Unknown stance '%s'. Options: "
                 "neutral, warding, striking, equilibrium.", token);
    }
    printf("Unknown stance '%s'. Valid: "
           "neutral, warding, striking, equilibrium.\n", token);
    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Stance State Machine
// =============================================================================

diag_result_t stances_preflight(void)
{
    diag_result_t res;
    res.subsystem = stances_manifest.module_id;
    res.check_name = "Stance State Machine Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Stance names and transition graph verified.";

    /* Test 1: Stance names matching exact invariant requirement */
    const char *w_name = stances_get_name(STANCE_WARDING);
    if (strcmp(w_name, "Aegis Warding (Shield High Guard)") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Aegis Warding name invariant failed.";
        return res;
    }

    /* Test 2: Transition graph connectivity */
    for (int i = 0; i < STANCE_COUNT; i++) {
        for (int j = 0; j < STANCE_COUNT; j++) {
            if (!stances_can_transition((combat_stance_t)i, (combat_stance_t)j)) {
                res.passed = false;
                res.severity = SEVERITY_FATAL;
                res.details = "Stance transition graph is disconnected.";
                return res;
            }
        }
    }

    return res;
}

bool stances_audit(void)
{
    diag_result_t res = stances_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
