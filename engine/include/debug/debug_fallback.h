/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          debug_fallback.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap Engine Debugger & Validation Gatekeeper
@role          Tier-0 BIOS Safe-Mode Debugger Code Page & Compiled-In ROM Vault
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined execution path: Never alters active state uninvited.
               4. Provenance tracking: loaded_from_data is unconditionally false
                  for monolith templates and ROM-hydrated Safe-Mode defaults.
               5. Profile identity explicitly stamped as
                  DEBUG_FALLBACK_PROFILE_NAME or DEBUG_FALLBACK_COMPACT_PROFILE_NAME.
               6. Compiled-in .rodata diagnostics/taxonomy ROM hydrates canonical
                  profiles, snapshot policies, and tripwire matrices with zero disk I/O.
               7. Universal 5-Block Lifecycle Protocol enforced across header/source.
               8. Strictly ISO C-style comments only (zero double-slash comments).

PURPOSE & ARCHITECTURAL ROLE:
  Tier-0 Safe-Mode BIOS debugger code page, emergency monolith defaults,
  shared JSON schema parsers, and compiled-in .rodata diagnostics/taxonomy ROM.
  Serves three cooperating roles:
  (a) Emergency Safe-Mode Boot: Provides static compile-time defaults and
      bootstraps debug state even when disk manifests are corrupt or deleted.
  (b) Compiled-In Diagnostics & Taxonomy ROM: Embeds all debugger and taxonomy
      JSON assets into .rodata arrays parsed cleanly via master_loader_load_from_memory().
  (c) Tripwire Fallback & AWAIT Flight Box: Records anomalies, completes halted
      executions cleanly under fallback guarantees, and latches AWAIT until the
      fault is inspected and a validated state is restored.

  ### For Game Designers & Non-Programmers:
  Think of this module as the game engine's Safe Mode circuit breaker and black
  box. Even if configuration files on disk are corrupted, missing, or on a
  locked floppy disk, Safe Mode keeps emergency rewind protection online,
  finishes broken operations safely without crashing, and pauses in AWAIT so you
  can inspect what went wrong and restore a healthy save point.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time .rodata & .bss: Zero dynamic heap allocation.
  - Shared JSON Parsers: Single authoritative parsers for debug_profile.v1,
    snapshot_policy.v1, and tripwire_matrix.v1 reused by debug.c and debug_fallback.c.
  - ROM Diagnostics Vault: Validates all 9 debugger + taxonomy JSON payloads
    in debug_fallback_audit() without touching disk I/O or active s_debug state.
================================================================================
*/

#ifndef TENEBRIS_LUX_DEBUG_FALLBACK_H
#define TENEBRIS_LUX_DEBUG_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & Validation Gatekeeper
================================================================================
Mission: Quarantined emergency fallback limits, 3-tier include ladder, shared
tripwire/reaction/test-tier ontology, and Safe-Mode inspection contracts.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Fallback Configuration Constants
--------------------------------------------------------------------------------
Compile-time static defaults for emergency boot and headless test execution.
For Designers: Guarantees that even if debugger setting files are missing, the
game can still pause on errors, record up to 64 turns, and dump state cards.
For Programmers: Immutable constants residing in .rodata with zero heap allocation.
--------------------------------------------------------------------------------
*/

/** Baseline profile name for the emergency debugger fallback. */
#define DEBUG_FALLBACK_PROFILE_NAME         "monolith_fallback"

/** Compact 1.44 MB Floppy Safe-Mode profile identifier string. */
#define DEBUG_FALLBACK_COMPACT_PROFILE_NAME "floppy_debug_safe"

/** Fallback snapshot ring buffer capacity limit (64 turns). */
#define DEBUG_FALLBACK_MAX_SNAPSHOTS        64

/** Compact Floppy Safe-Mode snapshot ring capacity (16 turns). */
#define DEBUG_FALLBACK_FLOPPY_SNAPSHOTS     16

/** Fallback turn interval between automatic state snapshots (every 1 turn). */
#define DEBUG_FALLBACK_INTERVAL_TURNS       1

/** Fallback active telemetry channel filter list. */
#define DEBUG_FALLBACK_TRACE_CHANNELS       "sim,combat,world,dispatcher"

/** Fallback breakpoint trigger policy on general errors. */
#define DEBUG_FALLBACK_BREAK_ON_ERROR       false

