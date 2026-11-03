// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          sim_state.c
// @id            sim.engine.state
// @layer         Layer 1: Simulation Engine
// @subsystem     Simulation Core Subsystem (sim/core)
// @version       1.3.0
// @interface_rev Rev 4
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implementation of player state lifecycle, canonical reset defaults,
//   customizable protagonist name assignment, inventory initialization, and
//   state invariant audits. Anchored in the 2017-2018 demo baseline.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/core/sim_state.h
//       sim/items/inventory.h
//       sim/items/items.h
//       util/log/log.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions)
//   - Latency Budget:        < 10 microseconds per state audit
//
// AXIOMATIC INVARIANTS:
//   1. Canonical reset matches lore: The Dreamscape, Lux 80, Tenebris 80, Neutral.
//   2. Shield power invariant: [0, MAX_RESONANCE].
//   3. Sword edge invariant: [0, MAX_RESONANCE].
//   4. Moral sway continuum invariant: [-100, +100].
//   5. Resource pools (HP/MP/TP) bounded by non-negative values.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

#include "sim/core/sim_state.h"
#include "sim/items/inventory.h"
#include "sim/items/items.h"
#include "util/log/log.h"
#include "util/string/string_utils.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>

static const char *const s_state_invariants[4] = {
    "Shield of Lux resonance bounded: [0, MAX_RESONANCE]",
    "Sword of Tenebris edge bounded: [0, MAX_RESONANCE]",
    "Moral sway continuum bounded: [-100, +100]",
    "Location and stance enumerations within valid range"
};

