// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          util/log/log.h
// @id            core.util.log
// @layer         Layer 0: Core Utilities
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-10-29
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   High-performance, zero-heap logging, telemetry, and forensic flight recorder
//   subsystem. Delivers structured, machine-greppable, and human-readable audit
//   trails with ISO 8601 timestamps, microsecond monotonic deltas, gradient
//   moral/diagnostic grading, automated file size rotation, and in-memory ring
//   buffering for live viewport HUD inspection.
//
//   Organized into the dedicated Layer 0 'util/log/' subsystem directory to
//   enforce modular encapsulation and 1:1 parity with 'data/log/'.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: core.util.manifest, core.util.time, core.util.math
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Static ring buffer & line queues).
//   - Concurrency / Safety:  REENTRANT_PURE (Synchronized I/O, reentrant writes).
//   - Latency Budget:        < 15 microseconds per formatted entry write.
//
// AXIOMATIC INVARIANTS:
//   1. Monotonic sequence counter: ID strictly increments per entry (ID#00000001).
//   2. Gradient grade range: Normalized [-1.0f, +1.0f] maps to [-100, +100].
//   3. Fixed-width token columns guarantee deterministic regex/grep parsing.
//   4. Automated rotation bounds: Log file never exceeds configured byte ceiling.
//   5. Static memory boundary: In-flight ring buffer never dynamic allocates.
// =============================================================================

#ifndef TENEBRIS_LUX_UTIL_LOG_LOG_H
#define TENEBRIS_LUX_UTIL_LOG_LOG_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Forensic Telemetry Engine
// Subsystem: util/log
// =============================================================================

#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern const module_metadata_t log_manifest;
diag_result_t log_preflight(void);

#define LOG_MAX_ENTITY_LEN       16
#define LOG_MAX_MESSAGE_LEN      256
#define LOG_MAX_LINE_BUFFER      512
#define LOG_DEFAULT_MAX_BYTES    (1024 * 1024) /* 1 MB rotation threshold */
#define LOG_DEFAULT_MAX_FILES    25            /* Retain top 10-25 rotated archives */
#define LOG_RING_BUFFER_CAPACITY 64            /* In-flight memory flight recorder */

/**
 * Graded severity taxonomy for engine diagnostics and telemetry events.
 * Ordered from highest severity (FATAL) down to finest detail (TRACE).
 */
