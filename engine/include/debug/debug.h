/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          debug.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive Engine Debugger, State Rewind & Tripwire Inspector
@role          High-performance, zero-heap interactive state debugger, historical
               state snapshot ring buffer, time-travel rewind, dynamic breakpoints,
               non-crashing tripwire traps, and ANSI visual state inspection.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Pure Layer 0 independence: Decoupled from Layer 1 simulation structs.
               4. Ring buffer capacity bounded by DEBUG_MAX_SNAPSHOTS (64).
               5. State hashes are deterministic and bitwise reproducible (FNV-1a 32-bit).
               6. Non-crashing diagnostic tripwires with latching error states.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Interactive engine state debugger and diagnostic inspection pipeline for the
  Tenebris and Lux simulation runtime. Provides:
    1. State Snapshots: Bounded circular ring buffer in .bss capturing discrete turn records.
    2. Time-Travel Rewind: Deterministic historical state restoration and diffing.
    3. Breakpoints & Watchpoints: Dynamic halt conditions on turns, stances, or sway.
    4. Tripwire Traps: Latches anomalies without invoking crude OS abort() or crashing.
    5. Visual State Inspector: Formatted state visualizers, snapshot dumps, and ASCII cards.
  Integrates directly with data-driven configuration assets residing in
  engine/config/diagnostics/debugger/ (manifest.json, default.json, strict.json, trace.json,
  snapshot_policy.json, inspection_cards.json) with quarantined fallback protection.

  ### For Game Designers & Non-Programmers:
  Acts like a time-machine control panel for the game simulation. If a combat
  calculation or narrative event produces unexpected results, you can rewind
  the game state turn-by-turn to inspect exactly what changed.

  ### For Systems Programmers & Engine Architects:
  - 100% Zero-Heap Memory Model (ALLOC_ZERO_HEAP): All snapshots reside in static .bss.
  - Bitwise deterministic 32-bit FNV-1a state hashing to detect simulation drift.
  - Complete decoupling from Layer 1 structs: uses generic primitive state fields.
================================================================================
*/

#ifndef TENEBRIS_LUX_DEBUG_H
#define TENEBRIS_LUX_DEBUG_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
Mission: Pre-condition limits, downward include ladder, manifest anchors,
decoupled state representations, breakpoints, and tripwire helpers.
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
#include "debug/debug_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Telemetry Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing bounds for historical snapshots and dynamic breakpoints.
For Designers: Bounds ensure the rewind system remembers up to 64 combat turns
and allows setting up to 16 simultaneous condition triggers.
For Programmers: Guarantees 100% zero-heap allocation (ALLOC_ZERO_HEAP) in .bss.
--------------------------------------------------------------------------------
*/

/** Maximum historical turns retained in the in-memory circular ring buffer. */
#define DEBUG_MAX_SNAPSHOTS       64

/** Maximum simultaneous user and system breakpoints registered. */
#define DEBUG_MAX_BREAKPOINTS     16

/** Maximum length for entity, character, and card label names. */
#define DEBUG_MAX_NAME_LEN        32

/** Maximum payload length for debug status previews and formatted messages. */
#define DEBUG_MAX_MSG_LEN         256

/** Maximum buffer length for comma-delimited channel filter strings. */
#define DEBUG_MAX_CHANNELS_LEN    64

/** Default configuration profile name for standard test execution. */
#define DEBUG_DEFAULT_PROFILE     "default"

/** Canonical virtual filesystem path to the debugger master manifest. */
#define DEBUG_MANIFEST_PATH         "diagnostics/debugger/manifest.json"

/** Canonical virtual filesystem path to snapshot retention policy JSON. */
#define DEBUG_SNAPSHOT_POLICY_PATH  "diagnostics/debugger/snapshot_policy.json"

/** Canonical virtual filesystem path to taxonomic tripwire reaction matrix JSON. */
#define DEBUG_TRIPWIRE_MATRIX_PATH  "diagnostics/taxonomy/tripwire_matrix.json"

/** Maximum registered subsystem validation probes evaluated by the debugger. */
#define DEBUG_MAX_VALIDATION_PROBES 32U

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the engine's Loose-Bolts initialization DAG.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap Debugger Engine. */
#define DEBUG_MODULE_ID                "debug.engine"

/**
 * Manifest record for the Zero-Heap Debugger Engine.
 */
extern const module_metadata_t debug_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for state snapshots, step modes, breakpoints, and tripwires.
All structs and enums MUST have explicit non-anonymous tags for DWARF symbols.
--------------------------------------------------------------------------------
*/

