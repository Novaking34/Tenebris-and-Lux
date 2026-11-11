/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          debug_fallback.c
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap Engine Debugger & Validation Gatekeeper
@role          Tier-0 BIOS Safe-Mode Debugger Code Page, Shared JSON Parser & ROM Vault
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Static .rodata ROM & .bss fault state: Never mutates active s_debug
                  buffers during preflight or audit checks.
               4. Provenance invariant: Monolith & ROM Safe-Mode templates stamp
                  loaded_from_data = false unconditionally.
               5. Fault isolation: Triggered fail-tests complete interrupted work in
                  Safe Mode first and latch AWAIT until validated recovery completes.
               6. Zero double-slash comments (ISO C-style comments exclusively).

PURPOSE & ARCHITECTURAL ROLE:
  Implements the quarantined Tier-0 Safe-Mode Debugger BIOS, static emergency
  config templates, unified zero-heap JSON parsers shared with debug.c,
  compiled-in .rodata diagnostics/taxonomy ROM vault, and Safe-Mode fault/AWAIT
  flight-box recording backed directly by log_fallback_record().

  ### For Game Designers & Non-Programmers:
  Keeps the engine's emergency pause, rewind memory, and diagnostic cards alive
  even on read-only disks or when external JSON files are deleted. When a safety
  check fails, this Safe-Mode backup finishes the broken step harmlessly and
  freezes in AWAIT while recording exact clues into the BIOS flight log.

  ### For Systems Programmers & Engine Architects:
  - Body Block Sub-Assembly parity with log_fallback.c:
    * 3.1 Static Constructors & Shared JSON Profile/Policy/Matrix Parsers
    * 3.2 Compiled-In .rodata Debugger & Taxonomy ROM Vault
    * 3.3 Tier-0 Fault Capture, Fallback Finishing & AWAIT Latch
  - Logs all Tier-0 Safe-Mode events strictly via log_fallback_record() so
    debug_fallback.c never induces circular primary-logger trap recursion.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug_fallback
================================================================================
Mission: 3-tier downward include hierarchy, compile-time emergency templates,
static BIOS fault/AWAIT registers, and pure non-business parser helpers.
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
#include <stdio.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug_diagnostics_rom.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Lookup Maps
--------------------------------------------------------------------------------
*/

#define MODULE_ID                DEBUG_FALLBACK_MODULE_ID
#define LOCAL_LOG_CAT            LOG_CAT_DEBUG
#define LOCAL_MODULE_TAG         "DebugFallback"
#define DEBUG_FB_PREFLIGHT_TAG   "core.util.debug_fallback"
#define DEBUG_FB_MAX_SLICE_BUF   2048U

typedef struct debug_fb_reaction_token {
    const char                *token;
    debug_tripwire_reaction_t  reaction;
} debug_fb_reaction_token_t;

