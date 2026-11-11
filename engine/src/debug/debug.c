/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          debug.c
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap Engine Debugger & Validation Gatekeeper
@role          Snapshot Ring Buffer, FNV-1a Determinism, Validation Probes & AWAIT Engine
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Data-driven resolution via diagnostics/debugger/ & taxonomy/
                  manifests with automatic escalation to debug_fallback ROM/BIOS.
               4. Fail-test / tripwire workflow: Fire validation probe -> downgrade
                  to Safe-Mode fallback to finish interrupted state -> pin locked
                  forensic snapshot -> sit idle in DEBUG_STEP_AWAIT until addressed
                  and valid state restored via debug_resolve_await().
               5. Non-destructive self-audit: debug_audit() preserves active ring,
                  breakpoints, validation probes, and telemetry provenance.
               6. Zero double-slash comments (ISO C-style comments exclusively).

PURPOSE & ARCHITECTURAL ROLE:
  Implements the Layer 0 Zero-Heap Engine Debugger, time-travel snapshot ring
  with locked critical-slot retention (snapshot_policy.json), FNV-1a state hash
  determinism, conditional breakpoints, registered validation probes, and the
  tripwire -> Safe-Mode fallback -> DEBUG_STEP_AWAIT recovery state machine.

  ### For Game Designers & Non-Programmers:
  Think of this file as the engine's flight-control safety console and time
  machine. It remembers up to 64 combat turns, pins critical emergency moments
  so they are never overwritten, lets you rewind to fix bad numbers, and
  automatically freezes gameplay in AWAIT whenever a safety test catches a bug.

  ### For Systems Programmers & Engine Architects:
  - Body Block Sub-Assembly parity with log.c:
    * 3.1 Deterministic FNV-1a State Hashing
    * 3.2 Data-Driven Config, Snapshot Policy & Tripwire Matrix Resolution
    * 3.3 Queries, Step Modes & DEBUG_STEP_AWAIT Recovery Controller
    * 3.4 Circular Snapshot Ring with Locked-Slot Quota, Rewind & Safe Diff
    * 3.5 Breakpoints & Domain Guardrail Watchpoints
    * 3.6 Validation Probe Registry, Tripwire Evaluation & Fallback Downgrade
    * 3.7 Console & Telemetry Readouts
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug
================================================================================
Mission: 3-tier downward include ladder, internal state ontology, static .bss
allocation, manifest provenance, and pure reentrant inline helpers.
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
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/math_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"
#include "io/master_loader.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Bounds Constants
--------------------------------------------------------------------------------
*/

#define MODULE_ID                "core.util.debug"
#define LOCAL_LOG_CAT            LOG_CAT_DEBUG
#define LOCAL_MODULE_TAG         "Debugger"
#define FNV1A_OFFSET_BASIS_32    2166136261u
#define FNV1A_PRIME_32           16777619u
#define DEBUG_SNAPSHOT_HIGH_PCT  85U

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/**
 * @struct debug_internal_state
 * @brief Complete zero-heap runtime state for the Layer 0 Debugger Engine.
 */
typedef struct debug_internal_state {
    debug_config_t                 config;
    debug_snapshot_t               snapshots[DEBUG_MAX_SNAPSHOTS];
    size_t                         snapshot_count;
    size_t                         snapshot_head;
    uint64_t                       next_snapshot_id;
    size_t                         locked_count;
    debug_breakpoint_t             breakpoints[DEBUG_MAX_BREAKPOINTS];
    size_t                         breakpoint_count;
    int                            next_breakpoint_id;
    debug_validation_probe_t       probes[DEBUG_MAX_VALIDATION_PROBES];
    size_t                         probe_count;
    debug_fallback_tripwire_rule_t rules[DEBUG_FALLBACK_MAX_TRIPWIRE_RULES];
    size_t                         rule_count;
    debug_step_mode_t              step_mode;
    debug_tripwire_t               tripwire_status;
    char                           tripwire_details[DEBUG_MAX_MSG_LEN];
    debug_tripwire_report_t        last_report;
    uint32_t                       traps_triggered;
    uint32_t                       probe_failures;
    size_t                         snapshot_watermark;
    uint64_t                       total_snapshots_captured;
    bool                           safe_mode;
    bool                           await_active;
    bool                           fallback_completed;
    bool                           in_probe;
    bool                           in_audit;
} debug_internal_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Module Manifest (.rodata) & Static State Storage (.bss)
--------------------------------------------------------------------------------
*/

const module_metadata_t debug_manifest = {
    MODULE_ID,
    "debug/debug.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,                          /* interface_rev Rev 5 */
    "2026-11-11",               /* last_updated */
    "Zero-heap engine debugger, snapshot ring buffer, probes & AWAIT recovery",
    3,                          /* dependency_count */
    {
        "core.util.log",
        "core.io.master_loader",
        "core.util.fs"
    },
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    500,                        /* max_budget_us */
    4,                          /* invariant_count */
    {
        "ALLOC_ZERO_HEAP: Fixed static ring buffer of max 64 snapshots",
        "Deterministic FNV-1a state hashing for drift and corruption detection",
        "Strict Layer 0 decoupling via primitive debug_state_fields_t snapshots",
        "Fail-tests downgrade cleanly to Safe-Mode fallback then hold DEBUG_STEP_AWAIT"
    },
    debug_preflight
};

static debug_internal_state_t s_debug;
static debug_internal_state_t s_audit_backup;
static bool s_debug_active = false;
static bool s_debug_data_driven = false;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Internal Ring Helpers
--------------------------------------------------------------------------------
Single-purpose typed helpers replacing macros and duplicating bounds guards.
--------------------------------------------------------------------------------
*/

/**
 * @brief Mixes a signed 32-bit integer into a running FNV-1a 32-bit hash.
 */
static inline uint32_t fnv1a_mix_int(uint32_t hash, int val)
{
    const uint8_t *bytes = (const uint8_t *)&val;
    for (size_t b = 0U; b < sizeof(int); b++) {
        hash ^= bytes[b];
        hash *= FNV1A_PRIME_32;
    }
    return hash;
}

/**
 * @brief Mixes a null-terminated C string into a running FNV-1a 32-bit hash.
 */
static inline uint32_t fnv1a_mix_cstr(uint32_t hash, const char *str)
{
    if (str == NULL) {
        return hash;
    }
    while (*str != '\0') {
        hash ^= (uint8_t)(*str);
        hash *= FNV1A_PRIME_32;
        str++;
    }
    return hash;
}

/**
 * @brief Bounded sprintf appender preventing signed offset wrap or overrun.
 */
static void dbg_append_fmt(char *buf, size_t cap, size_t *io_offset, const char *fmt, ...)
{
    if (buf == NULL || io_offset == NULL || cap == 0U || *io_offset >= cap - 1U) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(buf + *io_offset, cap - *io_offset, fmt, args);
    va_end(args);

    if (n > 0) {
        size_t adv = (size_t)n;
        if (*io_offset + adv >= cap) {
            *io_offset = cap - 1U;
        } else {
            *io_offset += adv;
        }
    }
}

/**
 * @brief Verifies domain bounds of restored hero/world fields before exiting AWAIT.
 */
static bool dbg_state_fields_are_sane(const debug_state_fields_t *fields)
{
    if (fields == NULL) {
        return true;
    }
    if (fields->max_hp <= 0 || fields->hp < 0 || fields->hp > fields->max_hp) {
        return false;
    }
    if (fields->moral_sway < -100 || fields->moral_sway > 100) {
        return false;
    }
    if (fields->shield_power < 0 || fields->shield_power > 100 ||
        fields->sword_edge < 0 || fields->sword_edge > 100) {
        return false;
    }
    return true;
}

/**
 * @brief Tracks high-watermark ring buffer usage and logs high-pressure thresholds.
 */
static void dbg_update_snapshot_watermark(size_t current_count)
{
    if (current_count > s_debug.snapshot_watermark) {
        s_debug.snapshot_watermark = current_count;
        if (s_debug.config.max_snapshots > 0U &&
            ((current_count * 100U) / s_debug.config.max_snapshots) >= DEBUG_SNAPSHOT_HIGH_PCT &&
            !s_debug.in_audit) {
            LOG_DEBUG(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.0f,
                      "Snapshot watermark advanced (%zu / %zu slots filled)",
                      current_count, s_debug.config.max_snapshots);
        }
    }
}

/**
 * @brief Linearizes a wrapped circular ring so index 0 matches physical slot 0.
 */
static void dbg_normalize_ring_for_locks(void)
{
    size_t cap = s_debug.config.max_snapshots;
    if (cap == 0U || s_debug.snapshot_count < cap || s_debug.snapshot_head == 0U) {
        return;
    }

    debug_snapshot_t ordered[DEBUG_MAX_SNAPSHOTS];
    for (size_t i = 0U; i < cap; i++) {
        size_t phys = (s_debug.snapshot_head + i) % cap;
        ordered[i] = s_debug.snapshots[phys];
    }

    memcpy(s_debug.snapshots, ordered, cap * sizeof(debug_snapshot_t));
    s_debug.snapshot_head = 0U;
}

/**
 * @brief Releases the oldest locked slot when locked_count reaches locked_slot_quota.
 */
