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
@date          2026-11-09
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
  Integrates directly with data-driven configuration assets residing in
  engine/config/diagnostics/telemetry/ (manifest.json, default.json, development.json,
  production.json, quiet.json, combat_focus.json, forensic_telemetry.json)
  with quarantined emergency fallback protection (log_fallback.h).

  ### For Game Designers & Non-Programmers:
  Acts as the black-box flight recorder of the game. Every combat action, stance
  shift, and narrative dialogue prompt is recorded with precise moral sway and
  timing metrics.

  ### For Systems Programmers & Engine Architects:
  - 100% Zero-Heap Memory Model (ALLOC_ZERO_HEAP): All buffers reside in .bss
    or on caller CPU stack frames.
  - Multi-tier severity hierarchy with fixed-width formatting.
  - Dual-destination dispatch: rotating disk files and static ring buffer.
================================================================================
*/

#define _POSIX_C_SOURCE 200809L

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
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
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Static Engine State Variables (Quarantined in .bss)
--------------------------------------------------------------------------------
*/

static log_config_t s_config;
static bool s_initialized = false;
static FILE *s_file = NULL;
static size_t s_current_bytes = 0;
static uint64_t s_sequence_counter = 0;
static size_t s_rotation_count = 0;

static log_tripwire_t s_tripwire_status = LOG_TRIPWIRE_CLEAR;
static char s_tripwire_details[256] = "All data pipeline tripwires clear.";

static FILE *s_cat_files[LOG_CAT_COUNT] = { NULL };
static size_t s_cat_bytes[LOG_CAT_COUNT] = { 0 };

static log_entry_t s_ring_buffer[LOG_RING_BUFFER_CAPACITY];
static size_t s_ring_head = 0;
static size_t s_ring_count = 0;

static const char *s_severity_labels[LOG_SEV_COUNT] = {
    "FATAL   ",
    "CRITICAL",
    "ERROR   ",
    "WARNING ",
    "INFO    ",
    "DEBUG   ",
    "TRACE   "
};

static const char *s_category_labels[LOG_CAT_COUNT] = {
    "DEBUG    ",
    "LOG      ",
    "EVENT    ",
    "MESSAGE  ",
    "CHARACTER"
};

static const char *s_category_dir_names[LOG_CAT_COUNT] = {
    "debug",
    "log",
    "event",
    "message",
    "character"
};

static const char *s_severity_colors[LOG_SEV_COUNT] = {
    "\033[1;31m", /* FATAL: Bold Red */
    "\033[0;31m", /* CRITICAL: Dark Red */
    "\033[1;91m", /* ERROR: Bright Red */
    "\033[0;33m", /* WARNING: Yellow */
    "\033[0;32m", /* INFO: Green */
    "\033[0;36m", /* DEBUG: Cyan */
    "\033[0;90m"  /* TRACE: Dark Gray */
};

#define ANSI_RESET "\033[0m"

static const char *const s_log_invariants[5] = {
    "Monotonic sequence counter: ID strictly increments per entry",
    "Gradient grade range: [-1.0f, +1.0f] maps to [-100, +100]",
    "Fixed-width token columns guarantee deterministic regex/grep parsing",
    "Automated rotation bounds: Log file never exceeds configured byte ceiling",
    "Static memory boundary: In-flight ring buffer never dynamic allocates"
};

