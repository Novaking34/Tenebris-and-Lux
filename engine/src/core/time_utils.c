/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          time_utils.c
@id            core.util.time
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        High-Resolution Timing Subsystem (core/time)
@role          Monotonic Clocks, Delta Timing, Frame Pacing & ISO Timestamps
@invariants    1. Monotonic precision: CLOCK_MONOTONIC queries never step backwards (t2 >= t1).
               2. Non-negative sleep clamp: Negative sleep requests safely clamped to zero.
               3. ISO 8601 UTC timestamp format: YYYY-MM-DDTHH:MM:SSZ (21 bytes null-terminated).
               4. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: stack/static only).

PURPOSE & ARCHITECTURAL ROLE:
  High-resolution monotonic timer queries, microsecond delta profiling, frame sleep
  pacing, and ISO 8601 calendar timestamp formatting. Ingests canonical frame budgets
  and latency limits from game/data/system/runtime/time/ via master_loader.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

#define _POSIX_C_SOURCE 199309L

/* --- Tier 1: Standard Library Headers --- */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

/* --- Tier 2: Core Engine Specifications & Subsystems --- */
#include "core/time_utils.h"
#include "core/time_utils_fallback.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"

static const char *const time_invariants[] = {
    "Monotonicity: t2 >= t1 strictly enforced across queries",
    "POSIX CLOCK_MONOTONIC high-resolution timing source",
    "Non-negative sleep duration safety clamping",
    "ISO 8601 UTC calendar timestamp formatting bounds"
};

const module_metadata_t time_utils_manifest = {
    "core.util.time",
    "time_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    4,                          /* interface_rev */
    "2026-11-07",               /* last_updated */
    "High-resolution monotonic timer, delta profiling, frame sleep, and ISO timestamps",
    0,                          /* dependency_count: leaf foundational cog */
    { NULL },                   /* dependencies */
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    5,                          /* max_budget_us */
    4,
    {
        time_invariants[0],
        time_invariants[1],
        time_invariants[2],
        time_invariants[3]
    },
    time_utils_preflight
};

/*
================================================================================
BLOCK 3: BODY / CORE TIMING LOGIC BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

double time_get_monotonic_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        debug_trap_tripwire("time", "clock_gettime(CLOCK_MONOTONIC) failed", __FILE__, __LINE__);
        return 0.0;
    }
    return ((double)ts.tv_sec * 1000.0) + ((double)ts.tv_nsec / 1000000.0);
}

double time_get_monotonic_us(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        debug_trap_tripwire("time", "clock_gettime(CLOCK_MONOTONIC) failed", __FILE__, __LINE__);
        return 0.0;
    }
    return ((double)ts.tv_sec * 1000000.0) + ((double)ts.tv_nsec / 1000.0);
}

void time_sleep_ms(double ms)
{
    if (ms < 0.0) {
        debug_trap_tripwire("time", "Negative duration passed to time_sleep_ms", __FILE__, __LINE__);
        return;
    }

    if (ms <= 0.0) {
        return;
    }

    struct timespec req;
    req.tv_sec = (time_t)(ms / 1000.0);
    req.tv_nsec = (long)((ms - (double)(req.tv_sec * 1000.0)) * 1000000.0);

    if (req.tv_nsec >= 1000000000L) {
        req.tv_sec += 1;
        req.tv_nsec -= 1000000000L;
    }

    nanosleep(&req, NULL);
}

double time_get_delta_ms(double start_ms, double end_ms)
{
    if (end_ms < start_ms) {
        debug_trap_tripwire("time", "end_ms < start_ms in time_get_delta_ms", __FILE__, __LINE__);
        return 0.0;
    }
    return end_ms - start_ms;
}

double time_get_delta_us(double start_us, double end_us)
{
    if (end_us < start_us) {
        debug_trap_tripwire("time", "end_us < start_us in time_get_delta_us", __FILE__, __LINE__);
        return 0.0;
    }
    return end_us - start_us;
}

bool time_is_elapsed_ms(double start_ms, double interval_ms)
{
    if (interval_ms <= 0.0) return true;
    double now = time_get_monotonic_ms();
    return (now >= start_ms) && ((now - start_ms) >= interval_ms);
}

bool time_get_iso_timestamp(char *buf, size_t buf_size)
{
    if (buf == NULL || buf_size < 21) {
        debug_trap_tripwire("time", "Insufficient buffer capacity for ISO timestamp", __FILE__, __LINE__);
        return false;
    }

    time_t raw_time = time(NULL);
    struct tm utc_tm;

    if (gmtime_r(&raw_time, &utc_tm) == NULL) {
        debug_trap_tripwire("time", "gmtime_r failed", __FILE__, __LINE__);
        return false;
    }

    int written = snprintf(
        buf, buf_size, "%04d-%02d-%02dT%02d:%02d:%02dZ",
        utc_tm.tm_year + 1900, utc_tm.tm_mon + 1, utc_tm.tm_mday,
        utc_tm.tm_hour, utc_tm.tm_min, utc_tm.tm_sec);

    return (written > 0 && (size_t)written < buf_size);
}

time_config_t time_get_default_config(void)
{
    time_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    str_safe_copy(cfg.id, "default", sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Canonical 60 FPS Pacing Baseline", sizeof(cfg.display_name));
    cfg.target_fps = 60;
    cfg.frame_budget_ms = 16.666667;
    cfg.turn_latency_budget_us = 50.0;
    cfg.preflight_latency_budget_us = 5000.0;
    cfg.disk_query_budget_us = 100.0;
    cfg.token_extraction_budget_us = 20.0;
    cfg.tick_rate_hz = 60;
    cfg.loaded_from_data = false;

    return cfg;
}

bool time_load_config_from_file(const char *profile_path, time_config_t *out_config)
{
    if (out_config == NULL) {
        debug_trap_tripwire("time", "NULL out_config passed to time_load_config_from_file", __FILE__, __LINE__);
        return false;
    }

    *out_config = time_get_default_config();

    const char *target_rel = profile_path;
    if (target_rel == NULL || target_rel[0] == '\0') {
        target_rel = "runtime/time/default.json";
    }

    if (strncmp(target_rel, "system/", 7) == 0) {
        target_rel += 7;
    }

    master_loader_t loader;
    char file_buf[8192];
    if (!master_loader_load(&loader, "system", target_rel, file_buf, sizeof(file_buf))) {
        LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_LOG, "Time", -0.2f,
                   "Failed to load time profile '%s', falling back to defaults (GRADE:-020)", target_rel);
        return false;
    }

    master_loader_get_string(&loader, "profile", "id", out_config->id, sizeof(out_config->id));
    master_loader_get_string(&loader, "profile", "display_name", out_config->display_name, sizeof(out_config->display_name));
    master_loader_get_int(&loader, "profile", "target_fps", &out_config->target_fps);

    float f_budget = 0.0f;
    if (master_loader_get_float(&loader, "profile", "frame_budget_ms", &f_budget)) {
        out_config->frame_budget_ms = (double)f_budget;
    }
    if (master_loader_get_float(&loader, "profile", "turn_latency_budget_us", &f_budget)) {
        out_config->turn_latency_budget_us = (double)f_budget;
    }
    if (master_loader_get_float(&loader, "profile", "preflight_latency_budget_us", &f_budget)) {
        out_config->preflight_latency_budget_us = (double)f_budget;
    }
    if (master_loader_get_float(&loader, "profile", "disk_query_budget_us", &f_budget)) {
        out_config->disk_query_budget_us = (double)f_budget;
    }
    if (master_loader_get_float(&loader, "profile", "token_extraction_budget_us", &f_budget)) {
        out_config->token_extraction_budget_us = (double)f_budget;
    }

    master_loader_get_int(&loader, "profile", "tick_rate_hz", &out_config->tick_rate_hz);
    out_config->loaded_from_data = true;

    LOG_RECORD(LOG_SEV_DEBUG, LOG_CAT_LOG, "Time", 0.0f,
               "Hydrated time profile '%s' (FPS: %d, Frame: %.2f ms) (GRADE:+000)",
               out_config->id, out_config->target_fps, out_config->frame_budget_ms);
    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: High-Resolution Timing Engine
================================================================================
*/

