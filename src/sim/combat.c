// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          combat.c
// @id            sim.engine.combat
// @layer         Layer 1: Simulation Engine
// @version       1.2.0
// @interface_rev Rev 3
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core combat mechanics implementation anchored in the 2017-2018 demo canon:
//   - Defend with Shield of Lux (Basic Skill: Lux Barrio barrier, 25% reduction).
//   - Attack with Sword of Tenebris (Basic Skill: Cuttatio Tenebris, 20% splash).
//   - Special abilities: Basic Magic, Basic Ward, Basic Charm (+20% MAT),
//     and Duo-Loca Bashin n Slash (adaptive Light/Dark targeting weakest defense).
//   - Deterministic 4-stance tactical state machine.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.math, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure deterministic state shifts)
//   - Latency Budget:        < 25 microseconds per combat action
//
// AXIOMATIC INVARIANTS:
//   1. Shield of Lux power bounded: 0 <= shield_power <= MAX_RESONANCE.
//   2. Sword of Tenebris edge bounded: 0 <= sword_edge <= MAX_RESONANCE.
//   3. Threat HP & surrounding HP bounded by 0 floor (saturating subtraction).
//   4. Stance state machine forms a strongly connected, deterministic graph.
//   5. Lux Barrio roll yields [2, 5] turns; Basic Charm roll yields [1, 3] turns.
//   6. Duo-Loca Bashin n Slash adaptively targets enemy's lowest defensive stat.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Combat Mechanics Engine
// =============================================================================

#include "sim/combat.h"
#include "util/log/log.h"
#include "util/math/math_utils.h"

#include <stdio.h>
#include <string.h>

static const char *s_stance_names[STANCE_COUNT] = {
    "Neutral (At Ease)",
    "Aegis Warding (Shield High Guard)",
    "Edge Striking (Sword Drawn)",
    "Equilibrium (Twin Harmonized)"
};

static const char *const s_combat_invariants[4] = {
    "Shield warding & Lux Barrio saturates at MAX_RESONANCE without overflow",
    "Sword striking & Cuttatio Tenebris applies 20% splash damage to flank",
    "Special Duo-Loca adaptively targets weakest defense and balances relics",
    "Stance state transitions form a deterministic complete graph"
};

