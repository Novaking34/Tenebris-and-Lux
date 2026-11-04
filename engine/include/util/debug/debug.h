// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          debug.h
// @id            core.util.debug
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-Heap Interactive Engine Debugger, State Rewind, and Tripwire Inspector.
//   Provides a cohesive debugging facility for Tenebris and Lux:
//     1. State Snapshots: Bounded static ring buffer capturing discrete state records.
//     2. Time-Travel Rewind: Deterministic historical state restoration and diffing.
//     3. Breakpoints & Watchpoints: Dynamic halt conditions on turns, stances, or sway.
//     4. Tripwire Traps: Latches anomalies without invoking OS traps or crashing.
//     5. Visual State Inspector: Formatted state visualizers and snapshot dumps.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.manifest, core.util.string, core.util.data,
//                            core.util.time, core.util.log, core.util.debug_fallback
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Pure static ring buffer, zero heap).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential main loop debugger).
//   - Latency Budget:        < 15 microseconds per snapshot record or inspection.
//
// AXIOMATIC INVARIANTS:
//   1. Zero dynamic heap memory allocations across all debugger routines.
//   2. Pure Layer 0 independence: Decoupled from Layer 1 simulation structs.
//   3. Ring buffer capacity bounded by DEBUG_MAX_SNAPSHOTS (64).
//   4. State hashes are deterministic and bitwise reproducible.
// =============================================================================

#ifndef TL_UTIL_DEBUG_H
#define TL_UTIL_DEBUG_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// =============================================================================

#include "util/manifest.h"
#include "util/debug/debug_fallback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEBUG_MAX_SNAPSHOTS       64
#define DEBUG_MAX_BREAKPOINTS     16
#define DEBUG_MAX_NAME_LEN        32
#define DEBUG_MAX_MSG_LEN         256
#define DEBUG_MAX_CHANNELS_LEN    64
#define DEBUG_DEFAULT_PROFILE     "data/debug/default.json"
#define DEBUG_MANIFEST_PATH       "data/debug/manifest.json"

/**
 * @brief Execution stepping mode of the engine debugger.
 */
typedef enum {
    DEBUG_STEP_RUN = 0,       /**< Continuous execution until breakpoint/tripwire */
    DEBUG_STEP_OVER_TURN = 1,  /**< Pause at turn boundaries */
    DEBUG_STEP_INTO_COG = 2,   /**< Micro-step individual cog sub-assemblies */
    DEBUG_STEP_PAUSED = 3      /**< Engine is currently paused */
} debug_step_mode_t;

/**
 * @brief Breakpoint condition types.
 */
typedef enum {
    DEBUG_BREAK_NONE = 0,
    DEBUG_BREAK_TURN,              /**< Break when turn equals target value */
    DEBUG_BREAK_STANCE,            /**< Break when stance equals target stance ID */
    DEBUG_BREAK_SWAY_RANGE,        /**< Break when |moral_sway| exceeds target */
    DEBUG_BREAK_RESONANCE_BREACH,  /**< Break when Lux or Tenebris exceeds target */
    DEBUG_BREAK_HP_THRESHOLD,      /**< Break when HP drops below target */
    DEBUG_BREAK_LOCATION,          /**< Break upon entering target location ID */
    DEBUG_BREAK_TRIPWIRE           /**< Break upon any tripwire trap */
} debug_breakpoint_type_t;

/**
 * @brief Latched diagnostic tripwire states for the debugger.
 */
typedef enum {
    DEBUG_TRIPWIRE_CLEAR = 0,
    DEBUG_TRIPWIRE_MANIFEST_MISSING,
    DEBUG_TRIPWIRE_SETTING_MISSING,
    DEBUG_TRIPWIRE_STATE_CORRUPT,
    DEBUG_TRIPWIRE_HASH_MISMATCH
} debug_tripwire_t;

/**
 * @brief Decoupled, generic state representation for Layer 0 snapshots.
 *
 * This struct uses fundamental integer and string types so that Layer 0
 * does NOT depend on Layer 1 game state structs.
 */
