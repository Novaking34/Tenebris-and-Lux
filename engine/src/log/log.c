/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          log.c
@id            log.engine
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-10
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
  Resolves layered telemetry settings from engine/config/diagnostics/telemetry/
  (via the VFS "config" domain: game/config/ -> engine/config/ -> linkers)
  with quarantined emergency fallback protection (log_fallback + debug_fallback).

  ### For Game Designers & Non-Programmers:
  Acts as the black-box flight recorder of the game. Every combat action, stance
  shift, and narrative dialogue prompt is recorded with precise moral sway and
  timing metrics. When Debug Mode is active or configuration files are broken,
  the subsystem automatically logs diagnostic telemetry banners and alerts the
  debugger without crashing the game.

  ### For Systems Programmers & Engine Architects:
  - 100% Zero-Heap Memory Model (ALLOC_ZERO_HEAP): All descriptors live in .rodata
    and all runtime queues/buffers reside in static .bss or caller stack frames.
  - Table-driven severity/category descriptors and VFS candidate resolution tables
    allow taxonomy extensions without rewriting conditional branches.
  - Consolidated archive rotation cog (rotate_archive_stream) eliminates duplicated
    rollover logic between master logs and categorized streams.
  - Integrated Debug Mode & Fallback diagnostics: tripwires latch local state and
    forward directly to debug_trap_tripwire() when debug_is_active(), while
    pre-flight checks validate both log_fallback_audit() and debug_fallback_audit().
================================================================================
*/

#define _POSIX_C_SOURCE 200809L

/*
================================================================================
BLOCK 2: OPENING BLOCK / MODULE ONTOLOGY & 3-TIER INCLUDES
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log
================================================================================
Mission: Telemetry limits, 3-tier downward include hierarchy, table-driven
taxonomy descriptors, encapsulated singleton state, and pre-conditional helpers.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/math_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "io/path_finder.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Compile-Time Constants
--------------------------------------------------------------------------------
Compile-time bounds for VFS buffer capacities, archive ceilings, and formatting.
--------------------------------------------------------------------------------
*/
#define MODULE_ID               "core.util.log"
#define LOCAL_LOG_CAT           LOG_CAT_LOG
#define LOCAL_MODULE_TAG        "LogEngine"
#define ANSI_RESET              "\033[0m"
#define LOG_MAX_ROTATION_CAP    25U
#define LOG_SETTING_BUF_CAP     8192U
#define LOG_MANIFEST_BUF_CAP    16384U
#define LOG_PATH_BUF_CAP        512U

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
Explicitly tagged descriptor and singleton state structures enabling table-driven
token resolution and consolidated zero-heap state encapsulation.
--------------------------------------------------------------------------------
*/

/**
 * @struct log_severity_desc
 * @brief Table row mapping a log_severity_t rank to tokens, labels, and ANSI styles.
 *
 * ### For Game Designers & Non-Programmers:
 * Defines each log importance level's name, terminal color, priority weight,
 * and whether entries must flush straight to disk so crash data is never lost.
 *
 * ### For Systems Programmers & Engine Architects:
 * Aligns canonical enum ordering [0 .. LOG_SEV_COUNT - 1] with
 * engine/config/diagnostics/telemetry/severity_weights.json for O(1) indexing.
 */
typedef struct log_severity_desc {
    log_severity_t severity;     /**< Enum rank identifier. */
    const char    *token;        /**< Canonical config string token. */
    const char    *padded_label; /**< Fixed 8-char width token for log columns. */
    const char    *ansi_color;   /**< ANSI terminal escape prefix. */
    uint8_t        weight;       /**< Priority weight matching severity_weights.json. */
    bool           sync_flush;   /**< Immediate fflush policy flag. */
} log_severity_desc_t;

/**
 * @struct log_category_desc
 * @brief Table row mapping a log_category_t channel to tokens, labels, and folders.
 *
 * ### For Game Designers & Non-Programmers:
 * Defines each thematic log channel name and the subfolder where its files are saved.
 *
 * ### For Systems Programmers & Engine Architects:
 * Enables zero-branch O(1) lookup for category labels and subdirectory paths.
 */
typedef struct log_category_desc {
    log_category_t category;     /**< Enum channel identifier. */
    const char    *token;        /**< Canonical config string token. */
    const char    *padded_label; /**< Fixed 9-char width token for log columns. */
    const char    *dir_name;     /**< Subfolder and file prefix identifier. */
} log_category_desc_t;

/**
 * @struct log_token_alias
 * @brief Extensible key-value pair binding alternative config tokens to enums.
 *
 * ### For Game Designers & Non-Programmers:
 * Allows config writers to use shorthand names like "WARN" or "SYSTEM" cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Decouples token alias expansion from conditional control flow.
 */
typedef struct log_token_alias {
    const char *alias;      /**< Alternative case-insensitive string token. */
    int         enum_value; /**< Mapped enum integer value. */
} log_token_alias_t;

/**
 * @struct log_vfs_probe
 * @brief Table entry describing a VFS search path candidate for telemetry assets.
 *
 * ### For Game Designers & Non-Programmers:
 * Lists the folders the engine checks (game overrides first, engine defaults next).
 *
 * ### For Systems Programmers & Engine Architects:
 * Supports domain-scoped path_finder resolution and legacy path fallback cleanly.
 */
typedef struct log_vfs_probe {
    const char *domain;   /**< VFS domain ("config", etc.) or NULL for generic data probe. */
    const char *template; /**< Relative path or printf format ("%s.json"). */
} log_vfs_probe_t;

/**
 * @struct log_subsystem_state
 * @brief Consolidated internal singleton state for the forensic logging engine.
 *
 * ### For Game Designers & Non-Programmers:
 * Holds everything the active flight recorder needs: current settings, open files,
 * alarm switches (tripwires), and the rolling 64-entry black-box memory buffer.
 *
 * ### For Systems Programmers & Engine Architects:
 * Consolidates all module .bss state into one cohesive zero-heap structure,
 * including safe_mode provenance and recursive-write reentrancy protection.
 */
typedef struct log_subsystem_state {
    log_config_t   config;
    bool           initialized;
    bool           safe_mode;
    bool           in_write;
    FILE          *file;
    size_t         current_bytes;
    uint64_t       sequence_counter;
    size_t         rotation_count;
    uint32_t       high_watermark_entries;
    log_tripwire_t tripwire_status;
    char           tripwire_details[256];
    FILE          *cat_files[LOG_CAT_COUNT];
    size_t         cat_bytes[LOG_CAT_COUNT];
    log_entry_t    ring_buffer[LOG_RING_BUFFER_CAPACITY];
    size_t         ring_head;
    size_t         ring_count;
    bool           debug_probes_registered;
} log_subsystem_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss & .rodata Allocations)
--------------------------------------------------------------------------------
Immutable taxonomy lookup tables in .rodata and the unified .bss state container.
--------------------------------------------------------------------------------
*/

static const log_severity_desc_t s_severity_table[LOG_SEV_COUNT] = {
    { LOG_SEV_FATAL,    "FATAL",    "FATAL   ", "\033[1;31m", 100U, true  },
    { LOG_SEV_CRITICAL, "CRITICAL", "CRITICAL", "\033[0;31m", 80U,  true  },
    { LOG_SEV_ERROR,    "ERROR",    "ERROR   ", "\033[1;91m", 60U,  true  },
    { LOG_SEV_WARNING,  "WARNING",  "WARNING ", "\033[0;33m", 40U,  true  },
    { LOG_SEV_INFO,     "INFO",     "INFO    ", "\033[0;32m", 20U,  false },
    { LOG_SEV_DEBUG,    "DEBUG",    "DEBUG   ", "\033[0;36m", 10U,  false },
    { LOG_SEV_TRACE,    "TRACE",    "TRACE   ", "\033[0;90m", 5U,   false }
};

static const log_category_desc_t s_category_table[LOG_CAT_COUNT] = {
    { LOG_CAT_DEBUG,     "DEBUG",     "DEBUG    ", "debug"     },
    { LOG_CAT_LOG,       "LOG",       "LOG      ", "log"       },
    { LOG_CAT_EVENT,     "EVENT",     "EVENT    ", "event"     },
    { LOG_CAT_MESSAGE,   "MESSAGE",   "MESSAGE  ", "message"   },
    { LOG_CAT_CHARACTER, "CHARACTER", "CHARACTER", "character" }
};

static const log_token_alias_t s_category_aliases[] = {
    { "SYSTEM", LOG_CAT_LOG }
};

static const log_vfs_probe_t s_manifest_probes[] = {
    { "config", "diagnostics/telemetry/manifest.json" },
    { "config", "telemetry/manifest.json"             },
    { NULL,     "telemetry/manifest.json"             },
    { NULL,     "log/manifest.json"                   }
};

static const log_vfs_probe_t s_profile_probes[] = {
    { "config", "diagnostics/telemetry/%s.json" },
    { NULL,     "telemetry/%s.json"             },
    { NULL,     "log/%s.json"                   },
    { NULL,     "%s.json"                       }
};

static const char *const s_log_invariants[5] = {
    "Monotonic sequence counter: ID strictly increments per entry",
    "Gradient grade range: [-1.0f, +1.0f] maps to [-100, +100]",
    "Fixed-width token columns guarantee deterministic regex/grep parsing",
    "Automated rotation bounds: Log file never exceeds configured byte ceiling",
    "Static memory boundary: In-flight ring buffer never dynamic allocates"
};