static void dbg_unlock_oldest_locked(void)
{
    size_t cap = s_debug.config.max_snapshots;
    for (size_t i = 0U; i < s_debug.snapshot_count; i++) {
        size_t phys = (s_debug.snapshot_count < cap)
                          ? i
                          : ((s_debug.snapshot_head + i) % cap);
        if (s_debug.snapshots[phys].locked) {
            s_debug.snapshots[phys].locked = false;
            str_safe_copy(s_debug.snapshots[phys].lock_reason, "",
                          sizeof(s_debug.snapshots[phys].lock_reason));
            if (s_debug.locked_count > 0U) {
                s_debug.locked_count--;
            }
            return;
        }
    }
}

/**
 * @brief Resolves physical ring index for chronological slot 0..snapshot_count-1.
 */
static size_t dbg_logical_to_physical(size_t logical_idx)
{
    size_t cap = s_debug.config.max_snapshots;
    if (cap == 0U) {
        return 0U;
    }
    if (s_debug.snapshot_count < cap) {
        return logical_idx;
    }
    return (s_debug.snapshot_head + logical_idx) % cap;
}

/**
 * @brief Populates canonical default reaction rule fallback by tripwire code.
 */
static debug_tripwire_reaction_t dbg_default_reaction_for_wire(debug_tripwire_t wire)
{
    switch (wire) {
        case DEBUG_TRIPWIRE_STATE_CORRUPT:
        case DEBUG_TRIPWIRE_HASH_MISMATCH:
            return DEBUG_REACTION_AUTO_REWIND;
        case DEBUG_TRIPWIRE_TEST_FAILED:
        case DEBUG_TRIPWIRE_AUDIT_BREACH:
            return DEBUG_REACTION_PANIC_HALT;
        case DEBUG_TRIPWIRE_MANIFEST_MISSING:
        case DEBUG_TRIPWIRE_SETTING_MISSING:
        case DEBUG_TRIPWIRE_FALLBACK_ENGAGED:
            return DEBUG_REACTION_FALLBACK_MODE;
        case DEBUG_TRIPWIRE_CLEAR:
        default:
            return DEBUG_REACTION_LOG_AND_CONTINUE;
    }
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug
================================================================================
Mission: Execute deterministic state hashing, layered config/policy/matrix
hydration, snapshot time-travel, breakpoints, and fail-test -> fallback -> AWAIT.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Deterministic Hashing & Configuration Provenance Cogs
--------------------------------------------------------------------------------
*/

uint32_t debug_compute_state_hash(const debug_state_fields_t *fields)
{
    /* Step 1: Precondition check on fields pointer */
    if (fields == NULL) {
        return 0U;
    }

    /* Step 2: Initialize canonical 32-bit FNV-1a digest */
    uint32_t hash = FNV1A_OFFSET_BASIS_32;

    /* Step 3: Mix simulation state fields cleanly in canonical field order */
    hash = fnv1a_mix_int(hash, fields->turn);
    hash = fnv1a_mix_cstr(hash, fields->hero_name);
    hash = fnv1a_mix_int(hash, fields->hp);
    hash = fnv1a_mix_int(hash, fields->max_hp);
    hash = fnv1a_mix_int(hash, fields->mp);
    hash = fnv1a_mix_int(hash, fields->tp);
    hash = fnv1a_mix_int(hash, fields->shield_power);
    hash = fnv1a_mix_int(hash, fields->sword_edge);
    hash = fnv1a_mix_int(hash, fields->stance);
    hash = fnv1a_mix_int(hash, fields->moral_sway);
    hash = fnv1a_mix_int(hash, fields->location);
    hash = fnv1a_mix_int(hash, fields->lux_barrio_turns);
    hash = fnv1a_mix_int(hash, fields->charm_turns);
    hash = fnv1a_mix_int(hash, fields->threat_hp);
    hash = fnv1a_mix_int(hash, fields->foes_vanquished);

    return hash;
}

bool debug_load_snapshot_policy(debug_config_t *cfg)
{
    if (cfg == NULL) {
        return false;
    }

    master_loader_t loader;
    char json_buffer[FS_BUFFER_MEDIUM_BYTES];
    if (master_loader_load(&loader, "config", DEBUG_SNAPSHOT_POLICY_PATH,
                           json_buffer, sizeof(json_buffer)) &&
        master_loader_validate_schema(&loader, "tenlux.data.snapshot_policy.v1") &&
        loader.body_start != NULL) {
        char err_msg[128];
        if (debug_fallback_parse_snapshot_policy_json(
                loader.body_start, cfg, sizeof(*cfg), err_msg, sizeof(err_msg))) {
            return true;
        }
    }

    (void)debug_fallback_load_rom_snapshot_policy(cfg, sizeof(*cfg));
    return false;
}

bool debug_load_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
) {
    if (out_rules == NULL || max_rules == 0U || out_count == NULL) {
        return false;
    }

    *out_count = 0U;

    master_loader_t loader;
    char json_buffer[FS_BUFFER_LARGE_BYTES];
    if (master_loader_load(&loader, "config", DEBUG_TRIPWIRE_MATRIX_PATH,
                           json_buffer, sizeof(json_buffer)) &&
        master_loader_validate_schema(&loader, "tenlux.data.tripwire_matrix.v1") &&
        loader.body_start != NULL) {
        char err_msg[128];
        if (debug_fallback_parse_tripwire_matrix_json(
                loader.body_start, out_rules, max_rules, out_count, err_msg, sizeof(err_msg))) {
            return true;
        }
    }

    (void)debug_fallback_load_rom_tripwire_matrix(out_rules, max_rules, out_count);
    return false;
}

bool debug_load_config_profile(const char *profile_name, debug_config_t *out_config)
{
    /* Step 1: Validate caller pointers */
    if (profile_name == NULL || out_config == NULL) {
        return false;
    }

    /* Step 2: Construct relative VFS path for profile JSON */
    char rel_path[128];
    snprintf(rel_path, sizeof(rel_path), "diagnostics/debugger/%s.json", profile_name);

    /* Step 3: Load via master_loader VFS; degrade cleanly on missing setting */
    master_loader_t loader;
    char json_buffer[FS_BUFFER_MEDIUM_BYTES];
    if (!master_loader_load(&loader, "config", rel_path, json_buffer, sizeof(json_buffer))) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_SETTING_MISSING;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug setting profile missing: %.64s", profile_name);
        (void)debug_fallback_record_fault(
            MODULE_ID, s_debug.tripwire_details, __FILE__, __LINE__,
            "test_debug_integrated:section2"
        );
        (void)debug_fallback_finish_with_fallback(
            MODULE_ID, "Engaged monolith_fallback after missing profile lookup"
        );
        if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                     "Missing debug profile '%s'; using monolith fallback.", profile_name);
        }
        *out_config = debug_get_monolith_fallback_config();
        return true;
    }

    /* Step 4: Validate canonical schema contract */
    if (!master_loader_validate_schema(&loader, "tenlux.data.debug_profile.v1") ||
        loader.body_start == NULL) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_STATE_CORRUPT;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug setting schema invalid for profile: %.64s", profile_name);
        (void)debug_fallback_record_fault(
            MODULE_ID, s_debug.tripwire_details, __FILE__, __LINE__,
            "test_debug_integrated:section2"
        );
        *out_config = debug_get_monolith_fallback_config();
        return false;
    }

    /* Step 5: Hydrate struct via shared JSON parser in debug_fallback */
    debug_tripwire_t tw = DEBUG_TRIPWIRE_CLEAR;
    char err_msg[128];
    if (!debug_fallback_parse_config_json(
            loader.body_start,
            profile_name,
            true,
            out_config,
            sizeof(*out_config),
            &tw,
            err_msg,
            sizeof(err_msg))) {
        s_debug.tripwire_status = (tw != DEBUG_TRIPWIRE_CLEAR) ? tw : DEBUG_TRIPWIRE_STATE_CORRUPT;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug setting bounds error (%s): %.96s", profile_name, err_msg);
        (void)debug_fallback_record_fault(
            MODULE_ID, s_debug.tripwire_details, __FILE__, __LINE__,
            "test_debug_integrated:section2"
        );
        *out_config = debug_get_monolith_fallback_config();
        return false;
    }

    /* Step 6: Overlay snapshot_policy quotas without mutating profile provenance */
    (void)debug_load_snapshot_policy(out_config);
    str_safe_copy(out_config->profile_name, profile_name, sizeof(out_config->profile_name));
    out_config->loaded_from_data = true;

    return true;
}

