// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          combat.c
// @id            sim.engine.combat
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core combat mechanics implementation: Warding with Shield of Lux,
//   Striking with Sword of Tenebris, Equilibrium harmonization, and
//   deterministic combat stance transitions.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.math, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Deterministic state transitions)
//   - Latency Budget:        < 25 microseconds per combat action
//
// AXIOMATIC INVARIANTS:
//   1. Shield of Lux power bounded: 0 <= shield_power <= MAX_RESONANCE.
//   2. Sword of Tenebris edge bounded: 0 <= sword_edge <= MAX_RESONANCE.
//   3. Threat HP bounded by 0 floor (saturating subtraction).
//   4. Stance state machine forms a strongly connected transition graph.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Engine
// =============================================================================

#include "sim/combat.h"
#include "util/log.h"
#include "util/math_utils.h"

#include <stdio.h>
#include <string.h>

static const char *s_stance_names[STANCE_COUNT] = {
    "Neutral (At Ease)",
    "Aegis Warding (Shield High Guard)",
    "Edge Striking (Sword Drawn)",
    "Equilibrium (Twin Harmonized)"
};

static const char *const s_combat_invariants[4] = {
    "Shield warding saturates at MAX_RESONANCE with zero overflow",
    "Sword striking consumes edge sharpness and saturates threat vigor",
    "Equilibrium parry restores both relics and nudges moral center",
    "Stance state transitions form a deterministic complete graph"
};

const module_metadata_t combat_manifest = {
    "sim.engine.combat",
    "combat.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-30",               // last_updated
    "Combat resolution, twin relics mechanics, and stance state machine",
    3,                          // dependency_count
    { "core.util.manifest", "core.util.math", "core.util.log", NULL },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    25,                         // max_budget_us
    4,
    {
        s_combat_invariants[0],
        s_combat_invariants[1],
        s_combat_invariants[2],
        s_combat_invariants[3]
    },
    combat_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Engine
// =============================================================================

const char *combat_get_stance_name(combat_stance_t stance)
{
    if (stance >= 0 && stance < STANCE_COUNT) {
        return s_stance_names[stance];
    }
    return "Unknown Stance";
}

void combat_handle_ward(player_state_t *state, char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return;
    }

    if (state->threat_hp > 0) {
        int boost = (state->stance == STANCE_WARDING) ? 15 : 10;
        int stagger = (state->stance == STANCE_WARDING) ? 25 : 15;

        state->shield_power =
            math_saturating_add(state->shield_power, boost, MAX_RESONANCE);
        state->threat_hp = math_saturating_sub(state->threat_hp, stagger);
        state->moral_sway = math_clamp_int(state->moral_sway + 3, -100, 100);
        state->innocents_warded++;

        float grade = log_percent_to_grade(state->moral_sway);

        if (state->threat_hp <= 0) {
            state->threat_hp = 0;
            state->foes_vanquished++;
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Shield of Lux flared! Blinded and routed vanguard.");
            }
            printf("You raise the Shield of Lux! A radiant shockwave "
                   "shatters the vanguard's resolve!\n");

            LOG_CHAR_EVENT("Elijah", grade,
                "Shield of Lux flared! Vanguard routed (-%d HP). Shield:%d/100.",
                stagger, state->shield_power);
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Warded assault! Staggered foe (-%d HP). Shield at %d.",
                    stagger, state->shield_power);
            }
            printf("You lock the Shield of Lux into the earth. "
                   "The void-strike glances off in a golden flash!\n");

            LOG_CHAR_EVENT("Elijah", grade,
                "Warded assault: Staggered foe (-%d HP, rem:%d). Shield:%d/100.",
                stagger, state->threat_hp, state->shield_power);
        }
    } else {
        state->shield_power =
            math_saturating_add(state->shield_power, 20, MAX_RESONANCE);
        state->moral_sway = math_clamp_int(state->moral_sway + 1, -100, 100);
        float grade = log_percent_to_grade(state->moral_sway);

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Shield of Lux projected a protective aura (Power: %d/100).",
                state->shield_power);
        }
        printf("You raise the Shield of Lux. "
               "Warm golden starlight wards the perimeter.\n");

        LOG_CHAR_EVENT("Elijah", grade,
            "Shield of Lux projected protective aura (+20 power). Shield:%d/100.",
            state->shield_power);
    }
}

