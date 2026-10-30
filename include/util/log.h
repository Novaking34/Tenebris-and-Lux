// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          log.h
// @id            core.util.log
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
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

#ifndef TENEBRIS_LUX_LOG_H
#define TENEBRIS_LUX_LOG_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Forensic Telemetry Engine
// =============================================================================

#include "util/manifest.h"
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
 * Runtime configuration for the logging subsystem.
 */
typedef struct {
    const char *log_dir;         /**< Target directory for log files (e.g. "logs"). */
    const char *base_filename;   /**< Primary active log filename (e.g. "tenebris_lux.log"). */
    size_t max_file_size_bytes;  /**< Byte threshold triggering file rotation. */
    size_t max_rotated_files;    /**< Maximum archive files retained (10-25, default 25). */
    log_severity_t min_severity; /**< Filter cutoff (events below this are ignored). */
    bool save_by_category;       /**< Write typed logs to logs/<category>/<cat>.log folders. */
    bool echo_to_console;        /**< Mirror formatted logs to stdout/stderr. */
    bool use_colors;             /**< Emit ANSI color escape sequences on console. */
} log_config_t;

// =============================================================================
// BLOCK 3: BODY BLOCK / API DECLARATIONS
// Stratum: Layer 0 (Core Utilities) | Machine: Forensic Telemetry Engine
// =============================================================================

/**
 * Returns default configuration settings.
 */
log_config_t log_get_default_config(void);

/**
 * Initializes the logging subsystem, creates directories, and opens active file.
 *
 * @param config Pointer to logging configuration (NULL uses defaults).
 * @return True on successful initialization, false on filesystem error.
 */
bool log_init(const log_config_t *config);

/**
 * Flushes active file streams and shuts down logging subsystem.
 */
void log_shutdown(void);

/**
 * Checks if the logging subsystem has been initialized and is active.
 */
bool log_is_active(void);

/**
 * Converts a normalized float grade [-1.0f, +1.0f] to integer percent [-100, +100].
 */
int log_grade_to_percent(float grade);

/**
 * Converts an integer percent [-100, +100] to a normalized float grade [-1.0f, +1.0f].
 */
float log_percent_to_grade(int percent);

/**
 * Returns the human-readable fixed-width string for a severity level.
 */
const char *log_severity_name(log_severity_t severity);

/**
 * Returns the human-readable fixed-width string for a category.
 */
const char *log_category_name(log_category_t category);

/**
 * Returns the folder name associated with a category (e.g. "character", "event").
 */
const char *log_category_dir_name(log_category_t category);

/**
 * Emits a structured log entry into the ring buffer, file, and console.
 *
 * @param severity Graded severity level.
 * @param category Target functional category.
 * @param entity Entity/Character name (e.g. "Elijah", "Katrina", "System").
 * @param grade Normalized grade [-1.0f .. +1.0f] (e.g. moral sway or health).
 * @param file Source file name (__FILE__).
 * @param line Source line number (__LINE__).
 * @param func Source function name (__func__).
 * @param fmt Printf-style format string.
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
 * Retrieves the total number of log entries recorded since initialization.
 */
uint64_t log_get_total_entries(void);

/**
 * Copies the recent log entries from the in-memory circular ring buffer in
 * chronological order.
 *
 * @param dest Output buffer array for log entries.
 * @param max_entries Maximum entries to copy.
 * @return Number of entries actually copied.
 */
size_t log_get_recent_entries(log_entry_t *dest, size_t max_entries);

/**
 * Retrieves the top 10-25 most recent log entries in reverse chronological order
 * (newest first), ideal for on-screen debug consoles or crash inspect summaries.
 *
 * @param dest Output buffer array for top log entries.
 * @param max_entries Maximum entries to retrieve (typically 10 to 25).
 * @return Number of entries retrieved.
 */
size_t log_get_top_entries(log_entry_t *dest, size_t max_entries);

/**
 * Triggers an immediate log rotation if current file size exceeds threshold.
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
 * Diagnostic self-audit verifying formatting buffers, grade clamping, and rotation.
 *
 * @param verbose Enable verbose trace output during audit.
 * @return Diagnostic verification result.
 */
diag_result_t log_audit(bool verbose);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Forensic Telemetry Engine
// =============================================================================

#endif /* TENEBRIS_LUX_LOG_H */
