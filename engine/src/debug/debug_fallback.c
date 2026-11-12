/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/src/debug/debug_fallback.c
@id            cog.debug.fallback
@layer         Layer 0: Core Utilities
@archetype     quarantined_fallback | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Engine Debugger Safe-Mode BIOS & Diagnostics ROM Vault
@role          Runtime Engine & Chassis: Tier-0 fault flight box, 76.1 KiB
               debugger/taxonomy ROM vault, and shared JSON schema extractors.
@maturity      Rev 5 Baseline | 9 compiled diagnostics ROMs & circular-free log_fallback flight logging active.
@chassis       zero-heap | .bss 256 B + .data 464 B + .rodata 76,136 B (ROM vault) + stack <= 1 KiB.
@scale         O(n) linear key scan across embedded JSON profiles; O(1) ROM lookup & BIOS fault latch.
@seams         in: debug_diagnostics_rom.h (generated) | uses: cog.manifest, cog.debug.types, cog.log.fallback | probes: debug_fb.* | dag: core.util.debug_fallback
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Logs Tier-0 Safe-Mode events strictly via log_fallback_record() to prevent primary logger trap recursion.
               3. Monolith & ROM templates accept debug_config_t * and stamp loaded_from_data = false without mutating live s_debug ring slots.
               4. debug_fallback_preflight() validates all 9 embedded JSON payloads cleanly with zero disk reads.

PURPOSE & ARCHITECTURAL ROLE:
  Executes the Tier-0 Safe-Mode debugger BIOS: houses immutable .rodata JSON
  ROMs for debugger, snapshot, and tripwire profiles, runs shared pure parsers,
  and latches fault/AWAIT telemetry safely onto log_fallback ring buffers.

  ### For Game Designers & Non-Programmers:
  Ensures emergency pause diagnostics, tripwire clues, and Safe-Mode recovery
  cards stay readable even on read-only media or damaged installs without dying
  inside recursive error loops.

  ### For Systems Programmers & Engine Architects:
  - Controlled Memory Chassis: .bss occupies just 256 B and .data 464 B, while
    .rodata stores 76,136 B of compiled diagnostics and taxonomy JSON assets.
  - Bottleneck & Headroom: Side-effect-free ROM verification completes during
    early boot before full VFS mounting or heavy snapshot ring activation.
  - Probe & Config Wiring: Eliminates recursive trap loops by logging solely
    through cog.log.fallback while sharing parsers upward with cog.debug.
================================================================================
*/

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/*
================================================================================
BLOCK 2: OPENING BLOCK
( BIOS Dependencies, Safe-Mode Chassis, Config ROM Vault & O(1) Guards )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug.fallback
Mission:       Wire BIOS deps and debug_diagnostics_rom.h vault, house Safe-Mode
               chassis in 2.3, and guard ROM/disk fallback.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None in Layer 0/1)
Tier 3: Internal Engine Foundation Headers & Config ROM Vault
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers & Config ROM Vault --- */
#include "debug/debug_fallback.h"
#include "core/manifest.h"
#include "debug/debug_types.h"
#include "debug_diagnostics_rom.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log_fallback.h"
#include "log/log_types.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Bounds Constants
--------------------------------------------------------------------------------
*/

/** @brief Canonical manifest identifier registering the debugger BIOS. */
#define MODULE_ID                DEBUG_FALLBACK_MODULE_ID
/** @brief Default telemetry category stamped on Safe-Mode BIOS notes. */
#define LOCAL_LOG_CAT            LOG_CAT_DEBUG
/** @brief Human-readable Safe-Mode debugger tag for fallback logging. */
#define LOCAL_MODULE_TAG         "DebugFallback"
/** @brief Pre-flight subsystem identifier checked in Block 4.2 diagnostics. */
#define DEBUG_FB_PREFLIGHT_TAG   "core.util.debug_fallback"
/** @brief Fixed stack buffer capacity (bytes) for scoped JSON sub-object scans. */
#define DEBUG_FB_MAX_SLICE_BUF   2048U
/** @brief Fixed capacity (bytes) storing the active debugger ROM profile key. */
#define DEBUG_FB_ROM_PROFILE_CAP 32U

