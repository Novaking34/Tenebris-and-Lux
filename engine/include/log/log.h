/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          log.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry & Flight Recorder Engine
@role          High-performance, zero-heap logging, telemetry, and forensic
               flight recorder subsystem delivering structured, machine-greppable
               audit trails with ISO 8601 timestamps, microsecond monotonic deltas,
               gradient moral grading, automated size rotation, and in-memory
               circular ring buffering for live viewport HUD inspection.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monotonic sequence counter: ID strictly increments per entry.
               4. Gradient grade strictly bounded in [-1.0f, +1.0f] <-> [-100, +100].
               5. Fixed-width token columns guarantee deterministic regex/grep parsing.
               6. Automated rotation bounds: active log file never exceeds byte threshold.
               7. Static memory boundary: in-flight ring buffer never dynamic allocates.
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Forensic flight recorder and diagnostic audit pipeline for the Tenebris and Lux
  tactical RPG simulation engine.
  Provides structured, reproducible execution trails that bridge pure headless C11
  simulation state with GUI runtime viewports and developer diagnostic telemetry.
  Integrates directly with data-driven configuration assets residing in
  engine/config/diagnostics/telemetry/ (manifest.json, default.json, development.json,
  production.json, quiet.json, combat_focus.json, forensic_telemetry.json)
  with quarantined emergency fallback protection (log_fallback.h).

  ### For Game Designers & Non-Programmers:
  This module acts like the black-box flight recorder of an aircraft. Whenever
  a hero strikes, casts a spell, shifts combat stances, or trips a story event,
  this system records exactly what happened, when it happened, which entity
  performed it, and the exact moral balance of the world at that moment.

  ### For Systems Programmers & Engine Architects:
  - 100% Zero-Heap Memory Model (ALLOC_ZERO_HEAP): All buffers, ring queues,
    and line formatting buffers reside in .bss or on caller CPU stack frames.
  - Multi-tier severity hierarchy: FATAL down to TRACE with bitmask filtering.
  - Dual-destination dispatch: simultaneous emission to rotating disk archives
    and fixed-capacity circular ring buffer for runtime HUD inspection.
================================================================================
*/

#ifndef TENEBRIS_LUX_LOG_H
#define TENEBRIS_LUX_LOG_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
Mission: Telemetry limits, downward include ladder, log manifests, structural
ontology, and pre-conditional validation predicates for diagnostic flight recording.
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
Sub-Assembly 2.1: Pre-Conditions & Telemetry Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing bounds for the flight recorder and log rotation pipeline.
For Designers: Bounds ensure log lines and entity names are cleanly formatted.
For Programmers: Guarantees zero heap allocation (ALLOC_ZERO_HEAP) in static buffers.
--------------------------------------------------------------------------------
*/

/**
 * @brief Maximum length in bytes for character or entity name tokens.
 * Sized to 16 bytes to fit "Player", "Elijah", "Katrina", "MasterLoader", etc.
 */
#define LOG_MAX_ENTITY_LEN       16

/**
 * @brief Maximum payload length in bytes for a single log message text.
 * Sized to 256 bytes for single-line combat, story, and diagnostic events.
 */
#define LOG_MAX_MESSAGE_LEN      256

/**
 * @brief Maximum capacity in bytes for a fully formatted output log line.
 * Sized to 512 bytes to safely contain ISO timestamp, category, monotonic
 * delta microsecond counter, moral grade percentage, and message payload.
 */
#define LOG_MAX_LINE_BUFFER      512

/**
 * @brief Default file size threshold triggering automatic log archive rotation.
 * Sized to 1,048,576 bytes (1 MB) to prevent disk bloat during long campaigns.
 */
#define LOG_DEFAULT_MAX_BYTES    (1024 * 1024)

/**
 * @brief Default maximum number of rotated archive files retained on disk.
 * Retains up to 25 historical files before purging the oldest archive.
 */
#define LOG_DEFAULT_MAX_FILES    25

/**
 * @brief Fixed capacity of the in-memory circular flight recorder ring buffer.
 * Holds the most recent 64 events for instantaneous live HUD display.
 */
#define LOG_RING_BUFFER_CAPACITY 64

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the engine's Loose-Bolts initialization DAG.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap Logging Engine. */
#define LOG_MODULE_ID                  "log.engine"

/**
 * Manifest record for the Zero-Heap Logging Engine.
 */
extern const module_metadata_t log_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for graded telemetry, category routing, and log snapshots.
--------------------------------------------------------------------------------
*/