/** Fallback breakpoint trigger policy on diagnostic tripwires. */
#define DEBUG_FALLBACK_BREAK_ON_TRIPWIRE    true

/** Fallback breakpoint trigger policy on moral sway breaches. */
#define DEBUG_FALLBACK_BREAK_ON_SWAY        true

/** Fallback breakpoint trigger policy on weapon resonance overcharge. */
#define DEBUG_FALLBACK_BREAK_ON_RESONANCE   true

/** Fallback breakpoint trigger policy on hero HP reaching zero. */
#define DEBUG_FALLBACK_BREAK_ON_HP_ZERO     true

/** Fallback breakpoint trigger policy on combat stance shifts. */
#define DEBUG_FALLBACK_BREAK_ON_STANCE      false

/** Fallback automatic state card dump on tripwire latch. */
#define DEBUG_FALLBACK_AUTO_DUMP            true

/** Fallback interactive REPL activation flag. */
#define DEBUG_FALLBACK_INTERACTIVE_REPL     true

/** Fallback ANSI color escape formatting flag. */
#define DEBUG_FALLBACK_ANSI_COLOR           true

/** Fallback domain trap gate for engine infrastructure checks. */
#define DEBUG_FALLBACK_ENGINE_TRAPS_ACTIVE  true

/** Fallback domain trap gate for Layer 1 gameplay invariant checks. */
#define DEBUG_FALLBACK_GAME_TRAPS_ACTIVE    true

/** Fallback locked critical snapshot slot quota (from snapshot_policy.json). */
#define DEBUG_FALLBACK_LOCKED_SLOT_QUOTA    4U

/** Fallback continuous step verification flag (triggers validation probes). */
#define DEBUG_FALLBACK_VERIFY_ON_STEP       true

/** Fallback automatic transition into DEBUG_STEP_AWAIT after tripwire handling. */
#define DEBUG_FALLBACK_AUTO_AWAIT           true

/** Maximum tripwire reaction rules parsed from tripwire_matrix.json. */
#define DEBUG_FALLBACK_MAX_TRIPWIRE_RULES   16U

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the fallback code page.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Debugger Fallback subsystem. */
#define DEBUG_FALLBACK_MODULE_ID            "debug.fallback"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Fallback Object Definitions
--------------------------------------------------------------------------------
Shared tripwire, reaction, test-tier ontology, ROM metadata, and Safe-Mode state.
All structs and enums carry explicit non-anonymous tags for DWARF debuggers.
--------------------------------------------------------------------------------
*/

/** Forward declaration of the full debugger configuration structure. */
struct debug_config;

/**
 * @enum debug_tripwire
 * @brief Latched diagnostic tripwire states for the debugger and validation pipeline.
 *
 * For Designers: Alarm switches that indicate if configuration data was missing
 * or damaged, if a fail-test fired, or if Safe Mode engaged after a fault.
 *
 * For Programmers: Latching error enum preserving 0..4 binary compatibility
 * while extending 5..7 for active validation probes, audits, and fallback halts.
 */
typedef enum debug_tripwire {
    DEBUG_TRIPWIRE_CLEAR = 0,         /**< All tripwires clear; simulation healthy. */
    DEBUG_TRIPWIRE_MANIFEST_MISSING,  /**< Debugger/subsystem manifest unavailable. */
    DEBUG_TRIPWIRE_SETTING_MISSING,   /**< Designated JSON setting profile missing. */
    DEBUG_TRIPWIRE_STATE_CORRUPT,     /**< Simulation state breached domain invariants. */
    DEBUG_TRIPWIRE_HASH_MISMATCH,     /**< Snapshot checksum diverged from recorded state. */
    DEBUG_TRIPWIRE_TEST_FAILED,       /**< Registered validation probe / fail-test fired. */
    DEBUG_TRIPWIRE_AUDIT_BREACH,      /**< Block 4 preflight or audit invariant failed. */
    DEBUG_TRIPWIRE_FALLBACK_ENGAGED   /**< Downgraded to Safe Mode to complete execution. */
} debug_tripwire_t;

/**
 * @enum debug_tripwire_reaction
 * @brief Recovery action taxonomy ingested from taxonomy/tripwire_matrix.json.
 *
 * For Designers: Chooses how the engine reacts when an alarm trips—from simply
 * logging a warning, clamping bad numbers, or dropping into Safe Mode, to
 * rewinding time or halting in AWAIT until repaired.
 *
 * For Programmers: Mapped 1:1 to tripwire_matrix.json reaction strings.
 */