/**
 * @enum debug_step_mode
 * @brief Execution stepping mode of the engine debugger.
 *
 * For Designers: Controls how the simulation advances:
 *   - RUN: Normal continuous play until something goes wrong.
 *   - OVER_TURN: Pauses at the end of each combat turn so you can inspect values.
 *   - INTO_COG: Micro-steps through individual calculations (damage, poise).
 *   - PAUSED: Frozen in place for interactive inspection.
 *   - AWAIT: Emergency idle halt entered after a tripwire completes safely in
 *     fallback mode; blocks RUN until the fault is repaired and a healthy state
 *     is restored.
 *
 * For Programmers: Execution cadence controller for simulation drivers.
 */
typedef enum debug_step_mode {
    DEBUG_STEP_RUN = 0,        /**< Continuous execution until breakpoint/tripwire */
    DEBUG_STEP_OVER_TURN = 1,  /**< Pause at turn boundaries */
    DEBUG_STEP_INTO_COG = 2,   /**< Micro-step individual cog sub-assemblies */
    DEBUG_STEP_PAUSED = 3,     /**< Engine is currently paused */
    DEBUG_STEP_AWAIT = 4       /**< Halted idle awaiting repair & state restore */
} debug_step_mode_t;

/**
 * @enum debug_breakpoint_type
 * @brief Breakpoint condition types.
 *
 * For Designers: Defines what triggers an automatic game pause:
 *   - TURN: Pauses when a specific turn number is reached.
 *   - STANCE: Pauses when a character shifts into a specific stance.
 *   - SWAY_RANGE: Pauses if moral balance swings past a chosen limit.
 *   - RESONANCE_BREACH: Pauses if weapon energy exceeds safety thresholds.
 *   - HP_THRESHOLD: Pauses if a unit's health drops too low.
 *   - TRIPWIRE: Pauses instantly if any diagnostic anomaly occurs.
 *
 * For Programmers: Evaluated in O(1) time at each simulation step boundary.
 */
typedef enum debug_breakpoint_type {
    DEBUG_BREAK_NONE = 0,
    DEBUG_BREAK_TURN,              /**< Break when turn equals target value */
    DEBUG_BREAK_STANCE,            /**< Break when stance equals target stance ID */
    DEBUG_BREAK_SWAY_RANGE,        /**< Break when |moral_sway| exceeds target */
    DEBUG_BREAK_RESONANCE_BREACH,  /**< Break when Lux or Tenebris exceeds target */
    DEBUG_BREAK_HP_THRESHOLD,      /**< Break when HP drops below target */
    DEBUG_BREAK_LOCATION,          /**< Break upon entering target location ID */
    DEBUG_BREAK_TRIPWIRE           /**< Break upon any tripwire trap */
} debug_breakpoint_type_t;

/* Note: debug_tripwire_t, debug_tripwire_reaction_t, and debug_test_tier_t
 * are defined canonically in debug/debug_fallback.h (Tier 3 include above). */

/**
 * @struct debug_state_fields
 * @brief Decoupled, generic state representation for Layer 0 snapshots.
 *
 * For Designers: A comprehensive snapshot of the hero's vitals, position,
 * weapon charge, stance, and world moral sway at a specific turn.
 *
 * For Programmers: Uses fundamental primitive types so Layer 0 never includes
 * Layer 1 game simulation headers, maintaining strict layer decoupling.
 */
typedef struct debug_state_fields {
    int turn;
    char hero_name[DEBUG_MAX_NAME_LEN];
    int hp;
    int max_hp;
    int mp;
    int tp;
    int shield_power;
    int sword_edge;
    int stance;
    int moral_sway;
    int location;
    int lux_barrio_turns;
    int charm_turns;
    int threat_hp;
    int foes_vanquished;
} debug_state_fields_t;

/**
 * @struct debug_snapshot
 * @brief Frozen state snapshot stored in the debugger circular ring buffer.
 *
 * For Designers: A single frame in the time-machine filmstrip holding all
 * numbers, the command that was executed, and the exact timestamp.
 *
 * For Programmers: Fixed-size record with 32-bit FNV-1a checksum to detect drift.
 */
typedef struct debug_snapshot {
    uint64_t snapshot_id;
    uint64_t timestamp_ms;
    debug_state_fields_t state;
    char last_command[64];
    char last_arg[64];
    uint32_t state_hash;
    uint64_t log_sequence_id;
    bool locked;               /**< Eviction guard enforced up to locked_slot_quota */
    char lock_reason[48];      /**< Reason snapshot was pinned (e.g., TRIPWIRE_LATCH) */
} debug_snapshot_t;

