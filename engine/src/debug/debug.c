/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/src/debug/debug.c
@id            cog.debug
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive Engine Debugger, State Rewind & Tripwire Inspector
@role          Runtime Engine & Chassis: Locked-slot snapshot ring, FNV-1a hashing,
               probe evaluation loop, Safe-Mode downgrade, and AWAIT state machine.
@maturity      Rev 5 Baseline | Probe -> Fallback -> DEBUG_STEP_AWAIT active; O(cap) single-slot GCD ring rotation.
@chassis       zero-heap | .bss 70.5 KiB (s_debug 35,232 B + s_audit_backup 35,232 B) + .rodata 9.7 KiB + single-slot normalize stack.
@scale         O(1) snapshot capture with locked-slot preservation; O(p) probe sweep across 32 slots; O(cap) in-place GCD wrap rotation.
@seams         in: diagnostics/{debugger,taxonomy}/<file>.json | uses: cog.debug.types, cog.debug.fallback, cog.log | probes: core AWAIT host | dag: core.util.debug
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Critical locked snapshots never evict under ring wrap until quota permits.
               3. Failed validation probe triggers Safe-Mode downgrade, pins forensic snapshot, and latches DEBUG_STEP_AWAIT.
               4. debug_audit() backs up and restores active ring, breakpoints, and probes cleanly.

PURPOSE & ARCHITECTURAL ROLE:
  Executes the zero-heap debugger chassis: hashes runtime snapshots, resolves
  debugger/taxonomy manifests with debug_fallback escalation, evaluates probe
  callbacks, and drives non-destructive time-travel inspection and AWAIT freezes.

  ### For Game Designers & Non-Programmers:
  Keeps up to 64 combat turns in rewind memory, locks critical error snapshots so
  they cannot scroll away, and pauses safely in AWAIT whenever a test probe catches
  a defect—allowing diagnosis instead of sudden termination.

  ### For Systems Programmers & Engine Architects:
  - Controlled Memory Chassis: .bss totals 70,466 B because s_audit_backup
    mirrors the full 35,232 B debug_state_t for rollback in debug_audit(); heavy
    sub-fields can migrate to targeted snapshot scratch later if RAM tightens.
  - Bottleneck & Headroom: Snapshot overwrite scans max 64 slots cleanly in O(64);
    dbg_normalize_ring_for_locks() uses O(cap) in-place GCD juggling with 1 slot on stack.
  - Probe & Config Wiring: Hydrates policy settings via debug_fallback parsers and
    coordinates tripwire escalation directly through debug_resolve_await().
================================================================================
*/

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/*
================================================================================
BLOCK 2: OPENING BLOCK
( 3-Tier Includes, Private Ontology, Unified Memory Chassis & Inline Frontier )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug
Mission:       Bind deps, log/debug setup, private structs, unified
               .bss/.rodata chassis, and support helpers.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None in Layer 0/1)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "debug/debug.h"
#include "core/manifest.h"
#include "debug/debug_types.h"
#include "debug/debug_fallback.h"
#include "core/math_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "io/master_loader.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Bounds Constants
--------------------------------------------------------------------------------
*/

/** @brief Canonical manifest identifier registering the primary debugger engine. */
#define MODULE_ID                DEBUG_MODULE_ID
/** @brief Default telemetry category emitted by debugger diagnostics. */
#define LOCAL_LOG_CAT            LOG_CAT_DEBUG
/** @brief Human-readable module tag stamped on internal debug telemetry. */
#define LOCAL_MODULE_TAG         "Debugger"
/** @brief Standard 32-bit FNV-1a offset basis used for deterministic state hashing. */
#define FNV1A_OFFSET_BASIS_32    2166136261u
/** @brief Standard 32-bit FNV-1a prime multiplier for snapshot integrity checks. */
#define FNV1A_PRIME_32           16777619u
/** @brief Ring occupancy threshold (%) triggering high-watermark telemetry notes. */
#define DEBUG_SNAPSHOT_HIGH_PCT  85U

