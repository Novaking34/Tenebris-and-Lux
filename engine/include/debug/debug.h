/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/debug/debug.h
@id            cog.debug
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive Engine Debugger, State Rewind & Tripwire Inspector
@role          Public Contract & Seam: Snapshot ring queries, FNV-1a determinism,
               breakpoints, probe registry, and DEBUG_STEP_AWAIT tripwire loop.
@maturity      Rev 5 Baseline | Validation probe registry & Safe-Mode AWAIT recovery contract active.
@chassis       zero-heap | Public POD types; 64-slot ring cap (DEBUG_MAX_SNAPSHOTS), 32 breakpoints/probes.
@scale         O(1) snapshot capture & FNV-1a step hash; O(n) probe/breakpoint scan capped by 32 slots.
@seams         in: diagnostics/{debugger,taxonomy}/<file>.json | uses: cog.debug.types, cog.debug.fallback, cog.log | probes: debug_register_probe -> DEBUG_STEP_AWAIT | dag: core.util.debug
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Pure Layer 0 independence: zero includes or struct dependencies on Layer 1 game/.
               3. Bitwise-deterministic 32-bit FNV-1a hashing catches desync across headless & X11 runs.
               4. Non-crashing probe failures downgrade cleanly and latch DEBUG_STEP_AWAIT until resolved.

PURPOSE & ARCHITECTURAL ROLE:
  Declares the public Layer 0 debugging, state-rewind, breakpoint, and validation
  probe interface shared by headless CLI checks, tests, and interactive X11
  developer overlays without leaking Layer 1 simulation types into engine/.

  ### For Game Designers & Non-Programmers:
  Acts as the simulation time-machine and safety-gate interface. Designers can
  step turn-by-turn, inspect locked emergency snapshots, check ASCII debug cards,
  and rely on clear AWAIT freezes instead of opaque crashes when tests fail.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Owns the singleton snapshot/probe tables internally;
    callers submit POD state descriptors and receive const views or diffs.
  - Extension Seam: Subsystems register validation probes via debug_register_probe()
    to participate uniformly in `--audit` and runtime tripwire latching.
  - Explicit Non-Goal: Does not interpret gameplay rules or store heap-allocated
    arbitrary entity graphs (uses decoupled fixed telemetry fields).
================================================================================
*/

#ifndef CORNERSTONE_DEBUG_DEBUG_H
#define CORNERSTONE_DEBUG_DEBUG_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Public Contract Limits, Fallback Link, Ontology & O(1) Pre-Conditions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug
Mission:       Declare limits, downward + fallback links, public types, and O(1) guards.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "debug/debug_types.h"
#include "debug/debug_fallback.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, VFS Paths & Compile-Time Proofs
--------------------------------------------------------------------------------
Operational VFS configuration routes and compile-time capacity proofs linking
shared ontology bounds (`debug_types.h`) to Tier-0 BIOS presets (`debug_fallback.h`).

### For Game Designers & Non-Programmers:
Identifies canonical JSON paths (`manifest.json`, `snapshot_policy.json`,
`tripwire_matrix.json`) and default profile (`"default"`) loaded at startup.

### For Systems Programmers & Engine Architects:
Guarantees 100% zero-heap `.bss` footprint bounds via `static_assert`.
--------------------------------------------------------------------------------
*/

/** Default configuration profile name for standard test execution. */
#define DEBUG_DEFAULT_PROFILE     "default"

/** Canonical virtual filesystem path to the debugger master manifest. */
#define DEBUG_MANIFEST_PATH         "diagnostics/debugger/manifest.json"

/** Canonical virtual filesystem path to snapshot retention policy JSON. */
#define DEBUG_SNAPSHOT_POLICY_PATH  "diagnostics/debugger/snapshot_policy.json"

/** Canonical virtual filesystem path to taxonomic tripwire reaction matrix JSON. */
#define DEBUG_TRIPWIRE_MATRIX_PATH  "diagnostics/taxonomy/tripwire_matrix.json"