bool debug_load_config_from_manifest(const char *manifest_path, debug_config_t *out_config)
{
    if (out_config == NULL) {
        return false;
    }

    const char *target = (manifest_path != NULL && manifest_path[0] != '\0')
                             ? manifest_path
                             : DEBUG_MANIFEST_PATH;

    master_loader_t loader;
    char json_buffer[FS_BUFFER_MEDIUM_BYTES];
    if (!master_loader_load(&loader, "config", target, json_buffer, sizeof(json_buffer))) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_MANIFEST_MISSING;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug manifest missing: %.64s", target);
        (void)debug_fallback_record_fault(
            MODULE_ID, s_debug.tripwire_details, __FILE__, __LINE__,
            "test_debug_integrated:section2"
        );
        (void)debug_fallback_finish_with_fallback(
            MODULE_ID, "Degraded to Safe-Mode fallback on missing debugger manifest"
        );
        if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                     "Manifest '%s' missing; populated monolith fallback.", target);
        }
        *out_config = debug_get_monolith_fallback_config();
        return false;
    }

    if (!master_loader_validate_schema(&loader, "tenlux.data.debug_menu.v1")) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_STATE_CORRUPT;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug manifest schema invalid: %.64s", target);
        (void)debug_fallback_record_fault(
            MODULE_ID, s_debug.tripwire_details, __FILE__, __LINE__,
            "test_debug_integrated:section2"
        );
        *out_config = debug_get_monolith_fallback_config();
        return false;
    }

    char active_profile[32];
    if (!master_loader_get_string(&loader, NULL, "active_profile",
                                  active_profile, sizeof(active_profile))) {
        str_safe_copy(active_profile, DEBUG_DEFAULT_PROFILE, sizeof(active_profile));
    }

    return debug_load_config_profile(active_profile, out_config);
}

debug_config_t debug_get_monolith_fallback_config(void)
{
    debug_config_t cfg;
    debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));
    return cfg;
}

debug_config_t debug_get_default_config(void)
{
    debug_config_t cfg;
    if (debug_load_config_from_manifest(NULL, &cfg)) {
        return cfg;
    }

    /* Safe-Mode diskless rescue: hydrate canonical "default" from compiled-in ROM */
    if (debug_fallback_load_rom_profile(DEBUG_DEFAULT_PROFILE, &cfg, sizeof(cfg))) {
        return cfg;
    }

    return debug_get_monolith_fallback_config();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Debugger State Queries, Step Control & AWAIT Recovery
--------------------------------------------------------------------------------
*/

bool debug_is_active(void)
{
    return s_debug_active;
}

bool debug_is_data_driven(void)
{
    return s_debug_data_driven;
}

bool debug_is_safe_mode(void)
{
    return s_debug_active && (s_debug.safe_mode || !s_debug_data_driven);
}

const char *debug_get_active_profile_name(void)
{
    if (!s_debug_active) {
        return "inactive";
    }
    return s_debug.config.profile_name;
}

debug_tripwire_t debug_get_tripwire_status(void)
{
    return s_debug.tripwire_status;
}

const char *debug_get_tripwire_details(void)
{
    return s_debug.tripwire_details;
}

void debug_reset_tripwire(void)
{
    s_debug.tripwire_status = DEBUG_TRIPWIRE_CLEAR;
    s_debug.tripwire_details[0] = '\0';
    s_debug.await_active = false;
    s_debug.fallback_completed = false;
    if (s_debug.step_mode == DEBUG_STEP_AWAIT) {
        s_debug.step_mode = DEBUG_STEP_RUN;
    }
    (void)debug_fallback_clear_await("Cleared tripwire latch via debug_reset_tripwire()");
}

void debug_set_step_mode(debug_step_mode_t mode)
{
    if (!debug_step_mode_is_valid(mode)) {
        return;
    }

    /* Enforce AWAIT lock: prevent unverified transition out of unresolved tripwire */
    if (s_debug.await_active && s_debug.tripwire_status != DEBUG_TRIPWIRE_CLEAR &&
        mode != DEBUG_STEP_AWAIT) {
        if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                     "Blocked step mode switch to %s while AWAIT active (%s). Call debug_resolve_await().",
                     debug_step_mode_name(mode), s_debug.tripwire_details);
        }
        return;
    }

    s_debug.step_mode = mode;
    if (mode == DEBUG_STEP_AWAIT) {
        s_debug.await_active = true;
    }
}

debug_step_mode_t debug_get_step_mode(void)
{
    return s_debug.step_mode;
}

bool debug_is_awaiting(void)
{
    return s_debug_active && (s_debug.await_active || s_debug.step_mode == DEBUG_STEP_AWAIT);
}

bool debug_can_step(void)
{
    if (!s_debug_active) {
        return true;
    }
    return !debug_is_awaiting() && (s_debug.step_mode != DEBUG_STEP_PAUSED);
}

bool debug_resolve_await(
    const debug_state_fields_t *restored_fields,
    debug_step_mode_t           resume_mode,
    const char                 *resolution_note
) {
    if (!s_debug_active) {
        return false;
    }

    /* Step 1: Reject corrupt repaired/rewound state fields cleanly */
    if (restored_fields != NULL && !dbg_state_fields_are_sane(restored_fields)) {
        if (!s_debug.in_audit) {
            LOG_ERROR(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.8f,
                      "Cannot exit AWAIT: restored fields violate invariants (HP=%d/%d Sway=%d).",
                      restored_fields->hp, restored_fields->max_hp, restored_fields->moral_sway);
        }
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "AWAIT recovery blocked: restored state fields invalid (HP=%d/%d, Sway=%+d)",
                 restored_fields->hp, restored_fields->max_hp, restored_fields->moral_sway);
        return false;
    }

    /* Step 2: Re-run failing registered probe validation check if applicable */
    if (s_debug.last_report.probe_id[0] != '\0') {
        for (size_t i = 0U; i < s_debug.probe_count; i++) {
            debug_validation_probe_t *probe = &s_debug.probes[i];
            if (probe->is_active &&
                strcmp(probe->probe_id, s_debug.last_report.probe_id) == 0 &&
                probe->validate_fn != NULL) {
                diag_result_t recheck = probe->validate_fn();
                if (!recheck.passed || recheck.severity >= SEVERITY_CRITICAL) {
                    snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                             "AWAIT blocked: probe '%s' still failing (%s)",
                             probe->probe_id,
                             (recheck.details != NULL) ? recheck.details : "failed");
                    if (!s_debug.in_audit) {
                        LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.6f,
                                 "%s", s_debug.tripwire_details);
                    }
                    return false;
                }
                break;
            }
        }
    }

    /* Step 3: Clear AWAIT/tripwire latches and synchronize Safe-Mode BIOS */
    s_debug.await_active = false;
    s_debug.fallback_completed = false;
    s_debug.tripwire_status = DEBUG_TRIPWIRE_CLEAR;
    s_debug.tripwire_details[0] = '\0';
    s_debug.safe_mode = !s_debug.config.loaded_from_data;
    s_debug.last_report.await_active = false;
    s_debug.last_report.tripwire_status = DEBUG_TRIPWIRE_CLEAR;

    debug_step_mode_t target_mode = (debug_step_mode_is_valid(resume_mode) &&
                                     resume_mode != DEBUG_STEP_AWAIT)
                                        ? resume_mode
                                        : DEBUG_STEP_RUN;
    s_debug.step_mode = target_mode;

    (void)debug_fallback_clear_await(resolution_note);

    if (!s_debug.in_audit) {
        LOG_INFO(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.5f,
                 "AWAIT resolved -> mode=%s (%s)",
                 debug_step_mode_name(target_mode),
                 (resolution_note != NULL && resolution_note[0] != '\0')
                     ? resolution_note
                     : "Validated state restored");
    }

    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Snapshots, Locked Retention, Time-Travel Rewind & Diff
--------------------------------------------------------------------------------
*/

uint64_t debug_record_snapshot(const debug_state_fields_t *fields,
                               const char *cmd, const char *arg)
{
    if (!s_debug_active || fields == NULL) {
        return 0ULL;
    }

    size_t cap = s_debug.config.max_snapshots;
    if (cap == 0U || cap > DEBUG_MAX_SNAPSHOTS) {
        cap = DEBUG_MAX_SNAPSHOTS;
    }

    size_t write_idx = 0U;

    /* Step 1: Select destination slot respecting CIRCULAR_FIFO_WITH_LOCKED_SLOTS */
    if (s_debug.snapshot_count < cap) {
        write_idx = s_debug.snapshot_head;
        s_debug.snapshot_head = (s_debug.snapshot_head + 1U) % cap;
        s_debug.snapshot_count++;
    } else if (s_debug.locked_count == 0U) {
        write_idx = s_debug.snapshot_head;
        s_debug.snapshot_head = (s_debug.snapshot_head + 1U) % cap;
    } else {
        /* Evict oldest UNLOCKED snapshot while preserving chronological order */
        dbg_normalize_ring_for_locks();
        size_t evict_idx = cap;
        for (size_t i = 0U; i < cap; i++) {
            if (!s_debug.snapshots[i].locked) {
                evict_idx = i;
                break;
            }
        }
        if (evict_idx == cap) {
            evict_idx = 0U;
            if (s_debug.locked_count > 0U) {
                s_debug.locked_count--;
            }
        }
        if (evict_idx < cap - 1U) {
            memmove(
                &s_debug.snapshots[evict_idx],
                &s_debug.snapshots[evict_idx + 1U],
                (cap - 1U - evict_idx) * sizeof(debug_snapshot_t)
            );
        }
        write_idx = cap - 1U;
        s_debug.snapshot_head = 0U;
    }

    /* Step 2: Populate immutable snapshot frame */
    debug_snapshot_t *snap = &s_debug.snapshots[write_idx];
    memset(snap, 0, sizeof(*snap));
    snap->snapshot_id = s_debug.next_snapshot_id++;
    snap->timestamp_ms = time_get_monotonic_ms();
    snap->state = *fields;
    str_safe_copy(snap->last_command, (cmd != NULL) ? cmd : "", sizeof(snap->last_command));
    str_safe_copy(snap->last_arg, (arg != NULL) ? arg : "", sizeof(snap->last_arg));
    snap->state_hash = debug_compute_state_hash(fields);
    snap->log_sequence_id = log_get_total_entries();
    snap->locked = false;

    /* Step 3: Track ring buffer telemetry */
    dbg_update_snapshot_watermark(s_debug.snapshot_count);
    s_debug.total_snapshots_captured++;

    return snap->snapshot_id;
}

