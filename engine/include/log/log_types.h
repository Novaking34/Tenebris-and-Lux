/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/log/log_types.h
@id            cog.log.types
@layer         Layer 0: Core Utilities
@archetype     pure_types | stateless
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry Shared Vocabulary & Types
@role          Public Contract & Seam: Zero-heap telemetry bounds, severity &
               category taxonomies, flight-record POD structs, and O(1) guards.
@maturity      Rev 5 Baseline | Extracted cleanly from cog.log so cog.log.fallback compiles standalone.
@chassis       zero-heap | Zero .bss/.data state; immutable limits & caller-owned POD value types only.
@scale         O(1) enum/range predicates; LOG_MAX_ENTITY_LEN (16), LOG_MAX_MESSAGE_LEN (256), ring cap 64.
@seams         in: none (pure vocabulary) | uses: core/manifest.h | probes: none (stateless) | dag: core.util.log_types
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Pure stateless types invariant: declares zero .bss/.data buffers or stateful functions.
               3. Standalone leaf contract: included cleanly by cog.log and quarantined cog.log.fallback.
               4. Non-anonymous DWARF tags preserved on all enums and structs with C++ extern "C" support.

PURPOSE & ARCHITECTURAL ROLE:
  Provides the shared, standalone Layer 0 telemetry vocabulary for the
  Cornerstone Game Engine so both primary flight recording (`cog.log`) and
  Tier-0 Safe-Mode BIOS (`cog.log.fallback`) share exact ABI structs without
  mid-file includes or circular header dependencies.

  ### For Game Designers & Non-Programmers:
  Defines the shared dictionary of log importance levels (FATAL down to TRACE),
  thematic channels (CHARACTER, MESSAGE, EVENT, IO), and settings layouts used
  across telemetry config JSONs and flight recorder status readouts.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Pure caller-owned POD value objects (`log_entry_t`,
    `log_config_t`, `log_stats_t`); completely stateless and reentrant.
  - Extension Seam: Add severities, channels, or config fields in one canonical
    spot inherited simultaneously by operational and BIOS compilation units.
  - Explicit Non-Goal: Holds zero storage buffers, stream handles, or JSON I/O
    routines (owned by `cog.log` and `cog.log.fallback`).
================================================================================
*/

#ifndef CORNERSTONE_LOG_LOG_TYPES_H
#define CORNERSTONE_LOG_LOG_TYPES_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Shared Pure Structural Vocabulary, Value Bounds & Stateless Predicates )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.log.types
Mission:       Provide shared telemetry limits, tagged structs/enums, and O(1) predicates.
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
Sub-Assembly 2.1: Pre-Conditions, Physical Bounds & Compile-Time Proofs
--------------------------------------------------------------------------------
Fixed compile-time sizing bounds shared across the primary flight recorder and
the Tier-0 BIOS replay pipeline.

### For Game Designers & Non-Programmers:
Ensures entity names (16 bytes), single-line telemetry events (256 bytes), and
live 64-entry HUD rings stay crisp, readable, and bounded against log spam.

### For Systems Programmers & Engine Architects:
Guarantees 100% zero-heap static sizing (ALLOC_ZERO_HEAP) with compile-time
proofs guarding line buffer overflow and nonzero circular capacity.
--------------------------------------------------------------------------------
*/

/**
 * @brief Maximum length in bytes for character or entity name tokens.
 * Sized to 16 bytes to fit standard actor, subsystem, or loader tags cleanly.
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
 * Sized to 1,048,576 bytes (1 MiB) to prevent disk bloat during long sessions.
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

static_assert(
    LOG_MAX_LINE_BUFFER > (LOG_MAX_ENTITY_LEN + LOG_MAX_MESSAGE_LEN + 64),
    "Formatted log line buffer must accommodate timestamp, category, entity, and payload"
);
static_assert(
    LOG_RING_BUFFER_CAPACITY > 0,
    "Flight recorder ring capacity must remain strictly positive"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Stateless Vocabulary Module Identifier
--------------------------------------------------------------------------------
Pure types header identity anchor; carries no link-time runtime manifest object.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Telemetry Shared Vocabulary header. */
#define LOG_TYPES_MODULE_ID      "log.types"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Shared Structural Entity & Telemetry Value Definitions
--------------------------------------------------------------------------------
Canonical non-anonymous enums and POD structures shared across operational and
quarantined BIOS telemetry cogs.
--------------------------------------------------------------------------------
*/

/**
 * @enum log_severity
 * @brief Graded severity taxonomy for engine diagnostics and telemetry events.
 *
 * Ordered from highest severity (FATAL) down to finest detail (TRACE).
 *
 * ### For Game Designers & Non-Programmers:
 * Defines how critical an event is:
 *   - FATAL/CRITICAL: Showstoppers that threaten gameplay continuity.
 *   - ERROR/WARNING: Problems handled gracefully with automatic fallbacks.
 *   - INFO: Normal story, quest, and milestone announcements.
 *   - DEBUG/TRACE: Granular technical values for combat balance and timing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Strict integer ordering enables threshold filtering and weight-table lookup.
 */