static_assert(
    DEBUG_MAX_SNAPSHOTS >= DEBUG_FALLBACK_MAX_SNAPSHOTS,
    "Operational rewind ring capacity must meet or exceed BIOS fallback limit"
);
static_assert(
    DEBUG_MAX_VALIDATION_PROBES >= 16U,
    "Validation probe table must reserve at least 16 registration slots"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the engine's initialization DAG.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap Debugger Engine. */
#define DEBUG_MODULE_ID                "core.util.debug"

/**
 * Manifest record for the Zero-Heap Debugger Engine.
 */
extern const module_metadata_t debug_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions (Cross-Reference)
--------------------------------------------------------------------------------
Shared debugger ontology (`debug_tripwire_t`, `debug_tripwire_reaction_t`,
`debug_test_tier_t`, `debug_step_mode_t`, `debug_breakpoint_type_t`,
`debug_state_fields_t`, `debug_snapshot_t`, `debug_breakpoint_t`,
`debug_config_t`, `debug_validation_probe_fn`, `debug_validation_probe_t`,
`debug_tripwire_report_t`, `debug_stats_t`) lives in `debug/debug_types.h`
(`cog.debug.types`); BIOS templates and tripwire reaction rows live cleanly in
`debug/debug_fallback.h` (`cog.debug.fallback`).
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Operational Inlines
--------------------------------------------------------------------------------
Pure O(1) static inline operational pre-conditions complementing the shared
enum stringifiers and range predicates in `debug/debug_types.h`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying whether a rewind snapshot is currently locked.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether an important turn snapshot is pinned so newer turns cannot overwrite it.
 *
 * ### For Systems Programmers & Engine Architects:
 * Null-safe O(1) inspection of `snap->locked`.
 *
 * @param[in] snap Snapshot descriptor pointer to inspect.
 * @return true if non-NULL and pinned against ring eviction.
 */
static inline bool debug_snapshot_is_locked(const debug_snapshot_t *snap)
{
    return (snap != NULL) && snap->locked;
}

/**
 * @brief Predicate verifying whether any diagnostic traps are active in a config.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports whether the debugger is actively watching engine or gameplay safety traps.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) gate predicate checking `cfg->enabled` alongside trap domain switches.
 *
 * @param[in] cfg Configuration pointer to inspect.
 * @return true if enabled and at least one trap domain gate is armed.
 */
static inline bool debug_config_traps_enabled(const debug_config_t *cfg)
{
    return (cfg != NULL) && cfg->enabled &&
           (cfg->engine_traps_active || cfg->game_traps_active || cfg->break_on_tripwire);
}

/**
 * @brief Predicate verifying whether a registered probe descriptor holds usable metadata.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms an automated test trigger has an assigned name and owning system before registration.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) structural pre-condition checked before copying into `s_debug.probes[]`.
 *
 * @param[in] probe Probe descriptor pointer to validate.
 * @return true if non-NULL, active, and carries non-empty `probe_id` and `subsystem`.
 */
static inline bool debug_probe_slot_is_valid(const debug_validation_probe_t *probe)
{
    return (probe != NULL) &&
           probe->is_active &&
           (probe->probe_id[0] != '\0') &&
           (probe->subsystem[0] != '\0');
}

/**
 * @brief Convenience call-site macro capturing `__FILE__`, `__LINE__`, and `__func__`.
 *
 * ### For Game Designers & Non-Programmers:
 * Shortcut letting subsystems report a failed safety check while automatically attaching
 * the exact source file and line where the problem surfaced.
 *
 * ### For Systems Programmers & Engine Architects:
 * Placed in Sub-Assembly 2.4 per Cornerstone 5-Block macro placement rules; forwards
 * directly to Block 3 `debug_trigger_probe()` at every downstream translation-unit call site
 * without heap allocation or variadic overhead.
 *
 * @param[in] probe_id  Registered probe identifier string.
 * @param[in] subsystem Originating module tag.
 * @param[in] condition Human-readable anomaly description.
 * @return true if probe executed; false if suppressed by active reentrancy guard.
 */
#define DEBUG_TRIGGER_PROBE(probe_id, subsystem, condition) \
    debug_trigger_probe((probe_id), (subsystem), (condition), __FILE__, __LINE__, __func__)

/*
================================================================================
BLOCK 3: BODY BLOCK
( Public Operational Cogs, State Accessors & Domain APIs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug
Mission:       Declare composable snapshot ring, FNV-1a determinism, breakpoint,
               probe registry, AWAIT recovery, and tripwire reporting APIs.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Deterministic Hashing & Configuration Provenance Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Computes a deterministic 32-bit FNV-1a digest of simulation state fields.
 *
 * ### For Game Designers & Non-Programmers:
 * Creates a unique digital fingerprint for each turn so the engine can detect
 * silent memory corruption or divergence between headless and graphical runs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure deterministic 32-bit FNV-1a hash mixing every member of `debug_state_fields_t`
 * in canonical order with zero heap or global state writes.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`,
 *                   `tests/functional/test_debug_functional.c`
 *
 * @param[in] fields Actor/simulation state snapshot fields to digest.
 * @return 32-bit FNV-1a hash digest, or 0U if `fields == NULL`.
 */
uint32_t debug_compute_state_hash(const debug_state_fields_t *fields);

/**
 * @brief Overlays snapshot retention policy rules onto a debugger configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads rules from `snapshot_policy.json` controlling how many key snapshots stay
 * pinned against overwriting and whether safety checks run on every single turn.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `DEBUG_SNAPSHOT_POLICY_PATH` via Layer 0 VFS (`path_finder` `"config"`
 * domain -> `debug_fallback_parse_snapshot_policy_json()`), falling back cleanly
 * to `debug_fallback_load_rom_snapshot_policy()` in `.rodata` if disk JSON is absent.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in,out] cfg Configuration struct to enrich with ring & lock quotas.
 * @return true if loaded from disk VFS; false if satisfied via ROM/defaults or bad args.
 */
bool debug_load_snapshot_policy(debug_config_t *cfg);

/**
 * @brief Loads taxonomic tripwire reaction rules from disk VFS or ROM fallback.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads `tripwire_matrix.json` to map specific alarms (bad scalar bounds, corrupt saves,
 * or failed safety tests) to automatic responses like clamping or Safe-Mode pause.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `diagnostics/taxonomy/tripwire_matrix.json` via `path_finder` and parses
 * up to `max_rules` (`<= DEBUG_FALLBACK_MAX_TRIPWIRE_RULES`, 16) via the shared
 * zero-heap parser, degrading automatically to compiled `.rodata` ROM on disk miss.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[out] out_rules Destination rule array.
 * @param[in]  max_rules Maximum rule capacity.
 * @param[out] out_count Number of parsed reaction rules.
 * @return true if loaded from disk VFS; false if satisfied via ROM fallback or error.
 */
bool debug_load_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
);