typedef struct {
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
 * @brief Frozen state snapshot in the debugger ring buffer.
 */
typedef struct {
    uint64_t snapshot_id;
    uint64_t timestamp_ms;
    debug_state_fields_t state;
    char last_command[64];
    char last_arg[64];
    uint32_t state_hash;
    uint64_t log_sequence_id;
} debug_snapshot_t;

/**
 * @brief Dynamic breakpoint registration descriptor.
 */
typedef struct {
    int id;
    debug_breakpoint_type_t type;
    int target_value;
    bool is_active;
    char label[64];
} debug_breakpoint_t;

/**
 * @brief Debugger configuration parameters loaded from data or fallback.
 */
typedef struct debug_config_s {
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
} debug_config_t;

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE DEBUGGER API
// =============================================================================

/* --- Lifecycle & Configuration --- */

/**
 * @brief Initializes the debugging system with a specific configuration.
 *
 * If config is NULL, attempts to load default profile from data/debug/manifest.json.
 * If data loading fails, gracefully activates the monolith fallback.
 *
 * @param[in] config Optional custom configuration, or NULL for data-driven default.
 * @return true on successful initialization, false on fatal failure.
 */
bool debug_init(const debug_config_t *config);

/**
 * @brief Shuts down the debugging system and resets state.
 */
void debug_shutdown(void);

/**
 * @brief Checks if the debugging system is currently initialized and active.
 */
bool debug_is_active(void);

/**
 * @brief Checks if active debugger configuration was loaded from data assets.
 */
bool debug_is_data_driven(void);

/**
 * @brief Returns the name of the active debugging profile.
 */
const char *debug_get_active_profile_name(void);

/**
 * @brief Returns the default configuration (data-driven or fallback).
 */
debug_config_t debug_get_default_config(void);

/**
 * @brief Returns the quarantined emergency fallback configuration.
 */
debug_config_t debug_get_monolith_fallback_config(void);

/**
 * @brief Loads a debug configuration from a manifest file.
 */
bool debug_load_config_from_manifest(const char *manifest_path, debug_config_t *out_config);

/**
 * @brief Loads a specific named debug profile (e.g. "default", "strict", "trace").
 */
bool debug_load_config_profile(const char *profile_name, debug_config_t *out_config);

/* --- Snapshots & Time-Travel Rewind --- */

/**
 * @brief Captures and records a state snapshot into the static ring buffer.
 *
 * @param[in] fields State fields to capture.
 * @param[in] cmd Command string that caused this state transition.
 * @param[in] arg Argument string associated with the command.
 * @return Unique monotonic snapshot ID.
 */
uint64_t debug_record_snapshot(const debug_state_fields_t *fields,
                               const char *cmd, const char *arg);

/**
 * @brief Returns the total number of valid snapshots currently recorded.
 */
size_t debug_get_snapshot_count(void);

/**
 * @brief Retrieves a historical snapshot by index (0 = oldest valid in buffer).
 */
bool debug_get_snapshot(size_t index, debug_snapshot_t *out_snap);

/**
 * @brief Retrieves the most recent snapshot in the buffer.
 */
const debug_snapshot_t *debug_get_latest_snapshot(void);

/**
 * @brief Restores state fields from a historical snapshot.
 *
 * @param[in] index Historical snapshot index to rewind to.
 * @param[out] out_fields Populated state fields upon successful rewind.
 * @return true if rewind succeeded, false if index invalid.
 */
bool debug_rewind_to_snapshot(size_t index, debug_state_fields_t *out_fields);

/**
 * @brief Computes a formatted textual difference between two snapshots.
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
 * @brief Computes a deterministic 32-bit hash of a state record.
 */
uint32_t debug_compute_state_hash(const debug_state_fields_t *fields);

/* --- Breakpoints & Watchpoints --- */

/**
 * @brief Registers a dynamic breakpoint.
 *
 * @param[in] type Breakpoint type.
 * @param[in] target_value Target value to trigger on.
 * @param[in] label Descriptive label.
 * @return Breakpoint ID (positive integer) or -1 if capacity exceeded.
 */
int debug_add_breakpoint(debug_breakpoint_type_t type, int target_value, const char *label);

/**
 * @brief Removes an active breakpoint by ID.
 */
bool debug_remove_breakpoint(int bp_id);

/**
 * @brief Clears all registered breakpoints.
 */
void debug_clear_breakpoints(void);

/**
 * @brief Evaluates active breakpoints against the current state.
 *
 * @param[in] fields Current state fields.
 * @param[out] out_reason Buffer populated with breakpoint trigger reason.
 * @param[in] max_len Size of out_reason buffer.
 * @return true if a breakpoint triggered, false otherwise.
 */
bool debug_check_breakpoints(const debug_state_fields_t *fields,
                             char *out_reason, size_t max_len);

/**
 * @brief Traps a tripwire event in the debugger.
 */
void debug_trap_tripwire(const char *subsystem, const char *condition,
                         const char *file, int line);

/**
 * @brief Returns the latched tripwire status.
 */
debug_tripwire_t debug_get_tripwire_status(void);

/**
 * @brief Returns details of the latched tripwire.
 */
const char *debug_get_tripwire_details(void);

/**
 * @brief Resets the debugger tripwire latch.
 */
void debug_reset_tripwire(void);

/* --- Stepping & Execution Control --- */

/**
 * @brief Sets the execution stepping mode.
 */
void debug_set_step_mode(debug_step_mode_t mode);

/**
 * @brief Retrieves the active execution stepping mode.
 */
debug_step_mode_t debug_get_step_mode(void);

/* --- Inspection & State Dumps --- */

/**
 * @brief Prints an ANSI-colored or formatted diagnostic dump of the current state.
 */
void debug_dump_state(const debug_state_fields_t *fields);

/**
 * @brief Prints a tabular timeline of recorded snapshots.
 */
void debug_dump_snapshot_history(void);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

/**
 * @brief Central module metadata for the debugger utility.
 */
extern const module_metadata_t debug_manifest;

/**
 * @brief Preflight diagnostic audit routine for central manifest registration.
 */
diag_result_t debug_preflight(void);

/**
 * @brief Comprehensive self-audit verifying all debugger invariants.
 */
diag_result_t debug_audit(bool verbose);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#ifdef __cplusplus
}
#endif

#endif // TL_UTIL_DEBUG_H
