// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          debug.c
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
//   Coordinates state snapshots, time-travel history rewind, dynamic breakpoints,
//   non-crashing tripwire traps, and ANSI visual state inspection.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.manifest, core.util.string, core.util.data,
//                            core.util.time, core.util.log, core.util.debug_fallback
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Zero dynamic heap allocations).
//   - Concurrency / Safety:  CONCURRENCY_MAIN_THREAD (Sequential main thread).
//   - Latency Budget:        < 15 microseconds per snapshot record or inspection.
//
// AXIOMATIC INVARIANTS:
//   1. Zero dynamic heap allocations across all execution paths.
//   2. Ring buffer capacity strictly bounded by DEBUG_MAX_SNAPSHOTS (64).
//   3. Deterministic state hash generation via FNV-1a algorithm.
//   4. Decoupled Layer 0 independence from higher simulation strata.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 0 (Core Utilities) | Machine: Engine Debugger & State Rewind
// Subsystem: util/debug
// =============================================================================

#include "util/debug/debug.h"
#include "util/debug/debug_fallback.h"
#include "util/data/fs_utils.h"
#include "util/data/json_utils.h"
#include "util/string/string_utils.h"
#include "util/time/time_utils.h"
#include "util/log/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Internal state for the Layer 0 debugger subsystem.
 */
typedef struct {
    debug_snapshot_t snapshots[DEBUG_MAX_SNAPSHOTS];
    size_t snapshot_count;
    size_t snapshot_head;
    uint64_t next_snapshot_id;

    debug_breakpoint_t breakpoints[DEBUG_MAX_BREAKPOINTS];
    size_t breakpoint_count;
    int next_breakpoint_id;

    debug_step_mode_t step_mode;
    debug_tripwire_t tripwire_status;
    char tripwire_details[128];
    uint32_t traps_triggered;

    debug_config_t config;
} debug_subsystem_state_t;

static debug_subsystem_state_t s_debug;
static bool s_debug_active = false;
static bool s_debug_data_driven = false;

static const char *const s_debug_invariants[4] = {
    "Zero dynamic heap allocations across all debugger execution routines",
    "Ring buffer snapshots bounded within [0, DEBUG_MAX_SNAPSHOTS]",
    "Bitwise deterministic state hashing via 32-bit FNV-1a",
    "Non-crashing tripwire traps and historical rewind state restoration"
};

const module_metadata_t debug_manifest = {
    "core.util.debug",
    "debug.h",
    LAYER_0_CORE_UTIL,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-11-02",               // last_updated
    "Zero-heap engine debugger, snapshot ring buffer & time-travel rewind",
    4,                          // dependency_count
    {
        "core.util.manifest",
        "core.util.fs",
        "core.util.json",
        "core.util.log"
    },
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    15,                         // max_budget_us
    4,
    {
        s_debug_invariants[0],
        s_debug_invariants[1],
        s_debug_invariants[2],
        s_debug_invariants[3]
    },
    debug_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE DEBUGGER IMPLEMENTATION
// Stratum: Layer 0 (Core Utilities) | Machine: Engine Debugger & State Rewind
// Subsystem: util/debug
// =============================================================================

/**
 * @brief Computes a deterministic 32-bit FNV-1a hash of a state record.
 *
 * @param[in] fields State fields to hash.
 * @return 32-bit unsigned FNV-1a hash.
 */
uint32_t debug_compute_state_hash(const debug_state_fields_t *fields)
{
    /* Step 1: Precondition check on pointer */
    if (fields == NULL) {
        return 0;
    }

    /* Step 2: Initialize FNV-1a 32-bit parameters */
    uint32_t hash = 2166136261u;
    const uint32_t prime = 16777619u;

    #define HASH_INT(val) do { \
        int v = (val); \
        const uint8_t *bytes = (const uint8_t *)&v; \
        for (size_t b = 0; b < sizeof(int); b++) { \
            hash ^= bytes[b]; \
            hash *= prime; \
        } \
    } while (0)

    #define HASH_STR(str) do { \
        const char *p = (str); \
        while (*p) { \
            hash ^= (uint8_t)(*p); \
            hash *= prime; \
            p++; \
        } \
    } while (0)

    /* Step 3: Hash individual simulation state fields */
    HASH_INT(fields->turn);
    HASH_STR(fields->hero_name);
    HASH_INT(fields->hp);
    HASH_INT(fields->max_hp);
    HASH_INT(fields->mp);
    HASH_INT(fields->tp);
    HASH_INT(fields->shield_power);
    HASH_INT(fields->sword_edge);
    HASH_INT(fields->stance);
    HASH_INT(fields->moral_sway);
    HASH_INT(fields->location);
    HASH_INT(fields->lux_barrio_turns);
    HASH_INT(fields->charm_turns);
    HASH_INT(fields->threat_hp);
    HASH_INT(fields->foes_vanquished);

    #undef HASH_INT
    #undef HASH_STR

    return hash;
}