const module_metadata_t log_manifest = {
    "core.util.log",
    "log/log.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    3,
    "2026-11-06",
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
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Profile & Manifest Resolution Cogs
--------------------------------------------------------------------------------
*/

static void set_tripwire(log_tripwire_t status, const char *msg)
{
    s_tripwire_status = status;
    str_safe_copy(s_tripwire_details, msg, sizeof(s_tripwire_details));
}

log_tripwire_t log_get_tripwire_status(void)
{
    return s_tripwire_status;
}

const char *log_get_tripwire_details(void)
{
    return s_tripwire_details;
}

void log_reset_tripwire(void)
{
    s_tripwire_status = LOG_TRIPWIRE_CLEAR;
    str_safe_copy(
        s_tripwire_details,
        "All data pipeline tripwires clear.",
        sizeof(s_tripwire_details)
    );
}

bool log_severity_from_string(const char *str, log_severity_t *out_sev)
{
    if (str == NULL || out_sev == NULL) {
        return false;
    }
    if (str_case_eq(str, "FATAL")) {
        *out_sev = LOG_SEV_FATAL;
        return true;
    }
    if (str_case_eq(str, "CRITICAL")) {
        *out_sev = LOG_SEV_CRITICAL;
        return true;
    }
    if (str_case_eq(str, "ERROR")) {
        *out_sev = LOG_SEV_ERROR;
        return true;
    }
    if (str_case_eq(str, "WARNING") || str_case_eq(str, "WARN")) {
        *out_sev = LOG_SEV_WARNING;
        return true;
    }
    if (str_case_eq(str, "INFO")) {
        *out_sev = LOG_SEV_INFO;
        return true;
    }
    if (str_case_eq(str, "DEBUG")) {
        *out_sev = LOG_SEV_DEBUG;
        return true;
    }
    if (str_case_eq(str, "TRACE")) {
        *out_sev = LOG_SEV_TRACE;
        return true;
    }
    return false;
}

bool log_category_from_string(const char *str, log_category_t *out_cat)
{
    if (str == NULL || out_cat == NULL) {
        return false;
    }
    if (str_case_eq(str, "DEBUG")) {
        *out_cat = LOG_CAT_DEBUG;
        return true;
    }
    if (str_case_eq(str, "LOG") || str_case_eq(str, "SYSTEM")) {
        *out_cat = LOG_CAT_LOG;
        return true;
    }
    if (str_case_eq(str, "EVENT")) {
        *out_cat = LOG_CAT_EVENT;
        return true;
    }
    if (str_case_eq(str, "MESSAGE")) {
        *out_cat = LOG_CAT_MESSAGE;
        return true;
    }
    if (str_case_eq(str, "CHARACTER")) {
        *out_cat = LOG_CAT_CHARACTER;
        return true;
    }
    return false;
}

bool log_load_config_from_file(const char *filepath, log_config_t *out_config)
{
    if (filepath == NULL || out_config == NULL) {
        set_tripwire(
            LOG_TRIPWIRE_SETTING_INVALID,
            "NULL filepath or out_config passed to log_load_config_from_file."
        );
        return false;
    }

    static char file_buf[8192];
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
            set_tripwire(LOG_TRIPWIRE_SETTING_MISSING, err_msg);
            return false;
        }
    }

    const char *scope = strstr(file_buf, "\"setting\"");
    if (scope == NULL) {
        scope = (loaded && loader.body_start != NULL) ? loader.body_start : file_buf;
    }

    memset(out_config, 0, sizeof(*out_config));
    out_config->min_severity = LOG_SEV_TRACE;
    out_config->save_by_category = true;
    out_config->echo_to_console = true;
    out_config->use_colors = true;
    out_config->loaded_from_data = false;

    char tmp[64];
    if (json_get_string(scope, "id", tmp, sizeof(tmp))) {
        str_safe_copy(out_config->profile_name, tmp, sizeof(out_config->profile_name));
    }
    if (json_get_string(scope, "log_dir", tmp, sizeof(tmp))) {
        str_safe_copy(out_config->log_dir, tmp, sizeof(out_config->log_dir));
    }
    if (json_get_string(scope, "base_filename", tmp, sizeof(tmp))) {
        str_safe_copy(out_config->base_filename, tmp, sizeof(out_config->base_filename));
    }

    size_t size_val = 0;
    if (json_get_size_t(scope, "max_file_size_bytes", &size_val)) {
        out_config->max_file_size_bytes = size_val;
    }
    if (json_get_size_t(scope, "max_rotated_files", &size_val)) {
        out_config->max_rotated_files = size_val;
    }

    if (json_get_string(scope, "min_severity", tmp, sizeof(tmp))) {
        log_severity_t parsed_sev;
        if (log_severity_from_string(tmp, &parsed_sev)) {
            out_config->min_severity = parsed_sev;
        }
    }

    bool bool_val = false;
    if (json_get_bool(scope, "save_by_category", &bool_val)) {
        out_config->save_by_category = bool_val;
    }
    if (json_get_bool(scope, "echo_to_console", &bool_val)) {
        out_config->echo_to_console = bool_val;
    }
    if (json_get_bool(scope, "use_colors", &bool_val)) {
        out_config->use_colors = bool_val;
    }

    if (out_config->max_file_size_bytes == 0 || out_config->max_rotated_files == 0) {
        set_tripwire(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Setting bounds violation: max_file_size_bytes or max_rotated_files is zero."
        );
        return false;
    }

    out_config->loaded_from_data = true;
    s_tripwire_status = LOG_TRIPWIRE_CLEAR;
    return true;
}