static_assert(
    DEBUG_MAX_SNAPSHOTS >= 8U && DEBUG_MAX_BREAKPOINTS >= 4U && DEBUG_MAX_VALIDATION_PROBES >= 8U,
    "Debugger snapshot ring, breakpoint, and validation probe tables must remain operational"
);
static_assert(
    DEBUG_SNAPSHOT_HIGH_PCT > 0U && DEBUG_SNAPSHOT_HIGH_PCT <= 100U,
    "DEBUG_SNAPSHOT_HIGH_PCT must express a valid percentage"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/**
 * @struct debug_internal_state
 * @brief Complete zero-heap runtime state for the Layer 0 Debugger Engine.
 *
 * ### For Game Designers & Non-Programmers:
 * Holds the debugger's live memory rewind slots, active crash watchpoints,
 * safety test probes, and emergency pause switches inside fixed engine memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Consolidated .bss singleton shared across `s_debug` and `s_audit_backup`
 * rollback isolation with zero dynamic allocations.
 */
typedef struct debug_internal_state {
    debug_config_t                 config;                                       /**< Active debugger policy configuration. */
    debug_snapshot_t               snapshots[DEBUG_MAX_SNAPSHOTS];               /**< Fixed circular snapshot history buffer. */
    size_t                         snapshot_count;                               /**< Currently populated snapshot count. */
    size_t                         snapshot_head;                                /**< Circular oldest-slot index. */
    uint64_t                       next_snapshot_id;                             /**< Monotonic snapshot sequence generator. */
    size_t                         locked_count;                                 /**< Active locked critical slot count. */
    debug_breakpoint_t             breakpoints[DEBUG_MAX_BREAKPOINTS];           /**< Registered expression/event breakpoints. */
    size_t                         breakpoint_count;                             /**< Active breakpoint registration count. */
    int                            next_breakpoint_id;                           /**< Monotonic breakpoint handle allocator. */
    debug_validation_probe_t       probes[DEBUG_MAX_VALIDATION_PROBES];          /**< Registered validation probe table. */
    size_t                         probe_count;                                  /**< Active validation probe count. */
    debug_fallback_tripwire_rule_t rules[DEBUG_FALLBACK_MAX_TRIPWIRE_RULES];     /**< Loaded tripwire reaction taxonomy rules. */
    size_t                         rule_count;                                   /**< Valid loaded rule count. */
    debug_step_mode_t              step_mode;                                    /**< Current execution stepping gate mode. */
    debug_tripwire_t               tripwire_status;                              /**< Latest latched tripwire code. */
    char                           tripwire_details[DEBUG_MAX_MSG_LEN];          /**< Human-readable tripwire summary. */
    debug_tripwire_report_t        last_report;                                  /**< Most recent probe failure forensic report. */
    uint32_t                       traps_triggered;                              /**< Cumulative fired breakpoint traps. */
    uint32_t                       probe_failures;                               /**< Cumulative validation probe failures. */
    size_t                         snapshot_watermark;                           /**< Peak snapshot occupancy observed. */
    uint64_t                       total_snapshots_captured;                     /**< Lifetime captured snapshot counter. */
    bool                           safe_mode;                                    /**< True once downgraded to Safe-Mode BIOS. */
    bool                           await_active;                                 /**< True when frozen in DEBUG_STEP_AWAIT. */
    bool                           fallback_completed;                           /**< True once Safe-Mode fallback step ran. */
    bool                           in_probe;                                     /**< Reentrancy guard inside debug_trigger_probe. */
    bool                           in_audit;                                     /**< Non-destructive rollback guard during audit. */
} debug_internal_state_t;

/* Forward declaration binding Sub-Assembly 2.3 debug_manifest to Block 4.2. */
diag_result_t debug_preflight(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Module Manifest (.rodata) & Static State Storage (.bss)
--------------------------------------------------------------------------------
*/

/** @brief Public module metadata node registering cog.debug in engine manifest. */
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

/**
 * @brief Primary debugger ring/probe/tripwire state container in .bss (~35.2 KB).
 *
 * Holds the 64-slot circular snapshot ring (`snapshots[]`), 16 breakpoints,
 * 32 `debug_validation_probe_t` bindings, 16 taxonomy rules, and `last_report`.
 */
static debug_internal_state_t s_debug;

/**
 * @brief Non-destructive rollback snapshot of `s_debug` in .bss (~35.2 KB).
 *
 * Used strictly by `debug_audit()` to run live snapshot/lock/rewind/AWAIT stress
 * tests without corrupting or allocating heap over an active gameplay session.
 */
static debug_internal_state_t s_audit_backup;
/** @brief True while primary debugger engine is initialized and armed. */
static bool s_debug_active = false;
/** @brief True when debugger profile hydrated from disk JSON rather than ROM/fallback. */
static bool s_debug_data_driven = false;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & O(1) Ring Predicates
--------------------------------------------------------------------------------
Pure inline bitwise hashers, state-field bounds checks, index translation,
and taxonomy fallback switches operating strictly without heap or side effects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Mixes a signed 32-bit integer into a running FNV-1a 32-bit hash.
 *
 * ### For Game Designers & Non-Programmers:
 * Incorporates numeric gameplay values (like HP or moral sway) into a digital
 * fingerprint that spots invisible corruption between turns.
 *
 * ### For Systems Programmers & Engine Architects:
 * Byte-order raw mix across `sizeof(int)` octets using canonical FNV-1a prime.
 *
 * @param hash Running 32-bit FNV-1a accumulator.
 * @param val  Signed integer value to mix.
 * @return Updated 32-bit FNV-1a hash digest.
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
 *
 * ### For Game Designers & Non-Programmers:
 * Adds scene names or character stance labels into the turn fingerprint safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Null-safe single-pass byte mixer; returns unchanged `hash` when `str == NULL`.
 *
 * @param hash Running 32-bit FNV-1a accumulator.
 * @param str  Null-terminated C string or NULL.
 * @return Updated 32-bit FNV-1a hash digest.
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
 * @brief Verifies domain bounds of restored hero/world fields before exiting AWAIT.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms HP, moral alignment (-100..+100), and twin relic gauges are healthy
 * before resuming play from an emergency pause or rewind.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure O(1) bounds predicate checking canonical simulation scalar invariants.
 *
 * @param fields Immutable state field bundle (NULL treated as vacuously sane).
 * @return True if all fields satisfy valid tactical simulation bounds.
 */
static inline bool dbg_state_fields_are_sane(const debug_state_fields_t *fields)
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
 * @brief Resolves physical ring index for chronological slot 0..snapshot_count-1.
 *
 * ### For Game Designers & Non-Programmers:
 * Translates "turn #3 in rewind history" into the exact circular memory slot.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure O(1) modular offset calculation guarding zero `max_snapshots`.
 *
 * @param logical_idx Chronological index `[0, snapshot_count)`.
 * @return Physical index within `s_debug.snapshots[]`.
 */
static inline size_t dbg_logical_to_physical(size_t logical_idx)
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
 *
 * ### For Game Designers & Non-Programmers:
 * Chooses the safest default response (rewind, Safe Mode, or pause) when custom
 * reaction config rules aren't available.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure O(1) switch table returning deterministic `debug_tripwire_reaction_t`.
 *
 * @param wire Tripwire classification enum.
 * @return Default Safe-Mode reaction policy for `wire`.
 */
static inline debug_tripwire_reaction_t dbg_default_reaction_for_wire(debug_tripwire_t wire)
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
--------------------------------------------------------------------------------
Sub-Assembly 2.5: Monitored Support Infrastructure (Non-Inline Internal)
--------------------------------------------------------------------------------
Support Role:  Variadic buffer formatting, high-watermark telemetry dispatch,
               O(cap) single-slot GCD circular ring rotation, and lock eviction.
Pattern Watch: If non-debug Layer 0 cogs duplicate `dbg_append_fmt()` cursor
               appending or in-place array GCD rotation, elevate into
               engine/include/core/string_utils.h or core collection helpers.
Linkage Rule:  Strictly static functions only; zero static arrays or .bss storage.
--------------------------------------------------------------------------------
*/

/**
 * @brief Bounded sprintf appender preventing signed offset wrap or overrun.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely builds multi-line turn diff summaries without cutting off halfway
 * incorrectly or overflowing text memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Variadic `vsnprintf` cursor appender saturating `*io_offset` at `cap - 1U`.
 *
 * @param buf       Destination character buffer.
 * @param cap       Total capacity of `buf` in bytes.
 * @param io_offset Read-write byte cursor position.
 * @param fmt       Standard printf format specification.
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
 * @brief Tracks high-watermark ring buffer usage and logs high-pressure thresholds.
 *
 * ### For Game Designers & Non-Programmers:
 * Watches how full rewind history gets and posts a debug notice once usage
 * crosses 85% capacity.
 *
 * ### For Systems Programmers & Engine Architects:
 * Updates `s_debug.snapshot_watermark` and emits `LOG_DEBUG` outside `in_audit`.
 *
 * @param current_count Current snapshot ring occupancy count.
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
 * @brief Computes greatest common divisor via Euclid's algorithm for ring rotation.
 *
 * ### For Game Designers & Non-Programmers:
 * Internal step helper letting rewind history reorganize itself in place without
 * reserving large extra scratch blocks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure iterative Euclidean GCD over `size_t` operands.
 *
 * @param a First non-negative integer.
 * @param b Second non-negative integer.
 * @return Greatest common divisor of `a` and `b`.
 */
static size_t dbg_gcd_size(size_t a, size_t b)
{
    while (b != 0U) {
        size_t rem = a % b;
        a = b;
        b = rem;
    }
    return a;
}

/**
 * @brief Linearizes a wrapped circular ring in-place so index 0 matches slot 0.
 *
 * ### For Game Designers & Non-Programmers:
 * Realigns oldest-to-newest turn snapshots in memory cleanly whenever locked
 * snapshots or resizing require linear chronological order.
 *
 * ### For Systems Programmers & Engine Architects:
 * Uses Bentley's O(cap) GCD juggling rotation with a single stack-allocated
 * `debug_snapshot_t tmp`, eliminating the previous `DEBUG_MAX_SNAPSHOTS` stack
 * array allocation (~64x stack reduction) while keeping exact element order.
 */
static void dbg_normalize_ring_for_locks(void)
{
    size_t cap = s_debug.config.max_snapshots;
    if (cap > DEBUG_MAX_SNAPSHOTS) {
        cap = DEBUG_MAX_SNAPSHOTS;
    }
    if (cap == 0U || s_debug.snapshot_count < cap || s_debug.snapshot_head == 0U) {
        return;
    }

    size_t k = s_debug.snapshot_head % cap;
    if (k == 0U) {
        s_debug.snapshot_head = 0U;
        return;
    }

    size_t cycles = dbg_gcd_size(k, cap);
    for (size_t cycle = 0U; cycle < cycles; cycle++) {
        debug_snapshot_t tmp = s_debug.snapshots[cycle];
        size_t j = cycle;

        for (;;) {
            size_t next = j + k;
            if (next >= cap) {
                next -= cap;
            }
            if (next == cycle) {
                break;
            }
            s_debug.snapshots[j] = s_debug.snapshots[next];
            j = next;
        }
        s_debug.snapshots[j] = tmp;
    }

    s_debug.snapshot_head = 0U;
}

/**
 * @brief Releases the oldest locked slot when locked_count reaches locked_slot_quota.
 *
 * ### For Game Designers & Non-Programmers:
 * Frees the oldest pinned crash snapshot only when lock quota is completely
 * full, making room for newer critical evidence.
 *
 * ### For Systems Programmers & Engine Architects:
 * Linear chronological scan across `snapshot_count` slots via `dbg_logical_to_physical()`.
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

/*
================================================================================
BLOCK 3: BODY BLOCK
( Operational Business Logic, Span Scopes & Runtime Hydration )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug
Mission:       Execute deterministic state hashing, layered config/policy/matrix
               hydration, snapshot time-travel, breakpoints, and fail-test -> AWAIT.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Deterministic Hashing & Configuration Provenance Cogs
Mission: Compute deterministic FNV-1a hashes and hydrate layered JSON profiles,
snapshot retention policies, and tripwire matrices with ROM Safe-Mode fallback.
  ### For Game Designers & Non-Programmers:
  Loads your debug rules from disk configs, or restores safe factory defaults
  from built-in ROM if disk manifests are damaged.
  ### For Systems Programmers & Engine Architects:
  Reentrancy-safe config hydration via `master_loader` VFS domain `"config"`,
  falling back cleanly to `debug_fallback` `.rodata` ROM on corruption.
@triggers
  - Primary Cogs  : `debug_compute_state_hash()`, `debug_load_config_profile()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`,
                    `tests/functional/test_debug_functional.c`
--------------------------------------------------------------------------------
*/

/**
 * @brief Computes a deterministic 32-bit FNV-1a hash over all fields of `fields`.
 *
 * ### For Game Designers & Non-Programmers:
 * Generates an exact digital fingerprint of every stat in a turn snapshot so the
 * debugger can prove replays stayed identical and detect memory tampering.
 *
 * ### For Systems Programmers & Engine Architects:
 * Unrolled field-by-field FNV-1a accumulator (`fnv1a_mix_int` + `fnv1a_mix_cstr`)
 * over `debug_state_fields_t`; purely functional (`0` global writes, `0` stack arrays).
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`,
 *                   `tests/functional/test_debug_functional.c`
 *
 * @param[in] fields Simulation state record to checksum.
 * @return 32-bit FNV-1a checksum digest, or `0U` when `fields == NULL`.
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

/**
 * @brief Overlays snapshot retention policy quotas from disk VFS or compiled ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads `snapshot_policy.json` to keep emergency snapshot lock limits up to date,
 * restoring Safe-Mode ROM rules automatically if disk files are unavailable or damaged.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-allocates `char json_buffer[FS_BUFFER_MEDIUM_BYTES]` to load `DEBUG_SNAPSHOT_POLICY_PATH`,
 * validates `"tenlux.data.snapshot_policy.v1"`, delegates to `debug_fallback_parse_snapshot_policy_json()`,
 * and emits guarded `LOG_WARN` if an existing disk file fails validation before ROM overlay.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in,out] cfg Caller configuration struct enriched with ring/lock policy limits.
 * @return true if loaded from disk VFS; false on NULL or ROM fallback completion.
 */
bool debug_load_snapshot_policy(debug_config_t *cfg)
{
    /* Step 1: Validate target configuration pointer */
    if (cfg == NULL) {
        return false;
    }

    /* Step 2: Load snapshot_policy JSON from disk VFS and overlay shared parser */
    master_loader_t loader;
    char json_buffer[FS_BUFFER_MEDIUM_BYTES];
    if (master_loader_load(&loader, "config", DEBUG_SNAPSHOT_POLICY_PATH,
                           json_buffer, sizeof(json_buffer))) {
        if (master_loader_validate_schema(&loader, "tenlux.data.snapshot_policy.v1") &&
            loader.body_start != NULL) {
            char err_msg[128];
            if (debug_fallback_parse_snapshot_policy_json(
                    loader.body_start, cfg, sizeof(*cfg), err_msg, sizeof(err_msg))) {
                return true;
            }
            if (!s_debug.in_audit) {
                LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                         "snapshot_policy parse failed (%s); falling back to ROM policy.", err_msg);
            }
        } else if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                     "Invalid snapshot_policy schema on disk; falling back to ROM policy.");
        }
    }

    /* Step 3: Fall back to compiled .rodata ROM snapshot policy cleanly */
    (void)debug_fallback_load_rom_snapshot_policy(cfg, sizeof(*cfg));
    return false;
}

/**
 * @brief Loads taxonomic tripwire reaction rules from VFS JSON or `.rodata` ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads `tripwire_matrix.json` to map specific error codes to automatic responses
 * (auto-rewind, Safe Mode, or halt), falling back cleanly to embedded backup rules.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-allocates `char json_buffer[FS_BUFFER_LARGE_BYTES]` to load `DEBUG_TRIPWIRE_MATRIX_PATH`,
 * validates `"tenlux.data.tripwire_matrix.v1"`, delegates parsing to
 * `debug_fallback_parse_tripwire_matrix_json()`, and degrades to ROM on disk miss or parse error.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[out] out_rules Destination caller rule buffer.
 * @param[in]  max_rules Maximum element capacity of `out_rules`.
 * @param[out] out_count Number of decoded rules populated.
 * @return true if loaded from disk VFS; false on invalid args or ROM fallback completion.
 */