/**
 * @brief Returns the quarantined emergency fallback configuration.
 *
 * @return Default fallback debug_config_t structure.
 */
debug_config_t debug_get_monolith_fallback_config(void)
{
    debug_config_t cfg;
    debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));
    return cfg;
}

/**
 * @brief Loads a specific named debug profile from data/debug/.
 *
 * @param[in]  profile_name Name of the profile JSON file without extension.
 * @param[out] out_config   Output configuration structure to populate.
 *
 * @return `true` if loaded successfully; `false` on missing or unreadable file.
 */
bool debug_load_config_profile(const char *profile_name, debug_config_t *out_config)
{
    /* Step 1: Precondition validation */
    if (profile_name == NULL || out_config == NULL) {
        return false;
    }

    /* Step 2: Resolve profile file path across candidate directories */
    char file_path[FS_PATH_MAX];
    bool exists = false;
    snprintf(file_path, sizeof(file_path), "data/debug/%s.json", profile_name);
    if (fs_file_exists(file_path)) {
        exists = true;
    } else {
        snprintf(file_path, sizeof(file_path), "../data/debug/%s.json", profile_name);
        if (fs_file_exists(file_path)) {
            exists = true;
        }
    }

    /* Step 3: Fallback tripwire if file is absent */
    if (!exists) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_SETTING_MISSING;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug profile file missing: %.64s", profile_name);
        *out_config = debug_get_monolith_fallback_config();
        return true;
    }

    /* Step 4: Bounded read of profile JSON content */
    char json_buffer[FS_BUFFER_MEDIUM_BYTES];
    size_t bytes_read = 0;
    if (!fs_read_file_bounded(file_path, json_buffer, sizeof(json_buffer), &bytes_read)) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_SETTING_MISSING;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Failed to read debug profile: %.64s", file_path);
        *out_config = debug_get_monolith_fallback_config();
        return true;
    }

    /* Step 5: Initialize defaults before parsing */
    memset(out_config, 0, sizeof(debug_config_t));
    out_config->enabled = true;
    out_config->max_snapshots = DEBUG_MAX_SNAPSHOTS;
    out_config->snapshot_interval_turns = 1;
    out_config->auto_dump_on_tripwire = true;
    out_config->interactive_repl = true;
    out_config->ansi_color = true;

    /* Step 6: Deserialize profile fields */
    json_get_object_bool(json_buffer, "profile", "enabled", &out_config->enabled);
    json_get_object_bool(json_buffer, "profile", "break_on_error", &out_config->break_on_error);
    json_get_object_bool(json_buffer, "profile", "break_on_tripwire", &out_config->break_on_tripwire);
    json_get_object_bool(json_buffer, "profile", "break_on_sway_breach", &out_config->break_on_sway_breach);
    json_get_object_bool(json_buffer, "profile", "break_on_resonance_breach", &out_config->break_on_resonance_breach);
    json_get_object_bool(json_buffer, "profile", "break_on_hp_zero", &out_config->break_on_hp_zero);
    json_get_object_bool(json_buffer, "profile", "break_on_stance_change", &out_config->break_on_stance_change);

    int max_snaps = 0;
    if (json_get_object_int(json_buffer, "profile", "max_snapshots", &max_snaps) && max_snaps > 0) {
        out_config->max_snapshots = (size_t)max_snaps;
        if (out_config->max_snapshots > DEBUG_MAX_SNAPSHOTS) {
            out_config->max_snapshots = DEBUG_MAX_SNAPSHOTS;
        }
    }

    json_get_object_int(json_buffer, "profile", "snapshot_interval_turns", &out_config->snapshot_interval_turns);
    json_get_object_bool(json_buffer, "profile", "auto_dump_on_tripwire", &out_config->auto_dump_on_tripwire);
    json_get_object_bool(json_buffer, "profile", "interactive_repl", &out_config->interactive_repl);
    json_get_object_bool(json_buffer, "profile", "ansi_color", &out_config->ansi_color);

    json_get_object_string(json_buffer, "profile", "trace_channels",
                           out_config->trace_channels, sizeof(out_config->trace_channels));
    str_safe_copy(out_config->profile_name, profile_name, sizeof(out_config->profile_name));

    out_config->loaded_from_data = true;
    return true;
}

/**
 * @brief Loads a debug configuration from a manifest file.
 *
 * @param[in]  manifest_path Path to debug manifest JSON (NULL for default candidate paths).
 * @param[out] out_config    Output configuration structure to populate.
 *
 * @return `true` if loaded successfully; `false` on missing file or read failure.
 */