bool debug_lock_snapshot(uint64_t snapshot_id, const char *lock_reason)
{
    if (!s_debug_active || s_debug.snapshot_count == 0U) {
        return false;
    }

    dbg_normalize_ring_for_locks();

    size_t target_idx = s_debug.snapshot_count;
    if (snapshot_id == 0ULL) {
        target_idx = s_debug.snapshot_count - 1U;
    } else {
        for (size_t i = 0U; i < s_debug.snapshot_count; i++) {
            size_t phys = dbg_logical_to_physical(i);
            if (s_debug.snapshots[phys].snapshot_id == snapshot_id) {
                target_idx = i;
                break;
            }
        }
    }

    if (target_idx >= s_debug.snapshot_count) {
        return false;
    }

    size_t phys = dbg_logical_to_physical(target_idx);
    debug_snapshot_t *snap = &s_debug.snapshots[phys];

    if (!snap->locked) {
        size_t quota = s_debug.config.locked_slot_quota;
        if (quota == 0U) {
            quota = DEBUG_FALLBACK_LOCKED_SLOT_QUOTA;
        }
        if (quota >= s_debug.config.max_snapshots && s_debug.config.max_snapshots > 1U) {
            quota = s_debug.config.max_snapshots - 1U;
        }
        while (s_debug.locked_count >= quota) {
            dbg_unlock_oldest_locked();
        }
        snap->locked = true;
        s_debug.locked_count++;
    }

    str_safe_copy(snap->lock_reason,
                  (lock_reason != NULL && lock_reason[0] != '\0')
                      ? lock_reason
                      : "TRIGGER_TRIPWIRE_LATCH",
                  sizeof(snap->lock_reason));
    return true;
}

size_t debug_get_snapshot_count(void)
{
    if (!s_debug_active) {
        return 0U;
    }
    return s_debug.snapshot_count;
}

bool debug_get_snapshot(size_t index, debug_snapshot_t *out_snap)
{
    if (!s_debug_active || out_snap == NULL || index >= s_debug.snapshot_count) {
        return false;
    }

    size_t phys = dbg_logical_to_physical(index);
    *out_snap = s_debug.snapshots[phys];
    return true;
}

const debug_snapshot_t *debug_get_latest_snapshot(void)
{
    if (!s_debug_active || s_debug.snapshot_count == 0U) {
        return NULL;
    }

    size_t latest_phys = dbg_logical_to_physical(s_debug.snapshot_count - 1U);
    return &s_debug.snapshots[latest_phys];
}

bool debug_rewind_to_snapshot(size_t index, debug_state_fields_t *out_fields)
{
    if (!s_debug_active || out_fields == NULL) {
        return false;
    }

    debug_snapshot_t snap;
    if (!debug_get_snapshot(index, &snap)) {
        return false;
    }

    /* Verify cryptographic determinism against tampering before restoring */
    uint32_t verify_hash = debug_compute_state_hash(&snap.state);
    if (verify_hash != snap.state_hash) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_HASH_MISMATCH;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Rewind checksum mismatch on snap #%lu (0x%08X != 0x%08X)",
                 (unsigned long)snap.snapshot_id, verify_hash, snap.state_hash);
        s_debug.traps_triggered++;
        (void)debug_fallback_record_fault(
            MODULE_ID, s_debug.tripwire_details, __FILE__, __LINE__,
            "debug_rewind_to_snapshot"
        );
        (void)debug_fallback_finish_with_fallback(
            MODULE_ID, "Aborted corrupted rewind target and entered AWAIT"
        );
        s_debug.await_active = true;
        s_debug.step_mode = DEBUG_STEP_AWAIT;
        return false;
    }

    *out_fields = snap.state;
    if (!s_debug.in_audit) {
        LOG_INFO(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.25f,
                 "Rewound to Snapshot #%lu (Turn %d, Hash 0x%08X%s)",
                 (unsigned long)snap.snapshot_id, snap.state.turn,
                 snap.state_hash, snap.locked ? ", LOCKED" : "");
    }
    return true;
}

size_t debug_diff_snapshots(const debug_snapshot_t *s1, const debug_snapshot_t *s2,
                            char *out_buf, size_t buf_len)
{
    if (s1 == NULL || s2 == NULL || out_buf == NULL || buf_len == 0U) {
        return 0U;
    }

    char tmp[1024];
    size_t offset = 0U;

    dbg_append_fmt(tmp, sizeof(tmp), &offset,
        "Snapshot Diff: #%lu (Turn %d) -> #%lu (Turn %d)\n"
        "--------------------------------------------------\n",
        (unsigned long)s1->snapshot_id, s1->state.turn,
        (unsigned long)s2->snapshot_id, s2->state.turn);

    if (s1->state.hp != s2->state.hp) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  HP:           %d -> %d (%+d)\n",
            s1->state.hp, s2->state.hp, s2->state.hp - s1->state.hp);
    }
    if (s1->state.shield_power != s2->state.shield_power) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Shield (Lux): %d -> %d (%+d)\n",
            s1->state.shield_power, s2->state.shield_power,
            s2->state.shield_power - s1->state.shield_power);
    }
    if (s1->state.sword_edge != s2->state.sword_edge) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Sword (Ten):  %d -> %d (%+d)\n",
            s1->state.sword_edge, s2->state.sword_edge,
            s2->state.sword_edge - s1->state.sword_edge);
    }
    if (s1->state.stance != s2->state.stance) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Stance:       %d -> %d\n", s1->state.stance, s2->state.stance);
    }
    if (s1->state.moral_sway != s2->state.moral_sway) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Moral Sway:   %d -> %d (%+d)\n",
            s1->state.moral_sway, s2->state.moral_sway,
            s2->state.moral_sway - s1->state.moral_sway);
    }
    if (s1->state.location != s2->state.location) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Location:     %d -> %d\n", s1->state.location, s2->state.location);
    }
    if (s1->state_hash != s2->state_hash) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  State Hash:   0x%08X -> 0x%08X\n", s1->state_hash, s2->state_hash);
    }

    str_safe_copy(out_buf, tmp, buf_len);
    return strlen(out_buf);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Conditional Breakpoints & Domain Guardrail Watchpoints
--------------------------------------------------------------------------------
*/

int debug_add_breakpoint(debug_breakpoint_type_t type, int target_value, const char *label)
{
    if (!s_debug_active || !debug_breakpoint_type_is_valid(type)) {
        return -1;
    }
    if (s_debug.breakpoint_count >= DEBUG_MAX_BREAKPOINTS) {
        return -1;
    }

    debug_breakpoint_t *bp = &s_debug.breakpoints[s_debug.breakpoint_count++];
    memset(bp, 0, sizeof(*bp));
    bp->id = s_debug.next_breakpoint_id++;
    bp->type = type;
    bp->target_value = target_value;
    bp->is_active = true;
    str_safe_copy(bp->label, (label != NULL) ? label : "Breakpoint", sizeof(bp->label));

    if (!s_debug.in_audit) {
        LOG_DEBUG(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.0f,
                  "Breakpoint #%d registered (%s, target=%d, label='%s')",
                  bp->id, debug_breakpoint_type_name(type), target_value, bp->label);
    }
    return bp->id;
}

bool debug_remove_breakpoint(int bp_id)
{
    if (!s_debug_active || bp_id <= 0) {
        return false;
    }

    for (size_t i = 0U; i < s_debug.breakpoint_count; i++) {
        if (s_debug.breakpoints[i].id == bp_id) {
            for (size_t j = i; j + 1U < s_debug.breakpoint_count; j++) {
                s_debug.breakpoints[j] = s_debug.breakpoints[j + 1U];
            }
            s_debug.breakpoint_count--;
            return true;
        }
    }
    return false;
}

void debug_clear_breakpoints(void)
{
    s_debug.breakpoint_count = 0U;
}

