// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          items.c
// @id            sim.items.orchestrator
// @layer         Layer 1: Simulation Engine
// @subsystem     Item, Relic & Inventory Subsystem (sim/items)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Master Orchestrator implementation for the Item, Relic & Inventory Subsystem.
//   Coordinates data loading, catalog lookup, player inventory management,
//   diegetic relic queries, tactical catalyst calculations, and consumable
//   applications without tight coupling or hardcoded data tables.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/items/items.h
//       sim/core/sim_types.h
//       util/string/string_utils.h
//       util/log/log.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless catalog queries).
//   - Latency Budget:        < 50 microseconds per orchestrated action.
//
// AXIOMATIC INVARIANTS:
//   1. Data-driven: Zero items hardcoded; catalog loaded from data/items files.
//   2. Orchestrator delegation: Type-specific logic delegated to sub-modules.
//   3. Invariant audit integrity: Subsystem pre-flight validates all sub-modules.
// =============================================================================

#define _POSIX_C_SOURCE 200809L

#include "sim/items/items.h"
#include "sim/core/sim_types.h"
#include "util/log/log.h"
#include "util/string/string_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Orchestrator
// =============================================================================

static const char *const s_items_invariants[3] = {
    "Data-driven: Zero items hardcoded; catalog loaded from data/items files",
    "Orchestrator delegation: Type-specific logic delegated to sub-modules",
    "Invariant audit integrity: Subsystem pre-flight validates all sub-modules"
};

const module_metadata_t items_manifest = {
    "sim.items.orchestrator",
    "items.h",
    LAYER_1_SIMULATION,
    { 1, 1, 0 },
    2,
    "2026-11-03",
    "Item and inventory domain orchestrator",
    4,
    {
        "sim.items.loader",
        "sim.items.relics",
        "sim.items.catalysts",
        "sim.items.inventory"
    },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    50,
    3,
    {
        s_items_invariants[0],
        s_items_invariants[1],
        s_items_invariants[2]
    },
    items_preflight
};

/** Runtime item definitions catalog pool. */
static item_def_t s_catalog[ITEM_CATALOG_MAX_ENTRIES];
static size_t s_catalog_count = 0;
static bool s_initialized = false;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Orchestrator
// =============================================================================

bool items_init(void)
{
    memset(s_catalog, 0, sizeof(s_catalog));
    s_catalog_count = 0;

    bool ok = item_loader_load_catalog(s_catalog, ITEM_CATALOG_MAX_ENTRIES,
                                       &s_catalog_count);
    s_initialized = ok;
    return ok;
}

const item_def_t *items_get_catalog(size_t *out_count)
{
    if (!s_initialized || s_catalog_count == 0) {
        items_init();
    }
    if (out_count) {
        *out_count = s_catalog_count;
    }
    return s_catalog;
}

const item_def_t *items_find_def(uint32_t item_id)
{
    if (!s_initialized || s_catalog_count == 0) {
        items_init();
    }

    for (size_t i = 0; i < s_catalog_count; i++) {
        if (s_catalog[i].id == item_id) {
            return &s_catalog[i];
        }
    }
    return NULL;
}


const item_def_t *items_find_by_name(const char *name)
{
    if (!name || name[0] == '\0') {
        return NULL;
    }

    if (!s_initialized || s_catalog_count == 0) {
        items_init();
    }

    for (size_t i = 0; i < s_catalog_count; i++) {
        if (str_case_eq(s_catalog[i].name, name) ||
            str_case_eq(s_catalog[i].code, name)) {
            return &s_catalog[i];
        }
    }
    return NULL;
}