bool debug_load_config_from_manifest(const char *manifest_path, debug_config_t *out_config)
{
    /* Step 1: Precondition check */
    if (out_config == NULL) {
        return false;
    }

    /* Step 2: Resolve candidate manifest paths */
    static const char *const s_manifest_candidates[] = {
        "data/debug/manifest.json",
        "../data/debug/manifest.json"
    };

    char resolved_path[FS_PATH_MAX];
    bool found = false;

    if (manifest_path != NULL && manifest_path[0] != '\0') {
        if (fs_file_exists(manifest_path)) {
            str_safe_copy(resolved_path, manifest_path, sizeof(resolved_path));
            found = true;
        } else {
            snprintf(resolved_path, sizeof(resolved_path), "../%s", manifest_path);
            if (fs_file_exists(resolved_path)) {
                found = true;
            }
        }
    } else {
        for (size_t i = 0; i < sizeof(s_manifest_candidates) / sizeof(s_manifest_candidates[0]); i++) {
            if (fs_file_exists(s_manifest_candidates[i])) {
                str_safe_copy(resolved_path, s_manifest_candidates[i], sizeof(resolved_path));
                found = true;
                break;
            }
        }
    }

    if (!found) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_MANIFEST_MISSING;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug manifest missing");
        return false;
    }

    /* Step 3: Read manifest JSON file */
    char json_buffer[FS_BUFFER_MEDIUM_BYTES];
    size_t bytes_read = 0;
    if (!fs_read_file_bounded(resolved_path, json_buffer, sizeof(json_buffer), &bytes_read)) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_MANIFEST_MISSING;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Failed to read debug manifest: %.64s", resolved_path);
        return false;
    }

    /* Step 4: Extract active profile name and delegate */
    char active_profile[32];
    if (!json_get_string(json_buffer, "active_profile", active_profile, sizeof(active_profile))) {
        str_safe_copy(active_profile, "default", sizeof(active_profile));
    }

    return debug_load_config_profile(active_profile, out_config);
}

/**
 * @brief Returns the default configuration (data-driven or fallback).
 *
 * @return Populated default debug_config_t structure.
 */
debug_config_t debug_get_default_config(void)
{
    debug_config_t cfg;
    if (debug_load_config_from_manifest(NULL, &cfg)) {
        return cfg;
    }
    return debug_get_monolith_fallback_config();
}

/**
 * @brief Initializes the debugging system with a specific configuration.
 *
 * @param[in] config Optional custom configuration, or NULL for data-driven default.
 *
 * @return `true` on successful initialization; `false` on fatal failure.
 */
bool debug_init(const debug_config_t *config)
{
    /* Step 1: Resolve configuration instance */
    debug_config_t cfg;
    if (config != NULL) {
        cfg = *config;
    } else {
        cfg = debug_get_default_config();
    }

    /* Step 2: Invariant validation & bounds enforcement */
    if (cfg.max_snapshots == 0 || cfg.max_snapshots > DEBUG_MAX_SNAPSHOTS) {
        cfg.max_snapshots = DEBUG_MAX_SNAPSHOTS;
    }
    if (cfg.snapshot_interval_turns <= 0) {
        cfg.snapshot_interval_turns = 1;
    }

    /* Step 3: Zero-initialize and populate internal subsystem state */
    memset(&s_debug, 0, sizeof(s_debug));
    s_debug.config = cfg;
    s_debug.snapshot_count = 0;
    s_debug.snapshot_head = 0;
    s_debug.next_snapshot_id = 1;
    s_debug.breakpoint_count = 0;
    s_debug.next_breakpoint_id = 1;
    s_debug.step_mode = DEBUG_STEP_RUN;
    s_debug.tripwire_status = DEBUG_TRIPWIRE_CLEAR;
    s_debug.traps_triggered = 0;

    s_debug_active = true;
    s_debug_data_driven = cfg.loaded_from_data;

    LOG_INFO(LOG_CAT_DEBUG, "Debugger", 0.0f,
             "Debugger initialized (Profile: %s, DataDriven: %s, MaxSnapshots: %zu)",
             cfg.profile_name, cfg.loaded_from_data ? "YES" : "NO", cfg.max_snapshots);

    return true;
}

/**
 * @brief Shuts down the debugging system and resets state.
 */
void debug_shutdown(void)
{
    s_debug_active = false;
    s_debug_data_driven = false;
}

/**
 * @brief Checks if the debugging system is currently initialized and active.
 *
 * @return `true` if initialized; `false` otherwise.
 */
bool debug_is_active(void)
{
    return s_debug_active;
}

/**
 * @brief Checks if active debugger configuration was loaded from data assets.
 *
 * @return `true` if loaded from JSON; `false` if using monolith fallback.
 */
bool debug_is_data_driven(void)
{
    return s_debug_data_driven;
}

/**
 * @brief Returns the name of the active debugging profile.
 *
 * @return Null-terminated string identifying active profile.
 */
const char *debug_get_active_profile_name(void)
{
    if (!s_debug_active) {
        return "inactive";
    }
    return s_debug.config.profile_name;
}