/**
 * @struct debug_breakpoint
 * @brief Dynamic breakpoint registration descriptor.
 *
 * For Designers: An active watchpoint trap configured to halt gameplay.
 *
 * For Programmers: Up to 16 active breakpoint slots stored in static .bss.
 */
typedef struct debug_breakpoint {
    int id;
    debug_breakpoint_type_t type;
    int target_value;
    bool is_active;
    char label[64];
} debug_breakpoint_t;

/**
 * @struct debug_config
 * @brief Debugger configuration parameters loaded from data or fallback.
 *
 * For Designers: Settings controlling whether the debugger is active, whether
 * errors pop up an interactive command prompt, and which events trigger halts.
 *
 * For Programmers: Ingests configuration from engine/config/diagnostics/debugger/.
 */
typedef struct debug_config {
    bool enabled;
    bool break_on_error;
    bool break_on_tripwire;
    bool break_on_sway_breach;
    bool break_on_resonance_breach;
    bool break_on_hp_zero;
    bool break_on_stance_change;
    size_t max_snapshots;
    int snapshot_interval_turns;
    bool auto_dump_on_tripwire;
    bool interactive_repl;
    bool ansi_color;
    char trace_channels[DEBUG_MAX_CHANNELS_LEN];
    char profile_name[32];
    bool loaded_from_data;
    bool engine_traps_active;      /**< Domain gate: Layer 0 infrastructure traps */
    bool game_traps_active;        /**< Domain gate: Layer 1 simulation traps */
    size_t locked_slot_quota;      /**< Locked critical slots in ring (default 4) */
    bool verify_on_step;           /**< Evaluate registered probes each step check */
    bool auto_await_on_tripwire;   /**< Enter DEBUG_STEP_AWAIT after fallback step */
} debug_config_t;

/**
 * @brief Validation probe callback signature returning Block 4 diag_result_t.
 */
typedef diag_result_t (*debug_validation_probe_fn)(void);

/**
 * @struct debug_validation_probe
 * @brief Registered Block 4 / Layer 4 fail-test trigger linked into tripwire monitoring.
 *
 * For Designers: Connects an automated safety check or test directly to the
 * debugger so any broken rule instantly stops playback in AWAIT rather than
 * continuing with corrupted numbers.
 *
 * For Programmers: Binds exact source function/block locations, Block 4
 * validate_fn callbacks, and Layer 4 test suite references into O(N) checks.
 */
typedef struct debug_validation_probe {
    char                      probe_id[48];       /**< Unique probe ID (e.g., "log.setting_missing") */
    char                      subsystem[32];      /**< Owning subsystem ID (e.g., "log.engine") */
    char                      source_section[64]; /**< Precise file/block/function trigger anchor */
    char                      test_suite_ref[64]; /**< Mapped Layer 4 test suite & section */
    debug_test_tier_t         tier;               /**< Verification stratum (AUDIT..SYSTEM) */
    debug_tripwire_t          tripwire_code;      /**< Categorized tripwire code latched on failure */
    debug_tripwire_reaction_t reaction;           /**< Configured reaction strategy */
    debug_validation_probe_fn validate_fn;        /**< Optional Block 4 validation probe callback */
    bool                      auto_await;         /**< True to pause in DEBUG_STEP_AWAIT after fallback */
    bool                      is_active;          /**< True if probe registration slot is live */
    uint32_t                  trigger_count;      /**< Lifetime number of times probe fired */
} debug_validation_probe_t;

/**
 * @struct debug_tripwire_report
 * @brief Forensic report capturing active tripwire, fallback completion, and AWAIT state.
 */
typedef struct debug_tripwire_report {
    debug_tripwire_t          tripwire_status;
    debug_tripwire_reaction_t reaction;
    debug_test_tier_t         tier;
    bool                      fallback_completed;
    bool                      await_active;
    bool                      snapshot_locked;
    uint64_t                  locked_snapshot_id;
    char                      probe_id[48];
    char                      subsystem[32];
    char                      condition[128];
    char                      source_location[96];
    char                      source_section[64];
    char                      test_suite_ref[64];
    char                      fallback_action[96];
} debug_tripwire_report_t;

