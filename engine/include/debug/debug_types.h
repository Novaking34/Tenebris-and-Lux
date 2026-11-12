/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/debug/debug_types.h
@id            cog.debug.types
@layer         Layer 0: Core Utilities
@archetype     pure_types | stateless
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Engine Debugger Shared Vocabulary & Ontology
@role          Public Contract & Seam: Shared debugger capacities, tripwire/step
               enums, POD snapshot/probe/config types, and O(1) stringifiers.
@maturity      Rev 5 Baseline | Extracted cleanly from cog.debug so cog.debug.fallback compiles with typed APIs.
@chassis       zero-heap | Zero .bss/.data state; immutable bounds & caller-owned POD debugger types only.
@scale         O(1) enum validation & stringification; DEBUG_MAX_SNAPSHOTS (64), breakpoints (16), probes (32).
@seams         in: none (pure vocabulary) | uses: core/manifest.h | probes: none (stateless) | dag: core.util.debug_types
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Pure stateless types invariant: declares zero .bss/.data buffers or stateful functions.
               3. Standalone leaf contract eliminating `struct debug_config;` forward-decl & `void *` erasure.
               4. Non-anonymous DWARF tags preserved on all enums and structs with C++ extern "C" support.

PURPOSE & ARCHITECTURAL ROLE:
  Provides the shared, standalone Layer 0 debugger vocabulary for the
  Cornerstone Game Engine so both the operational debugger (`cog.debug`) and
  Tier-0 Safe-Mode BIOS (`cog.debug.fallback`) accept strongly typed
  `debug_config_t *` and shared tripwire/probe descriptors cleanly.

  ### For Game Designers & Non-Programmers:
  Defines the common language of debugger safety switches (step modes, alarm
  tripwire codes, recovery actions, test tiers, and snapshot fields) shared by
  diagnostic cards, validation probes, and Safe-Mode backups.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Pure caller-owned POD structs and function-pointer
    typedefs (`debug_validation_probe_fn`); stateless and zero-heap.
  - Extension Seam: Add debugger config flags, tripwire codes, or telemetry
    slots once here with immediate visibility across operational and BIOS layers.
  - Explicit Non-Goal: Holds zero snapshot rings, fault latches, or Layer 1
    game simulation headers (`hero/`, `combat/`, `world/`, `relics/`, `ending/`).
================================================================================
*/

#ifndef CORNERSTONE_DEBUG_DEBUG_TYPES_H
#define CORNERSTONE_DEBUG_DEBUG_TYPES_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Shared Pure Structural Vocabulary, Value Bounds & Stateless Predicates )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug.types
Mission:       Provide shared debugger capacities, tagged types, and O(1) helpers.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers (Minimal Pure Roots Only) --- */
#include "core/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Physical Capacities & Compile-Time Proofs
--------------------------------------------------------------------------------
Shared compile-time array limits governing snapshot ring capacity, watchpoints,
channel strings, and registered subsystem validation probe slots.

### For Game Designers & Non-Programmers:
Determines how many combat turns stay in rewind memory (64), how many custom
watchpoints can run simultaneously (16), and max registered probes (32).

### For Systems Programmers & Engine Architects:
Enforces zero-heap `.bss` footprint bounds at compile time via `static_assert`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Maximum historical turns retained in the in-memory circular ring buffer.
 * Fixed to 64 slots so rewind state remains cleanly bounded in `.bss`.
 */
#define DEBUG_MAX_SNAPSHOTS         64

/**
 * @brief Maximum simultaneous user and system breakpoints registered.
 * Caps breakpoint scan overhead to at most 16 fixed slots per simulation turn.
 */
#define DEBUG_MAX_BREAKPOINTS       16

/**
 * @brief Maximum length in bytes for entity, character, and diagnostic card labels.
 */
#define DEBUG_MAX_NAME_LEN          32

/**
 * @brief Maximum payload length in bytes for debug status previews and diagnostics.
 */
#define DEBUG_MAX_MSG_LEN           256

/**
 * @brief Maximum buffer length in bytes for comma-delimited channel filter lists.
 */
#define DEBUG_MAX_CHANNELS_LEN      64

/**
 * @brief Maximum registered subsystem validation probes evaluated by the debugger.
 */