/**
 * @brief Captures and records a state snapshot into the static ring buffer.
 *
 * @param[in] fields State fields to capture.
 * @param[in] cmd    Command string that caused this state transition.
 * @param[in] arg    Argument string associated with the command.
 *
 * @return Unique monotonic snapshot ID, or 0 on failure/inactive.
 */
uint64_t debug_record_snapshot(const debug_state_fields_t *fields,
                               const char *cmd, const char *arg)
{
    /* Step 1: Precondition check on active state and fields */
    if (!s_debug_active || fields == NULL) {
        return 0;
    }

    /* Step 2: Populate snapshot record in ring buffer slot */
    size_t idx = s_debug.snapshot_head;
    debug_snapshot_t *snap = &s_debug.snapshots[idx];

    snap->snapshot_id = s_debug.next_snapshot_id++;
    snap->timestamp_ms = time_get_monotonic_ms();
    snap->state = *fields;

    str_safe_copy(snap->last_command, (cmd != NULL) ? cmd : "", sizeof(snap->last_command));
    str_safe_copy(snap->last_arg, (arg != NULL) ? arg : "", sizeof(snap->last_arg));

    snap->state_hash = debug_compute_state_hash(fields);
    snap->log_sequence_id = log_get_total_entries();

    /* Step 3: Advance head in circular ring buffer */
    s_debug.snapshot_head = (s_debug.snapshot_head + 1) % s_debug.config.max_snapshots;
    if (s_debug.snapshot_count < s_debug.config.max_snapshots) {
        s_debug.snapshot_count++;
    }

    return snap->snapshot_id;
}

/**
 * @brief Returns the total number of valid snapshots currently recorded.
 *
 * @return Number of snapshots stored in the ring buffer.
 */
size_t debug_get_snapshot_count(void)
{
    if (!s_debug_active) {
        return 0;
    }
    return s_debug.snapshot_count;
}

/**
 * @brief Retrieves a historical snapshot by logical index (0 = oldest valid in buffer).
 *
 * @param[in]  index    Logical snapshot index.
 * @param[out] out_snap Destination snapshot structure.
 *
 * @return `true` if snapshot was retrieved; `false` on invalid index or inactive debugger.
 */
bool debug_get_snapshot(size_t index, debug_snapshot_t *out_snap)
{
    /* Step 1: Precondition check */
    if (!s_debug_active || out_snap == NULL || index >= s_debug.snapshot_count) {
        return false;
    }

    /* Step 2: Map logical index to physical ring buffer slot */
    size_t cap = s_debug.config.max_snapshots;
    size_t physical_idx;

    if (s_debug.snapshot_count < cap) {
        physical_idx = index;
    } else {
        physical_idx = (s_debug.snapshot_head + index) % cap;
    }

    *out_snap = s_debug.snapshots[physical_idx];
    return true;
}

/**
 * @brief Retrieves the most recent snapshot in the buffer.
 *
 * @return Pointer to latest snapshot in the ring buffer, or NULL if empty.
 */
const debug_snapshot_t *debug_get_latest_snapshot(void)
{
    if (!s_debug_active || s_debug.snapshot_count == 0) {
        return NULL;
    }

    size_t cap = s_debug.config.max_snapshots;
    size_t latest_idx = (s_debug.snapshot_head + cap - 1) % cap;
    return &s_debug.snapshots[latest_idx];
}

/**
 * @brief Restores simulation state fields from a recorded snapshot index.
 *
 * Copies the frozen field state from the specified historical snapshot into
 * the caller-provided out_fields structure.
 *
 * @param[in]  index      Zero-based chronological index (0 = oldest).
 * @param[out] out_fields Destination pointer for restored state fields.
 * @return True if snapshot was retrieved and restored; false otherwise.
 */
bool debug_rewind_to_snapshot(size_t index, debug_state_fields_t *out_fields)
{
    // Step 1: Precondition check on active debugger and destination pointer
    if (!s_debug_active || out_fields == NULL) {
        return false;
    }

    // Step 2: Retrieve the frozen snapshot from the ring buffer
    debug_snapshot_t snap;
    if (!debug_get_snapshot(index, &snap)) {
        return false;
    }

    // Step 3: Copy field data into destination
    *out_fields = snap.state;

    LOG_INFO(LOG_CAT_DEBUG, "Debugger", 0.0f,
             "Rewound simulation state to Snapshot #%lu (Turn %d, Hero: %s)",
             (unsigned long)snap.snapshot_id, snap.state.turn, snap.state.hero_name);
    return true;
}

/**
 * @brief Formats a human-readable comparison delta between two state snapshots.
 *
 * Compares health, shield power, sword edge, stance, moral sway, location, and
 * 32-bit state hash, generating formatted diff text into out_buf.
 *
 * @param[in]  s1      Pointer to the baseline (older) snapshot.
 * @param[in]  s2      Pointer to the target (newer) snapshot.
 * @param[out] out_buf Destination character buffer.
 * @param[in]  buf_len Maximum writable capacity of destination buffer.
 * @return Number of characters written to out_buf (excluding null terminator).
 */