typedef enum log_severity {
    LOG_SEV_FATAL = 0,    /**< Unrecoverable failure or fatal state corruption. */
    LOG_SEV_CRITICAL,     /**< Severe failure threatening gameplay loop. */
    LOG_SEV_ERROR,        /**< Discrete operation failure with fallback. */
    LOG_SEV_WARNING,      /**< Suspicious condition or degraded state. */
    LOG_SEV_INFO,         /**< Major milestone or narrative state shift. */
    LOG_SEV_DEBUG,        /**< Granular spatial, coordinate, or stance telemetry. */
    LOG_SEV_TRACE,        /**< High-frequency per-tick calculation telemetry. */
    LOG_SEV_COUNT         /**< Enumerator bound sentinel (not a valid severity). */
} log_severity_t;

/**
 * @enum log_category
 * @brief Functional categorization of logging events for focused channel filtering.
 *
 * ### For Game Designers & Non-Programmers:
 * Divides events into distinct thematic channels (`CHARACTER`, `MESSAGE`,
 * `EVENT`, `IO`, `LOG`, `DEBUG`) so designers can inspect one lane at a time.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed enum indexes into static descriptor rows and categorical archive streams.
 */
typedef enum log_category {
    LOG_CAT_DEBUG = 0,    /**< Low-level developer engine trace. */
    LOG_CAT_LOG,          /**< Standard system lifecycle and startup logs. */
    LOG_CAT_EVENT,        /**< Significant world, combat, or quest trigger. */
    LOG_CAT_MESSAGE,      /**< Narrative dialogue or NPC communicative prompt. */
    LOG_CAT_CHARACTER,    /**< Actor/enemy spatial and equipment stance action. */
    LOG_CAT_IO,           /**< Filesystem, VFS, JSON & BIOS ROM I/O telemetry. */
    LOG_CAT_COUNT         /**< Enumerator bound sentinel (not a valid category). */
} log_category_t;

/**
 * @struct log_entry
 * @brief Static telemetry record stored in memory ring buffer and emitted to disk.
 *
 * ### For Game Designers & Non-Programmers:
 * A discrete "black box" flight recorder entry capturing when an event occurred,
 * which actor or module triggered it, and the normalized scalar grade [-1.0..+1.0].
 *
 * ### For Systems Programmers & Engine Architects:
 * 100% stack/static fixed-layout record. Source file/func pointers must point to
 * static `.rodata` string literals (`__FILE__`, `__func__`) rather than stack storage.
 */
typedef struct log_entry {
    uint64_t       sequence_id;                  /**< Monotonic sequence identifier. */
    uint64_t       monotonic_us;                 /**< Monotonic clock delta in microseconds. */
    char           iso_timestamp[32];            /**< UTC ISO 8601 timestamp string. */
    log_severity_t severity;                     /**< Graded severity rank. */
    log_category_t category;                     /**< Domain channel category. */
    char           entity[LOG_MAX_ENTITY_LEN + 1];   /**< Actor or system entity identifier. */
    float          grade;                        /**< Normalized continuum [-1.0f .. +1.0f]. */
    int            grade_percent;                /**< Scaled integer readout [-100 .. +100]. */
    const char    *file;                         /**< Source file anchor (__FILE__). */
    int            line;                         /**< Source line anchor (__LINE__). */
    const char    *func;                         /**< Source function anchor (__func__). */
    char           message[LOG_MAX_MESSAGE_LEN + 1]; /**< NUL-terminated message payload. */
} log_entry_t;

/**
 * @enum log_tripwire
 * @brief Diagnostic tripwire state for the data-driven logging pipeline.
 *
 * ### For Game Designers & Non-Programmers:
 * Circuit breakers catching corrupt telemetry JSON files so Safe Mode activates
 * automatically without crashing the session.
 *
 * ### For Systems Programmers & Engine Architects:
 * Latching status enum reported across shared `log_fallback_parse_config_json()`
 * and verified during Block 4 self-audits.
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
 * ### For Game Designers & Non-Programmers:
 * Configurable profile selecting log folder paths, max archive size, minimum
 * severity cutoff, and console color output.
 *
 * ### For Systems Programmers & Engine Architects:
 * Hydrated cleanly from `engine/config/diagnostics/telemetry/` or `.rodata` ROM;
 * `loaded_from_data` stays strictly `false` whenever hydrated from BIOS/ROM.
 */