const module_metadata_t combat_manifest = {
    "sim.engine.combat",
    "combat.h",
    LAYER_1_SIMULATION,
    { 1, 2, 0 },
    3,                          // interface_rev
    "2026-10-30",               // last_updated
    "Combat resolution, Lux Barrio, Cuttatio, Duo-Loca & stance machine",
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

void combat_handle_defend(player_state_t *state, bool use_lux_barrio,
                          char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return;
    }

    float grade = log_percent_to_grade(state->moral_sway);

    if (use_lux_barrio) {
        /* Roll turn duration deterministically within [2, 5] */
        int duration = 2 + (state->turns_taken % 4);
        state->lux_barrio_turns = duration;

        state->shield_power =
            math_saturating_add(state->shield_power, 20, MAX_RESONANCE);
        state->moral_sway = math_clamp_int(state->moral_sway + 3, -100, 100);
        state->innocents_warded++;

        grade = log_percent_to_grade(state->moral_sway);

        if (state->threat_hp > 0) {
            int stagger = (state->stance == STANCE_WARDING) ? 25 : 15;
            state->threat_hp = math_saturating_sub(state->threat_hp, stagger);

            if (state->threat_hp == 0) {
                state->foes_vanquished++;
                if (out_msg) {
                    snprintf(out_msg, msg_len,
                        "Lux Barrio flared! Radiant barrier routed %s (-%d HP)!",
                        state->threat_name, stagger);
                }
                printf("You cast Lux Barrio! A radiant wall of golden light "
                       "reflects the onslaught and shatters %s!\n",
                       state->threat_name);

                LOG_CHAR_EVENT(state->name, grade,
                    "Lux Barrio routed %s (-%d HP). 25%% mitigation for %d turns.",
                    state->threat_name, stagger, duration);
            } else {
                if (out_msg) {
                    snprintf(out_msg, msg_len,
                        "Lux Barrio erected! -25%% incoming damage for %d turns "
                        "(-%d HP stagger).",
                        duration, stagger);
                }
                printf("You invoke Lux Barrio! A glowing barrier absorbs the "
                       "impact, staggering %s (-%d HP) [Active: %d turns]!\n",
                       state->threat_name, stagger, duration);

                LOG_CHAR_EVENT(state->name, grade,
                    "Lux Barrio erected: %d turns (-25%% dmg), staggered %s (-%d HP).",
                    duration, state->threat_name, stagger);
            }
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Lux Barrio active! 25%% damage barrier erected for %d turns.",
                    duration);
            }
            printf("You invoke Lux Barrio. A luminous geometric shield forms "
                   "around you, providing 25%% damage protection for %d turns.\n",
                   duration);

            LOG_CHAR_EVENT(state->name, grade,
                "Lux Barrio activated: 25%% damage barrier for %d turns.",
                duration);
        }
    } else {
        /* Standard Defend */
        int boost = (state->stance == STANCE_WARDING) ? 15 : 10;
        state->shield_power =
            math_saturating_add(state->shield_power, boost, MAX_RESONANCE);
        state->moral_sway = math_clamp_int(state->moral_sway + 1, -100, 100);
        grade = log_percent_to_grade(state->moral_sway);

        if (state->threat_hp > 0) {
            int stagger = (state->stance == STANCE_WARDING) ? 15 : 10;
            state->threat_hp = math_saturating_sub(state->threat_hp, stagger);
            if (state->threat_hp == 0) state->foes_vanquished++;

            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Defended with Shield of Lux (-%d HP). Shield:%d/100.",
                    stagger, state->shield_power);
            }
            printf("You brace behind the Shield of Lux. The attack glances off "
                   "the golden rim (-%d HP stagger)!\n", stagger);

            LOG_CHAR_EVENT(state->name, grade,
                "Defended with Shield of Lux (-%d HP). Shield:%d/100.",
                stagger, state->shield_power);
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Guarded perimeter with Shield of Lux (Power: %d/100).",
                    state->shield_power);
            }
            printf("You raise the Shield of Lux in high guard (+%d power).\n",
                   boost);

            LOG_CHAR_EVENT(state->name, grade,
                "Guarded with Shield of Lux (+%d power). Shield:%d/100.",
                boost, state->shield_power);
        }
    }
}

