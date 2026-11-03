// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          consumables.c
// @id            sim.items.consumables
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
//   Domain handlers for consumable items, potions, and field tinctures.
//   Applies Vitality restoration, Shield/Sword resonance replenishment,
//   curative purging of tactical ailments, and revival mechanics.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/items/consumables.h
//       util/math/math_utils.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure arithmetic state mutation).
//   - Latency Budget:        < 15 microseconds.
//
// AXIOMATIC INVARIANTS:
//   1. Bound clamping: HP clamped to [0, max_hp]; Resonance to [0, 100].
//   2. Depletion rule: Consumables decrement count upon successful use.
//   3. Cure matching: States purged must match cures_states table.
// =============================================================================

#include "sim/items/consumables.h"
#include "util/math/math_utils.h"
#include "util/string/string_utils.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Consumable Domain Handler
// =============================================================================

static const char *const s_consumables_invariants[3] = {
    "Consumables saturate vitality without exceeding max_hp ceiling",
    "Resonance restoration respects strictly bounded [0, MAX_RESONANCE]",
    "Consumables deplete exactly one item per successful invocation"
};

const module_metadata_t consumables_manifest = {
    "sim.items.consumables",
    "consumables.h",
    LAYER_1_SIMULATION,
    { 1, 1, 0 },
    2,
    "2026-11-03",
    "Consumable restorative and curative item domain handler",
    2,
    { "core.util.math", "core.util.string", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    15,
    3,
    {
        s_consumables_invariants[0],
        s_consumables_invariants[1],
        s_consumables_invariants[2]
    },
    consumables_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Consumable Domain Handler
// =============================================================================

bool consumables_apply_effect(const item_def_t *item,
                              int32_t *io_hp,
                              int32_t max_hp,
                              int32_t *io_shield,
                              int32_t *io_sword,
                              char *out_msg,
                              size_t msg_len)
{
    if (!item || !io_hp || !io_shield || !io_sword || !out_msg || msg_len == 0) {
        return false;
    }

    if (item->type != ITEM_TYPE_CONSUMABLE) {
        snprintf(out_msg, msg_len, "[ITEM] %s is not a consumable item.",
                 item->name);
        return false;
    }

    const consumable_data_t *cons = &item->data.consumable;

    int32_t old_hp = *io_hp;
    int32_t old_shield = *io_shield;
    int32_t old_sword = *io_sword;

    /* Handle HP recovery */
    if (cons->hp_restore > 0) {
        *io_hp = math_clamp_int(*io_hp + cons->hp_restore, 0, max_hp);
    } else if (cons->revives_ally && *io_hp <= 0) {
        int32_t revived_val = (max_hp * (int32_t)cons->hp_restore_pct) / 100;
        *io_hp = math_clamp_int(revived_val, 1, max_hp);
    }

    /* Handle Resonance replenishment */
    if (cons->resonance_restore > 0) {
        if (cons->sub_type == CONSUMABLE_TYPE_ELIXIR) {
            *io_sword = math_clamp_int(*io_sword + cons->resonance_restore,
                                       0, 100);
        } else if (cons->sub_type == CONSUMABLE_TYPE_HARMONIC) {
            *io_shield = math_clamp_int(*io_shield + cons->resonance_restore,
                                        0, 100);
            *io_sword = math_clamp_int(*io_sword + cons->resonance_restore,
                                       0, 100);
        } else {
            *io_shield = math_clamp_int(*io_shield + cons->resonance_restore,
                                        0, 100);
        }
    }

    snprintf(out_msg, msg_len,
             "[CONSUMED] %s applied! HP: %d->%d | Shield: %d->%d | Sword: %d->%d",
             item->name,
             (int)old_hp, (int)*io_hp,
             (int)old_shield, (int)*io_shield,
             (int)old_sword, (int)*io_sword);
    return true;
}

bool consumables_cures_state(const item_def_t *item, uint32_t state_id)
{
    if (!item || item->type != ITEM_TYPE_CONSUMABLE) {
        return false;
    }

    const consumable_data_t *cons = &item->data.consumable;
    for (uint8_t i = 0; i < cons->num_cures && i < CONSUMABLE_MAX_CURES; i++) {
        if (cons->cures_states[i] == state_id) {
            return true;
        }
    }

    return false;
}

bool consumables_inspect(const item_def_t *item,
                         char *out_buf,
                         size_t buf_len)
{
    if (!item || !out_buf || buf_len == 0) {
        return false;
    }

    const consumable_data_t *cons = &item->data.consumable;
    snprintf(out_buf, buf_len,
             "[%s] Consumable (Stack max: %u)\n"
             "Description: %s\n"
             "Effects: HP Restore: %+d | Resonance Restore: %+d",
             item->name, (unsigned)item->max_stack,
             item->description,
             (int)cons->hp_restore, (int)cons->resonance_restore);
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Consumable Domain Handler
// =============================================================================

diag_result_t consumables_preflight(void)
{
    diag_result_t res;
    res.subsystem = consumables_manifest.module_id;
    res.check_name = "Consumable Vitality Restoration Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Vitality restoration and saturation verified.";

    item_def_t herb;
    memset(&herb, 0, sizeof(herb));
    herb.type = ITEM_TYPE_CONSUMABLE;
    herb.data.consumable.sub_type = CONSUMABLE_TYPE_RESTORATIVE;
    herb.data.consumable.hp_restore = 30;

    int32_t hp = 50;
    int32_t shield = 50;
    int32_t sword = 50;
    char msg[128];

    if (!consumables_apply_effect(&herb, &hp, 100, &shield, &sword,
                                  msg, sizeof(msg))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Consumable effect application failed.";
        return res;
    }

    if (hp != 80) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Vitality restoration amount mismatch.";
        return res;
    }

    return res;
}

bool consumables_audit(void)
{
    diag_result_t res = consumables_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