typedef struct log_config {
    char           log_dir[64];            /**< Target directory for log files. */
    char           base_filename[64];      /**< Primary active log filename. */
    size_t         max_file_size_bytes;    /**< Byte threshold triggering file rotation. */
    size_t         max_rotated_files;      /**< Maximum archive files retained (1..25). */
    log_severity_t min_severity;           /**< Cutoff severity (looser levels filtered). */
    bool           save_by_category;       /**< Write per-category archives under log_dir. */
    bool           echo_to_console;        /**< Mirror formatted logs to console streams. */
    bool           use_colors;             /**< Emit ANSI color escapes on console. */
    char           profile_name[32];       /**< Active configuration preset identifier. */
    bool           loaded_from_data;       /**< True only when hydrated from live JSON VFS. */
} log_config_t;

/**
 * @struct log_stats
 * @brief Aggregate runtime telemetry report constructed by inspection procedures.
 *
 * ### For Game Designers & Non-Programmers:
 * Summary scorecard showing total logged events, active file byte count,
 * archive rotations, and whether disk or Safe-Mode presets are active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Populated atomically by `log_execute_inspect()` with zero heap allocations.
 */
typedef struct log_stats {
    uint64_t       total_entries;       /**< Total log entries recorded since init. */
    size_t         ring_buffer_count;   /**< Active entries retained in circular ring. */
    size_t         rotation_count;      /**< Number of archive rotations performed. */
    size_t         active_file_bytes;   /**< Current size in bytes of primary log file. */
    log_severity_t min_severity;        /**< Active minimum severity cutoff. */
    bool           is_active;           /**< True when logging engine is initialized. */
    bool           loaded_from_data;    /**< True if active config originated from disk JSON. */
    char           active_profile[32];  /**< Active telemetry preset identifier. */
} log_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Stateless Value Predicates
--------------------------------------------------------------------------------
Pure O(1) inline range predicates safe for C11 and C++ headers alike.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying whether a log severity enum lies within valid bounds.
 *
 * ### For Game Designers & Non-Programmers:
 * Rejects corrupted severity numbers before logs are filtered or displayed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Branchless O(1) range check over [LOG_SEV_FATAL .. LOG_SEV_COUNT - 1].
 *
 * @param[in] sev Severity enumerator to validate.
 * @return true if valid severity rank; false otherwise.
 */
static inline bool log_severity_is_valid(log_severity_t sev)
{
    return (int)sev >= (int)LOG_SEV_FATAL && (int)sev < (int)LOG_SEV_COUNT;
}

/**
 * @brief Predicate verifying whether a log category enum lies within valid bounds.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures incoming log events belong to a recognized channel folder.
 *
 * ### For Systems Programmers & Engine Architects:
 * Branchless O(1) range check over [LOG_CAT_DEBUG .. LOG_CAT_COUNT - 1].
 *
 * @param[in] cat Category enumerator to validate.
 * @return true if valid category channel; false otherwise.
 */
static inline bool log_category_is_valid(log_category_t cat)
{
    return (int)cat >= (int)LOG_CAT_DEBUG && (int)cat < (int)LOG_CAT_COUNT;
}

/**
 * @brief Predicate verifying whether a normalized scalar grade lies in [-1.0f, +1.0f].
 *
 * ### For Game Designers & Non-Programmers:
 * Guards scalar balance indicators against invalid out-of-range telemetry inputs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure floating-point range predicate rejecting NaN as well as out-of-bounds values.
 *
 * @param[in] grade Normalized float continuum scalar.
 * @return true if grade is within [-1.0f, +1.0f]; false otherwise.
 */
static inline bool log_grade_is_valid(float grade)
{
    return grade >= -1.0f && grade <= 1.0f;
}

/**
 * @brief Predicate verifying whether a logging tripwire latch is clear.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks cleanly that telemetry configuration loaded without alarms.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) comparison against `LOG_TRIPWIRE_CLEAR`.
 *
 * @param[in] wire Tripwire enumerator to inspect.
 * @return true if clear (`LOG_TRIPWIRE_CLEAR`); false otherwise.
 */
static inline bool log_tripwire_is_clear(log_tripwire_t wire)
{
    return wire == LOG_TRIPWIRE_CLEAR;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Statutory Pure-Types Notice & Structural Boundary )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0 (Core Utilities)
Module:        cog.log.types
Mission:       Pure-types telemetry vocabulary header: define purely static structures,
               immutable enums, and pure inline predicates; runtime dispatch and
               flight-recorder operations reside in `log/log.h` and `log/log_fallback.h`.
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Telemetry Shared Vocabulary
================================================================================
Note: Functional audits, layout tripwires, and pre-flight routines are declared in
`log/log_fallback.h` (`log_fallback_preflight()`) and `log/log.h` (`log_preflight()`).
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK / STATUTORY EXEMPTION & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Telemetry Shared Vocabulary
================================================================================
Note: Operational telemetry emission, subsystem lifecycle initialization, and diagnostic
dump cards are declared in `log/log.h` and `log/log_fallback.h`.
Stateless Exemption Notice: None. (Stateless module / pure stack-allocated execution).
================================================================================
*/

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_LOG_LOG_TYPES_H */