typedef enum debug_tripwire_reaction {
    DEBUG_REACTION_LOG_AND_CONTINUE = 0, /**< Emit telemetry note and continue. */
    DEBUG_REACTION_SATURATE_CLAMP,       /**< Clamp out-of-range value to safe limits. */
    DEBUG_REACTION_FALLBACK_MODE,        /**< Downgrade step cleanly via fallback code. */
    DEBUG_REACTION_AUTO_REWIND,          /**< Restore last locked healthy snapshot. */
    DEBUG_REACTION_PANIC_HALT            /**< Complete via fallback then halt in AWAIT. */
} debug_tripwire_reaction_t;

/**
 * @enum debug_test_tier
 * @brief Ontological verification tier associated with a tripwire or probe.
 *
 * For Designers: Identifies which inspection level (in-file self-check or one
 * of the four external test suites) caught an anomaly.
 *
 * For Programmers: Maps each registered debug validation probe back to its
 * Block 4 audit helper and Layer 4 test suite (`Unit`, `Functional`, `Integrated`, `System`).
 */
typedef enum debug_test_tier {
    DEBUG_TEST_TIER_AUDIT = 0,     /**< In-file Block 4 preflight / validation cog. */
    DEBUG_TEST_TIER_UNIT = 1,      /**< Layer 4 Unit isolation test suite. */
    DEBUG_TEST_TIER_FUNCTIONAL,    /**< Layer 4 Functional cog verification suite. */
    DEBUG_TEST_TIER_INTEGRATED,    /**< Layer 4 Cross-subsystem integrated suite. */
    DEBUG_TEST_TIER_SYSTEM         /**< Layer 4 End-to-end system stress & soak suite. */
} debug_test_tier_t;

/**
 * @struct debug_fallback_profile
 * @brief Self-contained fallback configuration record for the debugger engine.
 *
 * For Designers: Emergency profile ensuring the state rewind system stays online.
 * For Programmers: Invariant loaded_from_data is strictly false.
 */
typedef struct debug_fallback_profile {
    const char *profile_name;     /**< Fixed profile identifier string. */
    bool        loaded_from_data; /**< Invariant: unconditionally false. */
    size_t      max_snapshots;    /**< Fallback snapshot capacity (64). */
} debug_fallback_profile_t;

/**
 * @struct debug_fallback_tripwire_rule
 * @brief Parsed tripwire reaction rule from diagnostics/taxonomy/tripwire_matrix.json.
 *
 * For Designers: Explains which rule was triggered and what emergency response
 * the debugger takes automatically.
 *
 * For Programmers: Zero-heap rule entry populated by debug_fallback_parse_tripwire_matrix_json().
 */
typedef struct debug_fallback_tripwire_rule {
    char                      id[48];                 /**< Rule identifier (e.g., TW_ENG_ZERO_HEAP). */
    char                      bound_error[48];        /**< Linked taxonomy error code ID. */
    char                      trigger_condition[128]; /**< Human/machine condition expression. */
    char                      fallback_action[128];   /**< Action taken when downgrading to fallback. */
    debug_tripwire_reaction_t reaction;               /**< Configured reaction strategy. */
    bool                      tripwire_latch;         /**< True if tripwire latches debugger trap. */
    bool                      is_recoverable;         /**< True if recoverable via fallback/rewind. */
} debug_fallback_tripwire_rule_t;

/**
 * @struct debug_fallback_rom_info
 * @brief Summary telemetry descriptor for the compiled-in .rodata diagnostics ROM.
 *
 * For Designers: Reports how many debugger and error-code JSON files are built
 * straight into the executable so Safe Mode works without external data folders.
 *
 * For Programmers: Populated cleanly from debug_diagnostics_rom.h metadata.
 */
typedef struct debug_fallback_rom_info {
    size_t asset_count;             /**< Number of embedded JSON assets in .rodata. */
    size_t total_rom_bytes;         /**< Total uncompressed JSON bytes in .rodata. */
    char   active_rom_profile[32];  /**< Most recently hydrated ROM setting ID. */
} debug_fallback_rom_info_t;

/**
 * @struct debug_fallback_stats
 * @brief Runtime telemetry report for Tier-0 BIOS Safe-Mode capture and AWAIT.
 *
 * For Designers: Status card detailing Safe-Mode catches, fallback completions,
 * and whether the engine is currently waiting in AWAIT for recovery.
 *
 * For Programmers: Zero-heap status struct populated by debug_fallback_get_stats().
 */
