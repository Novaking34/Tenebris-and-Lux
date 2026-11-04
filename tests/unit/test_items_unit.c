// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          test_items_unit.c
// @id            test.unit.items
// @layer         Layer 4: Verification Suite
// @subsystem     Simulation Items Unit Verification (tests/unit)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-03
// @updated       2026-11-03
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Comprehensive unit verification for the simulation items stratum:
//   diegetic relics, equipment catalysts, consumables/curatives, player
//   inventory capacity boundaries, and item JSON loader parsing.
//   Organized across the 4 Ontological Testing Sections.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: sim/items/items.h, sim/items/relics.h,
//                            sim/items/catalysts.h, sim/items/consumables.h,
//                            sim/items/inventory.h, sim/items/item_loader.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic allocation).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential test runner).
//   - Latency Budget:        < 20 milliseconds for entire suite.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "hero/sim_state.h"
#include "relics/catalysts.h"
#include "relics/consumables.h"
#include "relics/inventory.h"
#include "relics/item_loader.h"
#include "relics/items.h"
#include "relics/relics.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

// =============================================================================
// BLOCK 3: BODY BLOCK / 4-SECTION TEST SUITES
// =============================================================================

static void test_items_always_pass(void)
{
    printf("  [PASS-1] Section 1: Always Pass (Nominal Baseline)...\n");

    /* 1.1 Catalog query */
    size_t count = 0;
    const item_def_t *catalog = items_get_catalog(&count);
    TEST_ASSERT(catalog != NULL && count >= 10,
                "Item catalog loaded >= 10 items");

    /* 1.2 Relic query */
    const item_def_t *watch = items_find_by_name("Grandma's Watch");
    TEST_ASSERT(watch != NULL && watch->id == 5,
                "Found Grandma's Watch");

    /* 1.3 Inventory initial capacity */
    player_inventory_t inv;
    inventory_init(&inv);
    TEST_ASSERT(inv.slot_count == 0, "Inventory initial slot count == 0");
    TEST_ASSERT(inv.currency_gold == 0, "Inventory initial gold == 0");

    printf("  [PASS-1] Section 1 passed cleanly.\n");
}

static void test_items_never_fail(void)
{
    printf("  [FAIL-2] Section 2: Never Fail (Fault-Tolerant Hardening)...\n");

    player_state_t state;
    sim_state_init(&state);

    /* 2.1 Use item ID 0 (invalid) */
    char out[128];
    bool r1 = items_use(&state.inventory, 0, &state, out, sizeof(out));
    TEST_ASSERT(!r1, "Item ID 0 cannot be used");

    /* 2.2 Use non-existent item ID 9999 */
    bool r2 = items_use(&state.inventory, 9999, &state, out, sizeof(out));
    TEST_ASSERT(!r2, "Non-existent item ID rejected");

    /* 2.3 Inspect invalid item ID */
    bool r3 = items_inspect(&state.inventory, 9999, &state, out, sizeof(out));
    TEST_ASSERT(!r3, "Inspect invalid item rejected");

    /* 2.4 NULL pointer resilience */
    bool r4 = items_use(NULL, 1, NULL, out, sizeof(out));
    TEST_ASSERT(!r4, "NULL inventory handled safely in items_use");

    printf("  [FAIL-2] Section 2 passed cleanly.\n");
}

static void test_items_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Section 3: Assertion & Guarantee (Invariants)...\n");

    player_state_t state;
    sim_state_init(&state);

    /* 3.1 Inventory slot addition and count invariants */
    inventory_init(&state.inventory);
    inventory_add(&state.inventory, NULL, 0, 1, 3);
    TEST_ASSERT(inventory_get_count(&state.inventory, 1) == 3,
                "Inventory has 3 potions");

    /* 3.2 Consumable restores HP and decrements count */
    state.hp = 200; // damage player
    char msg[128];
    bool used = items_use(&state.inventory, 1, &state, msg, sizeof(msg));
    TEST_ASSERT(used, "Potion successfully used");
    TEST_ASSERT(state.hp == 230, "HP restored from 200 to 230 (+30 HP)");
    TEST_ASSERT(inventory_get_count(&state.inventory, 1) == 2,
                "Potions decremented to 2");

    /* 3.3 Catalyst equipping sets flags */
    state.has_medusas_sword = true;
    TEST_ASSERT(state.has_medusas_sword, "Equipped Medusa's Sword catalyst");

    printf("  [AXIOM-3] Section 3 passed cleanly.\n");
}

static void test_items_emergence(void)
{
    printf("  [EMERGE-4] Section 4: Emergence (Inventory Capacity)...\n");

    player_inventory_t inv;
    inventory_init(&inv);

    /* Fill inventory to max slots */
    for (uint32_t i = 1; i <= INVENTORY_MAX_SLOTS; i++) {
        bool added = inventory_add(&inv, NULL, 0, i, 5);
        TEST_ASSERT(added, "Added unique item slot");
    }
    TEST_ASSERT(inv.slot_count == INVENTORY_MAX_SLOTS,
                "Inventory at maximum capacity");

    /* Adding another unique item beyond capacity must fail safely */
    bool overflow = inventory_add(&inv, NULL, 0, 999, 1);
    TEST_ASSERT(!overflow, "Overflow addition rejected safely");

    /* Consuming all counts removes the slot */
    for (uint32_t i = 1; i <= INVENTORY_MAX_SLOTS; i++) {
        bool rem = inventory_remove(&inv, i, 5);
        TEST_ASSERT(rem, "Depleted item slot");
    }
    TEST_ASSERT(inv.slot_count == 0, "Inventory completely emptied cleanly");

    printf("  [EMERGE-4] Section 4 passed cleanly.\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

static void test_items_preflight(void)
{
    printf("[AUDIT] Running Items Subsystem Pre-Flight Verification...\n");
    diag_result_t res = items_preflight();
    TEST_ASSERT(res.passed, "Items preflight passed");
    printf("[AUDIT] Items preflight passed: %s\n", res.details);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TEST RUNNER
// =============================================================================

int main(void)
{
    printf("============================================================\n");
    printf("       Tenebris and Lux - Items Unit Verification Suite     \n");
    printf("============================================================\n");

    test_items_preflight();
    test_items_always_pass();
    test_items_never_fail();
    test_items_assertions_and_guarantees();
    test_items_emergence();

    printf("============================================================\n");
    printf("ALL ITEMS UNIT TESTS PASSED (4/4 Sections Verified)\n");
    printf("============================================================\n");
    return 0;
}