#define DEBUG_MAX_VALIDATION_PROBES 32U

static_assert(
    DEBUG_MAX_SNAPSHOTS > 0 && DEBUG_MAX_BREAKPOINTS > 0,
    "Snapshot ring and breakpoint capacities must remain strictly positive"
);
static_assert(
    DEBUG_MAX_VALIDATION_PROBES > 0U && DEBUG_MAX_MSG_LEN >= 128,
    "Validation probe slots and diagnostic message bounds must remain viable"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Stateless Vocabulary Module Identifier
--------------------------------------------------------------------------------
Pure types header identity anchor; carries no link-time runtime manifest object.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Debugger Shared Vocabulary header. */
#define DEBUG_TYPES_MODULE_ID       "debug.types"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Shared Structural Ontology & POD Debugger Types
--------------------------------------------------------------------------------
Canonical non-anonymous enums and structures shared identically between
`cog.debug` and quarantined `cog.debug.fallback`.
--------------------------------------------------------------------------------
*/

/**
 * @enum debug_tripwire
 * @brief Latched diagnostic tripwire states for the debugger and validation pipeline.
 *
 * ### For Game Designers & Non-Programmers:
 * Alarm switches indicating whether configuration data was missing or damaged,
 * an automated safety test fired, or Safe Mode engaged after a fault.
 *
 * ### For Systems Programmers & Engine Architects:
 * Latching error enum preserving `0..4` legacy discriminants and `5..7` active
 * validation probe, audit breach, and Safe-Mode downgrade discriminants.
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
 * ### For Game Designers & Non-Programmers:
 * Selects how the engine reacts when an alarm trips: logging warnings, clamping
 * bad numbers, dropping into Safe Mode, rewinding time, or halting in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Mapped 1:1 to `tripwire_matrix.json` canonical reaction identifier strings.
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
 * ### For Game Designers & Non-Programmers:
 * Identifies which inspection level (in-file self-check or one of the four
 * external verification suites) caught an anomaly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Binds registered `debug_validation_probe_t` entries cleanly to Block 4 audits
 * and Layer 4 `Unit`, `Functional`, `Integrated`, and `System` suites.
 */
typedef enum debug_test_tier {
    DEBUG_TEST_TIER_AUDIT = 0,     /**< In-file Block 4 preflight / validation cog. */
    DEBUG_TEST_TIER_UNIT = 1,      /**< Layer 4 Unit isolation test suite. */
    DEBUG_TEST_TIER_FUNCTIONAL,    /**< Layer 4 Functional cog verification suite. */
    DEBUG_TEST_TIER_INTEGRATED,    /**< Layer 4 Cross-subsystem integrated suite. */
    DEBUG_TEST_TIER_SYSTEM         /**< Layer 4 End-to-end system stress & soak suite. */
} debug_test_tier_t;

/**
 * @enum debug_step_mode
 * @brief Execution stepping mode of the engine debugger.
 *
 * ### For Game Designers & Non-Programmers:
 * Controls how simulation steps advance (`RUN`, `OVER_TURN`, `INTO_COG`,
 * `PAUSED`, or emergency `AWAIT` wait state after Safe-Mode recovery).
 *
 * ### For Systems Programmers & Engine Architects:
 * Execution cadence discriminator driving loop gating and tripwire freezes.
 */
typedef enum debug_step_mode {
    DEBUG_STEP_RUN = 0,        /**< Continuous execution until breakpoint/tripwire. */
    DEBUG_STEP_OVER_TURN = 1,  /**< Pause cleanly at turn boundaries. */
    DEBUG_STEP_INTO_COG = 2,   /**< Micro-step individual cog sub-assemblies. */
    DEBUG_STEP_PAUSED = 3,     /**< Simulation paused for interactive inspection. */
    DEBUG_STEP_AWAIT = 4       /**< Halted idle awaiting repair and healthy restore. */
} debug_step_mode_t;

/**
 * @enum debug_breakpoint_type
 * @brief Watchpoint trigger condition taxonomy for simulation breaks.
 *
 * ### For Game Designers & Non-Programmers:
 * Chooses what pauses gameplay automatically: specific turns, stance shifts,
 * moral sway swings, weapon resonance limits, low HP, locations, or tripwires.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluated in O(1) per breakpoint slot at each simulation step boundary.
 */
typedef enum debug_breakpoint_type {
    DEBUG_BREAK_NONE = 0,          /**< Disabled / unused breakpoint slot. */
    DEBUG_BREAK_TURN,              /**< Break when turn counter matches target value. */
    DEBUG_BREAK_STANCE,            /**< Break when actor enters target stance ID. */
    DEBUG_BREAK_SWAY_RANGE,        /**< Break when |moral_sway| exceeds threshold. */
    DEBUG_BREAK_RESONANCE_BREACH,  /**< Break when primary gauge exceeds threshold. */
    DEBUG_BREAK_HP_THRESHOLD,      /**< Break when HP falls at or below target. */
    DEBUG_BREAK_LOCATION,          /**< Break upon entering target zone/tile ID. */
    DEBUG_BREAK_TRIPWIRE           /**< Break whenever any diagnostic tripwire trips. */
} debug_breakpoint_type_t;

/**
 * @struct debug_state_fields
 * @brief Decoupled, generic state representation for Layer 0 snapshots.
 *
 * ### For Game Designers & Non-Programmers:
 * Captures an actor's generic vitals, posture, scalar balance, location index,
 * active turn timers, and threat readout inside the time-machine filmstrip.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure integer/char POD slot layout guaranteeing zero Layer 1 simulation headers
 * ever leak into `engine/`.
 */
typedef struct debug_state_fields {
    int  turn;                            /**< Active simulation turn index. */
    char hero_name[DEBUG_MAX_NAME_LEN];   /**< Generic active actor label slot. */
    int  hp;                              /**< Current health points slot. */
    int  max_hp;                          /**< Maximum health points capacity slot. */
    int  mp;                              /**< Secondary energy pool slot. */
    int  tp;                              /**< Tactical technique points slot. */
    int  shield_power;                    /**< Defensive artifact charge gauge slot. */
    int  sword_edge;                      /**< Offensive artifact charge gauge slot. */
    int  stance;                          /**< Active numerical posture/stance slot. */
    int  moral_sway;                      /**< Signed alignment/balance scalar slot. */
    int  location;                        /**< Discrete world zone/location identifier. */
    int  lux_barrio_turns;                /**< Protective ward duration counter slot. */
    int  charm_turns;                     /**< Status modifier duration counter slot. */
    int  threat_hp;                       /**< Opposing active target HP readout slot. */
    int  foes_vanquished;                 /**< Cumulative defeated opposition counter. */
} debug_state_fields_t;

/**
 * @struct debug_snapshot
 * @brief Frozen state snapshot stored in the debugger circular ring buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * One time-machine frame recording the exact turn values, command typed, log
 * line link, and whether the frame is pinned so later turns cannot overwrite it.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed POD record verified by 32-bit FNV-1a checksum (`state_hash`) across
 * rewind and determinism comparisons.
 */
typedef struct debug_snapshot {
    uint64_t             snapshot_id;      /**< Monotonic snapshot sequence number. */
    uint64_t             timestamp_ms;     /**< Capture timestamp in milliseconds. */
    debug_state_fields_t state;            /**< Decoupled state scalar payload. */
    char                 last_command[64]; /**< Canonical command verb executed. */
    char                 last_arg[64];     /**< Optional command argument string. */
    uint32_t             state_hash;       /**< 32-bit FNV-1a deterministic hash. */
    uint64_t             log_sequence_id;  /**< Correlated flight recorder sequence. */
    bool                 locked;           /**< Eviction guard enforced up to quota. */
    char                 lock_reason[48];  /**< Human/probe lock tag (e.g. TRIPWIRE_LATCH). */
} debug_snapshot_t;

/**
 * @struct debug_breakpoint
 * @brief Dynamic watchpoint registration descriptor.
 *
 * ### For Game Designers & Non-Programmers:
 * Holds one active debug watchpoint rule with its trigger type, target value,
 * and readable designer label.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed-width slot entry stored inside `debug_internal_state_t.breakpoints[]`.
 */
typedef struct debug_breakpoint {
    int                     id;           /**< Assigned positive breakpoint handle ID. */
    debug_breakpoint_type_t type;         /**< Watchpoint trigger discriminator. */
    int                     target_value; /**< Numeric threshold or target ID. */
    bool                    is_active;    /**< True while watchpoint is armed. */
    char                    label[64];    /**< Descriptive breakpoint tag. */
} debug_breakpoint_t;

/**
 * @struct debug_config
 * @brief Debugger configuration parameters loaded from data or BIOS fallback.
 *
 * ### For Game Designers & Non-Programmers:
 * Settings controlling rewind buffer length, watchpoint triggers, inspection
 * card auto-dumps, interactive console behavior, and Safe-Mode AWAIT pauses.
 *
 * ### For Systems Programmers & Engine Architects:
 * Shared cleanly between `cog.debug` and `cog.debug.fallback` with explicit
 * `loaded_from_data` provenance tracking (`false` for ROM/monolith defaults).
 */
typedef struct debug_config {
    bool   enabled;                         /**< Master debugger enable switch. */
    bool   break_on_error;                  /**< Break upon logged ERROR severity. */
    bool   break_on_tripwire;               /**< Break upon diagnostic tripwire latch. */
    bool   break_on_sway_breach;            /**< Break upon alignment bound breach. */
    bool   break_on_resonance_breach;       /**< Break upon artifact gauge overflow. */
    bool   break_on_hp_zero;                /**< Break when primary HP reaches zero. */
    bool   break_on_stance_change;          /**< Break upon actor posture transitions. */
    size_t max_snapshots;                   /**< Active rewind cap (<= DEBUG_MAX_SNAPSHOTS). */
    int    snapshot_interval_turns;         /**< Automatic snapshot cadence in turns. */
    bool   auto_dump_on_tripwire;           /**< Auto-format state card on tripwire. */
    bool   interactive_repl;                /**< Enable interactive terminal pause prompt. */
    bool   ansi_color;                      /**< Enable ANSI color styling on cards. */
    char   trace_channels[DEBUG_MAX_CHANNELS_LEN]; /**< Enabled channel filter CSV. */
    char   profile_name[32];                /**< Active configuration preset identifier. */
    bool   loaded_from_data;                /**< True only when loaded from disk VFS JSON. */
    bool   engine_traps_active;             /**< Gate for Layer 0 infrastructure traps. */
    bool   game_traps_active;               /**< Gate for Layer 1 simulation traps. */
    size_t locked_slot_quota;               /**< Reserved pinned critical slots in ring. */
    bool   verify_on_step;                  /**< Evaluate probes on each step check. */
    bool   auto_await_on_tripwire;          /**< Enter DEBUG_STEP_AWAIT after fallback step. */
} debug_config_t;

/**
 * @brief Validation probe callback signature returning a Block 4 `diag_result_t`.
 */
typedef diag_result_t (*debug_validation_probe_fn)(void);

/**
 * @struct debug_validation_probe
 * @brief Registered Block 4 / Layer 4 fail-test trigger linked into tripwire monitoring.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects an automated safety check or verification rule directly to the
 * debugger so any violation stops execution in AWAIT instead of silently drifting.
 *
 * ### For Systems Programmers & Engine Architects:
 * Binds source location tags, optional callback function pointers, and Layer 4
 * test suite references inside a 32-slot zero-heap table.
 */
typedef struct debug_validation_probe {
    char                      probe_id[48];       /**< Unique probe ID (e.g. "io.loader.envelope"). */
    char                      subsystem[32];      /**< Owning subsystem ID (e.g. "core.io.master_loader"). */
    char                      source_section[64]; /**< File/block/function trigger anchor. */
    char                      test_suite_ref[64]; /**< Mapped Layer 4 test suite & section. */
    debug_test_tier_t         tier;               /**< Verification tier (AUDIT..SYSTEM). */
    debug_tripwire_t          tripwire_code;      /**< Categorized tripwire latched on fail. */
    debug_tripwire_reaction_t reaction;           /**< Configured recovery reaction policy. */
    debug_validation_probe_fn validate_fn;        /**< Optional Block 4 validation probe callback. */
    bool                      auto_await;         /**< True to pause in AWAIT after Safe-Mode step. */
    bool                      is_active;          /**< True if probe registration slot is live. */
    uint32_t                  trigger_count;      /**< Lifetime count of triggered firings. */
} debug_validation_probe_t;

/**
 * @struct debug_tripwire_report
 * @brief Forensic report capturing active tripwire, fallback completion, and AWAIT state.
 *
 * ### For Game Designers & Non-Programmers:
 * Full emergency incident ticket explaining which probe fired, which snapshot
 * got locked, what Safe Mode did, and whether the engine is paused in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed-layout diagnostic report populated cleanly by `debug_trigger_probe()`.
 */
typedef struct debug_tripwire_report {
    debug_tripwire_t          tripwire_status;    /**< Categorized latched tripwire code. */
    debug_tripwire_reaction_t reaction;           /**< Selected reaction policy. */
    debug_test_tier_t         tier;               /**< Verification tier of firing probe. */
    bool                      fallback_completed; /**< True once Safe Mode finished step. */
    bool                      await_active;       /**< True while halted in DEBUG_STEP_AWAIT. */
    bool                      snapshot_locked;    /**< True if incident snapshot stayed pinned. */
    uint64_t                  locked_snapshot_id; /**< ID of pinned forensic snapshot. */
    char                      probe_id[48];       /**< Probe identifier that raised alarm. */
    char                      subsystem[32];      /**< Owning subsystem identifier. */
    char                      condition[128];     /**< Failure condition summary text. */
    char                      source_location[96];/**< Formatted `file:line` anchor. */
    char                      source_section[64]; /**< Source Block/function section anchor. */
    char                      test_suite_ref[64]; /**< Linked Layer 4 test reference. */
    char                      fallback_action[96];/**< Summary of Safe-Mode recovery action. */
} debug_tripwire_report_t;

/**
 * @struct debug_stats
 * @brief Aggregate runtime telemetry report constructed by Block 5 inspection procedures.
 *
 * ### For Game Designers & Non-Programmers:
 * Status scorecard showing rewind frame usage, locked emergency slots, probe
 * failure counts, Safe-Mode status, and whether playback is paused in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap runtime snapshot copied from internal debugger state cleanly.
 */
typedef struct debug_stats {
    size_t            snapshot_count;         /**< Active snapshots stored in ring buffer. */
    size_t            max_snapshots;          /**< Configured snapshot ring capacity. */
    size_t            breakpoint_count;       /**< Count of armed watchpoint slots. */
    uint32_t          traps_triggered;        /**< Lifetime debugger trap activations. */
    debug_step_mode_t step_mode;              /**< Active stepping execution mode. */
    bool              is_active;              /**< True if primary debugger is initialized. */
    bool              loaded_from_data;       /**< True if active config came from disk JSON. */
    char              active_profile[32];     /**< Active configuration preset identifier. */
    size_t            locked_snapshot_count;  /**< Pinned critical snapshots in ring. */
    size_t            registered_probe_count; /**< Active registered validation probes. */
    uint32_t          probe_failures;         /**< Total validation probe fail triggers. */
    bool              safe_mode;              /**< True if operating via Safe-Mode fallback. */
    bool              await_active;           /**< True while halted in DEBUG_STEP_AWAIT. */
    bool              fallback_completed;     /**< True if Safe Mode completed halted step. */
    debug_tripwire_t  tripwire_status;        /**< Active debugger tripwire latch code. */
} debug_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Stateless Enum Stringifiers
--------------------------------------------------------------------------------
Pure O(1) inline validation predicates and canonical enum-to-string mappers
available cleanly to both C11 and C++ consumers.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying whether an execution stepping mode is valid.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures debugger cadence switches (`RUN`, `PAUSED`, `AWAIT`, etc.) stay valid.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) bounded enum check across [DEBUG_STEP_RUN .. DEBUG_STEP_AWAIT].
 *
 * @param[in] mode Stepping mode enumerator to inspect.
 * @return true if within canonical range; false otherwise.
 */