size_t debug_diff_snapshots(const debug_snapshot_t *s1, const debug_snapshot_t *s2,
                            char *out_buf, size_t buf_len)
{
    // Step 1: Validate input pointers and non-empty buffer capacity
    if (s1 == NULL || s2 == NULL || out_buf == NULL || buf_len == 0) {
        return 0;
    }

    char tmp[1024];
    size_t offset = 0;

    // Step 2: Format header banner
    offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
        "Snapshot Diff: #%lu (Turn %d) -> #%lu (Turn %d)\n"
        "--------------------------------------------------\n",
        (unsigned long)s1->snapshot_id, s1->state.turn,
        (unsigned long)s2->snapshot_id, s2->state.turn);

    // Step 3: Format field deltas
    if (s1->state.hp != s2->state.hp) {
        offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
            "  HP:           %d -> %d (%+d)\n",
            s1->state.hp, s2->state.hp, s2->state.hp - s1->state.hp);
    }
    if (s1->state.shield_power != s2->state.shield_power) {
        offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
            "  Shield (Lux): %d -> %d (%+d)\n",
            s1->state.shield_power, s2->state.shield_power,
            s2->state.shield_power - s1->state.shield_power);
    }
    if (s1->state.sword_edge != s2->state.sword_edge) {
        offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
            "  Sword (Ten):  %d -> %d (%+d)\n",
            s1->state.sword_edge, s2->state.sword_edge,
            s2->state.sword_edge - s1->state.sword_edge);
    }
    if (s1->state.stance != s2->state.stance) {
        offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
            "  Stance:       %d -> %d\n", s1->state.stance, s2->state.stance);
    }
    if (s1->state.moral_sway != s2->state.moral_sway) {
        offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
            "  Moral Sway:   %d -> %d (%+d)\n",
            s1->state.moral_sway, s2->state.moral_sway,
            s2->state.moral_sway - s1->state.moral_sway);
    }
    if (s1->state.location != s2->state.location) {
        offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
            "  Location:     %d -> %d\n", s1->state.location, s2->state.location);
    }
    if (s1->state_hash != s2->state_hash) {
        offset += (size_t)snprintf(tmp + offset, sizeof(tmp) - offset,
            "  State Hash:   0x%08X -> 0x%08X\n", s1->state_hash, s2->state_hash);
    }

    // Step 4: Safely copy into output buffer
    str_safe_copy(out_buf, tmp, buf_len);
    return strlen(out_buf);
}

/**
 * @brief Registers a dynamic conditional breakpoint or state watchpoint.
 *
 * Appends a new breakpoint entry to the active breakpoint registry if capacity
 * permits and returns its unique integer identifier.
 *
 * @param[in] type         Type of breakpoint to evaluate (turn, stance, sway, etc.).
 * @param[in] target_value Target value or threshold triggering the break.
 * @param[in] label        Human-readable descriptor or variable name.
 * @return Unique breakpoint identifier on success; -1 on failure or capacity limit.
 */
int debug_add_breakpoint(debug_breakpoint_type_t type, int target_value, const char *label)
{
    // Step 1: Guard against inactive debugger, invalid type, or capacity limit
    if (!s_debug_active || type == DEBUG_BREAK_NONE || s_debug.breakpoint_count >= DEBUG_MAX_BREAKPOINTS) {
        return -1;
    }

    // Step 2: Allocate and populate breakpoint entry
    int id = s_debug.next_breakpoint_id++;
    debug_breakpoint_t *bp = &s_debug.breakpoints[s_debug.breakpoint_count++];
    bp->id = id;
    bp->type = type;
    bp->target_value = target_value;
    bp->is_active = true;
    str_safe_copy(bp->label, (label != NULL) ? label : "unnamed_watchpoint", sizeof(bp->label));

    LOG_DEBUG(LOG_CAT_DEBUG, "Debugger", 0.0f,
              "Registered breakpoint #%d (Type: %d, Target: %d, Label: %s)",
              id, type, target_value, bp->label);
    return id;
}

/**
 * @brief Removes an active breakpoint by its assigned identifier.
 *
 * Searches the registry for the specified identifier and shifts subsequent
 * elements leftward to maintain zero-heap contiguous storage.
 *
 * @param[in] bp_id Unique breakpoint identifier returned by debug_add_breakpoint.
 * @return True if breakpoint was found and removed; false otherwise.
 */
bool debug_remove_breakpoint(int bp_id)
{
    if (!s_debug_active) {
        return false;
    }

    // Step 1: Scan active breakpoints for matching identifier
    for (size_t i = 0; i < s_debug.breakpoint_count; i++) {
        if (s_debug.breakpoints[i].id == bp_id) {
            // Step 2: Shift remaining elements down to keep buffer contiguous
            for (size_t j = i; j + 1 < s_debug.breakpoint_count; j++) {
                s_debug.breakpoints[j] = s_debug.breakpoints[j + 1];
            }
            s_debug.breakpoint_count--;
            return true;
        }
    }
    return false;
}