bool debug_load_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
) {
    /* Step 1: Precondition checks on destination rule table and output count */
    if (out_rules == NULL || max_rules == 0U || out_count == NULL) {
        return false;
    }

    *out_count = 0U;

    /* Step 2: Attempt disk VFS matrix load and schema-validated extraction */
    master_loader_t loader;
    char json_buffer[FS_BUFFER_LARGE_BYTES];
    if (master_loader_load(&loader, "config", DEBUG_TRIPWIRE_MATRIX_PATH,
                           json_buffer, sizeof(json_buffer))) {
        if (master_loader_validate_schema(&loader, "tenlux.data.tripwire_matrix.v1") &&
            loader.body_start != NULL) {
            char err_msg[128];
            if (debug_fallback_parse_tripwire_matrix_json(
                    loader.body_start, out_rules, max_rules, out_count, err_msg, sizeof(err_msg))) {
                return true;
            }
            if (!s_debug.in_audit) {
                LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                         "tripwire_matrix parse failed (%s); falling back to ROM matrix.", err_msg);
            }
        } else if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                     "Invalid tripwire_matrix schema on disk; falling back to ROM matrix.");
        }
    }

    /* Step 3: Fall back to immutable .rodata taxonomy ROM rules */
    (void)debug_fallback_load_rom_tripwire_matrix(out_rules, max_rules, out_count);
    return false;
}

/**
 * @brief Resolves a named debugger JSON preset via VFS with Safe-Mode tripwire handling.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads presets like `"default"`, `"strict"`, or `"trace"` from disk configs so QA teams
 * can customize breakpoints and tracing; missing or broken files safely trigger Safe Mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-allocates `json_buffer[FS_BUFFER_MEDIUM_BYTES]` for `master_loader_load()`,
 * delegates body extraction cleanly to `debug_fallback_parse_config_json()`, latches
 * `DEBUG_TRIPWIRE_SETTING_MISSING` or `DEBUG_TRIPWIRE_STATE_CORRUPT` via `debug_fallback_record_fault()`,
 * and overlays `debug_load_snapshot_policy()` on clean disk loads.
 *
 * @triggers
 * - Layer 4 Tests : `tests/integrated/test_debug_integrated.c:Section2`
 *
 * @param[in]  profile_name Profile stem (e.g. `"default"`, `"strict"`, `"trace"`).
 * @param[out] out_config   Populated configuration buffer.
 * @return true if loaded cleanly or safely handled missing setting; false on corruption/args.
 */
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
        if (!s_debug.in_audit) {
            LOG_ERROR(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                      "%s", s_debug.tripwire_details);
        }
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
        if (!s_debug.in_audit) {
            LOG_ERROR(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                      "%s", s_debug.tripwire_details);
        }
        *out_config = debug_get_monolith_fallback_config();
        return false;
    }

    /* Step 6: Overlay snapshot_policy quotas without mutating profile provenance */
    (void)debug_load_snapshot_policy(out_config);
    str_safe_copy(out_config->profile_name, profile_name, sizeof(out_config->profile_name));
    out_config->loaded_from_data = true;

    return true;
}

/**
 * @brief Resolves a debugger manifest JSON and hydrates its active profile preset.
 *
 * ### For Game Designers & Non-Programmers:
 * Looks up the debugger menu manifest to discover which debugging preset should be
 * activated at startup, safely dropping back to built-in defaults if missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Loads `manifest_path` (or `DEBUG_MANIFEST_PATH`) via `master_loader`, verifies
 * `"tenlux.data.debug_menu.v1"`, extracts `"active_profile"`, and latches
 * `DEBUG_TRIPWIRE_MANIFEST_MISSING` / `DEBUG_TRIPWIRE_STATE_CORRUPT` on failure.
 *
 * @triggers
 * - Layer 4 Tests : `tests/integrated/test_debug_integrated.c:Section2`
 *
 * @param[in]  manifest_path Optional relative VFS path (NULL defaults to `DEBUG_MANIFEST_PATH`).
 * @param[out] out_config    Destination configuration struct populated on load or fallback.
 * @return true if manifest and referenced profile loaded cleanly; false otherwise.
 */
bool debug_load_config_from_manifest(const char *manifest_path, debug_config_t *out_config)
{
    /* Step 1: Validate caller output buffer and resolve effective manifest path */
    if (out_config == NULL) {
        return false;
    }

    const char *target = (manifest_path != NULL && manifest_path[0] != '\0')
                             ? manifest_path
                             : DEBUG_MANIFEST_PATH;

    /* Step 2: Load menu manifest JSON from disk VFS; engage Safe-Mode on miss */
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

    /* Step 3: Validate canonical manifest schema before inspecting active_profile */
    if (!master_loader_validate_schema(&loader, "tenlux.data.debug_menu.v1")) {
        s_debug.tripwire_status = DEBUG_TRIPWIRE_STATE_CORRUPT;
        snprintf(s_debug.tripwire_details, sizeof(s_debug.tripwire_details),
                 "Debug manifest schema invalid: %.64s", target);
        (void)debug_fallback_record_fault(
            MODULE_ID, s_debug.tripwire_details, __FILE__, __LINE__,
            "test_debug_integrated:section2"
        );
        if (!s_debug.in_audit) {
            LOG_ERROR(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                      "%s", s_debug.tripwire_details);
        }
        *out_config = debug_get_monolith_fallback_config();
        return false;
    }

    /* Step 4: Extract active_profile key and load referenced profile JSON */
    char active_profile[32];
    if (!master_loader_get_string(&loader, NULL, "active_profile",
                                  active_profile, sizeof(active_profile))) {
        str_safe_copy(active_profile, DEBUG_DEFAULT_PROFILE, sizeof(active_profile));
    }

    return debug_load_config_profile(active_profile, out_config);
}

/**
 * @brief Returns a stack-copied immutable monolith Safe-Mode configuration struct.
 *
 * ### For Game Designers & Non-Programmers:
 * Creates an emergency out-of-the-box config object when no external files or ROM
 * presets are available.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates directly to `debug_get_monolith_fallback_config_raw()` into local `cfg`
 * guaranteeing `loaded_from_data == false` and zero heap use.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @return Static fallback `debug_config_t` value copy.
 */
debug_config_t debug_get_monolith_fallback_config(void)
{
    /* Step 1: Hydrate stack copy from quarantined .rodata Safe-Mode template */
    debug_config_t cfg;
    debug_get_monolith_fallback_config_raw(&cfg, sizeof(cfg));
    return cfg;
}

/**
 * @brief Resolves default debugger settings across VFS manifest, `.rodata` ROM, and monolith.
 *
 * ### For Game Designers & Non-Programmers:
 * Picks the best available debugger settings cleanly at startup: external JSON configs first,
 * built-in ROM backups second, and bare emergency defaults last.
 *
 * ### For Systems Programmers & Engine Architects:
 * Three-tier deterministic cascade (`debug_load_config_from_manifest` ->
 * `debug_fallback_load_rom_profile` -> `debug_get_monolith_fallback_config`) with
 * zero heap allocation.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @return Resolved `debug_config_t` suitable for passing into `debug_init()`.
 */
debug_config_t debug_get_default_config(void)
{
    /* Step 1: Prefer data-driven disk manifest configuration */
    debug_config_t cfg;
    if (debug_load_config_from_manifest(NULL, &cfg)) {
        return cfg;
    }

    /* Step 2: Safe-Mode diskless rescue: hydrate canonical "default" from compiled ROM */
    if (debug_fallback_load_rom_profile(DEBUG_DEFAULT_PROFILE, &cfg, sizeof(cfg))) {
        return cfg;
    }

    /* Step 3: Hard emergency floor from static monolith template */
    return debug_get_monolith_fallback_config();
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Debugger State Queries, Step Control & AWAIT Recovery Cogs
Mission: Expose runtime active/step/AWAIT status and recover cleanly from traps.
  ### For Game Designers & Non-Programmers:
  Lets players and testers inspect whether playback is paused in AWAIT and
  safely resume once broken data is fixed or Safe Mode takes over.
  ### For Systems Programmers & Engine Architects:
  Re-validates restored state fields via `debug_resolve_await()` and syncs
  Safe-Mode clearing with `debug_fallback_clear_await()`.
@triggers
  - Primary Cogs  : `debug_is_active()`, `debug_is_awaiting()`, `debug_resolve_await()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`
--------------------------------------------------------------------------------
*/

/**
 * @brief Reports whether primary debugger initialization is currently active.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets status screens check if turn history and invariant checks are currently running.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) read of file-scoped active flag `s_debug_active`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @return true when debugger has been initialized via `debug_init()`.
 */
bool debug_is_active(void)
{
    return s_debug_active;
}

/**
 * @brief Returns the active profile name or `"inactive"` when uninitialized.
 *
 * ### For Game Designers & Non-Programmers:
 * Shows which debugger configuration name is active during play or tests.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns constant `"inactive"` string literal or pointer into `.bss` `s_debug.config.profile_name`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @return Null-terminated string naming active profile (never NULL).
 */
const char *debug_get_active_profile_name(void)
{
    /* Step 1: Return static sentinel when debugger is inactive */
    if (!s_debug_active) {
        return "inactive";
    }
    return s_debug.config.profile_name;
}

/**
 * @brief Updates the active stepping state machine while honoring AWAIT interlocks.
 *
 * ### For Game Designers & Non-Programmers:
 * Changes run/pause/step state unless an active emergency AWAIT halt is blocking progress
 * until bad simulation data is fixed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates enum via `debug_step_mode_is_valid()`, rejects unverified exits out of active
 * `AWAIT` with `LOG_WARN`, and updates `s_debug.step_mode` + `s_debug.await_active`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`
 *
 * @param[in] mode Desired target execution stepping mode.
 */
void debug_set_step_mode(debug_step_mode_t mode)
{
    /* Step 1: Reject invalid enum values cleanly with telemetry warning */
    if (!debug_step_mode_is_valid(mode)) {
        if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                     "Ignored invalid debug_step_mode_t enum value (%d).", (int)mode);
        }
        return;
    }

    /* Step 2: Enforce AWAIT lock: prevent unverified transition out of unresolved tripwire */
    if (s_debug.await_active && s_debug.tripwire_status != DEBUG_TRIPWIRE_CLEAR &&
        mode != DEBUG_STEP_AWAIT) {
        if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                     "Blocked step mode switch to %s while AWAIT active (%s). Call debug_resolve_await().",
                     debug_step_mode_name(mode), s_debug.tripwire_details);
        }
        return;
    }

    /* Step 3: Commit step mode and synchronize AWAIT flag */
    s_debug.step_mode = mode;
    if (mode == DEBUG_STEP_AWAIT) {
        s_debug.await_active = true;
    }
}

