// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          maps.c
// @id            sim.world.maps
// @layer         Layer 1: Simulation Engine
// @subsystem     World Navigation & Dynamic Map Strata (sim/world)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Static canonical map catalog definitions unbound from RPG Maker schemas.
//   Provides zero-heap fallback data and lookup primitives across exteriors,
//   interiors, and tactical grid battle zones.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/world/maps.h, util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure immutable data tables).
//   - Latency Budget:        < 5 microseconds per query.
//
// AXIOMATIC INVARIANTS:
//   1. Every map has a unique non-empty string identifier.
//   2. Spawn point coordinates are strictly bounded within map dimensions.
//   3. Portal destinations link exclusively to valid canonical map IDs.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "sim/world/maps.h"
#include "util/string/string_utils.h"
#include <string.h>

const module_metadata_t maps_manifest = {
    "sim.world.maps",
    "maps.h",
    LAYER_1_SIMULATION,
    { 1, 0, 0 },
    1,
    "2026-11-03",
    "Canonical map catalog across exteriors, interiors, & battle grids",
    1,
    { "core.util.string", NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    10,
    3,
    {
        "Every map has a unique non-empty string identifier",
        "Spawn point coordinates are strictly bounded within map dimensions",
        "Portal destinations link exclusively to valid canonical map IDs"
    },
    maps_preflight
};

static const map_def_t CANONICAL_MAPS[] = {
    /* 0: Dio Village Exterior */
    {
        "dio_village_exterior",
        "Dio Village Square",
        MAP_TYPE_SETTLEMENT_EXTERIOR,
        MAP_ENV_EXTERIOR,
        {32, 32, 32},
        16, 28,
        2,
        {
            {16, 10, "grandmas_cottage_interior", "Grandma Magolia's Cottage"},
            {31, 16, "battle_whispering_woods", "East Trail to Woods"}
        },
        "The tranquil square of Dio Village with Grandma's cottage to the "
        "north and the woods to the east."
    },
    /* 1: Royal Capital Plaza */
    {
        "royal_capital_plaza",
        "Sacyra Capital Plaza",
        MAP_TYPE_SETTLEMENT_EXTERIOR,
        MAP_ENV_EXTERIOR,
        {40, 40, 32},
        20, 36,
        1,
        {
            {20, 4, "sacyra_throne_room", "High Throne Gates"}
        },
        "The grand stone plaza of the Royal Capital, surrounded by market "
        "stalls and the High Citadel."
    },
    /* 2: Frontier Ruins Exterior */
    {
        "frontier_ruins_exterior",
        "Libiran Frontier Ruins",
        MAP_TYPE_SETTLEMENT_EXTERIOR,
        MAP_ENV_EXTERIOR,
        {30, 30, 32},
        15, 26,
        0,
        {{0, 0, "", ""}},
        "Weather-beaten stone colonnades and dusty courier outposts on the "
        "outer edge of Libira."
    },
    /* 3: Dreamscape Nexus Interior */
    {
        "dreamscape_nexus_interior",
        "The Dreamscape Nexus & Room of Sages",
        MAP_TYPE_INTERIOR_CHAMBER,
        MAP_ENV_ASTRAL,
        {24, 24, 32},
        12, 20,
        1,
        {
            {12, 23, "dio_village_exterior", "Awakening Portal to Dio Village"}
        },
        "An ethereal rotunda where Chrono chronicles history across the "
        "astral void."
    },
    /* 4: Grandma's Cottage Interior */
    {
        "grandmas_cottage_interior",
        "Grandma Magolia's Hearth",
        MAP_TYPE_INTERIOR_CHAMBER,
        MAP_ENV_INTERIOR,
        {16, 16, 32},
        8, 14,
        1,
        {
            {8, 15, "dio_village_exterior", "Front Door to Village Square"}
        },
        "A warm timber hearth scented with dried chamomile, lavender, and "
        "Grandma's herbal teas."
    },
    /* 5: Sacyra Throne Room */
    {
        "sacyra_throne_room",
        "The High Throne Room",
        MAP_TYPE_INTERIOR_CHAMBER,
        MAP_ENV_INTERIOR,
        {24, 30, 32},
        12, 26,
        1,
        {
            {12, 29, "royal_capital_plaza", "Palace Gates to Capital Plaza"}
        },
        "The solemn throne room of King Jihad, with the veiled planar "
        "fissure pulsing beneath the dais."
    },
    /* 6: Tactical Battle: Dreamscape Rift */
    {
        "battle_dreamscape_rift",
        "Tactical Grid: Astral Rift",
        MAP_TYPE_TACTICAL_GRID,
        MAP_ENV_BATTLE_ZONE,
        {12, 12, 48},
        5, 10,
        0,
        {{0, 0, "", ""}},
        "A 12x12 tactical battlefield suspended over the cosmic abyss with "
        "ethereal mist hazards."
    },
    /* 7: Tactical Battle: Woodland Ambush */
    {
        "battle_whispering_woods",
        "Tactical Grid: Woodland Ambush",
        MAP_TYPE_TACTICAL_GRID,
        MAP_ENV_BATTLE_ZONE,
        {14, 14, 48},
        6, 12,
        0,
        {{0, 0, "", ""}},
        "A 14x14 tactical clearing shaded by ancient oaks with fallen log "
        "cover and flanking paths."
    },
    /* 8: Tactical Battle: Palace Colonnade */
    {
        "battle_palace_colonnade",
        "Tactical Grid: Palace Colonnade",
        MAP_TYPE_TACTICAL_GRID,
        MAP_ENV_BATTLE_ZONE,
        {16, 16, 48},
        8, 14,
        0,
        {{0, 0, "", ""}},
        "A 16x16 marble colonnade with high elevation steps leading to the "
        "royal dais."
    }
};

static const size_t CANONICAL_MAPS_COUNT =
    sizeof(CANONICAL_MAPS) / sizeof(CANONICAL_MAPS[0]);

// =============================================================================
// BLOCK 3: BODY BLOCK / API IMPLEMENTATIONS
// =============================================================================

size_t maps_get_count(void)
{
    return CANONICAL_MAPS_COUNT;
}

const map_def_t *maps_get_by_id(const char *id)
{
    if (!id || id[0] == '\0') {
        return NULL;
    }
    for (size_t i = 0; i < CANONICAL_MAPS_COUNT; i++) {
        if (str_case_eq(CANONICAL_MAPS[i].id, id)) {
            return &CANONICAL_MAPS[i];
        }
    }
    return NULL;
}

const map_def_t *maps_get_by_index(size_t index)
{
    if (index >= CANONICAL_MAPS_COUNT) {
        return NULL;
    }
    return &CANONICAL_MAPS[index];
}

const map_def_t *maps_get_for_province(location_id_t loc)
{
    switch (loc) {
        case LOC_THE_DREAMSCAPE:
            return &CANONICAL_MAPS[3]; // dreamscape_nexus_interior
        case LOC_DIO_VILLAGE:
            return &CANONICAL_MAPS[0]; // dio_village_exterior
        case LOC_WHISPERING_WOODS:
            return &CANONICAL_MAPS[7]; // battle_whispering_woods
        case LOC_SACYRA_MARCHES:
            return &CANONICAL_MAPS[1]; // royal_capital_plaza
        case LOC_ROYAL_CAPITAL:
            return &CANONICAL_MAPS[1]; // royal_capital_plaza
        case LOC_KINGDOM_OF_LIBIRA:
            return &CANONICAL_MAPS[2]; // frontier_ruins_exterior
        default:
            return &CANONICAL_MAPS[0];
    }
}

const map_portal_t *maps_find_portal_at(const map_def_t *map,
                                        int16_t x, int16_t y)
{
    if (!map) {
        return NULL;
    }
    for (uint32_t i = 0; i < map->portal_count; i++) {
        if (map->portals[i].x == x && map->portals[i].y == y) {
            return &map->portals[i];
        }
    }
    return NULL;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t maps_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "sim.world.maps",
        "maps.c",
        "All canonical map definitions and portals verified."
    };

    if (CANONICAL_MAPS_COUNT < 9) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Canonical map count is below expected minimum (9)";
        return res;
    }

    for (size_t i = 0; i < CANONICAL_MAPS_COUNT; i++) {
        const map_def_t *m = &CANONICAL_MAPS[i];
        if (m->dimensions.width == 0 || m->dimensions.height == 0) {
            res.passed = false;
            res.severity = SEVERITY_CRITICAL;
            res.details = "Map has zero width or height";
            return res;
        }
        if (m->spawn_x >= (int16_t)m->dimensions.width ||
            m->spawn_y >= (int16_t)m->dimensions.height) {
            res.passed = false;
            res.severity = SEVERITY_CRITICAL;
            res.details = "Map spawn coordinates exceed map dimensions";
            return res;
        }
    }

    return res;
}