bool log_load_config_from_manifest(const char *manifest_path, log_config_t *out_config)
{
    if (out_config == NULL) {
        set_tripwire(
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
        if (path_finder_resolve("system", "diagnostics/telemetry/manifest.json", resolved_path, sizeof(resolved_path)) ||
            path_finder_resolve("system", "telemetry/manifest.json", resolved_path, sizeof(resolved_path)) ||
            fs_resolve_data_path("telemetry/manifest.json", resolved_path, sizeof(resolved_path)) ||
            fs_resolve_data_path("log/manifest.json", resolved_path, sizeof(resolved_path))) {
            found = true;
        }
    }

    if (!found) {
        set_tripwire(
            LOG_TRIPWIRE_MANIFEST_MISSING,
            "Manifest file system/diagnostics/telemetry/manifest.json not accessible in search paths."
        );
        return false;
    }

    static char manifest_buf[16384];
    master_loader_t loader;
    if (!master_loader_load_from_path(&loader, resolved_path, manifest_buf, sizeof(manifest_buf))) {
        if (!fs_read_file_contents(resolved_path, manifest_buf, sizeof(manifest_buf))) {
            set_tripwire(
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
            set_tripwire(
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

bool log_load_config_profile(const char *profile_name, log_config_t *out_config)
{
    if (profile_name == NULL || profile_name[0] == '\0' || out_config == NULL) {
        set_tripwire(
            LOG_TRIPWIRE_SETTING_INVALID,
            "Invalid profile_name or NULL out_config."
        );
        return false;
    }

    char file_req[PATH_FINDER_MAX_PATH];
    char resolved_profile[PATH_FINDER_MAX_PATH];

    /* 1. Primary path: system domain diagnostics/telemetry/<profile>.json */
    snprintf(file_req, sizeof(file_req), "diagnostics/telemetry/%s.json", profile_name);
    if (path_finder_resolve("system", file_req, resolved_profile, sizeof(resolved_profile))) {
        return log_load_config_from_file(resolved_profile, out_config);
    }

    /* 2. Secondary path: telemetry/<profile>.json */
    snprintf(file_req, sizeof(file_req), "telemetry/%s.json", profile_name);
    if (fs_resolve_data_path(file_req, resolved_profile, sizeof(resolved_profile))) {
        return log_load_config_from_file(resolved_profile, out_config);
    }

    /* 3. Tertiary path: log/<profile>.json */
    snprintf(file_req, sizeof(file_req), "log/%s.json", profile_name);
    if (fs_resolve_data_path(file_req, resolved_profile, sizeof(resolved_profile))) {
        return log_load_config_from_file(resolved_profile, out_config);
    }

    /* 4. Quaternary path: bare <profile>.json */
    snprintf(file_req, sizeof(file_req), "%s.json", profile_name);
    if (fs_resolve_data_path(file_req, resolved_profile, sizeof(resolved_profile))) {
        return log_load_config_from_file(resolved_profile, out_config);
    }

    char err_msg[128];
    snprintf(err_msg, sizeof(err_msg), "Setting profile '%s' not found.", profile_name);
    set_tripwire(LOG_TRIPWIRE_SETTING_MISSING, err_msg);
    return false;
}

const char *log_get_active_profile_name(void)
{
    if (s_config.profile_name[0] != '\0') {
        return s_config.profile_name;
    }
    return "default";
}

bool log_is_data_driven(void)
{
    return s_config.loaded_from_data;
}

log_config_t log_get_default_config(void)
{
    log_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Primary Path: Dynamic resolution from manifest */
    if (log_load_config_from_manifest(NULL, &cfg)) {
        return cfg;
    }

    /* Tripwire Fallback Path: Data pipeline breach triggers fallback monolith */
    set_tripwire(
        LOG_TRIPWIRE_MANIFEST_MISSING,
        "Data pipeline breach: telemetry manifest missing; activating tripwire fallback."
    );

    cfg = log_get_monolith_fallback_config();
    cfg.loaded_from_data = false;
    return cfg;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: File Engine & Telemetry State Cogs
Mission: Manage directory synthesis, state queries, and perform rotation.
--------------------------------------------------------------------------------
*/

static void generate_iso_timestamp(char *buf, size_t buf_size)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm_utc;
    gmtime_r(&ts.tv_sec, &tm_utc);

    long usec = ts.tv_nsec / 1000;
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

static void ensure_directory(const char *dir)
{
    fs_ensure_directory(dir);
}

static void get_log_filepath(char *dest, size_t dest_size, const char *dir, const char *name)
{
    str_path_join(dest, dest_size, dir, name);
}

bool log_rotate_if_needed(void)
{
    if (!s_initialized || s_file == NULL) {
        return false;
    }
    if (s_current_bytes < s_config.max_file_size_bytes) {
        return false;
    }

    fclose(s_file);
    s_file = NULL;

    char old_path[512];
    char new_path[512];

    for (int i = (int)s_config.max_rotated_files - 1; i >= 1; i--) {
        char name_old[256];
        char name_new[256];
        snprintf(name_old, sizeof(name_old), "%s.%d", s_config.base_filename, i);
        snprintf(name_new, sizeof(name_new), "%s.%d", s_config.base_filename, i + 1);
        get_log_filepath(old_path, sizeof(old_path), s_config.log_dir, name_old);
        get_log_filepath(new_path, sizeof(new_path), s_config.log_dir, name_new);
        rename(old_path, new_path);
    }

    char base_path[512];
    char first_rot[512];
    get_log_filepath(base_path, sizeof(base_path), s_config.log_dir, s_config.base_filename);
    char name_first[256];
    snprintf(name_first, sizeof(name_first), "%s.1", s_config.base_filename);
    get_log_filepath(first_rot, sizeof(first_rot), s_config.log_dir, name_first);
    rename(base_path, first_rot);

    s_file = fopen(base_path, "a");
    s_current_bytes = 0;
    s_rotation_count++;

    if (s_file != NULL) {
        char iso_ts[32];
        generate_iso_timestamp(iso_ts, sizeof(iso_ts));
        fprintf(
            s_file,
            "================================================================================\n"
            "[%s] [SYSTEM ROTATION] Log file rotated; previous archive saved to %s\n"
            "================================================================================\n",
            iso_ts,
            first_rot
        );
        fflush(s_file);
    }

    return true;
}

static void rotate_category_file(log_category_t cat)
{
    FILE *f = s_cat_files[cat];
    if (f == NULL || s_cat_bytes[cat] < s_config.max_file_size_bytes) {
        return;
    }

    fclose(f);
    s_cat_files[cat] = NULL;

    const char *cat_dir_name = log_category_dir_name(cat);
    char cat_dir[256];
    snprintf(cat_dir, sizeof(cat_dir), "%s/%s", s_config.log_dir, cat_dir_name);

    char base_name[128];
    snprintf(base_name, sizeof(base_name), "%s.log", cat_dir_name);

    char old_p[512];
    char new_p[512];

    for (int i = (int)s_config.max_rotated_files - 1; i >= 1; i--) {
        char name_old[256];
        char name_new[256];
        snprintf(name_old, sizeof(name_old), "%s.%d", base_name, i);
        snprintf(name_new, sizeof(name_new), "%s.%d", base_name, i + 1);
        get_log_filepath(old_p, sizeof(old_p), cat_dir, name_old);
        get_log_filepath(new_p, sizeof(new_p), cat_dir, name_new);
        rename(old_p, new_p);
    }

    char base_path[512];
    char first_rot[512];
    get_log_filepath(base_path, sizeof(base_path), cat_dir, base_name);
    char name_first[256];
    snprintf(name_first, sizeof(name_first), "%s.1", base_name);
    get_log_filepath(first_rot, sizeof(first_rot), cat_dir, name_first);
    rename(base_path, first_rot);

    s_cat_files[cat] = fopen(base_path, "a");
    s_cat_bytes[cat] = 0;
}

bool log_is_active(void)
{
    return s_initialized;
}

uint64_t log_get_total_entries(void)
{
    return s_sequence_counter;
}

int log_grade_to_percent(float grade)
{
    if (grade > 1.0f) {
        grade = 1.0f;
    } else if (grade < -1.0f) {
        grade = -1.0f;
    }
    float scaled = grade * 100.0f;
    int rounded = (scaled >= 0.0f) ? (int)(scaled + 0.5f) : (int)(scaled - 0.5f);
    return math_clamp_int(rounded, -100, 100);
}

float log_percent_to_grade(int percent)
{
    int clamped = math_clamp_int(percent, -100, 100);
    return (float)clamped / 100.0f;
}

const char *log_severity_name(log_severity_t severity)
{
    if ((size_t)severity < (size_t)LOG_SEV_COUNT) {
        return s_severity_labels[severity];
    }
    return "UNKNOWN ";
}

const char *log_category_name(log_category_t category)
{
    if ((size_t)category < (size_t)LOG_CAT_COUNT) {
        return s_category_labels[category];
    }
    return "UNKNOWN  ";
}

const char *log_category_dir_name(log_category_t category)
{
    if ((size_t)category < (size_t)LOG_CAT_COUNT) {
        return s_category_dir_names[category];
    }
    return "general";
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Formatted Entry Dispatch Cogs
--------------------------------------------------------------------------------
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
    if (!s_initialized || severity > s_config.min_severity) {
        return 0;
    }

    s_sequence_counter++;
    uint64_t seq = s_sequence_counter;

    log_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.sequence_id = seq;
    entry.monotonic_us = time_get_monotonic_us();
    generate_iso_timestamp(entry.iso_timestamp, sizeof(entry.iso_timestamp));
    entry.severity = severity;
    entry.category = category;

    if (entity != NULL && entity[0] != '\0') {
        str_safe_copy(entry.entity, entity, sizeof(entry.entity));
    } else {
        str_safe_copy(entry.entity, "System", sizeof(entry.entity));
    }

    entry.grade = (grade > 1.0f) ? 1.0f : ((grade < -1.0f) ? -1.0f : grade);
    entry.grade_percent = log_grade_to_percent(entry.grade);
    entry.file = file;
    entry.line = line;
    entry.func = func;

    va_list args;
    va_start(args, fmt);
    vsnprintf(entry.message, sizeof(entry.message), fmt, args);
    va_end(args);

    /* Insert into static circular in-memory ring buffer */
    s_ring_buffer[s_ring_head] = entry;
    s_ring_head = (s_ring_head + 1) % LOG_RING_BUFFER_CAPACITY;
    if (s_ring_count < LOG_RING_BUFFER_CAPACITY) {
        s_ring_count++;
    }

    /* Format machine-greppable fixed-width string */
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
        entry.file ? entry.file : "unknown",
        entry.line,
        entry.func ? entry.func : "unknown",
        entry.message
    );

    if (written < 0) {
        return seq;
    }

    /* Master unified file output */
    if (s_file != NULL) {
        fputs(line_buffer, s_file);
        s_current_bytes += (size_t)written;
        fflush(s_file);
        log_rotate_if_needed();
    }

    /* Category-specific file output */
    if (s_initialized && s_config.save_by_category && (size_t)category < (size_t)LOG_CAT_COUNT) {
        if (s_cat_files[category] == NULL) {
            const char *dir_name = log_category_dir_name(category);
            char cat_dir[256];
            snprintf(cat_dir, sizeof(cat_dir), "%s/%s", s_config.log_dir, dir_name);
            ensure_directory(cat_dir);

            char cat_path[512];
            snprintf(cat_path, sizeof(cat_path), "%s/%s.log", cat_dir, dir_name);
            s_cat_files[category] = fopen(cat_path, "a");

            struct stat st_cat;
            if (s_cat_files[category] != NULL && stat(cat_path, &st_cat) == 0) {
                s_cat_bytes[category] = (size_t)st_cat.st_size;
            } else {
                s_cat_bytes[category] = 0;
            }
        }

        if (s_cat_files[category] != NULL) {
            fputs(line_buffer, s_cat_files[category]);
            s_cat_bytes[category] += (size_t)written;
            fflush(s_cat_files[category]);
            rotate_category_file(category);
        }
    }

    /* Console output with optional ANSI colors */
    if (s_config.echo_to_console) {
        FILE *out = (severity <= LOG_SEV_ERROR) ? stderr : stdout;
        if (s_config.use_colors && (size_t)severity < (size_t)LOG_SEV_COUNT) {
            fprintf(
                out,
                "%s[%s] [ID#%08llu] [%-8s] [CAT:%-9s] [%-10s] [GRADE:%+04d] %s%s\n",
                s_severity_colors[severity],
                entry.iso_timestamp,
                (unsigned long long)entry.sequence_id,
                log_severity_name(entry.severity),
                log_category_name(entry.category),
                entry.entity,
                entry.grade_percent,
                entry.message,
                ANSI_RESET
            );
        } else {
            fputs(line_buffer, out);
        }
    }

    return seq;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Static Ring Buffer & Telemetry Inspection Cogs
--------------------------------------------------------------------------------
*/

size_t log_get_recent_entries(log_entry_t *dest, size_t max_entries)
{
    if (dest == NULL || max_entries == 0 || s_ring_count == 0) {
        return 0;
    }
    size_t count = (max_entries < s_ring_count) ? max_entries : s_ring_count;
    size_t start_idx =
        (s_ring_head + LOG_RING_BUFFER_CAPACITY - count) % LOG_RING_BUFFER_CAPACITY;

    for (size_t i = 0; i < count; i++) {
        size_t idx = (start_idx + i) % LOG_RING_BUFFER_CAPACITY;
        dest[i] = s_ring_buffer[idx];
    }
    return count;
}

size_t log_get_top_entries(log_entry_t *dest, size_t max_entries)
{
    if (dest == NULL || max_entries == 0) {
        return 0;
    }
    size_t count = (max_entries < s_ring_count) ? max_entries : s_ring_count;

    for (size_t i = 0; i < count; i++) {
        size_t idx = (s_ring_head + LOG_RING_BUFFER_CAPACITY - 1 - i) % LOG_RING_BUFFER_CAPACITY;
        dest[i] = s_ring_buffer[idx];
    }
    return count;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwires & Audits
--------------------------------------------------------------------------------
*/

diag_result_t log_preflight(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = "core.util.log";
    res.check_name = "grade_mapping_and_buffers";
    res.details = "Grade mapping, labels, and static ring buffer verified.";

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

    /* Check 2: Audit string labels */
    if (strcmp(log_severity_name(LOG_SEV_FATAL), "FATAL   ") != 0 ||
        strcmp(log_category_name(LOG_CAT_CHARACTER), "CHARACTER") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Telemetry label token length discrepancy.";
        return res;
    }

    /* Check 3: Audit category directory name */
    if (strcmp(log_category_dir_name(LOG_CAT_CHARACTER), "character") != 0 ||
        strcmp(log_category_dir_name(LOG_CAT_EVENT), "event") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Category directory name mapping discrepancy.";
        return res;
    }

    /* Check 4: Audit string-to-enum conversions */
    log_severity_t test_sev;
    if (!log_severity_from_string("TRACE", &test_sev) || test_sev != LOG_SEV_TRACE ||
        !log_severity_from_string("fatal", &test_sev) || test_sev != LOG_SEV_FATAL ||
        !log_severity_from_string("WARNING", &test_sev) || test_sev != LOG_SEV_WARNING ||
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

    /* Check 6: Audit emergency fallback code page invariants */
    if (!log_fallback_audit()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Monolithic emergency fallback code page failed audit.";
        return res;
    }

    return res;
}

diag_result_t log_audit(bool verbose)
{
    if (verbose) {
        printf("--- Running Core Utilities: Telemetry Subsystem Diagnostic Audit ---\n");
    }

    bool was_initialized = s_initialized;
    if (!s_initialized) {
        log_config_t test_cfg = log_get_default_config();
        test_cfg.echo_to_console = false;
        test_cfg.min_severity = LOG_SEV_TRACE;
        log_init(&test_cfg);
    }

    /* Check 1: Preflight self-check */
    diag_result_t pre = log_preflight();
    if (!pre.passed) {
        if (verbose) printf("  [FAIL] log_preflight invariant check failed: %s\n", pre.details);
        if (!was_initialized) log_shutdown();
        return pre;
    }
    if (verbose) printf("  [PASS] Preflight grade and label invariants verified.\n");

    /* Check 2: Percent to grade conversion */
    float g1 = log_percent_to_grade(100);
    float g2 = log_percent_to_grade(-100);
    float g3 = log_percent_to_grade(0);
    if (g1 < 0.99f || g1 > 1.01f || g2 < -1.01f || g2 > -0.99f || g3 != 0.0f) {
        diag_result_t err = { false, SEVERITY_CRITICAL, "core.util.log", "percent_conversion", "Precision failed" };
        if (verbose) printf("  [FAIL] Percent to grade conversion failed.\n");
        if (!was_initialized) log_shutdown();
        return err;
    }
    if (verbose) printf("  [PASS] Percent to grade precision verified.\n");

    /* Check 3: Monotonic counter test */
    uint64_t before = log_get_total_entries();
    LOG_INFO(LOG_CAT_DEBUG, "AuditTester", 0.0f, "Audit diagnostic message %d", 42);
    uint64_t after = log_get_total_entries();
    if (after != before + 1) {
        diag_result_t err = { false, SEVERITY_CRITICAL, "core.util.log", "sequence_monotonicity", "Counter failed" };
        if (verbose) printf("  [FAIL] Sequence counter monotonicity violated.\n");
        if (!was_initialized) log_shutdown();
        return err;
    }
    if (verbose) printf("  [PASS] Sequence counter monotonicity verified (%lu -> %lu).\n",
                        (unsigned long)before, (unsigned long)after);

    /* Check 4: Ring buffer entry retrieval */
    log_entry_t entries[4];
    size_t fetched = log_get_recent_entries(entries, 4);
    if (fetched == 0 || entries[fetched - 1].sequence_id != after) {
        diag_result_t err = { false, SEVERITY_CRITICAL, "core.util.log", "ring_buffer", "Retrieval failed" };
        if (verbose) printf("  [FAIL] Ring buffer entry retrieval failed.\n");
        if (!was_initialized) log_shutdown();
        return err;
    }
    if (verbose) printf("  [PASS] Ring buffer retrieved %zu recent entries.\n", fetched);

    /* Check 5: Top 10-25 entries retrieval (newest first) */
    log_entry_t top_entries[25];
    size_t top_fetched = log_get_top_entries(top_entries, 25);
    if (top_fetched == 0 || top_entries[0].sequence_id != after) {
        diag_result_t err = { false, SEVERITY_CRITICAL, "core.util.log", "top_entries", "Top retrieval failed" };
        if (verbose) printf("  [FAIL] Top entries retrieval failed.\n");
        if (!was_initialized) log_shutdown();
        return err;
    }
    if (verbose) printf("  [PASS] Top entries retrieved %zu entries (newest ID#%08llu).\n",
                        top_fetched, (unsigned long long)top_entries[0].sequence_id);

    /* Check 6: Data-driven configuration loading verification */
    log_config_t manifest_cfg;
    if (!log_load_config_from_manifest(NULL, &manifest_cfg) || !manifest_cfg.loaded_from_data) {
        diag_result_t err = { false, SEVERITY_CRITICAL, "core.util.log", "data_manifest", "Data-driven manifest loading failed" };
        if (verbose) printf("  [FAIL] Data-driven manifest loading failed.\n");
        if (!was_initialized) log_shutdown();
        return err;
    }
    if (verbose) {
        printf("  [PASS] Data-driven configuration loaded from manifest (Profile: %s, File: %s, Max: %zu bytes).\n",
               manifest_cfg.profile_name, manifest_cfg.base_filename, manifest_cfg.max_file_size_bytes);
    }

    /* Check 7: Specific named profile loading verification */
    log_config_t quiet_cfg;
    if (log_load_config_profile("quiet", &quiet_cfg)) {
        if (quiet_cfg.min_severity != LOG_SEV_WARNING || quiet_cfg.echo_to_console != false) {
            diag_result_t err = { false, SEVERITY_WARNING, "core.util.log", "profile_quiet", "Quiet profile attributes mismatch" };
            if (verbose) printf("  [WARN] Quiet profile attributes mismatch.\n");
            if (!was_initialized) log_shutdown();
            return err;
        }
        if (verbose) {
            printf("  [PASS] Profile 'quiet' successfully loaded and verified from data sources.\n");
        }
    }

    /* Check 8: Monolithic emergency fallback audit */
    if (!log_fallback_audit()) {
        diag_result_t err = { false, SEVERITY_CRITICAL, "core.util.log", "fallback_monolith", "Emergency fallback code page failed audit" };
        if (verbose) printf("  [FAIL] Monolithic fallback code page failed audit.\n");
        if (!was_initialized) log_shutdown();
        return err;
    }
    if (verbose) {
        printf("  [PASS] Emergency monolithic fallback code page audit verified.\n");
    }

    if (!was_initialized) {
        log_shutdown();
    }

    diag_result_t ok = { true, SEVERITY_PASS, "core.util.log", "full_audit", "All audit checks passed" };
    return ok;
}

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

bool log_execute_inspect(log_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }
    memset(out_stats, 0, sizeof(*out_stats));
    out_stats->total_entries = s_sequence_counter;
    out_stats->ring_buffer_count = s_ring_count;
    out_stats->rotation_count = s_rotation_count;
    out_stats->active_file_bytes = s_current_bytes;
    out_stats->min_severity = s_config.min_severity;
    out_stats->is_active = s_initialized;
    out_stats->loaded_from_data = s_config.loaded_from_data;
    str_safe_copy(
        out_stats->active_profile,
        log_get_active_profile_name(),
        sizeof(out_stats->active_profile)
    );
    return true;
}

bool log_execute_dump_card(const log_stats_t *stats, char *out_buf, size_t buf_size)
{
    if (out_buf == NULL || buf_size < 320) {
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
        "| Total Entries  : %-41llu |\n"
        "| Ring Count     : %-41zu |\n"
        "| Rotation Count : %-41zu |\n"
        "| File Bytes     : %-41zu |\n"
        "+------------------------------------------------------------+",
        stats->active_profile,
        stats->is_active ? "TRUE (Online)" : "FALSE (Offline)",
        stats->loaded_from_data ? "TRUE (Data-Driven)" : "FALSE (Procedural/Fallback)",
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
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

bool log_init(const log_config_t *config)
{
    if (s_initialized) {
        log_shutdown();
    }

    if (config != NULL) {
        s_config = *config;
    } else {
        s_config = log_get_default_config();
    }

    if (!s_config.loaded_from_data && !str_case_eq(s_config.profile_name, LOG_FALLBACK_PROFILE_NAME)) {
        if (s_tripwire_status == LOG_TRIPWIRE_CLEAR) {
            set_tripwire(
                LOG_TRIPWIRE_MANIFEST_MISSING,
                "Refusing boot: configuration not loaded from data-driven pipeline."
            );
        }
        return false;
    }

    if (s_config.log_dir[0] == '\0' || s_config.base_filename[0] == '\0') {
        set_tripwire(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Refusing boot: log_dir or base_filename is empty."
        );
        return false;
    }

    if (s_config.max_rotated_files > 25 || s_config.max_file_size_bytes == 0) {
        set_tripwire(
            LOG_TRIPWIRE_BOUNDS_VIOLATION,
            "Refusing boot: max_rotated_files > 25 or max_file_size_bytes == 0."
        );
        return false;
    }

    /* Ephemeral sandboxing resolution: route into build/logs if build/ exists */
    if (strcmp(s_config.log_dir, "logs") == 0) {
        struct stat st_build;
        if (stat("build", &st_build) == 0 && S_ISDIR(st_build.st_mode)) {
            str_safe_copy(s_config.log_dir, "build/logs", sizeof(s_config.log_dir));
        }
    } else if (strncmp(s_config.log_dir, "build/logs", 10) == 0) {
        struct stat st_build;
        if (stat("build", &st_build) != 0 || !S_ISDIR(st_build.st_mode)) {
            if (fs_file_exists("CTestTestfile.cmake") || fs_file_exists("CMakeCache.txt")) {
                str_safe_copy(s_config.log_dir, "logs", sizeof(s_config.log_dir));
            }
        }
    }

    ensure_directory(s_config.log_dir);

    char full_path[512];
    get_log_filepath(full_path, sizeof(full_path), s_config.log_dir, s_config.base_filename);

    s_file = fopen(full_path, "a");
    if (s_file == NULL) {
        return false;
    }

    struct stat st;
    if (stat(full_path, &st) == 0) {
        s_current_bytes = (size_t)st.st_size;
    } else {
        s_current_bytes = 0;
    }

    for (size_t i = 0; i < (size_t)LOG_CAT_COUNT; i++) {
        s_cat_files[i] = NULL;
        s_cat_bytes[i] = 0;
    }

    s_ring_head = 0;
    s_ring_count = 0;
    s_initialized = true;

    char iso_ts[32];
    generate_iso_timestamp(iso_ts, sizeof(iso_ts));
    fprintf(
        s_file,
        "================================================================================\n"
        "[%s] [SYSTEM INIT] Tenebris and Lux Forensic Telemetry Subsystem Initialized\n"
        "Standard: ISO 8601 | Profile: %s | Source: %s\n"
        "================================================================================\n",
        iso_ts,
        s_config.profile_name[0] != '\0' ? s_config.profile_name : "custom",
        s_config.loaded_from_data ? "DATA-DRIVEN (system/diagnostics/telemetry)" : "HARDCODED (fallback)"
    );
    fflush(s_file);

    return true;
}

void log_shutdown(void)
{
    if (!s_initialized) {
        return;
    }

    if (s_file != NULL) {
        char iso_ts[32];
        generate_iso_timestamp(iso_ts, sizeof(iso_ts));
        fprintf(
            s_file,
            "================================================================================\n"
            "[%s] [SYSTEM SHUTDOWN] Telemetry Subsystem Shutdown. Total events: %lu\n"
            "================================================================================\n",
            iso_ts,
            (unsigned long)s_sequence_counter
        );
        fflush(s_file);
        fclose(s_file);
        s_file = NULL;
    }

    for (size_t i = 0; i < (size_t)LOG_CAT_COUNT; i++) {
        if (s_cat_files[i] != NULL) {
            fflush(s_cat_files[i]);
            fclose(s_cat_files[i]);
            s_cat_files[i] = NULL;
        }
    }

    s_initialized = false;
}
