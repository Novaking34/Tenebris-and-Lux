/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          log_fallback.c
@id            log.fallback
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-10
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry & Flight Recorder Engine
@role          Implementation of the Tier-0 BIOS Safe-Mode Telemetry Code Page,
               Compiled-In .rodata ROM Vault, and Early-Boot Replay Ring
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined outside normal production initialization; external
                  data loading failure triggers tripwire + ROM Safe Mode.
               4. Safe-Mode & ROM configs unconditionally keep loaded_from_data=false.
               5. Read-only / full disk faults degrade seamlessly to .bss RAM Ring
                  + stderr without crashing or silencing critical alerts.
               6. Universal 5-Block Lifecycle Protocol enforced across files.
               7. Strictly ISO C-style comments only (zero double-slash comments).

PURPOSE & ARCHITECTURAL ROLE:
  Implements the BIOS-level Safe-Mode flight recorder and compiled-in .rodata
  telemetry ROM vault for Tenebris and Lux. Keeps forensic telemetry live from
  line 1 of main() before CLI flags, path_finder roots, or disk streams open,
  and hydrates full canonical telemetry profiles from embedded .rodata JSON
  byte arrays when external config directories are offline or missing.

  ### For Game Designers & Non-Programmers:
  Think of this module as the game's emergency black box and built-in ROM manual.
  If the game fails early during startup, boots off a tiny locked floppy disk,
  or has missing configuration folders, this module records events into emergency
  memory, prints fatal alerts safely, loads built-in factory presets from engine
  ROM, and replays stored boot logs as soon as the main log file opens.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time .rodata & .bss: Zero dynamic memory allocation.
  - Shared Pure JSON Setting Parser: Provides log_fallback_parse_config_json()
    and log_fallback_severity_from_string() directly to log.c without duplication.
  - Side-Effect-Free Audit Verification: Preflight checks validate monolith,
    floppy compact preset, and all 10 embedded ROM JSON files cleanly without
    mutating live runtime counters.
================================================================================
*/

#define _POSIX_C_SOURCE 200809L

/*
================================================================================
BLOCK 2: OPENING BLOCK / MODULE ONTOLOGY & 3-TIER INCLUDES
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log_fallback
================================================================================
Mission: 3-tier downward include hierarchy, Safe-Mode ROM/ring limits, private
state storage (.bss & .rodata), and reentrancy guard inlines.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None)
Tier 3: Internal Engine Foundation Headers + Build-Generated Telemetry ROM Header
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/math_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "debug/debug_fallback.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "log/log_fallback.h"
#include "log_telemetry_rom.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Compile-Time Constants
--------------------------------------------------------------------------------
*/
#define MODULE_ID                 LOG_FALLBACK_MODULE_ID
#define LOCAL_LOG_CAT             LOG_CAT_LOG
#define LOCAL_MODULE_TAG          "LogFallback"
#define LOG_FB_PATH_CAP           256U
#define LOG_FB_MAX_ROTATION_CAP   25U

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
Table-driven severity token descriptor and consolidated Safe-Mode singleton.
--------------------------------------------------------------------------------
*/

/**
 * @struct log_fb_sev_token
 * @brief Table row mapping severity string tokens (canonical & alias) to enums.
 *
 * ### For Game Designers & Non-Programmers:
 * Lists every severity keyword recognized in telemetry JSON settings.
 *
 * ### For Systems Programmers & Engine Architects:
 * Centralizes token lookup in .rodata so log_fallback and log.c share one map.
 */
typedef struct log_fb_sev_token {
    const char     *token;
    log_severity_t  severity;
} log_fb_sev_token_t;

/**
 * @struct log_fallback_state
 * @brief Consolidated internal singleton holding Tier-0 BIOS Safe-Mode buffers.
 *
 * ### For Game Designers & Non-Programmers:
 * Holds the emergency memory ring where boot notes wait until main logs start,
 * plus counters tracking ROM presets and read-only disk errors.
 *
 * ### For Systems Programmers & Engine Architects:
 * Strictly allocated in .bss with zero heap overhead and reentrant guard flags.
 */
typedef struct log_fallback_state {
    bool                 booted;
    bool                 disk_fault;
    bool                 rom_hydrated;
    bool                 in_record;
    log_fallback_sink_t  active_sink;
    log_config_t         config;
    FILE                *file;
    size_t               current_bytes;
    uint64_t             sequence_counter;
    uint64_t             total_replayed;
    uint64_t             disk_bytes_written;
    char                 active_rom_profile[32];
    log_entry_t          ring[LOG_FALLBACK_RING_CAPACITY];
    size_t               ring_head;
    size_t               ring_count;
} log_fallback_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.rodata & .bss Allocations)
--------------------------------------------------------------------------------
Immutable factory configuration templates, label tables, and zero-heap state.
--------------------------------------------------------------------------------
*/

static const log_config_t s_log_fallback_template = {
    .log_dir = LOG_FALLBACK_DIR,
    .base_filename = LOG_FALLBACK_BASE_FILENAME,
    .max_file_size_bytes = LOG_FALLBACK_MAX_BYTES,
    .max_rotated_files = LOG_FALLBACK_MAX_FILES,
    .min_severity = LOG_FALLBACK_MIN_SEVERITY,
    .save_by_category = LOG_FALLBACK_SAVE_BY_CAT,
    .echo_to_console = LOG_FALLBACK_ECHO_CONSOLE,
    .use_colors = LOG_FALLBACK_USE_COLORS,
    .profile_name = LOG_FALLBACK_PROFILE_NAME,
    .loaded_from_data = false
};

static const log_config_t s_log_floppy_template = {
    .log_dir = LOG_FALLBACK_FLOPPY_DIR,
    .base_filename = LOG_FALLBACK_FLOPPY_FILENAME,
    .max_file_size_bytes = LOG_FALLBACK_FLOPPY_MAX_BYTES,
    .max_rotated_files = LOG_FALLBACK_FLOPPY_MAX_FILES,
    .min_severity = LOG_FALLBACK_MIN_SEVERITY,
    .save_by_category = false,
    .echo_to_console = false,
    .use_colors = false,
    .profile_name = LOG_FALLBACK_FLOPPY_PROFILE_NAME,
    .loaded_from_data = false
};

static const log_fb_sev_token_t s_fb_sev_tokens[] = {
    { "FATAL",    LOG_SEV_FATAL    },
    { "CRITICAL", LOG_SEV_CRITICAL },
    { "ERROR",    LOG_SEV_ERROR    },
    { "WARNING",  LOG_SEV_WARNING  },
    { "WARN",     LOG_SEV_WARNING  },
    { "INFO",     LOG_SEV_INFO     },
    { "DEBUG",    LOG_SEV_DEBUG    },
    { "TRACE",    LOG_SEV_TRACE    }
};

