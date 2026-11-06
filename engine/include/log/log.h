/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          log.h
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
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
  system/diagnostics/telemetry/ (manifest.json, default.json, development.json,
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

#ifndef TENEBRIS_LUX_UTIL_LOG_LOG_H
#define TENEBRIS_LUX_UTIL_LOG_LOG_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy (Downward Dependency Ladder)
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Telemetry Buffer Limits
--------------------------------------------------------------------------------
*/

#define LOG_MAX_ENTITY_LEN       16
#define LOG_MAX_MESSAGE_LEN      256
#define LOG_MAX_LINE_BUFFER      512
#define LOG_DEFAULT_MAX_BYTES    (1024 * 1024) /**< 1 MB default rotation ceiling */
#define LOG_DEFAULT_MAX_FILES    25            /**< Retain up to 25 rotated archives */
#define LOG_RING_BUFFER_CAPACITY 64            /**< Fixed circular HUD flight recorder */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
*/

/**
 * @enum log_severity
 * @brief Graded severity taxonomy for engine diagnostics and telemetry events.
 * Ordered from highest severity (FATAL) down to finest detail (TRACE).
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

/**
 * Manifest record for the Zero-Heap Logging Engine.
 */
extern const module_metadata_t log_manifest;

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
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns the default configuration resolved strictly from the data-driven manifest.
 *
 * @return Resolved default log_config_t structure.
 */
log_config_t log_get_default_config(void);

/**
 * @brief Loads a log configuration profile from a specified JSON data file.
 *
 * @param[in]  filepath   Path to JSON setting file (e.g. "system/diagnostics/telemetry/development.json").
 * @param[out] out_config Output configuration structure to populate.
 * @return true if successfully loaded and validated; false on failure.
 */
bool log_load_config_from_file(const char *filepath, log_config_t *out_config);

/**
 * @brief Loads active log configuration profile from manifest "main menu" file.
 *
 * @param[in]  manifest_path Optional path to manifest (NULL for auto-discovery).
 * @param[out] out_config    Output configuration structure to populate.
 * @return true if manifest and target profile were loaded; false on fallback.
 */
bool log_load_config_from_manifest(const char *manifest_path, log_config_t *out_config);

/**
 * @brief Loads a named configuration profile from the telemetry directory.
 *
 * @param[in]  profile_name Name of the setting profile to load (e.g. "development").
 * @param[out] out_config   Output configuration structure to populate.
 * @return true if found and loaded; false otherwise.
 */
bool log_load_config_profile(const char *profile_name, log_config_t *out_config);

/**
 * @brief Returns the active configuration profile name.
 *
 * @return Null-terminated string identifying the profile (e.g. "development").
 */
const char *log_get_active_profile_name(void);

/**
 * @brief Returns whether the active logging subsystem was configured from data sources.
 *
 * @return true if loaded from JSON data; false if using fallback code page.
 */
bool log_is_data_driven(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Subsystem Lifecycle & File Engine Cogs
Mission: Initialize streams, manage directory synthesis, and perform rotation.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the logging subsystem, creates directories, and opens active file.
 *
 * @param[in] config Pointer to logging configuration (NULL uses defaults).
 * @return true on successful initialization; false on filesystem error.
 */
bool log_init(const log_config_t *config);

/**
 * @brief Flushes active file streams and shuts down logging subsystem.
 */
void log_shutdown(void);

/**
 * @brief Checks if the logging subsystem has been initialized and is active.
 *
 * @return true if initialized and open; false otherwise.
 */
bool log_is_active(void);

/**
 * @brief Triggers an immediate log rotation if current file size exceeds threshold.
 *
 * @return true if rotation occurred; false otherwise.
 */
bool log_rotate_if_needed(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Formatted Entry Dispatch Cogs
Mission: Format, serialize, and emit structured flight recorder entries.
--------------------------------------------------------------------------------
*/

/**
 * @brief Emits a structured log entry into the ring buffer, file, and console.
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
 */
int log_grade_to_percent(float grade);

/**
 * @brief Converts an integer percent [-100, +100] to a normalized float grade [-1.0f, +1.0f].
 */
float log_percent_to_grade(int percent);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Static Ring Buffer & Telemetry Inspection Cogs
Mission: Retrieve ordered entries from in-memory ring buffer for HUD readout.
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves cumulative total number of log entries recorded.
 */
uint64_t log_get_total_entries(void);

/**
 * @brief Copies recent log entries from in-memory ring buffer in chronological order.
 *
 * @param[out] dest        Destination buffer array.
 * @param[in]  max_entries Maximum entries to copy.
 * @return Number of entries copied.
 */
size_t log_get_recent_entries(log_entry_t *dest, size_t max_entries);

/**
 * @brief Retrieves top recent entries in reverse chronological order (newest first).
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
--------------------------------------------------------------------------------
*/

/**
 * @brief Converts a human-readable severity string into its typed log_severity_t enum.
 */
bool log_severity_from_string(const char *str, log_severity_t *out_sev);

/**
 * @brief Converts a human-readable category string into its typed log_category_t enum.
 */
bool log_category_from_string(const char *str, log_category_t *out_cat);

/**
 * @brief Returns the human-readable fixed-width string for a severity level.
 */
const char *log_severity_name(log_severity_t severity);

/**
 * @brief Returns the human-readable fixed-width string for a category.
 */
const char *log_category_name(log_category_t category);

/**
 * @brief Returns the folder name associated with a category (e.g. "character", "event").
 */
const char *log_category_dir_name(log_category_t category);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.6: Convenience Logging Macros
--------------------------------------------------------------------------------
*/

#define LOG_RECORD(sev, cat, entity, grade, ...) \
    log_write_entry(sev, cat, entity, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_FATAL(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_FATAL, cat, entity, grade, __VA_ARGS__)

#define LOG_CRITICAL(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_CRITICAL, cat, entity, grade, __VA_ARGS__)

#define LOG_ERROR(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_ERROR, cat, entity, grade, __VA_ARGS__)

#define LOG_WARN(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_WARNING, cat, entity, grade, __VA_ARGS__)

#define LOG_INFO(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_INFO, cat, entity, grade, __VA_ARGS__)

#define LOG_DEBUG(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_DEBUG, cat, entity, grade, __VA_ARGS__)

#define LOG_TRACE(cat, entity, grade, ...) \
    LOG_RECORD(LOG_SEV_TRACE, cat, entity, grade, __VA_ARGS__)

#define LOG_CHAR_EVENT(char_name, grade, ...) \
    log_write_entry(LOG_SEV_INFO, LOG_CAT_CHARACTER, char_name, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_CHAR_MSG(char_name, grade, ...) \
    log_write_entry(LOG_SEV_INFO, LOG_CAT_MESSAGE, char_name, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwires & Audits
Mission: Verify log subsystem invariants, grade clamping, and rotation.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on log subsystem invariants.
 *
 * @return diag_result_t reporting categorized diagnostic severity.
 */
diag_result_t log_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @param[in] verbose Enable verbose trace output during audit.
 * @return diag_result_t diagnostic outcome.
 */
diag_result_t log_audit(bool verbose);

/**
 * @brief Returns the current state of the logging data pipeline tripwire.
 */
log_tripwire_t log_get_tripwire_status(void);

/**
 * @brief Returns a detailed message explaining why a tripwire was tripped.
 */
const char *log_get_tripwire_details(void);

/**
 * @brief Resets the tripwire state to CLEAR (used in diagnostic test suites).
 */
void log_reset_tripwire(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures & Object Constructors
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs a structured runtime telemetry statistics report.
 *
 * @param[out] out_stats Destination statistics report object.
 * @return true if statistics were populated; false on NULL parameter.
 */
bool log_execute_inspect(log_stats_t *out_stats);

#endif /* TENEBRIS_LUX_UTIL_LOG_LOG_H */