static inline bool debug_step_mode_is_valid(debug_step_mode_t mode)
{
    return (int)mode >= (int)DEBUG_STEP_RUN && (int)mode <= (int)DEBUG_STEP_AWAIT;
}

/**
 * @brief Predicate verifying whether a breakpoint trigger type is valid.
 *
 * ### For Game Designers & Non-Programmers:
 * Rejects uninitialized or out-of-range breakpoint condition codes.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) bounded enum check across [DEBUG_BREAK_NONE .. DEBUG_BREAK_TRIPWIRE].
 *
 * @param[in] type Breakpoint type enumerator to inspect.
 * @return true if within canonical range; false otherwise.
 */
static inline bool debug_breakpoint_type_is_valid(debug_breakpoint_type_t type)
{
    return (int)type >= (int)DEBUG_BREAK_NONE && (int)type <= (int)DEBUG_BREAK_TRIPWIRE;
}

/**
 * @brief Predicate verifying whether a debugger tripwire latch is clear.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms no safety alarms are currently tripped.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) comparison against `DEBUG_TRIPWIRE_CLEAR`.
 *
 * @param[in] wire Tripwire enumerator to inspect.
 * @return true if `DEBUG_TRIPWIRE_CLEAR`; false otherwise.
 */
static inline bool debug_tripwire_is_clear(debug_tripwire_t wire)
{
    return wire == DEBUG_TRIPWIRE_CLEAR;
}