static const debug_validation_probe_t s_log_validation_probes[5] = {
    {
        "log.manifest_pipeline",
        MODULE_ID,
        "log.c:3.1(manifest_load/get_default)",
        "test_log_unit:Sec2|test_log_integrated:Sec2,3",
        DEBUG_TEST_TIER_INTEGRATED,
        DEBUG_TRIPWIRE_MANIFEST_MISSING,
        DEBUG_REACTION_FALLBACK_MODE,
        log_validate_manifest_contract,
        true,
        true,
        0U
    },
    {
        "log.setting_profile",
        MODULE_ID,
        "log.c:3.1(parse_payload/load_file/profile)",
        "test_log_unit:Sec2|test_log_functional:Sec2",
        DEBUG_TEST_TIER_FUNCTIONAL,
        DEBUG_TRIPWIRE_SETTING_MISSING,
        DEBUG_REACTION_FALLBACK_MODE,
        log_validate_setting_contract,
        true,
        true,
        0U
    },
    {
        "log.rotation_io_bounds",
        MODULE_ID,
        "log.c:3.2,5.2(rotate_archive/log_init)",
        "test_log_unit:Sec2,4|test_log_system:Sec2,4",
        DEBUG_TEST_TIER_UNIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_PANIC_HALT,
        log_validate_rotation_bounds,
        true,
        true,
        0U
    },
    {
        "log.write_telemetry",
        MODULE_ID,
        "log.c:3.3,3.4(dispatch_cat/write_entry/ring)",
        "test_log_integrated:Sec4|test_log_system:Sec1,3",
        DEBUG_TEST_TIER_SYSTEM,
        DEBUG_TRIPWIRE_TEST_FAILED,
        DEBUG_REACTION_FALLBACK_MODE,
        log_validate_telemetry_integrity,
        true,
        true,
        0U
    },
    {
        "log.fallback_bios",
        "core.util.log_fallback",
        "log_fallback.c:3.2,3.3(rom_load/record/rot)",
        "test_log_fallback_unit:Sec1-4|integrated:Sec3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_FALLBACK_ENGAGED,
        DEBUG_REACTION_FALLBACK_MODE,
        log_fallback_validate_boot_contract,
        true,
        true,
        0U
    }
};

static log_subsystem_state_t s_log = {
    .initialized = false,
    .safe_mode = false,
    .in_write = false,
    .file = NULL,
    .current_bytes = 0,
    .sequence_counter = 0,
    .rotation_count = 0,
    .high_watermark_entries = 0,
    .tripwire_status = LOG_TRIPWIRE_CLEAR,
    .tripwire_details = "All data pipeline tripwires clear.",
    .cat_files = { NULL },
    .cat_bytes = { 0 },
    .ring_head = 0,
    .ring_count = 0,
    .debug_probes_registered = false
};

const module_metadata_t log_manifest = {
    "core.util.log",
    "log/log.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-10",
    "Forensic telemetry, gradient moral grading, and rotated flight recorder",
    4,
    { "core.util.fs", "core.util.json", "core.util.string", "core.util.time" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    15,
    5,
    {
        s_log_invariants[0],
        s_log_invariants[1],
        s_log_invariants[2],
        s_log_invariants[3],
        s_log_invariants[4]
    },
    log_preflight
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
Fast range predicates, watermark updates, debug-mode checks, and tripwire latches
that bridge operational faults directly into debugger diagnostics when active.
--------------------------------------------------------------------------------
*/

/**
 * @brief Fast range check verifying a severity enum fits canonical table bounds.
 */
static inline bool log_is_severity_valid(log_severity_t sev)
{
    return (size_t)sev < (size_t)LOG_SEV_COUNT;
}

/**
 * @brief Fast range check verifying a category enum fits canonical table bounds.
 */
static inline bool log_is_category_valid(log_category_t cat)
{
    return (size_t)cat < (size_t)LOG_CAT_COUNT;
}

/**
 * @brief Predicate returning true when engine debugging or high-verbosity trace is active.
 */
static inline bool log_is_debug_mode(void)
{
    return debug_is_active() ||
           (s_log.initialized && s_log.config.min_severity >= LOG_SEV_DEBUG);
}

/**
 * @brief Tracks high watermark occupancy of the circular in-memory ring buffer.
 */
static inline void log_update_watermark(size_t current_count)
{
    if (current_count > s_log.high_watermark_entries) {
        s_log.high_watermark_entries = (uint32_t)current_count;
    }
}

/**
 * @brief Latches an internal tripwire state and notifies debugger when active.
 *
 * ### For Game Designers & Non-Programmers:
 * Records an alarm flag and explanation whenever logging setup hits broken
 * paths or invalid limits, notifying the debugger if debug mode is turned on.
 *
 * ### For Systems Programmers & Engine Architects:
 * Latches s_log.tripwire_status and s_log.tripwire_details. When debug_is_active()
 * is true, forwards exact source coordinates to debug_trap_tripwire(). When
 * debugger is inactive, inspects debug_fallback policy cleanly without polluting
 * standalone negative-path test suites.
 */
static inline const char *log_select_probe_for_tripwire(
    log_tripwire_t status,
    const char *func
) {
    switch (status) {
        case LOG_TRIPWIRE_MANIFEST_MISSING:
        case LOG_TRIPWIRE_MANIFEST_INVALID:
            return "log.manifest_pipeline";
        case LOG_TRIPWIRE_SETTING_MISSING:
        case LOG_TRIPWIRE_SETTING_INVALID:
            return "log.setting_profile";
        case LOG_TRIPWIRE_BOUNDS_VIOLATION:
            if (func != NULL &&
                (strcmp(func, "log_write_entry") == 0 ||
                 strcmp(func, "log_dispatch_category_file") == 0)) {
                return "log.write_telemetry";
            }
            return "log.rotation_io_bounds";
        default:
            return "log.write_telemetry";
    }
}

static inline void log_latch_tripwire_ex(
    const char *probe_id,
    log_tripwire_t status,
    const char *msg,
    const char *file,
    int line,
    const char *func
) {
    const char *safe_msg = (msg != NULL) ? msg : "Unspecified logging tripwire.";
    const char *safe_func = (func != NULL) ? func : "log_latch_tripwire_ex";
    s_log.tripwire_status = status;
    str_safe_copy(s_log.tripwire_details, safe_msg, sizeof(s_log.tripwire_details));

    if (status != LOG_TRIPWIRE_CLEAR &&
        (!s_log.initialized || s_log.safe_mode || debug_is_active())) {
        log_fallback_record(
            LOG_SEV_WARNING,
            LOG_CAT_LOG,
            MODULE_ID,
            -0.5f,
            file,
            line,
            safe_func,
            "[TRIPWIRE:%d] %s",
            (int)status,
            safe_msg
        );
    }

    if (debug_is_active()) {
        if (!s_log.debug_probes_registered) {
            log_register_debug_probes();
        }
        const char *resolved_probe = (probe_id != NULL && probe_id[0] != '\0')
                                         ? probe_id
                                         : log_select_probe_for_tripwire(status, safe_func);
        (void)debug_trigger_probe(
            resolved_probe,
            MODULE_ID,
            safe_msg,
            file,
            line,
            safe_func
        );
    } else if (s_log.initialized && s_log.safe_mode) {
        debug_config_t fb_dbg;
        if (debug_fallback_execute_inspect(&fb_dbg, sizeof(fb_dbg)) &&
            fb_dbg.break_on_tripwire && s_log.file != NULL) {
            fprintf(
                s_log.file,
                "[SAFE MODE TRIPWIRE] (%s:%d) %s\n",
                (file != NULL) ? file : "unknown",
                line,
                safe_msg
            );
            fflush(s_log.file);
        }
    }
}

#define LOG_SET_TRIPWIRE(status, msg) \
    log_latch_tripwire_ex(NULL, (status), (msg), __FILE__, __LINE__, __func__)

#define LOG_SET_TRIPWIRE_EX(probe_id, status, msg) \
    log_latch_tripwire_ex((probe_id), (status), (msg), __FILE__, __LINE__, __func__)

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log
================================================================================
Mission: Ingest, resolve, and hydrate telemetry configuration profiles via VFS;
evaluate gradient moral grades; manage automated log rotation; dispatch
formatted machine-greppable entries; and maintain circular flight recorder ring.
All verification gates live in Block 4 and lifecycle drivers live in Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Profile & Manifest Resolution Cogs
Mission: Ingest, resolve, and configure logging profiles via VFS and master loader.
@triggers
  - Tripwires      : LOG_TRIPWIRE_MANIFEST_MISSING / INVALID -> probe "log.manifest_pipeline"
                     LOG_TRIPWIRE_SETTING_MISSING / INVALID  -> probe "log.setting_profile"
  - Block 4 Audits : log_validate_manifest_contract(), log_validate_setting_contract()
  - Layer 4 Tests  : tests/unit/log/test_log_unit.c (Sections 1 & 2),
                     tests/functional/test_log_functional.c (Section 2),
                     tests/integrated/test_log_integrated.c (Sections 2 & 3)
--------------------------------------------------------------------------------
*/

/**
 * @brief Parses JSON setting key-value attributes into a validated log_config_t.
 *
 * ### For Game Designers & Non-Programmers:
 * Extracts folder names, file size limits, log level filters, and console color
 * flags from a telemetry settings JSON document.
 *
 * ### For Systems Programmers & Engine Architects:
 * Isolates setting attribute extraction and bounds checking into a single pure
 * helper so file and profile loaders never duplicate JSON parsing code.
 *
 * @param[in]  scope      Pointer to parsed JSON scope ("setting" object or file body).
 * @param[out] out_config Configuration struct initialized with parsed attributes.
 * @return true if parsed and bounds-checked cleanly; false with tripwire on violation.
 */
static bool log_parse_config_payload(const char *scope, log_config_t *out_config)
{
    log_tripwire_t err_code = LOG_TRIPWIRE_CLEAR;
    char err_msg[256];
    err_msg[0] = '\0';

    if (log_fallback_parse_config_json(scope, out_config, &err_code, err_msg, sizeof(err_msg))) {
        return true;
    }

    LOG_SET_TRIPWIRE(
        (err_code != LOG_TRIPWIRE_CLEAR) ? err_code : LOG_TRIPWIRE_SETTING_INVALID,
        (err_msg[0] != '\0') ? err_msg : "Failed to parse telemetry JSON setting."
    );
    return false;
}

/**
 * @brief Returns the default configuration resolved strictly from the data-driven manifest.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the starting recording settings (such as minimum log level and folder paths)
 * configured in the game's manifest files, automatically escalating through the compiled-in
 * .rodata ROM Safe-Mode preset (or factory monolith floor) if disk manifests are missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Probes manifest.json to locate and hydrate the default log_config_t structure,
 * escalating on failure via compiled-in ROM Safe-Mode hydration (loaded_from_data=false)
 * and finally log_get_monolith_fallback_config().
 *
 * @return Resolved default log_config_t structure.
 */
log_config_t log_get_default_config(void)
{
    log_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Primary Path: Dynamic resolution from VFS config telemetry manifest */
    if (log_load_config_from_manifest(NULL, &cfg)) {
        return cfg;
    }

    /* Tripwire Fallback Path 1: Data pipeline breach escalates to compiled-in ROM Safe Mode */
    LOG_SET_TRIPWIRE(
        LOG_TRIPWIRE_MANIFEST_MISSING,
        "Data pipeline breach: telemetry manifest missing; escalating to ROM Safe Mode."
    );

    if (log_fallback_load_rom_profile(NULL, &cfg)) {
        cfg.loaded_from_data = false;
        return cfg;
    }

    /* Tripwire Fallback Path 2: Final quarantined monolithic floor */
    cfg = log_get_monolith_fallback_config();
    cfg.loaded_from_data = false;
    return cfg;
}

/**
 * @brief Loads a log configuration profile from a specified JSON data file.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a custom logging profile from a specific file (e.g. for debugging combat).
 *
 * ### For Systems Programmers & Engine Architects:
 * Ingests file contents via master_loader (with fs_read fallback for bare test JSON),
 * extracts attributes into out_config, validates bounds, and marks provenance.
 *
 * @param[in]  filepath   Path to JSON setting file.
 * @param[out] out_config Output configuration structure to populate.
 * @return true if successfully loaded and validated; false on failure.
 */
bool log_load_config_from_file(const char *filepath, log_config_t *out_config)
{
    if (filepath == NULL || out_config == NULL) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_SETTING_INVALID,
            "NULL filepath or out_config passed to log_load_config_from_file."
        );
        return false;
    }

    static char file_buf[LOG_SETTING_BUF_CAP];
    master_loader_t loader;
    bool loaded = master_loader_load_from_path(&loader, filepath, file_buf, sizeof(file_buf));
    if (!loaded) {
        if (!fs_read_file_contents(filepath, file_buf, sizeof(file_buf))) {
            char err_msg[256];
            snprintf(
                err_msg,
                sizeof(err_msg),
                "Setting file unreadable or missing: %s",
                filepath
            );
            LOG_SET_TRIPWIRE(LOG_TRIPWIRE_SETTING_MISSING, err_msg);
            return false;
        }
    }

    const char *scope = strstr(file_buf, "\"setting\"");
    if (scope == NULL) {
        scope = (loaded && loader.body_start != NULL) ? loader.body_start : file_buf;
    }

    if (!log_parse_config_payload(scope, out_config)) {
        return false;
    }

    out_config->loaded_from_data = true;
    s_log.tripwire_status = LOG_TRIPWIRE_CLEAR;

    if (s_log.initialized && !s_log.in_write && log_is_debug_mode()) {
        LOG_DEBUG(
            LOG_CAT_DEBUG,
            LOCAL_MODULE_TAG,
            0.0f,
            "Loaded telemetry config '%s' from '%s' (envelope=%s, max_bytes=%zu)",
            out_config->profile_name[0] != '\0' ? out_config->profile_name : "unnamed",
            filepath,
            loaded ? "5block" : "raw",
            out_config->max_file_size_bytes
        );
    }

    return true;
}