const module_metadata_t sim_state_manifest = {
    "sim.engine.state",
    "sim_state.h",
    LAYER_1_SIMULATION,
    { 1, 3, 0 },
    4,                          // interface_rev
    "2026-11-03",               // last_updated
    "Player state lifecycle, canonical reset, inventory init, & audits",
    3,                          // dependency_count
    { "core.util.manifest", "core.util.log", "core.util.math", NULL },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    10,                         // max_budget_us
    4,
    {
        s_state_invariants[0],
        s_state_invariants[1],
        s_state_invariants[2],
        s_state_invariants[3]
    },
    sim_state_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

const char *sim_state_get_sway_desc(int sway)
{
    if (sway >= 40) return "Saint of Radiant Mercy (Virtue Unyielding)";
    if (sway >= 15) return "Protector of the Weak (Virtue)";
    if (sway > -15) return "Equilibrium (Cosmic Balance)";
    if (sway > -40) return "Harbinger of Ruin (Dominion)";
    return "Emissary of The Void (Ruthless Dominion)";
}

void sim_state_configure_protagonist(player_state_t *state,
                                     protagonist_id_t protagonist_id,
                                     const char *custom_name,
                                     sim_gender_t gender,
                                     class_id_t starting_class)
{
    if (state == NULL) return;

    state->protagonist = protagonist_id;
    state->gender = gender;

    /* Initialize zero-heap player inventory */
    inventory_init(&state->inventory);

    switch (protagonist_id) {
        case PROTAGONIST_ELIJAH:
            str_safe_copy(state->name, (custom_name != NULL && custom_name[0] != '\0')
                                       ? custom_name : "Elijah", sizeof(state->name));
            str_safe_copy(state->title, "The Steadfast Vanguard", sizeof(state->title));
            state->former_tutelage[0] = '\0';
            state->gender = GENDER_MALE;
            state->active_class = CLASS_WARRIOR;
            state->relic = RELIC_LUX;
            state->movement_points = 4;
            state->hp = INITIAL_MAX_HP;
            state->max_hp = INITIAL_MAX_HP;
            state->atk = INITIAL_BASE_ATK;
            state->def = INITIAL_BASE_DEF;
            state->mat = INITIAL_BASE_MAT;
            state->mdf = INITIAL_BASE_MDF;
            state->shield_power = INITIAL_SHIELD_POWER;
            state->sword_edge = INITIAL_SWORD_EDGE;
            state->moral_sway = INITIAL_MORAL_SWAY;

            /* Diegetic Starter Gear: Grandma's Watch, Sacyran Herbs, Gold */
            state->has_grandmas_watch = true;
            inventory_add(&state->inventory, NULL, 0, 5, 1);  /* Grandma's Watch */
            inventory_add(&state->inventory, NULL, 0, 1, 3);  /* Sacyran Herb x3 */
            inventory_add(&state->inventory, NULL, 0, 2, 2);  /* Phial of Light x2 */
            state->inventory.currency_gold = 100;
            state->gold = 100;
            break;

        case PROTAGONIST_KATRINA:
            str_safe_copy(state->name, (custom_name != NULL && custom_name[0] != '\0')
                                       ? custom_name : "Katrina", sizeof(state->name));
            str_safe_copy(state->title, "The Outrider Blade", sizeof(state->title));
            str_safe_copy(state->former_tutelage, "Court Mage of House Jaren", sizeof(state->former_tutelage));
            state->gender = GENDER_FEMALE;
            state->active_class = CLASS_ROGUE; // Rebelled against noble Mage tutelage!
            state->relic = RELIC_TENEBRIS;
            state->movement_points = 5;
            state->hp = INITIAL_MAX_HP;
            state->max_hp = INITIAL_MAX_HP;
            state->atk = INITIAL_BASE_ATK;
            state->def = INITIAL_BASE_DEF;
            state->mat = INITIAL_BASE_MAT;
            state->mdf = INITIAL_BASE_MDF;
            state->shield_power = INITIAL_SHIELD_POWER;
            state->sword_edge = INITIAL_SWORD_EDGE;
            state->moral_sway = INITIAL_MORAL_SWAY;

            inventory_add(&state->inventory, NULL, 0, 1, 3);  /* Sacyran Herb x3 */
            inventory_add(&state->inventory, NULL, 0, 4, 2);  /* Tenebrous Draught x2 */
            state->inventory.currency_gold = 150;
            state->gold = 150;
            break;

        case PROTAGONIST_OBSERVER:
        default:
            str_safe_copy(state->name, (custom_name != NULL && custom_name[0] != '\0')
                                       ? custom_name : "<Observer>", sizeof(state->name));
            str_safe_copy(state->title, "The Awakening Bearer", sizeof(state->title));
            state->former_tutelage[0] = '\0';
            state->gender = (gender != GENDER_UNSPECIFIED) ? gender : GENDER_NON_BINARY;
            state->relic = RELIC_EQUILIBRIUM;
            state->hp = INITIAL_MAX_HP;
            state->max_hp = INITIAL_MAX_HP;
            state->atk = INITIAL_BASE_ATK;
            state->def = INITIAL_BASE_DEF;
            state->mat = INITIAL_BASE_MAT;
            state->mdf = INITIAL_BASE_MDF;
            state->shield_power = INITIAL_SHIELD_POWER;
            state->sword_edge = INITIAL_SWORD_EDGE;
            state->moral_sway = INITIAL_MORAL_SWAY;

            inventory_add(&state->inventory, NULL, 0, 1, 2);  /* Sacyran Herb x2 */
            state->inventory.currency_gold = 50;
            state->gold = 50;

            if (starting_class == CLASS_MAGE) {
                state->active_class = CLASS_MAGE;
                state->movement_points = 4;
            } else if (starting_class == CLASS_ROGUE) {
                state->active_class = CLASS_ROGUE;
                state->movement_points = 5;
            } else {
                state->active_class = CLASS_WARRIOR;
                state->movement_points = 4;
            }
            break;
    }
}

void sim_state_set_name(player_state_t *state, const char *new_name)
{
    if (state == NULL) return;

    if (new_name != NULL && new_name[0] != '\0') {
        if (strcasecmp(new_name, "Katrina") == 0) {
            sim_state_configure_protagonist(state, PROTAGONIST_KATRINA, "Katrina",
                                            GENDER_FEMALE, CLASS_ROGUE);
        } else if (strcasecmp(new_name, "Elijah") == 0) {
            sim_state_configure_protagonist(state, PROTAGONIST_ELIJAH, "Elijah",
                                            GENDER_MALE, CLASS_WARRIOR);
        } else {
            sim_state_configure_protagonist(state, PROTAGONIST_OBSERVER, new_name,
                                            GENDER_NON_BINARY, CLASS_WARRIOR);
        }
        str_trim_inplace(state->name);
    } else {
        sim_state_configure_protagonist(state, PROTAGONIST_ELIJAH, DEFAULT_PROTAGONIST_NAME,
                                         GENDER_MALE, CLASS_WARRIOR);
    }

    LOG_INFO(
        LOG_CAT_LOG,
        "GameSim",
        log_percent_to_grade(state->moral_sway),
        "Protagonist designated as: %s (Class: %d, Relic: %d)",
        state->name,
        state->active_class,
        state->relic
    );
}

void sim_state_init(player_state_t *state)
{
    if (state == NULL) {
        return;
    }

    memset(state, 0, sizeof(*state));

    /* Configure starting protagonist: Elijah Ro-enheart (Warrior / Shield of Lux) */
    sim_state_configure_protagonist(state, PROTAGONIST_ELIJAH, DEFAULT_PROTAGONIST_NAME,
                                    GENDER_MALE, CLASS_WARRIOR);

    state->mp = INITIAL_MAX_MP;
    state->max_mp = INITIAL_MAX_MP;
    state->tp = INITIAL_MAX_TP;
    state->max_tp = INITIAL_MAX_TP;

    state->lux_barrio_turns = 0;
    state->charm_turns = 0;

    /* Authentic Demo Baseline: The opening encounter in The Dreamscape */
    str_safe_copy(state->threat_name, "Dream Eater", sizeof(state->threat_name));
    state->threat_hp = 70;
    state->threat_max_hp = 70;
    state->threat_def = 11;
    state->threat_mdf = 8;
    state->threat_surrounding_hp = 70;

    state->stance = STANCE_NEUTRAL;
    state->location = LOC_THE_DREAMSCAPE;
    state->is_running = true;
    state->turns_taken = 0;
    state->foes_vanquished = 0;
    state->innocents_warded = 0;

    LOG_INFO(
        LOG_CAT_LOG,
        "GameSim",
        log_percent_to_grade(state->moral_sway),
        "Simulation state initialized: %s (HP:%d, Lux:%d, Tenebris:%d, Realm:Dreamscape)",
        state->name,
        state->hp,
        state->shield_power,
        state->sword_edge
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

game_audit_report_t sim_state_audit(const player_state_t *state)
{
    game_audit_report_t report;
    report.is_valid = true;
    report.error_count = 0;
    report.error_log[0] = '\0';

    if (state == NULL) {
        report.is_valid = false;
        report.error_count = 1;
        snprintf(report.error_log, sizeof(report.error_log),
                 "[ERROR] Player state pointer is NULL.");
        return report;
    }

    if (state->shield_power < 0 || state->shield_power > MAX_RESONANCE) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Shield power (%d) outside bounds [0, %d]. ",
            state->shield_power, MAX_RESONANCE);
    }

    if (state->sword_edge < 0 || state->sword_edge > MAX_RESONANCE) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Sword edge (%d) outside bounds [0, %d]. ",
            state->sword_edge, MAX_RESONANCE);
    }

    if (state->moral_sway < -100 || state->moral_sway > 100) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Moral sway (%d) outside bounds [-100, 100]. ",
            state->moral_sway);
    }

    if (state->stance < 0 || state->stance >= STANCE_COUNT) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Stance enum (%d) outside valid range [0, %d). ",
            state->stance, STANCE_COUNT);
    }

    if (state->location < 0 || state->location >= LOC_COUNT) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Location enum (%d) outside valid range [0, %d). ",
            state->location, LOC_COUNT);
    }

    return report;
}

diag_result_t sim_state_preflight(void)
{
    diag_result_t res;
    memset(&res, 0, sizeof(res));

    player_state_t test_state;
    sim_state_init(&test_state);

    game_audit_report_t report = sim_state_audit(&test_state);
    if (!report.is_valid || report.error_count > 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Default canonical state failed invariant audit.";
        return res;
    }

    /* Test inventory initialization */
    if (test_state.inventory.slot_count == 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Starter inventory failed to populate slots.";
        return res;
    }

    /* Tripwire intentional corruption: shield_power over ceiling */
    test_state.shield_power = MAX_RESONANCE + 1;
    report = sim_state_audit(&test_state);
    if (report.is_valid) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Audit tripwire failed: Shield power overflow not caught.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "State initialization and invariant audit tripwires passed.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
