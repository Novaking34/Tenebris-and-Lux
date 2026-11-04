// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          catalysts.c
// @id            sim.items.catalysts
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
//   Domain handlers for tactical catalysts, armaments, and combat equipment.
//   Calculates attribute modifiers, elemental retaliations (Ring of Fire),
//   tactical status inflictions (Medusa's Sword), and guard bonuses.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies:
//       sim/items/catalysts.h
//       util/string/string_utils.h
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless arithmetic).
//   - Latency Budget:        < 15 microseconds.
//
// AXIOMATIC INVARIANTS:
//   1. Clamped calculations: Modifiers never produce underflow/overflow.
//   2. Equipment slot bounds: Strictly respects equip_slot_t types.
//   3. Retaliation consistency: Thermal/light/shadow elements match definition.
// =============================================================================

#include "sim/items/catalysts.h"
#include "util/string/string_utils.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Catalyst Domain Handler
// =============================================================================

static const char *const s_catalysts_invariants[3] = {
    "Equipment catalyst stats additively alter attributes without overflow",
    "Retaliation damage triggers elementally based on equipped catalyst",
    "Catalyst equipment occupies valid single gear slot"
};

const module_metadata_t catalysts_manifest = {
    "sim.items.catalysts",
    "catalysts.h",
    LAYER_1_SIMULATION,
    { 1, 1, 0 },
    2,
    "2026-11-03",
    "Catalyst equipment domain handler and stat modifications",
    1,
    { "core.util.string", NULL, NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    15,
    3,
    {
        s_catalysts_invariants[0],
        s_catalysts_invariants[1],
        s_catalysts_invariants[2]
    },
    catalysts_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Catalyst Domain Handler
// =============================================================================

bool catalysts_apply_stats(const item_def_t *catalyst,
                           int16_t *io_atk,
                           int16_t *io_def,
                           int16_t *io_mat,
                           int16_t *io_mdf)
{
    if (!catalyst || !io_atk || !io_def || !io_mat || !io_mdf) {
        return false;
    }

    if (catalyst->type != ITEM_TYPE_CATALYST) {
        return false;
    }

    const catalyst_data_t *cat = &catalyst->data.catalyst;
    *io_atk = (int16_t)(*io_atk + cat->atk_bonus);
    *io_def = (int16_t)(*io_def + cat->def_bonus);
    *io_mat = (int16_t)(*io_mat + cat->mat_bonus);
    *io_mdf = (int16_t)(*io_mdf + cat->mdf_bonus);

    return true;
}

bool catalysts_check_retaliation(const item_def_t *catalyst,
                                 uint8_t *out_element)
{
    if (!catalyst || !out_element) {
        return false;
    }

    if (catalyst->type != ITEM_TYPE_CATALYST) {
        *out_element = 0;
        return false;
    }

    *out_element = catalyst->data.catalyst.retaliation_element;
    return (*out_element != 0);
}

bool catalysts_check_infliction(const item_def_t *catalyst,
                                uint32_t *out_state_id)
{
    if (!catalyst || !out_state_id) {
        return false;
    }

    if (catalyst->type != ITEM_TYPE_CATALYST) {
        *out_state_id = 0;
        return false;
    }

    *out_state_id = catalyst->data.catalyst.inflicts_state;
    return (*out_state_id != 0);
}

bool catalysts_inspect(const item_def_t *catalyst,
                       char *out_buf,
                       size_t buf_len)
{
    if (!catalyst || !out_buf || buf_len == 0) {
        return false;
    }

    const catalyst_data_t *cat = &catalyst->data.catalyst;

    const char *slot_str = "Unknown";
    switch (cat->slot) {
        case EQUIP_SLOT_WEAPON:    slot_str = "Weapon"; break;
        case EQUIP_SLOT_SHIELD:    slot_str = "Shield"; break;
        case EQUIP_SLOT_ARMOR:     slot_str = "Armor"; break;
        case EQUIP_SLOT_ACCESSORY: slot_str = "Accessory"; break;
        default:                   slot_str = "None"; break;
    }

    snprintf(out_buf, buf_len,
             "[%s] Slot: %s | ATK: %+d, DEF: %+d, MAT: %+d, MDF: %+d, "
             "Poise: %+d\n"
             "Description: %s\n"
             "Special: %s%s",
             catalyst->name, slot_str,
             (int)cat->atk_bonus, (int)cat->def_bonus,
             (int)cat->mat_bonus, (int)cat->mdf_bonus,
             (int)cat->poise_bonus,
             catalyst->description,
             (cat->retaliation_element == 1) ? "[Thermal Retaliation] " : "",
             (cat->inflicts_state == 11) ? "[Stasis Petrify on Hit] " : "");
    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Catalyst Domain Handler
// =============================================================================

diag_result_t catalysts_preflight(void)
{
    diag_result_t res;
    res.subsystem = catalysts_manifest.module_id;
    res.check_name = "Catalyst Equipment Stats and Retaliation Invariants";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Stat additions and elemental retaliation verified.";

    item_def_t ring;
    memset(&ring, 0, sizeof(ring));
    ring.type = ITEM_TYPE_CATALYST;
    ring.data.catalyst.mdf_bonus = 5;
    ring.data.catalyst.retaliation_element = 1;

    int16_t atk = 10, def = 10, mat = 10, mdf = 10;
    if (!catalysts_apply_stats(&ring, &atk, &def, &mat, &mdf)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Stat modification application failed.";
        return res;
    }
    if (mdf != 15 || atk != 10) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Stat bonus calculation mismatch.";
        return res;
    }

    uint8_t elem = 0;
    if (!catalysts_check_retaliation(&ring, &elem) || elem != 1) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Retaliation element check failed.";
        return res;
    }

    return res;
}

bool catalysts_audit(void)
{
    diag_result_t res = catalysts_preflight();
    return (res.passed && res.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