void combat_handle_attack(player_state_t *state, bool use_cuttatio,
                          char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return;
    }

    float grade = log_percent_to_grade(state->moral_sway);

    if (state->threat_hp > 0) {
        /* Base primary damage from ATK and enemy DEF */
        int base_dmg = (state->atk * 2) - state->threat_def;
        if (state->stance == STANCE_STRIKING) {
            base_dmg += 15;
        }
        if (base_dmg < 10) base_dmg = 10;

        if (use_cuttatio) {
            /* Cuttatio Tenebris: Cleaves primary target & 20% splash */
            int cost = (state->stance == STANCE_STRIKING) ? 8 : 12;
            state->sword_edge = math_saturating_sub(state->sword_edge, cost);
            state->threat_hp = math_saturating_sub(state->threat_hp, base_dmg);

            int splash = (base_dmg * 20) / 100;
            if (splash < 1) splash = 1;
            state->threat_surrounding_hp =
                math_saturating_sub(state->threat_surrounding_hp, splash);

            if (state->threat_hp == 0) {
                state->foes_vanquished++;
                state->moral_sway =
                    math_clamp_int(state->moral_sway + 2, -100, 100);
                grade = log_percent_to_grade(state->moral_sway);

                if (out_msg) {
                    snprintf(out_msg, msg_len,
                        "Cuttatio Tenebris cleaved %s (-%d HP) & splashed -%d HP!",
                        state->threat_name, base_dmg, splash);
                }
                printf("You unleash Cuttatio Tenebris! A wide crescent of twilight "
                       "severs %s (-%d HP) and tears into flanking foes (-%d HP)!\n",
                       state->threat_name, base_dmg, splash);

                LOG_CHAR_EVENT(state->name, grade,
                    "Cuttatio Tenebris cleaved %s (-%d HP), splash (-%d HP). Edge:%d/100.",
                    state->threat_name, base_dmg, splash, state->sword_edge);
            } else {
                if (out_msg) {
                    snprintf(out_msg, msg_len,
                        "Cuttatio Tenebris struck %s (-%d HP, rem:%d) & splashed -%d HP.",
                        state->threat_name, base_dmg, state->threat_hp, splash);
                }
                printf("You slash with Cuttatio Tenebris! The abyssal arc slices "
                       "%s (-%d HP) and grazes surrounding foes (-%d HP)!\n",
                       state->threat_name, base_dmg, splash);

                LOG_CHAR_EVENT(state->name, grade,
                    "Cuttatio Tenebris struck %s (-%d HP, rem:%d), splash (-%d HP).",
                    state->threat_name, base_dmg, state->threat_hp, splash);
            }
        } else {
            /* Standard Attack */
            int cost = (state->stance == STANCE_STRIKING) ? 6 : 10;
            state->sword_edge = math_saturating_sub(state->sword_edge, cost);
            state->threat_hp = math_saturating_sub(state->threat_hp, base_dmg);

            if (state->threat_hp == 0) {
                state->foes_vanquished++;
                if (out_msg) {
                    snprintf(out_msg, msg_len,
                        "Sword of Tenebris defeated %s (-%d HP)!",
                        state->threat_name, base_dmg);
                }
                printf("With a swift thrust, the Sword of Tenebris fells %s!\n",
                       state->threat_name);

                LOG_CHAR_EVENT(state->name, grade,
                    "Attacked and defeated %s (-%d HP). Edge:%d/100.",
                    state->threat_name, base_dmg, state->sword_edge);
            } else {
                if (out_msg) {
                    snprintf(out_msg, msg_len,
                        "Attacked %s (-%d HP, rem:%d). Edge:%d/100.",
                        state->threat_name, base_dmg, state->threat_hp,
                        state->sword_edge);
                }
                printf("You strike %s with the Sword of Tenebris (-%d HP)!\n",
                       state->threat_name, base_dmg);

                LOG_CHAR_EVENT(state->name, grade,
                    "Attacked %s (-%d HP, rem:%d). Edge:%d/100.",
                    state->threat_name, base_dmg, state->threat_hp,
                    state->sword_edge);
            }
        }
    } else {
        state->sword_edge =
            math_saturating_add(state->sword_edge, 15, MAX_RESONANCE);
        grade = log_percent_to_grade(state->moral_sway);

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Sword of Tenebris attuned to twilight mist (Edge: %d/100).",
                state->sword_edge);
        }
        printf("You trace the edge of the Sword of Tenebris. Abyssal mist "
               "hones the blade (+15 edge).\n");

        LOG_CHAR_EVENT(state->name, grade,
            "Attuned Sword of Tenebris (+15 edge). Edge:%d/100.",
            state->sword_edge);
    }
}