/**
 * @brief Loads active log configuration profile from manifest "main menu" file.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the active logging preset selected in the master telemetry manifest.
 *
 * ### For Systems Programmers & Engine Architects:
 * Probes VFS "config" domain candidate paths (or caller override), validates the
 * manifest JSON menu, extracts active_setting/default_setting, and delegates loading.
 *
 * @param[in]  manifest_path Optional path to manifest (NULL for VFS auto-discovery).
 * @param[out] out_config    Output configuration structure to populate.
 * @return true if manifest and target profile were loaded; false on fallback.
 */
bool log_load_config_from_manifest(const char *manifest_path, log_config_t *out_config)
{
    if (out_config == NULL) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_MANIFEST_INVALID,
            "NULL out_config passed to log_load_config_from_manifest."
        );
        return false;
    }

    char resolved_path[PATH_FINDER_MAX_PATH];
    bool found = false;

    if (manifest_path != NULL && manifest_path[0] != '\0') {
        if (path_finder_resolve(NULL, manifest_path, resolved_path, sizeof(resolved_path)) ||
            fs_resolve_data_path(manifest_path, resolved_path, sizeof(resolved_path))) {
            found = true;
        } else if (fs_file_exists(manifest_path)) {
            str_safe_copy(resolved_path, manifest_path, sizeof(resolved_path));
            found = true;
        }
    } else {
        size_t probe_count = sizeof(s_manifest_probes) / sizeof(s_manifest_probes[0]);
        for (size_t i = 0; i < probe_count; i++) {
            const log_vfs_probe_t *probe = &s_manifest_probes[i];
            if (probe->domain != NULL) {
                found = path_finder_resolve(
                    probe->domain, probe->template, resolved_path, sizeof(resolved_path)
                );
            } else {
                found = fs_resolve_data_path(
                    probe->template, resolved_path, sizeof(resolved_path)
                );
            }
            if (found) {
                break;
            }
        }
    }

    if (!found) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_MANIFEST_MISSING,
            "Manifest file config/diagnostics/telemetry/manifest.json not accessible in search paths."
        );
        return false;
    }

    static char manifest_buf[LOG_MANIFEST_BUF_CAP];
    master_loader_t loader;
    if (!master_loader_load_from_path(&loader, resolved_path, manifest_buf, sizeof(manifest_buf))) {
        if (!fs_read_file_contents(resolved_path, manifest_buf, sizeof(manifest_buf))) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_MANIFEST_INVALID,
                "Manifest file unreadable or empty."
            );
            return false;
        }
    }

    const char *menu_block = strstr(manifest_buf, "\"menu\"");
    const char *scope = (menu_block != NULL) ? menu_block : manifest_buf;

    char active_id[64];
    if (!json_get_string(scope, "active_setting", active_id, sizeof(active_id))) {
        if (!json_get_string(scope, "default_setting", active_id, sizeof(active_id))) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_MANIFEST_INVALID,
                "Manifest menu missing active_setting or default_setting."
            );
            return false;
        }
    }

    if (log_load_config_profile(active_id, out_config)) {
        return true;
    }

    char parent_dir[PATH_FINDER_MAX_PATH];
    str_parent_dir(parent_dir, sizeof(parent_dir), resolved_path);

    char setting_path[PATH_FINDER_MAX_PATH * 2];
    snprintf(
        setting_path,
        sizeof(setting_path),
        "%s/%s.json",
        parent_dir,
        active_id
    );

    return log_load_config_from_file(setting_path, out_config);
}

/**
 * @brief Loads a named configuration profile from the telemetry directory.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches logging rules by name (such as "quiet", "development", or "forensic_telemetry").
 *
 * ### For Systems Programmers & Engine Architects:
 * Iterates the VFS probe table s_profile_probes[] across config overrides and
 * default telemetry paths, delegating file loading upon first match.
 *
 * @param[in]  profile_name Name of the setting profile to load.
 * @param[out] out_config   Output configuration structure to populate.
 * @return true if found and loaded; false otherwise.
 */
bool log_load_config_profile(const char *profile_name, log_config_t *out_config)
{
    if (profile_name == NULL || profile_name[0] == '\0' || out_config == NULL) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_SETTING_INVALID,
            "Invalid profile_name or NULL out_config."
        );
        return false;
    }

    char file_req[PATH_FINDER_MAX_PATH];
    char resolved_profile[PATH_FINDER_MAX_PATH];
    size_t probe_count = sizeof(s_profile_probes) / sizeof(s_profile_probes[0]);

    for (size_t i = 0; i < probe_count; i++) {
        const log_vfs_probe_t *probe = &s_profile_probes[i];
        snprintf(file_req, sizeof(file_req), probe->template, profile_name);

        bool matched = false;
        if (probe->domain != NULL) {
            matched = path_finder_resolve(
                probe->domain, file_req, resolved_profile, sizeof(resolved_profile)
            );
        } else {
            matched = fs_resolve_data_path(
                file_req, resolved_profile, sizeof(resolved_profile)
            );
        }

        if (matched) {
            return log_load_config_from_file(resolved_profile, out_config);
        }
    }

    char err_msg[128];
    snprintf(err_msg, sizeof(err_msg), "Setting profile '%s' not found.", profile_name);
    LOG_SET_TRIPWIRE(LOG_TRIPWIRE_SETTING_MISSING, err_msg);
    return false;
}

/**
 * @brief Returns the active configuration profile name.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks which logging preset is currently active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns pointer to the active null-terminated profile identifier in s_log.
 *
 * @return Null-terminated string identifying the profile (e.g. "development").
 */
const char *log_get_active_profile_name(void)
{
    if (s_log.config.profile_name[0] != '\0') {
        return s_log.config.profile_name;
    }
    return "default";
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: File Engine & Telemetry State Cogs
Mission: Manage directory synthesis, timestamps, archive rotation, and state.
@triggers
  - Tripwires      : LOG_TRIPWIRE_BOUNDS_VIOLATION -> probe "log.rotation_io_bounds"
                     (raised in rotate_archive_stream() / log_rotate_if_needed())
  - Block 4 Audits : log_validate_rotation_bounds()
  - Layer 4 Tests  : tests/unit/log/test_log_unit.c (Sections 2 & 4),
                     tests/system/test_log_system.c (Sections 2 & 4)
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ISO 8601 UTC timestamp string with microsecond resolution.
 *
 * Generates a fixed-width UTC timestamp matching YYYY-MM-DDTHH:MM:SS.uuuuuuZ.
 */
static void generate_iso_timestamp(char *buf, size_t buf_size)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm_utc;
    gmtime_r(&ts.tv_sec, &tm_utc);

    long usec = ts.tv_nsec / 1000L;
    snprintf(
        buf,
        buf_size,
        "%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ",
        tm_utc.tm_year + 1900,
        tm_utc.tm_mon + 1,
        tm_utc.tm_mday,
        tm_utc.tm_hour,
        tm_utc.tm_min,
        tm_utc.tm_sec,
        usec
    );
}