/**
 * @brief Returns the current debugger execution step mode enum.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports whether playback is running normally, paused by user, single-stepping, or AWAITed.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) read of `.bss` field `s_debug.step_mode`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`
 *
 * @return Active `debug_step_mode_t` value.
 */
debug_step_mode_t debug_get_step_mode(void)
{
    return s_debug.step_mode;
}

/**
 * @brief Checks if the debugger is actively halted idle in `DEBUG_STEP_AWAIT`.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets the simulation loop know when an error paused playback waiting for state repair.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) conjunction over `s_debug_active` and (`await_active || step_mode == DEBUG_STEP_AWAIT`).
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section2`
 *
 * @return true if AWAIT halt is engaged cleanly.
 */
bool debug_is_awaiting(void)
{
    return s_debug_active && (s_debug.await_active || s_debug.step_mode == DEBUG_STEP_AWAIT);
}

/**
 * @brief Determines if the simulation turn loop is permitted to advance another step.
 *
 * ### For Game Designers & Non-Programmers:
 * Stops turns from advancing while paused or waiting in emergency AWAIT recovery.
 *
 * ### For Systems Programmers & Engine Architects:
 * Permits stepping when `!s_debug_active`, otherwise denies execution if `debug_is_awaiting()`
 * holds or `s_debug.step_mode == DEBUG_STEP_PAUSED`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`
 *
 * @return true when stepping may proceed; false while paused or in AWAIT.
 */
bool debug_can_step(void)
{
    if (!s_debug_active) {
        return true;
    }
    return !debug_is_awaiting() && (s_debug.step_mode != DEBUG_STEP_PAUSED);
}

/**
 * @brief Validates restored simulation state and releases the `DEBUG_STEP_AWAIT` latch.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies that rewound or repaired actor stats and health probes are clean again, then
 * unfreezes simulation playback with a clear audit note.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates optional `restored_fields` via `dbg_state_fields_are_sane()`, re-invokes
 * `s_debug.last_report.probe_id`'s `validate_fn()`, clears `.bss` AWAIT/tripwire flags,
 * calls `debug_fallback_clear_await()`, and emits guarded `LOG_INFO`/`LOG_WARN`/`LOG_ERROR`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`,
 *                   `tests/unit/log/test_log_unit.c:Section3`
 *
 * @param[in] restored_fields Optional repaired state fields to verify (may be NULL).
 * @param[in] resume_mode     Stepping mode restored upon clearing (`DEBUG_STEP_RUN` default).
 * @param[in] resolution_note Human-readable explanation recorded in telemetry.
 * @return true if validation passed and AWAIT cleared; false if state or probe still invalid.
 */
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
Sub-Assembly 3.3: Snapshots, Locked Retention, Time-Travel Rewind & Diff Cogs
Mission: Record turn snapshots into `.bss` circular storage, enforce critical
locked-slot quotas, compute multi-field state diffs, and rewind time safely.
  ### For Game Designers & Non-Programmers:
  Powers the debugger's 64-turn rewind recorder and pins emergency snapshots so
  evidence from a crash or bug can never be overwritten.
  ### For Systems Programmers & Engine Architects:
  O(N) ring search across `DEBUG_MAX_SNAPSHOTS` (64) with hard locked-quota guard.
@triggers
  - Primary Cogs  : `debug_record_snapshot()`, `debug_rewind_to_snapshot()`, `debug_lock_snapshot()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
--------------------------------------------------------------------------------
*/

/**
 * @brief Captures an actor/world snapshot into `.bss` circular ring storage with lock safety.
 *
 * ### For Game Designers & Non-Programmers:
 * Records the complete state of a simulation step in rewind memory while protecting pinned
 * forensic evidence slots from being erased when history wraps around.
 *
 * ### For Systems Programmers & Engine Architects:
 * Selects destination index under `CIRCULAR_FIFO_WITH_LOCKED_SLOTS` (invoking
 * `dbg_normalize_ring_for_locks()` and `memmove()` when preserving locked frames), hashes
 * `fields`, stamps monotonic `next_snapshot_id` + `time_get_monotonic_ms()`, and updates
 * watermark telemetry.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`,
 *                   `tests/functional/test_debug_functional.c`
 *
 * @param[in] fields Simulation state fields to capture.
 * @param[in] cmd    Associated command verb string (may be NULL).
 * @param[in] arg    Associated command argument string (may be NULL).
 * @return Assigned monotonic snapshot ID (`>= 1`), or `0ULL` if inactive or `fields == NULL`.
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
            if (!s_debug.in_audit) {
                LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                         "All snapshot ring slots locked; forcefully retired oldest slot.");
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

/**
 * @brief Pins a stored snapshot slot against normal FIFO eviction under lock quota rules.
 *
 * ### For Game Designers & Non-Programmers:
 * Locks a critical moment in history (like when a tripwire alarm fires) so you can still
 * inspect it dozens of turns later.
 *
 * ### For Systems Programmers & Engine Architects:
 * Normalizes ring alignment (`dbg_normalize_ring_for_locks`), resolves `snapshot_id` (or
 * latest slot when `0ULL`), evicts oldest locked slot via `dbg_unlock_oldest_locked()` if
 * `locked_count >= quota`, and records bounded `lock_reason`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in] snapshot_id Target snapshot handle, or `0ULL` for most recent slot.
 * @param[in] lock_reason Optional reason string (defaults to `"TRIGGER_TRIPWIRE_LATCH"`).
 * @return true if snapshot found and locked; false if empty, inactive, or ID absent.
 */