/**
 * @enum log_severity
 * @brief Graded severity taxonomy for engine diagnostics and telemetry events.
 * Ordered from highest severity (FATAL) down to finest detail (TRACE).
 *
 * For Designers: Defines how critical an event is:
 *   - FATAL/CRITICAL: Showstoppers that break gameplay.
 *   - ERROR/WARNING: Problems that were handled gracefully with fallbacks.
 *   - INFO: Normal story, quest, and milestone announcements.
 *   - DEBUG/TRACE: Deep technical numbers for battle balance and frame timing.
 *
 * For Programmers: Strict integer ordering enables bitmask and threshold filtering.
 */
typedef enum log_severity {
    LOG_SEV_FATAL = 0,    /**< Unrecoverable crash or state corruption. */
    LOG_SEV_CRITICAL,     /**< Severe failure threatening gameplay loop. */
    LOG_SEV_ERROR,        /**< Discrete operation failure with fallback. */
    LOG_SEV_WARNING,      /**< Suspicious condition or degraded state. */
    LOG_SEV_INFO,         /**< Major milestone or narrative state shift. */
    LOG_SEV_DEBUG,        /**< Granular spatial, coordinate, or stance data. */
    LOG_SEV_TRACE,        /**< High-frequency per-tick calculation telemetry. */
    LOG_SEV_COUNT
} log_severity_t;

/**
 * @enum log_category
 * @brief Functional categorization of logging events for focused channel filtering.
 *
 * For Designers: Divides game events into distinct thematic channels:
 *   - CHARACTER: Hero and enemy combat actions, stance shifts, and health changes.
 *   - MESSAGE: Dialogue spoken by NPCs, narrative exposition, and story scrolls.
 *   - EVENT: World triggers, quest progression, and battle victory/defeat.
 *   - LOG/DEBUG: System startup messages and technical performance metrics.
 *
 * For Programmers: Allows splitting logs into separate files or viewport HUD tabs.
 */
typedef enum log_category {
    LOG_CAT_DEBUG = 0,    /**< Low-level developer engine trace. */
    LOG_CAT_LOG,          /**< Standard system lifecycle and startup logs. */
    LOG_CAT_EVENT,        /**< Significant world, combat, or quest trigger. */
    LOG_CAT_MESSAGE,      /**< Narrative dialogue or NPC communicative prompt. */
    LOG_CAT_CHARACTER,    /**< Protagonist/enemy spatial & relic stance action. */
    LOG_CAT_COUNT
} log_category_t;

/**
 * @struct log_entry
 * @brief Static telemetry record stored in memory ring buffer and emitted to disk.
 *
 * For Designers: A discrete "black box" flight recorder entry capturing everything
 * that happened at an exact moment in time, including which character acted and
 * what the moral alignment of the world was when it occurred.
 *
 * For Programmers: 100% stack/static record. Zero dynamic heap allocation.
 */
typedef struct log_entry {
    uint64_t sequence_id;                  /**< Monotonic sequence identifier. */
    uint64_t monotonic_us;                /**< Monotonic clock delta (microseconds). */
    char iso_timestamp[32];               /**< UTC ISO 8601 timestamp string. */
    log_severity_t severity;              /**< Graded severity level. */
    log_category_t category;              /**< Domain category. */
    char entity[LOG_MAX_ENTITY_LEN + 1];  /**< Character or system entity identifier. */
    float grade;                          /**< Normalized continuum [-1.0f .. +1.0f]. */
    int grade_percent;                    /**< Readout integer [-100 .. +100]. */
    const char *file;                     /**< Source file anchor (__FILE__). */
    int line;                             /**< Source line anchor (__LINE__). */
    const char *func;                     /**< Source function anchor (__func__). */
    char message[LOG_MAX_MESSAGE_LEN + 1];/**< Clean message payload. */
} log_entry_t;

/**
 * @enum log_tripwire
 * @brief Diagnostic tripwire state for the data-driven logging pipeline.
 *
 * For Designers: Circuit breakers that catch corrupt data files and automatically
 * engage safe defaults without crashing the game.
 *
 * For Programmers: Latching error enum verified during pre-flight self-audit.
 */
typedef enum log_tripwire {
    LOG_TRIPWIRE_CLEAR = 0,        /**< No tripwires active; data pipeline healthy. */
    LOG_TRIPWIRE_MANIFEST_MISSING, /**< Telemetry manifest could not be found. */
    LOG_TRIPWIRE_MANIFEST_INVALID, /**< Manifest unparseable or lacks menu entries. */
    LOG_TRIPWIRE_SETTING_MISSING,  /**< Designated setting JSON profile not found. */
    LOG_TRIPWIRE_SETTING_INVALID,  /**< Setting file syntax or attributes corrupted. */
    LOG_TRIPWIRE_BOUNDS_VIOLATION  /**< Configuration violated byte/file limits. */
} log_tripwire_t;

