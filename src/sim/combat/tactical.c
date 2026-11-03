// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tactical.c
// @id            sim.combat.tactical
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
//   Tactical Grid-Based (TRPG) combat engine implementation. Resolves
//   isometric/orthogonal grid movements, Manhattan distance, flanking bonuses,
//   rear severance attacks, and movement point (MP) budgets.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/combat/tactical.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless geometry and arithmetic)
//   - Latency Budget:        < 10 microseconds per movement calculation
//
// AXIOMATIC INVARIANTS:
//   1. Distance: Strictly positive Manhattan distance.
//   2. Flank multipliers: 100% (Front), 120% (Flank), 140% (Rear).
//   3. Non-negative MP: Movement rejected if movement points < distance.
// =============================================================================

#include "sim/combat/tactical.h"
#include <stdlib.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Grid Engine
// =============================================================================

static const char *const s_tactical_invariants[3] = {
    "Distance calculation strictly adheres to positive Manhattan metric",
    "Positional multipliers enforce Front 100%, Flank 120%, Rear 140%",
    "Movement point budget decrements monotonically without negative underflow"
};

const module_metadata_t tactical_manifest = {
    "sim.combat.tactical",
    "tactical.h",
    LAYER_1_SIMULATION,
    { 1, 3, 0 },
    4,
    "2026-11-03",
    "Tactical grid-based TRPG positioning and flanking engine",
    0,
    { NULL, NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    10,
    3,
    {
        s_tactical_invariants[0],
        s_tactical_invariants[1],
        s_tactical_invariants[2]
    },
    tactical_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Grid Engine
// =============================================================================

int16_t tactical_grid_distance(grid_pos_t a, grid_pos_t b)
{
    int16_t dx = (int16_t)abs(a.x - b.x);
    int16_t dy = (int16_t)abs(a.y - b.y);
    return (int16_t)(dx + dy);
}

int16_t tactical_calculate_position_multiplier(grid_pos_t attacker,
                                               grid_pos_t defender,
                                               unit_facing_t facing)
{
    int16_t dx = (int16_t)(attacker.x - defender.x);
    int16_t dy = (int16_t)(attacker.y - defender.y);

    /* Determine relative angle/direction of incoming attack */
    /* North: dy < 0; South: dy > 0; East: dx > 0; West: dx < 0 */
    bool is_rear = false;
    bool is_flank = false;

    switch (facing) {
        case FACING_NORTH:
            if (dy > 0) is_rear = true;
            else if (dx != 0 && dy == 0) is_flank = true;
            break;
        case FACING_SOUTH:
            if (dy < 0) is_rear = true;
            else if (dx != 0 && dy == 0) is_flank = true;
            break;
        case FACING_EAST:
            if (dx < 0) is_rear = true;
            else if (dy != 0 && dx == 0) is_flank = true;
            break;
        case FACING_WEST:
            if (dx > 0) is_rear = true;
            else if (dy != 0 && dx == 0) is_flank = true;
            break;
        default:
            break;
    }

    if (is_rear) {
        return 140; /* +40% Rear strike bonus */
    } else if (is_flank) {
        return 120; /* +20% Flank strike bonus */
    }
    return 100;     /* Frontal strike: standard damage */
}

bool tactical_move_unit(tactical_unit_t *unit, grid_pos_t target_pos)
{
    if (!unit) {
        return false;
    }

    int16_t dist = tactical_grid_distance(unit->position, target_pos);
    if (dist == 0) {
        return true; /* Already at position */
    }

    if (unit->movement_points < dist) {
        return false; /* Insufficient MP */
    }

    unit->movement_points = (int8_t)(unit->movement_points - dist);
    unit->position = target_pos;
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Tactical Grid Engine
// =============================================================================

diag_result_t tactical_preflight(void)
{
    diag_result_t res;
    res.subsystem = tactical_manifest.module_id;
    res.check_name = "Tactical Grid Metric and Flank Multiplier Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Grid distance, flanking, and movement budget verified.";

    grid_pos_t a = { 2, 3 };
    grid_pos_t b = { 5, 7 };

    int16_t d = tactical_grid_distance(a, b);
    if (d != 7) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Manhattan distance calculation failed.";
        return res;
    }

    /* Test rear strike */
    grid_pos_t defender = { 5, 5 };
    grid_pos_t rear_attacker = { 5, 6 };
    int16_t mult = tactical_calculate_position_multiplier(rear_attacker,
                                                          defender,
                                                          FACING_NORTH);
    if (mult != 140) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Rear strike multiplier mismatch; expected 140.";
        return res;
    }

    /* Test movement budget */
    tactical_unit_t unit = { 1, { 0, 0 }, FACING_NORTH, 4, 3, false };
    grid_pos_t move_to = { 2, 1 }; /* Dist = 3 */
    if (!tactical_move_unit(&unit, move_to) || unit.movement_points != 1) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Tactical movement budget deduction failed.";
        return res;
    }

    return res;
}

bool tactical_audit(void)
{
    diag_result_t res = tactical_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