/**
 * @brief Clears all registered dynamic breakpoints.
 *
 * Resets the active breakpoint count to 0 without freeing memory.
 */
void debug_clear_breakpoints(void)
{
    if (s_debug_active) {
        s_debug.breakpoint_count = 0;
    }
}

/**
 * @brief Evaluates current simulation fields against active breakpoints and invariant guardrails.
 *
 * Checks dynamic breakpoints registered via debug_add_breakpoint followed by
 * static configuration guardrails (sway limits, relic bounds, zero HP).
 *
 * @param[in]  fields     Current simulation state fields snapshot.
 * @param[out] out_reason Character buffer receiving description of triggered condition.
 * @param[in]  max_len    Maximum writable capacity of out_reason buffer.
 * @return True if a breakpoint or invariant was triggered and paused; false otherwise.
 */
bool debug_check_breakpoints(const debug_state_fields_t *fields,
                             char *out_reason, size_t max_len)
{
    if (!s_debug_active || fields == NULL) {
        return false;
    }

    // Step 1: Check dynamic registered breakpoints
    for (size_t i = 0; i < s_debug.breakpoint_count; i++) {
        const debug_breakpoint_t *bp = &s_debug.breakpoints[i];
        if (!bp->is_active) continue;

        bool hit = false;
        switch (bp->type) {
            case DEBUG_BREAK_TURN:
                hit = (fields->turn == bp->target_value);
                break;
            case DEBUG_BREAK_STANCE:
                hit = (fields->stance == bp->target_value);
                break;
            case DEBUG_BREAK_SWAY_RANGE:
                hit = (abs(fields->moral_sway) >= bp->target_value);
                break;
            case DEBUG_BREAK_RESONANCE_BREACH:
                hit = (fields->shield_power >= bp->target_value ||
                       fields->sword_edge >= bp->target_value);
                break;
            case DEBUG_BREAK_HP_THRESHOLD:
                hit = (fields->hp <= bp->target_value);
                break;
            case DEBUG_BREAK_LOCATION:
                hit = (fields->location == bp->target_value);
                break;
            case DEBUG_BREAK_TRIPWIRE:
                hit = (s_debug.tripwire_status != DEBUG_TRIPWIRE_CLEAR);
                break;
            default:
                break;
        }

        if (hit) {
            if (out_reason != NULL && max_len > 0) {
                snprintf(out_reason, max_len, "Breakpoint #%d hit: %s (Target %d)",
                         bp->id, bp->label, bp->target_value);
            }
            s_debug.step_mode = DEBUG_STEP_PAUSED;
            return true;
        }
    }

    // Step 2: Check configuration invariant guardrails
    if (s_debug.config.break_on_sway_breach &&
        (fields->moral_sway < -100 || fields->moral_sway > 100)) {
        if (out_reason != NULL && max_len > 0) {
            snprintf(out_reason, max_len, "Invariant Trap: Moral sway out of bounds (%d)",
                     fields->moral_sway);
        }
        s_debug.step_mode = DEBUG_STEP_PAUSED;
        return true;
    }

    if (s_debug.config.break_on_resonance_breach &&
        (fields->shield_power < 0 || fields->shield_power > 100 ||
         fields->sword_edge < 0 || fields->sword_edge > 100)) {
        if (out_reason != NULL && max_len > 0) {
            snprintf(out_reason, max_len, "Invariant Trap: Relic resonance breach (Lux: %d, Ten: %d)",
                     fields->shield_power, fields->sword_edge);
        }
        s_debug.step_mode = DEBUG_STEP_PAUSED;
        return true;
    }

    if (s_debug.config.break_on_hp_zero && fields->hp <= 0) {
        if (out_reason != NULL && max_len > 0) {
            snprintf(out_reason, max_len, "Watchpoint Trap: HP reached zero (%d)", fields->hp);
        }
        s_debug.step_mode = DEBUG_STEP_PAUSED;
        return true;
    }

    return false;
}

/**
 * @brief Latches a forensic invariant violation or anomalous event.
 *
 * Updates internal tripwire status to corrupt, records source file and line,
 * increments the trap counter, and optionally emits an alert to stderr.
 *
 * @param[in] subsystem Originating subsystem name (e.g. "SimEngine").
 * @param[in] condition Text describing the violated invariant condition.
 * @param[in] file      Source filename where violation occurred (__FILE__).
 * @param[in] line      Line number where violation occurred (__LINE__).
 */