/**
 * @brief Loads a specific named debug profile (e.g. `"default"`, `"strict"`, `"trace"`).
 *
 * ### For Game Designers & Non-Programmers:
 * Selects a pre-tuned debugger behavior profile from external config files so modders
 * and QA testers can toggle trace verbosity and safety break conditions by preset name.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `<profile_name>.json` via `master_loader`, delegates parsing to shared
 * `debug_fallback_parse_config_json()`, overlays `snapshot_policy.json`, and falls back
 * to Safe-Mode monolith/ROM while latching tripwires on missing or malformed profiles.
 *
 * @triggers
 * - Layer 4 Tests : `tests/integrated/test_debug_integrated.c:Section2`
 *
 * @param[in]  profile_name Profile stem (e.g. `"default"`, `"strict"`, `"trace"`).
 * @param[out] out_config   Caller destination configuration structure.
 * @return true if profile resolved cleanly (including graceful missing-profile fallback);
 *         false on invalid arguments, schema errors, or bounds corruption.
 */
bool debug_load_config_profile(const char *profile_name, debug_config_t *out_config);

/**
 * @brief Loads a debug configuration through the layered debugger menu manifest.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the debugger manifest to determine which preset (`"active_profile"`) is currently
 * selected, degrading cleanly to factory Safe-Mode rules if disk manifests are missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves manifest path via `master_loader` `"config"` VFS, verifies schema
 * `"tenlux.data.debug_menu.v1"`, reads `"active_profile"`, and delegates to
 * `debug_load_config_profile()`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/integrated/test_debug_integrated.c:Section2`
 *
 * @param[in]  manifest_path Relative VFS manifest path (NULL uses `DEBUG_MANIFEST_PATH`).
 * @param[out] out_config    Populated configuration structure (fallback-filled on miss).
 * @return true if manifest and referenced profile loaded from VFS; false otherwise.
 */
bool debug_load_config_from_manifest(const char *manifest_path, debug_config_t *out_config);

/**
 * @brief Returns the quarantined static emergency fallback configuration by value.
 *
 * ### For Game Designers & Non-Programmers:
 * Supplies factory default Safe-Mode settings whenever external configuration files
 * cannot be loaded from disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Invokes `debug_get_monolith_fallback_config_raw()` into a stack struct returning
 * `.rodata` defaults with `loaded_from_data == false`.
 *
 * @return Static fallback `debug_config_t` copy with Safe-Mode provenance marked.
 */
debug_config_t debug_get_monolith_fallback_config(void);

/**
 * @brief Returns the active default configuration resolved via manifest, ROM, or monolith.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the standard startup settings for the debugger cleanly whether running from
 * full disk assets, read-only ROM backups, or bare Safe Mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Cascades deterministically: disk manifest VFS -> `debug_fallback_load_rom_profile()` ->
 * `debug_get_monolith_fallback_config()`, avoiding heap allocation at every fallback step.
 *
 * @return Resolved `debug_config_t` preset ready for `debug_init()`.
 */
debug_config_t debug_get_default_config(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Debugger State Queries, Step Control & AWAIT Recovery Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Checks if the primary debugging subsystem is currently initialized and active.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells menus and diagnostic overlays whether state rewind recording is currently armed.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) lock-free accessor reading module active gate `s_debug_active`.
 *
 * @return true if `debug_init()` completed and `debug_shutdown()` has not run.
 */
bool debug_is_active(void);

/**
 * @brief Returns the name string of the active debugging profile.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays which preset (`"default"`, `"strict"`, `"trace"`, `"monolith_fallback"`)
 * is currently governing debugger checks, or `"inactive"` if shut down.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns immutable `"inactive"` literal when uninitialized or pointer into static
 * `.bss` `s_debug.config.profile_name`.
 *
 * @return Null-terminated profile identifier string (never NULL).
 */
const char *debug_get_active_profile_name(void);

/**
 * @brief Sets the interactive execution stepping mode cleanly.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches playback speed between continuous run, paused hold, single-step advance, or
 * emergency AWAIT halt. While an alarm keeps playback locked in AWAIT, manual switching
 * remains blocked until repaired state clears the alarm.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates enum domain (`debug_step_mode_is_valid`) and enforces AWAIT interlock
 * whenever `s_debug.await_active && tripwire_status != DEBUG_TRIPWIRE_CLEAR`.
 *
 * @param[in] mode Target stepping mode (`debug_step_mode_t`).
 */
void debug_set_step_mode(debug_step_mode_t mode);

/**
 * @brief Retrieves the active execution stepping mode.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports whether simulation playback is currently running freely, paused, stepping, or halted in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) accessor reading `s_debug.step_mode`.
 *
 * @return Current `debug_step_mode_t` state.
 */
debug_step_mode_t debug_get_step_mode(void);