/**
 * @brief Shared archive rotation cog cascading old logs and reopening active stream.
 *
 * ### For Game Designers & Non-Programmers:
 * Keeps log folders tidy by sliding older backups down (.1 -> .2 -> .3 ...) and
 * starting a clean new active log file whenever the size limit is reached.
 *
 * ### For Systems Programmers & Engine Architects:
 * Unifies master log rotation and per-category archive rotation into one shared
 * cog with explicit rename() and fopen() error checking via LOG_SET_TRIPWIRE.
 *
 * @param[in]     dir            Directory holding active and archived files.
 * @param[in]     base_filename  Base archive file name (e.g. "tenebris_lux.log").
 * @param[in,out] io_file        Address of active FILE* stream handle (reopened on return).
 * @param[in,out] io_bytes       Address of active stream byte counter (reset to 0).
 * @param[out]    out_first_rot  Optional buffer receiving path to archive .1.
 * @param[in]     first_rot_size Capacity of out_first_rot buffer.
 * @return true if rotation and reopen succeeded; false on stream reopen fault.
 */
static bool rotate_archive_stream(
    const char *dir,
    const char *base_filename,
    FILE **io_file,
    size_t *io_bytes,
    char *out_first_rot,
    size_t first_rot_size
) {
    if (*io_file != NULL) {
        fclose(*io_file);
        *io_file = NULL;
    }

    char old_path[LOG_PATH_BUF_CAP];
    char new_path[LOG_PATH_BUF_CAP];
    char name_old[256];
    char name_new[256];

    for (int i = (int)s_log.config.max_rotated_files - 1; i >= 1; i--) {
        snprintf(name_old, sizeof(name_old), "%s.%d", base_filename, i);
        snprintf(name_new, sizeof(name_new), "%s.%d", base_filename, i + 1);
        str_path_join(old_path, sizeof(old_path), dir, name_old);
        str_path_join(new_path, sizeof(new_path), dir, name_new);
        if (rename(old_path, new_path) != 0 && errno != ENOENT) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_BOUNDS_VIOLATION,
                "OS error renaming historical rotated log archive file."
            );
        }
    }

    char base_path[LOG_PATH_BUF_CAP];
    char first_rot[LOG_PATH_BUF_CAP];
    char name_first[256];

    str_path_join(base_path, sizeof(base_path), dir, base_filename);
    snprintf(name_first, sizeof(name_first), "%s.1", base_filename);
    str_path_join(first_rot, sizeof(first_rot), dir, name_first);

    if (rename(base_path, first_rot) != 0 && errno != ENOENT) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "OS error archiving active log file into .1 slot."
        );
    }

    if (out_first_rot != NULL && first_rot_size > 0) {
        str_safe_copy(out_first_rot, first_rot, first_rot_size);
    }

    *io_file = fopen(base_path, "a");
    *io_bytes = 0;

    if (*io_file == NULL) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Failed to reopen log file stream after rotation."
        );
        return false;
    }

    return true;
}

/**
 * @brief Triggers an immediate log rotation if current file size exceeds threshold.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if the current log file has grown past its byte limit and archives it,
 * starting a fresh file so disk storage stays cleanly bounded.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks s_log.current_bytes against max_file_size_bytes, invokes the shared
 * rotate_archive_stream() helper, increments s_log.rotation_count, and stamps
 * a rotation boundary header line into the new file.
 *
 * @return true if rotation occurred; false otherwise.
 */
bool log_rotate_if_needed(void)
{
    if (!s_log.initialized || s_log.file == NULL) {
        return false;
    }
    if (s_log.current_bytes < s_log.config.max_file_size_bytes) {
        return false;
    }

    size_t prev_bytes = s_log.current_bytes;
    char first_rot[LOG_PATH_BUF_CAP];
    first_rot[0] = '\0';

    bool rotated = rotate_archive_stream(
        s_log.config.log_dir,
        s_log.config.base_filename,
        &s_log.file,
        &s_log.current_bytes,
        first_rot,
        sizeof(first_rot)
    );
    s_log.rotation_count++;

    if (rotated && s_log.file != NULL) {
        char iso_ts[32];
        generate_iso_timestamp(iso_ts, sizeof(iso_ts));
        fprintf(
            s_log.file,
            "================================================================================\n"
            "[%s] [SYSTEM ROTATION] Log file rotated (bytes=%zu -> archived to %s)\n"
            "================================================================================\n",
            iso_ts,
            prev_bytes,
            first_rot
        );
        fflush(s_log.file);
    }

    return true;
}

/**
 * @brief Rotates category-specific dedicated log file if byte threshold is breached.
 */
static void rotate_category_file(log_category_t cat)
{
    if (!log_is_category_valid(cat)) {
        return;
    }
    if (s_log.cat_files[cat] == NULL ||
        s_log.cat_bytes[cat] < s_log.config.max_file_size_bytes) {
        return;
    }

    const char *cat_dir_name = log_category_dir_name(cat);
    char cat_dir[256];
    char base_name[128];

    snprintf(cat_dir, sizeof(cat_dir), "%s/%s", s_log.config.log_dir, cat_dir_name);
    snprintf(base_name, sizeof(base_name), "%s.log", cat_dir_name);

    rotate_archive_stream(
        cat_dir,
        base_name,
        &s_log.cat_files[cat],
        &s_log.cat_bytes[cat],
        NULL,
        0
    );
}

/**
 * @brief Checks if the logging subsystem has been initialized and is active.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies whether the flight recorder is currently online and recording events.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct query against internal singleton flag s_log.initialized.
 *
 * @return true if initialized and active; false otherwise.
 */
bool log_is_active(void)
{
    return s_log.initialized;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Formatted Entry Dispatch Cogs
Mission: Format, serialize, buffer, and emit structured flight recorder entries.
@triggers
  - Tripwires      : LOG_TRIPWIRE_SETTING_INVALID / BOUNDS_VIOLATION -> probe "log.write_telemetry"
                     (raised in log_dispatch_category_file() / log_write_entry())
  - Block 4 Audits : log_validate_telemetry_integrity()
  - Layer 4 Tests  : tests/unit/log/test_log_unit.c (Sections 3 & 4),
                     tests/functional/test_log_functional.c (Section 3),
                     tests/integrated/test_log_integrated.c (Section 4),
                     tests/system/test_log_system.c (Sections 1 & 3)
--------------------------------------------------------------------------------
*/

/**
 * @brief Pushes a formatted entry into the static circular flight recorder buffer.
 */
static void log_ring_push(const log_entry_t *entry)
{
    s_log.ring_buffer[s_log.ring_head] = *entry;
    s_log.ring_head = (s_log.ring_head + 1U) % LOG_RING_BUFFER_CAPACITY;
    if (s_log.ring_count < LOG_RING_BUFFER_CAPACITY) {
        s_log.ring_count++;
    }
    log_update_watermark(s_log.ring_count);
}

/**
 * @brief Lazily opens category stream and dispatches formatted line to category disk archive.
 */
static void log_dispatch_category_file(
    log_category_t category,
    const char *line_buffer,
    size_t write_len
) {
    if (!s_log.config.save_by_category || !log_is_category_valid(category)) {
        return;
    }

    if (s_log.cat_files[category] == NULL) {
        const char *dir_name = log_category_dir_name(category);
        char cat_dir[256];
        snprintf(cat_dir, sizeof(cat_dir), "%s/%s", s_log.config.log_dir, dir_name);
        if (!fs_ensure_directory(cat_dir)) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_BOUNDS_VIOLATION,
                "Failed to synthesize category log subdirectory."
            );
            return;
        }

        char cat_path[LOG_PATH_BUF_CAP];
        snprintf(cat_path, sizeof(cat_path), "%s/%s.log", cat_dir, dir_name);
        s_log.cat_files[category] = fopen(cat_path, "a");
        if (s_log.cat_files[category] == NULL) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_BOUNDS_VIOLATION,
                "Failed to open category log stream for writing."
            );
            return;
        }

        struct stat st_cat;
        if (stat(cat_path, &st_cat) == 0) {
            s_log.cat_bytes[category] = (size_t)st_cat.st_size;
        } else {
            s_log.cat_bytes[category] = 0;
        }
    }

    if (fputs(line_buffer, s_log.cat_files[category]) == EOF) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "I/O write fault while writing categorized log entry."
        );
        return;
    }

    s_log.cat_bytes[category] += write_len;
    fflush(s_log.cat_files[category]);
    rotate_category_file(category);
}

/**
 * @brief Mirrors formatted entry to stdout/stderr with table-driven ANSI color highlighting.
 */
static void log_dispatch_console(
    log_severity_t severity,
    const log_entry_t *entry,
    const char *line_buffer
) {
    if (!s_log.config.echo_to_console) {
        return;
    }

    FILE *out = (severity <= LOG_SEV_ERROR) ? stderr : stdout;
    if (s_log.config.use_colors && log_is_severity_valid(severity)) {
        fprintf(
            out,
            "%s[%s] [ID#%08llu] [%-8s] [CAT:%-9s] [%-10s] [GRADE:%+04d] %s%s\n",
            s_severity_table[severity].ansi_color,
            entry->iso_timestamp,
            (unsigned long long)entry->sequence_id,
            log_severity_name(entry->severity),
            log_category_name(entry->category),
            entry->entity,
            entry->grade_percent,
            entry->message,
            ANSI_RESET
        );
    } else {
        fputs(line_buffer, out);
    }
}