typedef struct debug_fallback_stats {
    bool     booted;                   /**< True once debug_fallback_boot() is armed. */
    bool     safe_mode_engaged;        /**< True after fallback downgrade triggered. */
    bool     fallback_completed;       /**< True once fallback finished the halted step. */
    bool     await_active;             /**< True while BIOS sits idle in AWAIT. */
    bool     rom_hydrated;             /**< True if a ROM asset hydrated cleanly. */
    uint32_t fault_count;              /**< Total Tier-0 faults captured. */
    uint32_t recovery_count;           /**< Total AWAIT resolutions completed. */
    char     last_subsystem[32];       /**< Subsystem that raised the last fault. */
    char     last_condition[128];      /**< Last captured fault description. */
    char     last_source[96];          /**< Formatted file:line anchor. */
    char     last_test_ref[64];        /**< Test suite / Block 4 reference tag. */
    char     last_action_taken[96];    /**< Fallback action executed before AWAIT. */
    char     active_rom_profile[32];   /**< Active ROM profile identifier. */
} debug_fallback_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline inspection predicates and enum stringifiers.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a profile name matches the canonical fallback.
 *
 * @param[in] name Profile name string to compare.
 * @return true if name matches DEBUG_FALLBACK_PROFILE_NAME; false otherwise.
 */
static inline bool debug_fallback_is_profile_name(const char *name)
{
    if (name == NULL) {
        return false;
    }
    const char *expected = DEBUG_FALLBACK_PROFILE_NAME;
    while (*name && *expected) {
        if (*name != *expected) {
            return false;
        }
        name++;
        expected++;
    }
    return *name == *expected;
}

/**
 * @brief Returns canonical string name for a debug_tripwire_t enum code.
 *
 * @param[in] wire Tripwire enumerator.
 * @return Null-terminated static identifier string.
 */
static inline const char *debug_tripwire_name(debug_tripwire_t wire)
{
    switch (wire) {
        case DEBUG_TRIPWIRE_CLEAR:            return "CLEAR";
        case DEBUG_TRIPWIRE_MANIFEST_MISSING: return "MANIFEST_MISSING";
        case DEBUG_TRIPWIRE_SETTING_MISSING:  return "SETTING_MISSING";
        case DEBUG_TRIPWIRE_STATE_CORRUPT:    return "STATE_CORRUPT";
        case DEBUG_TRIPWIRE_HASH_MISMATCH:    return "HASH_MISMATCH";
        case DEBUG_TRIPWIRE_TEST_FAILED:      return "TEST_FAILED";
        case DEBUG_TRIPWIRE_AUDIT_BREACH:     return "AUDIT_BREACH";
        case DEBUG_TRIPWIRE_FALLBACK_ENGAGED: return "FALLBACK_ENGAGED";
        default:                              return "UNKNOWN";
    }
}

/**
 * @brief Returns canonical string name for a debug_tripwire_reaction_t enum code.
 *
 * @param[in] reaction Reaction enumerator.
 * @return Null-terminated static identifier string.
 */
static inline const char *debug_tripwire_reaction_name(debug_tripwire_reaction_t reaction)
{
    switch (reaction) {
        case DEBUG_REACTION_LOG_AND_CONTINUE: return "LOG_AND_CONTINUE";
        case DEBUG_REACTION_SATURATE_CLAMP:   return "SATURATE_CLAMP";
        case DEBUG_REACTION_FALLBACK_MODE:    return "FALLBACK_MODE";
        case DEBUG_REACTION_AUTO_REWIND:      return "AUTO_REWIND";
        case DEBUG_REACTION_PANIC_HALT:       return "PANIC_HALT";
        default:                              return "UNKNOWN";
    }
}

/**
 * @brief Returns canonical string name for a debug_test_tier_t enum code.
 *
 * @param[in] tier Verification tier enumerator.
 * @return Null-terminated static identifier string.
 */
static inline const char *debug_test_tier_name(debug_test_tier_t tier)
{
    switch (tier) {
        case DEBUG_TEST_TIER_AUDIT:      return "AUDIT";
        case DEBUG_TEST_TIER_UNIT:       return "UNIT";
        case DEBUG_TEST_TIER_FUNCTIONAL: return "FUNCTIONAL";
        case DEBUG_TEST_TIER_INTEGRATED: return "INTEGRATED";
        case DEBUG_TEST_TIER_SYSTEM:     return "SYSTEM";
        default:                         return "UNKNOWN";
    }
}