static const char *const s_fb_sev_labels[LOG_SEV_COUNT] = {
    "FATAL",
    "CRITICAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE"
};

static const char *const s_fb_cat_labels[LOG_CAT_COUNT] = {
    "DEBUG",
    "LOG",
    "EVENT",
    "MESSAGE",
    "CHARACTER"
};

static const char *const s_fb_sink_names[] = {
    "UNARMED",
    "RING_ONLY",
    "RING_AND_STDERR",
    "DISK_AND_RING"
};

static log_fallback_state_t s_fb = {
    .booted = false,
    .disk_fault = false,
    .rom_hydrated = false,
    .in_record = false,
    .active_sink = LOG_FALLBACK_SINK_UNARMED,
    .file = NULL,
    .current_bytes = 0,
    .sequence_counter = 0,
    .total_replayed = 0,
    .disk_bytes_written = 0,
    .ring_head = 0,
    .ring_count = 0
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Predicates
--------------------------------------------------------------------------------
Pure inline validators and formatting helpers operating strictly without heap.
--------------------------------------------------------------------------------
*/

static inline bool log_fb_is_sev_valid(log_severity_t sev)
{
    return (size_t)sev < (size_t)LOG_SEV_COUNT;
}

static inline bool log_fb_is_cat_valid(log_category_t cat)
{
    return (size_t)cat < (size_t)LOG_CAT_COUNT;
}

static inline int log_fb_grade_to_percent(float grade)
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

static void log_fb_format_iso_ts(char *buf, size_t buf_size)
{
    struct timespec ts;
    struct tm tm_utc;
    long usec;

    if (buf == NULL || buf_size == 0U) {
        return;
    }

    clock_gettime(CLOCK_REALTIME, &ts);
    gmtime_r(&ts.tv_sec, &tm_utc);
    usec = ts.tv_nsec / 1000L;

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

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log_fallback
================================================================================
Mission: Construct baseline and floppy Safe-Mode configurations, parse JSON
telemetry configs cleanly, resolve compiled-in .rodata ROM payloads, capture
early-boot telemetry into static .bss storage, and handle disk-fault degradation.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration & Shared Parser Cogs
Mission: Construct stack-allocated Safe-Mode profiles and extract JSON settings.
@triggers
  - Block 4 Audits : log_fallback_preflight() [Steps 1-4],
                     log_fallback_validate_boot_contract()
  - Layer 4 Tests  : tests/unit/log/test_log_fallback_unit.c (Sections 1, 2, 3),
                     tests/functional/test_log_functional.c (Section 2)
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs and returns the static fallback configuration structure.
 *
 * Returns an immutable compile-time constant log_config_t instance. Guarantees
 * that any caller receives an immutable, bounds-checked configuration even when
 * all external filesystem calls fail.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides the absolute bedrock logging defaults (1MB max file size, 10 rotated
 * archives) so logging never crashes even if configuration files are missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap configuration constructor setting loaded_from_data = false.
 *
 * @return Fully-populated log_config_t with loaded_from_data == false.
 */
log_config_t log_get_monolith_fallback_config(void)
{
    return s_log_fallback_template;
}

/**
 * @brief Returns an ultra-compact Safe-Mode profile sized for 1.44 MB floppy media.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns small, space-saving logging limits (64 KB file cap, max 2 files) built
 * for running directly off compact or constrained storage media.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap getter returning s_log_floppy_template with
 * loaded_from_data == false and profile_name == "floppy_compact".
 *
 * @return Compact bounds-checked log_config_t preset.
 */
log_config_t log_fallback_get_floppy_config(void)
{
    return s_log_floppy_template;
}

/**
 * @brief Checks if a Safe-Mode / fallback configuration carries recognized provenance.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies a Safe-Mode config matches an official emergency preset or built-in ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates offline provenance (!loaded_from_data) against monolith_fallback,
 * floppy_compact, or an embedded setting ID present in .rodata telemetry ROM.
 *
 * @param[in] cfg Configuration struct to inspect.
 * @return true if cfg represents a valid recognized Safe-Mode/ROM profile.
 */
bool log_fallback_is_recognized_profile(const log_config_t *cfg)
{
    if (cfg == NULL || cfg->loaded_from_data) {
        return false;
    }

    if (str_case_eq(cfg->profile_name, LOG_FALLBACK_PROFILE_NAME) ||
        str_case_eq(cfg->profile_name, LOG_FALLBACK_FLOPPY_PROFILE_NAME)) {
        return true;
    }

    if (cfg->profile_name[0] != '\0' &&
        !str_case_eq(cfg->profile_name, "manifest") &&
        !str_case_eq(cfg->profile_name, "severity_weights") &&
        !str_case_eq(cfg->profile_name, "engine_channels") &&
        !str_case_eq(cfg->profile_name, "game_channels")) {
        return log_fallback_get_rom_asset(cfg->profile_name, NULL, NULL);
    }

    return false;
}

/**
 * @brief Parses a severity token string case-insensitively using canonical & alias tables.
 *
 * ### For Game Designers & Non-Programmers:
 * Translates words like "INFO", "WARN", or "TRACE" inside telemetry configs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Iterates .rodata s_fb_sev_tokens[] via str_case_eq() cleanly without heap.
 *
 * @param[in]  str     Case-insensitive severity string.
 * @param[out] out_sev Populated severity rank.
 * @return true if recognized; false otherwise.
 */
bool log_fallback_severity_from_string(const char *str, log_severity_t *out_sev)
{
    if (str == NULL || out_sev == NULL) {
        return false;
    }

    size_t count = sizeof(s_fb_sev_tokens) / sizeof(s_fb_sev_tokens[0]);
    for (size_t i = 0; i < count; i++) {
        if (str_case_eq(str, s_fb_sev_tokens[i].token)) {
            *out_sev = s_fb_sev_tokens[i].severity;
            return true;
        }
    }

    return false;
}

/**
 * @brief Parses a JSON "setting" payload into a validated log_config_t.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads log folders, file caps, and severity filters from JSON config text.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure zero-heap setting extractor shared between compiled-in ROM hydration and
 * log.c filesystem config loading, reporting failure tripwires cleanly to callers
 * without hidden global state mutations.
 *
 * @param[in]  scope        Pointer into JSON payload ("setting" block or body).
 * @param[out] out_config   Populated configuration structure.
 * @param[out] out_tripwire Optional failure tripwire enum code (may be NULL).
 * @param[out] err_buf      Optional human-readable error message buffer (may be NULL).
 * @param[in]  err_cap      Capacity of err_buf in bytes.
 * @return true if attributes parsed and validated within bounds; false otherwise.
 */
bool log_fallback_parse_config_json(
    const char *scope,
    log_config_t *out_config,
    log_tripwire_t *out_tripwire,
    char *err_buf,
    size_t err_cap
) {
    if (scope == NULL || out_config == NULL) {
        if (out_tripwire != NULL) {
            *out_tripwire = LOG_TRIPWIRE_SETTING_INVALID;
        }
        if (err_buf != NULL && err_cap > 0U) {
            str_safe_copy(err_buf, "NULL scope or out_config in JSON setting parser.", err_cap);
        }
        return false;
    }

    char tmp[64];
    size_t size_val = 0;
    bool bool_val = false;

    memset(out_config, 0, sizeof(*out_config));
    out_config->min_severity = LOG_SEV_TRACE;
    out_config->save_by_category = true;
    out_config->echo_to_console = true;
    out_config->use_colors = true;
    out_config->loaded_from_data = false;

    if (json_get_string(scope, "id", tmp, sizeof(tmp))) {
        str_safe_copy(out_config->profile_name, tmp, sizeof(out_config->profile_name));
    }
    if (json_get_string(scope, "log_dir", tmp, sizeof(tmp))) {
        str_safe_copy(out_config->log_dir, tmp, sizeof(out_config->log_dir));
    }
    if (json_get_string(scope, "base_filename", tmp, sizeof(tmp))) {
        str_safe_copy(out_config->base_filename, tmp, sizeof(out_config->base_filename));
    }

    if (json_get_size_t(scope, "max_file_size_bytes", &size_val)) {
        out_config->max_file_size_bytes = size_val;
    }
    if (json_get_size_t(scope, "max_rotated_files", &size_val)) {
        out_config->max_rotated_files = size_val;
    }

    if (json_get_string(scope, "min_severity", tmp, sizeof(tmp))) {
        log_severity_t parsed_sev;
        if (log_fallback_severity_from_string(tmp, &parsed_sev)) {
            out_config->min_severity = parsed_sev;
        } else {
            if (out_tripwire != NULL) {
                *out_tripwire = LOG_TRIPWIRE_SETTING_INVALID;
            }
            if (err_buf != NULL && err_cap > 0U) {
                snprintf(
                    err_buf,
                    err_cap,
                    "Unrecognized min_severity token '%s' in telemetry setting.",
                    tmp
                );
            }
            return false;
        }
    }

    if (json_get_bool(scope, "save_by_category", &bool_val)) {
        out_config->save_by_category = bool_val;
    }
    if (json_get_bool(scope, "echo_to_console", &bool_val)) {
        out_config->echo_to_console = bool_val;
    }
    if (json_get_bool(scope, "use_colors", &bool_val)) {
        out_config->use_colors = bool_val;
    }

    if (out_config->max_file_size_bytes == 0U ||
        out_config->max_rotated_files == 0U ||
        out_config->max_rotated_files > LOG_FB_MAX_ROTATION_CAP) {
        if (out_tripwire != NULL) {
            *out_tripwire = LOG_TRIPWIRE_BOUNDS_VIOLATION;
        }
        if (err_buf != NULL && err_cap > 0U) {
            str_safe_copy(
                err_buf,
                "Setting bounds violation: byte limit is zero or rotation cap outside [1, 25].",
                err_cap
            );
        }
        return false;
    }

    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Compiled-In Telemetry ROM Vault Cogs
Mission: Resolve and hydrate telemetry JSON payloads directly out of .rodata.
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves an immutable compiled-in telemetry JSON payload from .rodata ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Fetches a factory backup copy of any telemetry JSON config right from engine ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Matches id_or_rel_path case-insensitively against s_log_telemetry_rom_table[]
 * by short stem ID or full VFS relative path. Zero heap, zero disk I/O.
 *
 * @param[in]  id_or_rel_path Asset ID ("default") or VFS relative path.
 * @param[out] out_json       Optional pointer receiving immutable JSON text.
 * @param[out] out_len        Optional pointer receiving byte length excluding NUL.
 * @return true if matching compiled-in ROM asset exists; false otherwise.
 */
bool log_fallback_get_rom_asset(
    const char *id_or_rel_path,
    const char **out_json,
    size_t *out_len
) {
    if (id_or_rel_path == NULL || id_or_rel_path[0] == '\0') {
        return false;
    }

    for (size_t i = 0; i < (size_t)LOG_TELEMETRY_ROM_ASSET_COUNT; i++) {
        const log_rom_asset_t *asset = &s_log_telemetry_rom_table[i];
        if (str_case_eq(id_or_rel_path, asset->id) ||
            str_case_eq(id_or_rel_path, asset->rel_path)) {
            if (out_json != NULL) {
                *out_json = (const char *)asset->data;
            }
            if (out_len != NULL) {
                *out_len = asset->size;
            }
            return true;
        }
    }

    return false;
}

/**
 * @brief Populates summary metadata about the compiled-in .rodata telemetry ROM vault.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports how many factory config backups are stored in ROM and their byte size.
 *
 * ### For Systems Programmers & Engine Architects:
 * Populates caller log_fallback_rom_info_t strictly from compile-time macros
 * and live active_rom_profile tracking.
 *
 * @param[out] out_info Destination ROM summary struct.
 * @return true on success; false on NULL argument.
 */
bool log_fallback_get_rom_info(log_fallback_rom_info_t *out_info)
{
    if (out_info == NULL) {
        return false;
    }

    memset(out_info, 0, sizeof(*out_info));
    out_info->asset_count = (size_t)LOG_TELEMETRY_ROM_ASSET_COUNT;
    out_info->total_rom_bytes = (size_t)LOG_TELEMETRY_ROM_TOTAL_BYTES;
    str_safe_copy(
        out_info->active_rom_profile,
        (s_fb.active_rom_profile[0] != '\0') ? s_fb.active_rom_profile : "none",
        sizeof(out_info->active_rom_profile)
    );
    return true;
}

/**
 * @brief Internal ROM hydration cog with selectable side-effect telemetry recording.
 */
static bool log_fallback_load_rom_profile_internal(
    const char *profile_name,
    log_config_t *out_config,
    bool update_telemetry
) {
    if (out_config == NULL) {
        return false;
    }

    char target_id[64];
    if (profile_name == NULL || profile_name[0] == '\0') {
        const char *menu_json = NULL;
        size_t menu_len = 0;
        if (!log_fallback_get_rom_asset("manifest", &menu_json, &menu_len)) {
            return false;
        }

        master_loader_t menu_loader;
        if (!master_loader_load_from_memory(&menu_loader, menu_json, menu_len)) {
            return false;
        }

        const char *menu_scope = strstr(menu_loader.body_start, "\"menu\"");
        if (menu_scope == NULL) {
            menu_scope = menu_loader.body_start;
        }

        if (!json_get_string(menu_scope, "active_setting", target_id, sizeof(target_id))) {
            if (!json_get_string(menu_scope, "default_setting", target_id, sizeof(target_id))) {
                return false;
            }
        }
    } else {
        str_safe_copy(target_id, profile_name, sizeof(target_id));
    }

    if (str_case_eq(target_id, "manifest") ||
        str_case_eq(target_id, "severity_weights") ||
        str_case_eq(target_id, "engine_channels") ||
        str_case_eq(target_id, "game_channels")) {
        return false;
    }

    const char *prof_json = NULL;
    size_t prof_len = 0;
    if (!log_fallback_get_rom_asset(target_id, &prof_json, &prof_len)) {
        return false;
    }

    master_loader_t prof_loader;
    if (!master_loader_load_from_memory(&prof_loader, prof_json, prof_len)) {
        return false;
    }

    const char *setting_scope = strstr(prof_loader.body_start, "\"setting\"");
    if (setting_scope == NULL) {
        setting_scope = prof_loader.body_start;
    }

    if (!log_fallback_parse_config_json(setting_scope, out_config, NULL, NULL, 0U)) {
        return false;
    }

    if (out_config->log_dir[0] == '\0' || out_config->base_filename[0] == '\0') {
        return false;
    }

    /* Critical Provenance Invariant: Offline ROM profiles NEVER claim live disk provenance */
    out_config->loaded_from_data = false;

    if (update_telemetry) {
        s_fb.rom_hydrated = true;
        str_safe_copy(
            s_fb.active_rom_profile,
            (out_config->profile_name[0] != '\0') ? out_config->profile_name : target_id,
            sizeof(s_fb.active_rom_profile)
        );
    }

    return true;
}

/**
 * @brief Hydrates a log_config_t directly from compiled-in .rodata ROM assets.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads official telemetry presets directly from compiled-in engine ROM when the
 * disk configuration directories are offline or unavailable.
 *
 * ### For Systems Programmers & Engine Architects:
 * Feeds embedded ROM JSON into master_loader_load_from_memory() and parses cleanly
 * via log_fallback_parse_config_json(), enforcing loaded_from_data == false.
 *
 * @param[in]  profile_name Profile ID (or NULL to resolve ROM manifest's active_setting).
 * @param[out] out_config   Populated configuration structure.
 * @return true if ROM manifest/profile parsed cleanly; false otherwise.
 */
bool log_fallback_load_rom_profile(const char *profile_name, log_config_t *out_config)
{
    return log_fallback_load_rom_profile_internal(profile_name, out_config, true);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Tier-0 Safe-Mode Capture, Disk-Fault Degradation & Replay
Mission: Capture early-boot and Safe-Mode events into static .bss storage,
cascade compact floppy archives cleanly, and drain entries for primary handoff.
--------------------------------------------------------------------------------
*/

/**
 * @brief Normalizes log_dir inside repository build layouts to prevent root pollution.
 */
static void log_fb_resolve_safe_dir(log_config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    if (strcmp(cfg->log_dir, "logs") == 0) {
        struct stat st_build;
        if (stat("build", &st_build) == 0 && S_ISDIR(st_build.st_mode)) {
            str_safe_copy(cfg->log_dir, "build/logs", sizeof(cfg->log_dir));
        }
    } else if (strncmp(cfg->log_dir, "build/logs", 10) == 0) {
        struct stat st_build;
        if (stat("build", &st_build) != 0 || !S_ISDIR(st_build.st_mode)) {
            if (fs_file_exists("CTestTestfile.cmake") || fs_file_exists("CMakeCache.txt")) {
                str_safe_copy(cfg->log_dir, "logs", sizeof(cfg->log_dir));
            }
        }
    }
}

/**
 * @brief Rotates the compact Safe-Mode disk archive stream if size cap is reached.
 */
static void log_fb_rotate_disk_if_needed(void)
{
    if (s_fb.file == NULL || s_fb.config.max_file_size_bytes == 0U ||
        s_fb.current_bytes < s_fb.config.max_file_size_bytes) {
        return;
    }

    fclose(s_fb.file);
    s_fb.file = NULL;

    size_t max_files = s_fb.config.max_rotated_files;
    if (max_files == 0U) {
        max_files = 1U;
    } else if (max_files > LOG_FB_MAX_ROTATION_CAP) {
        max_files = LOG_FB_MAX_ROTATION_CAP;
    }

    char base_path[LOG_FB_PATH_CAP];
    char src_path[LOG_FB_PATH_CAP + 16U];
    char dst_path[LOG_FB_PATH_CAP + 16U];

    str_path_join(base_path, sizeof(base_path), s_fb.config.log_dir, s_fb.config.base_filename);

    snprintf(dst_path, sizeof(dst_path), "%s.%zu", base_path, max_files);
    remove(dst_path);

    for (size_t idx = max_files; idx > 1U; idx--) {
        snprintf(src_path, sizeof(src_path), "%s.%zu", base_path, idx - 1U);
        snprintf(dst_path, sizeof(dst_path), "%s.%zu", base_path, idx);
        rename(src_path, dst_path);
    }

    snprintf(dst_path, sizeof(dst_path), "%s.1", base_path);
    rename(base_path, dst_path);

    s_fb.file = fopen(base_path, "a");
    s_fb.current_bytes = 0;

    if (s_fb.file == NULL) {
        s_fb.disk_fault = true;
        s_fb.active_sink = s_fb.config.echo_to_console
            ? LOG_FALLBACK_SINK_RING_AND_STDERR
            : LOG_FALLBACK_SINK_RING_ONLY;
        debug_fallback_record_fault(
            "log.fallback",
            "Fallback log archive rotation failed opening new file stream",
            __FILE__,
            __LINE__,
            "test_log_fallback_unit.c:Section4"
        );
        debug_fallback_finish_with_fallback(
            "log.fallback",
            "Degraded Safe-Mode rotation sink cleanly to static .bss RAM ring/stderr"
        );
    }
}

/**
 * @brief Pushes an entry into the circular Tier-0 .bss FIFO black-box queue.
 */
static void log_fb_ring_push(const log_entry_t *entry)
{
    s_fb.ring[s_fb.ring_head] = *entry;
    s_fb.ring_head = (s_fb.ring_head + 1U) % LOG_FALLBACK_RING_CAPACITY;
    if (s_fb.ring_count < LOG_FALLBACK_RING_CAPACITY) {
        s_fb.ring_count++;
    }
}

/**
 * @brief Captures a telemetry message inside the Tier-0 BIOS Safe-Mode black box.
 *
 * ### For Game Designers & Non-Programmers:
 * Records critical startup messages or emergency errors even before the main
 * log system finishes initializing or when storage is read-only.
 *
 * ### For Systems Programmers & Engine Architects:
 * Self-arms BIOS ring on first invocation if not yet booted, preserves sequence
 * ordering, mirrors critical/diskless errors to stderr, and writes compact
 * Safe-Mode files with automatic disk-fault degradation.
 *
 * @param[in] severity Log urgency level.
 * @param[in] category Thematic log channel.
 * @param[in] entity   Entity or module identifier.
 * @param[in] grade    Normalized alignment gradient [-1.0f .. +1.0f].
 * @param[in] file     Source file anchor.
 * @param[in] line     Source line anchor.
 * @param[in] func     Source function anchor.
 * @param[in] fmt      Printf format string.
 * @return Non-zero Safe-Mode sequence ID if captured; 0 if filtered or reentrant.
 */
uint64_t log_fallback_record(
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
    if (s_fb.in_record) {
        return 0;
    }
    if (!log_fb_is_sev_valid(severity)) {
        return 0;
    }

    if (!s_fb.booted) {
        log_fallback_boot(NULL);
    }
    if (severity > s_fb.config.min_severity) {
        return 0;
    }

    s_fb.in_record = true;
    s_fb.sequence_counter++;
    uint64_t seq = s_fb.sequence_counter;

    log_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.sequence_id = seq;
    entry.monotonic_us = time_get_monotonic_us();
    log_fb_format_iso_ts(entry.iso_timestamp, sizeof(entry.iso_timestamp));
    entry.severity = severity;
    entry.category = log_fb_is_cat_valid(category) ? category : LOG_CAT_LOG;

    if (entity != NULL && entity[0] != '\0') {
        str_safe_copy(entry.entity, entity, sizeof(entry.entity));
    } else {
        str_safe_copy(entry.entity, "SafeMode", sizeof(entry.entity));
    }

    if (!(grade >= -1.0f && grade <= 1.0f)) {
        entry.grade = (grade > 1.0f) ? 1.0f : ((grade < -1.0f) ? -1.0f : 0.0f);
    } else {
        entry.grade = grade;
    }
    entry.grade_percent = log_fb_grade_to_percent(entry.grade);
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
    }

    log_fb_ring_push(&entry);

    char line_buf[LOG_MAX_LINE_BUFFER];
    double rel_ms = (double)entry.monotonic_us / 1000.0;
    int written = snprintf(
        line_buf,
        sizeof(line_buf),
        "[%s] [SAFE#%06llu] [+%010.3fms] [%-8s] [CAT:%-9s] [ENTITY:%-16s] [GRADE:%+04d] (%s:%d:%s) %s\n",
        entry.iso_timestamp,
        (unsigned long long)entry.sequence_id,
        rel_ms,
        s_fb_sev_labels[entry.severity],
        s_fb_cat_labels[entry.category],
        entry.entity,
        entry.grade_percent,
        (entry.file != NULL) ? entry.file : "unknown",
        entry.line,
        (entry.func != NULL) ? entry.func : "unknown",
        entry.message
    );

    if (written > 0) {
        size_t write_len = ((size_t)written < sizeof(line_buf))
            ? (size_t)written
            : (sizeof(line_buf) - 1U);

        if (s_fb.active_sink == LOG_FALLBACK_SINK_DISK_AND_RING && s_fb.file != NULL) {
            if (fputs(line_buf, s_fb.file) != EOF) {
                s_fb.current_bytes += write_len;
                s_fb.disk_bytes_written += (uint64_t)write_len;
                fflush(s_fb.file);
                log_fb_rotate_disk_if_needed();
            } else {
                s_fb.disk_fault = true;
                fclose(s_fb.file);
                s_fb.file = NULL;
                s_fb.active_sink = LOG_FALLBACK_SINK_RING_AND_STDERR;
                debug_fallback_record_fault(
                    "log.fallback",
                    "Fallback disk stream fputs() write failure; degraded to RAM ring + stderr",
                    __FILE__,
                    __LINE__,
                    "test_log_fallback_unit.c:Section4"
                );
                debug_fallback_finish_with_fallback(
                    "log.fallback",
                    "Closed failed Safe-Mode disk handle and preserved entry in RAM ring + stderr"
                );
            }
        }

        if (s_fb.active_sink == LOG_FALLBACK_SINK_RING_AND_STDERR &&
            severity <= LOG_SEV_ERROR) {
            fputs(line_buf, stderr);
        }
    }

    s_fb.in_record = false;
    return seq;
}

/**
 * @brief Non-destructively inspects buffered Tier-0 Safe-Mode entries in FIFO order.
 *
 * ### For Game Designers & Non-Programmers:
 * Views early-boot backup messages without erasing them from Safe-Mode memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies oldest-first entries out of the static .bss circular buffer cleanly.
 *
 * @param[out] out_entries Caller array receiving copies of buffered entries.
 * @param[in]  max_entries Maximum element capacity of out_entries.
 * @return Number of entries copied.
 */
size_t log_fallback_peek_entries(log_entry_t *out_entries, size_t max_entries)
{
    if (out_entries == NULL || max_entries == 0U || s_fb.ring_count == 0U) {
        return 0U;
    }

    size_t count = (s_fb.ring_count < max_entries) ? s_fb.ring_count : max_entries;
    size_t oldest =
        (s_fb.ring_head + LOG_FALLBACK_RING_CAPACITY - s_fb.ring_count) %
        LOG_FALLBACK_RING_CAPACITY;

    for (size_t i = 0; i < count; i++) {
        size_t idx = (oldest + i) % LOG_FALLBACK_RING_CAPACITY;
        out_entries[i] = s_fb.ring[idx];
    }

    return count;
}

/**
 * @brief Drains buffered Tier-0 Safe-Mode entries in FIFO order for primary handoff.
 *
 * ### For Game Designers & Non-Programmers:
 * Hands off early boot messages stored in Safe Mode to the main log file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Consumes up to max_entries in oldest-first order, updating ring_count and
 * total_replayed so log_init() can flush boot logs cleanly into s_log.file.
 *
 * @param[out] out_entries Caller array receiving entries (or NULL to discard).
 * @param[in]  max_entries Maximum capacity of out_entries.
 * @return Number of entries drained.
 */
size_t log_fallback_drain_entries(log_entry_t *out_entries, size_t max_entries)
{
    if (max_entries == 0U || s_fb.ring_count == 0U) {
        return 0U;
    }

    size_t count = (s_fb.ring_count < max_entries) ? s_fb.ring_count : max_entries;
    size_t oldest =
        (s_fb.ring_head + LOG_FALLBACK_RING_CAPACITY - s_fb.ring_count) %
        LOG_FALLBACK_RING_CAPACITY;

    if (out_entries != NULL) {
        for (size_t i = 0; i < count; i++) {
            size_t idx = (oldest + i) % LOG_FALLBACK_RING_CAPACITY;
            out_entries[i] = s_fb.ring[idx];
        }
    }

    s_fb.ring_count -= count;
    s_fb.total_replayed += (uint64_t)count;
    return count;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log_fallback
================================================================================
Mission: Verify incoming parameters, monolith contracts, compact floppy preset
limits, and compiled-in .rodata ROM health cleanly with zero side effects.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Self-Audit & Pre-Flight Validation Tripwires
@triggers
  - Validates fallback monolith + floppy preset + 10 compiled-in .rodata ROMs
  - Invoked by log.c Block 4 preflight/audit & "log.fallback_bios" debug probe
  - Layer 4 Tests : tests/unit/log/test_log_fallback_unit.c (Sections 1 & 4),
                    tests/integrated/test_log_integrated.c (Section 3)
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic verification routine evaluating fallback & ROM health.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies factory defaults, compact floppy Safe-Mode presets, and every built-in
 * ROM backup config file before the game relies on them.
 *
 * ### For Systems Programmers & Engine Architects:
 * Side-effect-free audit validating monolith/floppy constants, provenance rules,
 * all 10 embedded .rodata ROM JSON envelopes via master_loader_load_from_memory(),
 * and end-to-end Safe-Mode ROM profile hydration without mutating live s_fb state.
 *
 * @return diag_result_t reporting categorized diagnostic outcome.
 */
diag_result_t log_fallback_preflight(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = "core.util.log_fallback";
    res.check_name = "fallback_code_page_and_rom_invariants";
    res.details = "Fallback monolith, floppy preset, and 10 telemetry ROM assets verified.";

    /* Step 1: Retrieve and verify static monolith configuration */
    log_config_t cfg = log_get_monolith_fallback_config();

    if (cfg.loaded_from_data != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Provenance breach: fallback loaded_from_data is not false.";
        return res;
    }

    if (!str_case_eq(cfg.profile_name, LOG_FALLBACK_PROFILE_NAME)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Profile identity breach: does not match LOG_FALLBACK_PROFILE_NAME.";
        return res;
    }

    if (cfg.max_file_size_bytes != LOG_FALLBACK_MAX_BYTES || cfg.max_file_size_bytes == 0U) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Byte bounds violation in fallback configuration.";
        return res;
    }

    if (cfg.max_rotated_files != LOG_FALLBACK_MAX_FILES ||
        cfg.max_rotated_files > LOG_FB_MAX_ROTATION_CAP) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Archive retention bounds violation in fallback configuration.";
        return res;
    }

    if (cfg.min_severity != LOG_FALLBACK_MIN_SEVERITY) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Severity cutoff mismatch in fallback configuration.";
        return res;
    }

    if (!str_case_eq(cfg.log_dir, LOG_FALLBACK_DIR) ||
        !str_case_eq(cfg.base_filename, LOG_FALLBACK_BASE_FILENAME)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Path constants mismatch in fallback configuration.";
        return res;
    }

    if (cfg.save_by_category != LOG_FALLBACK_SAVE_BY_CAT ||
        cfg.echo_to_console != LOG_FALLBACK_ECHO_CONSOLE ||
        cfg.use_colors != LOG_FALLBACK_USE_COLORS) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Channel routing flags mismatch in fallback configuration.";
        return res;
    }

    /* Step 2: Verify compact 1.44 MB floppy Safe-Mode configuration */
    log_config_t floppy_cfg = log_fallback_get_floppy_config();
    if (floppy_cfg.loaded_from_data != false ||
        !str_case_eq(floppy_cfg.profile_name, LOG_FALLBACK_FLOPPY_PROFILE_NAME) ||
        floppy_cfg.max_file_size_bytes != LOG_FALLBACK_FLOPPY_MAX_BYTES ||
        floppy_cfg.max_rotated_files != LOG_FALLBACK_FLOPPY_MAX_FILES ||
        floppy_cfg.save_by_category != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Compact floppy Safe-Mode configuration violates invariants.";
        return res;
    }

    /* Step 3: Verify all compiled-in .rodata telemetry ROM assets cleanly */
    size_t rom_bytes_sum = 0U;
    for (size_t i = 0; i < (size_t)LOG_TELEMETRY_ROM_ASSET_COUNT; i++) {
        const log_rom_asset_t *asset = &s_log_telemetry_rom_table[i];
        if (asset->id == NULL || asset->rel_path == NULL ||
            asset->data == NULL || asset->size == 0U ||
            asset->data[asset->size] != '\0') {
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "Embedded ROM telemetry asset header/NUL boundary corrupt.";
            return res;
        }

        master_loader_t loader;
        if (!master_loader_load_from_memory(&loader, (const char *)asset->data, asset->size) ||
            !loader_is_loaded(&loader) ||
            !loader_header_is_5block(&loader.header) ||
            loader.header.schema_id[0] == '\0') {
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "Embedded ROM telemetry asset failed 5-block master_loader audit.";
            return res;
        }
        rom_bytes_sum += asset->size;
    }

    if (rom_bytes_sum != (size_t)LOG_TELEMETRY_ROM_TOTAL_BYTES) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "ROM total byte counter mismatch.";
        return res;
    }

    /* Step 4: Verify ROM Safe-Mode profile hydration without mutating s_fb */
    log_config_t rom_cfg;
    if (!log_fallback_load_rom_profile_internal(NULL, &rom_cfg, false) ||
        rom_cfg.loaded_from_data != false ||
        rom_cfg.max_file_size_bytes == 0U ||
        rom_cfg.base_filename[0] == '\0') {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "ROM Safe-Mode default hydration test failed preflight.";
        return res;
    }

    return res;
}