typedef enum {
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
 * Functional categorization of logging events for focused channel filtering.
 */
typedef enum {
    LOG_CAT_DEBUG = 0,    /**< Low-level developer engine trace. */
    LOG_CAT_LOG,          /**< Standard system lifecycle and startup logs. */
    LOG_CAT_EVENT,        /**< Significant world, combat, or quest trigger. */
    LOG_CAT_MESSAGE,      /**< Narrative dialogue or NPC communicative prompt. */
    LOG_CAT_CHARACTER,    /**< Protagonist/enemy spatial & relic stance action. */
    LOG_CAT_COUNT
} log_category_t;

/**
 * Static telemetry record stored in memory and emitted to disk.
 */
typedef struct {
    uint64_t sequence_id;                  /**< Monotonic sequence identifier. */
    uint64_t monotonic_us;                /**< Monotonic clock delta (microseconds). */
    char iso_timestamp[32];               /**< UTC ISO 8601: YYYY-MM-DDTHH:MM:SS.uuuuuuZ. */
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
 * Diagnostic tripwire state for the data-driven logging pipeline.
 *
 * Enforces the "Don't start a machine you know has loose bolts" axiom by
 * halting boot or emitting diagnostic manifests when data assets are compromised.
 */
typedef enum {
    LOG_TRIPWIRE_CLEAR = 0,        /**< No tripwires active; data pipeline healthy. */
    LOG_TRIPWIRE_MANIFEST_MISSING, /**< data/log/manifest.json could not be found. */
    LOG_TRIPWIRE_MANIFEST_INVALID, /**< Manifest unparseable or lacks menu entries. */
    LOG_TRIPWIRE_SETTING_MISSING,  /**< Designated setting JSON profile not found. */
    LOG_TRIPWIRE_SETTING_INVALID,  /**< Setting file syntax or attributes corrupted. */
    LOG_TRIPWIRE_BOUNDS_VIOLATION  /**< Configuration violated byte/file limits. */
} log_tripwire_t;

/**
 * Runtime configuration for the logging subsystem.
 *
 * Supports data-driven initialization from JSON setting profiles with
 * compiled fallback safety defaults.
 */
typedef struct {
    char log_dir[64];            /**< Target directory for log files (e.g. "logs"). */
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

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 0 (Core Utilities) | Machine: Forensic Telemetry Engine
// =============================================================================

/**
 * @brief Returns the current state of the logging data pipeline tripwire.
 *
 * @return Active log_tripwire_t status code.
 */
log_tripwire_t log_get_tripwire_status(void);

/**
 * @brief Returns a detailed message explaining why a tripwire was tripped.
 *
 * @return Null-terminated diagnostic string.
 */
const char *log_get_tripwire_details(void);

/**
 * @brief Resets the tripwire state to CLEAR (used in diagnostic test suites).
 */
void log_reset_tripwire(void);

/**
 * @brief Returns the default configuration resolved strictly from the data-driven manifest.
 *
 * Resolves from data/log/manifest.json. If data assets are missing or corrupted,
 * this function trips a diagnostic tripwire and flags loaded_from_data == false.
 *
 * @return Resolved default log_config_t structure.
 */
log_config_t log_get_default_config(void);

/**
 * @brief Converts a human-readable severity string into its typed log_severity_t enum.
 *
 * Parses "FATAL", "CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE".
 *
 * @param[in]  str     Case-insensitive severity name.
 * @param[out] out_sev Output pointer for parsed severity.
 * @return True if matched, false if invalid token.
 */
bool log_severity_from_string(const char *str, log_severity_t *out_sev);

/**
 * @brief Converts a human-readable category string into its typed log_category_t enum.
 *
 * Parses "SYSTEM", "LOG", "EVENT", "MESSAGE", "CHARACTER", "DEBUG".
 *
 * @param[in]  str     Case-insensitive category name.
 * @param[out] out_cat Output pointer for parsed category.
 * @return True if matched, false if invalid token.
 */
bool log_category_from_string(const char *str, log_category_t *out_cat);

/**
 * @brief Loads a log configuration profile from a specified JSON data file.
 *
 * @param[in]  filepath   Path to JSON setting file (e.g. "data/log/development.json").
 * @param[out] out_config Output configuration structure to populate.
 * @return True if successfully parsed, false on missing file or syntax error.
 */
bool log_load_config_from_file(const char *filepath, log_config_t *out_config);

/**
 * @brief Loads the active log configuration profile from a manifest "main menu" file.
 *
 * @param[in]  manifest_path Path to log manifest JSON (NULL for auto-discovery).
 * @param[out] out_config    Output configuration structure to populate.
 * @return True if manifest and target profile were loaded, false on fallback.
 */
bool log_load_config_from_manifest(const char *manifest_path, log_config_t *out_config);

/**
 * @brief Loads a named configuration profile from the data/log/ directory.
 *
 * @param[in]  profile_name Name of the setting profile to load (e.g. "development").
 * @param[out] out_config   Output configuration structure to populate.
 * @return True if found and loaded, false otherwise.
 */
bool log_load_config_profile(const char *profile_name, log_config_t *out_config);

/**
 * @brief Returns the active configuration profile name (e.g. "development", "fallback").
 *
 * @return Null-terminated string identifying the profile.
 */
const char *log_get_active_profile_name(void);

/**
 * @brief Returns whether the active logging subsystem was configured from data sources.
 *
 * @return True if loaded from JSON data, false if using fallback code page.
 */
bool log_is_data_driven(void);

/**
 * @brief Initializes the logging subsystem, creates directories, and opens active file.
 *
 * @param[in] config Pointer to logging configuration (NULL uses defaults).
 * @return True on successful initialization, false on filesystem error.
 */
bool log_init(const log_config_t *config);

/**
 * @brief Flushes active file streams and shuts down logging subsystem.
 */
void log_shutdown(void);

/**
 * @brief Checks if the logging subsystem has been initialized and is active.
 *
 * @return True if initialized and open, false otherwise.
 */
bool log_is_active(void);

/**
 * @brief Converts a normalized float grade [-1.0f, +1.0f] to integer percent [-100, +100].
 *
 * @param[in] grade Normalized grade value.
 * @return Integer percentage clamped to [-100, +100].
 */
int log_grade_to_percent(float grade);

/**
 * @brief Converts an integer percent [-100, +100] to a normalized float grade [-1.0f, +1.0f].
 *
 * @param[in] percent Integer percentage [-100, +100].
 * @return Normalized floating-point grade in [-1.0f, +1.0f].
 */
float log_percent_to_grade(int percent);

/**
 * @brief Returns the human-readable fixed-width string for a severity level.
 *
 * @param[in] severity Graded severity enum.
 * @return Fixed-width severity label (e.g. "[INFO ]").
 */
const char *log_severity_name(log_severity_t severity);

/**
 * @brief Returns the human-readable fixed-width string for a category.
 *
 * @param[in] category Domain category enum.
 * @return Fixed-width category label (e.g. "[EVENT ]").
 */
const char *log_category_name(log_category_t category);

/**
 * @brief Returns the folder name associated with a category (e.g. "character", "event").
 *
 * @param[in] category Domain category enum.
 * @return Directory name string.
 */
const char *log_category_dir_name(log_category_t category);

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
uint64_t log_write_entry(
    log_severity_t severity,
    log_category_t category,
    const char *entity,
    float grade,
    const char *file,
    int line,
    const char *func,
    const char *fmt,
    ...
);

/**
 * @brief Retrieves the total number of log entries recorded since initialization.
 *
 * @return Cumulative monotonic log count.
 */
uint64_t log_get_total_entries(void);

/**
 * @brief Copies recent log entries from the in-memory circular ring buffer.
 *
 * Entries are copied in chronological order (oldest to newest).
 *
 * @param[out] dest        Output buffer array for log entries.
 * @param[in]  max_entries Maximum entries to copy.
 * @return Number of entries actually copied.
 */
size_t log_get_recent_entries(log_entry_t *dest, size_t max_entries);

/**
 * @brief Retrieves the top 10-25 most recent log entries in reverse chronological order.
 *
 * Newest entries are placed first, ideal for HUD and on-screen debug consoles.
 *
 * @param[out] dest        Output buffer array for top log entries.
 * @param[in]  max_entries Maximum entries to retrieve (typically 10 to 25).
 * @return Number of entries retrieved.
 */
size_t log_get_top_entries(log_entry_t *dest, size_t max_entries);

/**
 * @brief Triggers an immediate log rotation if current file size exceeds threshold.
 *
 * @return True if rotation occurred, false otherwise.
 */
bool log_rotate_if_needed(void);

// -----------------------------------------------------------------------------
// [ CONVENIENCE LOGGING MACROS ]
// -----------------------------------------------------------------------------

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

// Specialized character & narrative action macros
#define LOG_CHAR_EVENT(char_name, grade, ...) \
    log_write_entry(LOG_SEV_INFO, LOG_CAT_CHARACTER, char_name, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_CHAR_MSG(char_name, grade, ...) \
    log_write_entry(LOG_SEV_INFO, LOG_CAT_MESSAGE, char_name, grade, __FILE__, __LINE__, __func__, __VA_ARGS__)

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Forensic Telemetry Engine
// =============================================================================

/**
 * @brief Diagnostic self-audit verifying formatting buffers, grade clamping, and rotation.
 *
 * @param[in] verbose Enable verbose trace output during audit.
 * @return Diagnostic verification result.
 */
diag_result_t log_audit(bool verbose);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Forensic Telemetry Engine
// =============================================================================

#endif /* TENEBRIS_LUX_UTIL_LOG_LOG_H */