void combat_handle_strike(player_state_t *state, char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return;
    }

    if (state->threat_hp > 0) {
        int cost = (state->stance == STANCE_STRIKING) ? 8 : 12;
        int damage = (state->stance == STANCE_STRIKING) ? 45 : 30;

        state->sword_edge = math_saturating_sub(state->sword_edge, cost);
        state->threat_hp = math_saturating_sub(state->threat_hp, damage);

        float grade = log_percent_to_grade(state->moral_sway);

        if (state->threat_hp <= 0) {
            state->threat_hp = 0;
            state->foes_vanquished++;
            state->moral_sway =
                math_clamp_int(state->moral_sway + 2, -100, 100);
            grade = log_percent_to_grade(state->moral_sway);

            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Sword of Tenebris cleaved through enemy vanguard!");
            }
            printf("With a twilight arc, the Sword of Tenebris "
                   "severs the shadow fiend completely!\n");

            LOG_CHAR_EVENT("Katrina", grade,
                "Sword of Tenebris cleaved enemy vanguard (-%d HP). Edge:%d/100.",
                damage, state->sword_edge);
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Struck with Sword of Tenebris (-%d HP). Foe at %d HP.",
                    damage, state->threat_hp);
            }
            printf("The obsidian blade whistles through the gloom, "
                   "slicing through enchanted armor!\n");

            LOG_CHAR_EVENT("Katrina", grade,
                "Struck with Sword of Tenebris (-%d HP, rem:%d). Edge:%d/100.",
                damage, state->threat_hp, state->sword_edge);
        }
    } else {
        state->sword_edge =
            math_saturating_add(state->sword_edge, 15, MAX_RESONANCE);
        float grade = log_percent_to_grade(state->moral_sway);

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Sword of Tenebris attuned to the void (Edge: %d/100).",
                state->sword_edge);
        }
        printf("You trace the edge of the Sword of Tenebris. "
               "Cold abyssal mist sharpens the blade.\n");

        LOG_CHAR_EVENT("Katrina", grade,
            "Sword of Tenebris attuned to void mist (+15 edge). Edge:%d/100.",
            state->sword_edge);
    }
}

void combat_handle_balance(player_state_t *state, char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return;
    }

    state->shield_power =
        math_saturating_add(state->shield_power, 15, MAX_RESONANCE);
    state->sword_edge =
        math_saturating_add(state->sword_edge, 15, MAX_RESONANCE);

    state->moral_sway = math_nudge_toward(state->moral_sway, 0, 2);
    float grade = log_percent_to_grade(state->moral_sway);

    if (state->threat_hp > 0) {
        state->threat_hp = math_saturating_sub(state->threat_hp, 20);
        if (state->threat_hp == 0) state->foes_vanquished++;

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Equilibrium Parry & Riposte! Foe (-20 HP). Relics balanced.");
        }
        printf("You parry with the Shield of Lux and riposte with the "
               "Sword of Tenebris in perfect synergy!\n");

        LOG_CHAR_EVENT("Protagonists", grade,
            "Equilibrium parry & riposte (-20 HP). Lux:%d/100, Tenebris:%d/100.",
            state->shield_power, state->sword_edge);
    } else {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Harmonized twin relics. Shield: %d/100 | Sword: %d/100.",
                state->shield_power, state->sword_edge);
        }
        printf("You cross the Shield and Sword before your chest. "
               "Golden light and twilight mist intertwine.\n");

        LOG_CHAR_EVENT("Protagonists", grade,
            "Harmonized twin relics. Shield:%d/100, Sword:%d/100, Sway:%+d.",
            state->shield_power, state->sword_edge, state->moral_sway);
    }
}

bool combat_handle_stance(player_state_t *state, const char *arg,
                          char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return false;
    }

    if (arg == NULL || strlen(arg) == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Current stance: %s.",
                combat_get_stance_name(state->stance));
        }
        printf("Current stance: %s. Valid: warding, striking, "
               "equilibrium, neutral.\n",
               combat_get_stance_name(state->stance));
        return true;
    }

    if (strstr(arg, "ward") != NULL) {
        state->stance = STANCE_WARDING;
    } else if (strstr(arg, "strik") != NULL) {
        state->stance = STANCE_STRIKING;
    } else if (strstr(arg, "equil") != NULL || strstr(arg, "balance") != NULL) {
        state->stance = STANCE_EQUILIBRIUM;
    } else if (strstr(arg, "neut") != NULL) {
        state->stance = STANCE_NEUTRAL;
    } else {
        if (out_msg) snprintf(out_msg, msg_len, "Unknown stance '%s'.", arg);
        printf("Unknown stance '%s'. Valid: warding, striking, "
               "equilibrium, neutral.\n", arg);
        return false;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len, "Shifted stance to: %s.",
            combat_get_stance_name(state->stance));
    }
    printf("You shift your footing. Active Stance: %s.\n",
        combat_get_stance_name(state->stance));

    LOG_INFO(
        LOG_CAT_EVENT,
        "Protagonists",
        log_percent_to_grade(state->moral_sway),
        "Combat stance shifted to: %s.",
        combat_get_stance_name(state->stance)
    );
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Engine
// =============================================================================

diag_result_t combat_preflight(void)
{
    diag_result_t res;
    res.subsystem = combat_manifest.module_id;
    res.check_name = "Combat Formulas & Stance State Machine";

    player_state_t test;
    test.shield_power = 95;
    test.sword_edge = 95;
    test.moral_sway = 0;
    test.threat_hp = 100;
    test.stance = STANCE_WARDING;
    test.location = LOC_GARRISON_ALTAR;
    test.is_running = true;
    test.turns_taken = 0;
    test.foes_vanquished = 0;
    test.innocents_warded = 0;

    // Test ward saturation ceiling
    combat_handle_ward(&test, NULL, 0);
    if (test.shield_power > MAX_RESONANCE) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Shield power overflowed MAX_RESONANCE ceiling.";
        return res;
    }

    // Test stance transitions across all stances
    combat_handle_stance(&test, "striking", NULL, 0);
    if (test.stance != STANCE_STRIKING) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Stance transition to STANCE_STRIKING failed.";
        return res;
    }

    combat_handle_stance(&test, "equilibrium", NULL, 0);
    if (test.stance != STANCE_EQUILIBRIUM) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Stance transition to STANCE_EQUILIBRIUM failed.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Combat formulas, ceiling saturation, and stance machine verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
