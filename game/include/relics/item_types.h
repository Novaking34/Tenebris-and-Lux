// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          item_types.h
// @id            sim.items.types
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
//   Ontological type definitions, categorizations, and data structures for
//   diegetic relics, catalysts, consumables, and player inventory tracking.
//   Provides the foundational data layout for the data-driven item engine.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational domain types header).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% fixed-buffer structures).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless type layouts).
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap: All structures are value types with bounded fixed buffers.
//   2. Bound limits: Inventory slots <= 32; Catalog entries <= 64.
//   3. Fixed string capacities: Names <= 48 chars; Descriptions <= 160 chars.
// =============================================================================

#ifndef TENEBRIS_LUX_SIM_ITEMS_ITEM_TYPES_H
#define TENEBRIS_LUX_SIM_ITEMS_ITEM_TYPES_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Ontological Types
// =============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Maximum entries in the runtime item definition catalog. */
#define ITEM_CATALOG_MAX_ENTRIES   64

/** Maximum capacity of player inventory slots. */
#define INVENTORY_MAX_SLOTS        32

/** Maximum string length for item names (including null terminator). */
#define ITEM_NAME_MAX_LEN          48

/** Maximum string length for item descriptions (including null terminator). */
#define ITEM_DESC_MAX_LEN          160

/** Maximum string length for item code identifiers. */
#define ITEM_CODE_MAX_LEN          24

/** Maximum items that can be cured by a single consumable. */
#define CONSUMABLE_MAX_CURES       4

/**
 * Top-level item category classifications.
 */
typedef enum {
    ITEM_TYPE_NONE        = 0,  /**< Uninitialized or empty slot. */
    ITEM_TYPE_RELIC       = 1,  /**< Non-consumable diegetic key relic. */
    ITEM_TYPE_CATALYST    = 2,  /**< Tactically equipped catalyst/gear. */
    ITEM_TYPE_CONSUMABLE  = 3,  /**< Field or combat restorative item. */
    ITEM_TYPE_VALUABLE    = 4   /**< Currency bullion or quest token. */
} item_type_t;

/**
 * Sub-classifications for diegetic key relics.
 */
typedef enum {
    RELIC_TYPE_NONE       = 0,
    RELIC_TYPE_TEMPORAL   = 1,  /**< Clock / monotonic calendar interface. */
    RELIC_TYPE_ARCHIVAL   = 2,  /**< In-universe bestiary / item catalog. */
    RELIC_TYPE_TELEMETRY  = 3,  /**< Moral sway continuum indicator. */
    RELIC_TYPE_PLANAR     = 4,  /**< Astral rift / nocturnal gate key. */
    RELIC_TYPE_DIPLOMATIC = 5   /**< Sovereign treaty / diplomatic charter. */
} relic_type_t;

/**
 * Sub-classifications for catalysts and equipment.
 */
typedef enum {
    CATALYST_TYPE_NONE      = 0,
    CATALYST_TYPE_ELEMENTAL = 1,  /**< Elemental ring / retaliation gear. */
    CATALYST_TYPE_WEAPON    = 2,  /**< Sword / blade offensive catalyst. */
    CATALYST_TYPE_ARMOR     = 3,  /**< Shield / mantle defensive catalyst. */
    CATALYST_TYPE_MYSTIC    = 4   /**< Altar talisman / sanctuary aura. */
} catalyst_type_t;

/**
 * Tactical equipment slot assignments.
 */
typedef enum {
    EQUIP_SLOT_NONE      = 0,
    EQUIP_SLOT_WEAPON    = 1,
    EQUIP_SLOT_SHIELD    = 2,
    EQUIP_SLOT_ARMOR     = 3,
    EQUIP_SLOT_ACCESSORY = 4
} equip_slot_t;

/**
 * Sub-classifications for consumable items.
 */