/**
 * @brief Queries whether the debugger is currently halted idle in `DEBUG_STEP_AWAIT`.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells the game loop or UI whether an anomaly halted playback in Safe-Mode AWAIT
 * waiting for you to rewind or fix the broken value.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) predicate returning true when initialized and either `s_debug.await_active`
 * is latched or `s_debug.step_mode == DEBUG_STEP_AWAIT`.
 *
 * @return true if AWAIT latch is engaged.
 */
bool debug_is_awaiting(void);

/**
 * @brief Returns true if execution may advance (debugger inactive or not blocked in AWAIT).
 *
 * ### For Game Designers & Non-Programmers:
 * Gatekeeper checked before advancing a simulation turn: prevents turns from rolling
 * forward while paused or halted on an unresolved error.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) scheduler predicate permitting advance only when `!s_debug_active` or
 * `(!s_debug.await_active && step_mode != DEBUG_STEP_PAUSED && step_mode != DEBUG_STEP_AWAIT)`.
 *
 * @return true if simulation stepping is permitted; false if blocked in PAUSED/AWAIT.
 */
bool debug_can_step(void);

/**
 * @brief Addresses an active AWAIT halt by validating repaired state and clearing latch.
 *
 * ### For Game Designers & Non-Programmers:
 * Exits emergency pause once you have rewound to a healthy turn or repaired
 * invalid numbers, verifying the repaired state passes validation before resuming.
 *
 * ### For Systems Programmers & Engine Architects:
 * Optionally validates `restored_fields` against scalar bounds (HP, alignment sway `[-100, +100]`,
 * primary/secondary channels `[0, 100]`), re-runs any failed probe callback, resets
 * AWAIT + tripwire latches cleanly, and synchronizes `debug_fallback_clear_await()`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`
 *
 * @param[in] restored_fields Optional repaired/rewound state to validate (may be NULL).
 * @param[in] resume_mode     Desired mode after clearing AWAIT (`DEBUG_STEP_RUN` default).
 * @param[in] resolution_note Explanation logged to telemetry & Safe-Mode history.
 * @return true if validation succeeded and AWAIT cleared; false if state or probe still failing.
 */
bool debug_resolve_await(
    const debug_state_fields_t *restored_fields,
    debug_step_mode_t           resume_mode,
    const char                 *resolution_note
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Snapshots, Locked Retention, Time-Travel Rewind & Diff Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Captures and records a state snapshot into the static ring buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Saves a freeze-frame picture of actor vitals, position, and channel values
 * at a specific simulation turn so you can rewind back to it.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies fields into static circular ring buffer respecting locked-slot eviction rules,
 * computes 32-bit FNV-1a checksum, links `log_get_total_entries()`, and advances head index.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in] fields State fields to capture.
 * @param[in] cmd    Command string that caused this state transition (may be NULL).
 * @param[in] arg    Argument string associated with the command (may be NULL).
 * @return Unique monotonic snapshot ID, or 0ULL if inactive or `fields == NULL`.
 */
uint64_t debug_record_snapshot(const debug_state_fields_t *fields,
                               const char *cmd, const char *arg);

/**
 * @brief Marks a recorded snapshot slot as locked against circular FIFO eviction.
 *
 * ### For Game Designers & Non-Programmers:
 * Pins an important turn snapshot (like a phase transition or anomaly catch)
 * so newer turns cannot overwrite it before you inspect or rewind back to it.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scans the static 64-slot ring (`O(N <= 64)`) to pin `snapshot_id` (or newest slot
 * when `snapshot_id == 0ULL`). Enforces `locked_slot_quota` (`<= max_snapshots`) by
 * evicting the oldest non-critical locked slot cleanly with zero heap allocation.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in] snapshot_id Monotonic snapshot ID to pin (0 locks most recent).
 * @param[in] lock_reason Reason tag (e.g. `"TRIGGER_TRIPWIRE_LATCH"`).
 * @return true if snapshot was found and locked (respecting `locked_slot_quota`).
 */
bool debug_lock_snapshot(uint64_t snapshot_id, const char *lock_reason);

/**
 * @brief Returns the total number of valid snapshots currently recorded in the ring.
 *
 * ### For Game Designers & Non-Programmers:
 * Counts how many saved turn frames are currently available for inspection or rewind.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) accessor returning `s_debug.snapshot_count` when active (capped at `max_snapshots`).
 *
 * @return Count of recorded snapshots in static ring, or 0U if uninitialized.
 */
size_t debug_get_snapshot_count(void);

/**
 * @brief Retrieves a historical snapshot by chronological index (0 = oldest valid).
 *
 * ### For Game Designers & Non-Programmers:
 * Looks up a saved turn snapshot from history so menus or diffs can inspect it safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Maps logical index `0..snapshot_count-1` to physical ring offset cleanly and copies
 * `debug_snapshot_t` by value into caller storage with zero heap allocation.
 *
 * @param[in]  index    Zero-based chronological snapshot offset.
 * @param[out] out_snap Caller destination snapshot structure.
 * @return true if snapshot existed and copied cleanly; false if inactive or out of range.
 */
bool debug_get_snapshot(size_t index, debug_snapshot_t *out_snap);