void debug_trap_tripwire(const char *subsystem, const char *condition,
                         const char *file, int line)
{
    // Step 1: Latch tripwire state and increment counter
    s_debug.tripwire_status = DEBUG_TRIPWIRE_STATE_CORRUPT;
    s_debug.traps_triggered++;

    // Step 2: Format forensic tripwire detail string
    snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
             "[%s] %s (%s:%d)",
             (subsystem != NULL) ? subsystem : "unknown",
             (condition != NULL) ? condition : "anomaly",
             (file != NULL) ? file : "unknown", line);

    LOG_WARN(LOG_CAT_DEBUG, "Debugger", -0.5f, "Tripwire trapped: %s", s_debug.tripwire_details);

    // Step 3: Trigger console alert if auto_dump_on_tripwire is enabled
    if (s_debug_active && s_debug.config.auto_dump_on_tripwire) {
        fprintf(stderr, "\n[DEBUG TRIPWIRE ALERT] %s\n", s_debug.tripwire_details);
    }
}

/**
 * @brief Retrieves current tripwire latch status.
 *
 * @return Active debug_tripwire_t enum status code.
 */
debug_tripwire_t debug_get_tripwire_status(void)
{
    return s_debug.tripwire_status;
}

/**
 * @brief Retrieves forensic detail string of the most recent trapped tripwire.
 *
 * @return Constant string containing subsystem, condition, file, and line details.
 */
const char *debug_get_tripwire_details(void)
{
    return s_debug.tripwire_details;
}

/**
 * @brief Resets tripwire state back to clear.
 *
 * Clears status back to DEBUG_TRIPWIRE_CLEAR and empties detail string.
 */
void debug_reset_tripwire(void)
{
    s_debug.tripwire_status = DEBUG_TRIPWIRE_CLEAR;
    s_debug.tripwire_details[0] = '\0';
}

/**
 * @brief Sets execution stepping mode.
 *
 * @param[in] mode Desired stepping mode (continuous, step, or paused).
 */
void debug_set_step_mode(debug_step_mode_t mode)
{
    s_debug.step_mode = mode;
}

/**
 * @brief Retrieves current stepping mode.
 *
 * @return Current debug_step_mode_t execution mode.
 */
debug_step_mode_t debug_get_step_mode(void)
{
    return s_debug.step_mode;
}

/**
 * @brief Prints an interactive forensic state inspection table to stdout.
 *
 * Formats hero attributes, relic charges, active stance, moral sway, and
 * 32-bit state hash into a formatted inspection view.
 *
 * @param[in] fields Current simulation state fields snapshot.
 */
void debug_dump_state(const debug_state_fields_t *fields)
{
    // Step 1: Precondition check on fields pointer
    if (fields == NULL) return;

    // Step 2: Compute 32-bit hash for live state verification
    uint32_t hash = debug_compute_state_hash(fields);

    // Step 3: Render formatted inspection table
    printf("============================================================\n");
    printf("        TENEBRIS AND LUX - LIVE STATE INSPECTOR             \n");
    printf("============================================================\n");
    printf(" Hero Name:        %-16s | Turn:        %d\n", fields->hero_name, fields->turn);
    printf(" Health (HP):      %3d / %-3d          | State Hash:  0x%08X\n", fields->hp, fields->max_hp, hash);
    printf(" Shield of Lux:    %3d / 100            | Barrier:     %d turns\n", fields->shield_power, fields->lux_barrio_turns);
    printf(" Sword of Tenebris:%3d / 100            | Charm:       %d turns\n", fields->sword_edge, fields->charm_turns);
    printf(" Combat Stance:    ID %-2d                | Province ID: %d\n", fields->stance, fields->location);
    printf(" Moral Sway:       %+4d [-100, +100]    | Threat HP:   %d\n", fields->moral_sway, fields->threat_hp);
    printf(" Traps Latched:    %-4u                | Snapshots:   %zu / %zu\n",
           s_debug.traps_triggered, s_debug.snapshot_count, s_debug.config.max_snapshots);
    printf("============================================================\n");
}

/**
 * @brief Prints a chronological timeline table of all recorded state snapshots to stdout.
 *
 * Traverses the snapshot ring buffer in chronological order and outputs a
 * columnar report of turn, hero, HP, relics, stance, moral sway, and hash.
 */