/*
================================================================================
BLOCK 3: BODY BLOCK / FALLBACK CODE PAGE DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & Validation Gatekeeper
================================================================================
Mission: Expose emergency static defaults, shared JSON profile/policy/matrix
parsers, compiled-in .rodata ROM lookup routines, and Safe-Mode fault/AWAIT hooks.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Emergency Static Templates & Shared JSON Schema Parsers
Mission: Guarantee baseline construction and unify JSON hydration across disk
and ROM paths with zero duplicated parser logic.
--------------------------------------------------------------------------------
*/

/**
 * @brief Populates a raw destination buffer with static monolith fallback settings.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads the engine's emergency factory default configuration directly from RAM
 * when data manifests cannot be read or are intentionally bypassed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct type-erased memory copy avoiding cyclic includes with debug.h.
 * Target buffer size must match sizeof(debug_config_t). Sets loaded_from_data = false.
 *
 * @param[out] out_config  Caller-allocated destination debug_config_t buffer.
 * @param[in]  config_size Size verification parameter matching sizeof(debug_config_t).
 */
void debug_get_monolith_fallback_config_raw(void *out_config, size_t config_size);

/**
 * @brief Populates a raw buffer with the compact 1.44 MB Floppy Safe-Mode preset.
 *
 * @param[out] out_config  Caller-allocated destination debug_config_t buffer.
 * @param[in]  config_size Size verification parameter matching sizeof(debug_config_t).
 */
void debug_get_floppy_safe_config_raw(void *out_config, size_t config_size);

/**
 * @brief Shared parser hydrating debug_config_t from a tenlux.data.debug_profile.v1 scope.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads debugger JSON settings (including traps, snapshot limits, and REPL modes)
 * identically whether they come from a file on disk or from built-in ROM backups.
 *
 * ### For Systems Programmers & Engine Architects:
 * Unifies disk and ROM profile parsing into one bounded zero-heap routine.
 * Validates snapshot capacity and channel strings, rejecting malformed bounds cleanly.
 *
 * @param[in]  scope        Null-terminated JSON object slice (`profile` block or body).
 * @param[in]  profile_id   Fallback profile identifier stamp if JSON omits `id`.
 * @param[in]  from_data    Provenance flag: true when read from disk VFS; false from ROM.
 * @param[out] out_config   Destination debug_config_t buffer.
 * @param[in]  config_size  Must equal sizeof(debug_config_t).
 * @param[out] out_tripwire Optional failure tripwire enum code (may be NULL).
 * @param[out] err_buf      Optional caller error string buffer (may be NULL).
 * @param[in]  err_size     Capacity of err_buf.
 * @return true if parsed within all Layer 0 bounds; false on NULL or invalid bounds.
 */
bool debug_fallback_parse_config_json(
    const char       *scope,
    const char       *profile_id,
    bool              from_data,
    void             *out_config,
    size_t            config_size,
    debug_tripwire_t *out_tripwire,
    char             *err_buf,
    size_t            err_size
);

/**
 * @brief Shared parser hydrating snapshot ring policy fields from snapshot_policy.json.
 *
 * @param[in]     scope       Null-terminated JSON slice (`policy` block or body).
 * @param[in,out] out_config  Destination debug_config_t buffer whose policy fields are updated.
 * @param[in]     config_size Must equal sizeof(debug_config_t).
 * @param[out]    err_buf     Optional error string buffer (may be NULL).
 * @param[in]     err_size    Capacity of err_buf.
 * @return true if policy JSON is valid and within static bounds; false otherwise.
 */
bool debug_fallback_parse_snapshot_policy_json(
    const char *scope,
    void       *out_config,
    size_t      config_size,
    char       *err_buf,
    size_t      err_size
);

/**
 * @brief Shared parser extracting reaction rules from taxonomy/tripwire_matrix.json.
 *
 * @param[in]  scope     Null-terminated JSON slice (body containing `"tripwires"` array).
 * @param[out] out_rules Destination rule array (capacity max_rules).
 * @param[in]  max_rules Maximum rules storable in out_rules.
 * @param[out] out_count Number of rules parsed and written.
 * @param[out] err_buf   Optional error buffer (may be NULL).
 * @param[in]  err_size  Capacity of err_buf.
 * @return true if at least one valid rule parsed cleanly; false otherwise.
 */