/**
 * @struct log_config
 * @brief Runtime configuration for the logging subsystem.
 *
 * For Designers: Configurable settings profile determining where logs are saved,
 * how much history is kept, and whether colored text appears in developer consoles.
 *
 * For Programmers: Loaded dynamically from engine/config/diagnostics/telemetry/.
 */
typedef struct log_config {
    char log_dir[64];            /**< Target directory for log files (e.g. "build/logs"). */
    char base_filename[64];      /**< Primary active log filename (e.g. "tenebris_lux.log"). */
    size_t max_file_size_bytes;  /**< Byte threshold triggering file rotation. */
    size_t max_rotated_files;    /**< Maximum archive files retained (10-25, default 25). */
    log_severity_t min_severity; /**< Filter cutoff (events below this are ignored). */
    bool save_by_category;       /**< Write typed logs to logs/<category>/<cat>.log folders. */
    bool echo_to_console;        /**< Mirror formatted logs to stdout/stderr. */
    bool use_colors;             /**< Emit ANSI color escape sequences on console. */
    char profile_name[32];       /**< Name of loaded data profile (e.g. "development"). */
    bool loaded_from_data;       /**< True if dynamically loaded from JSON data sources. */
} log_config_t;

/**
 * @struct log_stats
 * @brief Aggregate runtime telemetry report constructed by Block 5 execution procedures.
 *
 * For Designers: A scorecard showing total messages logged, file size, and status.
 *
 * For Programmers: Telemetry structure constructed by log_execute_inspect().
 */
typedef struct log_stats {
    uint64_t total_entries;       /**< Total log entries written since initialization. */
    size_t ring_buffer_count;     /**< Number of active entries in circular ring buffer. */
    size_t rotation_count;        /**< Number of file rotations executed. */
    size_t active_file_bytes;     /**< Current size in bytes of active log file. */
    log_severity_t min_severity;  /**< Current active severity filter threshold. */
    bool is_active;               /**< true if logging subsystem is initialized. */
    bool loaded_from_data;        /**< true if active profile was loaded from JSON. */
    char active_profile[32];      /**< Active profile identifier. */
} log_stats_t;

/*
--------------------------------------------------------------------------------
Fallback Safety Subsystem Integration
--------------------------------------------------------------------------------
The complete logging engine API incorporates the quarantined emergency fallback.
--------------------------------------------------------------------------------
*/
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure, static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a log severity value is within valid range.
 *
 * @param[in] sev Severity value to check.
 * @return true if severity is within canonical [0, LOG_SEV_COUNT - 1] bounds.
 */
static inline bool log_severity_is_valid(log_severity_t sev) {
    return sev >= LOG_SEV_FATAL && sev < LOG_SEV_COUNT;
}

/**
 * @brief Predicate verifying if a log category value is within valid range.
 *
 * @param[in] cat Category value to check.
 * @return true if category is within canonical [0, LOG_CAT_COUNT - 1] bounds.
 */
static inline bool log_category_is_valid(log_category_t cat) {
    return cat >= LOG_CAT_DEBUG && cat < LOG_CAT_COUNT;
}

/**
 * @brief Predicate verifying if a moral grade value is within valid bounds.
 *
 * @param[in] grade Normalized float grade value.
 * @return true if grade is within [-1.0f, +1.0f].
 */
static inline bool log_grade_is_valid(float grade) {
    return grade >= -1.0f && grade <= 1.0f;
}

/**
 * @brief Predicate verifying if the logging subsystem tripwire is clear.
 *
 * @param[in] wire Tripwire state to check.
 * @return true if tripwire is LOG_TRIPWIRE_CLEAR.
 */