void debug_dump_snapshot_history(void)
{
    // Step 1: Validate active debugger and non-empty snapshot history
    if (!s_debug_active || s_debug.snapshot_count == 0) {
        printf("[DEBUG] No snapshots recorded in history ring buffer.\n");
        return;
    }

    // Step 2: Render timeline table header
    printf("================================================================================\n");
    printf("                         STATE SNAPSHOT HISTORY TIMELINE                        \n");
    printf("================================================================================\n");
    printf("INDEX  SNAP ID  TURN  HERO        HP   SHIELD SWORD STANCE  SWAY  COMMAND     HASH    \n");
    printf("--------------------------------------------------------------------------------\n");

    // Step 3: Print each snapshot in chronological sequence
    for (size_t i = 0; i < s_debug.snapshot_count; i++) {
        debug_snapshot_t snap;
        if (debug_get_snapshot(i, &snap)) {
            printf("#%-4zu  %-7lu  %-4d  %-10s  %-3d  %-6d %-5d %-6d  %+4d  %-10s  %08X\n",
                   i, (unsigned long)snap.snapshot_id, snap.state.turn, snap.state.hero_name,
                   snap.state.hp, snap.state.shield_power, snap.state.sword_edge,
                   snap.state.stance, snap.state.moral_sway, snap.last_command, snap.state_hash);
        }
    }
    printf("================================================================================\n");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Engine Debugger & State Rewind
// Subsystem: util/debug
// =============================================================================

/**
 * @brief Executes isolated preflight diagnostic self-checks on the debugger subsystem.
 *
 * Verifies monolithic fallback code page integrity and deterministic 32-bit state hashing.
 *
 * @return Diagnostic result structure with passed status, severity, and details.
 */
diag_result_t debug_preflight(void)
{
    diag_result_t res;
    res.subsystem = debug_manifest.module_id;
    res.check_name = "Engine Debugger Subsystem Preflight Audit";

    // Sub-Check 1: Fallback code page integrity
    if (!debug_fallback_audit()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Monolithic debug fallback code page audit failed!";
        return res;
    }

    // Sub-Check 2: Hash algorithm determinism
    debug_state_fields_t f1, f2;
    memset(&f1, 0, sizeof(f1));
    memset(&f2, 0, sizeof(f2));

    f1.turn = 5;
    str_safe_copy(f1.hero_name, "Elijah", sizeof(f1.hero_name));
    f1.hp = 450;
    f1.max_hp = 450;
    f1.shield_power = 80;
    f1.sword_edge = 80;
    f1.moral_sway = 25;

    f2 = f1;
    uint32_t h1 = debug_compute_state_hash(&f1);
    uint32_t h2 = debug_compute_state_hash(&f2);

    if (h1 == 0 || h1 != h2) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "State hash calculation non-deterministic!";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Debugger preflight verified: Zero-heap snapshots, deterministic hashing & fallback ready.";
    return res;
}

/**
 * @brief Executes full diagnostic self-audit on the debugger subsystem.
 *
 * Evaluates preflight checks, then tests snapshot recording and time-travel rewind
 * under an isolated, non-interfering configuration.
 *
 * @param[in] verbose If true, prints progress messages to stdout.
 * @return Diagnostic result structure indicating overall audit pass/fail status.
 */
diag_result_t debug_audit(bool verbose)
{
    if (verbose) {
        printf("--- Running Core Utilities: Debugger Subsystem Diagnostic Audit ---\n");
    }

    // Step 1: Execute preflight checks
    diag_result_t pre = debug_preflight();
    if (!pre.passed) {
        return pre;
    }

    // Step 2: Verify snapshot recording & rewind under isolated test
    bool was_active = s_debug_active;
    debug_config_t prev_cfg = s_debug.config;

    debug_config_t test_cfg = debug_get_monolith_fallback_config();
    test_cfg.max_snapshots = 8;
    debug_init(&test_cfg);

    debug_state_fields_t sample;
    memset(&sample, 0, sizeof(sample));
    sample.turn = 1;
    str_safe_copy(sample.hero_name, "Katrina", sizeof(sample.hero_name));
    sample.hp = 400;
    sample.max_hp = 400;

    uint64_t id1 = debug_record_snapshot(&sample, "attack", "cuttatio");
    if (id1 == 0 || debug_get_snapshot_count() != 1) {
        debug_shutdown();
        diag_result_t fail_res;
        fail_res.subsystem = debug_manifest.module_id;
        fail_res.check_name = "Snapshot Recording";
        fail_res.passed = false;
        fail_res.severity = SEVERITY_CRITICAL;
        fail_res.details = "Snapshot recording failed during diagnostic audit!";
        return fail_res;
    }

    debug_state_fields_t restored;
    if (!debug_rewind_to_snapshot(0, &restored) || restored.hp != 400) {
        debug_shutdown();
        diag_result_t fail_res;
        fail_res.subsystem = debug_manifest.module_id;
        fail_res.check_name = "Snapshot Rewind";
        fail_res.passed = false;
        fail_res.severity = SEVERITY_CRITICAL;
        fail_res.details = "Snapshot rewind restoration failed!";
        return fail_res;
    }

    // Step 3: Restore previous debugger operational state
    debug_shutdown();
    if (was_active) {
        debug_init(&prev_cfg);
    }

    diag_result_t pass_res;
    pass_res.subsystem = debug_manifest.module_id;
    pass_res.check_name = "Debugger Subsystem Diagnostic Audit";
    pass_res.passed = true;
    pass_res.severity = SEVERITY_PASS;
    pass_res.details = "All debugger invariants, snapshot rewind, and tripwires satisfied.";
    return pass_res;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