typedef enum {
    CONSUMABLE_TYPE_NONE        = 0,
    CONSUMABLE_TYPE_RESTORATIVE = 1,  /**< Restores HP or resonance. */
    CONSUMABLE_TYPE_CLEANSE     = 2,  /**< Cures negative tactical states. */
    CONSUMABLE_TYPE_ELIXIR      = 3,  /**< Advanced tonic / edge recovery. */
    CONSUMABLE_TYPE_HARMONIC    = 4,  /**< Dual-phase restorative brew. */
    CONSUMABLE_TYPE_REVIVAL     = 5   /**< Resurrects incapacitated ally. */
} consumable_type_t;

/**
 * Sub-classifications for valuables and tokens.
 */
typedef enum {
    VALUABLE_TYPE_NONE        = 0,
    VALUABLE_TYPE_BULLION     = 1,  /**< Gold bullion / merchant currency. */
    VALUABLE_TYPE_QUEST_TOKEN = 2,  /**< Proof of deed / receipt. */
    VALUABLE_TYPE_TOKEN       = 3,  /**< Commemorative festival token. */
    VALUABLE_TYPE_MAP_TOKEN   = 4   /**< Topographical charter / guide map. */
} valuable_type_t;

/**
 * Specific properties for diegetic relics.
 */
typedef struct {
    relic_type_t sub_type;
    bool clock_interface;
    uint32_t evolves_into;
    bool unlocks_bestiary;
    bool unlocks_compendium;
    bool tracks_moral_sway;
    bool planar_key;
    uint8_t nocturnal_start_hour;
    uint8_t nocturnal_end_hour;
} relic_data_t;

/**
 * Specific properties for tactical catalysts.
 */
typedef struct {
    catalyst_type_t sub_type;
    equip_slot_t slot;
    int16_t atk_bonus;
    int16_t def_bonus;
    int16_t mat_bonus;
    int16_t mdf_bonus;
    int16_t poise_bonus;
    int16_t resonance_rate_bonus;
    uint32_t skill_grant;
    uint32_t inflicts_state;
    uint8_t retaliation_element;  /**< 0: none, 1: fire, 2: light, 3: shadow. */
} catalyst_data_t;

/**
 * Specific properties for consumable items.
 */
typedef struct {
    consumable_type_t sub_type;
    int16_t hp_restore;
    int16_t resonance_restore;
    bool revives_ally;
    uint8_t hp_restore_pct;
    uint8_t num_cures;
    uint32_t cures_states[CONSUMABLE_MAX_CURES];
} consumable_data_t;

/**
 * Specific properties for valuables.
 */
typedef struct {
    valuable_type_t sub_type;
    uint32_t value_gold;
} valuable_data_t;

/**
 * Universal item descriptor.
 */
typedef struct {
    uint32_t id;
    char code[ITEM_CODE_MAX_LEN];
    char name[ITEM_NAME_MAX_LEN];
    char description[ITEM_DESC_MAX_LEN];
    item_type_t type;
    bool consumable;
    uint16_t max_stack;
    union {
        relic_data_t relic;
        catalyst_data_t catalyst;
        consumable_data_t consumable;
        valuable_data_t valuable;
    } data;
} item_def_t;

/**
 * An individual slot in player inventory.
 */
typedef struct {
    uint32_t item_id;
    uint16_t quantity;
} inventory_slot_t;

/**
 * Complete player inventory tracking structure.
 */
typedef struct {
    inventory_slot_t slots[INVENTORY_MAX_SLOTS];
    uint8_t slot_count;
    uint32_t currency_gold;
} player_inventory_t;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Ontological Types
// =============================================================================

// Helper macros and inline validation functions
static inline bool item_is_valid_id(uint32_t id) {
    return (id > 0 && id < 10000);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 1 (Simulation Engine) | Machine: Item Ontological Types
// =============================================================================

// Type invariants are verified at compile time and by subsystem audits.

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_SIM_ITEMS_ITEM_TYPES_H */

