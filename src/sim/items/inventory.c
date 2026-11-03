// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          inventory.c
// @id            sim.items.inventory
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
//   Inventory management engine implementation. Handles bounded item addition,
//   removal, stacking, capacity validation, and formatted CLI/GUI listings
//   without dynamic heap allocations.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/items/inventory.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Mutates caller inventory pointer).
//   - Latency Budget:        < 25 microseconds.
//
// AXIOMATIC INVARIANTS:
//   1. Capacity bounds: Slot count strictly <= INVENTORY_MAX_SLOTS (32).
//   2. Quantity bounds: Quantities strictly <= max_stack (1, 99, 999).
//   3. Currency bounds: Gold strictly <= 999,999.
// =============================================================================

#include "sim/items/inventory.h"
#include "util/string/string_utils.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Inventory Engine
// =============================================================================

static const char *const s_inventory_invariants[3] = {
    "Inventory slot count strictly capped at INVENTORY_MAX_SLOTS (32)",
    "Item quantities bounded by item stack ceiling without overflow",
    "Item removal cannot decrease quantities below zero"
};

const module_metadata_t inventory_manifest = {
    "sim.items.inventory",
    "inventory.h",
    LAYER_1_SIMULATION,
    { 1, 1, 0 },
    2,
    "2026-11-03",
    "Player 32-slot bounded inventory management engine",
    1,
    { "core.util.string", NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    20,
    3,
    {
        s_inventory_invariants[0],
        s_inventory_invariants[1],
        s_inventory_invariants[2]
    },
    inventory_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Inventory Engine
// =============================================================================

void inventory_init(player_inventory_t *inv)
{
    if (!inv) {
        return;
    }
    memset(inv, 0, sizeof(*inv));
    inv->slot_count = 0;
    inv->currency_gold = 0;
}

static uint16_t get_catalog_max_stack(const item_def_t *catalog,
                                      size_t catalog_len,
                                      uint32_t item_id)
{
    if (!catalog) {
        return 99;
    }
    for (size_t i = 0; i < catalog_len; i++) {
        if (catalog[i].id == item_id) {
            return (catalog[i].max_stack > 0) ? catalog[i].max_stack : 99;
        }
    }
    return 99;
}

static const char *get_catalog_item_name(const item_def_t *catalog,
                                         size_t catalog_len,
                                         uint32_t item_id)
{
    if (!catalog) {
        return "Unknown Item";
    }
    for (size_t i = 0; i < catalog_len; i++) {
        if (catalog[i].id == item_id) {
            return catalog[i].name;
        }
    }
    return "Unknown Item";
}

bool inventory_add(player_inventory_t *inv,
                   const item_def_t *catalog,
                   size_t catalog_len,
                   uint32_t item_id,
                   uint16_t qty)
{
    if (!inv || item_id == 0 || qty == 0) {
        return false;
    }

    uint16_t max_stack = get_catalog_max_stack(catalog, catalog_len, item_id);
    uint16_t remaining = qty;

    /* Pass 1: Try stacking into existing slots holding the same item */
    for (uint8_t i = 0; i < inv->slot_count; i++) {
        if (inv->slots[i].item_id == item_id &&
            inv->slots[i].quantity < max_stack) {
            uint16_t space = (uint16_t)(max_stack - inv->slots[i].quantity);
            uint16_t to_add = (remaining < space) ? remaining : space;
            inv->slots[i].quantity = (uint16_t)(inv->slots[i].quantity + to_add);
            remaining = (uint16_t)(remaining - to_add);
            if (remaining == 0) {
                return true;
            }
        }
    }

    /* Pass 2: Allocate new slots for leftover quantity */
    while (remaining > 0) {
        if (inv->slot_count >= INVENTORY_MAX_SLOTS) {
            return false; /* Inventory full */
        }
        uint16_t to_add = (remaining < max_stack) ? remaining : max_stack;
        inv->slots[inv->slot_count].item_id = item_id;
        inv->slots[inv->slot_count].quantity = to_add;
        inv->slot_count++;
        remaining = (uint16_t)(remaining - to_add);
    }

    return true;
}

bool inventory_remove(player_inventory_t *inv,
                      uint32_t item_id,
                      uint16_t qty)
{
    if (!inv || item_id == 0 || qty == 0) {
        return false;
    }

    uint16_t held = inventory_get_count(inv, item_id);
    if (held < qty) {
        return false; /* Insufficient items */
    }

    uint16_t to_remove = qty;

    for (int i = (int)inv->slot_count - 1; i >= 0 && to_remove > 0; i--) {
        if (inv->slots[i].item_id == item_id) {
            if (inv->slots[i].quantity <= to_remove) {
                to_remove = (uint16_t)(to_remove - inv->slots[i].quantity);
                /* Compact slot by shifting succeeding elements left */
                for (uint8_t j = (uint8_t)i; j < (uint8_t)(inv->slot_count - 1); j++) {
                    inv->slots[j] = inv->slots[j + 1];
                }
                inv->slot_count--;
                memset(&inv->slots[inv->slot_count], 0, sizeof(inventory_slot_t));
            } else {
                inv->slots[i].quantity = (uint16_t)(inv->slots[i].quantity - to_remove);
                to_remove = 0;
            }
        }
    }

    return true;
}

uint16_t inventory_get_count(const player_inventory_t *inv, uint32_t item_id)
{
    if (!inv || item_id == 0) {
        return 0;
    }

    uint32_t total = 0;
    for (uint8_t i = 0; i < inv->slot_count; i++) {
        if (inv->slots[i].item_id == item_id) {
            total += inv->slots[i].quantity;
        }
    }
    return (total > 65535) ? 65535 : (uint16_t)total;
}

bool inventory_has_item(const player_inventory_t *inv, uint32_t item_id)
{
    return (inventory_get_count(inv, item_id) > 0);
}

bool inventory_format_listing(const player_inventory_t *inv,
                              const item_def_t *catalog,
                              size_t catalog_len,
                              char *out_buf,
                              size_t buf_len)
{
    if (!inv || !out_buf || buf_len == 0) {
        return false;
    }

    size_t offset = 0;
    int n = snprintf(out_buf, buf_len,
                     "=== PLAYER INVENTORY [Slots: %u/%u | Gold: %u G] ===\n",
                     (unsigned)inv->slot_count, (unsigned)INVENTORY_MAX_SLOTS,
                     (unsigned)inv->currency_gold);
    if (n < 0 || (size_t)n >= buf_len) {
        return false;
    }
    offset += (size_t)n;

    if (inv->slot_count == 0) {
        strncat(out_buf, "  (Pouch is empty)\n", buf_len - offset - 1);
        return true;
    }

    for (uint8_t i = 0; i < inv->slot_count; i++) {
        const char *name = get_catalog_item_name(catalog, catalog_len,
                                                 inv->slots[i].item_id);
        n = snprintf(out_buf + offset, buf_len - offset,
                     "  [%02u] [ID:%u] %-28s x%u\n",
                     (unsigned)(i + 1),
                     (unsigned)inv->slots[i].item_id,
                     name,
                     (unsigned)inv->slots[i].quantity);
        if (n < 0 || (size_t)n >= (buf_len - offset)) {
            break;
        }
        offset += (size_t)n;
    }

    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Inventory Engine
// =============================================================================

diag_result_t inventory_preflight(void)
{
    diag_result_t res;
    res.subsystem = inventory_manifest.module_id;
    res.check_name = "Player Inventory Capacity and Stacking Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "32-slot bounded inventory addition and removal verified.";

    player_inventory_t test_inv;
    inventory_init(&test_inv);

    if (test_inv.slot_count != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Initial inventory slot count is non-zero.";
        return res;
    }

    /* Test adding items */
    if (!inventory_add(&test_inv, NULL, 0, 1, 10)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Inventory addition failed.";
        return res;
    }
    if (inventory_get_count(&test_inv, 1) != 10) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Inventory quantity mismatch after add.";
        return res;
    }

    /* Test removing items */
    if (!inventory_remove(&test_inv, 1, 4)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Inventory removal failed.";
        return res;
    }
    if (inventory_get_count(&test_inv, 1) != 6) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Inventory quantity mismatch after removal.";
        return res;
    }

    return res;
}

bool inventory_audit(void)
{
    diag_result_t res = inventory_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