/**
 * @struct debug_stats
 * @brief Aggregate runtime telemetry report constructed by Block 5 execution procedures.
 *
 * For Designers: A status scorecard showing total snapshots, active watchpoints,
 * Safe-Mode fallback status, and whether any tripwire alarms have paused in AWAIT.
 *
 * For Programmers: Telemetry structure constructed by debug_execute_inspect().
 */
typedef struct debug_stats {
    size_t snapshot_count;         /**< Number of active snapshots in ring buffer. */
    size_t max_snapshots;          /**< Configured snapshot capacity limit. */
    size_t breakpoint_count;       /**< Number of active breakpoints registered. */
    uint32_t traps_triggered;      /**< Number of tripwire traps triggered. */
    debug_step_mode_t step_mode;   /**< Active stepping execution mode. */
    bool is_active;                /**< true if debugger is initialized and active. */
    bool loaded_from_data;         /**< true if loaded from JSON profile. */
    char active_profile[32];       /**< Name of loaded profile. */
    size_t locked_snapshot_count;  /**< Number of locked critical snapshots in ring. */
    size_t registered_probe_count; /**< Active registered validation probes. */
    uint32_t probe_failures;       /**< Count of validation probe fail triggers. */
    bool safe_mode;                /**< true if operating via Safe-Mode fallback/ROM. */
    bool await_active;             /**< true if halted in DEBUG_STEP_AWAIT. */
    bool fallback_completed;       /**< true if fallback completed interrupted step. */
    debug_tripwire_t tripwire_status; /**< Active tripwire code. */
} debug_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an execution stepping mode is valid.
 *
 * @param[in] mode Stepping mode to check.
 * @return true if mode is within canonical bounds; false otherwise.
 */
static inline bool debug_step_mode_is_valid(debug_step_mode_t mode) {
    return mode >= DEBUG_STEP_RUN && mode <= DEBUG_STEP_AWAIT;
}

/**
 * @brief Predicate verifying if a breakpoint type is valid.
 *
 * @param[in] type Breakpoint type to check.
 * @return true if type is within canonical bounds; false otherwise.
 */
static inline bool debug_breakpoint_type_is_valid(debug_breakpoint_type_t type) {
    return type >= DEBUG_BREAK_NONE && type <= DEBUG_BREAK_TRIPWIRE;
}

/**
 * @brief Predicate verifying if the debugger tripwire is in a clear state.
 *
 * @param[in] wire Tripwire state to check.
 * @return true if wire is DEBUG_TRIPWIRE_CLEAR; false otherwise.
 */
static inline bool debug_tripwire_is_clear(debug_tripwire_t wire) {
    return wire == DEBUG_TRIPWIRE_CLEAR;
}

/**
 * @brief Fast inline conversion from stepping mode to human-readable string.
 *
 * @param[in] mode Stepping mode enum.
 * @return Static string constant naming the step mode.
 */