/**
 * @brief Returns canonical static label naming a `debug_step_mode_t` value.
 *
 * @param[in] mode Stepping mode enumerator.
 * @return Null-terminated static `.rodata` string constant.
 */
static inline const char *debug_step_mode_name(debug_step_mode_t mode)
{
    switch (mode) {
        case DEBUG_STEP_RUN:       return "RUN";
        case DEBUG_STEP_OVER_TURN: return "OVER_TURN";
        case DEBUG_STEP_INTO_COG:  return "INTO_COG";
        case DEBUG_STEP_PAUSED:    return "PAUSED";
        case DEBUG_STEP_AWAIT:     return "AWAIT";
        default:                   return "UNKNOWN";
    }
}

/**
 * @brief Returns canonical static label naming a `debug_breakpoint_type_t` value.
 *
 * @param[in] type Breakpoint type enumerator.
 * @return Null-terminated static `.rodata` string constant.
 */
static inline const char *debug_breakpoint_type_name(debug_breakpoint_type_t type)
{
    switch (type) {
        case DEBUG_BREAK_NONE:             return "NONE";
        case DEBUG_BREAK_TURN:             return "TURN";
        case DEBUG_BREAK_STANCE:           return "STANCE";
        case DEBUG_BREAK_SWAY_RANGE:       return "SWAY_RANGE";
        case DEBUG_BREAK_RESONANCE_BREACH: return "RESONANCE_BREACH";
        case DEBUG_BREAK_HP_THRESHOLD:     return "HP_THRESHOLD";
        case DEBUG_BREAK_LOCATION:         return "LOCATION";
        case DEBUG_BREAK_TRIPWIRE:         return "TRIPWIRE";
        default:                           return "UNKNOWN";
    }
}