bool debug_fallback_parse_tripwire_matrix_json(
    const char                     *scope,
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count,
    char                           *err_buf,
    size_t                          err_size
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Compiled-In .rodata Debugger & Taxonomy ROM Vault
Mission: Expose read-only embedded debugger/taxonomy JSON assets and hydrate
Safe-Mode configurations cleanly with zero disk I/O.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns total count of embedded JSON assets compiled into diagnostics ROM.
 *
 * @return Asset count in s_debug_diagnostics_rom_table.
 */
size_t debug_fallback_rom_count(void);

/**
 * @brief Returns raw JSON string pointer and byte length for ROM index `index`.
 *
 * @param[in]  index    Zero-based index `< debug_fallback_rom_count()`.
 * @param[out] out_size Optional pointer receiving asset byte size.
 * @return Immutable null-terminated `.rodata` pointer, or NULL if out of bounds.
 */
const char *debug_fallback_rom_asset_at(size_t index, size_t *out_size);

/**
 * @brief Finds an embedded diagnostics or taxonomy JSON asset by ID or rel_path.
 *
 * @param[in]  id_or_rel_path e.g. `"debugger/default"`, `"default"`, or
 *                            `"diagnostics/taxonomy/tripwire_matrix.json"`.
 * @param[out] out_size       Optional byte size output.
 * @return Immutable `.rodata` pointer to raw JSON text, or NULL if not found.
 */
const char *debug_fallback_find_rom_asset(const char *id_or_rel_path, size_t *out_size);

/**
 * @brief Hydrates a debug_config_t directly from compiled-in .rodata ROM JSON.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses the specified debugger profile via master_loader_load_from_memory() and
 * applies `snapshot_policy.json` from ROM, preserving loaded_from_data = false.
 *
 * @param[in]  profile_id Profile stem (`"default"`, `"strict"`, `"trace"`).
 * @param[out] out_config Caller debug_config_t destination.
 * @param[in]  config_size Must equal sizeof(debug_config_t).
 * @return true if parsed from ROM cleanly; false on missing asset or error.
 */
bool debug_fallback_load_rom_profile(
    const char *profile_id,
    void       *out_config,
    size_t      config_size
);

/**
 * @brief Overlays snapshot_policy fields onto out_config from compiled-in ROM.
 *
 * @param[in,out] out_config Destination debug_config_t buffer.
 * @param[in]     config_size Must equal sizeof(debug_config_t).
 * @return true on clean ROM parse; false otherwise.
 */
bool debug_fallback_load_rom_snapshot_policy(void *out_config, size_t config_size);

/**
 * @brief Loads canonical tripwire reaction rules from compiled-in taxonomy ROM.
 *
 * @param[out] out_rules Destination rule array.
 * @param[in]  max_rules Array capacity.
 * @param[out] out_count Parsed rule count.
 * @return true on clean ROM parse; false otherwise.
 */
bool debug_fallback_load_rom_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Tier-0 Safe-Mode Fault Capture, Fallback Finishing & AWAIT
Mission: Guarantee that any failed test or tripwire latches its fault context,
completes execution cleanly using Safe-Mode fallback, and enters AWAIT.
--------------------------------------------------------------------------------
*/

/**
 * @brief Captures a fault or fail-test signal into Tier-0 Safe-Mode .bss memory.
 *
 * @param[in] subsystem Subsystem identifier tag.
 * @param[in] condition Fault condition or failing assertion description.
 * @param[in] file      Source file anchor (`__FILE__`).
 * @param[in] line      Source line anchor (`__LINE__`).
 * @param[in] test_ref  Associated Block 4 / Layer 4 test identifier (may be NULL).
 * @return true if recorded into Tier-0 Safe-Mode fault latch; false on invalid args.
 */
bool debug_fallback_record_fault(
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *test_ref
);

/**
 * @brief Completes the interrupted operation in Safe Mode and enters idle AWAIT.
 *
 * @param[in] subsystem       Subsystem executing the fallback recovery action.
 * @param[in] fallback_action Summary of Safe-Mode completion step taken.
 * @return true once fallback completion and AWAIT latch are armed.
 */
bool debug_fallback_finish_with_fallback(
    const char *subsystem,
    const char *fallback_action
);

/**
 * @brief Queries whether Tier-0 Safe Mode is currently latched in idle AWAIT.
 *
 * @return true while awaiting fault repair and validated state restoration.
 */
bool debug_fallback_is_awaiting(void);

/**
 * @brief Clears Tier-0 Safe-Mode AWAIT state once a valid state has been restored.
 *
 * @param[in] resolution_note Human-readable recovery explanation (may be NULL).
 * @return true once Safe-Mode AWAIT is cleared cleanly.
 */
bool debug_fallback_clear_await(const char *resolution_note);

/**
 * @brief Explicitly initializes the debugger using ROM/monolith emergency defaults.
 *
 * @return true on Safe-Mode debugger initialization; false on critical failure.
 */
bool debug_init_emergency_monolith(void);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & Validation Gatekeeper
================================================================================
Mission: Verify incoming parameters, contracts, ROM assets, and fallback invariants.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwires & Audits
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight verification routine checking fallback parameter invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the emergency backup debugging settings before gameplay starts.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates raw template attributes, verifies snapshot buffer caps, and checks
 * loaded_from_data = false provenance contract cleanly without altering s_debug.
 * Preserves diag.subsystem == "core.util.debug_fallback".
 *
 * @return diag_result_t reporting SEVERITY_PASS or failure cause.
 */
diag_result_t debug_fallback_preflight(void);

/**
 * @brief Verification harness validating the static fallback code page and ROM vault.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated test confirming both the emergency defaults and all built-in ROM
 * debugger/error backups are undamaged.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes preflight diagnostics plus non-destructive master_loader_load_from_memory()
 * checks across all 9 embedded diagnostics/taxonomy ROM assets.
 *
 * @return true if all fallback and ROM invariants hold cleanly; false otherwise.
 */
bool debug_fallback_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & Validation Gatekeeper
================================================================================
Mission: Expose inspectable Safe-Mode telemetry objects, diagnostic ASCII cards,
and Tier-0 BIOS boot/shutdown lifecycle procedures.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

/**
 * @brief Populates caller ROM summary metadata from embedded diagnostics ROM.
 *
 * @param[out] out_info Destination ROM descriptor.
 * @return true on success; false on NULL argument.
 */
bool debug_fallback_get_rom_info(debug_fallback_rom_info_t *out_info);

/**
 * @brief Populates caller statistics report from Tier-0 Safe-Mode BIOS state.
 *
 * @param[out] out_stats Destination Safe-Mode telemetry struct.
 * @return true on success; false on NULL argument.
 */
bool debug_fallback_get_stats(debug_fallback_stats_t *out_stats);

/**
 * @brief Populates caller memory buffer with inspected fallback configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Inspects the emergency debugger settings stored in ROM or static code.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection procedure copying the compile-time fallback template.
 *
 * @param[out] out_config  Caller destination debug_config_t memory buffer.
 * @param[in]  config_size Size verification matching sizeof(debug_config_t).
 * @return true on successful population; false on NULL or invalid byte size.
 */
bool debug_fallback_execute_inspect(void *out_config, size_t config_size);

/**
 * @brief Formats a standardized fixed-column ASCII diagnostic card into out_buf.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a readable card displaying emergency debugger defaults, ROM status,
 * and whether Safe Mode is currently waiting in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded buffer formatter returning exact written character length (excluding NUL).
 *
 * @param[out] out_buf  Destination formatted string buffer.
 * @param[in]  buf_size Capacity of destination string buffer in bytes.
 * @return Number of bytes written excluding null terminator, or 0 on error.
 */
size_t debug_fallback_execute_dump_card(char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

/**
 * @brief Arms the Tier-0 Safe-Mode debugger BIOS with optional config or ROM defaults.
 *
 * @param[in] opt_config  Optional debug_config_t pointer (or NULL for ROM/monolith default).
 * @param[in] config_size Byte size of opt_config (ignored when opt_config is NULL).
 * @return true once Tier-0 Safe-Mode debugger BIOS is armed.
 */
bool debug_fallback_boot(const void *opt_config, size_t config_size);

/**
 * @brief Clears Tier-0 Safe-Mode BIOS state and fault latches cleanly.
 */
void debug_fallback_shutdown(void);

/**
 * @brief Queries whether Tier-0 Safe-Mode debugger BIOS is currently booted.
 *
 * @return true if debug_fallback_boot() is active.
 */
bool debug_fallback_is_booted(void);

#endif /* TENEBRIS_LUX_DEBUG_FALLBACK_H */