static const debug_fb_reaction_token_t s_fb_reaction_tokens[] = {
    { "LOG_AND_CONTINUE", DEBUG_REACTION_LOG_AND_CONTINUE },
    { "SATURATE_CLAMP",   DEBUG_REACTION_SATURATE_CLAMP   },
    { "FALLBACK_MODE",    DEBUG_REACTION_FALLBACK_MODE    },
    { "AUTO_REWIND",      DEBUG_REACTION_AUTO_REWIND      },
    { "PANIC_HALT",       DEBUG_REACTION_PANIC_HALT       }
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

typedef struct debug_fb_state {
    bool     booted;
    bool     safe_mode_engaged;
    bool     fallback_completed;
    bool     await_active;
    bool     rom_hydrated;
    bool     in_bios_log;
    bool     in_audit;
    uint32_t fault_count;
    uint32_t recovery_count;
    char     last_subsystem[32];
    char     last_condition[128];
    char     last_source[96];
    char     last_test_ref[64];
    char     last_action_taken[96];
    char     active_rom_profile[32];
} debug_fb_state_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module Templates (.rodata) & State Storage (.bss)
--------------------------------------------------------------------------------
*/

/**
 * @brief Immutable baseline fallback template preserving all contract constants.
 */
static const debug_config_t s_debug_fallback_template = {
    .enabled = true,
    .break_on_error = DEBUG_FALLBACK_BREAK_ON_ERROR,
    .break_on_tripwire = DEBUG_FALLBACK_BREAK_ON_TRIPWIRE,
    .break_on_sway_breach = DEBUG_FALLBACK_BREAK_ON_SWAY,
    .break_on_resonance_breach = DEBUG_FALLBACK_BREAK_ON_RESONANCE,
    .break_on_hp_zero = DEBUG_FALLBACK_BREAK_ON_HP_ZERO,
    .break_on_stance_change = DEBUG_FALLBACK_BREAK_ON_STANCE,
    .max_snapshots = DEBUG_FALLBACK_MAX_SNAPSHOTS,
    .snapshot_interval_turns = DEBUG_FALLBACK_INTERVAL_TURNS,
    .auto_dump_on_tripwire = DEBUG_FALLBACK_AUTO_DUMP,
    .interactive_repl = DEBUG_FALLBACK_INTERACTIVE_REPL,
    .ansi_color = DEBUG_FALLBACK_ANSI_COLOR,
    .trace_channels = DEBUG_FALLBACK_TRACE_CHANNELS,
    .profile_name = DEBUG_FALLBACK_PROFILE_NAME,
    .loaded_from_data = false,
    .engine_traps_active = DEBUG_FALLBACK_ENGINE_TRAPS_ACTIVE,
    .game_traps_active = DEBUG_FALLBACK_GAME_TRAPS_ACTIVE,
    .locked_slot_quota = DEBUG_FALLBACK_LOCKED_SLOT_QUOTA,
    .verify_on_step = DEBUG_FALLBACK_VERIFY_ON_STEP,
    .auto_await_on_tripwire = DEBUG_FALLBACK_AUTO_AWAIT
};

/**
 * @brief Compact 1.44 MB Floppy Safe-Mode template minimizing memory/IO overhead.
 */
static const debug_config_t s_debug_floppy_safe_template = {
    .enabled = true,
    .break_on_error = true,
    .break_on_tripwire = true,
    .break_on_sway_breach = true,
    .break_on_resonance_breach = true,
    .break_on_hp_zero = true,
    .break_on_stance_change = false,
    .max_snapshots = DEBUG_FALLBACK_FLOPPY_SNAPSHOTS,
    .snapshot_interval_turns = 1,
    .auto_dump_on_tripwire = true,
    .interactive_repl = false,
    .ansi_color = false,
    .trace_channels = "sim,tripwire",
    .profile_name = DEBUG_FALLBACK_COMPACT_PROFILE_NAME,
    .loaded_from_data = false,
    .engine_traps_active = true,
    .game_traps_active = true,
    .locked_slot_quota = 2U,
    .verify_on_step = true,
    .auto_await_on_tripwire = true
};

/** @brief Tier-0 BIOS Safe-Mode fault/AWAIT ledger residing in static .bss. */
static debug_fb_state_t s_fb = {
    .booted = false,
    .safe_mode_engaged = false,
    .fallback_completed = false,
    .await_active = false,
    .rom_hydrated = false,
    .in_bios_log = false,
    .fault_count = 0U,
    .recovery_count = 0U,
    .last_subsystem = "none",
    .last_condition = "No active Safe-Mode faults captured.",
    .last_source = "none:0",
    .last_test_ref = "NONE",
    .last_action_taken = "NONE",
    .active_rom_profile = "none"
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Fast Safe-Mode Helpers
--------------------------------------------------------------------------------
*/

/**
 * @brief Writes bounded error diagnostic into optional caller buffer.
 */
static inline void fb_set_err(char *err_buf, size_t err_size, const char *msg)
{
    if (err_buf != NULL && err_size > 0U && msg != NULL) {
        str_safe_copy(err_buf, msg, err_size);
    }
}

/**
 * @brief Converts tripwire_matrix.json reaction token into strongly typed enum.
 */
static bool fb_reaction_from_string(const char *str, debug_tripwire_reaction_t *out_reaction)
{
    if (str == NULL || out_reaction == NULL) {
        return false;
    }

    size_t count = sizeof(s_fb_reaction_tokens) / sizeof(s_fb_reaction_tokens[0]);
    for (size_t i = 0U; i < count; i++) {
        if (str_case_eq(str, s_fb_reaction_tokens[i].token)) {
            *out_reaction = s_fb_reaction_tokens[i].reaction;
            return true;
        }
    }

    return false;
}

/**
 * @brief Emits a guarded Tier-0 BIOS diagnostic note into log_fallback_record().
 */
static void fb_emit_bios_log(
    log_severity_t sev,
    const char    *file,
    int            line,
    const char    *func,
    const char    *subsystem,
    const char    *detail
) {
    if (s_fb.in_bios_log || s_fb.in_audit) {
        return;
    }

    s_fb.in_bios_log = true;
    (void)log_fallback_record(
        sev,
        LOCAL_LOG_CAT,
        (subsystem != NULL) ? subsystem : MODULE_ID,
        0.0f,
        file,
        line,
        func,
        "[DEBUG_BIOS] %s",
        (detail != NULL) ? detail : "No details"
    );
    s_fb.in_bios_log = false;
}

/**
 * @brief Extracts clean bounded JSON slice for an embedded sub-object key if present.
 */
static const char *fb_select_object_slice(
    const char *parent_scope,
    const char *sub_key,
    char       *scratch,
    size_t      scratch_cap
) {
    const char *slice_ptr = NULL;
    size_t slice_len = 0U;

    if (parent_scope == NULL || scratch == NULL || scratch_cap == 0U) {
        return parent_scope;
    }

    if (sub_key != NULL &&
        json_find_object(parent_scope, sub_key, &slice_ptr, &slice_len) &&
        slice_len > 0U && slice_len < scratch_cap) {
        memcpy(scratch, slice_ptr, slice_len);
        scratch[slice_len] = '\0';
        return scratch;
    }

    return parent_scope;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug_fallback
================================================================================
Mission: Construct static Safe-Mode templates, parse shared debugger/taxonomy
JSON payloads, resolve embedded .rodata ROM backups, and track fault/AWAIT state.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Static Emergency Constructors & Shared JSON Parsers
Mission: Guarantee baseline construction and eliminate parser duplication across
disk and compiled-in ROM loading paths.
--------------------------------------------------------------------------------
*/

void debug_get_monolith_fallback_config_raw(void *out_config, size_t config_size)
{
    /* Step 1: Validate caller buffer and exact struct byte size */
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return;
    }

    /* Step 2: Copy immutable .rodata Safe-Mode template atomically */
    *(debug_config_t *)out_config = s_debug_fallback_template;
}

void debug_get_floppy_safe_config_raw(void *out_config, size_t config_size)
{
    /* Step 1: Validate caller buffer and exact struct byte size */
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return;
    }

    /* Step 2: Copy compact 1.44 MB Floppy Safe-Mode template atomically */
    *(debug_config_t *)out_config = s_debug_floppy_safe_template;
}

bool debug_fallback_parse_config_json(
    const char       *scope,
    const char       *profile_id,
    bool              from_data,
    void             *out_config,
    size_t            config_size,
    debug_tripwire_t *out_tripwire,
    char             *err_buf,
    size_t            err_size
) {
    /* Step 1: Precondition checks on scope pointer and destination struct size */
    if (scope == NULL || out_config == NULL || config_size != sizeof(debug_config_t)) {
        if (out_tripwire != NULL) {
            *out_tripwire = DEBUG_TRIPWIRE_STATE_CORRUPT;
        }
        fb_set_err(err_buf, err_size, "Invalid argument or size mismatch in profile JSON parser.");
        return false;
    }

    debug_config_t *cfg = (debug_config_t *)out_config;
    char profile_slice[DEBUG_FB_MAX_SLICE_BUF];
    char id_buf[32];
    int int_val = 0;
    size_t sz_val = 0U;
    bool b_val = false;

    /* Step 2: Scope strictly to nested "profile" sub-object if present */
    const char *obj = fb_select_object_slice(scope, "profile", profile_slice, sizeof(profile_slice));

    /* Step 3: Baseline configuration cleanly before overlaying parsed keys */
    *cfg = s_debug_fallback_template;
    cfg->loaded_from_data = from_data;

    if (json_get_string(obj, "id", id_buf, sizeof(id_buf)) && id_buf[0] != '\0') {
        str_safe_copy(cfg->profile_name, id_buf, sizeof(cfg->profile_name));
    } else if (profile_id != NULL && profile_id[0] != '\0') {
        str_safe_copy(cfg->profile_name, profile_id, sizeof(cfg->profile_name));
    }

    /* Step 4: Hydrate core boolean switches and breakpoint triggers */
    if (json_get_bool(obj, "enabled", &b_val)) {
        cfg->enabled = b_val;
    }
    if (json_get_bool(obj, "break_on_error", &b_val)) {
        cfg->break_on_error = b_val;
    }
    if (json_get_bool(obj, "break_on_tripwire", &b_val)) {
        cfg->break_on_tripwire = b_val;
    }
    if (json_get_bool(obj, "break_on_sway_breach", &b_val)) {
        cfg->break_on_sway_breach = b_val;
    }
    if (json_get_bool(obj, "break_on_resonance_breach", &b_val)) {
        cfg->break_on_resonance_breach = b_val;
    }
    if (json_get_bool(obj, "break_on_hp_zero", &b_val)) {
        cfg->break_on_hp_zero = b_val;
    }
    if (json_get_bool(obj, "break_on_stance_change", &b_val)) {
        cfg->break_on_stance_change = b_val;
    }

    /* Step 5: Hydrate bounded ring capacity & snapshot intervals with bounds check */
    if (json_get_size_t(obj, "max_snapshots", &sz_val)) {
        if (sz_val == 0U || sz_val > DEBUG_MAX_SNAPSHOTS) {
            if (out_tripwire != NULL) {
                *out_tripwire = DEBUG_TRIPWIRE_STATE_CORRUPT;
            }
            fb_set_err(err_buf, err_size,
                       "Profile max_snapshots outside static ring bounds [1, 64].");
            fb_emit_bios_log(
                LOG_SEV_WARNING,
                __FILE__,
                __LINE__,
                __func__,
                MODULE_ID,
                "Rejected debug profile due to max_snapshots bounds violation."
            );
            return false;
        }
        cfg->max_snapshots = sz_val;
    }

    if (json_get_int(obj, "snapshot_interval_turns", &int_val)) {
        if (int_val < 0 || int_val > DEBUG_MAX_SNAPSHOTS) {
            if (out_tripwire != NULL) {
                *out_tripwire = DEBUG_TRIPWIRE_STATE_CORRUPT;
            }
            fb_set_err(err_buf, err_size,
                       "Profile snapshot_interval_turns negative or exceeds max capacity.");
            return false;
        }
        cfg->snapshot_interval_turns = int_val;
    }

    if (json_get_bool(obj, "auto_dump_on_tripwire", &b_val)) {
        cfg->auto_dump_on_tripwire = b_val;
    }
    if (json_get_bool(obj, "interactive_repl", &b_val)) {
        cfg->interactive_repl = b_val;
    }
    if (json_get_bool(obj, "ansi_color", &b_val)) {
        cfg->ansi_color = b_val;
    }
    (void)json_get_string(obj, "trace_channels", cfg->trace_channels, sizeof(cfg->trace_channels));

    /* Step 6: Hydrate nested domain_traps object gates */
    if (json_get_object_bool(obj, "domain_traps", "engine_traps_active", &b_val)) {
        cfg->engine_traps_active = b_val;
    }
    if (json_get_object_bool(obj, "domain_traps", "game_traps_active", &b_val)) {
        cfg->game_traps_active = b_val;
    }

    if (out_tripwire != NULL) {
        *out_tripwire = DEBUG_TRIPWIRE_CLEAR;
    }
    return true;
}

bool debug_fallback_parse_snapshot_policy_json(
    const char *scope,
    void       *out_config,
    size_t      config_size,
    char       *err_buf,
    size_t      err_size
) {
    if (scope == NULL || out_config == NULL || config_size != sizeof(debug_config_t)) {
        fb_set_err(err_buf, err_size, "Invalid argument in snapshot policy parser.");
        return false;
    }

    debug_config_t *cfg = (debug_config_t *)out_config;
    char policy_slice[DEBUG_FB_MAX_SLICE_BUF];
    size_t ring_cap = 0U;
    size_t lock_quota = 0U;

    const char *obj = fb_select_object_slice(scope, "policy", policy_slice, sizeof(policy_slice));

    if (json_get_size_t(obj, "ring_buffer_capacity", &ring_cap)) {
        if (ring_cap == 0U || ring_cap > DEBUG_MAX_SNAPSHOTS) {
            fb_set_err(err_buf, err_size,
                       "snapshot_policy ring_buffer_capacity outside [1, 64].");
            return false;
        }
        if (cfg->max_snapshots > ring_cap) {
            cfg->max_snapshots = ring_cap;
        }
    }

    if (json_get_size_t(obj, "locked_slot_quota", &lock_quota)) {
        if (lock_quota > cfg->max_snapshots || lock_quota > 16U) {
            fb_set_err(err_buf, err_size,
                       "snapshot_policy locked_slot_quota exceeds safe ring threshold.");
            return false;
        }
        cfg->locked_slot_quota = lock_quota;
    }

    return true;
}

bool debug_fallback_parse_tripwire_matrix_json(
    const char                     *scope,
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count,
    char                           *err_buf,
    size_t                          err_size
) {
    if (scope == NULL || out_rules == NULL || max_rules == 0U || out_count == NULL) {
        fb_set_err(err_buf, err_size, "Invalid argument in tripwire_matrix parser.");
        return false;
    }

    *out_count = 0U;

    const char *arr_start = NULL;
    size_t arr_len = 0U;
    if (!json_find_array(scope, "tripwires", &arr_start, &arr_len)) {
        fb_set_err(err_buf, err_size, "Missing 'tripwires' array in tripwire_matrix JSON.");
        return false;
    }

    size_t total_elems = json_array_get_count(arr_start, arr_len);
    size_t limit = (total_elems < max_rules) ? total_elems : max_rules;
    size_t parsed = 0U;

    for (size_t i = 0U; i < limit; i++) {
        char item_obj[1024];
        char react_str[32];

        if (!json_array_get_object(arr_start, arr_len, i, item_obj, sizeof(item_obj))) {
            continue;
        }

        debug_fallback_tripwire_rule_t *rule = &out_rules[parsed];
        memset(rule, 0, sizeof(*rule));

        if (!json_get_string(item_obj, "tripwire_id", rule->id, sizeof(rule->id))) {
            continue;
        }

        (void)json_get_string(item_obj, "bound_error", rule->bound_error, sizeof(rule->bound_error));
        (void)json_get_string(item_obj, "trigger_condition", rule->trigger_condition,
                              sizeof(rule->trigger_condition));
        (void)json_get_string(item_obj, "fallback_action", rule->fallback_action,
                              sizeof(rule->fallback_action));

        if (json_get_string(item_obj, "reaction", react_str, sizeof(react_str))) {
            if (!fb_reaction_from_string(react_str, &rule->reaction)) {
                rule->reaction = DEBUG_REACTION_FALLBACK_MODE;
            }
        } else {
            rule->reaction = DEBUG_REACTION_FALLBACK_MODE;
        }

        (void)json_get_bool(item_obj, "tripwire_latch", &rule->tripwire_latch);
        (void)json_get_bool(item_obj, "is_recoverable", &rule->is_recoverable);
        parsed++;
    }

    *out_count = parsed;
    if (parsed == 0U) {
        fb_set_err(err_buf, err_size, "No valid rules extracted from tripwire_matrix array.");
        return false;
    }

    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Compiled-In .rodata Debugger & Taxonomy ROM Vault
Mission: Resolve and hydrate embedded JSON profiles, policies, and tripwire
reaction rules directly from immutable .rodata memory with zero disk I/O.
--------------------------------------------------------------------------------
*/

size_t debug_fallback_rom_count(void)
{
    return (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT;
}

const char *debug_fallback_rom_asset_at(size_t index, size_t *out_size)
{
    if (index >= (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT) {
        if (out_size != NULL) {
            *out_size = 0U;
        }
        return NULL;
    }

    const debug_rom_asset_t *asset = &s_debug_diagnostics_rom_table[index];
    if (out_size != NULL) {
        *out_size = asset->size;
    }
    return (const char *)asset->data;
}

const char *debug_fallback_find_rom_asset(const char *id_or_rel_path, size_t *out_size)
{
    if (id_or_rel_path == NULL || id_or_rel_path[0] == '\0') {
        if (out_size != NULL) {
            *out_size = 0U;
        }
        return NULL;
    }

    /* Step 1: Prefer exact ID ("debugger/default") or relative path match */
    for (size_t i = 0U; i < (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT; i++) {
        const debug_rom_asset_t *asset = &s_debug_diagnostics_rom_table[i];
        if (str_case_eq(asset->id, id_or_rel_path) ||
            str_case_eq(asset->rel_path, id_or_rel_path)) {
            if (out_size != NULL) {
                *out_size = asset->size;
            }
            return (const char *)asset->data;
        }
    }

    /* Step 2: Allow unambiguous leaf stem lookup ("default", "snapshot_policy", "tripwire_matrix") */
    for (size_t i = 0U; i < (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT; i++) {
        const debug_rom_asset_t *asset = &s_debug_diagnostics_rom_table[i];
        const char *slash = strrchr(asset->id, '/');
        const char *leaf = (slash != NULL) ? (slash + 1) : asset->id;
        if (str_case_eq(leaf, id_or_rel_path)) {
            if (out_size != NULL) {
                *out_size = asset->size;
            }
            return (const char *)asset->data;
        }
    }

    if (out_size != NULL) {
        *out_size = 0U;
    }
    return NULL;
}

bool debug_fallback_load_rom_profile(
    const char *profile_id,
    void       *out_config,
    size_t      config_size
) {
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return false;
    }

    const char *target = (profile_id != NULL && profile_id[0] != '\0')
                             ? profile_id
                             : "default";

    char qualified_id[64];
    snprintf(qualified_id, sizeof(qualified_id), "debugger/%s", target);

    size_t raw_size = 0U;
    const char *raw_json = debug_fallback_find_rom_asset(qualified_id, &raw_size);
    if (raw_json == NULL) {
        raw_json = debug_fallback_find_rom_asset(target, &raw_size);
    }
    if (raw_json == NULL || raw_size == 0U) {
        return false;
    }

    master_loader_t loader;
    if (!master_loader_load_from_memory(&loader, raw_json, raw_size) ||
        loader.body_start == NULL) {
        return false;
    }

    debug_tripwire_t tw = DEBUG_TRIPWIRE_CLEAR;
    char err_msg[128];
    if (!debug_fallback_parse_config_json(
            loader.body_start,
            target,
            false,
            out_config,
            config_size,
            &tw,
            err_msg,
            sizeof(err_msg))) {
        return false;
    }

    /* Overlay canonical snapshot_policy.json quotas from ROM cleanly */
    (void)debug_fallback_load_rom_snapshot_policy(out_config, config_size);

    /* Ensure ROM provenance stays explicitly marked non-disk */
    ((debug_config_t *)out_config)->loaded_from_data = false;

    s_fb.rom_hydrated = true;
    str_safe_copy(s_fb.active_rom_profile,
                  ((const debug_config_t *)out_config)->profile_name,
                  sizeof(s_fb.active_rom_profile));

    fb_emit_bios_log(
        LOG_SEV_INFO,
        __FILE__,
        __LINE__,
        __func__,
        MODULE_ID,
        s_fb.active_rom_profile
    );
    return true;
}

bool debug_fallback_load_rom_snapshot_policy(void *out_config, size_t config_size)
{
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return false;
    }

    size_t raw_size = 0U;
    const char *raw_json = debug_fallback_find_rom_asset("debugger/snapshot_policy", &raw_size);
    if (raw_json == NULL || raw_size == 0U) {
        return false;
    }

    master_loader_t loader;
    if (!master_loader_load_from_memory(&loader, raw_json, raw_size) ||
        loader.body_start == NULL) {
        return false;
    }

    char err_msg[128];
    return debug_fallback_parse_snapshot_policy_json(
        loader.body_start,
        out_config,
        config_size,
        err_msg,
        sizeof(err_msg)
    );
}

bool debug_fallback_load_rom_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
) {
    if (out_rules == NULL || max_rules == 0U || out_count == NULL) {
        return false;
    }

    size_t raw_size = 0U;
    const char *raw_json = debug_fallback_find_rom_asset("taxonomy/tripwire_matrix", &raw_size);
    if (raw_json == NULL || raw_size == 0U) {
        return false;
    }

    master_loader_t loader;
    if (!master_loader_load_from_memory(&loader, raw_json, raw_size) ||
        loader.body_start == NULL) {
        return false;
    }

    char err_msg[128];
    return debug_fallback_parse_tripwire_matrix_json(
        loader.body_start,
        out_rules,
        max_rules,
        out_count,
        err_msg,
        sizeof(err_msg)
    );
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Tier-0 Safe-Mode Fault Capture, Fallback Finishing & AWAIT
Mission: Record exact failure context into Safe-Mode RAM, execute downgraded
completion cleanly, and hold idle AWAIT until validated state recovery clears it.
--------------------------------------------------------------------------------
*/

bool debug_fallback_record_fault(
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *test_ref
) {
    if (condition == NULL || condition[0] == '\0') {
        return false;
    }

    s_fb.booted = true;
    s_fb.fault_count++;
    s_fb.fallback_completed = false;

    str_safe_copy(s_fb.last_subsystem,
                  (subsystem != NULL && subsystem[0] != '\0') ? subsystem : "unknown",
                  sizeof(s_fb.last_subsystem));
    str_safe_copy(s_fb.last_condition, condition, sizeof(s_fb.last_condition));
    snprintf(s_fb.last_source, sizeof(s_fb.last_source), "%s:%d",
             (file != NULL) ? file : "unknown", line);
    str_safe_copy(s_fb.last_test_ref,
                  (test_ref != NULL && test_ref[0] != '\0') ? test_ref : "BLOCK4_AUDIT",
                  sizeof(s_fb.last_test_ref));

    fb_emit_bios_log(
        LOG_SEV_ERROR,
        (file != NULL) ? file : __FILE__,
        line,
        __func__,
        s_fb.last_subsystem,
        s_fb.last_condition
    );
    return true;
}

bool debug_fallback_finish_with_fallback(
    const char *subsystem,
    const char *fallback_action
) {
    s_fb.booted = true;
    s_fb.safe_mode_engaged = true;
    s_fb.fallback_completed = true;
    s_fb.await_active = true;

    if (subsystem != NULL && subsystem[0] != '\0') {
        str_safe_copy(s_fb.last_subsystem, subsystem, sizeof(s_fb.last_subsystem));
    }
    str_safe_copy(s_fb.last_action_taken,
                  (fallback_action != NULL && fallback_action[0] != '\0')
                      ? fallback_action
                      : "Engaged Safe-Mode fallback completion and entered idle AWAIT",
                  sizeof(s_fb.last_action_taken));

    fb_emit_bios_log(
        LOG_SEV_WARNING,
        __FILE__,
        __LINE__,
        __func__,
        s_fb.last_subsystem,
        s_fb.last_action_taken
    );
    return true;
}

bool debug_fallback_is_awaiting(void)
{
    return s_fb.await_active;
}

bool debug_fallback_clear_await(const char *resolution_note)
{
    s_fb.await_active = false;
    s_fb.fallback_completed = false;
    s_fb.recovery_count++;

    str_safe_copy(s_fb.last_action_taken,
                  (resolution_note != NULL && resolution_note[0] != '\0')
                      ? resolution_note
                      : "Validated recovery cleared Safe-Mode AWAIT.",
                  sizeof(s_fb.last_action_taken));

    fb_emit_bios_log(
        LOG_SEV_INFO,
        __FILE__,
        __LINE__,
        __func__,
        s_fb.last_subsystem,
        s_fb.last_action_taken
    );
    return true;
}

bool debug_init_emergency_monolith(void)
{
    debug_config_t cfg;
    if (!debug_fallback_load_rom_profile("default", &cfg, sizeof(cfg))) {
        debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));
    }

    cfg.loaded_from_data = false;
    (void)debug_fallback_boot(&cfg, sizeof(cfg));
    s_fb.safe_mode_engaged = true;
    return debug_init(&cfg);
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug_fallback
================================================================================
Mission: Verify compile-time monolith parameters, floppy Safe-Mode presets,
and every embedded diagnostics/taxonomy ROM asset without side effects.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwires & Audits
Mission: Assert bounds invariants, !loaded_from_data contracts, and ROM parity.
--------------------------------------------------------------------------------
*/

diag_result_t debug_fallback_preflight(void)
{
    static char s_preflight_details[256];

    diag_result_t res;
    memset(&res, 0, sizeof(res));
    res.subsystem = DEBUG_FB_PREFLIGHT_TAG;
    res.check_name = "Debugger Fallback BIOS & ROM Pre-Flight Check";
    res.severity = SEVERITY_PASS;
    res.passed = true;

    /* Step 1: Retrieve static compile-time monolith template cleanly */
    debug_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));

    /* Step 2: Validate invariant provenance contract (unconditionally false) */
    if (cfg.loaded_from_data) {
        res.severity = SEVERITY_FATAL;
        res.passed = false;
        res.details = "PREFLIGHT BREACH: Fallback loaded_from_data invariant violated.";
        return res;
    }

    /* Step 3: Verify canonical profile identifier string */
    if (strcmp(cfg.profile_name, DEBUG_FALLBACK_PROFILE_NAME) != 0) {
        res.severity = SEVERITY_CRITICAL;
        res.passed = false;
        res.details = "PREFLIGHT BREACH: Fallback profile name mismatch.";
        return res;
    }

    /* Step 4: Verify snapshot capacity and quota bounds */
    if (cfg.max_snapshots == 0U ||
        cfg.max_snapshots > DEBUG_MAX_SNAPSHOTS ||
        cfg.locked_slot_quota > cfg.max_snapshots) {
        res.severity = SEVERITY_CRITICAL;
        res.passed = false;
        res.details = "PREFLIGHT BREACH: Snapshot ring capacity or lock quota out of bounds.";
        return res;
    }

    /* Step 5: Emit healthy preflight status summary */
    snprintf(
        s_preflight_details,
        sizeof(s_preflight_details),
        "Fallback preflight verified: profile='%s', max_snaps=%zu, rom_assets=%zu (%u B)",
        cfg.profile_name,
        cfg.max_snapshots,
        debug_fallback_rom_count(),
        (unsigned)DEBUG_DIAGNOSTICS_ROM_TOTAL_BYTES
    );
    res.details = s_preflight_details;

    return res;
}

bool debug_fallback_audit(void)
{
    /* Step 1: Execute core template preflight invariants */
    diag_result_t res = debug_fallback_preflight();
    if (!res.passed) {
        return false;
    }

    /* Step 2: Verify compact 1.44 MB Floppy Safe-Mode template invariants */
    debug_config_t floppy_cfg;
    debug_get_floppy_safe_config_raw(&floppy_cfg, sizeof(floppy_cfg));
    if (floppy_cfg.loaded_from_data ||
        floppy_cfg.max_snapshots == 0U ||
        floppy_cfg.max_snapshots > DEBUG_MAX_SNAPSHOTS ||
        strcmp(floppy_cfg.profile_name, DEBUG_FALLBACK_COMPACT_PROFILE_NAME) != 0) {
        return false;
    }

    /* Step 3: Validate all 9 compiled-in .rodata ROM JSON envelopes */
    for (size_t i = 0U; i < (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT; i++) {
        size_t sz = 0U;
        const char *json = debug_fallback_rom_asset_at(i, &sz);
        if (json == NULL || sz == 0U) {
            return false;
        }
        master_loader_t loader;
        if (!master_loader_load_from_memory(&loader, json, sz) ||
            !loader.header.is_5block ||
            loader.body_start == NULL) {
            return false;
        }
    }

    /* Step 4: Verify ROM profile, policy, and tripwire matrix hydration without mutating s_fb */
    const bool prev_rom_hydrated = s_fb.rom_hydrated;
    char prev_rom_prof[32];
    str_safe_copy(prev_rom_prof, s_fb.active_rom_profile, sizeof(prev_rom_prof));
    s_fb.in_audit = true;

    const char *profiles[3] = { "default", "strict", "trace" };
    for (size_t p = 0U; p < 3U; p++) {
        debug_config_t parsed;
        if (!debug_fallback_load_rom_profile(profiles[p], &parsed, sizeof(parsed)) ||
            parsed.loaded_from_data) {
            s_fb.in_audit = false;
            s_fb.rom_hydrated = prev_rom_hydrated;
            str_safe_copy(s_fb.active_rom_profile, prev_rom_prof, sizeof(s_fb.active_rom_profile));
            return false;
        }
    }

    debug_fallback_tripwire_rule_t rules[DEBUG_FALLBACK_MAX_TRIPWIRE_RULES];
    size_t rule_count = 0U;
    if (!debug_fallback_load_rom_tripwire_matrix(rules, DEBUG_FALLBACK_MAX_TRIPWIRE_RULES, &rule_count) ||
        rule_count == 0U) {
        s_fb.in_audit = false;
        s_fb.rom_hydrated = prev_rom_hydrated;
        str_safe_copy(s_fb.active_rom_profile, prev_rom_prof, sizeof(s_fb.active_rom_profile));
        return false;
    }

    /* Step 5: Restore pre-audit Safe-Mode metadata so audit stays non-destructive */
    s_fb.in_audit = false;
    s_fb.rom_hydrated = prev_rom_hydrated;
    str_safe_copy(s_fb.active_rom_profile, prev_rom_prof, sizeof(s_fb.active_rom_profile));
    return true;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Machine: core.util.debug_fallback
================================================================================
Mission: Expose inspectable Safe-Mode telemetry objects, fixed-column diagnostic
ASCII cards, and Tier-0 BIOS boot/shutdown lifecycle hooks.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

bool debug_fallback_get_rom_info(debug_fallback_rom_info_t *out_info)
{
    if (out_info == NULL) {
        return false;
    }

    out_info->asset_count = (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT;
    out_info->total_rom_bytes = (size_t)DEBUG_DIAGNOSTICS_ROM_TOTAL_BYTES;
    str_safe_copy(out_info->active_rom_profile, s_fb.active_rom_profile,
                  sizeof(out_info->active_rom_profile));
    return true;
}

bool debug_fallback_get_stats(debug_fallback_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }

    out_stats->booted = s_fb.booted;
    out_stats->safe_mode_engaged = s_fb.safe_mode_engaged;
    out_stats->fallback_completed = s_fb.fallback_completed;
    out_stats->await_active = s_fb.await_active;
    out_stats->rom_hydrated = s_fb.rom_hydrated;
    out_stats->fault_count = s_fb.fault_count;
    out_stats->recovery_count = s_fb.recovery_count;
    str_safe_copy(out_stats->last_subsystem, s_fb.last_subsystem,
                  sizeof(out_stats->last_subsystem));
    str_safe_copy(out_stats->last_condition, s_fb.last_condition,
                  sizeof(out_stats->last_condition));
    str_safe_copy(out_stats->last_source, s_fb.last_source,
                  sizeof(out_stats->last_source));
    str_safe_copy(out_stats->last_test_ref, s_fb.last_test_ref,
                  sizeof(out_stats->last_test_ref));
    str_safe_copy(out_stats->last_action_taken, s_fb.last_action_taken,
                  sizeof(out_stats->last_action_taken));
    str_safe_copy(out_stats->active_rom_profile, s_fb.active_rom_profile,
                  sizeof(out_stats->active_rom_profile));
    return true;
}

bool debug_fallback_execute_inspect(void *out_config, size_t config_size)
{
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return false;
    }

    debug_get_monolith_fallback_config_raw(out_config, config_size);
    return true;
}

size_t debug_fallback_execute_dump_card(char *out_buf, size_t buf_size)
{
    if (out_buf == NULL || buf_size == 0U) {
        return 0U;
    }

    debug_config_t cfg;
    debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));

    const bool audit_ok = debug_fallback_audit();

    int n = snprintf(
        out_buf,
        buf_size,
        "+============================================================+\n"
        "| DEBUGGER FALLBACK BIOS & ROM TELEMETRY CARD               |\n"
        "+============================================================+\n"
        "| Fallback Profile : %-38s |\n"
        "| Active ROM Prof  : %-38s |\n"
        "| Provenance Flag  : %-38s |\n"
        "| ROM Assets       : %-3u embedded (%6u bytes)           |\n"
        "| Snapshot Ring    : Max %3zu | Interval %2d | Locks Quota %2zu |\n"
        "| Domain Traps     : Engine=%-5s | Game=%-5s              |\n"
        "| BIOS Safe Mode   : Booted=%-3s | AWAIT=%-3s | Faults=%-4u   |\n"
        "| Last Fault Src   : %-38s |\n"
        "| Last Test Ref    : %-38s |\n"
        "| Self-Audit       : %-38s |\n"
        "+============================================================+\n",
        cfg.profile_name,
        s_fb.active_rom_profile,
        cfg.loaded_from_data ? "DISK_DATA" : "HARDCODE_FALLBACK",
        (unsigned)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT,
        (unsigned)DEBUG_DIAGNOSTICS_ROM_TOTAL_BYTES,
        cfg.max_snapshots,
        cfg.snapshot_interval_turns,
        cfg.locked_slot_quota,
        cfg.engine_traps_active ? "ON" : "OFF",
        cfg.game_traps_active ? "ON" : "OFF",
        s_fb.booted ? "YES" : "NO",
        s_fb.await_active ? "YES" : "NO",
        (unsigned)s_fb.fault_count,
        s_fb.last_source,
        s_fb.last_test_ref,
        audit_ok ? "PASS" : "FAIL"
    );

    if (n < 0) {
        out_buf[0] = '\0';
        return 0U;
    }

    size_t written = (size_t)n;
    if (written >= buf_size) {
        return buf_size - 1U;
    }
    return written;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

bool debug_fallback_boot(const void *opt_config, size_t config_size)
{
    debug_config_t scratch;
    if (opt_config != NULL && config_size == sizeof(debug_config_t)) {
        const debug_config_t *in_cfg = (const debug_config_t *)opt_config;
        str_safe_copy(s_fb.active_rom_profile, in_cfg->profile_name,
                      sizeof(s_fb.active_rom_profile));
    } else if (!debug_fallback_load_rom_profile("default", &scratch, sizeof(scratch))) {
        debug_get_monolith_fallback_config_raw(&scratch, sizeof(scratch));
        str_safe_copy(s_fb.active_rom_profile, scratch.profile_name,
                      sizeof(s_fb.active_rom_profile));
    }

    s_fb.booted = true;
    fb_emit_bios_log(
        LOG_SEV_INFO,
        __FILE__,
        __LINE__,
        __func__,
        MODULE_ID,
        "Tier-0 Debugger Safe-Mode BIOS armed."
    );
    return true;
}

void debug_fallback_shutdown(void)
{
    memset(&s_fb, 0, sizeof(s_fb));
    str_safe_copy(s_fb.last_subsystem, "none", sizeof(s_fb.last_subsystem));
    str_safe_copy(s_fb.last_condition, "No active Safe-Mode faults captured.",
                  sizeof(s_fb.last_condition));
    str_safe_copy(s_fb.last_source, "none:0", sizeof(s_fb.last_source));
    str_safe_copy(s_fb.last_test_ref, "NONE", sizeof(s_fb.last_test_ref));
    str_safe_copy(s_fb.last_action_taken, "NONE", sizeof(s_fb.last_action_taken));
    str_safe_copy(s_fb.active_rom_profile, "none", sizeof(s_fb.active_rom_profile));
}

bool debug_fallback_is_booted(void)
{
    return s_fb.booted;
}