void combat_handle_special(player_state_t *state, special_skill_t skill,
                           char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return;
    }

    float grade = log_percent_to_grade(state->moral_sway);

    switch (skill) {
    case SPECIAL_BASIC_MAGIC: {
        /* Neutral magic damage (MAT vs MDF) */
        int cost = 10;
        state->mp = math_saturating_sub(state->mp, cost);

        int eff_mat = (state->charm_turns > 0)
            ? (state->mat * 120 / 100)
            : state->mat;
        int mag_dmg = (eff_mat * 2) - state->threat_mdf;
        if (mag_dmg < 12) mag_dmg = 12;

        if (state->threat_hp > 0) {
            state->threat_hp = math_saturating_sub(state->threat_hp, mag_dmg);
            if (state->threat_hp == 0) state->foes_vanquished++;

            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Basic Magic cast! Neutral blast struck %s (-%d HP, MP:%d/%d).",
                    state->threat_name, mag_dmg, state->mp, state->max_mp);
            }
            printf("You cast Basic Magic! A neutral orb of pure essence "
                   "detonates against %s (-%d HP)!\n",
                   state->threat_name, mag_dmg);

            LOG_CHAR_EVENT(state->name, grade,
                "Basic Magic dealt %d magic damage to %s (MP:%d/%d).",
                mag_dmg, state->threat_name, state->mp, state->max_mp);
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Basic Magic sparkled harmlessly in the air (MP:%d/%d).",
                    state->mp, state->max_mp);
            }
            printf("You channel Basic Magic into the atmosphere (MP:%d/%d).\n",
                   state->mp, state->max_mp);

            LOG_CHAR_EVENT(state->name, grade,
                "Basic Magic channeled (MP:%d/%d).",
                state->mp, state->max_mp);
        }
        break;
    }

    case SPECIAL_BASIC_WARD: {
        /* Magic-based defend */
        int cost = 6;
        state->mp = math_saturating_sub(state->mp, cost);
        state->shield_power =
            math_saturating_add(state->shield_power, 15, MAX_RESONANCE);

        if (state->threat_hp > 0) {
            int stagger = 12;
            state->threat_hp = math_saturating_sub(state->threat_hp, stagger);
            if (state->threat_hp == 0) state->foes_vanquished++;

            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Basic Ward woven! Arcane barrier staggered %s (-%d HP).",
                    state->threat_name, stagger);
            }
            printf("You weave a Basic Ward! Shimmering runes deflect the enemy "
                   "advance and stagger %s (-%d HP)!\n",
                   state->threat_name, stagger);

            LOG_CHAR_EVENT(state->name, grade,
                "Basic Ward staggered %s (-%d HP). Shield:%d/100, MP:%d/%d.",
                state->threat_name, stagger, state->shield_power, state->mp,
                state->max_mp);
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Basic Ward woven! Arcane shield bolstered (+15 Shield: %d/100).",
                    state->shield_power);
            }
            printf("You weave a Basic Ward around your spirit (+15 Shield).\n");

            LOG_CHAR_EVENT(state->name, grade,
                "Basic Ward reinforced shield (+15 Shield). MP:%d/%d.",
                state->mp, state->max_mp);
        }
        break;
    }

    case SPECIAL_BASIC_CHARM: {
        /* Magic attack up 20% for 1-3 turns */
        int cost = 8;
        state->mp = math_saturating_sub(state->mp, cost);
        int duration = 1 + (state->turns_taken % 3);
        state->charm_turns = duration;

        if (out_msg) {
            snprintf(out_msg, msg_len,
                "Basic Charm active! Magic Attack increased +20%% for %d turns.",
                duration);
        }
        printf("You chant Basic Charm! Astral glyphs empower your mind: "
               "Magic Attack is boosted by 20%% for %d turns!\n", duration);

        LOG_CHAR_EVENT(state->name, grade,
            "Basic Charm activated: +20%% MAT for %d turns (MP:%d/%d).",
            duration, state->mp, state->max_mp);
        break;
    }

    case SPECIAL_DUO_LOCA:
    default: {
        /* Duo-Loca Bashin n Slash: Adaptive Light/Dark attack targeting lower defense */
        int cost = 12;
        state->mp = math_saturating_sub(state->mp, cost);

        /* Synchronize and balance twin relics */
        state->shield_power =
            math_saturating_add(state->shield_power, 15, MAX_RESONANCE);
        state->sword_edge =
            math_saturating_add(state->sword_edge, 15, MAX_RESONANCE);
        state->moral_sway = math_nudge_toward(state->moral_sway, 0, 3);
        grade = log_percent_to_grade(state->moral_sway);

        int eff_atk = state->atk;
        int eff_mat = (state->charm_turns > 0)
            ? (state->mat * 120 / 100)
            : state->mat;

        int phys_dmg = (eff_atk * 2) - state->threat_def;
        int mag_dmg = (eff_mat * 2) - state->threat_mdf;
        if (phys_dmg < 15) phys_dmg = 15;
        if (mag_dmg < 15) mag_dmg = 15;

        bool use_magic = (mag_dmg > phys_dmg) ||
                         (state->threat_mdf < state->threat_def) ||
                         (eff_mat > eff_atk);
        int chosen_dmg = use_magic ? mag_dmg : phys_dmg;
        const char *flavor = use_magic ? "Twilight Magic Slash" : "Radiant Light Bash";

        if (state->threat_hp > 0) {
            state->threat_hp = math_saturating_sub(state->threat_hp, chosen_dmg);
            if (state->threat_hp == 0) state->foes_vanquished++;

            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Duo-Loca Bashin n Slash! %s struck %s (-%d HP)! Relics balanced.",
                    flavor, state->threat_name, chosen_dmg);
            }
            printf("You unleash Duo-Loca Bashin n Slash! Combining the Shield of Lux "
                   "and Sword of Tenebris, you strike with %s targeting %s's "
                   "weakness (-%d HP)!\n",
                   flavor, state->threat_name, chosen_dmg);

            LOG_CHAR_EVENT(state->name, grade,
                "Duo-Loca Bashin n Slash (%s): -%d HP to %s. Relics balanced.",
                flavor, chosen_dmg, state->threat_name);
        } else {
            if (out_msg) {
                snprintf(out_msg, msg_len,
                    "Duo-Loca Bashin n Slash harmonized twin relics (+15 Lux, +15 Tenebris).");
            }
            printf("You cross the Shield of Lux and Sword of Tenebris in "
                   "Duo-Loca stance, harmonizing light and shadow.\n");

            LOG_CHAR_EVENT(state->name, grade,
                "Duo-Loca harmonized twin relics. Shield:%d/100, Sword:%d/100, Sway:%+d.",
                state->shield_power, state->sword_edge, state->moral_sway);
        }
        break;
    }
    }
}

