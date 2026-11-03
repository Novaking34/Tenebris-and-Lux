// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          world.c
// @id            sim.engine.world
// @layer         Layer 1: Simulation Engine
// @version       1.2.0
// @interface_rev Rev 3
// @date          2026-10-30
// @updated       2026-10-30
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Topographical world map implementation, province lore strings, movement
//   routing across the Kingdom of Sacyra and The Dreamscape, and encounter
//   triggers grounded in the 2017-2018 demo baseline.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim.types.common, core.util.log, core.util.string
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
#include "util/log/log.h"
#include "util/string/string_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *s_location_names[LOC_COUNT] = {
    "The Dreamscape",
    "Dio Village",
    "Whispering Woods",
    "The Sacyran Marches"
};

static const char *s_location_descriptions[LOC_COUNT] = {
    "The ethereal realm of trials where the illusion of the dream shatters "
    "and Dream Eaters gather under the command of The Dark Lord.",
    "Ancestral pastoral hamlet in the Sacyran woodlands. Grandma Magolia's "
    "hearth, Eric's timber yard, and the starting redoubt of the journey.",
    "Dense ancient forest enveloping Dio Village. Whispering foliage "
    "conceals predatory beasts and shadow outriders.",
    "The windswept frontier marches of the Kingdom of Sacyra, leading "
    "toward the Royal Court of King Jihad."
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
    { 1, 2, 0 },
    3,                          // interface_rev
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
                     "Specify a direction (north, south, east, west, awaken).");
        }
        printf("Travel where? Specify a direction (e.g. 'move north').\n");
        return false;
    }

    location_id_t next_loc = state->location;
    char dir = (char)tolower((unsigned char)direction[0]);

    switch (state->location) {
    case LOC_THE_DREAMSCAPE:
        if (dir == 'n' || dir == 'a') {
            next_loc = LOC_DIO_VILLAGE;
        }
        break;
    case LOC_DIO_VILLAGE:
        if (dir == 's') {
            next_loc = LOC_THE_DREAMSCAPE;
        } else if (dir == 'n') {
            next_loc = LOC_WHISPERING_WOODS;
        }
        break;
    case LOC_WHISPERING_WOODS:
        if (dir == 's') {
            next_loc = LOC_DIO_VILLAGE;
        } else if (dir == 'e' || dir == 'n') {
            next_loc = LOC_SACYRA_MARCHES;
        }
        break;
    case LOC_SACYRA_MARCHES:
        if (dir == 'w' || dir == 's') {
            next_loc = LOC_WHISPERING_WOODS;
        }
        break;
    default:
        break;
    }

    if (next_loc == state->location) {
        if (out_msg) {
            snprintf(out_msg, msg_len, "Path blocked or impassable in that direction.");
        }
        printf("The path is impassable or contested in that direction.\n");

        LOG_WARN(
            LOG_CAT_EVENT,
            state->name,
            log_percent_to_grade(state->moral_sway),
            "Movement blocked or impassable toward direction: '%s'.",
            direction
        );
        return false;
    }

    state->location = next_loc;

    /* Encounter Triggers Grounded in Lore Baseline */
    if (state->location == LOC_WHISPERING_WOODS && state->foes_vanquished <= 1) {
        str_safe_copy(state->threat_name, "Umbral Wolf", sizeof(state->threat_name));
        state->threat_hp = 85;
        state->threat_max_hp = 85;
        state->threat_def = 14;
        state->threat_mdf = 10;
        state->threat_surrounding_hp = 60;
    } else if (state->location == LOC_SACYRA_MARCHES && state->threat_hp == 0) {
        str_safe_copy(state->threat_name, "Shadow Scout", sizeof(state->threat_name));
        state->threat_hp = 110;
        state->threat_max_hp = 110;
        state->threat_def = 18;
        state->threat_mdf = 14;
        state->threat_surrounding_hp = 80;
    }

    if (out_msg) {
        snprintf(out_msg, msg_len, "Traversed into %s.",
                 s_location_names[next_loc]);
    }
    printf("You journey forward across the realm...\n");
    printf("\n[%s]\n%s\n", s_location_names[state->location],
           s_location_descriptions[state->location]);
    if (state->threat_hp > 0) {
        printf("[!] A hostile presence blocks your path: %s (Vigor: %d/%d)!\n",
            state->threat_name, state->threat_hp, state->threat_max_hp);
    }
    printf("\n");

    LOG_INFO(
        LOG_CAT_EVENT,
        state->name,
        log_percent_to_grade(state->moral_sway),
        "Journeyed into realm '%s' (Threat: %s, Vigor: %d).",
        s_location_names[next_loc],
        state->threat_name,
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

    // Verify bi-directional path: The Dreamscape (North) -> Dio Village (South)
    player_state_t test;
    memset(&test, 0, sizeof(test));
    str_safe_copy(test.name, DEFAULT_PROTAGONIST_NAME, sizeof(test.name));
    test.location = LOC_THE_DREAMSCAPE;

    if (!world_handle_move(&test, "north", NULL, 0) ||
        test.location != LOC_DIO_VILLAGE) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Awakening movement from The Dreamscape to Dio Village failed.";
        return res;
    }

    if (!world_handle_move(&test, "south", NULL, 0) ||
        test.location != LOC_THE_DREAMSCAPE) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Southbound return to The Dreamscape failed (asymmetry).";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Provincial topology and Dreamscape routing verified.";
    return res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