/**
 * @brief Emits a structured log entry into the ring buffer, file, and console.
 *
 * ### For Game Designers & Non-Programmers:
 * Writes an event to the game's flight recorder journal, capturing the character involved,
 * their current moral balance percentage, and what action took place.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates severity/reentrancy, formats fixed-width records with microsecond deltas
 * and ISO 8601 UTC timestamps, commits into circular ring buffer, and emits safely
 * across primary log, category archive, and console outputs with zero heap allocation.
 *
 * @param[in] severity Graded severity level.
 * @param[in] category Target functional category.
 * @param[in] entity   Entity/Character name (e.g. "Elijah", "Katrina", "System").
 * @param[in] grade    Normalized grade [-1.0f .. +1.0f].
 * @param[in] file     Source file name (__FILE__).
 * @param[in] line     Source line number (__LINE__).
 * @param[in] func     Source function name (__func__).
 * @param[in] fmt      Printf-style format string.
 * @return Monotonic sequence ID assigned to the entry, or 0 on filter drop/failure.
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
) {
    if (s_log.in_write) {
        return 0;
    }

    if (!s_log.initialized) {
        if (!log_is_severity_valid(severity)) {
            return 0;
        }
        char early_msg[LOG_MAX_MESSAGE_LEN + 1];
        if (fmt != NULL) {
            va_list early_args;
            va_start(early_args, fmt);
            vsnprintf(early_msg, sizeof(early_msg), fmt, early_args);
            va_end(early_args);
        } else {
            str_safe_copy(early_msg, "(null format string)", sizeof(early_msg));
        }
        return log_fallback_record(
            severity,
            category,
            entity,
            grade,
            file,
            line,
            func,
            "%s",
            early_msg
        );
    }

    if (!log_is_severity_valid(severity) || severity > s_log.config.min_severity) {
        return 0;
    }

    s_log.in_write = true;
    s_log.sequence_counter++;
    uint64_t seq = s_log.sequence_counter;

    log_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.sequence_id = seq;
    entry.monotonic_us = time_get_monotonic_us();
    generate_iso_timestamp(entry.iso_timestamp, sizeof(entry.iso_timestamp));
    entry.severity = severity;
    entry.category = log_is_category_valid(category) ? category : LOG_CAT_LOG;

    if (entity != NULL && entity[0] != '\0') {
        str_safe_copy(entry.entity, entity, sizeof(entry.entity));
    } else {
        str_safe_copy(entry.entity, "System", sizeof(entry.entity));
    }

    /* NaN-safe gradient clamping inside [-1.0f, +1.0f] */
    if (!(grade >= -1.0f && grade <= 1.0f)) {
        entry.grade = (grade > 1.0f) ? 1.0f : ((grade < -1.0f) ? -1.0f : 0.0f);
    } else {
        entry.grade = grade;
    }
    entry.grade_percent = log_grade_to_percent(entry.grade);
    entry.file = file;
    entry.line = line;
    entry.func = func;

    if (fmt != NULL) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(entry.message, sizeof(entry.message), fmt, args);
        va_end(args);
    } else {
        str_safe_copy(entry.message, "(null format string)", sizeof(entry.message));
        if (log_is_debug_mode()) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_SETTING_INVALID,
                "NULL format string passed to log_write_entry."
            );
        }
    }

    log_ring_push(&entry);

    char line_buffer[LOG_MAX_LINE_BUFFER];
    double rel_ms = (double)entry.monotonic_us / 1000.0;

    int written = snprintf(
        line_buffer,
        sizeof(line_buffer),
        "[%s] [ID#%08llu] [+%010.3fms] [%-8s] [CAT:%-9s] [ENTITY:%-16s] [GRADE:%+04d] (%s:%d:%s) %s\n",
        entry.iso_timestamp,
        (unsigned long long)entry.sequence_id,
        rel_ms,
        log_severity_name(entry.severity),
        log_category_name(entry.category),
        entry.entity,
        entry.grade_percent,
        (entry.file != NULL) ? entry.file : "unknown",
        entry.line,
        (entry.func != NULL) ? entry.func : "unknown",
        entry.message
    );

    if (written < 0) {
        if (log_is_debug_mode()) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_BOUNDS_VIOLATION,
                "Formatting error in log_write_entry snprintf."
            );
        }
        s_log.in_write = false;
        return seq;
    }

    /* Clamp tracked disk write length to actual emitted buffer bytes */
    size_t write_len = ((size_t)written < sizeof(line_buffer))
        ? (size_t)written
        : (sizeof(line_buffer) - 1U);

    if (s_log.file != NULL) {
        if (fputs(line_buffer, s_log.file) != EOF) {
            s_log.current_bytes += write_len;
            if (s_severity_table[severity].sync_flush || log_is_debug_mode() ||
                s_log.safe_mode) {
                fflush(s_log.file);
            }
            log_rotate_if_needed();
        } else {
            s_log.safe_mode = true;
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_BOUNDS_VIOLATION,
                "I/O write fault while emitting master log entry."
            );
            log_fallback_record(
                severity,
                entry.category,
                entry.entity,
                entry.grade,
                entry.file,
                entry.line,
                entry.func,
                "%s",
                entry.message
            );
        }
    }

    log_dispatch_category_file(entry.category, line_buffer, write_len);
    log_dispatch_console(severity, &entry, line_buffer);

    s_log.in_write = false;
    return seq;
}

/**
 * @brief Converts a normalized float grade [-1.0f, +1.0f] to integer percent [-100, +100].
 *
 * ### For Game Designers & Non-Programmers:
 * Converts an internal moral sway decimal into a clean -100% to +100% readout.
 *
 * ### For Systems Programmers & Engine Architects:
 * NaN-safe clamps grade to [-1.0f, 1.0f] and rounds symmetrically into [-100, 100].
 */
int log_grade_to_percent(float grade)
{
    if (!(grade >= -1.0f && grade <= 1.0f)) {
        if (grade > 1.0f) {
            grade = 1.0f;
        } else if (grade < -1.0f) {
            grade = -1.0f;
        } else {
            return 0;
        }
    }
    float scaled = grade * 100.0f;
    int rounded = (scaled >= 0.0f) ? (int)(scaled + 0.5f) : (int)(scaled - 0.5f);
    return math_clamp_int(rounded, -100, 100);
}

/**
 * @brief Converts an integer percent [-100, +100] to a normalized float grade [-1.0f, +1.0f].
 *
 * ### For Game Designers & Non-Programmers:
 * Converts a whole-number percentage into an internal decimal balance ratio.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clamps percent to [-100, 100] and maps to floating-point representation in [-1.0f, 1.0f].
 */