/**
 * @brief Retrieves a read-only pointer to the newest snapshot recorded in the ring.
 *
 * ### For Game Designers & Non-Programmers:
 * Inspects the latest saved frame directly without stepping backward in time.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) lookup returning `const debug_snapshot_t *` inside `.bss` `s_debug.snapshots[]`
 * (valid until subsequent wrap overwrites that slot).
 *
 * @return Pointer to latest `debug_snapshot_t`, or NULL if inactive or empty.
 */
const debug_snapshot_t *debug_get_latest_snapshot(void);

/**
 * @brief Restores state fields from a historical snapshot after verifying its checksum.
 *
 * ### For Game Designers & Non-Programmers:
 * Rewinds simulation state back to an earlier saved turn after ensuring the stored
 * data was not tampered with or corrupted in memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Recomputes 32-bit FNV-1a checksum against stored `snap.state_hash`; mismatches latch
 * `DEBUG_TRIPWIRE_HASH_MISMATCH`, engage Safe-Mode AWAIT, and reject corrupt restores.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in]  index      Historical snapshot index to rewind to.
 * @param[out] out_fields Populated state fields upon checksum-verified rewind.
 * @return true if rewind succeeded, false on invalid index or checksum mismatch.
 */
bool debug_rewind_to_snapshot(size_t index, debug_state_fields_t *out_fields);

/**
 * @brief Computes a formatted domain-neutral textual diff between two snapshots.
 *
 * ### For Game Designers & Non-Programmers:
 * Compares two recorded turns and highlights exactly which stats, channels, stances,
 * or coordinates shifted between them.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats changed scalar/channel diffs and hash transitions into caller `out_buf` via
 * bounded cursor formatting using temporary 1 KiB stack scratch.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`,
 *                   `tests/functional/test_debug_functional.c`
 *
 * @param[in]  s1      First snapshot (earlier baseline).
 * @param[in]  s2      Second snapshot (later target).
 * @param[out] out_buf Destination text buffer.
 * @param[in]  buf_len Size of destination buffer in bytes.
 * @return Number of formatted characters written to `out_buf`, or 0U on error.
 */
size_t debug_diff_snapshots(const debug_snapshot_t *s1, const debug_snapshot_t *s2,
                            char *out_buf, size_t buf_len);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Conditional Breakpoints & Domain Guardrail Watchpoints Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Registers a dynamic conditional breakpoint in the static table.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets an automatic pause rule (e.g. pausing when HP drops below a threshold, on turn N,
 * or when stance switches) during test runs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Appends `debug_breakpoint_t` entry into `.bss` `s_debug.breakpoints[DEBUG_MAX_BREAKPOINTS]`
 * with monotonic positive handle assignment; fails cleanly with warning when table is full.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in] type         Breakpoint trigger category (`debug_breakpoint_type_t`).
 * @param[in] target_value Target scalar comparison value.
 * @param[in] label        Optional descriptive label (defaults to `"Breakpoint"`).
 * @return Positive breakpoint ID handle, or -1 if inactive, invalid type, or table full.
 */
int debug_add_breakpoint(debug_breakpoint_type_t type, int target_value, const char *label);

/**
 * @brief Removes an active conditional breakpoint by assigned handle ID.
 *
 * ### For Game Designers & Non-Programmers:
 * Deletes a previously created pause rule once you no longer need the debugger to stop there.
 *
 * ### For Systems Programmers & Engine Architects:
 * Linearly searches `s_debug.breakpoints[]` (max 16 slots) and compacts subsequent
 * elements cleanly via shift loop with zero heap operations.
 *
 * @param[in] bp_id Positive handle returned by `debug_add_breakpoint()`.
 * @return true if matching breakpoint was removed; false if inactive or not found.
 */
bool debug_remove_breakpoint(int bp_id);

/**
 * @brief Clears all registered conditional breakpoints immediately.
 *
 * ### For Game Designers & Non-Programmers:
 * Removes every active turn/stat pause rule in one step.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) counter reset setting `s_debug.breakpoint_count = 0U`.
 */
void debug_clear_breakpoints(void);

/**
 * @brief Evaluates validation probes and conditional breakpoints against live fields.
 *
 * ### For Game Designers & Non-Programmers:
 * Sweeps active safety rules and pause conditions after a step to stop gameplay cleanly
 * whenever an invariant breaks or a breakpoint target matches.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks `verify_on_step` registered probes, scalar range guardrails (alignment `[-100, 100]`,
 * channels `[0, 100]`, zero HP), and `s_debug.breakpoints[]`, formatting exact trigger reason
 * into `out_reason`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in]  fields     Current simulation state fields.
 * @param[out] out_reason Optional buffer populated with trigger description.
 * @param[in]  max_len    Byte capacity of `out_reason`.
 * @return true if a probe or breakpoint halted execution; false otherwise.
 */