static inline bool log_tripwire_is_clear(log_tripwire_t wire) {
    return wire == LOG_TRIPWIRE_CLEAR;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Profile & Manifest Resolution Cogs
Mission: Ingest, resolve, and configure logging profiles via VFS and master loader.
@triggers
  - Tripwires      : LOG_TRIPWIRE_MANIFEST_MISSING / INVALID -> probe "log.manifest_pipeline"
                     LOG_TRIPWIRE_SETTING_MISSING / INVALID  -> probe "log.setting_profile"
  - Block 4 Audits : log_validate_manifest_contract(), log_validate_setting_contract(),
                     log_preflight(), log_audit()
  - Layer 4 Tests  :
      * tests/unit/log/test_log_unit.c
        [Section 1: NOMINAL GOLDEN BASELINE / test_log_unit_always_pass]
        [Section 2: FAULT-TOLERANT TRIPWIRES & DEGRADATION / test_log_unit_never_fail]
      * tests/functional/test_log_functional.c
        [Section 2: NEVER FAIL / GRACEFUL DEGRADATION / test_log_functional_never_fail]
      * tests/integrated/test_log_integrated.c
        [Section 2: FAILURE DEGRADATION & PIPELINE RECOVERY / test_log_integrated_never_fail]
        [Section 3: TELEMETRY ASSET AUDIT / test_log_integrated_assertions_and_guarantees]
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns the default configuration resolved strictly from the data-driven manifest.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the starting recording settings (such as minimum log level and folder paths)
 * configured in the game's manifest files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Probes manifest.json to locate and hydrate the default log_config_t structure,
 * falling back to the quarantined monolith only upon critical data failure.
 *
 * @return Resolved default log_config_t structure.
 */
log_config_t log_get_default_config(void);

/**
 * @brief Loads a log configuration profile from a specified JSON data file.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a custom logging profile from a specific file (e.g. for debugging combat).
 *
 * ### For Systems Programmers & Engine Architects:
 * Deserializes JSON telemetry profile keys into caller-allocated log_config_t.
 *
 * @param[in]  filepath   Path to JSON setting file (e.g. "system/diagnostics/telemetry/development.json").
 * @param[out] out_config Output configuration structure to populate.
 * @return true if successfully loaded and validated; false on failure.
 */
bool log_load_config_from_file(const char *filepath, log_config_t *out_config);

/**
 * @brief Loads active log configuration profile from manifest "main menu" file.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the active logging preset selected in the master telemetry manifest.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves manifest JSON, queries the active_setting token, and loads that profile.
 *
 * @param[in]  manifest_path Optional path to manifest (NULL for auto-discovery).
 * @param[out] out_config    Output configuration structure to populate.
 * @return true if manifest and target profile were loaded; false on fallback.
 */
bool log_load_config_from_manifest(const char *manifest_path, log_config_t *out_config);

/**
 * @brief Loads a named configuration profile from the telemetry directory.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches logging rules by name (such as "quiet", "development", or "forensic").
 *
 * ### For Systems Programmers & Engine Architects:
 * Searches for <profile_name>.json across system domains and populates out_config.
 *
 * @param[in]  profile_name Name of the setting profile to load (e.g. "development").
 * @param[out] out_config   Output configuration structure to populate.
 * @return true if found and loaded; false otherwise.
 */
bool log_load_config_profile(const char *profile_name, log_config_t *out_config);

/**
 * @brief Returns the active configuration profile name.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks which logging preset is currently active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns pointer to the null-terminated active profile name string.
 *
 * @return Null-terminated string identifying the profile (e.g. "development").
 */
const char *log_get_active_profile_name(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: File Engine & Telemetry State Cogs
Mission: Manage directory synthesis, state queries, and perform rotation.
@triggers
  - Tripwires      : LOG_TRIPWIRE_BOUNDS_VIOLATION -> probe "log.rotation_io_bounds"
  - Block 4 Audits : log_validate_rotation_bounds(), log_audit()
  - Layer 4 Tests  :
      * tests/unit/log/test_log_unit.c
        [Section 2: FAULT-TOLERANT TRIPWIRES & DEGRADATION / test_log_unit_never_fail]
        [Section 4: RING BUFFER FIFO OVERWRITE & FILE ROTATION / test_log_unit_emergence]
      * tests/system/test_log_system.c
        [Section 2: CASCADE ROTATION & RETENTION CEILING / test_log_system_never_fail]
        [Section 4: 10,000-ENTRY CHAOS STRESS & RING BUFFER DYNAMICS / test_log_system_emergence]
--------------------------------------------------------------------------------
*/

/**
 * @brief Checks if the logging subsystem has been initialized and is active.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies whether the flight recorder is currently running and recording events.
 *
 * ### For Systems Programmers & Engine Architects:
 * Atomic check of internal s_initialized and active file stream status.
 *
 * @return true if initialized and open; false otherwise.
 */
bool log_is_active(void);

/**
 * @brief Triggers an immediate log rotation if current file size exceeds threshold.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if the current log file has gotten too large (over 1MB) and archives it,
 * starting a fresh log file so your hard drive doesn't fill up.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks s_current_bytes against s_config.max_file_size_bytes, renames active
 * log files sequentially (.1, .2, ... up to max_rotated_files), and reopens the primary stream.
 *
 * @return true if rotation occurred; false otherwise.
 */
bool log_rotate_if_needed(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Formatted Entry Dispatch Cogs
Mission: Format, serialize, and emit structured flight recorder entries.
@triggers
  - Tripwires      : LOG_TRIPWIRE_SETTING_INVALID / BOUNDS_VIOLATION -> probe "log.write_telemetry"
  - Block 4 Audits : log_validate_telemetry_integrity(), log_preflight()
  - Layer 4 Tests  :
      * tests/unit/log/test_log_unit.c
        [Section 3: INVARIANTS & CONSTRAINTS / test_log_unit_assertions_and_guarantees]
        [Section 4: RING BUFFER FIFO OVERWRITE & FILE ROTATION / test_log_unit_emergence]
      * tests/functional/test_log_functional.c
        [Section 3: INVARIANTS & SATURATION / test_log_functional_assertions_and_guarantees]
      * tests/integrated/test_log_integrated.c
        [Section 4: CATEGORICAL DISK ROUTING & HUD RING BUFFER / test_log_integrated_emergence]
      * tests/system/test_log_system.c
        [Section 1: NOMINAL HIGH-THROUGHPUT BURST / test_log_system_always_pass]
        [Section 3: FIXED-WIDTH REGEX DETERMINISM / test_log_system_assertions_and_guarantees]
--------------------------------------------------------------------------------
*/

/**
 * @brief Emits a structured log entry into the ring buffer, file, and console.
 *
 * ### For Game Designers & Non-Programmers:
 * Writes an event to the game's flight recorder journal, capturing the character involved,
 * their current moral balance percentage, and what action took place.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats a fixed-column log record with microsecond monotonic timestamp and ISO 8601 UTC time,
 * pushes it to the static circular ring buffer, and writes to disk archives with zero heap allocation.
 *
 * @param[in] severity Graded severity level.
 * @param[in] category Target functional category.
 * @param[in] entity   Entity/Character name (e.g. "Elijah", "Katrina", "System").
 * @param[in] grade    Normalized grade [-1.0f .. +1.0f] (e.g. moral sway or health).
 * @param[in] file     Source file name (__FILE__).
 * @param[in] line     Source line number (__LINE__).
 * @param[in] func     Source function name (__func__).
 * @param[in] fmt      Printf-style format string.
 * @return Monotonic sequence ID assigned to the entry, or 0 on drop/failure.
 */
uint64_t log_write_entry(log_severity_t severity,
                         log_category_t category,
                         const char *entity,
                         float grade,
                         const char *file,
                         int line,
                         const char *func,
                         const char *fmt,
                         ...);

/**
 * @brief Converts a normalized float grade [-1.0f, +1.0f] to integer percent [-100, +100].
 *
 * ### For Game Designers & Non-Programmers:
 * Converts an internal moral sway decimal into a clean -100% to +100% readout.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamps grade to [-1.0f, 1.0f] and scales to signed 32-bit integer percentage.
 */
int log_grade_to_percent(float grade);

/**
 * @brief Converts an integer percent [-100, +100] to a normalized float grade [-1.0f, +1.0f].
 *
 * ### For Game Designers & Non-Programmers:
 * Converts a whole-number percentage into an internal decimal balance ratio.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamps percent to [-100, 100] and maps to floating-point representation in [-1.0f, 1.0f].
 */
float log_percent_to_grade(int percent);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Static Ring Buffer & Telemetry Inspection Cogs
Mission: Retrieve ordered entries from in-memory ring buffer for HUD readout.
@triggers
  - Block 4 Audits : log_validate_telemetry_integrity()
  - Layer 4 Tests  :
      * tests/unit/log/test_log_unit.c
        [Section 4: RING BUFFER FIFO OVERWRITE & FILE ROTATION / test_log_unit_emergence]
      * tests/integrated/test_log_integrated.c
        [Section 4: CATEGORICAL DISK ROUTING & HUD RING BUFFER / test_log_integrated_emergence]
      * tests/system/test_log_system.c
        [Section 4: 10,000-ENTRY CHAOS STRESS & RING BUFFER DYNAMICS / test_log_system_emergence]
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves cumulative total number of log entries recorded.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns the lifetime count of events recorded since the game started.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns the 64-bit monotonic sequence counter value.
 */
uint64_t log_get_total_entries(void);

/**
 * @brief Copies recent log entries from in-memory ring buffer in chronological order.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the latest recorded events in chronological order (oldest to newest) for on-screen display.
 *
 * ### For Systems Programmers & Engine Architects:
 * Unrolls circular ring buffer into caller-provided destination array with bounds checking.
 *
 * @param[out] dest        Destination buffer array.
 * @param[in]  max_entries Maximum entries to copy.
 * @return Number of entries copied.
 */
size_t log_get_recent_entries(log_entry_t *dest, size_t max_entries);

/**
 * @brief Retrieves top recent entries in reverse chronological order (newest first).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the most recent messages with newest first, ideal for battle log scrolling.
 *
 * ### For Systems Programmers & Engine Architects:
 * Iterates backward from ring head into caller destination array.
 *
 * @param[out] dest        Destination buffer array.
 * @param[in]  max_entries Maximum entries to retrieve.
 * @return Number of entries retrieved.
 */
size_t log_get_top_entries(log_entry_t *dest, size_t max_entries);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: String & Enum Parsing Cogs
Mission: Convert between string tokens and strongly-typed enum representations.
@triggers
  - Block 4 Audits : log_preflight(), log_validate_manifest_contract()
  - Layer 4 Tests  :
      * tests/unit/log/test_log_unit.c
        [Section 3: INVARIANTS & CONSTRAINTS / test_log_unit_assertions_and_guarantees]
      * tests/functional/test_log_functional.c
        [Section 1: NOMINAL GOLDEN BASELINE FUNCTION EVALUATION / test_log_functional_always_pass]
      * tests/system/test_log_system.c
        [Section 3: FIXED-WIDTH REGEX DETERMINISM / test_log_system_assertions_and_guarantees]
--------------------------------------------------------------------------------
*/

/**
 * @brief Converts a human-readable severity string into its typed log_severity_t enum.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts text like "WARNING" or "INFO" from a config file into an internal code.
 *
 * ### For Systems Programmers & Engine Architects:
 * Case-insensitive string matching returning log_severity_t value via out_sev pointer.
 */
bool log_severity_from_string(const char *str, log_severity_t *out_sev);

/**
 * @brief Converts a human-readable category string into its typed log_category_t enum.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts category text like "combat" or "relic" into an internal routing category.
 *
 * ### For Systems Programmers & Engine Architects:
 * Case-insensitive string matching returning log_category_t value via out_cat pointer.
 */
bool log_category_from_string(const char *str, log_category_t *out_cat);

/**
 * @brief Returns the human-readable fixed-width string for a severity level.
 *
 * ### For Game Designers & Non-Programmers:
 * Gets a clean 5-letter label (such as "FATAL", "ERROR", "INFO ") for readable log files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns pointer to fixed-width statically allocated string in .rodata.
 */
const char *log_severity_name(log_severity_t severity);

/**
 * @brief Returns the human-readable fixed-width string for a category.
 *
 * ### For Game Designers & Non-Programmers:
 * Gets the display name of a subsystem category for log filtering.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns fixed-width 10-character string identifier for deterministic alignment.
 */
const char *log_category_name(log_category_t category);

/**
 * @brief Returns the folder name associated with a category (e.g. "character", "event").
 *
 * ### For Game Designers & Non-Programmers:
 * Gets the subfolder name where specialized logs for this topic are stored.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns category subfolder name string constant for filesystem routing.
 */
const char *log_category_dir_name(log_category_t category);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.6: Convenience Logging Macros
--------------------------------------------------------------------------------
*/

/**
 * @brief Master logging macro capturing source location (__FILE__, __LINE__, __func__).
 */
#define LOG_RECORD(sev, cat, entity, grade, ...) \
    log_write_entry(sev, cat, entity, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * @brief Convenience logging macro for FATAL severity events.
 */
#define LOG_FATAL(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_FATAL, cat, entity, grade, __VA_ARGS__)

/**
 * @brief Convenience logging macro for CRITICAL severity events.
 */
#define LOG_CRITICAL(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_CRITICAL, cat, entity, grade, __VA_ARGS__)

/**
 * @brief Convenience logging macro for ERROR severity events.
 */
#define LOG_ERROR(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_ERROR, cat, entity, grade, __VA_ARGS__)

/**
 * @brief Convenience logging macro for WARNING severity events.
 */
#define LOG_WARN(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_WARNING, cat, entity, grade, __VA_ARGS__)

/**
 * @brief Convenience logging macro for INFO severity events.
 */
#define LOG_INFO(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_INFO, cat, entity, grade, __VA_ARGS__)

/**
 * @brief Convenience logging macro for DEBUG severity events.
 */
#define LOG_DEBUG(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_DEBUG, cat, entity, grade, __VA_ARGS__)

/**
 * @brief Convenience logging macro for TRACE severity events.
 */
#define LOG_TRACE(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_TRACE, cat, entity, grade, __VA_ARGS__)

/**
 * @brief Convenience logging macro for character event telemetry.
 */
#define LOG_CHAR_EVENT(char_name, grade, ...) \
    log_write_entry(LOG_SEV_INFO, LOG_CAT_CHARACTER, char_name, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * @brief Convenience logging macro for in-game dialogue and messages.
 */
#define LOG_CHAR_MSG(char_name, grade, ...) \
    log_write_entry(LOG_SEV_INFO, LOG_CAT_MESSAGE, char_name, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
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
 * @brief Returns whether the active logging subsystem was configured from data sources.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if settings were loaded from actual JSON files rather than hardcoded emergency defaults.
 *
 * ### For Systems Programmers & Engine Architects:
 * Invariant contract verification query returning loaded_from_data boolean status.
 *
 * @return true if loaded from JSON data; false if using fallback code page.
 */
bool log_is_data_driven(void);

/**
 * @brief Queries whether the logging engine is currently operating in Safe Mode.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns true if the engine booted using compiled-in ROM backups or hardcoded
 * emergency presets instead of external configuration folders on disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns true when initialized and s_log.safe_mode is active (or !loaded_from_data).
 *
 * @return true if active in Safe Mode / fallback mode; false in normal disk mode.
 */
bool log_is_safe_mode(void);

/**
 * @brief Returns the current state of the logging data pipeline tripwire.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether the data-driven logging pipeline encountered an error or is running cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries the active log_tripwire_t status latch to verify pipeline contract integrity.
 *
 * @return Active log_tripwire_t enum status code.
 */
log_tripwire_t log_get_tripwire_status(void);

/**
 * @brief Returns a detailed message explaining why a tripwire was tripped.
 *
 * ### For Game Designers & Non-Programmers:
 * Gives a clear, readable explanation of what went wrong if a logging configuration error occurs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns static string pointer to the internal tripwire diagnostic message buffer.
 *
 * @return Null-terminated string describing the latched tripwire condition.
 */
const char *log_get_tripwire_details(void);

/**
 * @brief Resets the tripwire state to CLEAR (used in diagnostic test suites).
 *
 * ### For Game Designers & Non-Programmers:
 * Clears any tripwire warning once the error has been noted or handled.
 *
 * ### For Systems Programmers & Engine Architects:
 * Restores s_tripwire_status to LOG_TRIPWIRE_CLEAR and reinitializes the detail buffer.
 */
void log_reset_tripwire(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Registers logging Block 4 validation probes with the engine debugger.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects five automated logging safety tests directly to the debugger so any
 * missing telemetry file, corrupted preset, or disk rotation error automatically
 * finishes safely via Safe Mode and pauses in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Idempotently registers five `debug_validation_probe_t` entries (`log.manifest_pipeline`,
 * `log.setting_profile`, `log.rotation_io_bounds`, `log.write_telemetry`, and
 * `log.fallback_bios`) via `debug_register_validation_probe()`.
 */
void log_register_debug_probes(void);

/**
 * @brief Block 4 validation cog asserting telemetry manifest pipeline & ROM table health.
 *
 * @triggers
 * - Probe ID     : `"log.manifest_pipeline"` (`LOG_TRIPWIRE_MANIFEST_MISSING`/`INVALID`)
 * - Source       : Sub-Assembly 3.1 (`log_load_config_from_manifest`, `log_get_default_config`)
 * - Layer 4 Test : `tests/unit/log/test_log_unit.c` (Sec 2), `tests/integrated/test_log_integrated.c` (Sec 2 & 3)
 *
 * @return diag_result_t reporting `SEVERITY_PASS` or `SEVERITY_CRITICAL` on manifest fault.
 */
diag_result_t log_validate_manifest_contract(void);

/**
 * @brief Block 4 validation cog asserting telemetry setting profile & config invariants.
 *
 * @triggers
 * - Probe ID     : `"log.setting_profile"` (`LOG_TRIPWIRE_SETTING_MISSING`/`INVALID`)
 * - Source       : Sub-Assembly 3.1 (`log_parse_config_payload`, `log_load_config_from_file`,
 *                  `log_load_config_profile`)
 * - Layer 4 Test : `tests/unit/log/test_log_unit.c` (Sec 2),
 *                  `tests/functional/test_log_functional.c` (Sec 2),
 *                  `tests/integrated/test_log_integrated.c` (Sec 3)
 *
 * @return diag_result_t reporting `SEVERITY_PASS` or `SEVERITY_CRITICAL` on setting fault.
 */
diag_result_t log_validate_setting_contract(void);

/**
 * @brief Block 4 validation cog asserting log rotation ceiling and storage stream bounds.
 *
 * @triggers
 * - Probe ID     : `"log.rotation_io_bounds"` (`LOG_TRIPWIRE_BOUNDS_VIOLATION`)
 * - Source       : Sub-Assembly 3.2 & 5.2 (`rotate_archive_stream`, `log_rotate_if_needed`,
 *                  `log_init` validation gate)
 * - Layer 4 Test : `tests/unit/log/test_log_unit.c` (Sec 2 & 4),
 *                  `tests/system/test_log_system.c` (Sec 2 & 4)
 *
 * @return diag_result_t reporting `SEVERITY_PASS` or `SEVERITY_CRITICAL` on rotation/bounds breach.
 */
diag_result_t log_validate_rotation_bounds(void);

/**
 * @brief Block 4 validation cog asserting ring buffer FIFO watermarks & dispatch stream state.
 *
 * @triggers
 * - Probe ID     : `"log.write_telemetry"`
 * - Source       : Sub-Assembly 3.3 & 3.4 (`log_write_entry`, `log_dispatch_category_file`,
 *                  `log_ring_push`)
 * - Layer 4 Test : `tests/unit/log/test_log_unit.c` (Sec 3 & 4),
 *                  `tests/integrated/test_log_integrated.c` (Sec 4),
 *                  `tests/system/test_log_system.c` (Sec 1, 3, 4)
 *
 * @return diag_result_t reporting `SEVERITY_PASS` or `SEVERITY_CRITICAL` on ring/dispatch breach.
 */
diag_result_t log_validate_telemetry_integrity(void);

/**
 * @brief Executes machine pre-flight diagnostic checks on log subsystem invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs an instant diagnostic check before the game starts to verify that logging
 * limits, moral alignment grade math, and status labels are completely accurate.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates core telemetry invariants, grade clamping bounds, label array mappings,
 * string-to-enum conversions, manifest resolution, and emergency fallback sanity.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t log_preflight(void);

/**
 * @brief High-level diagnostic self-audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Performs an automated health check on the logging engine to ensure recording,
 * rotation, and ring buffers work properly before live gameplay.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes preflight checks, validates sequence monotonicity, tests ring buffer
 * retrieval, checks profile loading, and exercises fallback code page.
 *
 * @param[in] verbose Enable verbose trace output during audit.
 * @return diag_result_t diagnostic outcome indicating overall subsystem health.
 */
diag_result_t log_audit(bool verbose);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
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
 * @brief Constructs a structured runtime telemetry statistics report.
 *
 * ### For Game Designers & Non-Programmers:
 * Collects a summary of the game's flight recorder (total messages recorded,
 * ring buffer usage, and active profile name) for debugging and status displays.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection procedure populating a `log_stats_t` record
 * directly from static ring counters and current sequence numbers.
 *
 * @param[out] out_stats Destination statistics report object.
 * @return true if statistics were populated; false on NULL parameter.
 */
bool log_execute_inspect(log_stats_t *out_stats);

/**
 * @brief Formats an ASCII diagnostic inspection card for logging engine telemetry.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates an easy-to-read text card showing how many log messages have been
 * recorded, active file sizes, and whether logging is running in data-driven mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a standardized 60-column fixed-width ASCII telemetry card into
 * `out_buf`. Requires at least 320 bytes buffer capacity.
 *
 * @param[in]  stats    Optional pre-gathered stats (or NULL to inspect live engine).
 * @param[out] out_buf  Destination character buffer (minimum 320 bytes).
 * @param[in]  buf_size Size of destination buffer.
 * @return true if inspection card was formatted cleanly; false on error or truncation.
 */
bool log_execute_dump_card(const log_stats_t *stats, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the logging subsystem, creates directories, and opens active file.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts up the flight recorder at game boot so errors, gameplay choices, and
 * combat actions can be written to disk safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets up file streams, rotates existing logs if capacity is exceeded, creates
 * output directory structure, and initializes internal sequence ring buffer.
 *
 * @param[in] config Pointer to logging configuration (NULL uses defaults).
 * @return true on successful initialization; false on filesystem error.
 */
bool log_init(const log_config_t *config);

/**
 * @brief Flushes active file streams and shuts down logging subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Flushes all pending messages to disk and safely closes the log file when
 * the game exits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Flushes active file descriptor via fflush(), resets sequence counters, marks
 * subsystem inactive, and clears the internal file stream pointer.
 */
void log_shutdown(void);

#endif /* TENEBRIS_LUX_LOG_H */
