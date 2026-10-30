// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          sim_state.c
// @id            sim.engine.state
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Implementation of player state lifecycle, canonical reset defaults,
//   moral sway alignment continuum descriptions, and state invariant audits.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.log, core.util.string
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions)
//   - Latency Budget:        < 10 microseconds per state audit
//
// AXIOMATIC INVARIANTS:
//   1. Canonical reset matches lore: 80 Lux, 80 Tenebris, 0 Sway, Neutral.
//   2. Shield power invariant: [0, MAX_RESONANCE].
//   3. Sword edge invariant: [0, MAX_RESONANCE].
//   4. Moral sway continuum invariant: [-100, +100].
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

#include "sim/sim_state.h"
#include "util/log.h"
#include "util/math_utils.h"
#include "util/string_utils.h"

#include <stdio.h>
#include <string.h>

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
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-30",               // last_updated
    "Player state lifecycle, canonical reset, and invariant audits",
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

/**
 * Returns narrative description of moral sway alignment.
 */
const char *sim_state_get_sway_desc(int sway)
{
    if (sway >= 40) return "Saint of Radiant Mercy (Virtue Unyielding)";
    if (sway >= 15) return "Protector of the Weak (Virtue)";
    if (sway > -15) return "Equilibrium (Cosmic Balance)";
    if (sway > -40) return "Harbinger of Ruin (Dominion)";
    return "Lord of the Abyssal Void (Ruthless Dominion)";
}

/**
 * Initializes player state with canonical starting values.
 */
void sim_state_init(player_state_t *state)
{
    if (state == NULL) {
        return;
    }

    state->shield_power = INITIAL_SHIELD_POWER;
    state->sword_edge = INITIAL_SWORD_EDGE;
    state->moral_sway = INITIAL_MORAL_SWAY;
    state->threat_hp = 0;
    state->stance = STANCE_NEUTRAL;
    state->location = LOC_GARRISON_ALTAR;
    state->is_running = true;
    state->turns_taken = 0;
    state->foes_vanquished = 0;
    state->innocents_warded = 0;

    LOG_INFO(
        LOG_CAT_LOG,
        "GameSim",
        log_percent_to_grade(state->moral_sway),
        "Simulation state initialized (Shield:%d, Sword:%d, Sway:%+d)",
        state->shield_power,
        state->sword_edge,
        state->moral_sway
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: State Lifecycle Core
// =============================================================================

/**
 * Validates all state invariants on a given player state struct.
 */
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
            "[ERROR] Moral sway (%d) outside bounds [-100, +100]. ",
            state->moral_sway);
    }

    if (state->stance < 0 || state->stance >= STANCE_COUNT) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Stance (%d) outside valid range [0, %d). ",
            state->stance, STANCE_COUNT);
    }

    if (state->location < 0 || state->location >= LOC_COUNT) {
        report.is_valid = false;
        report.error_count++;
        snprintf(report.error_log + strlen(report.error_log),
            sizeof(report.error_log) - strlen(report.error_log),
            "[ERROR] Location (%d) outside valid bounds [0, %d). ",
            state->location, LOC_COUNT);
    }

    return report;
}

/**
 * Pre-flight diagnostic callback verifying initial state invariants.
 */
diag_result_t sim_state_preflight(void)
{
    diag_result_t res;
    res.subsystem = sim_state_manifest.module_id;
    res.check_name = "Canonical State Bounds & Invariants";

    player_state_t test_state;
    sim_state_init(&test_state);

    game_audit_report_t report = sim_state_audit(&test_state);
    if (!report.is_valid) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Canonical state failed invariant bounds check.";
        return res;
    }

    // Verify corruption detection tripwire
    test_state.shield_power = 999;
    game_audit_report_t trap_report = sim_state_audit(&test_state);
    if (trap_report.is_valid || trap_report.error_count == 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "State corruption trap failed to catch invalid resonance.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "State bounds, moral sway, and tripwires verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