diag_result_t time_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = time_utils_manifest.module_id;
    res.check_name = "Monotonicity, Resolution & Calendar Timestamps";

    if (!time_utils_audit()) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: time_utils failed invariant audit.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Monotonic clocks, delta timers & ISO timestamps verified.";
    return res;
}

bool time_utils_audit(void)
{
    /* Test 1: Monotonicity across consecutive queries */
    double prev = time_get_monotonic_us();
    for (int i = 0; i < 100; i++) {
        double curr = time_get_monotonic_us();
        if (curr < prev) {
            return false;
        }
        prev = curr;
    }

    /* Test 2: ISO 8601 formatting length and anchor characters */
    char ts_buf[32];
    if (!time_get_iso_timestamp(ts_buf, sizeof(ts_buf))) {
        return false;
    }
    if (strlen(ts_buf) != 20 || ts_buf[10] != 'T' || ts_buf[19] != 'Z') {
        return false;
    }

    /* Test 3: Insufficient buffer capacity rejects safely */
    char small_buf[10];
    if (time_get_iso_timestamp(small_buf, sizeof(small_buf))) {
        return false;
    }

    /* Test 4: Delta timing calculation */
    double d1 = time_get_delta_ms(100.0, 150.0);
    if (d1 < 49.99 || d1 > 50.01) return false;
    if (time_get_delta_ms(150.0, 100.0) != 0.0) return false; /* Reversed timestamps safely clamped */

    /* Test 5: Default configuration bounds */
    time_config_t def = time_get_default_config();
    if (def.target_fps != 60 || def.frame_budget_ms < 16.0 || def.frame_budget_ms > 17.0) {
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING & DIAGNOSTIC CARD DUMP BLOCK
================================================================================
*/

bool time_execute_dump_card(const time_config_t *config, char *out_buf, size_t buf_size)
{
    if (config == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|               TIME PACING INSPECTION CARD                |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Target Framerate   : %2d FPS                                 |\n"
        "| Frame Budget       : %6.2f ms                             |\n"
        "| Turn Latency Limit : %6.2f us                             |\n"
        "| Simulation Rate    : %2d Hz                                  |\n"
        "| Data Hydration     : %-36s|\n"
        "'----------------------------------------------------------'\n",
        config->id, config->target_fps, config->frame_budget_ms,
        config->turn_latency_budget_us, config->tick_rate_hz,
        config->loaded_from_data ? "ACTIVE (game/data/system/runtime)" : "FALLBACK (Static Hardcoded)");

    return (written > 0 && (size_t)written < buf_size);
}

/* End of time_utils.c */