static inline const char *debug_step_mode_name(debug_step_mode_t mode) {
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
 * @brief Fast inline conversion from breakpoint type to human-readable string.
 *
 * @param[in] type Breakpoint type enum.
 * @return Static string constant naming the breakpoint condition.
 */
static inline const char *debug_breakpoint_type_name(debug_breakpoint_type_t type) {
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

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE DEBUGGER API & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Configuration & Profile Resolution Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Checks if the debugging system is currently initialized and active.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if debug mode and state rewind recording are currently turned on.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns internal boolean status flag s_initialized.
 */
bool debug_is_active(void);

/**
 * @brief Returns the name of the active debugging profile.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the name of the current debug profile (such as "default", "strict", or "trace").
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns pointer to static null-terminated profile name string.
 */
const char *debug_get_active_profile_name(void);

/**
 * @brief Returns the default configuration (data-driven or fallback).
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the standard starting settings for the debugger.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves manifest and returns default debug_config_t with fallback protection.
 */
debug_config_t debug_get_default_config(void);

/**
 * @brief Returns the quarantined emergency fallback configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Factory default settings used if debugger configuration files are missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Constructs static debug_config_t from .rodata with loaded_from_data == false.
 */
debug_config_t debug_get_monolith_fallback_config(void);

/**
 * @brief Loads a debug configuration from a manifest file.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads debugger rules from a specific manifest file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses manifest JSON, reads active preset, and hydrates out_config.
 */
bool debug_load_config_from_manifest(const char *manifest_path, debug_config_t *out_config);

/**
 * @brief Loads a specific named debug profile (e.g. "default", "strict", "trace").
 *
 * ### For Game Designers & Non-Programmers:
 * Selects a pre-tuned debugger behavior profile by name.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves <profile_name>.json across system search directories.
 */
bool debug_load_config_profile(const char *profile_name, debug_config_t *out_config);

/**
 * @brief Overlays snapshot retention policy rules onto a debugger configuration.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `DEBUG_SNAPSHOT_POLICY_PATH` via Layer 0 VFS (`path_finder`) and
 * falls back cleanly to embedded `.rodata` ROM if disk assets are absent.
 *
 * @param[in,out] cfg Configuration struct to enrich with ring & lock quotas.
 * @return true if loaded from disk VFS; false if satisfied via ROM/defaults.
 */
bool debug_load_snapshot_policy(debug_config_t *cfg);

/**
 * @brief Loads taxonomic tripwire reaction rules from disk VFS or ROM fallback.
 *
 * @param[out] out_rules Destination rule array.
 * @param[in]  max_rules Maximum rule capacity.
 * @param[out] out_count Number of parsed reaction rules.
 * @return true if loaded from disk VFS; false if satisfied via ROM fallback.
 */
bool debug_load_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Snapshots & Time-Travel Rewind Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Captures and records a state snapshot into the static ring buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Saves a freeze-frame picture of the hero's vitals, position, and moral balance
 * at a specific game turn so you can rewind back to it.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies fields into static circular ring buffer, computes FNV-1a hash, and advances head pointer.
 *
 * @param[in] fields State fields to capture.
 * @param[in] cmd Command string that caused this state transition.
 * @param[in] arg Argument string associated with the command.
 * @return Unique monotonic snapshot ID.
 */
uint64_t debug_record_snapshot(const debug_state_fields_t *fields,
                               const char *cmd, const char *arg);

/**
 * @brief Marks a recorded snapshot slot as locked against circular FIFO eviction.
 *
 * @param[in] snapshot_id Monotonic snapshot ID to pin (0 locks most recent).
 * @param[in] lock_reason Reason tag (e.g. `"TRIGGER_TRIPWIRE_LATCH"`).
 * @return true if snapshot was found and locked (respecting locked_slot_quota).
 */
bool debug_lock_snapshot(uint64_t snapshot_id, const char *lock_reason);

/**
 * @brief Returns the total number of valid snapshots currently recorded.
 *
 * ### For Game Designers & Non-Programmers:
 * Counts how many rewind points are currently available in memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns the count of valid elements in the ring buffer up to max_snapshots.
 */
size_t debug_get_snapshot_count(void);

/**
 * @brief Retrieves a historical snapshot by index (0 = oldest valid in buffer).
 *
 * ### For Game Designers & Non-Programmers:
 * Looks up a previous moment in time from the saved snapshot list.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reads entry from circular ring buffer at chronological offset index into out_snap.
 */
bool debug_get_snapshot(size_t index, debug_snapshot_t *out_snap);

/**
 * @brief Retrieves the most recent snapshot in the buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks the very latest recorded game state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns const pointer to entry at (s_head - 1) % capacity.
 */
const debug_snapshot_t *debug_get_latest_snapshot(void);

/**
 * @brief Restores state fields from a historical snapshot.
 *
 * ### For Game Designers & Non-Programmers:
 * Rewinds the game state back to an earlier turn or position.
 *
 * ### For Systems Programmers & Engine Architects:
 * Restores state fields structure from historical snapshot at index.
 *
 * @param[in] index Historical snapshot index to rewind to.
 * @param[out] out_fields Populated state fields upon successful rewind.
 * @return true if rewind succeeded, false if index invalid.
 */
bool debug_rewind_to_snapshot(size_t index, debug_state_fields_t *out_fields);

/**
 * @brief Computes a formatted textual difference between two snapshots.
 *
 * ### For Game Designers & Non-Programmers:
 * Compares two moments in time and prints out what changed (like HP lost or stance switched).
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats a multi-line ASCII diff between snapshot fields into out_buf.
 *
 * @param[in] s1 First snapshot (earlier in time).
 * @param[in] s2 Second snapshot (later in time).
 * @param[out] out_buf Destination text buffer.
 * @param[in] buf_len Size of destination buffer.
 * @return Number of bytes written to out_buf.
 */
size_t debug_diff_snapshots(const debug_snapshot_t *s1, const debug_snapshot_t *s2,
                            char *out_buf, size_t buf_len);

/**
 * @brief Computes a deterministic 32-bit FNV-1a hash of a state record.
 *
 * ### For Game Designers & Non-Programmers:
 * Creates a unique digital fingerprint of the game state to ensure simulation replay determinism.
 *
 * ### For Systems Programmers & Engine Architects:
 * Hashes all fields of debug_state_fields_t using FNV-1a for rapid state divergence detection.
 */
uint32_t debug_compute_state_hash(const debug_state_fields_t *fields);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Breakpoints, Watchpoints & Tripwire Traps Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Registers a dynamic breakpoint.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets an automatic pause condition (like pausing whenever the hero's health drops below 20%).
 *
 * ### For Systems Programmers & Engine Architects:
 * Appends breakpoint record to static table with type, target value, and label.
 *
 * @param[in] type Breakpoint type.
 * @param[in] target_value Target value to trigger on.
 * @param[in] label Descriptive label.
 * @return Breakpoint ID (positive integer) or -1 if capacity exceeded.
 */
int debug_add_breakpoint(debug_breakpoint_type_t type, int target_value, const char *label);

/**
 * @brief Removes an active breakpoint by ID.
 *
 * ### For Game Designers & Non-Programmers:
 * Removes a previously set pause trigger.
 *
 * ### For Systems Programmers & Engine Architects:
 * Deactivates breakpoint entry matching bp_id in static registry.
 */
bool debug_remove_breakpoint(int bp_id);

/**
 * @brief Clears all registered breakpoints.
 *
 * ### For Game Designers & Non-Programmers:
 * Clears all pause triggers at once.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets breakpoint counter and zeros all breakpoint slots.
 */
void debug_clear_breakpoints(void);

/**
 * @brief Evaluates active breakpoints against the current state.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether any pause triggers were hit on this turn.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates active breakpoint predicates against fields, formatting trigger reason into out_reason.
 *
 * @param[in] fields Current state fields.
 * @param[out] out_reason Buffer populated with breakpoint trigger reason.
 * @param[in] max_len Size of out_reason buffer.
 * @return true if a breakpoint triggered, false otherwise.
 */
bool debug_check_breakpoints(const debug_state_fields_t *fields,
                             char *out_reason, size_t max_len);

/**
 * @brief Traps a tripwire event in the debugger without crashing.
 *
 * ### For Game Designers & Non-Programmers:
 * Catches an engine warning or rule breach so the game can inspect it safely without crashing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Latches tripwire details into internal debugger diagnostic registers.
 */
void debug_trap_tripwire(const char *subsystem, const char *condition,
                         const char *file, int line);

/**
 * @brief Registers a subsystem Block 4 / Layer 4 validation probe with the debugger.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects an automated self-check test to the debugger so that if an anomaly
 * or failing rule occurs, the matching test fires immediately, Safe Mode
 * finishes the interrupted action safely, and playback halts in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stores probe metadata (exact source function/block section, Block 4 validate_fn,
 * and Layer 4 test suite reference) in the static s_debug.probes[] table.
 * Idempotent on identical `probe_id` (updates slot cleanly without leaking).
 *
 * @param[in] probe Populated validation probe descriptor.
 * @return Zero-based probe slot index on success, or -1 if capacity reached/invalid.
 */
int debug_register_validation_probe(const debug_validation_probe_t *probe);

/**
 * @brief Unregisters all active validation probes (used cleanly in isolated tests).
 */
void debug_clear_validation_probes(void);

/**
 * @brief Fires a registered validation probe (or fallback anomaly tripwire) immediately.
 *
 * ### For Systems Programmers & Engine Architects:
 * Workflow executed on invocation:
 *   1. Locates matching registered probe (`probe_id`), or synthesizes ad-hoc probe.
 *   2. Invokes `probe->validate_fn()` (guarded against recursion) when registered.
 *   3. Traps diagnostic details and pins an emergency locked snapshot when active.
 *   4. Downgrades to Safe-Mode fallback to finish the interrupted execution cleanly.
 *   5. Latches `DEBUG_STEP_AWAIT` (when active, `auto_await`, and `!in_probe`).
 *
 * @param[in] probe_id  Registered probe identifier (e.g., `"log.setting_missing"`).
 * @param[in] subsystem Subsystem raising the event.
 * @param[in] condition Detailed anomaly or failure reason.
 * @param[in] file      Source file anchor (`__FILE__`).
 * @param[in] line      Source line anchor (`__LINE__`).
 * @param[in] func      Source function anchor (`__func__`).
 * @return true if probe executed and completed fallback/await handling.
 */
bool debug_trigger_probe(
    const char *probe_id,
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *func
);

/**
 * @brief Evaluates all registered validation probes against live state invariants.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes every registered `validate_fn` hook in order. If any probe fails
 * (`!res.passed` or `SEVERITY_FATAL`/`SEVERITY_FAIL`), automatically fires
 * `debug_trigger_probe()` to downgrade via fallback, lock a forensic snapshot,
 * and pause in `DEBUG_STEP_AWAIT`.
 *
 * @param[in] fields Optional live state snapshot fields checked for NaN/bounds (may be NULL).
 * @return Number of probes that failed and triggered fallback -> AWAIT.
 */
size_t debug_check_tripwires(const debug_state_fields_t *fields);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Stepping & Execution Control Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the execution stepping mode.
 *
 * ### For Game Designers & Non-Programmers:
 * Controls playback speed (continuous run, pause, step forward, or AWAIT halt).
 * Note: While locked in AWAIT after an unresolved tripwire, continuous RUN and
 * stepping remain blocked until `debug_resolve_await()` confirms recovery.
 *
 * ### For Systems Programmers & Engine Architects:
 * Updates the active debug_step_mode_t state machine cleanly.
 */
void debug_set_step_mode(debug_step_mode_t mode);

/**
 * @brief Retrieves the active execution stepping mode.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether the game is paused, awaiting recovery, or running normally.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns current debug_step_mode_t enum value.
 */
debug_step_mode_t debug_get_step_mode(void);

/**
 * @brief Queries whether the debugger is currently halted idle in DEBUG_STEP_AWAIT.
 *
 * @return true if AWAIT latch is engaged.
 */
bool debug_is_awaiting(void);

/**
 * @brief Returns true if execution may advance (debugger inactive or not blocked in AWAIT).
 *
 * @return true if simulation stepping is permitted; false if blocked in PAUSED/AWAIT.
 */
bool debug_can_step(void);

/**
 * @brief Addresses an active AWAIT halt by validating repaired state and clearing latch.
 *
 * ### For Game Designers & Non-Programmers:
 * Exits emergency pause once you have rewound to a healthy turn or repaired
 * invalid numbers, verifying the repaired state passes validation before resuming.
 *
 * ### For Systems Programmers & Engine Architects:
 * Optionally validates `restored_fields` against state invariants (hp bounds,
 * sway range [-100, +100], shield/sword range [0, 100]), re-runs any failed
 * probe callback if clearable, resets AWAIT + tripwire latches, and restores
 * `resume_mode` cleanly (default `DEBUG_STEP_RUN` if valid).
 *
 * @param[in] restored_fields Optional repaired/rewound state to validate (may be NULL).
 * @param[in] resume_mode     Desired mode after clearing AWAIT.
 * @param[in] resolution_note Explanation logged to telemetry & Safe-Mode history.
 * @return true if validation succeeded and AWAIT cleared; false if state still corrupt.
 */
bool debug_resolve_await(
    const debug_state_fields_t *restored_fields,
    debug_step_mode_t           resume_mode,
    const char                 *resolution_note
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Inspection & State Dumps Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Prints an ANSI-colored or formatted diagnostic dump of the current state.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a colorful overview of the hero's status, stats, and moral alignment to the console.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats fields into formatted columns with ANSI terminal color highlights.
 */
void debug_dump_state(const debug_state_fields_t *fields);

/**
 * @brief Prints a tabular timeline of recorded snapshots.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a turn-by-turn history table showing past decisions and moral balance changes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Dumps chronological ASCII table of all ring buffer snapshots.
 */
void debug_dump_snapshot_history(void);

/**
 * @brief Retrieves the latest forensic tripwire, fallback, and AWAIT status report.
 *
 * @param[out] out_report Destination tripwire report struct.
 * @return true on success; false on NULL pointer.
 */
bool debug_get_tripwire_report(debug_tripwire_report_t *out_report);

/**
 * @brief Formats a human-readable forensic tripwire / AWAIT summary into out_buf.
 *
 * @param[out] out_buf Destination buffer.
 * @param[in]  buf_len Byte capacity of out_buf.
 * @return true if formatted without truncation; false otherwise.
 */
bool debug_dump_tripwire_report(char *out_buf, size_t buf_len);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
*/

/**
 * @brief Checks if active debugger configuration was loaded from data assets.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies if debugger settings were loaded from JSON files rather than emergency fallback code.
 *
 * ### For Systems Programmers & Engine Architects:
 * Invariant contract verification query returning loaded_from_data boolean flag.
 *
 * @return true if loaded from JSON profile; false if running on emergency fallback defaults.
 */
bool debug_is_data_driven(void);

/**
 * @brief Queries whether the debugger is currently running in Safe Mode / fallback.
 *
 * @return true if active without disk-loaded config (monolith or ROM Safe Mode).
 */
bool debug_is_safe_mode(void);

/**
 * @brief Returns the latched tripwire status.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if any tripwires are currently tripped.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns debug_tripwire_t enum status code.
 *
 * @return Active debug_tripwire_t status code indicating error state.
 */
debug_tripwire_t debug_get_tripwire_status(void);

/**
 * @brief Returns details of the latched tripwire.
 *
 * ### For Game Designers & Non-Programmers:
 * Explains what caused a tripwire warning.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns static string pointer to latched tripwire message.
 *
 * @return Constant string containing subsystem, condition, file, and line details.
 */
const char *debug_get_tripwire_details(void);

/**
 * @brief Resets the debugger tripwire latch.
 *
 * ### For Game Designers & Non-Programmers:
 * Clears any tripwire warnings once inspected.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets tripwire latch back to DEBUG_TRIPWIRE_CLEAR.
 */
void debug_reset_tripwire(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check for debugger subsystem invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs an instant health check on the state rewind and snapshot system before gameplay starts.
 *
 * ### For Systems Programmers & Engine Architects:
 * Asserts fallback integrity, FNV-1a hash determinism and collision sensitivity,
 * snapshot capacity bounds, and inspection procedure stability.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t debug_preflight(void);

/**
 * @brief High-level diagnostic self-audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Comprehensive automated inspection verifying that turns can be recorded, rewound,
 * and checked against pause conditions without data loss.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes preflight checks, tests snapshot recording and rewind restoration,
 * validates breakpoint registration and triggering, and verifies card formatting.
 *
 * @param[in] verbose Enable verbose trace output during audit.
 * @return diag_result_t diagnostic outcome indicating overall subsystem health.
 */
diag_result_t debug_audit(bool verbose);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs a structured runtime debugger statistics report.
 *
 * ### For Game Designers & Non-Programmers:
 * Collects a summary of the interactive debugger (active snapshots, memory
 * limit warnings, and current inspection mode) for display in menus or status bars.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection procedure populating a `debug_stats_t` record
 * from current ring pointers, tripwire registers, and snapshot indexes.
 *
 * @param[out] out_stats Destination statistics report object.
 * @return true if statistics were populated; false on NULL parameter.
 */
bool debug_execute_inspect(debug_stats_t *out_stats);

/**
 * @brief Formats an ASCII inspection card readout for the current simulation state.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean 60-column terminal status card showing hero stats, relic charge,
 * stance posture, or tripwire status during interactive debugging sessions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a deterministic ASCII inspection card into `out_buf` formatted to 60-column
 * terminal boundaries. Supports template identifiers such as "CARD_HERO_VITALS",
 * "CARD_STATERA_LEDGER", "CARD_COMBAT_ENGAGEMENT", "CARD_ENGINE_HEARTBEAT", and
 * "CARD_TRIPWIRE_DIAGNOSTIC".
 *
 * @param[in]  card_id Target template identifier (e.g. "CARD_HERO_VITALS").
 * @param[in]  fields  State fields to populate card with.
 * @param[out] out_buf Destination text buffer.
 * @param[in]  buf_len Size of destination buffer.
 * @return true if formatted successfully; false on invalid arguments or truncation.
 */
bool debug_execute_dump_card(const char *card_id, const debug_state_fields_t *fields,
                             char *out_buf, size_t buf_len);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the debugging system with a specific configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts up the diagnostic debugger at game boot so snapshots can be recorded,
 * rewind commands can be executed, and errors can be diagnosed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets up the snapshot ring buffer, resets sequence counters, and initializes
 * the tripwire matrix. If config is NULL, attempts data-driven load; falls back
 * gracefully to monolithic fallback if manifests are missing.
 *
 * @param[in] config Optional custom configuration, or NULL for data-driven default.
 * @return true on successful initialization, false on fatal failure.
 */
bool debug_init(const debug_config_t *config);

/**
 * @brief Shuts down the debugging system and resets state.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely turns off the diagnostic debugger and frees snapshot buffers when the game exits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears active snapshots, resets the ring buffer index, and marks the subsystem inactive.
 */
void debug_shutdown(void);

#endif /* TENEBRIS_LUX_DEBUG_H */