bool debug_check_breakpoints(const debug_state_fields_t *fields,
                             char *out_reason, size_t max_len)
{
    if (!s_debug_active || fields == NULL) {
        return false;
    }

    /* Step 1: Evaluate registered validation probes if engine traps active */
    if (s_debug.config.engine_traps_active &&
        s_debug.config.verify_on_step &&
        !s_debug.in_probe &&
        s_debug.probe_count > 0U) {
        if (debug_check_tripwires(NULL) > 0U) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len, "Validation probe halted execution: %s",
                         s_debug.tripwire_details);
            }
            return true;
        }
    }

    /* Step 2: Evaluate Layer 1 domain guardrails when game traps are active */
    if (s_debug.config.game_traps_active) {
        if (s_debug.config.break_on_sway_breach &&
            (fields->moral_sway > 100 || fields->moral_sway < -100)) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len, "Moral Sway out of range: %d", fields->moral_sway);
            }
            return true;
        }

        if (s_debug.config.break_on_resonance_breach &&
            (fields->shield_power > 100 || fields->sword_edge > 100)) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len,
                         "Relic Resonance overflow (Shield: %d, Sword: %d)",
                         fields->shield_power, fields->sword_edge);
            }
            return true;
        }

        if (s_debug.config.break_on_hp_zero && fields->hp <= 0) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len, "Hero HP reached zero (HP: %d)", fields->hp);
            }
            return true;
        }
    }

    /* Step 3: Evaluate custom user-registered watchpoints */
    for (size_t i = 0U; i < s_debug.breakpoint_count; i++) {
        const debug_breakpoint_t *bp = &s_debug.breakpoints[i];
        if (!bp->is_active) {
            continue;
        }

        bool triggered = false;
        switch (bp->type) {
            case DEBUG_BREAK_TURN:
                triggered = (fields->turn == bp->target_value);
                break;
            case DEBUG_BREAK_STANCE:
                triggered = (fields->stance == bp->target_value);
                break;
            case DEBUG_BREAK_SWAY_RANGE:
                triggered = (abs(fields->moral_sway) >= abs(bp->target_value));
                break;
            case DEBUG_BREAK_RESONANCE_BREACH:
                triggered = (fields->shield_power >= bp->target_value ||
                             fields->sword_edge >= bp->target_value);
                break;
            case DEBUG_BREAK_HP_THRESHOLD:
                triggered = (fields->hp <= bp->target_value);
                break;
            case DEBUG_BREAK_LOCATION:
                triggered = (fields->location == bp->target_value);
                break;
            case DEBUG_BREAK_TRIPWIRE:
                triggered = (s_debug.tripwire_status != DEBUG_TRIPWIRE_CLEAR);
                break;
            default:
                break;
        }

        if (triggered) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len, "Breakpoint #%d Hit: %s (Target: %d)",
                         bp->id, bp->label, bp->target_value);
            }
            return true;
        }
    }

    return false;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Validation Probe Registry, Tripwires, Fallback & AWAIT
--------------------------------------------------------------------------------
*/

int debug_register_validation_probe(const debug_validation_probe_t *probe)
{
    if (probe == NULL || probe->probe_id[0] == '\0') {
        return -1;
    }

    /* Step 1: Update existing slot idempotently if probe_id matches */
    for (size_t i = 0U; i < s_debug.probe_count; i++) {
        if (strcmp(s_debug.probes[i].probe_id, probe->probe_id) == 0) {
            uint32_t prev_count = s_debug.probes[i].trigger_count;
            s_debug.probes[i] = *probe;
            s_debug.probes[i].is_active = true;
            s_debug.probes[i].trigger_count = prev_count;
            return (int)i;
        }
    }

    /* Step 2: Allocate next available static probe slot */
    if (s_debug.probe_count >= DEBUG_MAX_VALIDATION_PROBES) {
        return -1;
    }

    size_t slot = s_debug.probe_count++;
    s_debug.probes[slot] = *probe;
    s_debug.probes[slot].is_active = true;
    return (int)slot;
}

void debug_clear_validation_probes(void)
{
    memset(s_debug.probes, 0, sizeof(s_debug.probes));
    s_debug.probe_count = 0U;
}

bool debug_trigger_probe(
    const char *probe_id,
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *func
) {
    if (s_debug.in_probe) {
        return false;
    }

    s_debug.in_probe = true;

    /* Step 1: Locate registered probe metadata if present */
    debug_validation_probe_t *matched = NULL;
    if (probe_id != NULL && probe_id[0] != '\0') {
        for (size_t i = 0U; i < s_debug.probe_count; i++) {
            if (s_debug.probes[i].is_active &&
                strcmp(s_debug.probes[i].probe_id, probe_id) == 0) {
                matched = &s_debug.probes[i];
                matched->trigger_count++;
                break;
            }
        }
    }

    const char *eff_sub = (subsystem != NULL && subsystem[0] != '\0')
                              ? subsystem
                              : ((matched != NULL && matched->subsystem[0] != '\0')
                                     ? matched->subsystem
                                     : MODULE_ID);
    const char *eff_cond = (condition != NULL && condition[0] != '\0')
                               ? condition
                               : "Validation tripwire raised";
    debug_tripwire_t eff_wire = (matched != NULL && matched->tripwire_code != DEBUG_TRIPWIRE_CLEAR)
                                    ? matched->tripwire_code
                                    : DEBUG_TRIPWIRE_TEST_FAILED;
    debug_tripwire_reaction_t eff_react = (matched != NULL)
                                              ? matched->reaction
                                              : dbg_default_reaction_for_wire(eff_wire);
    debug_test_tier_t eff_tier = (matched != NULL) ? matched->tier : DEBUG_TEST_TIER_AUDIT;
    const char *eff_sec = (matched != NULL && matched->source_section[0] != '\0')
                              ? matched->source_section
                              : ((func != NULL) ? func : "unknown_section");
    const char *eff_test = (matched != NULL && matched->test_suite_ref[0] != '\0')
                               ? matched->test_suite_ref
                               : "BLOCK4_PREFLIGHT";
    bool want_await = (matched != NULL) ? matched->auto_await : s_debug.config.auto_await_on_tripwire;

    /* Step 2: Invoke Block 4 validation callback when registered to confirm health */
    char probe_detail_buf[128];
    if (matched != NULL && matched->validate_fn != NULL) {
        diag_result_t probe_res = matched->validate_fn();
        if (!probe_res.passed && probe_res.details != NULL && condition == NULL) {
            eff_cond = probe_res.details;
        }
        snprintf(probe_detail_buf, sizeof(probe_detail_buf), "%s [probe=%s]",
                 eff_cond, probe_res.passed ? "SELF_OK_BY_FALLBACK" : "FAIL_CONFIRMED");
    } else {
        str_safe_copy(probe_detail_buf, eff_cond, sizeof(probe_detail_buf));
    }

    /* Step 3: Latch primary debugger tripwire and forensic counters */
    s_debug.tripwire_status = eff_wire;
    s_debug.traps_triggered++;
    s_debug.probe_failures++;

    snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
             "[%s:%s] %s (%s:%d | %s)",
             eff_sub,
             debug_test_tier_name(eff_tier),
             probe_detail_buf,
             (file != NULL) ? file : "unknown",
             line,
             eff_test);

    /* Step 4: Record into Tier-0 BIOS & downgrade to complete safely in fallback */
    (void)debug_fallback_record_fault(eff_sub, probe_detail_buf, file, line, eff_test);

    char fb_action[96];
    snprintf(fb_action, sizeof(fb_action),
             "Reaction=%s | Safe-Mode completed execution -> AWAIT",
             debug_tripwire_reaction_name(eff_react));
    (void)debug_fallback_finish_with_fallback(eff_sub, fb_action);

    s_debug.safe_mode = true;
    s_debug.fallback_completed = true;

    /* Step 5: Pin critical forensic snapshot in circular ring if snapshots exist */
    uint64_t pinned_id = 0ULL;
    bool snap_locked = false;
    if (s_debug_active && s_debug.snapshot_count > 0U &&
        debug_lock_snapshot(0ULL, "TRIGGER_TRIPWIRE_LATCH")) {
        const debug_snapshot_t *latest = debug_get_latest_snapshot();
        if (latest != NULL) {
            pinned_id = latest->snapshot_id;
            snap_locked = true;
        }
    }

    /* Step 6: Enter DEBUG_STEP_AWAIT when debugger active and break_on_tripwire armed */
    bool engaged_await = false;
    if (s_debug_active && !s_debug.in_audit &&
        s_debug.config.break_on_tripwire && want_await) {
        s_debug.step_mode = DEBUG_STEP_AWAIT;
        s_debug.await_active = true;
        engaged_await = true;
    }

    /* Step 7: Store full forensic tripwire report for CLI / REPL / test inspection */
    memset(&s_debug.last_report, 0, sizeof(s_debug.last_report));
    s_debug.last_report.tripwire_status = eff_wire;
    s_debug.last_report.reaction = eff_react;
    s_debug.last_report.tier = eff_tier;
    s_debug.last_report.fallback_completed = true;
    s_debug.last_report.await_active = engaged_await;
    s_debug.last_report.snapshot_locked = snap_locked;
    s_debug.last_report.locked_snapshot_id = pinned_id;
    str_safe_copy(s_debug.last_report.probe_id,
                  (probe_id != NULL) ? probe_id : "ad_hoc.tripwire",
                  sizeof(s_debug.last_report.probe_id));
    str_safe_copy(s_debug.last_report.subsystem, eff_sub, sizeof(s_debug.last_report.subsystem));
    str_safe_copy(s_debug.last_report.condition, probe_detail_buf,
                  sizeof(s_debug.last_report.condition));
    snprintf(s_debug.last_report.source_location, sizeof(s_debug.last_report.source_location),
             "%s:%d", (file != NULL) ? file : "unknown", line);
    str_safe_copy(s_debug.last_report.source_section, eff_sec,
                  sizeof(s_debug.last_report.source_section));
    str_safe_copy(s_debug.last_report.test_suite_ref, eff_test,
                  sizeof(s_debug.last_report.test_suite_ref));
    str_safe_copy(s_debug.last_report.fallback_action, fb_action,
                  sizeof(s_debug.last_report.fallback_action));

    s_debug.in_probe = false;
    return true;
}