void combat_handle_ward(player_state_t *state, char *out_msg, size_t msg_len)
{
    combat_handle_defend(state, true, out_msg, msg_len);
}

void combat_handle_strike(player_state_t *state, char *out_msg, size_t msg_len)
{
    combat_handle_attack(state, true, out_msg, msg_len);
}

void combat_handle_balance(player_state_t *state, char *out_msg, size_t msg_len)
{
    combat_handle_special(state, SPECIAL_DUO_LOCA, out_msg, msg_len);
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
        state->name,
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
    memset(&test, 0, sizeof(test));
    snprintf(test.name, sizeof(test.name), "%s", DEFAULT_PROTAGONIST_NAME);
    test.shield_power = 95;
    test.sword_edge = 95;
    test.moral_sway = 0;
    test.atk = INITIAL_BASE_ATK;
    test.def = INITIAL_BASE_DEF;
    test.mat = INITIAL_BASE_MAT;
    test.mdf = INITIAL_BASE_MDF;
    test.mp = INITIAL_MAX_MP;
    test.max_mp = INITIAL_MAX_MP;
    test.threat_hp = 100;
    test.threat_max_hp = 100;
    test.threat_def = 10;
    test.threat_mdf = 12;
    test.threat_surrounding_hp = 50;
    test.stance = STANCE_WARDING;
    test.location = LOC_THE_DREAMSCAPE;
    test.is_running = true;
    test.turns_taken = 0;
    test.foes_vanquished = 0;
    test.innocents_warded = 0;

    // 1. Test Lux Barrio turn roll and ceiling
    combat_handle_defend(&test, true, NULL, 0);
    if (test.shield_power > MAX_RESONANCE) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Shield power overflowed MAX_RESONANCE ceiling.";
        return res;
    }
    if (test.lux_barrio_turns < 2 || test.lux_barrio_turns > 5) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Lux Barrio duration outside valid range [2, 5].";
        return res;
    }

    // 2. Test Cuttatio Tenebris 20% splash damage
    int initial_flank = test.threat_surrounding_hp;
    combat_handle_attack(&test, true, NULL, 0);
    if (test.threat_surrounding_hp >= initial_flank) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Cuttatio Tenebris failed to apply splash damage.";
        return res;
    }

    // 3. Test Duo-Loca adaptive strike and relic balance
    test.shield_power = 50;
    test.sword_edge = 50;
    combat_handle_special(&test, SPECIAL_DUO_LOCA, NULL, 0);
    if (test.shield_power != 65 || test.sword_edge != 65) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Duo-Loca failed to balance both relics by +15.";
        return res;
    }

    // 4. Test stance transitions across all stances
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
    res.details = "Lux Barrio, Cuttatio, Duo-Loca and stance machine verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