/**
 * @brief Diagnostic verification routine asserting integrity of fallback & ROM vault.
 *
 * @return true if all static fallback & ROM invariants hold cleanly; false otherwise.
 */
bool log_fallback_audit(void)
{
    diag_result_t res = log_fallback_preflight();
    return res.passed;
}

/**
 * @brief Validates Tier-0 BIOS Safe-Mode boot contract, ROM table, and disk health.
 *
 * @triggers
 * - Invoked by log.c validation probe "log.fallback_bios" when debug is active
 *   or explicitly after emergency boot / recovery handoff.
 * - Maps to tests/unit/log/test_log_fallback_unit.c (Sections 1 & 4) and
 *   tests/integrated/test_log_integrated.c (Section 3).
 *
 * @return diag_result_t reporting SEVERITY_PASS or categorized ROM/disk fault.
 */
diag_result_t log_fallback_validate_boot_contract(void)
{
    diag_result_t res = log_fallback_preflight();
    if (!res.passed) {
        return res;
    }

    log_fallback_rom_info_t rom_info;
    if (!log_fallback_get_rom_info(&rom_info) ||
        rom_info.asset_count == 0U ||
        rom_info.total_rom_bytes == 0U) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.subsystem = "core.util.log_fallback";
        res.check_name = "log_fallback_validate_boot_contract";
        res.details = "Telemetry ROM vault metadata reported zero compiled-in assets.";
        return res;
    }

    if (s_fb.disk_fault) {
        res.passed = false;
        res.severity = SEVERITY_WARNING;
        res.subsystem = "core.util.log_fallback";
        res.check_name = "log_fallback_validate_boot_contract";
        res.details = "Tier-0 BIOS Safe-Mode degraded from disk stream due to storage I/O fault.";
        return res;
    }

    res.subsystem = "core.util.log_fallback";
    res.check_name = "log_fallback_validate_boot_contract";
    res.details = "Tier-0 BIOS Safe-Mode boot contract, ROM vault, and sink verified healthy.";
    return res;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Machine: core.util.log_fallback