static_assert(
    DEBUG_FB_MAX_SLICE_BUF >= 512U && DEBUG_FALLBACK_FLOPPY_SNAPSHOTS >= 4U,
    "Debugger Safe-Mode slice buffer and floppy snapshot caps must remain valid"
);
static_assert(
    DEBUG_MAX_SNAPSHOTS >= DEBUG_FALLBACK_MAX_SNAPSHOTS,
    "Primary debugger ring capacity must cover Safe-Mode baseline snapshots"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/**
 * @struct debug_fb_reaction_token
 * @brief Lookup row mapping tripwire reaction string tokens to typed enums.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects human-readable failure response keywords in config files (like
 * AUTO_REWIND or PANIC_HALT) to debugger Safe-Mode actions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Immutable token/enum pair stored in `s_fb_reaction_tokens[]` (.rodata).
 */
typedef struct debug_fb_reaction_token {
    const char                *token;    /**< Uppercase JSON reaction keyword. */
    debug_tripwire_reaction_t  reaction; /**< Mapped tripwire reaction enumeration. */
} debug_fb_reaction_token_t;

/**
 * @struct debug_fb_state
 * @brief Tier-0 BIOS Safe-Mode fault, AWAIT, and ROM provenance ledger.
 *
 * ### For Game Designers & Non-Programmers:
 * Keeps a compact black-box summary of which safety probe fired, which source
 * line tripped, and whether AWAIT pause recovery is holding cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Strictly allocated in .bss (`s_fb`) with reentrancy flags (`in_bios_log`,
 * `in_audit`) preventing primary-logger feedback recursion.
 */
typedef struct debug_fb_state {
    bool     booted;                                 /**< True after Safe-Mode boot. */
    bool     safe_mode_engaged;                      /**< True while downgraded to BIOS. */
    bool     fallback_completed;                     /**< True after recovery pass ran. */
    bool     await_active;                           /**< True when frozen in DEBUG_STEP_AWAIT. */
    bool     rom_hydrated;                           /**< True once ROM profile decoded. */
    bool     in_bios_log;                            /**< Recursion lock for log_fallback(). */
    bool     in_audit;                               /**< Audit-isolation latch suppressing logs. */
    uint32_t fault_count;                            /**< Lifetime BIOS fault latch count. */
    uint32_t recovery_count;                         /**< Successful AWAIT resume count. */
    char     last_subsystem[32];                     /**< Last faulting module identifier. */
    char     last_condition[128];                    /**< Human-readable fault condition note. */
    char     last_source[96];                        /**< Source file:line provenance label. */
    char     last_test_ref[64];                      /**< Layer 4 failing test target reference. */
    char     last_action_taken[96];                  /**< Last Safe-Mode recovery action taken. */
    char     active_rom_profile[DEBUG_FB_ROM_PROFILE_CAP]; /**< Active ROM profile name. */
} debug_fb_state_t;

/* Forward declaration binding Sub-Assembly 2.3 debug_fallback_manifest to Block 4.2. */
diag_result_t debug_fallback_preflight(void);

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

/** @brief Immutable tripwire reaction token lookup table (.rodata). */
static const debug_fb_reaction_token_t s_fb_reaction_tokens[] = {
    { "LOG_AND_CONTINUE", DEBUG_REACTION_LOG_AND_CONTINUE },
    { "SATURATE_CLAMP",   DEBUG_REACTION_SATURATE_CLAMP   },
    { "FALLBACK_MODE",    DEBUG_REACTION_FALLBACK_MODE    },
    { "AUTO_REWIND",      DEBUG_REACTION_AUTO_REWIND      },
    { "PANIC_HALT",       DEBUG_REACTION_PANIC_HALT       }
};

/** @brief Documented debugger Safe-Mode invariants bound into manifest. */
static const char *const s_debug_fb_invariants[] = {
    "ALLOC_ZERO_HEAP across Tier-0 BIOS circuit breaker, ROM vault, and parsers",
    "Tier-0 logs route strictly through log_fallback_record() preventing recursion",
    "Monolith, floppy preset, and 9 diagnostics ROMs preserve loaded_from_data = false",
    "Failed probes complete interrupted step safely then latch DEBUG_STEP_AWAIT"
};

/** @brief Public module metadata node registering debugger BIOS in manifest. */
const module_metadata_t debug_fallback_manifest = {
    DEBUG_FALLBACK_MODULE_ID,
    "debug/debug_fallback.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-11",
    "Tier-0 Safe-Mode debugger BIOS, shared policy parsers, and 9-ROM diagnostics vault",
    4,
    { "core.util.string", "core.util.json", "core.io.master_loader", "core.util.log_fallback" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    100,
    4,
    {
        s_debug_fb_invariants[0],
        s_debug_fb_invariants[1],
        s_debug_fb_invariants[2],
        s_debug_fb_invariants[3]
    },
    debug_fallback_preflight
};

/** @brief Tier-0 BIOS Safe-Mode fault/AWAIT ledger residing in static .bss. */
static debug_fb_state_t s_fb = {
    .booted = false,
    .safe_mode_engaged = false,
    .fallback_completed = false,
    .await_active = false,
    .rom_hydrated = false,
    .in_bios_log = false,
    .in_audit = false,
    .fault_count = 0U,
    .recovery_count = 0U,
    .last_subsystem = "none",
    .last_condition = "No active Safe-Mode faults captured.",
    .last_source = "none:0",
    .last_test_ref = "NONE",
    .last_action_taken = "NONE",
    .active_rom_profile = "none"
};

/** @brief Dedicated Tier-0 preflight message scratch buffer kept cleanly in .bss (256 B). */
static char s_fb_preflight_details[256];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & O(1) Safe-Mode Inlines
--------------------------------------------------------------------------------
*/

/**
 * @brief Writes bounded error diagnostic into optional caller buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely records why a Safe-Mode config or rule file could not be read without
 * overflowing text memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Null-safe bounded wrapper around `str_safe_copy()` with zero heap allocations.
 *
 * @param err_buf  Destination buffer or NULL to suppress error text.
 * @param err_size Destination byte capacity.
 * @param msg      Null-terminated error summary or NULL.
 */
static inline void fb_set_err(char *err_buf, size_t err_size, const char *msg)
{
    if (err_buf != NULL && err_size > 0U && msg != NULL) {
        str_safe_copy(err_buf, msg, err_size);
    }
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.5: Monitored BIOS Support Infrastructure (Non-Inline Internal)
--------------------------------------------------------------------------------
Support Role:  Tripwire reaction token decoding, reentrancy-guarded BIOS log
               forwarding, and scoped JSON sub-object slice extraction.
Pattern Watch: If core subsystems outside log/debug duplicate bounded JSON
               sub-object extraction (`fb_select_object_slice`), elevate a shared
               `json_extract_object_slice()` helper into engine/include/io/json_utils.h.
Linkage Rule:  Strictly static functions only; zero static arrays or .bss storage.
--------------------------------------------------------------------------------
*/

/**
 * @brief Converts tripwire_matrix.json reaction token into strongly typed enum.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads failure response names like AUTO_REWIND from diagnostic matrix files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Linearly scans immutable `s_fb_reaction_tokens[]` using case-insensitive match.
 *
 * @param str          Token text to parse.
 * @param out_reaction Output pointer receiving decoded reaction enum.
 * @return True if matched; false on invalid arguments or unknown token.
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
 *
 * ### For Game Designers & Non-Programmers:
 * Records emergency pause and recovery notices safely to the flight recorder
 * without waking the primary logger during an active fault.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrancy-guarded (`s_fb.in_bios_log` / `s_fb.in_audit`) relay into `cog.log.fallback`.
 *
 * @param sev       Log severity level.
 * @param file      Originating source file path.
 * @param line      Originating source line number.
 * @param func      Originating C function symbol.
 * @param subsystem Triggering subsystem identifier or NULL.
 * @param detail    Human-readable message text or NULL.
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
 *
 * ### For Game Designers & Non-Programmers:
 * Zooms cleanly into the relevant section ("profile" or "policy") of a config file
 * so unrelated metadata never interferes with settings parsing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies delimited object bytes into caller stack `scratch` (`< scratch_cap`) and
 * null-terminates; falls back to `parent_scope` when absent or oversized.
 *
 * @param parent_scope Null-terminated parent JSON object slice.
 * @param sub_key      Child JSON object key to locate, or NULL.
 * @param scratch      Caller stack buffer receiving copied sub-object text.
 * @param scratch_cap  Capacity of `scratch` in bytes.
 * @return Pointer to extracted slice inside `scratch`, or `parent_scope` fallback.
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
BLOCK 3: BODY BLOCK
( Safe-Mode BIOS Implementation, .rodata ROM Hydration & Fault Recording )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug.fallback
Mission:       Construct static Safe-Mode templates, parse shared debugger/taxonomy
               JSON payloads, resolve embedded .rodata ROM backups, and track AWAIT.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Emergency Static Templates & Shared JSON Schema Parsers
Mission: Guarantee baseline construction and eliminate parser duplication across
disk and compiled-in ROM loading paths.
  ### For Game Designers & Non-Programmers:
  Provides built-in emergency debugger presets and reads JSON settings consistently
  whether loaded from disk files or emergency ROM memory.
  ### For Systems Programmers & Engine Architects:
  Single authoritative zero-heap parser path shared cleanly with `debug.c`.
@triggers
  - Primary Cogs  : `debug_get_monolith_fallback_config_raw()`,
                    `debug_fallback_parse_config_json()`,
                    `debug_fallback_parse_tripwire_matrix_json()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section1`
--------------------------------------------------------------------------------
*/

/**
 * @brief Writes the compiled monolith Safe-Mode template into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Fills a blank settings container with full emergency defaults so debugging tools
 * stay active whenever disk presets cannot be read.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct struct assignment from `.rodata` `s_debug_fallback_template` (0 stack loops,
 * zero heap); requires exact `sizeof(debug_config_t)` ABI match from caller.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section1`
 *
 * @param[out] out_config  Caller-owned `debug_config_t` destination buffer.
 * @param[in]  config_size Must equal `sizeof(debug_config_t)`.
 */
void debug_get_monolith_fallback_config_raw(debug_config_t *out_config, size_t config_size)
{
    /* Step 1: Validate caller buffer and exact struct byte size */
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return;
    }

    /* Step 2: Copy immutable .rodata Safe-Mode template atomically */
    *out_config = s_debug_fallback_template;
}

/**
 * @brief Writes the ultra-compact 1.44 MB Floppy Safe-Mode template into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a trimmed Safe-Mode preset built for tight memory environments (16-turn
 * history cap, quiet monochrome output, safety checks preserved).
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct struct assignment from `.rodata` `s_debug_floppy_safe_template`, keeping
 * `loaded_from_data = false` and avoiding any VFS or heap dependency.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section1`
 *
 * @param[out] out_config  Caller-owned `debug_config_t` destination buffer.
 * @param[in]  config_size Must equal `sizeof(debug_config_t)`.
 */
void debug_get_floppy_safe_config_raw(debug_config_t *out_config, size_t config_size)
{
    /* Step 1: Validate caller buffer and exact struct byte size */
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return;
    }

    /* Step 2: Copy compact 1.44 MB Floppy Safe-Mode template atomically */
    *out_config = s_debug_floppy_safe_template;
}

/**
 * @brief Hydrates debugger settings from a bounded JSON profile payload with bounds guards.
 *
 * ### For Game Designers & Non-Programmers:
 * Extracts switches (`max_snapshots`, `break_on_hp_zero`, `trace_channels`, domain traps)
 * identically for modder JSON files on disk and compiled ROM profiles, rejecting out-of-range
 * capacities before corrupting engine memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-allocated slice isolation (`DEBUG_FB_MAX_SLICE_BUF` = 4 KiB) via `fb_select_object_slice()`
 * followed by single-pass `json_get_*` extraction; emits `fb_emit_bios_log(LOG_SEV_WARNING)`
 * on static ring cap or interval bounds breaches.
 *
 * @triggers
 * - Shared Callers: `debug.c` (`debug_load_config_profile`), `debug_fallback_load_rom_profile()`
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section1`,
 *                   `tests/integrated/test_debug_integrated.c:Section1`
 *
 * @param[in]  scope        Null-terminated JSON object slice (`profile` block or block 3 body).
 * @param[in]  profile_id   Fallback profile name if JSON `id` field is omitted.
 * @param[in]  from_data    True when sourced from disk VFS; false for compiled ROM.
 * @param[out] out_config   Populated configuration struct on success.
 * @param[in]  config_size  Must match `sizeof(debug_config_t)`.
 * @param[out] out_tripwire Optional failure classification enum output.
 * @param[out] err_buf      Optional bounded error text output.
 * @param[in]  err_size     Capacity of `err_buf` in bytes.
 * @return true if valid and within Layer 0 bounds; false on bad args or range violation.
 */
bool debug_fallback_parse_config_json(
    const char       *scope,
    const char       *profile_id,
    bool              from_data,
    debug_config_t   *out_config,
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

    debug_config_t *cfg = out_config;
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
            fb_emit_bios_log(
                LOG_SEV_WARNING,
                __FILE__,
                __LINE__,
                __func__,
                MODULE_ID,
                "Rejected debug profile due to snapshot_interval_turns bounds violation."
            );
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

/**
 * @brief Overlays snapshot retention policy quotas onto an initialized debugger config.
 *
 * ### For Game Designers & Non-Programmers:
 * Applies memory limits from `snapshot_policy.json` (like maximum rewind history size and
 * pinned emergency slot quotas) without wiping out profile toggles already loaded.
 *
 * ### For Systems Programmers & Engine Architects:
 * Uses `DEBUG_FB_MAX_SLICE_BUF` (4 KiB) stack slice via `fb_select_object_slice()`, clamps
 * `max_snapshots` downward when `ring_buffer_capacity` is stricter, and guards `locked_slot_quota <= 16`.
 *
 * @triggers
 * - Shared Callers: `debug.c` (`debug_load_snapshot_policy`), `debug_fallback_load_rom_snapshot_policy()`
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section1`
 *
 * @param[in]     scope       Null-terminated JSON slice (`policy` block or block 3 body).
 * @param[in,out] out_config  Existing config mutated in place with validated quotas.
 * @param[in]     config_size Must equal `sizeof(debug_config_t)`.
 * @param[out]    err_buf     Optional error string output buffer.
 * @param[in]     err_size    Capacity of `err_buf` in bytes.
 * @return true when policy quotas pass bounds checks; false on bad args or overflow.
 */
bool debug_fallback_parse_snapshot_policy_json(
    const char     *scope,
    debug_config_t *out_config,
    size_t          config_size,
    char           *err_buf,
    size_t          err_size
) {
    /* Step 1: Validate inputs and struct size before mutating out_config */
    if (scope == NULL || out_config == NULL || config_size != sizeof(debug_config_t)) {
        fb_set_err(err_buf, err_size, "Invalid argument in snapshot policy parser.");
        return false;
    }

    debug_config_t *cfg = out_config;
    char policy_slice[DEBUG_FB_MAX_SLICE_BUF];
    size_t ring_cap = 0U;
    size_t lock_quota = 0U;

    /* Step 2: Focus parser cursor onto nested "policy" block when present */
    const char *obj = fb_select_object_slice(scope, "policy", policy_slice, sizeof(policy_slice));

    /* Step 3: Enforce ring buffer cap within compile-time DEBUG_MAX_SNAPSHOTS ceiling */
    if (json_get_size_t(obj, "ring_buffer_capacity", &ring_cap)) {
        if (ring_cap == 0U || ring_cap > DEBUG_MAX_SNAPSHOTS) {
            fb_set_err(err_buf, err_size,
                       "snapshot_policy ring_buffer_capacity outside [1, 64].");
            fb_emit_bios_log(
                LOG_SEV_WARNING,
                __FILE__,
                __LINE__,
                __func__,
                MODULE_ID,
                "Rejected snapshot_policy ring_buffer_capacity outside [1, 64]."
            );
            return false;
        }
        if (cfg->max_snapshots > ring_cap) {
            cfg->max_snapshots = ring_cap;
        }
    }

    /* Step 4: Validate locked snapshot quota against active ring capacity */
    if (json_get_size_t(obj, "locked_slot_quota", &lock_quota)) {
        if (lock_quota > cfg->max_snapshots || lock_quota > 16U) {
            fb_set_err(err_buf, err_size,
                       "snapshot_policy locked_slot_quota exceeds safe ring threshold.");
            fb_emit_bios_log(
                LOG_SEV_WARNING,
                __FILE__,
                __LINE__,
                __func__,
                MODULE_ID,
                "Rejected snapshot_policy locked_slot_quota above safe ring threshold."
            );
            return false;
        }
        cfg->locked_slot_quota = lock_quota;
    }

    return true;
}

/**
 * @brief Extracts taxonomic tripwire reaction rules from a JSON `"tripwires"` array.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the lookup table connecting specific alarm IDs to corrective behaviors (auto-rewind,
 * Safe Mode, or halt-and-inspect) so fault responses stay data-driven.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded `json_find_array` + stack `char item_obj[1024]` scan extracting up to
 * `min(total_elems, max_rules)` rules into caller `.bss`/stack storage; defaults unmapped
 * reaction strings safely to `DEBUG_REACTION_FALLBACK_MODE`.
 *
 * @triggers
 * - Shared Callers: `debug.c` (`debug_load_tripwire_matrix`), `debug_fallback_load_rom_tripwire_matrix()`
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section1`
 *
 * @param[in]  scope     Null-terminated JSON object containing `"tripwires"` array.
 * @param[out] out_rules Destination caller array (`DEBUG_FALLBACK_MAX_TRIPWIRE_RULES`).
 * @param[in]  max_rules Maximum element capacity of `out_rules`.
 * @param[out] out_count Number of decoded rules written to `out_rules`.
 * @param[out] err_buf   Optional bounded error description buffer.
 * @param[in]  err_size  Capacity of `err_buf` in bytes.
 * @return true if at least one valid rule parsed cleanly; false on missing array or 0 valid entries.
 */
bool debug_fallback_parse_tripwire_matrix_json(
    const char                     *scope,
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count,
    char                           *err_buf,
    size_t                          err_size
) {
    /* Step 1: Validate arguments and initialize output counter to zero */
    if (scope == NULL || out_rules == NULL || max_rules == 0U || out_count == NULL) {
        fb_set_err(err_buf, err_size, "Invalid argument in tripwire_matrix parser.");
        return false;
    }

    *out_count = 0U;

    /* Step 2: Locate canonical "tripwires" JSON array span */
    const char *arr_start = NULL;
    size_t arr_len = 0U;
    if (!json_find_array(scope, "tripwires", &arr_start, &arr_len)) {
        fb_set_err(err_buf, err_size, "Missing 'tripwires' array in tripwire_matrix JSON.");
        return false;
    }

    /* Step 3: Iterate bounded elements and hydrate strongly typed reaction structs */
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

    /* Step 4: Publish parsed rule count and reject empty reaction matrices */
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
  ### For Game Designers & Non-Programmers:
  Stores built-in emergency copies of debugger presets directly inside the
  executable so the game starts cleanly even without config folders on disk.
  ### For Systems Programmers & Engine Architects:
  Exposes generated `.rodata` ROM byte spans via `debug_fallback_find_rom_asset()`.
@triggers
  - Primary Cogs  : `debug_fallback_load_rom_profile()`, `debug_fallback_find_rom_asset()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
--------------------------------------------------------------------------------
*/

/**
 * @brief Reports total embedded debugger and taxonomy JSON assets in `.rodata` ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Counts how many emergency debugger presets and rulebooks are compiled into the binary.
 *
 * ### For Systems Programmers & Engine Architects:
 * Compile-time constant wrapper returning `DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section3`
 *
 * @return Number of immutable ROM entries stored in `s_debug_diagnostics_rom_table`.
 */
size_t debug_fallback_rom_count(void)
{
    return (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT;
}

/**
 * @brief Retrieves raw `.rodata` JSON bytes and byte length by zero-based ROM slot.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets built-in self-tests enumerate every embedded backup preset during verification.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounds-checked O(1) index lookup into immutable `s_debug_diagnostics_rom_table`; never
 * allocates or mutates `.bss`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section3`
 *
 * @param[in]  index    ROM slot index `[0, debug_fallback_rom_count())`.
 * @param[out] out_size Optional pointer receiving byte size (0 on invalid index).
 * @return Immutable `.rodata` JSON string pointer, or NULL when `index` is out of range.
 */
const char *debug_fallback_rom_asset_at(size_t index, size_t *out_size)
{
    /* Step 1: Reject out-of-range ROM index cleanly */
    if (index >= (size_t)DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT) {
        if (out_size != NULL) {
            *out_size = 0U;
        }
        return NULL;
    }

    /* Step 2: Return direct immutable pointer into generated .rodata vault */
    const debug_rom_asset_t *asset = &s_debug_diagnostics_rom_table[index];
    if (out_size != NULL) {
        *out_size = asset->size;
    }
    return (const char *)asset->data;
}

/**
 * @brief Searches embedded diagnostics/taxonomy `.rodata` ROM assets by ID or filename stem.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds an emergency backup file inside the binary whether callers ask by full folder path
 * (`"debugger/default"`) or short stem (`"default"`, `"tripwire_matrix"`).
 *
 * ### For Systems Programmers & Engine Architects:
 * Two-pass case-insensitive scan (`O(N)` over ~9 ROM records) checking full `id`/`rel_path`
 * first before matching unambiguous leaf stems after `'/'`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section3`
 *
 * @param[in]  id_or_rel_path Target asset ID, relative path, or leaf filename stem.
 * @param[out] out_size       Optional pointer receiving embedded byte count (0 on miss).
 * @return Immutable `.rodata` pointer to raw JSON payload, or NULL if not found.
 */
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

/**
 * @brief Hydrates a complete Safe-Mode debugger config from `.rodata` ROM JSON.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a factory debugger preset (`"default"`, `"strict"`, `"trace"`) plus snapshot locking
 * rules straight from executable ROM when disk folders are unreadable.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses `.rodata` 5-block envelope in memory via `master_loader_load_from_memory()` +
 * `debug_fallback_parse_config_json()`, stamps `loaded_from_data = false`, overlays ROM
 * `snapshot_policy`, and emits guarded BIOS telemetry on corruption or hydration.
 *
 * @triggers
 * - Primary Callers: `debug_get_default_config()`, `debug_init_emergency_monolith()`
 * - Layer 4 Tests  : `tests/unit/debug/test_debug_fallback_unit.c:Section3`,
 *                    `tests/integrated/test_debug_integrated.c:Section1`
 *
 * @param[in]  profile_id  Target profile stem (defaults cleanly to `"default"` if NULL).
 * @param[out] out_config  Caller `debug_config_t` destination buffer.
 * @param[in]  config_size Must equal `sizeof(debug_config_t)`.
 * @return true if ROM profile hydrated cleanly; false on missing/corrupt asset or bad args.
 */
bool debug_fallback_load_rom_profile(
    const char     *profile_id,
    debug_config_t *out_config,
    size_t          config_size
) {
    /* Step 1: Validate destination struct size and resolve target profile stem */
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return false;
    }

    const char *target = (profile_id != NULL && profile_id[0] != '\0')
                             ? profile_id
                             : "default";

    /* Step 2: Locate qualified debugger/<id> ROM payload or fallback leaf */
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

    /* Step 3: Unwrap 5-block envelope directly from immutable .rodata memory */
    master_loader_t loader;
    if (!master_loader_load_from_memory(&loader, raw_json, raw_size) ||
        loader.body_start == NULL) {
        fb_emit_bios_log(
            LOG_SEV_ERROR, __FILE__, __LINE__, __func__, MODULE_ID,
            "Corrupt 5-block envelope inside diagnostics ROM profile asset."
        );
        return false;
    }

    /* Step 4: Parse shared profile schema and overlay ROM snapshot retention quotas */
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
        fb_emit_bios_log(
            LOG_SEV_ERROR, __FILE__, __LINE__, __func__, MODULE_ID,
            err_msg
        );
        return false;
    }

    (void)debug_fallback_load_rom_snapshot_policy(out_config, config_size);

    /* Step 5: Guarantee non-disk provenance and log Safe-Mode ROM hydration */
    out_config->loaded_from_data = false;

    s_fb.rom_hydrated = true;
    str_safe_copy(s_fb.active_rom_profile,
                  out_config->profile_name,
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

/**
 * @brief Overlays embedded `.rodata` `snapshot_policy.json` quotas onto `out_config`.
 *
 * ### For Game Designers & Non-Programmers:
 * Restores factory Safe-Mode snapshot lock limits whenever the external `snapshot_policy.json`
 * file is missing or unreadable on disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Unwraps `"debugger/snapshot_policy"` from `.rodata` via `master_loader_load_from_memory()`
 * and delegates cleanly to `debug_fallback_parse_snapshot_policy_json()`, logging ROM envelope
 * damage through `fb_emit_bios_log(LOG_SEV_ERROR)`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section3`
 *
 * @param[in,out] out_config  Caller config buffer updated in place.
 * @param[in]     config_size Must match `sizeof(debug_config_t)`.
 * @return true if ROM snapshot policy parsed cleanly; false on missing asset or error.
 */
bool debug_fallback_load_rom_snapshot_policy(debug_config_t *out_config, size_t config_size)
{
    /* Step 1: Validate caller buffer and ABI struct byte size */
    if (out_config == NULL || config_size != sizeof(debug_config_t)) {
        return false;
    }

    /* Step 2: Locate compiled snapshot_policy asset in .rodata vault */
    size_t raw_size = 0U;
    const char *raw_json = debug_fallback_find_rom_asset("debugger/snapshot_policy", &raw_size);
    if (raw_json == NULL || raw_size == 0U) {
        return false;
    }

    /* Step 3: Decode 5-block envelope and delegate to single-source policy parser */
    master_loader_t loader;
    if (!master_loader_load_from_memory(&loader, raw_json, raw_size) ||
        loader.body_start == NULL) {
        fb_emit_bios_log(
            LOG_SEV_ERROR, __FILE__, __LINE__, __func__, MODULE_ID,
            "Corrupt 5-block envelope in ROM debugger/snapshot_policy asset."
        );
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

/**
 * @brief Loads built-in `.rodata` tripwire response matrix rules into caller array.
 *
 * ### For Game Designers & Non-Programmers:
 * Supplies the engine's emergency rule playbook cleanly from ROM so fault alarms continue
 * mapping to proper rewind or pause actions without external taxonomy files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Extracts `"taxonomy/tripwire_matrix"` from `.rodata` via `master_loader_load_from_memory()`,
 * delegates to `debug_fallback_parse_tripwire_matrix_json()`, and emits `LOG_SEV_ERROR` via
 * `fb_emit_bios_log()` only if the compiled ROM payload fails schema extraction.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section3`
 *
 * @param[out] out_rules Caller array receiving parsed tripwire reaction entries.
 * @param[in]  max_rules Element capacity of `out_rules`.
 * @param[out] out_count Number of decoded rules stored.
 * @return true on clean ROM matrix load; false on bad args or corrupt ROM payload.
 */
bool debug_fallback_load_rom_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
) {
    /* Step 1: Validate caller destination array and output counter pointer */
    if (out_rules == NULL || max_rules == 0U || out_count == NULL) {
        return false;
    }

    /* Step 2: Fetch embedded taxonomy/tripwire_matrix payload from .rodata */
    size_t raw_size = 0U;
    const char *raw_json = debug_fallback_find_rom_asset("taxonomy/tripwire_matrix", &raw_size);
    if (raw_json == NULL || raw_size == 0U) {
        return false;
    }

    /* Step 3: Unwrap 5-block envelope and hydrate rules via single-source parser */
    master_loader_t loader;
    if (!master_loader_load_from_memory(&loader, raw_json, raw_size) ||
        loader.body_start == NULL) {
        fb_emit_bios_log(
            LOG_SEV_ERROR, __FILE__, __LINE__, __func__, MODULE_ID,
            "Corrupt 5-block envelope in ROM taxonomy/tripwire_matrix asset."
        );
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
  ### For Game Designers & Non-Programmers:
  Captures exact clues inside emergency BIOS memory when something breaks and
  keeps Safe-Mode AWAIT latched until healthy data is restored.
  ### For Systems Programmers & Engine Architects:
  Logs fault events strictly through `log_fallback_record()` (avoiding primary
  logger reentrancy loops) into `.bss` `s_fb` diagnostic strings.
@triggers
  - Primary Cogs  : `debug_fallback_record_fault()`, `debug_fallback_finish_with_fallback()`,
                    `debug_fallback_clear_await()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section2`
--------------------------------------------------------------------------------
*/

/**
 * @brief Latches exact fault location, test reference, and clue string into `.bss` BIOS memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Writes an un-erasable emergency black-box incident record capturing which subsystem broke,
 * exact source line clues, and which test suite flagged the anomaly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Auto-boots `s_fb.booted = true`, increments `s_fb.fault_count`, stores bounded ASCII fields
 * inside static `s_fb` `.data`/`.bss` buffers, and relays an isolated `LOG_SEV_ERROR` packet
 * via `fb_emit_bios_log()` (`log_fallback_record()`).
 *
 * @triggers
 * - Primary Hook  : `debug_trigger_probe()`, `debug_trap_tripwire()`, `debug_load_config_*()`
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section2`
 *
 * @param[in] subsystem Subsystem identifier tag (defaults to `"unknown"` if NULL).
 * @param[in] condition Required non-empty failure description.
 * @param[in] file      Originating source file anchor (`__FILE__`).
 * @param[in] line      Originating source line anchor (`__LINE__`).
 * @param[in] test_ref  Associated Block 4 / Layer 4 test identifier (may be NULL).
 * @return true if latched into Safe-Mode fault state; false on NULL/empty condition.
 */
bool debug_fallback_record_fault(
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *test_ref
) {
    /* Step 1: Reject empty fault descriptions to avoid overwriting live clues */
    if (condition == NULL || condition[0] == '\0') {
        return false;
    }

    /* Step 2: Arm BIOS registers and latch bounded fault forensic context */
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

    /* Step 3: Forward guarded error packet through Tier-0 log_fallback channel */
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

/**
 * @brief Engages Safe-Mode fallback completion and locks idle `AWAIT` state.
 *
 * ### For Game Designers & Non-Programmers:
 * Marks an interrupted turn or failed file load as safely recovered under backup defaults,
 * then freezes progression cleanly so testers can inspect clues instead of crashing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets `s_fb.safe_mode_engaged`, `s_fb.fallback_completed`, and `s_fb.await_active = true`,
 * records bounded action summary into `s_fb.last_action_taken`, and logs `LOG_SEV_WARNING`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section2`
 *
 * @param[in] subsystem       Subsystem identifier engaging Safe Mode (may be NULL).
 * @param[in] fallback_action Concise summary of fallback action completed (may be NULL).
 * @return true unconditionally once Tier-0 Safe-Mode AWAIT latch is armed.
 */
bool debug_fallback_finish_with_fallback(
    const char *subsystem,
    const char *fallback_action
) {
    /* Step 1: Arm Safe-Mode execution flags and AWAIT freeze register */
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

    /* Step 2: Record Safe-Mode downgrade warning into Tier-0 flight log */
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

/**
 * @brief Returns whether Tier-0 Safe Mode is currently latched idle in `AWAIT`.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets overlays and pause loops check if emergency Safe-Mode protection has halted gameplay.
 *
 * ### For Systems Programmers & Engine Architects:
 * Lock-free O(1) read of `s_fb.await_active` without side effects or I/O.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section2`
 *
 * @return true when AWAIT latch is currently engaged.
 */
bool debug_fallback_is_awaiting(void)
{
    return s_fb.await_active;
}

/**
 * @brief Releases the Tier-0 Safe-Mode `AWAIT` latch after validated state restoration.
 *
 * ### For Game Designers & Non-Programmers:
 * Unlocks gameplay once damaged data has been fixed or rewound, saving a recovery note
 * explaining what resolved the incident.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears `s_fb.await_active` and `s_fb.fallback_completed`, increments `s_fb.recovery_count`,
 * and logs the recovery note via `fb_emit_bios_log(LOG_SEV_INFO)`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section2`
 *
 * @param[in] resolution_note Human-readable recovery explanation (may be NULL).
 * @return true once Tier-0 AWAIT latch is cleared cleanly.
 */
bool debug_fallback_clear_await(const char *resolution_note)
{
    /* Step 1: Clear AWAIT interlocks and record monotonic recovery count */
    s_fb.await_active = false;
    s_fb.fallback_completed = false;
    s_fb.recovery_count++;

    str_safe_copy(s_fb.last_action_taken,
                  (resolution_note != NULL && resolution_note[0] != '\0')
                      ? resolution_note
                      : "Validated recovery cleared Safe-Mode AWAIT.",
                  sizeof(s_fb.last_action_taken));

    /* Step 2: Emit guarded informational recovery telemetry into BIOS flight box */
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
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Guarantee parameter bounds and ROM pointer safety before audit sweeps.
--------------------------------------------------------------------------------
(Monolith/Floppy template bounds, !loaded_from_data provenance, and embedded
ROM payload envelopes are exercised non-destructively in Sub-Assembly 4.2.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Assert bounds invariants, !loaded_from_data contracts, and ROM parity.
@triggers
  - Primary Hook  : Invoked by `debug_preflight()`, `debug_audit()`, and CLI `--audit`
  - Layer 4 Tests : tests/unit/debug/test_debug_fallback_unit.c (Sections 1 & 3),
                    tests/functional/test_debug_functional.c (Section 1)
--------------------------------------------------------------------------------
*/

diag_result_t debug_fallback_preflight(void)
{
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
        s_fb_preflight_details,
        sizeof(s_fb_preflight_details),
        "Fallback preflight verified: profile='%s', max_snaps=%zu, rom_assets=%zu (%u B)",
        cfg.profile_name,
        cfg.max_snapshots,
        debug_fallback_rom_count(),
        (unsigned)DEBUG_DIAGNOSTICS_ROM_TOTAL_BYTES
    );
    res.details = s_fb_preflight_details;

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
Mission: Expose Safe-Mode ROM telemetry structs and fixed-column ASCII BIOS cards.
  ### For Game Designers & Non-Programmers:
  Prints a boxed health status report showing ROM assets, snapshot settings, and
  whether Safe-Mode emergency recovery is holding playback in AWAIT.
  ### For Systems Programmers & Engine Architects:
  Zero-heap `snprintf` card formatting directly from `.bss` fault registers.
@triggers
  - Primary Cogs  : `debug_fallback_get_rom_info()`, `debug_fallback_execute_dump_card()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
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

bool debug_fallback_execute_inspect(debug_config_t *out_config, size_t config_size)
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
        "| DEBUGGER FALLBACK BIOS & ROM TELEMETRY CARD                |\n"
        "+============================================================+\n"
        "| Fallback Profile : %-39s |\n"
        "| Active ROM Prof  : %-39s |\n"
        "| Provenance Flag  : %-39s |\n"
        "| ROM Assets       : %-3u embedded (%6u bytes)            |\n"
        "| Snapshot Ring    : Max %3zu | Interval %2d | Locks Quota %2zu  |\n"
        "| Domain Traps     : Engine=%-5s | Game=%-5s               |\n"
        "| BIOS Safe Mode   : Booted=%-3s | AWAIT=%-3s | Faults=%-4u    |\n"
        "| Last Fault Src   : %-39s |\n"
        "| Last Test Ref    : %-39s |\n"
        "| Self-Audit       : %-39s |\n"
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
Mission: Arm and cleanly disarm the Tier-0 Safe-Mode BIOS debugger registers.
  ### For Game Designers & Non-Programmers:
  Turns emergency rewind and Safe-Mode crash protection on at engine boot and
  resets it neatly when shutting down.
  ### For Systems Programmers & Engine Architects:
  Idempotent initialization of `s_fb` `.bss` registers and ROM profile seed.
@triggers
  - Primary Cogs  : `debug_fallback_boot()`, `debug_fallback_shutdown()`,
                    `debug_init_emergency_monolith()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
--------------------------------------------------------------------------------
*/

/**
 * @brief Arms Tier-0 Safe-Mode BIOS state and seeds active ROM profile telemetry.
 *
 * ### For Game Designers & Non-Programmers:
 * Turns on emergency fault recording right at startup so any failure before external
 * configs finish loading is still captured and recoverable.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies caller profile name into `s_fb.active_rom_profile`, or hydrates `"default"`
 * via `debug_fallback_load_rom_profile()` with monolith fallback, sets `s_fb.booted = true`,
 * and logs an isolated `LOG_SEV_INFO` boot record into `cog.log.fallback`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
 *
 * @param[in] opt_config  Optional pre-populated `debug_config_t` (or NULL for ROM default).
 * @param[in] config_size Must equal `sizeof(debug_config_t)` when `opt_config != NULL`.
 * @return true once Tier-0 Safe-Mode debugger BIOS is booted.
 */
bool debug_fallback_boot(const debug_config_t *opt_config, size_t config_size)
{
    /* Step 1: Resolve active Safe-Mode profile string from caller, ROM, or monolith */
    debug_config_t scratch;
    if (opt_config != NULL && config_size == sizeof(debug_config_t)) {
        str_safe_copy(s_fb.active_rom_profile, opt_config->profile_name,
                      sizeof(s_fb.active_rom_profile));
    } else if (!debug_fallback_load_rom_profile("default", &scratch, sizeof(scratch))) {
        debug_get_monolith_fallback_config_raw(&scratch, sizeof(scratch));
        str_safe_copy(s_fb.active_rom_profile, scratch.profile_name,
                      sizeof(s_fb.active_rom_profile));
        fb_emit_bios_log(
            LOG_SEV_WARNING,
            __FILE__,
            __LINE__,
            __func__,
            MODULE_ID,
            "ROM default profile unavailable; seeded Safe-Mode BIOS from monolith template."
        );
    }

    /* Step 2: Arm Tier-0 BIOS active flag and emit guarded boot log */
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

/**
 * @brief Resets Tier-0 Safe-Mode BIOS registers and fault strings cleanly.
 *
 * ### For Game Designers & Non-Programmers:
 * Disarms emergency fault latches and clears temporary troubleshooting notes when
 * closing the engine or starting a clean test case.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zeroes static `.bss`/`.data` `s_fb` struct completely and initializes default
 * diagnostic string placeholders (`"none"`, `"NONE"`).
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
 */
void debug_fallback_shutdown(void)
{
    /* Step 1: Zero Safe-Mode register state and reset sentinel status strings */
    memset(&s_fb, 0, sizeof(s_fb));
    str_safe_copy(s_fb.last_subsystem, "none", sizeof(s_fb.last_subsystem));
    str_safe_copy(s_fb.last_condition, "No active Safe-Mode faults captured.",
                  sizeof(s_fb.last_condition));
    str_safe_copy(s_fb.last_source, "none:0", sizeof(s_fb.last_source));
    str_safe_copy(s_fb.last_test_ref, "NONE", sizeof(s_fb.last_test_ref));
    str_safe_copy(s_fb.last_action_taken, "NONE", sizeof(s_fb.last_action_taken));
    str_safe_copy(s_fb.active_rom_profile, "none", sizeof(s_fb.active_rom_profile));
}

/**
 * @brief Queries whether Tier-0 Safe-Mode BIOS boot has completed cleanly.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells diagnostics and startup checks if the emergency debugger backup is armed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Lock-free O(1) read of `s_fb.booted`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
 *
 * @return true if `debug_fallback_boot()` or a latched fault armed `s_fb.booted`.
 */
bool debug_fallback_is_booted(void)
{
    return s_fb.booted;
}

/**
 * @brief Forces immediate debugger initialization from ROM or monolith emergency defaults.
 *
 * ### For Game Designers & Non-Programmers:
 * Recovers debug controls and rewind safety even on completely diskless or corrupted
 * launches without attempting external VFS config reads first.
 *
 * ### For Systems Programmers & Engine Architects:
 * Hydrates `cfg` from `debug_fallback_load_rom_profile("default")` (degrading to static
 * `debug_get_monolith_fallback_config_raw()` on ROM miss), enforces `loaded_from_data = false`,
 * arms `debug_fallback_boot()`, engages `s_fb.safe_mode_engaged = true`, and starts `debug_init()`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
 *
 * @return true if Safe-Mode primary debugger initialization succeeded.
 */
bool debug_init_emergency_monolith(void)
{
    /* Step 1: Hydrate non-disk emergency config from ROM or static monolith template */
    debug_config_t cfg;
    if (!debug_fallback_load_rom_profile("default", &cfg, sizeof(cfg))) {
        debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));
        fb_emit_bios_log(
            LOG_SEV_WARNING,
            __FILE__,
            __LINE__,
            __func__,
            MODULE_ID,
            "ROM default profile missing during emergency boot; using static monolith."
        );
    }

    /* Step 2: Lock Safe-Mode provenance, arm BIOS, and initialize primary debugger */
    cfg.loaded_from_data = false;
    (void)debug_fallback_boot(&cfg, sizeof(cfg));
    s_fb.safe_mode_engaged = true;
    return debug_init(&cfg);
}