/**
 * @brief Returns canonical static label naming a `debug_tripwire_t` enumerator.
 *
 * @param[in] wire Tripwire enumerator.
 * @return Null-terminated static `.rodata` identifier string.
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
 * @brief Returns canonical static label naming a `debug_tripwire_reaction_t` enumerator.
 *
 * @param[in] reaction Reaction enumerator.
 * @return Null-terminated static `.rodata` identifier string.
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
 * @brief Returns canonical static label naming a `debug_test_tier_t` enumerator.
 *
 * @param[in] tier Verification tier enumerator.
 * @return Null-terminated static `.rodata` identifier string.
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
BLOCK 3: BODY BLOCK
( Statutory Pure-Types Notice & Structural Boundary )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug.types
Mission:       Note: This header is strictly intended to define purely static structures
               and inline predicates shared cleanly between cog.debug and cog.debug.fallback;
               runtime stepping, probe registry, and BIOS APIs live in primary headers.
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Debugger Shared Vocabulary
================================================================================
Note: Functional audits, layout tripwires, and pre-flight routines are declared in
`debug/debug_fallback.h` (`debug_fallback_preflight()`) and `debug/debug.h`
(`debug_preflight()`).
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK / STATUTORY EXEMPTION & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Debugger Shared Vocabulary
================================================================================
Note: Operational snapshot capture, subsystem lifecycle initialization, and
diagnostic dump cards are declared in `debug/debug.h` and `debug/debug_fallback.h`.
Stateless Exemption Notice: None. (Stateless module / pure stack-allocated execution).
================================================================================
*/

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_DEBUG_DEBUG_TYPES_H */