size_t debug_check_tripwires(const debug_state_fields_t *fields)
{
    if (!s_debug_active || s_debug.in_probe) {
        return 0U;
    }

    size_t failures = 0U;

    /* Step 1: Verify live state fields if caller passed simulation snapshot */
    if (fields != NULL && s_debug.config.game_traps_active &&
        !dbg_state_fields_are_sane(fields)) {
        (void)debug_trigger_probe(
            "debug.state_invariant",
            MODULE_ID,
            "Live simulation fields violated domain bounds (HP/Sway/Relics)",
            __FILE__,
            __LINE__,
            __func__
        );
        failures++;
    }

    /* Step 2: Evaluate all registered Block 4 validation probes */
    for (size_t i = 0U; i < s_debug.probe_count; i++) {
        debug_validation_probe_t *probe = &s_debug.probes[i];
        if (!probe->is_active || probe->validate_fn == NULL) {
            continue;
        }

        s_debug.in_probe = true;
        diag_result_t res = probe->validate_fn();
        s_debug.in_probe = false;

        if (!res.passed || res.severity >= SEVERITY_CRITICAL) {
            (void)debug_trigger_probe(
                probe->probe_id,
                probe->subsystem,
                (res.details != NULL) ? res.details : "Validation probe returned failure",
                __FILE__,
                __LINE__,
                __func__
            );
            failures++;
        }
    }

    return failures;
}

void debug_trap_tripwire(const char *subsystem, const char *condition,
                         const char *file, int line)
{
    /* Step 1: Latch classic STATE_CORRUPT code and forensic detail string verbatim */
    s_debug.tripwire_status = DEBUG_TRIPWIRE_STATE_CORRUPT;
    s_debug.traps_triggered++;

    snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
             "[%s] %s (%s:%d)",
             (subsystem != NULL) ? subsystem : "unknown",
             (condition != NULL) ? condition : "anomaly",
             (file != NULL) ? file : "unknown", line);

    /* Step 2: Record fault in Tier-0 BIOS and complete via Safe-Mode fallback */
    (void)debug_fallback_record_fault(
        subsystem,
        (condition != NULL) ? condition : "anomaly",
        file,
        line,
        "debug_trap_tripwire"
    );

    if (s_debug_active && !s_debug.in_probe && !s_debug.in_audit) {
        (void)debug_fallback_finish_with_fallback(
            (subsystem != NULL) ? subsystem : MODULE_ID,
            "Completed trap safely via Safe-Mode fallback -> AWAIT"
        );
        s_debug.safe_mode = true;
        s_debug.fallback_completed = true;

        if (s_debug.snapshot_count > 0U) {
            (void)debug_lock_snapshot(0ULL, "TRIGGER_TRIPWIRE_LATCH");
        }
        if (s_debug.config.break_on_tripwire && s_debug.config.auto_await_on_tripwire) {
            s_debug.step_mode = DEBUG_STEP_AWAIT;
            s_debug.await_active = true;
        }
    }

    /* Step 3: Emit guarded telemetry warning without inducing recursive log traps */
    if (!s_debug.in_probe && !s_debug.in_audit) {
        s_debug.in_probe = true;
        LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                 "Tripwire trapped: %s", s_debug.tripwire_details);
        s_debug.in_probe = false;
    }

    if (s_debug_active && s_debug.config.auto_dump_on_tripwire && !s_debug.in_audit) {
        fprintf(stderr, "\n[DEBUG TRIPWIRE ALERT] %s\n", s_debug.tripwire_details);
    }
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.6: Console Diagnostics & Forensic Tripwire Readouts
--------------------------------------------------------------------------------
*/

void debug_dump_state(const debug_state_fields_t *fields)
{
    if (fields == NULL) {
        return;
    }

    uint32_t hash = debug_compute_state_hash(fields);

    printf("============================================================\n");
    printf("        TENEBRIS AND LUX - LIVE STATE INSPECTOR             \n");
    printf("============================================================\n");
    printf(" Hero Name:        %-16s | Turn:        %d\n", fields->hero_name, fields->turn);
    printf(" Health (HP):      %3d / %-3d          | State Hash:  0x%08X\n",
           fields->hp, fields->max_hp, hash);
    printf(" Shield of Lux:    %3d / 100            | Barrier:     %d turns\n",
           fields->shield_power, fields->lux_barrio_turns);
    printf(" Sword of Tenebris:%3d / 100            | Charm:       %d turns\n",
           fields->sword_edge, fields->charm_turns);
    printf(" Combat Stance:    ID %-2d                | Province ID: %d\n",
           fields->stance, fields->location);
    printf(" Moral Sway:       %+4d [-100, +100]    | Threat HP:   %d\n",
           fields->moral_sway, fields->threat_hp);
    printf(" Traps Latched:    %-4u                | Snapshots:   %zu / %zu\n",
           s_debug.traps_triggered, s_debug.snapshot_count, s_debug.config.max_snapshots);
    printf(" Mode / AWAIT:     %-10s           | Locked Snaps:%zu\n",
           debug_step_mode_name(s_debug.step_mode), s_debug.locked_count);
    printf("============================================================\n");
}

void debug_dump_snapshot_history(void)
{
    if (!s_debug_active || s_debug.snapshot_count == 0U) {
        printf("[DEBUG] No snapshots recorded in history.\n");
        return;
    }

    printf("\n+------+------+------------------+------+------+------+------------+--------+--------------------+\n");
    printf("|  ID  | Turn | Command          |  HP  | Sway | Lock | Hash       | LogSeq | Stance / Loc       |\n");
    printf("+------+------+------------------+------+------+------+------------+--------+--------------------+\n");

    for (size_t i = 0U; i < s_debug.snapshot_count; i++) {
        debug_snapshot_t snap;
        if (debug_get_snapshot(i, &snap)) {
            printf("| %4lu | %4d | %-16s | %4d | %+4d | %-4s | 0x%08X | %6lu | S:%-2d P:%-2d          |\n",
                   (unsigned long)snap.snapshot_id,
                   snap.state.turn,
                   snap.last_command[0] ? snap.last_command : "-",
                   snap.state.hp,
                   snap.state.moral_sway,
                   snap.locked ? "YES" : "no",
                   snap.state_hash,
                   (unsigned long)snap.log_sequence_id,
                   snap.state.stance,
                   snap.state.location);
        }
    }

    printf("+------+------+------------------+------+------+------+------------+--------+--------------------+\n\n");
}

bool debug_get_tripwire_report(debug_tripwire_report_t *out_report)
{
    if (out_report == NULL) {
        return false;
    }

    *out_report = s_debug.last_report;
    out_report->tripwire_status = s_debug.tripwire_status;
    out_report->await_active = s_debug.await_active;
    out_report->fallback_completed = s_debug.fallback_completed;
    return true;
}

bool debug_dump_tripwire_report(char *out_buf, size_t buf_len)
{
    if (out_buf == NULL || buf_len < 128U) {
        return false;
    }

    const debug_tripwire_report_t *rep = &s_debug.last_report;
    int written = snprintf(
        out_buf,
        buf_len,
        "+============================================================+\n"
        "| DEBUGGER TRIPWIRE, FALLBACK & AWAIT REPORT                 |\n"
        "+============================================================+\n"
        "| Tripwire Code   : %-40s |\n"
        "| Step Mode       : %-12s | AWAIT Active: %-10s |\n"
        "| Probe / Tier    : %-26s [%-10s] |\n"
        "| Source Section  : %-40s |\n"
        "| Test Suite Ref  : %-40s |\n"
        "| Fallback Action : %-40s |\n"
        "| Locked Snap ID  : %-12lu | Safe Mode   : %-10s |\n"
        "+============================================================+\n",
        debug_tripwire_name(s_debug.tripwire_status),
        debug_step_mode_name(s_debug.step_mode),
        s_debug.await_active ? "YES" : "NO",
        rep->probe_id[0] ? rep->probe_id : "none",
        debug_test_tier_name(rep->tier),
        rep->source_section[0] ? rep->source_section : "none",
        rep->test_suite_ref[0] ? rep->test_suite_ref : "NONE",
        rep->fallback_action[0] ? rep->fallback_action : "NONE",
        (unsigned long)rep->locked_snapshot_id,
        s_debug.safe_mode ? "ACTIVE" : "OFF"
    );

    return (written > 0 && (size_t)written < buf_len);
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug
================================================================================
Mission: Validate hash determinism, fallback ROM integrity, snapshot ring/lock
behavior, breakpoint hits, and probe -> fallback -> AWAIT recovery invariants.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwires & Non-Destructive Self-Audit
--------------------------------------------------------------------------------
*/

diag_result_t debug_preflight(void)
{
    diag_result_t res;
    memset(&res, 0, sizeof(res));
    res.subsystem = debug_manifest.module_id;
    res.check_name = "Engine Debugger Subsystem Preflight";

    /* Step 1: Verify Tier-0 fallback code page and embedded ROM */
    diag_result_t fb_res = debug_fallback_preflight();
    if (!fb_res.passed) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Debugger fallback preflight check failed!";
        return res;
    }

    /* Step 2: Verify FNV-1a determinism and avalanche collision sensitivity */
    debug_state_fields_t s1;
    debug_state_fields_t s2;
    memset(&s1, 0, sizeof(s1));
    s1.turn = 1;
    s1.hp = 500;
    s1.max_hp = 500;
    s1.shield_power = 50;
    s1.sword_edge = 50;
    s1.moral_sway = 0;
    str_safe_copy(s1.hero_name, "Elijah", sizeof(s1.hero_name));

    s2 = s1;
    uint32_t h1 = debug_compute_state_hash(&s1);
    uint32_t h2 = debug_compute_state_hash(&s2);

    if (h1 == 0U || h1 != h2) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "State hashing failed bitwise determinism preflight!";
        return res;
    }

    s2.hp = 499;
    uint32_t h3 = debug_compute_state_hash(&s2);
    if (h1 == h3) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "State hashing failed collision sensitivity preflight!";
        return res;
    }

    /* Step 3: Verify fallback inspect procedure & card contract */
    debug_config_t test_fb_cfg;
    if (!debug_fallback_execute_inspect(&test_fb_cfg, sizeof(test_fb_cfg)) ||
        test_fb_cfg.loaded_from_data) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Fallback execute_inspect invariant check failed!";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Debugger fallback, FNV-1a hashing, and inspect invariants passed preflight.";
    return res;
}