float log_percent_to_grade(int percent)
{
    int clamped = math_clamp_int(percent, -100, 100);
    return (float)clamped / 100.0f;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Static Ring Buffer & Telemetry Inspection Cogs
Mission: Retrieve ordered entries from in-memory ring buffer for HUD readout.
@triggers
  - Block 4 Audits : log_validate_telemetry_integrity() [ring FIFO & watermark bounds]
  - Layer 4 Tests  : tests/unit/log/test_log_unit.c (Section 4),
                     tests/integrated/test_log_integrated.c (Section 4),
                     tests/system/test_log_system.c (Section 4)
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves cumulative total number of log entries recorded.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns the lifetime count of events recorded since the game started.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns the 64-bit monotonic sequence counter value held in s_log.
 */
uint64_t log_get_total_entries(void)
{
    return s_log.sequence_counter;
}

/**
 * @brief Copies recent log entries from in-memory ring buffer in chronological order.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the latest recorded events in chronological order (oldest to newest) for HUD display.
 *
 * ### For Systems Programmers & Engine Architects:
 * Unrolls circular ring buffer into caller-provided array with zero heap allocation.
 *
 * @param[out] dest        Destination buffer array.
 * @param[in]  max_entries Maximum entries to copy.
 * @return Number of entries copied.
 */
size_t log_get_recent_entries(log_entry_t *dest, size_t max_entries)
{
    if (dest == NULL || max_entries == 0 || s_log.ring_count == 0) {
        return 0;
    }
    size_t count = (max_entries < s_log.ring_count) ? max_entries : s_log.ring_count;
    size_t start_idx =
        (s_log.ring_head + LOG_RING_BUFFER_CAPACITY - count) % LOG_RING_BUFFER_CAPACITY;

    for (size_t i = 0; i < count; i++) {
        size_t idx = (start_idx + i) % LOG_RING_BUFFER_CAPACITY;
        dest[i] = s_log.ring_buffer[idx];
    }
    return count;
}

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
size_t log_get_top_entries(log_entry_t *dest, size_t max_entries)
{
    if (dest == NULL || max_entries == 0 || s_log.ring_count == 0) {
        return 0;
    }
    size_t count = (max_entries < s_log.ring_count) ? max_entries : s_log.ring_count;

    for (size_t i = 0; i < count; i++) {
        size_t idx =
            (s_log.ring_head + LOG_RING_BUFFER_CAPACITY - 1U - i) % LOG_RING_BUFFER_CAPACITY;
        dest[i] = s_log.ring_buffer[idx];
    }
    return count;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: String & Enum Parsing Cogs
Mission: Convert between string tokens and strongly-typed enum representations.
@triggers
  - Block 4 Audits : log_preflight(), log_validate_manifest_contract()
  - Layer 4 Tests  : tests/unit/log/test_log_unit.c (Section 3),
                     tests/functional/test_log_functional.c (Section 1),
                     tests/system/test_log_system.c (Section 3)
--------------------------------------------------------------------------------
*/

/**
 * @brief Converts a human-readable severity string into its typed log_severity_t enum.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts text like "WARNING" or "INFO" from a config file into an internal code.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scans static descriptor table s_severity_table[] followed by s_severity_aliases[]
 * for extensible, branch-free token resolution.
 */
bool log_severity_from_string(const char *str, log_severity_t *out_sev)
{
    return log_fallback_severity_from_string(str, out_sev);
}

/**
 * @brief Converts a human-readable category string into its typed log_category_t enum.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts category text like "CHARACTER" or "EVENT" into an internal routing channel.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scans static descriptor table s_category_table[] followed by s_category_aliases[]
 * for extensible, branch-free token resolution.
 */
bool log_category_from_string(const char *str, log_category_t *out_cat)
{
    if (str == NULL || out_cat == NULL) {
        return false;
    }

    for (size_t i = 0; i < (size_t)LOG_CAT_COUNT; i++) {
        if (str_case_eq(str, s_category_table[i].token)) {
            *out_cat = s_category_table[i].category;
            return true;
        }
    }

    size_t alias_count = sizeof(s_category_aliases) / sizeof(s_category_aliases[0]);
    for (size_t i = 0; i < alias_count; i++) {
        if (str_case_eq(str, s_category_aliases[i].alias)) {
            *out_cat = (log_category_t)s_category_aliases[i].enum_value;
            return true;
        }
    }

    return false;
}

/**
 * @brief Returns the human-readable fixed-width string for a severity level.
 *
 * ### For Game Designers & Non-Programmers:
 * Gets a clean aligned label (such as "FATAL   ", "INFO    ") for readable logs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct O(1) lookup against s_severity_table[] in .rodata.
 */
const char *log_severity_name(log_severity_t severity)
{
    if (log_is_severity_valid(severity)) {
        return s_severity_table[severity].padded_label;
    }
    return "UNKNOWN ";
}

/**
 * @brief Returns the human-readable fixed-width string for a category.
 *
 * ### For Game Designers & Non-Programmers:
 * Gets the display name of a subsystem category for log filtering.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct O(1) lookup against s_category_table[] in .rodata.
 */
const char *log_category_name(log_category_t category)
{
    if (log_is_category_valid(category)) {
        return s_category_table[category].padded_label;
    }
    return "UNKNOWN  ";
}

/**
 * @brief Returns the folder name associated with a category (e.g. "character", "event").
 *
 * ### For Game Designers & Non-Programmers:
 * Gets the subfolder name where specialized logs for this topic are stored.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct O(1) lookup returning s_category_table[].dir_name constant.
 */
const char *log_category_dir_name(log_category_t category)
{
    if (log_is_category_valid(category)) {
        return s_category_table[category].dir_name;
    }
    return "general";
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log
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
 * Checks if settings were loaded from actual JSON files rather than emergency defaults.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries provenance boolean s_log.config.loaded_from_data.
 *
 * @return true if loaded from JSON data; false if using fallback code page.
 */
bool log_is_data_driven(void)
{
    return s_log.config.loaded_from_data;
}

/**
 * @brief Queries whether the logging engine is currently operating in Safe Mode.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns true if the engine booted using compiled-in ROM backups or emergency
 * defaults instead of live external JSON directories on disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks initialization state alongside s_log.safe_mode (!loaded_from_data).
 *
 * @return true if active and running in Safe Mode; false otherwise.
 */
bool log_is_safe_mode(void)
{
    return s_log.initialized && s_log.safe_mode;
}

/**
 * @brief Returns the current state of the logging data pipeline tripwire.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether the data-driven logging pipeline encountered an error or is clean.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries s_log.tripwire_status latch.
 *
 * @return Active log_tripwire_t enum status code.
 */
log_tripwire_t log_get_tripwire_status(void)
{
    return s_log.tripwire_status;
}

/**
 * @brief Returns a detailed message explaining why a tripwire was tripped.
 *
 * ### For Game Designers & Non-Programmers:
 * Gives a clear, readable explanation of what went wrong if a configuration fault occurred.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns pointer to internal diagnostic message buffer s_log.tripwire_details.
 *
 * @return Null-terminated string describing the latched tripwire condition.
 */
const char *log_get_tripwire_details(void)
{
    return s_log.tripwire_details;
}

/**
 * @brief Resets the tripwire state to CLEAR (used in diagnostic test suites).
 *
 * ### For Game Designers & Non-Programmers:
 * Clears any tripwire warning once the error has been noted or handled.
 *
 * ### For Systems Programmers & Engine Architects:
 * Restores s_log.tripwire_status to LOG_TRIPWIRE_CLEAR and reinitializes details.
 */
void log_reset_tripwire(void)
{
    s_log.tripwire_status = LOG_TRIPWIRE_CLEAR;
    str_safe_copy(
        s_log.tripwire_details,
        "All data pipeline tripwires clear.",
        sizeof(s_log.tripwire_details)
    );
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
@triggers
  - Probe Registry : log_register_debug_probes() wires s_log_validation_probes[5]
  - Block 4 Audits : log_validate_manifest_contract(), log_validate_setting_contract(),
                     log_validate_rotation_bounds(), log_validate_telemetry_integrity(),
                     log_preflight(), log_audit()
  - Layer 4 Tests  : tests/unit/log/ (Sec 1-4), tests/functional/test_log_functional.c,
                     tests/integrated/test_log_integrated.c, tests/system/test_log_system.c
--------------------------------------------------------------------------------
*/

void log_register_debug_probes(void)
{
    size_t count = sizeof(s_log_validation_probes) / sizeof(s_log_validation_probes[0]);
    for (size_t i = 0U; i < count; i++) {
        (void)debug_register_validation_probe(&s_log_validation_probes[i]);
    }
    s_log.debug_probes_registered = true;
}

diag_result_t log_validate_manifest_contract(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        MODULE_ID,
        "log_validate_manifest_contract",
        "Telemetry manifest pipeline, taxonomy tables, and ROM index verified."
    };

    if (s_log.tripwire_status == LOG_TRIPWIRE_MANIFEST_MISSING ||
        s_log.tripwire_status == LOG_TRIPWIRE_MANIFEST_INVALID) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = s_log.tripwire_details;
        return res;
    }

    log_fallback_rom_info_t rom_info;
    if (!log_fallback_get_rom_info(&rom_info) || rom_info.asset_count == 0U) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Telemetry ROM table unavailable for fallback manifest resolution.";
        return res;
    }

    if (s_log.initialized && s_log.config.profile_name[0] == '\0') {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Initialized telemetry subsystem missing resolved profile_name.";
        return res;
    }

    return res;
}

diag_result_t log_validate_setting_contract(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        MODULE_ID,
        "log_validate_setting_contract",
        "Telemetry active setting profile and parameter bounds verified."
    };

    if (s_log.tripwire_status == LOG_TRIPWIRE_SETTING_MISSING ||
        s_log.tripwire_status == LOG_TRIPWIRE_SETTING_INVALID) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = s_log.tripwire_details;
        return res;
    }

    if (s_log.initialized) {
        if (s_log.config.max_file_size_bytes == 0U ||
            s_log.config.max_rotated_files > LOG_MAX_ROTATION_CAP ||
            !log_severity_is_valid(s_log.config.min_severity) ||
            s_log.config.base_filename[0] == '\0') {
            res.passed = false;
            res.severity = SEVERITY_CRITICAL;
            res.details = "Live log_config_t breached severity, file size, or filename invariants.";
            return res;
        }
    }

    return res;
}

diag_result_t log_validate_rotation_bounds(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        MODULE_ID,
        "log_validate_rotation_bounds",
        "Telemetry archive rotation limits and active file stream verified."
    };

    if (s_log.tripwire_status == LOG_TRIPWIRE_BOUNDS_VIOLATION) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = s_log.tripwire_details;
        return res;
    }

    if (s_log.initialized &&
        (s_log.config.max_file_size_bytes == 0U ||
         s_log.config.max_rotated_files > LOG_MAX_ROTATION_CAP ||
         (!s_log.safe_mode && s_log.file == NULL))) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Active archive rotation limits or primary file stream breached invariants.";
        return res;
    }

    return res;
}

diag_result_t log_validate_telemetry_integrity(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        MODULE_ID,
        "log_validate_telemetry_integrity",
        "Static FIFO ring buffer watermarks and monotonic telemetry dispatch verified."
    };

    if (s_log.ring_count > LOG_RING_BUFFER_CAPACITY ||
        s_log.ring_head >= LOG_RING_BUFFER_CAPACITY ||
        s_log.high_watermark_entries > LOG_RING_BUFFER_CAPACITY) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Telemetry FIFO ring indices or watermark breached LOG_RING_BUFFER_CAPACITY.";
        return res;
    }

    if (s_log.tripwire_status == LOG_TRIPWIRE_BOUNDS_VIOLATION) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = s_log.tripwire_details;
        return res;
    }

    return res;
}

/**
 * @brief Executes machine pre-flight diagnostic checks on log subsystem invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Performs rapid startup safety checks on telemetry text tables, grade math, JSON
 * configuration loading, and both logging and debugger emergency backup modules.
 *
 * ### For Systems Programmers & Engine Architects:
 * Side-effect-safe static & manifest contract check verifying taxonomy table
 * alignment, gradient bounds, data provenance, log_fallback_audit(), and
 * debug_fallback_audit().
 *
 * @return diag_result_t reporting SEVERITY_PASS or failure details.
 */
diag_result_t log_preflight(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = MODULE_ID;
    res.check_name = "grade_mapping_and_buffers";
    res.details = "Grade mapping, tables, manifest, and fallback code pages verified.";

    /* Check 1: Audit gradient grade mapping bounds */
    if (log_grade_to_percent(1.0f) != 100 ||
        log_grade_to_percent(-1.0f) != -100 ||
        log_grade_to_percent(0.0f) != 0 ||
        log_grade_to_percent(1.5f) != 100 ||
        log_grade_to_percent(-2.0f) != -100) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Gradient grade mapping bounds violation.";
        return res;
    }

    /* Check 2: Audit severity and category lookup table alignment */
    for (size_t i = 0; i < (size_t)LOG_SEV_COUNT; i++) {
        if (s_severity_table[i].severity != (log_severity_t)i) {
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "Severity descriptor table index misalignment.";
            return res;
        }
    }
    for (size_t i = 0; i < (size_t)LOG_CAT_COUNT; i++) {
        if (s_category_table[i].category != (log_category_t)i) {
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "Category descriptor table index misalignment.";
            return res;
        }
    }

    /* Check 3: Audit string labels and category directory names */
    if (strcmp(log_severity_name(LOG_SEV_FATAL), "FATAL   ") != 0 ||
        strcmp(log_category_name(LOG_CAT_CHARACTER), "CHARACTER") != 0 ||
        strcmp(log_category_dir_name(LOG_CAT_CHARACTER), "character") != 0 ||
        strcmp(log_category_dir_name(LOG_CAT_EVENT), "event") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Telemetry label or directory mapping discrepancy.";
        return res;
    }

    /* Check 4: Audit string-to-enum conversions and aliases */
    log_severity_t test_sev;
    if (!log_severity_from_string("TRACE", &test_sev) || test_sev != LOG_SEV_TRACE ||
        !log_severity_from_string("fatal", &test_sev) || test_sev != LOG_SEV_FATAL ||
        !log_severity_from_string("WARNING", &test_sev) || test_sev != LOG_SEV_WARNING ||
        !log_severity_from_string("WARN", &test_sev) || test_sev != LOG_SEV_WARNING ||
        log_severity_from_string("NON_EXISTENT", &test_sev)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Severity string-to-enum conversion mismatch.";
        return res;
    }

    log_category_t test_cat;
    if (!log_category_from_string("CHARACTER", &test_cat) || test_cat != LOG_CAT_CHARACTER ||
        !log_category_from_string("event", &test_cat) || test_cat != LOG_CAT_EVENT ||
        !log_category_from_string("system", &test_cat) || test_cat != LOG_CAT_LOG ||
        log_category_from_string("UNKNOWN_CAT", &test_cat)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Category string-to-enum conversion mismatch.";
        return res;
    }

    /* Check 5: Audit data-driven default configuration resolution */
    log_config_t def_cfg = log_get_default_config();
    if (!def_cfg.loaded_from_data || def_cfg.profile_name[0] == '\0') {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Data-driven logging invariant breach: default config not loaded from manifest.";
        return res;
    }

    /* Check 6: Audit logging emergency fallback code page invariants */
    if (!log_fallback_audit()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Monolithic emergency fallback code page failed audit.";
        return res;
    }

    /* Check 7: Audit debugger emergency fallback code page invariants */
    if (!debug_fallback_audit()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Debugger emergency fallback code page failed audit.";
        return res;
    }

    return res;
}