bool debug_check_breakpoints(const debug_state_fields_t *fields,
                             char *out_reason, size_t max_len);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Validation Probes, Tripwires & Forensic Reporting Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Registers a subsystem Block 4 / Layer 4 validation probe with the debugger.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects an automated self-check test to the debugger so that if an anomaly
 * or failing rule occurs, the matching test fires immediately, Safe Mode
 * finishes the interrupted action safely, and playback halts in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stores probe metadata (exact source function/block section, Block 4 `validate_fn`,
 * and Layer 4 test suite reference) in the static `s_debug.probes[]` table.
 * Idempotent on identical `probe_id` (updates slot cleanly without leaking).
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section2`
 *
 * @param[in] probe Populated validation probe descriptor.
 * @return Zero-based probe slot index on success, or -1 if capacity reached/invalid.
 */
int debug_register_validation_probe(const debug_validation_probe_t *probe);

/**
 * @brief Unregisters all active validation probes (used cleanly in isolated tests).
 *
 * ### For Game Designers & Non-Programmers:
 * Resets the list of automatic self-check probes between isolated test runs so tests
 * do not interfere with each other.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) static memory zeroing (`memset` over `s_debug.probes[DEBUG_MAX_VALIDATION_PROBES]`)
 * resetting `s_debug.probe_count = 0`. Never touches BIOS fault history in `s_fb`.
 */
void debug_clear_validation_probes(void);

/**
 * @brief Queries how many validation probe descriptors are currently registered.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports how many automatic subsystem safety checks (`log.*`, `io.*`, or
 * custom test probes) are currently plugged into the engine debugger.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) lock-free accessor returning active slot count `s_debug.probe_count`
 * (`<= DEBUG_MAX_VALIDATION_PROBES`). Safe to query before `debug_init()`.
 *
 * @return Active validation probe count in `[0, DEBUG_MAX_VALIDATION_PROBES]`.
 */
size_t debug_get_probe_count(void);

/**
 * @brief Copies a registered validation probe descriptor by zero-based index.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets status panels, bug reports, or test harnesses inspect each active safety
 * probe along with how many times it has fired and which test suite owns it.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) bounded snapshot copy from `s_debug.probes[index]` into caller storage.
 *
 * @param[in]  index     Zero-based probe slot (`< debug_get_probe_count()`).
 * @param[out] out_probe Caller destination probe descriptor copy.
 * @return true if slot valid and copied; false on NULL pointer or out-of-range index.
 */
bool debug_get_probe_at(size_t index, debug_validation_probe_t *out_probe);

/**
 * @brief Searches registered validation probes by identifier and copies metadata.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether a specific safety test trigger (such as `"log.setting_profile"`
 * or `"io.vfs.route_bounds"`) is currently wired into the debugger.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(P) scan over `s_debug.probe_count` (`<= 32`) active slots matching exact
 * `probe_id`. Passing `out_probe == NULL` performs a fast existence check.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section1`
 *
 * @param[in]  probe_id  Unique registered probe identifier string.
 * @param[out] out_probe Optional destination descriptor copy (may be NULL).
 * @return true if an active matching probe exists; false otherwise.
 */
bool debug_find_probe(const char *probe_id, debug_validation_probe_t *out_probe);

/**
 * @brief Non-destructively executes one registered probe's Block 4 callback.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets UI tools or repair scripts re-run a single subsystem self-test without
 * pausing gameplay in `AWAIT`, confirming whether an error is truly fixed first.
 *
 * ### For Systems Programmers & Engine Architects:
 * Locates `probe_id` under `in_probe` reentrancy protection and calls `validate_fn()`
 * without mutating tripwire latches, Safe-Mode flags, or step modes. Probes
 * without a callback yield `SEVERITY_PASS` when active.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c:Section3`
 *
 * @param[in]  probe_id   Registered probe identifier string.
 * @param[out] out_result Optional diagnostic result payload (may be NULL).
 * @return true if probe exists and passed (`severity < SEVERITY_CRITICAL`); false otherwise.
 */
bool debug_evaluate_probe(const char *probe_id, diag_result_t *out_result);

/**
 * @brief Fires a registered validation probe (or fallback anomaly tripwire) immediately.
 *
 * ### For Game Designers & Non-Programmers:
 * Immediately runs the matching self-check test when an anomaly occurs, records exact
 * file/line clues, finishes any interrupted action in Safe Mode, pins the current
 * snapshot against erasure, and pauses execution in `AWAIT` until inspected and fixed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Guarded by `s_debug.in_probe` against reentrant recursion. Matches `probe_id`,
 * evaluates optional `validate_fn()`, latches tripwire + `debug_fallback_record_fault()`,
 * completes Safe Mode via `debug_fallback_finish_with_fallback()`, locks latest snapshot,
 * and enters `DEBUG_STEP_AWAIT` when armed.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c` (Sec 2),
 *                   `tests/unit/log/test_log_unit.c` (Sec 2 & 3)
 *
 * @param[in] probe_id  Registered probe identifier (e.g., `"log.setting_profile"`).
 * @param[in] subsystem Subsystem raising the event.
 * @param[in] condition Detailed anomaly or failure reason.
 * @param[in] file      Source file anchor (`__FILE__`).
 * @param[in] line      Source line anchor (`__LINE__`).
 * @param[in] func      Source function anchor (`__func__`).
 * @return true if probe executed and completed fallback/await handling; false if reentrant.
 */
bool debug_trigger_probe(
    const char *probe_id,
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *func
);