diag_result_t debug_audit(bool verbose)
{
    if (verbose) {
        printf("\n=== DEBUGGER SUBSYSTEM DIAGNOSTIC AUDIT ===\n");
    }

    /* Step 1: Run pure stateless preflight and fallback ROM audit */
    diag_result_t pre = debug_preflight();
    if (!pre.passed) {
        return pre;
    }
    if (!debug_fallback_audit()) {
        diag_result_t fb_fail = {
            false, SEVERITY_CRITICAL, debug_manifest.module_id,
            "Fallback & ROM Audit", "Embedded debug/taxonomy ROM audit failed!"
        };
        return fb_fail;
    }

    /* Step 2: Preserve active debugger state non-destructively in static .bss backup */
    const bool was_active = s_debug_active;
    const bool was_data_driven = s_debug_data_driven;
    s_audit_backup = s_debug;

    debug_config_t test_cfg = debug_get_monolith_fallback_config();
    test_cfg.auto_dump_on_tripwire = false;
    (void)debug_init(&test_cfg);
    s_debug.in_audit = true;

    /* Step 3: Verify snapshot recording, locked protection, and hash-checked rewind */
    debug_state_fields_t sample;
    memset(&sample, 0, sizeof(sample));
    sample.turn = 1;
    sample.hp = 400;
    sample.max_hp = 500;
    sample.moral_sway = 15;
    str_safe_copy(sample.hero_name, "AuditHero", sizeof(sample.hero_name));

    uint64_t snap_id = debug_record_snapshot(&sample, "audit_test", "step1");
    if (snap_id == 0ULL || debug_get_snapshot_count() != 1U ||
        !debug_lock_snapshot(snap_id, "AUDIT_PIN")) {
        s_debug = s_audit_backup;
        s_debug_active = was_active;
        s_debug_data_driven = was_data_driven;
        diag_result_t fail_res = {
            false, SEVERITY_CRITICAL, debug_manifest.module_id,
            "Snapshot Recording & Lock", "Snapshot record or lock failed during audit!"
        };
        return fail_res;
    }

    debug_state_fields_t restored;
    if (!debug_rewind_to_snapshot(0U, &restored) || restored.hp != 400) {
        s_debug = s_audit_backup;
        s_debug_active = was_active;
        s_debug_data_driven = was_data_driven;
        diag_result_t fail_res = {
            false, SEVERITY_CRITICAL, debug_manifest.module_id,
            "Snapshot Rewind", "Snapshot rewind restoration failed during audit!"
        };
        return fail_res;
    }

    /* Step 4: Verify breakpoint registration, evaluation, and clearing */
    int bp_turn = debug_add_breakpoint(DEBUG_BREAK_TURN, 5, "Turn 5 Checkpoint");
    char break_reason[128];
    sample.turn = 5;
    if (bp_turn <= 0 || !debug_check_breakpoints(&sample, break_reason, sizeof(break_reason))) {
        s_debug = s_audit_backup;
        s_debug_active = was_active;
        s_debug_data_driven = was_data_driven;
        diag_result_t fail_res = {
            false, SEVERITY_CRITICAL, debug_manifest.module_id,
            "Breakpoint Evaluation", "Breakpoint condition failed during audit!"
        };
        return fail_res;
    }
    debug_clear_breakpoints();

    /* Step 5: Verify AWAIT gatekeeping (corrupt state rejected; valid state resolves) */
    s_debug.in_audit = false;
    (void)debug_trigger_probe("audit.self_check", MODULE_ID, "Simulated audit fault",
                              __FILE__, __LINE__, __func__);
    s_debug.in_audit = true;
    debug_state_fields_t bad_state = sample;
    bad_state.hp = 9999;
    bool rejected_corrupt = !debug_resolve_await(&bad_state, DEBUG_STEP_RUN, "Corrupt reject check");
    bool resolved_valid = debug_resolve_await(&restored, DEBUG_STEP_RUN, "Audit recovery OK");
    if (!rejected_corrupt || !resolved_valid) {
        s_debug = s_audit_backup;
        s_debug_active = was_active;
        s_debug_data_driven = was_data_driven;
        diag_result_t fail_res = {
            false, SEVERITY_CRITICAL, debug_manifest.module_id,
            "AWAIT Gatekeeper", "AWAIT rejection/resolution contract failed!"
        };
        return fail_res;
    }

    /* Step 6: Verify ASCII inspection cards format cleanly within 1024-byte bounds */
    char card_buf[1024];
    if (!debug_execute_dump_card("CARD_HERO_VITALS", &sample, card_buf, sizeof(card_buf)) ||
        !debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", &sample, card_buf, sizeof(card_buf))) {
        s_debug = s_audit_backup;
        s_debug_active = was_active;
        s_debug_data_driven = was_data_driven;
        diag_result_t fail_res = {
            false, SEVERITY_CRITICAL, debug_manifest.module_id,
            "Inspection Card Formatting", "Card dumping failed during audit!"
        };
        return fail_res;
    }

    if (verbose) {
        printf("%s", card_buf);
    }

    /* Step 7: Restore original live debugger state transparently */
    s_debug = s_audit_backup;
    s_debug_active = was_active;
    s_debug_data_driven = was_data_driven;

    diag_result_t pass_res = {
        true,
        SEVERITY_PASS,
        debug_manifest.module_id,
        "Debugger Subsystem Diagnostic Audit",
        "All debugger invariants, ROM checks, locked snapshots, breakpoints, AWAIT & cards passed."
    };
    return pass_res;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug
================================================================================
Mission: Construct inspection telemetry structs, format ASCII diagnostic cards,
and manage subsystem initialization and shutdown lifecycles cleanly.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

bool debug_execute_inspect(debug_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }

    memset(out_stats, 0, sizeof(*out_stats));
    out_stats->snapshot_count = s_debug.snapshot_count;
    out_stats->max_snapshots = s_debug.config.max_snapshots;
    out_stats->breakpoint_count = s_debug.breakpoint_count;
    out_stats->traps_triggered = s_debug.traps_triggered;
    out_stats->step_mode = s_debug.step_mode;
    out_stats->is_active = s_debug_active;
    out_stats->loaded_from_data = s_debug_data_driven;
    str_safe_copy(out_stats->active_profile, s_debug.config.profile_name,
                  sizeof(out_stats->active_profile));

    out_stats->locked_snapshot_count = s_debug.locked_count;
    out_stats->registered_probe_count = s_debug.probe_count;
    out_stats->probe_failures = s_debug.probe_failures;
    out_stats->safe_mode = s_debug.safe_mode;
    out_stats->await_active = s_debug.await_active;
    out_stats->fallback_completed = s_debug.fallback_completed;
    out_stats->tripwire_status = s_debug.tripwire_status;
    return true;
}