/**
 * @brief Executes comprehensive runtime diagnostic audit suite for telemetry engine.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs an end-to-end health check of the flight recorder, testing counters, ring
 * buffers, configuration loading, and both emergency fallback modules.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes preflight, precision conversions, monotonic counter increments, ring
 * buffer chronological/reverse unrolling, manifest & named profile parsing, and
 * emergency fallback code page verification without leaking state.
 *
 * @param[in] verbose Emit formatted diagnostic progress lines to stdout when true.
 * @return diag_result_t summarizing overall pass/fail verdict.
 */
diag_result_t log_audit(bool verbose)
{
    if (verbose) {
        printf("--- Running Core Utilities: Telemetry Subsystem Diagnostic Audit ---\n");
    }

    bool was_initialized = s_log.initialized;
    if (!s_log.initialized) {
        log_config_t test_cfg = log_get_default_config();
        test_cfg.echo_to_console = false;
        test_cfg.min_severity = LOG_SEV_TRACE;
        log_init(&test_cfg);
    }

    /* Check 1: Preflight self-check */
    diag_result_t pre = log_preflight();
    if (!pre.passed) {
        if (verbose) {
            printf("  [FAIL] log_preflight invariant check failed: %s\n", pre.details);
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return pre;
    }
    if (verbose) {
        printf("  [PASS] Preflight tables, grade math, and fallback invariants verified.\n");
    }

    /* Check 2: Percent to grade conversion precision */
    float g1 = log_percent_to_grade(100);
    float g2 = log_percent_to_grade(-100);
    float g3 = log_percent_to_grade(0);
    if (g1 < 0.99f || g1 > 1.01f || g2 < -1.01f || g2 > -0.99f || g3 != 0.0f) {
        diag_result_t err = { false, SEVERITY_CRITICAL, MODULE_ID, "percent_conversion", "Precision failed" };
        if (verbose) {
            printf("  [FAIL] Percent to grade conversion failed.\n");
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return err;
    }
    if (verbose) {
        printf("  [PASS] Percent to grade precision verified.\n");
    }

    /* Check 3: Monotonic sequence counter test */
    uint64_t before = log_get_total_entries();
    LOG_INFO(LOG_CAT_DEBUG, "AuditTester", 0.0f, "Audit diagnostic message %d", 42);
    uint64_t after = log_get_total_entries();
    if (after != before + 1) {
        diag_result_t err = { false, SEVERITY_CRITICAL, MODULE_ID, "sequence_monotonicity", "Counter failed" };
        if (verbose) {
            printf("  [FAIL] Sequence counter monotonicity violated.\n");
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return err;
    }
    if (verbose) {
        printf("  [PASS] Sequence counter monotonicity verified (%lu -> %lu).\n",
               (unsigned long)before, (unsigned long)after);
    }

    /* Check 4: Ring buffer chronological retrieval */
    log_entry_t entries[4];
    size_t fetched = log_get_recent_entries(entries, 4);
    if (fetched == 0 || entries[fetched - 1].sequence_id != after) {
        diag_result_t err = { false, SEVERITY_CRITICAL, MODULE_ID, "ring_buffer", "Retrieval failed" };
        if (verbose) {
            printf("  [FAIL] Ring buffer entry retrieval failed.\n");
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return err;
    }
    if (verbose) {
        printf("  [PASS] Ring buffer retrieved %zu recent entries.\n", fetched);
    }

    /* Check 5: Top entries retrieval (newest first) */
    log_entry_t top_entries[25];
    size_t top_fetched = log_get_top_entries(top_entries, 25);
    if (top_fetched == 0 || top_entries[0].sequence_id != after) {
        diag_result_t err = { false, SEVERITY_CRITICAL, MODULE_ID, "top_entries", "Top retrieval failed" };
        if (verbose) {
            printf("  [FAIL] Top entries retrieval failed.\n");
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return err;
    }
    if (verbose) {
        printf("  [PASS] Top entries retrieved %zu entries (newest ID#%08llu).\n",
               top_fetched, (unsigned long long)top_entries[0].sequence_id);
    }

    /* Check 6: Data-driven configuration loading verification */
    log_config_t manifest_cfg;
    if (!log_load_config_from_manifest(NULL, &manifest_cfg) || !manifest_cfg.loaded_from_data) {
        diag_result_t err = { false, SEVERITY_CRITICAL, MODULE_ID, "data_manifest", "Data-driven manifest loading failed" };
        if (verbose) {
            printf("  [FAIL] Data-driven manifest loading failed.\n");
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return err;
    }
    if (verbose) {
        printf("  [PASS] Data-driven config loaded from manifest (Profile: %s, File: %s, Max: %zu bytes).\n",
               manifest_cfg.profile_name, manifest_cfg.base_filename, manifest_cfg.max_file_size_bytes);
    }

    /* Check 7: Specific named profile loading verification */
    log_config_t quiet_cfg;
    if (log_load_config_profile("quiet", &quiet_cfg)) {
        if (quiet_cfg.min_severity != LOG_SEV_WARNING || quiet_cfg.echo_to_console != false) {
            diag_result_t err = { false, SEVERITY_WARNING, MODULE_ID, "profile_quiet", "Quiet profile attributes mismatch" };
            if (verbose) {
                printf("  [WARN] Quiet profile attributes mismatch.\n");
            }
            if (!was_initialized) {
                log_shutdown();
            }
            return err;
        }
        if (verbose) {
            printf("  [PASS] Profile 'quiet' loaded and verified from telemetry config sources.\n");
        }
    }

    /* Check 8: Logging emergency fallback audit */
    if (!log_fallback_audit()) {
        diag_result_t err = { false, SEVERITY_CRITICAL, MODULE_ID, "fallback_monolith", "Emergency fallback code page failed audit" };
        if (verbose) {
            printf("  [FAIL] Monolithic fallback code page failed audit.\n");
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return err;
    }
    if (verbose) {
        printf("  [PASS] Emergency monolithic fallback code page audit verified.\n");
    }

    /* Check 9: Debugger emergency fallback audit */
    if (!debug_fallback_audit()) {
        diag_result_t err = { false, SEVERITY_CRITICAL, MODULE_ID, "debug_fallback", "Debugger fallback code page failed audit" };
        if (verbose) {
            printf("  [FAIL] Debugger fallback code page failed audit.\n");
        }
        if (!was_initialized) {
            log_shutdown();
        }
        return err;
    }
    if (verbose) {
        printf("  [PASS] Emergency debugger fallback code page audit verified.\n");
    }

    if (!was_initialized) {
        log_shutdown();
    }

    diag_result_t ok = { true, SEVERITY_PASS, MODULE_ID, "full_audit", "All audit checks passed" };
    return ok;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log
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
 * @brief Constructs a programmatic snapshot of current log subsystem telemetry.
 *
 * ### For Game Designers & Non-Programmers:
 * Collects live telemetry statistics (total messages written, archive rotations,
 * active profile name) into an inspectable report struct.
 *
 * ### For Systems Programmers & Engine Architects:
 * Populates caller-provided log_stats_t without dynamic heap allocation.
 *
 * @param[out] out_stats Destination telemetry statistics structure.
 * @return true if populated successfully; false if out_stats is NULL.
 */
bool log_execute_inspect(log_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }
    memset(out_stats, 0, sizeof(*out_stats));
    out_stats->total_entries = s_log.sequence_counter;
    out_stats->ring_buffer_count = s_log.ring_count;
    out_stats->rotation_count = s_log.rotation_count;
    out_stats->active_file_bytes = s_log.current_bytes;
    out_stats->min_severity = s_log.config.min_severity;
    out_stats->is_active = s_log.initialized;
    out_stats->loaded_from_data = s_log.config.loaded_from_data;
    str_safe_copy(
        out_stats->active_profile,
        log_get_active_profile_name(),
        sizeof(out_stats->active_profile)
    );
    return true;
}

/**
 * @brief Formats terminal-friendly ASCII diagnostic status card into output buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Renders a human-readable scorecard showing active telemetry settings and usage.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats fixed-column 60-character diagnostic card safely into out_buf.
 *
 * @param[in]  stats    Optional pre-computed telemetry stats (or NULL to auto-query).
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination character buffer (minimum 320 bytes).
 * @return true if card was successfully formatted; false on buffer overflow.
 */
bool log_execute_dump_card(const log_stats_t *stats, char *out_buf, size_t buf_size)
{
    if (out_buf == NULL || buf_size < 320U) {
        return false;
    }
    log_stats_t local_stats;
    if (stats == NULL) {
        if (!log_execute_inspect(&local_stats)) {
            return false;
        }
        stats = &local_stats;
    }
    int written = snprintf(
        out_buf, buf_size,
        "+------------------------------------------------------------+\n"
        "| FORENSIC TELEMETRY ENGINE STATUS CARD                      |\n"
        "+------------------------------------------------------------+\n"
        "| Active Profile : %-41s |\n"
        "| Initialized    : %-41s |\n"
        "| Data Driven    : %-41s |\n"
        "| Safe Mode      : %-41s |\n"
        "| Total Entries  : %-41llu |\n"
        "| Ring Count     : %-41zu |\n"
        "| Rotation Count : %-41zu |\n"
        "| File Bytes     : %-41zu |\n"
        "+------------------------------------------------------------+",
        stats->active_profile,
        stats->is_active ? "TRUE (Online)" : "FALSE (Offline)",
        stats->loaded_from_data ? "TRUE (Data-Driven)" : "FALSE (Procedural/Fallback)",
        s_log.safe_mode ? "TRUE (Fallback Guard Active)" : "FALSE (Nominal)",
        (unsigned long long)stats->total_entries,
        stats->ring_buffer_count,
        stats->rotation_count,
        stats->active_file_bytes
    );
    return written > 0 && (size_t)written < buf_size;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, open/close streams, emit debug & fallback
diagnostics into master header, and tear down open file descriptors cleanly.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the forensic telemetry subsystem with specified or default config.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts the flight recorder engine. When configuration files are absent, engages
 * emergency safe mode from backup constants and records the backup card to the log.
 * When Debug Mode is enabled, records additional diagnostic state banners directly
 * in the file header for rapid troubleshooting.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates data provenance, byte/retention limits, resolves build/logs sandboxing,
 * verifies directory synthesis via fs_ensure_directory(), opens s_log.file, and
 * integrates both log_fallback and debug_fallback diagnostic cards into file boot.
 * Preserves s_log.sequence_counter across re-initialization for strict monotonicity.
 *
 * @param[in] config Optional configuration profile (or NULL for manifest default).
 * @return true if initialized successfully; false if boot was refused.
 */
bool log_init(const log_config_t *config)
{
    if (s_log.initialized) {
        log_shutdown();
    }

    if (config != NULL) {
        s_log.config = *config;
    } else {
        s_log.config = log_get_default_config();
    }

    if (!s_log.config.loaded_from_data &&
        !log_fallback_is_recognized_profile(&s_log.config)) {
        if (s_log.tripwire_status == LOG_TRIPWIRE_CLEAR) {
            LOG_SET_TRIPWIRE(
                LOG_TRIPWIRE_MANIFEST_MISSING,
                "Refusing boot: configuration not loaded from data-driven or Safe-Mode ROM pipeline."
            );
        }
        return false;
    }

    if (s_log.config.log_dir[0] == '\0' || s_log.config.base_filename[0] == '\0') {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Refusing boot: log_dir or base_filename is empty."
        );
        return false;
    }

    if (s_log.config.max_rotated_files == 0 ||
        s_log.config.max_rotated_files > LOG_MAX_ROTATION_CAP ||
        s_log.config.max_file_size_bytes == 0) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Refusing boot: max_rotated_files invalid (> 25 or 0) or max_file_size_bytes is 0."
        );
        return false;
    }

    s_log.safe_mode = (!s_log.config.loaded_from_data);

    /* Ephemeral sandboxing resolution: route into build/logs if build/ exists */
    if (strcmp(s_log.config.log_dir, "logs") == 0) {
        struct stat st_build;
        if (stat("build", &st_build) == 0 && S_ISDIR(st_build.st_mode)) {
            str_safe_copy(s_log.config.log_dir, "build/logs", sizeof(s_log.config.log_dir));
        }
    } else if (strncmp(s_log.config.log_dir, "build/logs", 10) == 0) {
        struct stat st_build;
        if (stat("build", &st_build) != 0 || !S_ISDIR(st_build.st_mode)) {
            if (fs_file_exists("CTestTestfile.cmake") || fs_file_exists("CMakeCache.txt")) {
                str_safe_copy(s_log.config.log_dir, "logs", sizeof(s_log.config.log_dir));
            }
        }
    }

    if (!fs_ensure_directory(s_log.config.log_dir)) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Failed to synthesize target log output directory."
        );
        return false;
    }

    char full_path[LOG_PATH_BUF_CAP];
    str_path_join(full_path, sizeof(full_path), s_log.config.log_dir, s_log.config.base_filename);

    s_log.file = fopen(full_path, "a");
    if (s_log.file == NULL) {
        LOG_SET_TRIPWIRE(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Failed to open primary log file stream for writing."
        );
        return false;
    }

    struct stat st;
    if (stat(full_path, &st) == 0) {
        s_log.current_bytes = (size_t)st.st_size;
    } else {
        s_log.current_bytes = 0;
    }

    for (size_t i = 0; i < (size_t)LOG_CAT_COUNT; i++) {
        s_log.cat_files[i] = NULL;
        s_log.cat_bytes[i] = 0;
    }

    s_log.ring_head = 0;
    s_log.ring_count = 0;
    s_log.in_write = false;
    s_log.initialized = true;
    log_register_debug_probes();

    char iso_ts[32];
    generate_iso_timestamp(iso_ts, sizeof(iso_ts));
    fprintf(
        s_log.file,
        "================================================================================\n"
        "[%s] [SYSTEM INIT] Tenebris and Lux Forensic Telemetry Subsystem Initialized\n"
        "Standard: ISO 8601 | Profile: %s | Source: %s | SafeMode: %s\n"
        "================================================================================\n",
        iso_ts,
        s_log.config.profile_name[0] != '\0' ? s_log.config.profile_name : "custom",
        s_log.config.loaded_from_data
            ? "DATA-DRIVEN (config/diagnostics/telemetry)"
            : "HARDCODED (log_fallback)",
        s_log.safe_mode ? "ACTIVE" : "OFF"
    );

    /* Emergency Safe-Mode Telemetry: Stamp fallback card into disk archive */
    if (s_log.safe_mode) {
        char fb_card[1024];
        bool fb_ok = log_fallback_audit();
        if (log_fallback_execute_dump_card(&s_log.config, fb_card, sizeof(fb_card))) {
            fprintf(
                s_log.file,
                "[SYSTEM FALLBACK DIAGNOSTIC] Audit: %s\n%s\n",
                fb_ok ? "VERIFIED_PASS" : "AUDIT_FAULT",
                fb_card
            );
        }
    }

    /* Built-in Debug Mode Introspection: Stamp debugger/fallback telemetry without mutating ring */
    if (debug_is_active()) {
        debug_stats_t dbg_stats;
        bool dbg_fb_ok = debug_fallback_audit();
        if (debug_execute_inspect(&dbg_stats)) {
            fprintf(
                s_log.file,
                "[SYSTEM DEBUG MODE] Debugger Active | Profile: %s | Step: %s | Snapshots: %zu/%zu | Traps: %u | DebugFallback: %s\n",
                dbg_stats.active_profile,
                debug_step_mode_name(dbg_stats.step_mode),
                dbg_stats.snapshot_count,
                dbg_stats.max_snapshots,
                dbg_stats.traps_triggered,
                dbg_fb_ok ? "PASS" : "FAIL"
            );
        } else {
            char dbg_card[1024];
            if (debug_fallback_execute_dump_card(dbg_card, sizeof(dbg_card)) > 0) {
                fprintf(
                    s_log.file,
                    "[SYSTEM DEBUG MODE] Debugger Inspect Unavailable; Fallback Profile:\n%s",
                    dbg_card
                );
            }
        }
    }

    /* Replay any pre-init / BIOS Safe-Mode entries into disk archive without mutating ring counts */
    {
        log_entry_t replay_buf[LOG_FALLBACK_RING_CAPACITY];
        size_t drained = log_fallback_drain_entries(replay_buf, LOG_FALLBACK_RING_CAPACITY);
        if (drained > 0U) {
            fprintf(
                s_log.file,
                "[SYSTEM EARLY-BOOT REPLAY] Flushed %zu Tier-0 Safe-Mode entries to primary archive:\n",
                drained
            );
            for (size_t i = 0; i < drained; i++) {
                const log_entry_t *e = &replay_buf[i];
                double rel_ms = (double)e->monotonic_us / 1000.0;
                fprintf(
                    s_log.file,
                    "[%s] [BOOT#%06llu] [+%010.3fms] [%-8s] [CAT:%-9s] [ENTITY:%-16s] [GRADE:%+04d] (%s:%d:%s) %s\n",
                    e->iso_timestamp,
                    (unsigned long long)e->sequence_id,
                    rel_ms,
                    log_severity_name(e->severity),
                    log_category_name(e->category),
                    e->entity,
                    e->grade_percent,
                    (e->file != NULL) ? e->file : "unknown",
                    e->line,
                    (e->func != NULL) ? e->func : "unknown",
                    e->message
                );
            }
        }
    }

    fflush(s_log.file);
    return true;
}

/**
 * @brief Flushes all in-flight buffers, closes file streams, and tears down subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely seals and closes all active log files when the game exits or swaps profiles.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stamps shutdown event count banner, flushes and closes primary and category streams,
 * and marks s_log.initialized and s_log.safe_mode false while preserving monotonic counters.
 */
void log_shutdown(void)
{
    if (!s_log.initialized) {
        return;
    }

    if (s_log.file != NULL) {
        char iso_ts[32];
        generate_iso_timestamp(iso_ts, sizeof(iso_ts));
        fprintf(
            s_log.file,
            "================================================================================\n"
            "[%s] [SYSTEM SHUTDOWN] Telemetry Subsystem Shutdown. Total events: %lu\n"
            "================================================================================\n",
            iso_ts,
            (unsigned long)s_log.sequence_counter
        );
        fflush(s_log.file);
        fclose(s_log.file);
        s_log.file = NULL;
    }

    for (size_t i = 0; i < (size_t)LOG_CAT_COUNT; i++) {
        if (s_log.cat_files[i] != NULL) {
            fflush(s_log.cat_files[i]);
            fclose(s_log.cat_files[i]);
            s_log.cat_files[i] = NULL;
        }
    }

    s_log.initialized = false;
    s_log.safe_mode = false;
    s_log.in_write = false;
    s_log.debug_probes_registered = false;
}