/**
 * @brief Evaluates all registered validation probes against live state invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Sweeps every registered health probe (vitals/channel bounds, telemetry rules, audio/VFS
 * invariants) at step checkpoints; if anything fails, automatically triggers Safe-Mode
 * fallback protection and pauses cleanly in `AWAIT`.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes bounds validation on optional `fields` (`dbg_state_fields_are_sane`) plus
 * every registered `validate_fn` in `s_debug.probes[]` (`<= 32` slots). Any failing
 * result (`!res.passed` or `SEVERITY_CRITICAL+`) invokes `debug_trigger_probe()`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c` (Sec 2),
 *                   `tests/integrated/test_debug_integrated.c` (Sec 2 & 4)
 *
 * @param[in] fields Optional live state snapshot fields checked for bounds (may be NULL).
 * @return Number of probes that failed and triggered fallback -> AWAIT.
 */
size_t debug_check_tripwires(const debug_state_fields_t *fields);

/**
 * @brief Traps a legacy tripwire event in the debugger without crashing.
 *
 * @deprecated Legacy un-probed tripwire shim retained until unmodernized
 *             `core/` modules migrate; new code must register probes via
 *             `debug_register_validation_probe()` and call `debug_trigger_probe()`.
 *
 * ### For Game Designers & Non-Programmers:
 * Catches an engine warning or rule breach so the game can inspect it safely without crashing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Latches `DEBUG_TRIPWIRE_STATE_CORRUPT`, increments `traps_triggered`, forwards into
 * `debug_fallback_record_fault()`, pins newest snapshot, arms AWAIT if configured, and
 * emits a reentrancy-guarded `LOG_WARN`.
 *
 * @param[in] subsystem Triggering subsystem tag.
 * @param[in] condition Description of tripped invariant condition.
 * @param[in] file      Originating source file (`__FILE__`).
 * @param[in] line      Originating source line (`__LINE__`).
 */
void debug_trap_tripwire(const char *subsystem, const char *condition,
                         const char *file, int line);

/**
 * @brief Retrieves the latest forensic tripwire, fallback, and AWAIT status report.
 *
 * ### For Game Designers & Non-Programmers:
 * Copies a complete crash-free incident report (which test failed, source file/line,
 * emergency Safe-Mode action taken, and whether a rewind snapshot was pinned).
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) zero-heap struct copy from `s_debug.last_report` (`debug_tripwire_report_t`).
 *
 * @param[out] out_report Destination tripwire report struct.
 * @return true on success; false on NULL pointer.
 */
bool debug_get_tripwire_report(debug_tripwire_report_t *out_report);

/**
 * @brief Formats a human-readable forensic tripwire / AWAIT summary into `out_buf`.
 *
 * ### For Game Designers & Non-Programmers:
 * Formats the latest error report into clean readable lines for terminal logs, crash
 * overlays, or bug reports without modifying any game state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded `snprintf` formatter serializing `s_debug.last_report` into caller buffer
 * `out_buf` (`buf_len >= 128`); returns false if truncated or given invalid args.
 *
 * @param[out] out_buf Destination buffer.
 * @param[in]  buf_len Byte capacity of `out_buf`.
 * @return true if formatted without truncation; false otherwise.
 */
bool debug_dump_tripwire_report(char *out_buf, size_t buf_len);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
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
 * @brief Checks if active debugger configuration was loaded from data assets.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies if debugger settings were loaded from JSON files rather than emergency fallback code.
 *
 * ### For Systems Programmers & Engine Architects:
 * Invariant contract verification query returning loaded_from_data boolean flag.
 *
 * @return true if loaded from JSON profile; false if running on emergency fallback defaults.
 */
bool debug_is_data_driven(void);

/**
 * @brief Queries whether the debugger is currently running in Safe Mode / fallback.
 *
 * ### For Game Designers & Non-Programmers:
 * Indicates whether the debugger is operating from built-in ROM/factory defaults or
 * stepped down into emergency Safe Mode after catching an error.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) query returning `s_debug_active && (s_debug.safe_mode || !s_debug_data_driven)`.
 *
 * @return true if active without disk-loaded config (monolith or ROM Safe Mode).
 */
bool debug_is_safe_mode(void);

/**
 * @brief Returns the latched tripwire status.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if any tripwires are currently tripped.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns debug_tripwire_t enum status code.
 *
 * @return Active debug_tripwire_t status code indicating error state.
 */
debug_tripwire_t debug_get_tripwire_status(void);

/**
 * @brief Returns details of the latched tripwire.
 *
 * ### For Game Designers & Non-Programmers:
 * Explains what caused a tripwire warning.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns static string pointer to latched tripwire message.
 *
 * @return Constant string containing subsystem, condition, file, and line details.
 */
const char *debug_get_tripwire_details(void);

/**
 * @brief Resets the debugger tripwire latch.
 *
 * ### For Game Designers & Non-Programmers:
 * Clears any tripwire warnings once inspected.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets tripwire latch back to DEBUG_TRIPWIRE_CLEAR.
 */