bool debug_execute_dump_card(const char *card_id, const debug_state_fields_t *fields,
                             char *out_buf, size_t buf_len)
{
    if (fields == NULL || out_buf == NULL || buf_len < 128U) {
        return false;
    }

    const char *id = (card_id != NULL && card_id[0] != '\0') ? card_id : "CARD_HERO_VITALS";
    uint32_t hash = debug_compute_state_hash(fields);
    int written = 0;

    if (strcmp(id, "CARD_HERO_VITALS") == 0) {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "              HERO VITALS & STANCE READOUT                  \n"
            "============================================================\n"
            "Protagonist:   %-20s (The Custom Bearer)\n"
            "Bound Relics:  Shield of Lux / Sword of Tenebris\n"
            "Vitals:        HP: %d/%d  MP: %d  TP: %d\n"
            "Active Stance: ID %d\n"
            "Relic Resonance:\n"
            "  [LUX] Shield: %d/100  (Warding Power)\n"
            "  [TEN] Sword:  %d/100  (Edge Sharpness)\n"
            "Location:      Province ID %d | Threat HP: %d\n"
            "State Hash:    0x%08X\n"
            "============================================================\n",
            fields->hero_name[0] ? fields->hero_name : "The Custom Bearer",
            fields->hp, fields->max_hp, fields->mp, fields->tp,
            fields->stance,
            fields->shield_power, fields->sword_edge,
            fields->location, fields->threat_hp,
            hash);
    } else if (strcmp(id, "CARD_STATERA_LEDGER") == 0) {
        const char *ending =
            (fields->moral_sway >= 75)
                ? "Path of Radiance (Lux Overload)"
                : (fields->moral_sway <= -75)
                      ? "Path of Abyssal Sovereign (Tenebrous Eclipse)"
                      : (fields->moral_sway >= -25 && fields->moral_sway <= 25)
                            ? "The Sovereign Accord (Equilibrium)"
                            : "The Forgotten Stasis (The King's Slumber)";
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "              STATERA CONSCIENCE LEDGER                     \n"
            "============================================================\n"
            "Moral Sway:    %+4d [-100, +100]\n"
            "Ending Fork:   %s\n"
            "Turn Sequence: %d\n"
            "Barrier / Mist:Lux: %d turns | Ten: %d turns\n"
            "State Hash:    0x%08X\n"
            "============================================================\n",
            fields->moral_sway, ending, fields->turn,
            fields->lux_barrio_turns, fields->charm_turns, hash);
    } else if (strcmp(id, "CARD_COMBAT_ENGAGEMENT") == 0) {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "              TACTICAL COMBAT ENGAGEMENT                    \n"
            "============================================================\n"
            "Combat Turn:   Turn #%d\n"
            "Active Threat: Threat HP: %d\n"
            "Hero Health:   HP: %d / %d\n"
            "Shield Power:  %d/100 (Lux Barrio: %d turns)\n"
            "Sword Edge:    %d/100 (Cuttatio Mist: %d turns)\n"
            "Foes Vanquished: %d\n"
            "State Hash:    0x%08X\n"
            "============================================================\n",
            fields->turn, fields->threat_hp, fields->hp, fields->max_hp,
            fields->shield_power, fields->lux_barrio_turns,
            fields->sword_edge, fields->charm_turns,
            fields->foes_vanquished, hash);
    } else if (strcmp(id, "CARD_ENGINE_HEARTBEAT") == 0) {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "              ENGINE PLATFORM HEARTBEAT                     \n"
            "============================================================\n"
            "Memory Stratum:Zero-Heap (ALLOC_ZERO_HEAP) Verified\n"
            "Snapshots:     %zu / %zu (Head: %zu, Locked: %zu)\n"
            "Breakpoints:   %zu | Probes: %zu\n"
            "Tripwire Traps:%u latched | Safe Mode: %s\n"
            "Stepping Mode: %s\n"
            "State Hash:    0x%08X\n"
            "============================================================\n",
            s_debug.snapshot_count, s_debug.config.max_snapshots,
            s_debug.snapshot_head, s_debug.locked_count,
            s_debug.breakpoint_count, s_debug.probe_count,
            s_debug.traps_triggered, s_debug.safe_mode ? "ON" : "OFF",
            debug_step_mode_name(s_debug.step_mode), hash);
    } else if (strcmp(id, "CARD_TRIPWIRE_DIAGNOSTIC") == 0) {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "             DIAGNOSTIC TRIPWIRE FORENSIC REPORT            \n"
            "============================================================\n"
            "Tripwire Code: 0x%02X (%s)\n"
            "Mode / AWAIT:  %s (AWAIT=%s | FallbackDone=%s)\n"
            "Details:       %s\n"
            "Probe / Test:  %s -> %s\n"
            "Traps Latched: %u | Last Turn: %d | Hash: 0x%08X\n"
            "============================================================\n",
            (unsigned int)s_debug.tripwire_status,
            debug_tripwire_name(s_debug.tripwire_status),
            debug_step_mode_name(s_debug.step_mode),
            s_debug.await_active ? "YES" : "NO",
            s_debug.fallback_completed ? "YES" : "NO",
            s_debug.tripwire_details[0] ? s_debug.tripwire_details : "CLEAR",
            s_debug.last_report.probe_id[0] ? s_debug.last_report.probe_id : "none",
            s_debug.last_report.test_suite_ref[0] ? s_debug.last_report.test_suite_ref : "NONE",
            s_debug.traps_triggered, fields->turn, hash);
    } else {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "               STATE DIAGNOSTIC CARD: %s\n"
            "============================================================\n"
            "Hero: %s | Turn: %d | HP: %d/%d | Sway: %+d\n"
            "Shield: %d | Sword: %d | Stance: %d | Location: %d\n"
            "State Hash: 0x%08X\n"
            "============================================================\n",
            id, fields->hero_name, fields->turn, fields->hp, fields->max_hp,
            fields->moral_sway, fields->shield_power, fields->sword_edge,
            fields->stance, fields->location, hash);
    }

    return (written > 0 && (size_t)written < buf_len);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

bool debug_init(const debug_config_t *config)
{
    /* Step 1: Preserve existing registered validation probes across soft re-init */
    debug_validation_probe_t saved_probes[DEBUG_MAX_VALIDATION_PROBES];
    size_t saved_probe_count = s_debug.probe_count;
    if (saved_probe_count > DEBUG_MAX_VALIDATION_PROBES) {
        saved_probe_count = DEBUG_MAX_VALIDATION_PROBES;
    }
    if (saved_probe_count > 0U) {
        memcpy(saved_probes, s_debug.probes, saved_probe_count * sizeof(debug_validation_probe_t));
    }

    /* Step 2: Resolve configuration profile (caller, disk VFS, or ROM Safe Mode) */
    debug_config_t cfg;
    if (config != NULL) {
        cfg = *config;
    } else {
        cfg = debug_get_default_config();
    }

    /* Step 3: Enforce Layer 0 static ring bounds and policy defaults */
    if (cfg.max_snapshots == 0U || cfg.max_snapshots > DEBUG_MAX_SNAPSHOTS) {
        cfg.max_snapshots = DEBUG_MAX_SNAPSHOTS;
    }
    if (cfg.snapshot_interval_turns <= 0) {
        cfg.snapshot_interval_turns = 1;
    }
    if (cfg.locked_slot_quota == 0U || cfg.locked_slot_quota >= cfg.max_snapshots) {
        cfg.locked_slot_quota = (cfg.max_snapshots > DEBUG_FALLBACK_LOCKED_SLOT_QUOTA)
                                    ? DEBUG_FALLBACK_LOCKED_SLOT_QUOTA
                                    : 1U;
    }

    /* Step 4: Zero-initialize subsystem storage and apply configuration */
    memset(&s_debug, 0, sizeof(s_debug));
    s_debug.config = cfg;
    s_debug.next_snapshot_id = 1ULL;
    s_debug.next_breakpoint_id = 1;
    s_debug.step_mode = DEBUG_STEP_RUN;
    s_debug.tripwire_status = DEBUG_TRIPWIRE_CLEAR;
    s_debug.safe_mode = !cfg.loaded_from_data;

    if (saved_probe_count > 0U) {
        memcpy(s_debug.probes, saved_probes, saved_probe_count * sizeof(debug_validation_probe_t));
        s_debug.probe_count = saved_probe_count;
    }

    /* Step 5: Load canonical tripwire reaction matrix from VFS or compiled-in ROM */
    (void)debug_load_tripwire_matrix(
        s_debug.rules,
        DEBUG_FALLBACK_MAX_TRIPWIRE_RULES,
        &s_debug.rule_count
    );

    debug_fallback_shutdown();
    if (!cfg.loaded_from_data) {
        (void)debug_fallback_boot(&cfg, sizeof(cfg));
    }

    s_debug_active = true;
    s_debug_data_driven = cfg.loaded_from_data;

    LOG_INFO(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.0f,
             "Debugger initialized (Profile: %s, DataDriven: %s, SafeMode: %s, MaxSnaps: %zu, LockQuota: %zu, Rules: %zu)",
             cfg.profile_name,
             cfg.loaded_from_data ? "YES" : "NO",
             s_debug.safe_mode ? "YES" : "NO",
             cfg.max_snapshots,
             cfg.locked_slot_quota,
             s_debug.rule_count);

    return true;
}

void debug_shutdown(void)
{
    if (s_debug_active) {
        LOG_INFO(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.0f,
                 "Debugger shutting down cleanly (Snaps=%llu, Traps=%u, ProbeFailures=%u)",
                 (unsigned long long)s_debug.total_snapshots_captured,
                 s_debug.traps_triggered,
                 s_debug.probe_failures);
    }

    s_debug_active = false;
    s_debug_data_driven = false;
    s_debug.await_active = false;
    s_debug.fallback_completed = false;
    debug_fallback_shutdown();
}