bool items_use(player_inventory_t *inv,
               uint32_t item_id,
               void *player_state,
               char *out_msg,
               size_t msg_len)
{
    if (!inv || !out_msg || msg_len == 0) {
        return false;
    }

    const item_def_t *def = items_find_def(item_id);
    if (!def) {
        snprintf(out_msg, msg_len, "[ITEM-ERROR] Item ID %u does not exist.",
                 (unsigned)item_id);
        return false;
    }

    if (!inventory_has_item(inv, item_id)) {
        snprintf(out_msg, msg_len,
                 "[INVENTORY] You do not possess '%s' (ID:%u).",
                 def->name, (unsigned)item_id);
        return false;
    }

    player_state_t *ps = (player_state_t *)player_state;

    switch (def->type) {
        case ITEM_TYPE_CONSUMABLE: {
            if (!ps) {
                snprintf(out_msg, msg_len,
                         "[ITEM-ERROR] Cannot consume item without player context.");
                return false;
            }
            int32_t hp = ps->hp;
            int32_t max_hp = ps->max_hp;
            int32_t shield = ps->shield_power;
            int32_t sword = ps->sword_edge;

            bool success = consumables_apply_effect(def, &hp, max_hp,
                                                    &shield, &sword,
                                                    out_msg, msg_len);
            if (success) {
                ps->hp = (int)hp;
                ps->shield_power = (int)shield;
                ps->sword_edge = (int)sword;
                inventory_remove(inv, item_id, 1);
            }
            return success;
        }

        case ITEM_TYPE_RELIC:
            /* Diegetic relics are consulted, not depleted */
            return relics_inspect(def, out_msg, msg_len);

        case ITEM_TYPE_CATALYST:
            return catalysts_inspect(def, out_msg, msg_len);

        case ITEM_TYPE_VALUABLE:
            snprintf(out_msg, msg_len,
                     "[%s] Valuable Token (Worth %u Gold). "
                     "Cannot be consumed; present to merchants for trade.",
                     def->name, (unsigned)def->data.valuable.value_gold);
            return true;

        default:
            snprintf(out_msg, msg_len,
                     "[ITEM] %s has no active invocation routine.", def->name);
            return false;
    }
}

bool items_inspect(const player_inventory_t *inv,
                   uint32_t item_id,
                   const void *player_state,
                   char *out_msg,
                   size_t msg_len)
{
    (void)inv;
    (void)player_state;

    if (!out_msg || msg_len == 0) {
        return false;
    }

    const item_def_t *def = items_find_def(item_id);
    if (!def) {
        snprintf(out_msg, msg_len, "[ITEM-ERROR] Item ID %u was not found.",
                 (unsigned)item_id);
        return false;
    }

    switch (def->type) {
        case ITEM_TYPE_RELIC:
            return relics_inspect(def, out_msg, msg_len);

        case ITEM_TYPE_CATALYST:
            return catalysts_inspect(def, out_msg, msg_len);

        case ITEM_TYPE_CONSUMABLE:
            return consumables_inspect(def, out_msg, msg_len);

        case ITEM_TYPE_VALUABLE:
            snprintf(out_msg, msg_len,
                     "[%s] Valuable Commodity\n"
                     "Description: %s\n"
                     "Appraisal: %u Sacyran Gold.",
                     def->name, def->description,
                     (unsigned)def->data.valuable.value_gold);
            return true;

        default:
            snprintf(out_msg, msg_len, "[%s] %s", def->name, def->description);
            return true;
    }
}

bool items_is_nocturnal_window(const struct player_state_s *state)
{
    (void)state;
    time_t now = time(NULL);
    struct tm tm_info;
    if (localtime_r(&now, &tm_info) == NULL) {
        return false;
    }
    int hour = tm_info.tm_hour;
    return (hour >= 20 || hour < 4);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Orchestrator
// =============================================================================

diag_result_t items_preflight(void)
{
    diag_result_t res;
    res.subsystem = items_manifest.module_id;
    res.check_name = "Item Subsystem End-to-End Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "All item sub-assemblies audited cleanly.";

    /* Step 1: Audit item loader */
    diag_result_t r_loader = item_loader_preflight();
    if (!r_loader.passed && r_loader.severity == SEVERITY_FATAL) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Item loader preflight check failed.";
        return res;
    }

    /* Step 2: Audit relics domain */
    diag_result_t r_relics = relics_preflight();
    if (!r_relics.passed && r_relics.severity == SEVERITY_FATAL) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Relics domain preflight check failed.";
        return res;
    }

    /* Step 3: Audit catalysts domain */
    diag_result_t r_cats = catalysts_preflight();
    if (!r_cats.passed && r_cats.severity == SEVERITY_FATAL) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Catalysts domain preflight check failed.";
        return res;
    }

    /* Step 4: Audit consumables domain */
    diag_result_t r_cons = consumables_preflight();
    if (!r_cons.passed && r_cons.severity == SEVERITY_FATAL) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Consumables domain preflight check failed.";
        return res;
    }

    /* Step 5: Audit inventory engine */
    diag_result_t r_inv = inventory_preflight();
    if (!r_inv.passed && r_inv.severity == SEVERITY_FATAL) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Inventory engine preflight check failed.";
        return res;
    }

    return res;
}

bool items_audit(void)
{
    diag_result_t res = items_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
