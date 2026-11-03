// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          dispatch_combat.c
// @id            sim.dispatch.combat
// @layer         Layer 1: Simulation Engine
// @subsystem     Command Dispatcher Subsystem (sim/dispatch)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Specialized domain dispatcher for combat actions. Delegates to the master
//   combat orchestrator (combat.c) across Turn-Based, Tactical Grid, and
//   Action Skirmish battle modes.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/dispatch/dispatch_combat.h,
//                            sim/combat/combat.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state evaluation).
//   - Latency Budget:        < 30 microseconds per command dispatch.
//
// AXIOMATIC INVARIANTS:
//   1. Only evaluates actions when state != NULL and ctx != NULL.
//   2. Successfully executed rounds advance state->turns_taken.
//   3. Stance transitions respect deterministic finite-state machine graph.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/dispatch/dispatch_combat.h"
#include "sim/combat/combat.h"
#include "sim/core/sim_state.h"
#include "util/string/string_utils.h"
#include <stdio.h>
#include <string.h>

const module_metadata_t dispatch_combat_manifest = {
    "sim.dispatch.combat",
    "dispatch_combat.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,
    "2026-11-03",
    "Combat domain command dispatcher and stance transition router",
    1,
    { "sim.engine.combat", NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    30,
    3,
    {
        "Evaluates only verbs assigned to DOMAIN_COMBAT",
        "Successfully executed combat rounds advance state->turns_taken",
        "Stance transitions respect deterministic finite-state machine graph"
    },
    dispatch_combat_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATIONS
// =============================================================================

bool dispatch_combat_handle(player_state_t *state,
                            const dispatch_context_t *ctx,
                            char *out_msg, size_t msg_len)
{
    if (!state || !ctx) {
        return false;
    }

    const char *verb = ctx->canonical_verb;
    const char *arg = ctx->arg;

    /* 1. Stance Shifts */
    if (str_case_eq(verb, "stance")) {
        combat_handle_stance(state, arg, out_msg, msg_len);
        return true;
    }

    /* 2. Combat Mode Shifts */
    if (str_case_eq(verb, "mode")) {
        if (!arg || strlen(arg) == 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Specify mode: turn, tact, or act.");
            }
            return true;
        }
        if (str_case_eq(arg, "turn") || str_case_eq(arg, "turn_based")) {
            combat_set_battle_mode(BATTLE_MODE_TURN_BASED);
            if (out_msg) {
                snprintf(out_msg, msg_len, "Battle mode set to Turn-Based.");
            }
        } else if (str_case_eq(arg, "tact") || str_case_eq(arg, "tactical")) {
            combat_set_battle_mode(BATTLE_MODE_TACTICAL);
            if (out_msg) {
                snprintf(out_msg, msg_len, "Battle mode set to Tactical Grid.");
            }
        } else if (str_case_eq(arg, "act") || str_case_eq(arg, "action")) {
            combat_set_battle_mode(BATTLE_MODE_ACTION);
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "Battle mode set to Action Skirmish.");
            }
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len, "Unknown combat mode '%s'.", arg);
            }
        }
        return true;
    }

    /* 3. Flee / Disengage */
    if (str_case_eq(verb, "flee")) {
        if (state->threat_hp <= 0) {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                         "There is no active threat to flee from.");
            }
            return true;
        }
        state->threat_hp = 0;
        state->threat_name[0] = '\0';
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "You fall back from the encounter into safety.");
        }
        return true;
    }

    /* 4. Attack / Strike / Cuttatio */
    if (str_case_eq(verb, "attack") || str_case_eq(verb, "cuttatio")) {
        bool use_cut = (str_case_eq(verb, "cuttatio") ||
                        (arg && str_case_eq(arg, "cuttatio")));
        combat_handle_attack(state, use_cut, out_msg, msg_len);
        state->turns_taken++;
        return true;
    }

    /* 5. Defend / Barrio */
    if (str_case_eq(verb, "defend") || str_case_eq(verb, "barrio")) {
        bool use_bar = (str_case_eq(verb, "barrio") ||
                        (arg && str_case_eq(arg, "barrio")));
        combat_handle_defend(state, use_bar, out_msg, msg_len);
        state->turns_taken++;
        return true;
    }

    /* 6. Special Skills (Magic, Ward, Charm, Duo) */
    if (str_case_eq(verb, "special")) {
        special_skill_t skill = SPECIAL_BASIC_MAGIC;
        if (arg) {
            if (str_case_eq(arg, "ward")) {
                skill = SPECIAL_BASIC_WARD;
            } else if (str_case_eq(arg, "charm")) {
                skill = SPECIAL_BASIC_CHARM;
            } else if (str_case_eq(arg, "duo") ||
                       str_case_eq(arg, "balance") ||
                       str_case_eq(arg, "bashin")) {
                skill = SPECIAL_DUO_LOCA;
            }
        }
        combat_handle_special(state, skill, out_msg, msg_len);
        state->turns_taken++;
        return true;
    }

    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t dispatch_combat_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.dispatch.combat",
        "dispatch_combat.c",
        "Combat domain dispatcher and verb mappings verified."
    };

    player_state_t test_state;
    memset(&test_state, 0, sizeof(test_state));
    sim_state_init(&test_state);

    dispatch_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    str_safe_copy(ctx.canonical_verb, "stance", sizeof(ctx.canonical_verb));
    str_safe_copy(ctx.arg, "striking", sizeof(ctx.arg));
    ctx.domain = DOMAIN_COMBAT;

    char msg[128];
    bool ok = dispatch_combat_handle(&test_state, &ctx, msg, sizeof(msg));
    if (!ok || test_state.stance != STANCE_STRIKING) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Combat dispatcher failed to process stance shift";
        return res;
    }

    return res;
}