void debug_reset_tripwire(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
@triggers
  - Primary Cogs  : `debug_preflight()`, `debug_audit()` (wired into `debug_manifest`)
  - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c` (Sec 1..4),
                    `tests/functional/test_debug_functional.c` (Sec 1..4),
                    `tests/integrated/test_debug_integrated.c` (Sec 1..4),
                    `tests/system/test_debug_system.c` (Sec 1..4)
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check for debugger subsystem invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs an instant health check on the state rewind and snapshot system before gameplay starts.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-pure diagnostic verifying `debug_fallback_preflight()`, FNV-1a bitwise
 * determinism & collision sensitivity, and `debug_fallback_execute_inspect()`
 * without mutating `s_debug`.
 *
 * @triggers
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c` (Sec 1),
 *                   `tests/functional/test_debug_functional.c` (Sec 1)
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t debug_preflight(void);

/**
 * @brief High-level diagnostic self-audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Comprehensive automated inspection verifying that turns can be recorded, pinned,
 * rewound, paused on breakpoints, and recovered from Safe-Mode AWAIT without losing
 * live gameplay progress.
 *
 * ### For Systems Programmers & Engine Architects:
 * Non-destructive audit preserving/restoring `s_debug` in static `.bss` (`s_audit_backup`):
 * runs preflight + ROM audit, tests snapshot lock & rewind, breakpoints, AWAIT reject/resolve,
 * and 60-column inspection cards (`CARD_HERO_VITALS`, `CARD_TRIPWIRE_DIAGNOSTIC`).
 *
 * @triggers
 * - CLI Entry     : `./build/tenebris-lux --audit`
 * - Layer 4 Tests : `tests/unit/debug/test_debug_unit.c` (Sec 1..4),
 *                   `tests/system/test_debug_system.c` (Sec 1..4)
 *
 * @param[in] verbose Enable verbose trace output during audit.
 * @return diag_result_t diagnostic outcome indicating overall subsystem health.
 */
diag_result_t debug_audit(bool verbose);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & State Rewind
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
 * @brief Constructs a structured runtime debugger statistics report.
 *
 * ### For Game Designers & Non-Programmers:
 * Collects a summary of the interactive debugger (active snapshots, memory
 * limit warnings, and current inspection mode) for display in menus or status bars.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection procedure populating a `debug_stats_t` record
 * from current ring pointers, tripwire registers, and snapshot indexes.
 *
 * @param[out] out_stats Destination statistics report object.
 * @return true if statistics were populated; false on NULL parameter.
 */
bool debug_execute_inspect(debug_stats_t *out_stats);

/**
 * @brief Formats an ASCII inspection card readout for the current simulation state.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean 60-column terminal status card showing actor vitals, channel charge,
 * stance posture, or tripwire status during interactive debugging sessions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a deterministic ASCII inspection card into `out_buf` formatted to 60-column
 * terminal boundaries. Supports template identifiers `"CARD_HERO_VITALS"`,
 * `"CARD_STATERA_LEDGER"`, `"CARD_COMBAT_ENGAGEMENT"`, `"CARD_ENGINE_HEARTBEAT"`, and
 * `"CARD_TRIPWIRE_DIAGNOSTIC"`.
 *
 * @param[in]  card_id Target template identifier (e.g. `"CARD_HERO_VITALS"`).
 * @param[in]  fields  State fields to populate card with (nullable for engine-only
 *                     `"CARD_ENGINE_HEARTBEAT"` and `"CARD_TRIPWIRE_DIAGNOSTIC"`).
 * @param[out] out_buf Destination text buffer.
 * @param[in]  buf_len Size of destination buffer in bytes (`>= 128`).
 * @return true if formatted successfully; false on invalid arguments or truncation.
 */
bool debug_execute_dump_card(const char *card_id, const debug_state_fields_t *fields,
                             char *out_buf, size_t buf_len);

/**
 * @brief Prints a formatted console readout summarizing live simulation state fields.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a clean multi-column console overview of the active actor's vitals,
 * alignment sway, channel gauges, and latched tripwire counters.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes stdout terminal procedure hashing `fields` via `debug_compute_state_hash()`
 * and formatting columns with domain-neutral labels.
 *
 * @param[in] fields Simulation state fields to display (no-op if NULL).
 */
void debug_dump_state(const debug_state_fields_t *fields);

/**
 * @brief Prints a tabular chronological timeline of all recorded ring snapshots.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a turn-by-turn history table showing past commands, scalar changes, and lock
 * flags stored inside the debugger's rewind memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Iterates logical indices `0..snapshot_count-1` via `debug_get_snapshot()` and
 * emits an aligned ASCII diagnostic table to `stdout`.
 */
void debug_dump_snapshot_history(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the debugging system with a specific configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts up the diagnostic debugger at game boot so snapshots can be recorded,
 * rewind commands can be executed, and errors can be diagnosed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets up the snapshot ring buffer, resets sequence counters, and initializes
 * the tripwire matrix. If config is NULL, attempts data-driven load; falls back
 * gracefully to monolithic fallback if manifests are missing.
 *
 * @param[in] config Optional custom configuration, or NULL for data-driven default.
 * @return true on successful initialization, false on fatal failure.
 */
bool debug_init(const debug_config_t *config);

/**
 * @brief Shuts down the debugging system and resets state.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely turns off the diagnostic debugger and frees snapshot buffers when the game exits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears active snapshots, resets the ring buffer index, and marks the subsystem inactive.
 */
void debug_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_DEBUG_DEBUG_H */
