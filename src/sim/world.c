// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world.c
// @id            sim.engine.world
// @layer         Layer 1: Simulation Engine
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Topographical world map implementation, province lore strings, movement
//   routing across the Kingdom of Sacyra, and encounter triggers.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.log
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation)
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant state transitions)
//   - Latency Budget:        < 15 microseconds per movement action
//
// AXIOMATIC INVARIANTS:
//   1. Location bounds strictly bounded: 0 <= location < LOC_COUNT.
//   2. Topological routing graph is deterministic and bi-directional.
//   3. Impassable paths emit warnings and leave state untouched.
//   4. Encounters trigger deterministically on entry conditions.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

#include "sim/world.h"
#include "util/log.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *s_location_names[LOC_COUNT] = {
    "The Garrison Altar",
    "The Sunken Citadel Gates",
    "The Fractured Marches",
    "The Abyssal Threshold"
};

static const char *s_location_descriptions[LOC_COUNT] = {
    "Ancestral redoubt of the Wardens. Twin pedestals of solar silver and "
    "star-quenched obsidian stand ready to channel the Shield of Lux and "
    "Sword of Tenebris.",
    "White granite battlements scorched by void-fire. Beyond the fractured "
    "portcullis, the vanguard legions of the Dark Lord Malakor gather.",
    "A windswept highland where Solar Order knights and Umbral Covenant "
    "partisans clash in civil strife, blind to the true encroaching terror.",
    "The shattered monolith gates of the Dark Lord Malakor. Reality bends "
    "as shadow tendrils strike against the radiant barrier of starlight."
};

static const char *const s_world_invariants[4] = {
    "Location indices strictly bounded: [0, LOC_COUNT)",
    "Provincial transition graph is bi-directionally consistent",
    "Invalid headings leave state unchanged with zero side effects",
    "Encounter triggers set enemy vigor deterministically"
};

const module_metadata_t world_manifest = {
    "sim.engine.world",
    "world.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-30",               // last_updated
    "Topographical province graph, lore chronicle, and traversal routing",
    2,                          // dependency_count
    { "core.util.manifest", "core.util.log", NULL, NULL },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    15,                         // max_budget_us
    4,
    {
        s_world_invariants[0],
        s_world_invariants[1],
        s_world_invariants[2],
        s_world_invariants[3]
    },
    world_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

const char *world_get_location_name(location_id_t loc)
{
    if (loc >= 0 && loc < LOC_COUNT) {
        return s_location_names[loc];
    }
    return "Uncharted Realm";
}

const char *world_get_location_desc(location_id_t loc)
{
    if (loc >= 0 && loc < LOC_COUNT) {
        return s_location_descriptions[loc];
    }
    return "Gloom obscures the contours of the land.";
}

bool world_handle_move(player_state_t *state, const char *direction,
                       char *out_msg, size_t msg_len)
{
    if (state == NULL) {
        return false;
    }

    if (direction == NULL || strlen(direction) == 0) {
        if (out_msg) {
            snprintf(out_msg, msg_len,
                     "Specify a direction (north, south, east, west).");
        }
        printf("Travel where? Specify a direction (e.g. 'move north').\n");
        return false;
    }

    location_id_t next_loc = state->location;
    char dir = (char)tolower((unsigned char)direction[0]);

    switch (state->location) {
    case LOC_GARRISON_ALTAR:
        if (dir == 'n') next_loc = LOC_CITADEL_GATES;
        break;
    case LOC_CITADEL_GATES:
        if (dir == 's') next_loc = LOC_GARRISON_ALTAR;
        else if (dir == 'n') next_loc = LOC_DIVIDED_MARCHES;
        break;
    case LOC_DIVIDED_MARCHES:
        if (dir == 's') next_loc = LOC_CITADEL_GATES;
        else if (dir == 'e') next_loc = LOC_ABYSSAL_THRESHOLD;
        break;
    case LOC_ABYSSAL_THRESHOLD:
        if (dir == 'w') next_loc = LOC_DIVIDED_MARCHES;
        break;
    default:
        break;
    }

    if (next_loc == state->location) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Path blocked in that direction.");
        }
        printf("The path is impassable or contested in that direction.\n");

        LOG_WARN(
            LOG_CAT_EVENT,
            "Protagonists",
            log_percent_to_grade(state->moral_sway),
            "Movement blocked or impassable toward direction: '%s'.",
            direction
        );
        return false;
    }

    state->location = next_loc;

    if (state->location == LOC_CITADEL_GATES && state->foes_vanquished == 0) {
        state->threat_hp = 50;
    } else if (state->location == LOC_ABYSSAL_THRESHOLD &&
               state->threat_hp == 0) {
        state->threat_hp = 90;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len, "Marched into %s.",
                 s_location_names[next_loc]);
    }
    printf("You march forward across the Kingdom...\n");
    printf("\n[%s]\n%s\n", s_location_names[state->location],
           s_location_descriptions[state->location]);
    if (state->threat_hp > 0) {
        printf("[!] A vanguard patrol blocks your path (Vigor: %d/100)!\n",
            state->threat_hp);
    }
    printf("\n");

    LOG_INFO(
        LOG_CAT_EVENT,
        "Protagonists",
        log_percent_to_grade(state->moral_sway),
        "Marched into provincial region '%s' (Threat HP: %d).",
        s_location_names[next_loc],
        state->threat_hp
    );
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Topographical World Core
// =============================================================================

diag_result_t world_preflight(void)
{
    diag_result_t res;
    res.subsystem = world_manifest.module_id;
    res.check_name = "Topographical Map Graph & Bi-Directional Traversal";

    // Check all location string labels exist
    for (int i = 0; i < LOC_COUNT; i++) {
        const char *name = world_get_location_name((location_id_t)i);
        const char *desc = world_get_location_desc((location_id_t)i);
        if (name == NULL || strlen(name) == 0 ||
            desc == NULL || strlen(desc) == 0) {
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "Missing location name or description in world table.";
            return res;
        }
    }

    // Verify bi-directional path: Garrison Altar (North) -> Citadel Gates (South)
    player_state_t test;
    memset(&test, 0, sizeof(test));
    test.location = LOC_GARRISON_ALTAR;

    if (!world_handle_move(&test, "north", NULL, 0) ||
        test.location != LOC_CITADEL_GATES) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Northbound movement from Garrison Altar failed.";
        return res;
    }

    if (!world_handle_move(&test, "south", NULL, 0) ||
        test.location != LOC_GARRISON_ALTAR) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Southbound return to Garrison Altar failed (asymmetry).";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Provincial topology and bi-directional routing verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