bool debug_lock_snapshot(uint64_t snapshot_id, const char *lock_reason)
{
    /* Step 1: Reject empty or uninitialized snapshot ring */
    if (!s_debug_active || s_debug.snapshot_count == 0U) {
        return false;
    }

    dbg_normalize_ring_for_locks();

    /* Step 2: Locate requested snapshot slot (0ULL resolves newest frame) */
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

    /* Step 3: Enforce locked_slot_quota before marking slot pinned */
    if (!snap->locked) {
        size_t quota = s_debug.config.locked_slot_quota;
        if (quota == 0U) {
            quota = DEBUG_FALLBACK_LOCKED_SLOT_QUOTA;
        }
        if (quota >= s_debug.config.max_snapshots && s_debug.config.max_snapshots > 1U) {
            quota = s_debug.config.max_snapshots - 1U;
        }
        while (s_debug.locked_count >= quota) {
            if (!s_debug.in_audit) {
                LOG_DEBUG(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.0f,
                          "Locked snapshot quota (%zu) reached; retiring oldest lock.", quota);
            }
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

/**
 * @brief Reports active snapshot count stored in `.bss` ring storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells menus how many saved turn frames are currently loaded and available for rewind.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) accessor returning `s_debug.snapshot_count` when active, else `0U`.
 *
 * @return Valid snapshot count `[0, DEBUG_MAX_SNAPSHOTS]`.
 */
size_t debug_get_snapshot_count(void)
{
    if (!s_debug_active) {
        return 0U;
    }
    return s_debug.snapshot_count;
}

/**
 * @brief Copies chronological snapshot entry `index` (`0` = oldest) into `out_snap`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a saved historical turn frame cleanly without mutating live rewind storage.
 *
 * ### For Systems Programmers & Engine Architects:
 * Translates chronological index via `dbg_logical_to_physical()` and copies struct to caller
 * buffer in O(1) time with zero heap usage.
 *
 * @param[in]  index    Chronological ring offset `[0, snapshot_count)`.
 * @param[out] out_snap Caller destination snapshot buffer.
 * @return true if valid snapshot copied; false on NULL or out-of-range index.
 */
bool debug_get_snapshot(size_t index, debug_snapshot_t *out_snap)
{
    /* Step 1: Validate active state, caller pointer, and chronological bounds */
    if (!s_debug_active || out_snap == NULL || index >= s_debug.snapshot_count) {
        return false;
    }

    /* Step 2: Copy physical ring slot cleanly */
    size_t phys = dbg_logical_to_physical(index);
    *out_snap = s_debug.snapshots[phys];
    return true;
}

/**
 * @brief Returns read-only pointer to the newest snapshot entry in `.bss` ring storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets diagnostics inspect the most recent saved turn immediately.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) lookup computing `dbg_logical_to_physical(s_debug.snapshot_count - 1U)`. Pointer is
 * borrowed from static `.bss` and invalidated if subsequent writes evict that slot.
 *
 * @return Read-only pointer to newest `debug_snapshot_t`, or NULL when inactive or empty.
 */
const debug_snapshot_t *debug_get_latest_snapshot(void)
{
    if (!s_debug_active || s_debug.snapshot_count == 0U) {
        return NULL;
    }

    size_t latest_phys = dbg_logical_to_physical(s_debug.snapshot_count - 1U);
    return &s_debug.snapshots[latest_phys];
}

/**
 * @brief Rewinds state fields back to snapshot `index` after cryptographic checksum check.
 *
 * ### For Game Designers & Non-Programmers:
 * Restores actor stats from an earlier saved frame, pausing safely in AWAIT if memory
 * corruption altered the snapshot's stored fingerprint.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fetches snapshot via `debug_get_snapshot()`, verifies `debug_compute_state_hash(&snap.state)`
 * matches `snap.state_hash` (engaging `DEBUG_TRIPWIRE_HASH_MISMATCH` + Safe-Mode AWAIT +
 * `LOG_ERROR` on mismatch), and emits `LOG_INFO` on verified restore.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in]  index      Chronological target snapshot offset.
 * @param[out] out_fields Restored simulation state fields on verified success.
 * @return true if checksum passed and `out_fields` populated; false on error or mismatch.
 */
bool debug_rewind_to_snapshot(size_t index, debug_state_fields_t *out_fields)
{
    /* Step 1: Validate preconditions and fetch chronological snapshot copy */
    if (!s_debug_active || out_fields == NULL) {
        return false;
    }

    debug_snapshot_t snap;
    if (!debug_get_snapshot(index, &snap)) {
        return false;
    }

    /* Step 2: Verify determinism checksum against tampering before restoring */
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
        if (!s_debug.in_audit) {
            LOG_ERROR(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.8f,
                      "%s", s_debug.tripwire_details);
        }
        s_debug.await_active = true;
        s_debug.step_mode = DEBUG_STEP_AWAIT;
        return false;
    }

    /* Step 3: Commit verified snapshot state into caller out_fields */
    *out_fields = snap.state;
    if (!s_debug.in_audit) {
        LOG_INFO(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.25f,
                 "Rewound to Snapshot #%lu (Turn %d, Hash 0x%08X%s)",
                 (unsigned long)snap.snapshot_id, snap.state.turn,
                 snap.state_hash, snap.locked ? ", LOCKED" : "");
    }
    return true;
}

/**
 * @brief Formats changed simulation scalars, channels, and hashes between two snapshots.
 *
 * ### For Game Designers & Non-Programmers:
 * Produces a concise plain-text diff showing HP, primary/secondary defense & offense gauges,
 * stance, alignment sway, location, and hash changes between two saved frames.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-allocates temporary `char tmp[1024]`, formats deltas via `dbg_append_fmt()` with
 * domain-neutral channel labels, and bounded-copies to `out_buf` via `str_safe_copy()`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`,
 *                   `tests/functional/test_debug_functional.c`
 *
 * @param[in]  s1      Earlier baseline snapshot pointer.
 * @param[in]  s2      Later comparison snapshot pointer.
 * @param[out] out_buf Caller destination string buffer.
 * @param[in]  buf_len Capacity of `out_buf` in bytes.
 * @return Length in bytes written into `out_buf` (excluding NUL), or `0U` on invalid args.
 */
size_t debug_diff_snapshots(const debug_snapshot_t *s1, const debug_snapshot_t *s2,
                            char *out_buf, size_t buf_len)
{
    /* Step 1: Precondition validation on snapshot pointers and destination buffer */
    if (s1 == NULL || s2 == NULL || out_buf == NULL || buf_len == 0U) {
        return 0U;
    }

    char tmp[1024];
    size_t offset = 0U;

    /* Step 2: Emit diff header and inspect per-field scalar/hash changes */
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
            "  Defense Ch:   %d -> %d (%+d)\n",
            s1->state.shield_power, s2->state.shield_power,
            s2->state.shield_power - s1->state.shield_power);
    }
    if (s1->state.sword_edge != s2->state.sword_edge) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Offense Ch:   %d -> %d (%+d)\n",
            s1->state.sword_edge, s2->state.sword_edge,
            s2->state.sword_edge - s1->state.sword_edge);
    }
    if (s1->state.stance != s2->state.stance) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Stance:       %d -> %d\n", s1->state.stance, s2->state.stance);
    }
    if (s1->state.moral_sway != s2->state.moral_sway) {
        dbg_append_fmt(tmp, sizeof(tmp), &offset,
            "  Align Sway:   %d -> %d (%+d)\n",
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

    /* Step 3: Copy cleanly bounded diff output into caller buffer */
    str_safe_copy(out_buf, tmp, buf_len);
    return strlen(out_buf);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Conditional Breakpoints & Domain Guardrail Watchpoints Cogs
Mission: Manage dynamic breakpoints in `.bss` and detect turn, stance, or sway
threshold crossings across incoming simulation snapshots.
  ### For Game Designers & Non-Programmers:
  Lets designers pause automatically on key moments like HP hitting zero,
  stance changes, or extreme alignment swings.
  ### For Systems Programmers & Engine Architects:
  Fixed `DEBUG_MAX_BREAKPOINTS` (16) table scanning in O(B) time with zero heap.
@triggers
  - Primary Cogs  : `debug_add_breakpoint()`, `debug_check_breakpoints()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
--------------------------------------------------------------------------------
*/

/**
 * @brief Registers a conditional breakpoint slot in static `.bss` table `s_debug.breakpoints`.
 *
 * ### For Game Designers & Non-Programmers:
 * Adds an automatic pause rule checking turn numbers, scalar thresholds, or active tripwires.
 *
 * ### For Systems Programmers & Engine Architects:
 * Enforces active debugger state, enum validity, and `DEBUG_MAX_BREAKPOINTS` (`16`) cap,
 * emitting guarded `LOG_WARN` on table overflow or `LOG_DEBUG` on successful registration.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in] type         Condition category (`debug_breakpoint_type_t`).
 * @param[in] target_value Threshold value compared during step checks.
 * @param[in] label        Optional descriptive tag (defaults to `"Breakpoint"`).
 * @return Positive unique breakpoint handle (`>= 1`), or `-1` on invalid state/full table.
 */
int debug_add_breakpoint(debug_breakpoint_type_t type, int target_value, const char *label)
{
    /* Step 1: Verify active state, valid enum type, and static table headroom */
    if (!s_debug_active || !debug_breakpoint_type_is_valid(type)) {
        return -1;
    }
    if (s_debug.breakpoint_count >= DEBUG_MAX_BREAKPOINTS) {
        if (!s_debug.in_audit) {
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.25f,
                     "Breakpoint table saturated (%zu / %d slots used); ignored registration.",
                     s_debug.breakpoint_count, DEBUG_MAX_BREAKPOINTS);
        }
        return -1;
    }

    /* Step 2: Populate breakpoint slot and increment table length */
    debug_breakpoint_t *bp = &s_debug.breakpoints[s_debug.breakpoint_count++];
    memset(bp, 0, sizeof(*bp));
    bp->id = s_debug.next_breakpoint_id++;
    bp->type = type;
    bp->target_value = target_value;
    bp->is_active = true;
    str_safe_copy(bp->label, (label != NULL) ? label : "Breakpoint", sizeof(bp->label));

    /* Step 3: Emit guarded debug telemetry trace */
    if (!s_debug.in_audit) {
        LOG_DEBUG(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, 0.0f,
                  "Breakpoint #%d registered (%s, target=%d, label='%s')",
                  bp->id, debug_breakpoint_type_name(type), target_value, bp->label);
    }
    return bp->id;
}

/**
 * @brief Deletes an active conditional breakpoint matching positive handle `bp_id`.
 *
 * ### For Game Designers & Non-Programmers:
 * Clears a specific pause trigger once you are done investigating that step condition.
 *
 * ### For Systems Programmers & Engine Architects:
 * Linear `O(16)` lookup across `s_debug.breakpoints[]` followed by contiguous in-place array
 * compaction cleanly preserving slot ordering with zero heap operations.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in] bp_id Positive handle returned by `debug_add_breakpoint()`.
 * @return true if matching entry removed; false if inactive, `bp_id <= 0`, or missing.
 */
bool debug_remove_breakpoint(int bp_id)
{
    /* Step 1: Reject invalid breakpoint handles or inactive debugger */
    if (!s_debug_active || bp_id <= 0) {
        return false;
    }

    /* Step 2: Locate handle and compact table entries left in place */
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

/**
 * @brief Clears every registered breakpoint in static `.bss` storage immediately.
 *
 * ### For Game Designers & Non-Programmers:
 * Resets all conditional pause triggers at once cleanly between scenarios or tests.
 *
 * ### For Systems Programmers & Engine Architects:
 * Performs an O(1) counter reset setting `s_debug.breakpoint_count = 0U` in static
 * `.bss` storage without clearing stale array slots, heap churn, or telemetry writes.
 */
void debug_clear_breakpoints(void)
{
    s_debug.breakpoint_count = 0U;
}

/**
 * @brief Evaluates verification probes, scalar channel guardrails, and user breakpoints.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks each simulation step for out-of-range stats (alignment past +/-100, channels above
 * 100, or zero HP) and user pause conditions, explaining clearly why execution paused.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sequential 3-stage predicate gate: (1) `verify_on_step` probes via `debug_check_tripwires()`,
 * (2) `game_traps_active` domain-neutral range checks with guarded `LOG_WARN` telemetry, and
 * (3) `s_debug.breakpoints[]` switch evaluation writing `out_reason`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`,
 *                   `tests/functional/test_debug_functional.c`
 *
 * @param[in]  fields     Current simulation state fields to evaluate.
 * @param[out] out_reason Optional caller string buffer explaining halt reason.
 * @param[in]  max_len    Byte capacity of `out_reason`.
 * @return true if any probe, domain guardrail, or user breakpoint fired; false otherwise.
 */
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

    /* Step 2: Evaluate bounded channel guardrails when domain traps are active */
    if (s_debug.config.game_traps_active) {
        if (s_debug.config.break_on_sway_breach &&
            (fields->moral_sway > 100 || fields->moral_sway < -100)) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len, "Alignment sway out of range: %d", fields->moral_sway);
            }
            if (!s_debug.in_audit) {
                LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                         "Guardrail hit: Alignment sway out of range (%d).", fields->moral_sway);
            }
            return true;
        }

        if (s_debug.config.break_on_resonance_breach &&
            (fields->shield_power > 100 || fields->sword_edge > 100)) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len,
                         "Primary/secondary gauge overflow (Primary: %d, Secondary: %d)",
                         fields->shield_power, fields->sword_edge);
            }
            if (!s_debug.in_audit) {
                LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.5f,
                         "Guardrail hit: Channel gauge overflow (Primary=%d, Secondary=%d).",
                         fields->shield_power, fields->sword_edge);
            }
            return true;
        }

        if (s_debug.config.break_on_hp_zero && fields->hp <= 0) {
            if (out_reason != NULL && max_len > 0U) {
                snprintf(out_reason, max_len, "Actor HP reached zero (HP: %d)", fields->hp);
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
Sub-Assembly 3.5: Validation Probes, Tripwires & Forensic Reporting Cogs
Mission: Bind subsystem Block 4 validation probes, escalate anomalies to Safe-Mode
fallback execution, pin locked forensic snapshots, hold DEBUG_STEP_AWAIT, and
format machine- or operator-facing forensic tripwire reports.
  ### For Game Designers & Non-Programmers:
  The engine's circuit-breaker switchboard: when any check detects corrupted or
  out-of-bounds state, this assembly finishes active work in Safe Mode, freezes
  execution cleanly in AWAIT, and preserves a forensic summary explaining why.
  ### For Systems Programmers & Engine Architects:
  Reentrancy-guarded (`in_probe`) registry and tripwire latch backed by static
  `.bss` `s_debug.probes[]` slots, delegating fallback recovery into
  `debug_fallback_record_fault()` and `debug_lock_snapshot()`.
@triggers
  - Primary Cogs  : `debug_register_validation_probe()`, `debug_clear_validation_probes()`,
                    `debug_trigger_probe()`, `debug_check_tripwires()`,
                    `debug_trap_tripwire()`, `debug_get_tripwire_report()`,
                    `debug_dump_tripwire_report()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`,
                    `tests/unit/debug/test_debug_unit.c:Section4`,
                    `tests/integrated/test_io_integrated.c:Section3`
--------------------------------------------------------------------------------
*/

/**
 * @brief Register or idempotently update a Block 4 subsystem validation probe.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Plugs a subsystem's automated health monitor into the debugger so anomalies
 *   immediately report exact failing checks and test names.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Updates an existing `probe_id` slot in place while preserving its historical
 *   `trigger_count`, or claims the next free slot in `s_debug.probes[]`. Logs a
 *   guarded `LOG_WARN` if `DEBUG_MAX_VALIDATION_PROBES` capacity is exhausted.
 *
 * @param[in] probe Descriptor defining probe ID, subsystem tag, callback, and reaction.
 * @return Slot index `[0, DEBUG_MAX_VALIDATION_PROBES - 1]` on success; `-1` if null,
 *         empty `probe_id`, or probe registry is saturated.
 * @triggers Validates input guard and emits guarded `LOG_WARN` on registry saturation.
 */
int debug_register_validation_probe(const debug_validation_probe_t *probe)
{
    /* Step 1: Reject null descriptors or empty probe identifiers */
    if (probe == NULL || probe->probe_id[0] == '\0') {
        return -1;
    }

    /* Step 2: Update existing slot idempotently if probe_id already registered */
    for (size_t i = 0U; i < s_debug.probe_count; i++) {
        if (strcmp(s_debug.probes[i].probe_id, probe->probe_id) == 0) {
            uint32_t prev_count = s_debug.probes[i].trigger_count;
            s_debug.probes[i] = *probe;
            s_debug.probes[i].is_active = true;
            s_debug.probes[i].trigger_count = prev_count;
            return (int)i;
        }
    }

    /* Step 3: Guard static probe registry ceiling and log saturation telemetry */
    if (s_debug.probe_count >= DEBUG_MAX_VALIDATION_PROBES) {
        if (s_debug_active && !s_debug.in_audit && !s_debug.in_probe) {
            s_debug.in_probe = true;
            LOG_WARN(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.4f,
                     "Probe registry saturated (%zu/%d); rejected '%s'",
                     s_debug.probe_count, DEBUG_MAX_VALIDATION_PROBES, probe->probe_id);
            s_debug.in_probe = false;
        }
        return -1;
    }

    /* Step 4: Claim next free slot in `.bss` and activate probe */
    size_t slot = s_debug.probe_count++;
    s_debug.probes[slot] = *probe;
    s_debug.probes[slot].is_active = true;
    return (int)slot;
}

/**
 * @brief Clear all registered Block 4 validation probes from static `.bss` storage.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Wipes temporary subsystem monitors clean between test suites or profiles.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Zero-fills `s_debug.probes` and resets `s_debug.probe_count` to zero without
 *   touching locked snapshots, breakpoints, or latched tripwire history.
 *
 * @triggers Resets active probe table deterministically with zero heap side effects.
 */
void debug_clear_validation_probes(void)
{
    /* Step 1: Zero probe slot array and reset active probe counter */
    memset(s_debug.probes, 0, sizeof(s_debug.probes));
    s_debug.probe_count = 0U;
}

/**
 * @brief Query how many validation probe slots are currently registered.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Returns the count of active automated safety checks connected to the debugger.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   O(1) non-mutating getter over `s_debug.probe_count`. Works cleanly before `debug_init()`.
 *
 * @return Registered probe slot count in `[0, DEBUG_MAX_VALIDATION_PROBES]`.
 * @triggers Read-only inspector with zero tripwire or state side effects.
 */
size_t debug_get_probe_count(void)
{
    /* Step 1: Return bounded registered probe table count */
    return s_debug.probe_count;
}

/**
 * @brief Retrieve a copy of a registered validation probe by zero-based index.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Exposes registered safety probe names, owners, and lifetime trigger tallies for UI panels.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Bounds-checked O(1) copy out of `s_debug.probes[index]`.
 *
 * @param[in]  index     Zero-based slot index (`< s_debug.probe_count`).
 * @param[out] out_probe Destination caller descriptor structure.
 * @return `true` when copied; `false` if `out_probe == NULL` or `index >= s_debug.probe_count`.
 * @triggers Validates slot bounds cleanly without latching tripwires.
 */
bool debug_get_probe_at(size_t index, debug_validation_probe_t *out_probe)
{
    /* Step 1: Reject NULL output buffer or out-of-range slot index */
    if (out_probe == NULL || index >= s_debug.probe_count) {
        return false;
    }

    /* Step 2: Copy immutable descriptor snapshot to caller memory */
    *out_probe = s_debug.probes[index];
    return true;
}

/**
 * @brief Look up an active registered validation probe by exact string ID.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Confirms whether a named safety monitor (such as `"log.setting_profile"`) is registered.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Linear scan bounded by `DEBUG_MAX_VALIDATION_PROBES` (32) slots. Copies matched
 *   descriptor when `out_probe != NULL` so callers can also use NULL for boolean probes.
 *
 * @param[in]  probe_id  Registered probe identifier string.
 * @param[out] out_probe Optional destination descriptor copy (may be NULL).
 * @return `true` if active probe exists; `false` on missing ID or inactive slot.
 * @triggers Enforces null/empty string guard cleanly.
 */
bool debug_find_probe(const char *probe_id, debug_validation_probe_t *out_probe)
{
    /* Step 1: Guard against NULL or empty probe identifier */
    if (probe_id == NULL || probe_id[0] == '\0') {
        return false;
    }

    /* Step 2: Scan bounded static slot array for exact active identifier match */
    for (size_t i = 0U; i < s_debug.probe_count; i++) {
        if (s_debug.probes[i].is_active &&
            strcmp(s_debug.probes[i].probe_id, probe_id) == 0) {
            if (out_probe != NULL) {
                *out_probe = s_debug.probes[i];
            }
            return true;
        }
    }

    return false;
}

/**
 * @brief Evaluate one registered validation probe non-destructively without triggering AWAIT.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Tests whether a single safety check passes right now without locking gameplay in AWAIT,
 *   allowing tests and recovery tools to confirm repairs cleanly.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Guarded against `in_probe` recursion. Resolves `probe_id`, invokes optional `validate_fn()`,
 *   and populates `*out_result` without altering tripwire latches, Safe Mode, or snapshot rings.
 *
 * @param[in]  probe_id   Registered probe identifier string.
 * @param[out] out_result Optional destination diagnostic result payload (may be NULL).
 * @return `true` if probe is registered and passed; `false` if missing, reentrant, or failed.
 * @triggers Executes Block 4 `validate_fn` cleanly under recursion guard.
 */
bool debug_evaluate_probe(const char *probe_id, diag_result_t *out_result)
{
    /* Step 1: Synthesize failure default when ID invalid, missing, or reentrant */
    diag_result_t res;
    memset(&res, 0, sizeof(res));
    res.passed = false;
    res.severity = SEVERITY_WARNING;
    res.subsystem = MODULE_ID;
    res.details = "Probe not found or reentrant evaluation suppressed";

    if (probe_id == NULL || probe_id[0] == '\0' || s_debug.in_probe) {
        if (out_result != NULL) {
            *out_result = res;
        }
        return false;
    }

    /* Step 2: Locate active probe slot and execute optional Block 4 callback */
    for (size_t i = 0U; i < s_debug.probe_count; i++) {
        const debug_validation_probe_t *probe = &s_debug.probes[i];
        if (probe->is_active && strcmp(probe->probe_id, probe_id) == 0) {
            if (probe->validate_fn != NULL) {
                s_debug.in_probe = true;
                res = probe->validate_fn();
                s_debug.in_probe = false;
            } else {
                res.passed = true;
                res.severity = SEVERITY_PASS;
                res.subsystem = probe->subsystem[0] ? probe->subsystem : MODULE_ID;
                res.details = "Registered probe active (no callback bound)";
            }

            if (out_result != NULL) {
                *out_result = res;
            }
            return res.passed && (res.severity < SEVERITY_CRITICAL);
        }
    }

    if (out_result != NULL) {
        *out_result = res;
    }
    return false;
}

/**
 * @brief Trigger a validation probe tripwire, downgrade safely, and enter DEBUG_STEP_AWAIT.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Handles a live safety tripwire cleanly: confirms which check failed, lets
 *   Safe Mode preserve critical data, freezes the offending frame, and prints an
 *   actionable diagnostic packet rather than crashing abruptly.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Non-reentrant (`in_probe`) pipeline: resolves matched probe defaults, runs optional
 *   `validate_fn()`, increments `traps_triggered`/`probe_failures`, notifies BIOS via
 *   `debug_fallback_record_fault()` and `debug_fallback_finish_with_fallback()`, locks the
 *   latest snapshot if present, arms `DEBUG_STEP_AWAIT` (when active and non-audit),
 *   populates `s_debug.last_report`, and emits a guarded `LOG_ERROR`.
 *
 * @param[in] probe_id  Registered probe identifier, or NULL for ad-hoc tripwire.
 * @param[in] subsystem Originating module identifier override, or NULL to inherit probe/module default.
 * @param[in] condition Human-readable invariant failure description, or NULL for callback/default message.
 * @param[in] file      Source file name captured at trap call site (`__FILE__`).
 * @param[in] line      Source line number captured at trap call site (`__LINE__`).
 * @param[in] func      Enclosing C function name (`__func__`).
 * @return `true` when latched and Safe-Mode downgrade completed; `false` if suppressed due
 *         to active `in_probe` recursion guard.
 * @triggers Enforces reentrancy guard, Safe-Mode downgrade, snapshot pinning, and AWAIT latch.
 */
bool debug_trigger_probe(
    const char *probe_id,
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *func
) {
    /* Step 1: Prevent reentrant tripwire loops while handling an active probe */
    if (s_debug.in_probe) {
        return false;
    }

    s_debug.in_probe = true;

    /* Step 2: Locate registered probe metadata if present and merge fallback overrides */
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

    /* Step 3: Invoke Block 4 validation callback when registered to confirm health */
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

    /* Step 4: Latch primary debugger tripwire status and forensic failure counters */
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

    /* Step 5: Record into Tier-0 BIOS & downgrade to complete safely in fallback */
    (void)debug_fallback_record_fault(eff_sub, probe_detail_buf, file, line, eff_test);

    char fb_action[96];
    snprintf(fb_action, sizeof(fb_action),
             "Reaction=%s | Safe-Mode completed execution -> AWAIT",
             debug_tripwire_reaction_name(eff_react));
    (void)debug_fallback_finish_with_fallback(eff_sub, fb_action);

    s_debug.safe_mode = true;
    s_debug.fallback_completed = true;

    /* Step 6: Pin critical forensic snapshot in circular ring if snapshots exist */
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

    /* Step 7: Enter DEBUG_STEP_AWAIT when debugger active, non-audit, and armed */
    bool engaged_await = false;
    if (s_debug_active && !s_debug.in_audit &&
        s_debug.config.break_on_tripwire && want_await) {
        s_debug.step_mode = DEBUG_STEP_AWAIT;
        s_debug.await_active = true;
        engaged_await = true;
    }

    /* Step 8: Populate structured forensic report and emit guarded error telemetry */
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

    if (s_debug_active && !s_debug.in_audit) {
        LOG_ERROR(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, -0.75f,
                  "Probe tripwire latched: %s", s_debug.tripwire_details);
    }

    s_debug.in_probe = false;
    return true;
}

/**
 * @brief Sweep live state invariants and all active Block 4 validation probes.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Performs a scheduled full-engine safety sweep, catching corrupt telemetry
 *   channels or failing subsystem probes before bad frames cascade further.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   No-op returning `0` when debugger is inactive or `in_probe` is set. Validates
 *   `fields` bounds via `dbg_state_fields_are_sane()` when `game_traps_active`
 *   is armed, then executes each active `validate_fn` callback under `in_probe`.
 *
 * @param[in] fields Optional live state snapshot to sanity-check, or NULL to sweep probes only.
 * @return Number of invariant or probe failures detected and latched during this sweep.
 * @triggers Triggers `debug_trigger_probe()` for every failing state bound or subsystem callback.
 */
size_t debug_check_tripwires(const debug_state_fields_t *fields)
{
    /* Step 1: Skip sweep cleanly when debugger is inactive or inside a probe */
    if (!s_debug_active || s_debug.in_probe) {
        return 0U;
    }

    size_t failures = 0U;

    /* Step 2: Verify live state fields if caller passed a simulation snapshot */
    if (fields != NULL && s_debug.config.game_traps_active &&
        !dbg_state_fields_are_sane(fields)) {
        (void)debug_trigger_probe(
            "debug.state_invariant",
            MODULE_ID,
            "Live simulation fields violated channel bounds (HP/Alignment/Gauges)",
            __FILE__,
            __LINE__,
            __func__
        );
        failures++;
    }

    /* Step 3: Evaluate all registered Block 4 validation probes under guard */
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

/**
 * @brief Legacy compatibility helper that latches a state-corruption tripwire trap.
 *
 * @deprecated Prefer `debug_trigger_probe()` or `DEBUG_TRIGGER_PROBE()` so every trap
 *             carries a structured probe ID, tier classification, and forensic report.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Preserved safety brake for older modules that report state errors without
 *   naming a specific modern probe identifier.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Latches `DEBUG_TRIPWIRE_STATE_CORRUPT`, increments `traps_triggered`, notifies
 *   BIOS via `debug_fallback_record_fault()`, performs guarded snapshot lock and
 *   `DEBUG_STEP_AWAIT` entry when active and non-audit, and emits `LOG_WARN`.
 *
 * @param[in] subsystem Subsystem tag reporting the fault, or NULL for `"unknown"`.
 * @param[in] condition Description of the violated condition, or NULL for `"anomaly"`.
 * @param[in] file      Originating source file (`__FILE__`).
 * @param[in] line      Originating source line (`__LINE__`).
 * @triggers Latches tripwire state, records BIOS fault, and warns via guarded log output.
 */
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

/**
 * @brief Retrieve a copy of the latest structured forensic tripwire report.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Hands UI panels or test harnesses the exact summary of the most recent
 *   debugger freeze so players and QA see clear fault provenance.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Copies `s_debug.last_report` by value into `out_report` and refreshes live
 *   `tripwire_status`, `await_active`, and `fallback_completed` flags atomically.
 *
 * @param[out] out_report Destination forensic report structure.
 * @return `true` on success; `false` if `out_report` is NULL.
 * @triggers Validates destination pointer strictly under non-awaiting negative policy.
 */
bool debug_get_tripwire_report(debug_tripwire_report_t *out_report)
{
    /* Step 1: Guard null output pointer */
    if (out_report == NULL) {
        return false;
    }

    /* Step 2: Copy cached forensic packet and sync live AWAIT/fallback flags */
    *out_report = s_debug.last_report;
    out_report->tripwire_status = s_debug.tripwire_status;
    out_report->await_active = s_debug.await_active;
    out_report->fallback_completed = s_debug.fallback_completed;
    return true;
}

/**
 * @brief Format the latest forensic tripwire, fallback, and AWAIT report into ASCII text.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Renders a crisp, box-framed summary of what broke, which test suite catches it,
 *   whether Safe Mode stepped in, and whether AWAIT is holding execution.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Uses single bounded `snprintf` against caller-owned `out_buf` (`buf_len >= 128U`)
 *   with zero dynamic allocations. Truncation causes `false` return without buffer overrun.
 *
 * @param[out] out_buf Destination caller character buffer.
 * @param[in]  buf_len Byte capacity of `out_buf` (must be at least `128`).
 * @return `true` if full formatted table fit in `out_buf`; `false` on invalid buffer or truncation.
 * @triggers Validates output buffer floor strictly without altering tripwire state.
 */
bool debug_dump_tripwire_report(char *out_buf, size_t buf_len)
{
    /* Step 1: Enforce destination pointer validity and minimum 128-byte capacity */
    if (out_buf == NULL || buf_len < 128U) {
        return false;
    }

    /* Step 2: Format bounded ASCII forensic tripwire report from cached state */
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
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Expose provenance, Safe-Mode status, and tripwire inspection/reset
cogs declared in debug.h Block 4 Sub-Assembly 4.1.
--------------------------------------------------------------------------------
*/

bool debug_is_data_driven(void)
{
    return s_debug_data_driven;
}

bool debug_is_safe_mode(void)
{
    return s_debug_active && (s_debug.safe_mode || !s_debug_data_driven);
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, fallback ROM parity, and
non-destructive ring/breakpoint/AWAIT execution before Block 5 progression.
@triggers
  - Primary Cogs  : `debug_preflight()`, `debug_audit()` (wired into `debug_manifest`)
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c` (Sec 1..4),
                    `tests/functional/test_debug_functional.c` (Sec 1..4),
                    `tests/integrated/test_debug_integrated.c` (Sec 1..4),
                    `tests/system/test_debug_system.c` (Sec 1..4)
--------------------------------------------------------------------------------
*/

/**
 * @brief Restores live debugger state from `s_audit_backup` and formats failure verdict.
 *
 * ### For Systems Programmers & Engine Architects:
 * Single exit rollback helper guaranteeing `debug_audit()` never leaks test
 * snapshots, breakpoints, or latched `AWAIT` flags into caller state on failure.
 */
static diag_result_t dbg_restore_audit_failure(
    bool        was_active,
    bool        was_data_driven,
    const char *check_name,
    const char *details
) {
    s_debug = s_audit_backup;
    s_debug_active = was_active;
    s_debug_data_driven = was_data_driven;
    diag_result_t fail_res = {
        false,
        SEVERITY_CRITICAL,
        debug_manifest.module_id,
        check_name,
        details
    };
    return fail_res;
}

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
        return dbg_restore_audit_failure(
            was_active, was_data_driven,
            "Snapshot Recording & Lock", "Snapshot record or lock failed during audit!"
        );
    }

    debug_state_fields_t restored;
    if (!debug_rewind_to_snapshot(0U, &restored) || restored.hp != 400) {
        return dbg_restore_audit_failure(
            was_active, was_data_driven,
            "Snapshot Rewind", "Snapshot rewind restoration failed during audit!"
        );
    }

    /* Step 4: Verify breakpoint registration, evaluation, and clearing */
    int bp_turn = debug_add_breakpoint(DEBUG_BREAK_TURN, 5, "Turn 5 Checkpoint");
    char break_reason[128];
    sample.turn = 5;
    if (bp_turn <= 0 || !debug_check_breakpoints(&sample, break_reason, sizeof(break_reason))) {
        return dbg_restore_audit_failure(
            was_active, was_data_driven,
            "Breakpoint Evaluation", "Breakpoint condition failed during audit!"
        );
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
        return dbg_restore_audit_failure(
            was_active, was_data_driven,
            "AWAIT Gatekeeper", "AWAIT rejection/resolution contract failed!"
        );
    }

    /* Step 6: Verify ASCII inspection cards format cleanly within 1024-byte bounds */
    char card_buf[1024];
    if (!debug_execute_dump_card("CARD_HERO_VITALS", &sample, card_buf, sizeof(card_buf)) ||
        !debug_execute_dump_card("CARD_TRIPWIRE_DIAGNOSTIC", &sample, card_buf, sizeof(card_buf))) {
        return dbg_restore_audit_failure(
            was_active, was_data_driven,
            "Inspection Card Formatting", "Card dumping failed during audit!"
        );
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
Mission: Populate inspectable telemetry structs, format domain-neutral ASCII
diagnostic cards, and emit live console inspection/history readouts cleanly.
  ### For Game Designers & Non-Programmers:
  Provides structured summary cards and human-readable console tables showing
  live actor gauges, alignment routes, heartbeat counts, and snapshot timelines.
  ### For Systems Programmers & Engine Architects:
  Zero-heap `snprintf` card renderers and bounded stdio readouts backed solely
  by caller pointers and `.bss` state, preserving engine-layer domain neutrality.
@triggers
  - Primary Cogs  : `debug_execute_inspect()`, `debug_execute_dump_card()`,
                    `debug_dump_state()`, `debug_dump_snapshot_history()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section4`
--------------------------------------------------------------------------------
*/

/**
 * @brief Populate an operational telemetry summary of the active debugger session.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Hands dashboards and QA monitors a clean scorecard showing snapshot counts,
 *   latched tripwires, stepping state, and active configuration profile.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Copies `.bss` debugger counters and flags into caller-owned `out_stats` in O(1)
 *   time using bounded `str_safe_copy()` with zero dynamic allocations.
 *
 * @param[out] out_stats Destination statistics structure populated on success.
 * @return `true` when telemetry copied cleanly; `false` if `out_stats` is NULL.
 * @triggers Enforces null pointer guard strictly under non-awaiting negative policy.
 */
bool debug_execute_inspect(debug_stats_t *out_stats)
{
    /* Step 1: Reject null output statistics structure */
    if (out_stats == NULL) {
        return false;
    }

    /* Step 2: Zero-initialize output and populate live `.bss` telemetry fields */
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

/**
 * @brief Render a domain-neutral ASCII diagnostic inspection card into a buffer.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Produces formatted readout cards (`CARD_HERO_VITALS`, `CARD_STATERA_LEDGER`,
 *   `CARD_COMBAT_ENGAGEMENT`, `CARD_ENGINE_HEARTBEAT`, `CARD_TRIPWIRE_DIAGNOSTIC`)
 *   for console overlays and log captures.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Selects card layout by canonical identifier (defaulting empty IDs to
 *   `CARD_HERO_VITALS`), hashes `fields` via `debug_compute_state_hash()`, and
 *   writes directly via bounded `snprintf` with truncation detection.
 *
 * @param[in]  card_id Card selector token, or NULL/empty for default vitals layout.
 * @param[in]  fields  Source simulation snapshot fields to summarize.
 * @param[out] out_buf Destination caller character buffer.
 * @param[in]  buf_len Capacity of `out_buf` in bytes (minimum `128`).
 * @return `true` when card formatted completely inside `buf_len`; `false` on invalid args or truncation.
 * @triggers Validates input pointers and capacity floor deterministically.
 */
bool debug_execute_dump_card(const char *card_id, const debug_state_fields_t *fields,
                             char *out_buf, size_t buf_len)
{
    /* Step 1: Resolve card selector and allow engine-only cards without actor fields */
    if (out_buf == NULL || buf_len < 128U) {
        return false;
    }

    const char *id = (card_id != NULL && card_id[0] != '\0') ? card_id : "CARD_HERO_VITALS";
    bool is_engine_card = (strcmp(id, "CARD_ENGINE_HEARTBEAT") == 0 ||
                           strcmp(id, "CARD_TRIPWIRE_DIAGNOSTIC") == 0);
    if (fields == NULL && !is_engine_card) {
        return false;
    }

    /* Step 2: Compute deterministic hash and fallback turn metadata safely */
    uint32_t hash = (fields != NULL) ? debug_compute_state_hash(fields) : 0U;
    int last_turn = 0;
    if (fields != NULL) {
        last_turn = fields->turn;
    } else if (s_debug.snapshot_count > 0U) {
        const debug_snapshot_t *latest = debug_get_latest_snapshot();
        if (latest != NULL) {
            last_turn = latest->state.turn;
        }
    }
    int written = 0;

    /* Step 3: Render requested ASCII diagnostic card without game lore coupling */
    if (strcmp(id, "CARD_HERO_VITALS") == 0) {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "              HERO VITALS & STANCE READOUT                  \n"
            "============================================================\n"
            "Actor ID:      %-20s (Default Profile)\n"
            "Resource Pair: Primary Channel / Secondary Channel\n"
            "Vitals:        HP: %d/%d  MP: %d  TP: %d\n"
            "Active Stance: ID %d\n"
            "Channel Levels:\n"
            "  [PRI] Defense: %d/100  (Ward Gauge)\n"
            "  [SEC] Offense: %d/100  (Strike Gauge)\n"
            "Zone / Threat: Sector ID %d | Target HP: %d\n"
            "State Hash:    0x%08X\n"
            "============================================================\n",
            fields->hero_name[0] ? fields->hero_name : "Generic Actor",
            fields->hp, fields->max_hp, fields->mp, fields->tp,
            fields->stance,
            fields->shield_power, fields->sword_edge,
            fields->location, fields->threat_hp,
            hash);
    } else if (strcmp(id, "CARD_STATERA_LEDGER") == 0) {
        const char *ending =
            (fields->moral_sway >= 75)
                ? "Branch Alpha (+High Threshold)"
                : (fields->moral_sway <= -75)
                      ? "Branch Omega (-Low Threshold)"
                      : (fields->moral_sway >= -25 && fields->moral_sway <= 25)
                            ? "Branch Neutral (Equilibrium)"
                            : "Branch Neutral (Default Stasis)";
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "              STATERA CONSCIENCE LEDGER                     \n"
            "============================================================\n"
            "Alignment Val: %+4d [-100, +100]\n"
            "Branch Route:  %s\n"
            "Turn Sequence: %d\n"
            "Active Timers: Pri: %d turns | Sec: %d turns\n"
            "State Hash:    0x%08X\n"
            "============================================================\n",
            fields->moral_sway, ending, fields->turn,
            fields->lux_barrio_turns, fields->charm_turns, hash);
    } else if (strcmp(id, "CARD_COMBAT_ENGAGEMENT") == 0) {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "              TACTICAL COMBAT ENGAGEMENT                    \n"
            "============================================================\n"
            "Sim Turn:      Turn #%d\n"
            "Active Target: Target HP: %d\n"
            "Actor Health:  HP: %d / %d\n"
            "Primary Gauge: %d/100 (Buff Timer A: %d turns)\n"
            "Secondary Gge: %d/100 (Debuff Timer B: %d turns)\n"
            "Targets Cleared: %d\n"
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
            s_debug.traps_triggered, last_turn, hash);
    } else {
        written = snprintf(out_buf, buf_len,
            "============================================================\n"
            "               STATE DIAGNOSTIC CARD: %s\n"
            "============================================================\n"
            "Actor: %s | Turn: %d | HP: %d/%d | Align: %+d\n"
            "Primary: %d | Secondary: %d | Stance: %d | Sector: %d\n"
            "State Hash: 0x%08X\n"
            "============================================================\n",
            id, fields->hero_name, fields->turn, fields->hp, fields->max_hp,
            fields->moral_sway, fields->shield_power, fields->sword_edge,
            fields->stance, fields->location, hash);
    }

    return (written > 0 && (size_t)written < buf_len);
}

/**
 * @brief Print a human-readable snapshot of live simulation fields to stdout.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Dumps an immediate formatted console table of health, alignment sway,
 *   channel timers, trap counters, and stepping state during live sessions.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Read-only interactive console procedure hashing `fields` via
 *   `debug_compute_state_hash()` and formatting neutral engine telemetry.
 *
 * @param[in] fields Simulation snapshot structure to display; no-op if NULL.
 * @triggers Enforces null pointer guard silently without mutating debugger state.
 */
void debug_dump_state(const debug_state_fields_t *fields)
{
    /* Step 1: Reject null snapshot fields without touching console output */
    if (fields == NULL) {
        return;
    }

    /* Step 2: Compute deterministic hash and emit domain-neutral console table */
    uint32_t hash = debug_compute_state_hash(fields);

    printf("============================================================\n");
    printf("        CORNERSTONE ENGINE - LIVE STATE INSPECTOR           \n");
    printf("============================================================\n");
    printf(" Actor Label:      %-16s | Turn:        %d\n", fields->hero_name, fields->turn);
    printf(" Health (HP):      %3d / %-3d          | State Hash:  0x%08X\n",
           fields->hp, fields->max_hp, hash);
    printf(" Defense Channel:  %3d / 100          | Guard Timer: %d turns\n",
           fields->shield_power, fields->lux_barrio_turns);
    printf(" Offense Channel:  %3d / 100          | Boost Timer: %d turns\n",
           fields->sword_edge, fields->charm_turns);
    printf(" Active Stance:    ID %-2d              | Sector ID:   %d\n",
           fields->stance, fields->location);
    printf(" Alignment Sway:   %+4d [-100, +100]  | Target HP:   %d\n",
           fields->moral_sway, fields->threat_hp);
    printf(" Traps Latched:    %-4u               | Snapshots:   %zu / %zu\n",
           s_debug.traps_triggered, s_debug.snapshot_count, s_debug.config.max_snapshots);
    printf(" Mode / AWAIT:     %-10s         | Locked Snaps:%zu\n",
           debug_step_mode_name(s_debug.step_mode), s_debug.locked_count);
    printf("============================================================\n");
}

/**
 * @brief Print the chronological circular snapshot history table to stdout.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Lists recorded time-machine frames from oldest to newest so developers can
 *   spot exact turn numbers, HP shifts, alignment changes, and locked pins.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Iterates `s_debug.snapshot_count` entries sequentially via `debug_get_snapshot()`
 *   without altering head cursors or lock counts.
 *
 * @triggers Guards inactive debugger sessions or empty rings cleanly on entry.
 */
void debug_dump_snapshot_history(void)
{
    /* Step 1: Report empty or inactive ring cleanly before formatting headers */
    if (!s_debug_active || s_debug.snapshot_count == 0U) {
        printf("[DEBUG] No snapshots recorded in history.\n");
        return;
    }

    /* Step 2: Render ASCII timeline header and iterate captured ring frames */
    printf("\n+------+------+------------------+------+------+------+------------+--------+--------------------+\n");
    printf("|  ID  | Turn | Command          |  HP  | Align| Lock | Hash       | LogSeq | Stance / Zone      |\n");
    printf("+------+------+------------------+------+------+------+------------+--------+--------------------+\n");

    for (size_t i = 0U; i < s_debug.snapshot_count; i++) {
        debug_snapshot_t snap;
        if (debug_get_snapshot(i, &snap)) {
            printf("| %4lu | %4d | %-16s | %4d | %+4d | %-4s | 0x%08X | %6lu | S:%-2d Z:%-2d          |\n",
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Initialize or soft-reinitialize debugger state cleanly while preserving
registered Block 4 validation probes across mode transitions.
  ### For Game Designers & Non-Programmers:
  Starts the time-machine debugger, loads active debug rules, and resets cleanly
  when leaving debug sessions.
  ### For Systems Programmers & Engine Architects:
  Copies active `.bss` probe slots across `memset(&s_debug, 0, ...)`, arms
  `debug_fallback_boot()`, and falls back cleanly on missing config paths.
@triggers
  - Primary Cogs  : `debug_init()`, `debug_shutdown()`
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section4`
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