================================================================================
Mission: Expose Safe-Mode stats inspection, render ASCII diagnostic status cards,
and manage Tier-0 BIOS Safe-Mode boot, ROM emergency fallback, and teardown.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Retrieve Safe-Mode counters and format aligned ASCII inspection cards.
--------------------------------------------------------------------------------
*/

/**
 * @brief Populates caller statistics report from live Tier-0 Safe-Mode state.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads whether Safe Mode is armed, how many early messages were caught, and ROM metrics.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection copying s_fb telemetry into caller struct.
 *
 * @param[out] out_stats Destination Safe-Mode telemetry struct.
 * @return true on success; false on NULL argument.
 */
bool log_fallback_get_stats(log_fallback_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }

    memset(out_stats, 0, sizeof(*out_stats));
    out_stats->booted = s_fb.booted;
    out_stats->disk_fault = s_fb.disk_fault;
    out_stats->rom_hydrated = s_fb.rom_hydrated;
    out_stats->active_sink = s_fb.active_sink;
    out_stats->total_recorded = s_fb.sequence_counter;
    out_stats->total_replayed = s_fb.total_replayed;
    out_stats->disk_bytes_written = s_fb.disk_bytes_written;
    out_stats->ring_count = s_fb.ring_count;
    out_stats->ring_capacity = (size_t)LOG_FALLBACK_RING_CAPACITY;
    out_stats->rom_asset_count = (size_t)LOG_TELEMETRY_ROM_ASSET_COUNT;
    out_stats->rom_total_bytes = (size_t)LOG_TELEMETRY_ROM_TOTAL_BYTES;

    if (s_fb.booted && s_fb.config.profile_name[0] != '\0') {
        str_safe_copy(
            out_stats->active_profile,
            s_fb.config.profile_name,
            sizeof(out_stats->active_profile)
        );
    } else if (s_fb.active_rom_profile[0] != '\0') {
        str_safe_copy(
            out_stats->active_profile,
            s_fb.active_rom_profile,
            sizeof(out_stats->active_profile)
        );
    } else {
        str_safe_copy(
            out_stats->active_profile,
            LOG_FALLBACK_PROFILE_NAME,
            sizeof(out_stats->active_profile)
        );
    }

    return true;
}

/**
 * @brief Populates caller configuration with inspected fallback state.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the emergency backup logging configuration parameters.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection procedure populating a `log_config_t` object
 * with static compile-time fallback defaults.
 *
 * @param[out] out_cfg Destination configuration structure to populate.
 * @return true on successful population; false on NULL argument.
 */
bool log_fallback_execute_inspect(log_config_t *out_cfg)
{
    if (out_cfg == NULL) {
        return false;
    }
    *out_cfg = log_get_monolith_fallback_config();
    return true;
}

/**
 * @brief Formats an ASCII diagnostic inspection card for logging fallback parameters.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text summary of Safe-Mode configuration, sink status, and ROM state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a standardized 60-column fixed-width ASCII status card into `out_buf`.
 * Requires at least 768 bytes buffer capacity (recommended: 1024 bytes).
 *
 * @param[in]  cfg      Optional configuration pointer (or NULL for default fallback).
 * @param[out] out_buf  Destination character buffer (minimum 768 bytes).
 * @param[in]  buf_size Size of destination buffer.
 * @return true if inspection card was formatted cleanly; false on error or truncation.
 */
bool log_fallback_execute_dump_card(const log_config_t *cfg, char *out_buf, size_t buf_size)
{
    if (out_buf == NULL || buf_size < 768U) {
        return false;
    }

    log_config_t local_cfg;
    if (cfg == NULL) {
        if (!log_fallback_execute_inspect(&local_cfg)) {
            return false;
        }
        cfg = &local_cfg;
    }

    size_t sink_idx = ((size_t)s_fb.active_sink <= (size_t)LOG_FALLBACK_SINK_DISK_AND_RING)
        ? (size_t)s_fb.active_sink
        : 0U;

    char cap_col[48];
    char rom_col[48];
    char sink_col[48];
    char dbg_col[48];

    snprintf(
        cap_col,
        sizeof(cap_col),
        "%zu B / %zu files",
        cfg->max_file_size_bytes,
        cfg->max_rotated_files
    );
    snprintf(
        rom_col,
        sizeof(rom_col),
        "%u assets (%u B) [%s]",
        (unsigned)LOG_TELEMETRY_ROM_ASSET_COUNT,
        (unsigned)LOG_TELEMETRY_ROM_TOTAL_BYTES,
        s_fb.rom_hydrated ? "HYDRATED" : "STANDBY"
    );
    snprintf(
        sink_col,
        sizeof(sink_col),
        "%s%s",
        s_fb_sink_names[sink_idx],
        s_fb.disk_fault ? " [DISK_FAULT]" : ""
    );
    snprintf(
        dbg_col,
        sizeof(dbg_col),
        "FALSE (%s | DbgFb:%s)",
        s_fb.rom_hydrated ? "ROM Safe-Mode" : "Static Monolith",
        debug_fallback_audit() ? "PASS" : "FAIL"
    );

    int written = snprintf(
        out_buf, buf_size,
        "+------------------------------------------------------------+\n"
        "| FORENSIC TELEMETRY FALLBACK / BIOS SAFE-MODE CARD          |\n"
        "+------------------------------------------------------------+\n"
        "| Profile Name   : %-41.41s |\n"
        "| Log Directory  : %-41.41s |\n"
        "| Base Filename  : %-41.41s |\n"
        "| Rotation Cap   : %-41.41s |\n"
        "| Telemetry ROM  : %-41.41s |\n"
        "| Active Sink    : %-41.41s |\n"
        "| Data Driven    : %-41.41s |\n"
        "+------------------------------------------------------------+",
        cfg->profile_name,
        cfg->log_dir,
        cfg->base_filename,
        cap_col,
        rom_col,
        sink_col,
        cfg->loaded_from_data ? "TRUE (Data-Driven)" : dbg_col
    );

    return written > 0 && (size_t)written < buf_size;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Boot Tier-0 BIOS Safe Mode early in startup, initialize emergency
monolith or ROM profiles on disk failure, and tear down cleanly on shutdown.
--------------------------------------------------------------------------------
*/

/**
 * @brief Arms the Tier-0 BIOS Safe-Mode logger before CLI flags or disk VFS boot.
 *
 * ### For Game Designers & Non-Programmers:
 * Turns on emergency logging immediately when the program starts so early boot
 * bugs or missing folders are never lost, while leaving repo folders clean.
 *
 * ### For Systems Programmers & Engine Architects:
 * When opt_cfg is NULL, arms purely in .bss ring + stderr alert mode with zero
 * filesystem I/O. When opt_cfg is supplied, opens bounded Safe-Mode disk output
 * (applying build/logs sandbox protection) and degrades gracefully to RAM Ring +
 * stderr if mkdir/fopen fails on read-only or full media.
 *
 * @param[in] opt_cfg Optional Safe-Mode config (NULL for clean RAM BIOS boot).
 * @return true on clean BIOS Safe-Mode arming (survives disk failures).
 */
bool log_fallback_boot(const log_config_t *opt_cfg)
{
    if (s_fb.file != NULL) {
        fflush(s_fb.file);
        fclose(s_fb.file);
        s_fb.file = NULL;
    }

    s_fb.disk_fault = false;
    s_fb.current_bytes = 0;

    if (opt_cfg == NULL) {
        s_fb.config = s_log_floppy_template;
        s_fb.config.loaded_from_data = false;
        s_fb.active_sink = LOG_FALLBACK_SINK_RING_AND_STDERR;
        s_fb.booted = true;
        return true;
    }

    s_fb.config = *opt_cfg;
    s_fb.config.loaded_from_data = false;

    if (s_fb.config.max_file_size_bytes == 0U) {
        s_fb.config.max_file_size_bytes = LOG_FALLBACK_FLOPPY_MAX_BYTES;
    }
    if (s_fb.config.max_rotated_files == 0U ||
        s_fb.config.max_rotated_files > LOG_FB_MAX_ROTATION_CAP) {
        s_fb.config.max_rotated_files = LOG_FALLBACK_FLOPPY_MAX_FILES;
    }
    if (s_fb.config.log_dir[0] == '\0' || s_fb.config.base_filename[0] == '\0') {
        s_fb.disk_fault = true;
        s_fb.active_sink = s_fb.config.echo_to_console
            ? LOG_FALLBACK_SINK_RING_AND_STDERR
            : LOG_FALLBACK_SINK_RING_ONLY;
        s_fb.booted = true;
        return true;
    }

    log_fb_resolve_safe_dir(&s_fb.config);

    if (!fs_ensure_directory(s_fb.config.log_dir)) {
        s_fb.disk_fault = true;
        s_fb.active_sink = s_fb.config.echo_to_console
            ? LOG_FALLBACK_SINK_RING_AND_STDERR
            : LOG_FALLBACK_SINK_RING_ONLY;
        s_fb.booted = true;
        return true;
    }

    char full_path[LOG_FB_PATH_CAP];
    str_path_join(full_path, sizeof(full_path), s_fb.config.log_dir, s_fb.config.base_filename);

    s_fb.file = fopen(full_path, "a");
    if (s_fb.file == NULL) {
        s_fb.disk_fault = true;
        s_fb.active_sink = s_fb.config.echo_to_console
            ? LOG_FALLBACK_SINK_RING_AND_STDERR
            : LOG_FALLBACK_SINK_RING_ONLY;
        s_fb.booted = true;
        return true;
    }

    struct stat st;
    if (stat(full_path, &st) == 0) {
        s_fb.current_bytes = (size_t)st.st_size;
    } else {
        s_fb.current_bytes = 0;
    }

    s_fb.active_sink = LOG_FALLBACK_SINK_DISK_AND_RING;
    s_fb.booted = true;
    return true;
}

/**
 * @brief Flushes any Safe-Mode file stream and tears down Tier-0 BIOS runtime state.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely seals the emergency backup file and resets Safe-Mode capture state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Flushes/closes s_fb.file and resets .bss counters cleanly without freeing heap.
 */
void log_fallback_shutdown(void)
{
    if (s_fb.file != NULL) {
        fflush(s_fb.file);
        fclose(s_fb.file);
        s_fb.file = NULL;
    }

    s_fb.booted = false;
    s_fb.disk_fault = false;
    s_fb.in_record = false;
    s_fb.active_sink = LOG_FALLBACK_SINK_UNARMED;
    s_fb.current_bytes = 0;
    s_fb.ring_head = 0;
    s_fb.ring_count = 0;
}

/**
 * @brief Queries whether the Tier-0 BIOS Safe-Mode logger is currently armed.
 *
 * @return true if BIOS Safe Mode has been booted or self-armed.
 */
bool log_fallback_is_booted(void)
{
    return s_fb.booted;
}

/**
 * @brief Emergency initialization helper utilizing the monolithic code page.
 *
 * Invokes log_init() with the static code page returned by
 * log_get_monolith_fallback_config(). If writable disk creation fails completely
 * (e.g. write-protected or full floppy), falls back seamlessly into Tier-0
 * RAM Ring + stderr Safe Mode so emergency logging remains online.
 *
 * ### For Game Designers & Non-Programmers:
 * Emergency ignition button: boots the logging system using hardcoded backup
 * settings if manifest files fail to load or storage is locked.
 *
 * ### For Systems Programmers & Engine Architects:
 * Safe-mode lifecycle constructor attempting full primary monolith attachment
 * before gracefully degrading to Tier-0 Safe Mode on OS I/O failure.
 *
 * @return true on successful primary or Safe-Mode activation.
 */
bool log_init_emergency_monolith(void)
{
    log_config_t cfg = log_get_monolith_fallback_config();
    if (log_init(&cfg)) {
        return true;
    }

    return log_fallback_boot(&cfg);
}
