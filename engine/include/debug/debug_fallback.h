/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/debug/debug_fallback.h
@id            cog.debug.fallback
@layer         Layer 0: Core Utilities
@archetype     quarantined_fallback | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Engine Debugger Safe-Mode BIOS & Diagnostics ROM Vault
@role          Public Contract & Seam: Tier-0 debugger BIOS circuit breaker,
               compiled ROM accessors, shared schema parsers, and AWAIT latch API.
@maturity      Rev 5 Baseline | Shared parsers, 9 embedded debugger/taxonomy ROMs & AWAIT circuit breaker ready.
@chassis       zero-heap | POD fault registers & Safe-Mode card buffers exposed cleanly with zero dynamic state.
@scale         O(n) linear scan across JSON policy keys; scales over 9 compiled diagnostics & taxonomy ROM tables.
@seams         in: diagnostics_rom.h | uses: cog.manifest, cog.debug.types, cog.log.fallback (quarantined) | probes: debug_fb.* | dag: core.util.debug_fallback
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Quarantined include invariant: includes only core/manifest.h and debug/debug_types.h (never debug.h or log.h).
               3. Monolith & ROM Safe-Mode templates accept strongly typed debug_config_t * and preserve loaded_from_data = false.
               4. Failed probes complete interrupted work in Safe Mode first, then latch AWAIT cleanly.

PURPOSE & ARCHITECTURAL ROLE:
  Declares the quarantined Tier-0 Safe-Mode debugger circuit breaker, shared
  JSON policy/matrix parsers, and compiled diagnostics/taxonomy ROM vault so
  debugger failures or diskless launches never blind the runtime or abort.

  ### For Game Designers & Non-Programmers:
  Provides the engine's circuit breaker and built-in emergency rulebook. Even if
  config folders vanish or a safety test trips mid-turn, Safe Mode finishes the
  broken step harmlessly and pauses in AWAIT so healthy snapshots stay intact.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Exposes immutable ROM queries and shares stateless
    JSON parsers (`debug_profile.v1`, `snapshot_policy.v1`, `tripwire_matrix.v1`)
    with `cog.debug` cleanly via `debug_types.h` without caller heap allocation.
  - Extension Seam: Add JSON files under diagnostics/debugger/ or taxonomy/ to
    expand compiled ROM coverage via cmake/GenerateConfigRom.cmake automatically.
  - Explicit Non-Goal: Does not store full 64-turn rewind history (delegated
    upward to cog.debug).
================================================================================
*/

#ifndef CORNERSTONE_DEBUG_DEBUG_FALLBACK_H
#define CORNERSTONE_DEBUG_DEBUG_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Tier-0 Safe-Mode BIOS Quarantine, ROM Contracts & Standalone Frontier )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug.fallback
Mission:       Quarantined standalone BIOS interface free of primary headers.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers (Quarantined) --- */
#include "core/manifest.h"
#include "debug/debug_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, BIOS Limits & Compile-Time Proofs
--------------------------------------------------------------------------------
Compile-time static defaults for emergency boot and headless test execution.

### For Game Designers & Non-Programmers:
Guarantees that even if debugger setting files are missing, the game can still
pause on errors, record up to 64 turns, and dump state cards cleanly.

### For Systems Programmers & Engine Architects:
Immutable constants residing in `.rodata` with zero heap allocation and
compile-time bounds proofs tying BIOS limits cleanly to `debug_types.h`.
--------------------------------------------------------------------------------
*/

/** Baseline profile name for the emergency debugger fallback. */
#define DEBUG_FALLBACK_PROFILE_NAME         "monolith_fallback"

/** Compact 1.44 MB Floppy Safe-Mode profile identifier string. */
#define DEBUG_FALLBACK_COMPACT_PROFILE_NAME "floppy_debug_safe"

/** Fallback snapshot ring buffer capacity limit (64 turns). */
#define DEBUG_FALLBACK_MAX_SNAPSHOTS        64

/** Compact Floppy Safe-Mode snapshot ring capacity (16 turns). */
#define DEBUG_FALLBACK_FLOPPY_SNAPSHOTS     16

/** Fallback turn interval between automatic state snapshots (every 1 turn). */
#define DEBUG_FALLBACK_INTERVAL_TURNS       1

/** Fallback active telemetry channel filter list. */
#define DEBUG_FALLBACK_TRACE_CHANNELS       "sim,combat,world,dispatcher"

/** Fallback breakpoint trigger policy on general errors. */
#define DEBUG_FALLBACK_BREAK_ON_ERROR       false

/** Fallback breakpoint trigger policy on diagnostic tripwires. */
#define DEBUG_FALLBACK_BREAK_ON_TRIPWIRE    true

/** Fallback breakpoint trigger policy on moral sway breaches. */
#define DEBUG_FALLBACK_BREAK_ON_SWAY        true

/** Fallback breakpoint trigger policy on weapon resonance overcharge. */
#define DEBUG_FALLBACK_BREAK_ON_RESONANCE   true

/** Fallback breakpoint trigger policy on hero HP reaching zero. */
#define DEBUG_FALLBACK_BREAK_ON_HP_ZERO     true

/** Fallback breakpoint trigger policy on combat stance shifts. */
#define DEBUG_FALLBACK_BREAK_ON_STANCE      false

/** Fallback automatic state card dump on tripwire latch. */
#define DEBUG_FALLBACK_AUTO_DUMP            true

/** Fallback interactive REPL activation flag. */
#define DEBUG_FALLBACK_INTERACTIVE_REPL     true

/** Fallback ANSI color escape formatting flag. */
#define DEBUG_FALLBACK_ANSI_COLOR           true

/** Fallback domain trap gate for engine infrastructure checks. */
#define DEBUG_FALLBACK_ENGINE_TRAPS_ACTIVE  true

/** Fallback domain trap gate for Layer 1 gameplay invariant checks. */
#define DEBUG_FALLBACK_GAME_TRAPS_ACTIVE    true

/** Fallback locked critical snapshot slot quota (from snapshot_policy.json). */
#define DEBUG_FALLBACK_LOCKED_SLOT_QUOTA    4U

/** Fallback continuous step verification flag (triggers validation probes). */
#define DEBUG_FALLBACK_VERIFY_ON_STEP       true

/** Fallback automatic transition into DEBUG_STEP_AWAIT after tripwire handling. */
#define DEBUG_FALLBACK_AUTO_AWAIT           true

/** Maximum tripwire reaction rules parsed from tripwire_matrix.json. */
#define DEBUG_FALLBACK_MAX_TRIPWIRE_RULES   16U

static_assert(
    DEBUG_FALLBACK_MAX_SNAPSHOTS <= DEBUG_MAX_SNAPSHOTS,
    "Fallback snapshot cap must remain within shared ring bounds"
);
static_assert(
    DEBUG_FALLBACK_FLOPPY_SNAPSHOTS <= DEBUG_FALLBACK_MAX_SNAPSHOTS &&
    DEBUG_FALLBACK_LOCKED_SLOT_QUOTA <= DEBUG_FALLBACK_FLOPPY_SNAPSHOTS,
    "Compact floppy Safe-Mode budget must accommodate locked critical snapshots"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor and stateful BIOS manifest record.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Debugger Fallback subsystem. */
#define DEBUG_FALLBACK_MODULE_ID            "debug.fallback"

/**
 * Manifest record for the Tier-0 Safe-Mode Debugger BIOS & ROM Vault.
 */
extern const module_metadata_t debug_fallback_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Fallback Object Definitions
--------------------------------------------------------------------------------
Self-contained fallback profile templates, tripwire rule rows, ROM vault metadata,
and Safe-Mode runtime statistics structures. Shared primary ontology lives cleanly
in `debug/debug_types.h`. All structs carry explicit non-anonymous DWARF tags.
--------------------------------------------------------------------------------
*/

/**
 * @struct debug_fallback_profile
 * @brief Self-contained fallback configuration record for the debugger engine.
 *
 * ### For Game Designers & Non-Programmers:
 * Emergency profile ensuring the state rewind system stays online.
 *
 * ### For Systems Programmers & Engine Architects:
 * Invariant `loaded_from_data` is unconditionally false.
 */
typedef struct debug_fallback_profile {
    const char *profile_name;     /**< Fixed profile identifier string. */
    bool        loaded_from_data; /**< Invariant: unconditionally false. */
    size_t      max_snapshots;    /**< Fallback snapshot capacity (64). */
} debug_fallback_profile_t;

/**
 * @struct debug_fallback_tripwire_rule
 * @brief Parsed tripwire reaction rule from diagnostics/taxonomy/tripwire_matrix.json.
 *
 * ### For Game Designers & Non-Programmers:
 * Explains which rule was triggered and what emergency response
 * the debugger takes automatically.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap rule entry populated by `debug_fallback_parse_tripwire_matrix_json()`.
 */
typedef struct debug_fallback_tripwire_rule {
    char                      id[48];                 /**< Rule identifier (e.g., TW_ENG_ZERO_HEAP). */
    char                      bound_error[48];        /**< Linked taxonomy error code ID. */
    char                      trigger_condition[128]; /**< Human/machine condition expression. */
    char                      fallback_action[128];   /**< Action taken when downgrading to fallback. */
    debug_tripwire_reaction_t reaction;               /**< Configured reaction strategy. */
    bool                      tripwire_latch;         /**< True if tripwire latches debugger trap. */
    bool                      is_recoverable;         /**< True if recoverable via fallback/rewind. */
} debug_fallback_tripwire_rule_t;

/**
 * @struct debug_fallback_rom_info
 * @brief Summary telemetry descriptor for the compiled-in .rodata diagnostics ROM.
 *
 * For Designers: Reports how many debugger and error-code JSON files are built
 * straight into the executable so Safe Mode works without external data folders.
 *
 * For Programmers: Populated cleanly from debug_diagnostics_rom.h metadata.
 */
typedef struct debug_fallback_rom_info {
    size_t asset_count;             /**< Number of embedded JSON assets in .rodata. */
    size_t total_rom_bytes;         /**< Total uncompressed JSON bytes in .rodata. */
    char   active_rom_profile[32];  /**< Most recently hydrated ROM setting ID. */
} debug_fallback_rom_info_t;

/**
 * @struct debug_fallback_stats
 * @brief Runtime telemetry report for Tier-0 BIOS Safe-Mode capture and AWAIT.
 *
 * For Designers: Status card detailing Safe-Mode catches, fallback completions,
 * and whether the engine is currently waiting in AWAIT for recovery.
 *
 * For Programmers: Zero-heap status struct populated by debug_fallback_get_stats().
 */
typedef struct debug_fallback_stats {
    bool     booted;                   /**< True once debug_fallback_boot() is armed. */
    bool     safe_mode_engaged;        /**< True after fallback downgrade triggered. */
    bool     fallback_completed;       /**< True once fallback finished the halted step. */
    bool     await_active;             /**< True while BIOS sits idle in AWAIT. */
    bool     rom_hydrated;             /**< True if a ROM asset hydrated cleanly. */
    uint32_t fault_count;              /**< Total Tier-0 faults captured. */
    uint32_t recovery_count;           /**< Total AWAIT resolutions completed. */
    char     last_subsystem[32];       /**< Subsystem that raised the last fault. */
    char     last_condition[128];      /**< Last captured fault description. */
    char     last_source[96];          /**< Formatted file:line anchor. */
    char     last_test_ref[64];        /**< Test suite / Block 4 reference tag. */
    char     last_action_taken[96];    /**< Fallback action executed before AWAIT. */
    char     active_rom_profile[32];   /**< Active ROM profile identifier. */
} debug_fallback_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline inspection predicates and enum stringifiers.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a profile name matches the canonical fallback.
 *
 * @param[in] name Profile name string to compare.
 * @return true if name matches DEBUG_FALLBACK_PROFILE_NAME; false otherwise.
 */
static inline bool debug_fallback_is_profile_name(const char *name)
{
    if (name == NULL) {
        return false;
    }
    const char *expected = DEBUG_FALLBACK_PROFILE_NAME;
    while (*name && *expected) {
        if (*name != *expected) {
            return false;
        }
        name++;
        expected++;
    }
    return *name == *expected;
}

/**
 * @brief Predicate verifying whether a `debug_config_t` retains offline BIOS provenance.
 *
 * @param[in] cfg Pointer to strongly typed debugger configuration.
 * @return true if non-NULL and `loaded_from_data == false`.
 */
static inline bool debug_fallback_config_is_bios_origin(const debug_config_t *cfg)
{
    return (cfg != NULL) && (!cfg->loaded_from_data);
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Tier-0 Safe-Mode BIOS Cogs, ROM Vault Queries & Fault Recovery )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.debug.fallback
Mission:       Declare Safe-Mode templates, single-source JSON policy/matrix
               parsers, compiled .rodata ROM lookup routines, and BIOS fault hooks.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Emergency Static Templates & Shared JSON Schema Parsers
Mission: Guarantee baseline construction and unify JSON hydration across disk
and ROM paths with zero duplicated parser logic.
@triggers
  - Shared Parser Users : debug.c Sub-Assembly 3.1/5.2 & debug_fallback.c Sub-Assembly 3.2
  - Layer 4 Tests       : tests/unit/debug/test_debug_fallback_unit.c (Sections 1 & 3),
                          tests/integrated/test_debug_integrated.c (Sections 1 & 3)
--------------------------------------------------------------------------------
*/

/**
 * @brief Populates a caller destination buffer with static monolith fallback settings.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads the engine's full emergency factory default configuration directly from RAM
 * when debugger manifests cannot be read or are intentionally bypassed. Modders can
 * mirror these defaults as a safe starting template when authoring custom profiles.
 *
 * ### For Systems Programmers & Engine Architects:
 * Strongly typed O(1) `.rodata` copy guarded by caller `config_size == sizeof(debug_config_t)`
 * via shared `debug/debug_types.h`. Enforces `loaded_from_data = false` and stamps
 * `profile_name = "monolith_fallback"`.
 *
 * @param[out] out_config  Caller-allocated destination debug_config_t buffer.
 * @param[in]  config_size Size verification parameter matching sizeof(debug_config_t).
 */
void debug_get_monolith_fallback_config_raw(debug_config_t *out_config, size_t config_size);

/**
 * @brief Populates a caller buffer with the compact 1.44 MB Floppy Safe-Mode preset.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides an ultra-lightweight emergency Safe-Mode preset built for strict memory
 * budgets or read-only removable media (keeping traps armed while trimming history
 * to 16 snapshots and disabling interactive console prompts/ANSI color codes).
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) `.rodata` template copy (`max_snapshots = 16`, `locked_slot_quota = 2`,
 * `interactive_repl = false`, `ansi_color = false`, `loaded_from_data = false`).
 *
 * @param[out] out_config  Caller-allocated destination debug_config_t buffer.
 * @param[in]  config_size Size verification parameter matching sizeof(debug_config_t).
 */
void debug_get_floppy_safe_config_raw(debug_config_t *out_config, size_t config_size);

/**
 * @brief Shared parser hydrating debug_config_t from a tenlux.data.debug_profile.v1 scope.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads debugger JSON settings (`max_snapshots`, trap toggles like `break_on_sway_breach`,
 * `trace_channels`, and `auto_dump_on_tripwire`) identically whether modders edit
 * external files on disk or Safe Mode boots from built-in ROM backups.
 *
 * ### For Systems Programmers & Engine Architects:
 * Single authoritative bounded O(N) stack-only JSON parser shared by disk (`debug.c`)
 * and `.rodata` ROM paths (`debug_fallback.c`). Clamps/rejects out-of-range
 * `max_snapshots > DEBUG_MAX_SNAPSHOTS`, preserves `loaded_from_data = from_data`,
 * and reports precise failure classification via optional `out_tripwire` + `err_buf`.
 *
 * @param[in]  scope        Null-terminated JSON object slice (`profile` block or body).
 * @param[in]  profile_id   Fallback profile identifier stamp if JSON omits `id`.
 * @param[in]  from_data    Provenance flag: true when read from disk VFS; false from ROM.
 * @param[out] out_config   Destination debug_config_t buffer.
 * @param[in]  config_size  Must equal sizeof(debug_config_t).
 * @param[out] out_tripwire Optional failure tripwire enum code (may be NULL).
 * @param[out] err_buf      Optional caller error string buffer (may be NULL).
 * @param[in]  err_size     Capacity of err_buf.
 * @return true if parsed within all Layer 0 bounds; false on NULL or invalid bounds.
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
);

/**
 * @brief Shared parser hydrating snapshot ring policy fields from snapshot_policy.json.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads snapshot retention rules such as how many critical turn snapshots are pinned
 * against overwriting (`locked_slot_quota`) and whether every step runs safety checks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded zero-heap JSON extractor updating `locked_slot_quota` (<= `max_snapshots`),
 * `verify_on_step`, and `auto_await_on_tripwire` inside `out_config` without altering
 * unrelated profile flags or provenance.
 *
 * @param[in]     scope       Null-terminated JSON slice (`policy` block or body).
 * @param[in,out] out_config  Destination debug_config_t buffer whose policy fields are updated.
 * @param[in]     config_size Must equal sizeof(debug_config_t).
 * @param[out]    err_buf     Optional error string buffer (may be NULL).
 * @param[in]     err_size    Capacity of err_buf.
 * @return true if policy JSON is valid and within static bounds; false otherwise.
 */
bool debug_fallback_parse_snapshot_policy_json(
    const char     *scope,
    debug_config_t *out_config,
    size_t          config_size,
    char           *err_buf,
    size_t          err_size
);

/**
 * @brief Shared parser extracting reaction rules from taxonomy/tripwire_matrix.json.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads the table connecting engine alarm IDs (e.g., invalid HP or corrupt manifests)
 * to automatic responses like clamping bad values, switching to Safe Mode, or pausing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Single-pass bounded zero-heap JSON array scanner populating up to
 * `min(max_rules, DEBUG_FALLBACK_MAX_TRIPWIRE_RULES)` rules in caller stack/`.bss` storage.
 *
 * @param[in]  scope     Null-terminated JSON slice (body containing `"tripwires"` array).
 * @param[out] out_rules Destination rule array (capacity max_rules).
 * @param[in]  max_rules Maximum rules storable in out_rules.
 * @param[out] out_count Number of rules parsed and written.
 * @param[out] err_buf   Optional error buffer (may be NULL).
 * @param[in]  err_size  Capacity of err_buf.
 * @return true if at least one valid rule parsed cleanly; false otherwise.
 */
bool debug_fallback_parse_tripwire_matrix_json(
    const char                     *scope,
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count,
    char                           *err_buf,
    size_t                          err_size
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Compiled-In .rodata Debugger & Taxonomy ROM Vault
Mission: Resolve and hydrate embedded JSON debugger/taxonomy assets directly from
immutable ELF .rodata with zero disk I/O.
@triggers
  - ROM Generator : cmake/GenerateConfigRom.cmake -> build/generated/debug_diagnostics_rom.h
  - Layer 4 Tests : tests/unit/debug/test_debug_fallback_unit.c (Sections 1 & 3),
                    tests/integrated/test_debug_integrated.c (Section 1)
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns total count of embedded JSON assets compiled into diagnostics ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells diagnostic screens how many debugger and error-taxonomy backup files are
 * built straight inside the executable (`default`, `strict`, `trace`, `error_codes`, etc.).
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) constant accessor returning `DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT` from `.rodata`.
 *
 * @return Asset count in s_debug_diagnostics_rom_table.
 */
size_t debug_fallback_rom_count(void);

/**
 * @brief Returns raw JSON string pointer and byte length for ROM index `index`.
 *
 * ### For Game Designers & Non-Programmers:
 * Allows diagnostic tools and test suites to inspect any built-in backup JSON file
 * by slot number even without an external `engine/config/` folder on disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) bounds-checked `.rodata` pointer lookup; zero copy and zero heap allocation.
 * Returned pointer remains valid for the entire lifetime of the process.
 *
 * @param[in]  index    Zero-based index `< debug_fallback_rom_count()`.
 * @param[out] out_size Optional pointer receiving asset byte size.
 * @return Immutable null-terminated `.rodata` pointer, or NULL if out of bounds.
 */
const char *debug_fallback_rom_asset_at(size_t index, size_t *out_size);

/**
 * @brief Finds an embedded diagnostics or taxonomy JSON asset by ID or rel_path.
 *
 * ### For Game Designers & Non-Programmers:
 * Looks up a built-in backup config by preset name (`"strict"`, `"tripwire_matrix"`)
 * or relative file path whenever disk configuration files cannot be found.
 *
 * ### For Systems Programmers & Engine Architects:
 * Case-insensitive linear scan over static `.rodata` descriptor table (~9 assets,
 * <1 us) matching exact ID/`rel_path` first and unambiguous leaf stem second.
 *
 * @param[in]  id_or_rel_path e.g. `"debugger/default"`, `"default"`, or
 *                            `"diagnostics/taxonomy/tripwire_matrix.json"`.
 * @param[out] out_size       Optional byte size output.
 * @return Immutable `.rodata` pointer to raw JSON text, or NULL if not found.
 */
const char *debug_fallback_find_rom_asset(const char *id_or_rel_path, size_t *out_size);

/**
 * @brief Hydrates a debug_config_t directly from compiled-in .rodata ROM JSON.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a complete built-in debugger preset (`"default"`, `"strict"`, or `"trace"`)
 * together with its snapshot-locking rules straight from engine ROM on read-only media.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses the specified embedded debugger profile via `master_loader_load_from_memory()`
 * + `debug_fallback_parse_config_json()` and overlays `snapshot_policy.json` from ROM,
 * enforcing `loaded_from_data = false` unconditionally and updating `s_fb` ROM telemetry
 * unless `s_fb.in_audit` is active.
 *
 * @param[in]  profile_id Profile stem (`"default"`, `"strict"`, `"trace"`).
 * @param[out] out_config Caller debug_config_t destination.
 * @param[in]  config_size Must equal sizeof(debug_config_t).
 * @return true if parsed from ROM cleanly; false on missing asset or error.
 */
bool debug_fallback_load_rom_profile(
    const char     *profile_id,
    debug_config_t *out_config,
    size_t          config_size
);

/**
 * @brief Overlays snapshot_policy fields onto out_config from compiled-in ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Applies built-in snapshot locking and safety-verification policy defaults when
 * the disk `snapshot_policy.json` file is unavailable.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `"diagnostics/debugger/snapshot_policy.json"` in `.rodata` ROM, strips
 * its 5-block envelope via `master_loader_load_from_memory()`, and delegates to
 * `debug_fallback_parse_snapshot_policy_json()`.
 *
 * @param[in,out] out_config Destination debug_config_t buffer.
 * @param[in]     config_size Must equal sizeof(debug_config_t).
 * @return true on clean ROM parse; false otherwise.
 */
bool debug_fallback_load_rom_snapshot_policy(debug_config_t *out_config, size_t config_size);

/**
 * @brief Loads canonical tripwire reaction rules from compiled-in taxonomy ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads the engine's built-in emergency response playbook (`tripwire_matrix.json`)
 * so the debugger always knows how to react cleanly even without disk files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Hydrates up to `DEBUG_FALLBACK_MAX_TRIPWIRE_RULES` (16) entries directly from
 * `.rodata` ROM into caller storage with zero dynamic allocations.
 *
 * @param[out] out_rules Destination rule array.
 * @param[in]  max_rules Array capacity.
 * @param[out] out_count Parsed rule count.
 * @return true on clean ROM parse; false otherwise.
 */
bool debug_fallback_load_rom_tripwire_matrix(
    debug_fallback_tripwire_rule_t *out_rules,
    size_t                          max_rules,
    size_t                         *out_count
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Tier-0 Safe-Mode Fault Capture, Fallback Finishing & AWAIT
Mission: Guarantee that any failed test or tripwire latches its fault context,
completes execution cleanly using Safe-Mode fallback, and enters AWAIT.
@triggers
  - Primary Hook  : Invoked by debug.c `debug_trigger_probe()`, `debug_trap_tripwire()`,
                    and `debug_resolve_await()`
  - Layer 4 Tests : tests/unit/debug/test_debug_fallback_unit.c (Sections 2 & 4),
                    tests/unit/debug/test_debug_unit.c (Section 2),
                    tests/integrated/test_debug_integrated.c (Section 2)
--------------------------------------------------------------------------------
*/

/**
 * @brief Captures a fault or fail-test signal into Tier-0 Safe-Mode .bss memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Writes an immediate black-box note recording which system raised an alarm, what
 * went wrong, and which automated test rule flagged it so the bug is never lost.
 *
 * ### For Systems Programmers & Engine Architects:
 * Latches fault context into static `s_fb` `.data`/`.bss` registers (`fault_count++`)
 * and mirrors a guarded `LOG_SEV_ERROR` entry through `log_fallback_record()` (never
 * calling full `log.h` to guarantee zero circular trap recursion).
 *
 * @param[in] subsystem Subsystem identifier tag.
 * @param[in] condition Fault condition or failing assertion description.
 * @param[in] file      Source file anchor (`__FILE__`).
 * @param[in] line      Source line anchor (`__LINE__`).
 * @param[in] test_ref  Associated Block 4 / Layer 4 test identifier (may be NULL).
 * @return true if recorded into Tier-0 Safe-Mode fault latch; false on invalid args.
 */
bool debug_fallback_record_fault(
    const char *subsystem,
    const char *condition,
    const char *file,
    int         line,
    const char *test_ref
);

/**
 * @brief Completes the interrupted operation in Safe Mode and enters idle AWAIT.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely finishes any in-progress step after an error instead of crashing abruptly,
 * then parks execution in AWAIT until you or a developer inspect and repair the state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets `s_fb.safe_mode_engaged = true`, `s_fb.fallback_completed = true`, and
 * `s_fb.await_active = true`, stores bounded recovery description, and logs a
 * Safe-Mode `LOG_SEV_WARNING` via `log_fallback_record()`.
 *
 * @param[in] subsystem       Subsystem executing the fallback recovery action.
 * @param[in] fallback_action Summary of Safe-Mode completion step taken.
 * @return true once fallback completion and AWAIT latch are armed.
 */
bool debug_fallback_finish_with_fallback(
    const char *subsystem,
    const char *fallback_action
);

/**
 * @brief Queries whether Tier-0 Safe Mode is currently latched in idle AWAIT.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether Safe Mode is currently holding the simulation paused after a fault.
 *
 * ### For Systems Programmers & Engine Architects:
 * Lock-free O(1) read of `s_fb.await_active`.
 *
 * @return true while awaiting fault repair and validated state restoration.
 */
bool debug_fallback_is_awaiting(void);

/**
 * @brief Clears Tier-0 Safe-Mode AWAIT state once a valid state has been restored.
 *
 * ### For Game Designers & Non-Programmers:
 * Releases the Safe-Mode pause lock once a healthy state has been restored or verified
 * so normal turn execution can continue safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Increments `s_fb.recovery_count`, resets `s_fb.await_active` and `fallback_completed`,
 * and records `resolution_note` to the Tier-0 BIOS telemetry stream.
 *
 * @param[in] resolution_note Human-readable recovery explanation (may be NULL).
 * @return true once Safe-Mode AWAIT is cleared cleanly.
 */
bool debug_fallback_clear_await(const char *resolution_note);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & Validation Gatekeeper
================================================================================
Mission: Verify incoming parameters, contracts, ROM assets, and fallback invariants.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, Safe-Mode provenance guarantees, and shared
ROM/parser cogs independently.
--------------------------------------------------------------------------------
(Static template constants, !loaded_from_data invariants, and .rodata ROM vault
checksums/envelopes are verified via preflight & audit harnesses in Sub-Assembly 4.2.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
@triggers
  - Primary Hook  : Invoked by `debug_preflight()`, `debug_audit()`, and CLI `--audit`
  - Layer 4 Tests : tests/unit/debug/test_debug_fallback_unit.c (Sections 1 & 3),
                    tests/functional/test_debug_functional.c (Section 1)
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight verification routine checking fallback parameter invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the emergency backup debugging settings before gameplay starts so Safe Mode
 * is always ready if a configuration file goes missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Stack-pure invariant check verifying monolith template `loaded_from_data == false`,
 * canonical `DEBUG_FALLBACK_PROFILE_NAME`, and `max_snapshots`/`locked_slot_quota`
 * ceiling bounds without mutating live `s_debug` or `s_fb`. Preserves
 * `diag.subsystem == "core.util.debug_fallback"`.
 *
 * @triggers
 * - Called by     : `debug_preflight()`, `debug_audit()`, `test_debug_fallback_unit.c` (Sec 1)
 *
 * @return diag_result_t reporting SEVERITY_PASS or failure cause.
 */
diag_result_t debug_fallback_preflight(void);

/**
 * @brief Verification harness validating the static fallback code page and ROM vault.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated test confirming both the compact Floppy Safe-Mode preset and all 9 built-in
 * ROM debugger/error-taxonomy backup files are intact and readable.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes `debug_fallback_preflight()`, verifies Floppy Safe-Mode template invariants,
 * parses all 9 `.rodata` ROM JSON envelopes via `master_loader_load_from_memory()`,
 * and dry-hydrates `"default"`, `"strict"`, `"trace"`, and `tripwire_matrix` while
 * preserving & restoring pre-audit `s_fb` ROM flags cleanly.
 *
 * @triggers
 * - Called by     : `debug_audit()`, `test_debug_fallback_unit.c` (Sec 1 & 3)
 *
 * @return true if all fallback and ROM invariants hold cleanly; false otherwise.
 */
bool debug_fallback_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Engine Debugger & Validation Gatekeeper
================================================================================
Mission: Expose inspectable Safe-Mode telemetry objects, diagnostic ASCII cards,
and Tier-0 BIOS boot/shutdown lifecycle procedures.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

/**
 * @brief Populates caller ROM summary metadata from embedded diagnostics ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports how many backup JSON files are built into the engine and which ROM profile
 * was most recently used during Safe-Mode startup.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap getter copying `DEBUG_DIAGNOSTICS_ROM_ASSET_COUNT`, byte total,
 * and `s_fb.active_rom_profile` into caller storage in O(1) time.
 *
 * @param[out] out_info Destination ROM descriptor.
 * @return true on success; false on NULL argument.
 */
bool debug_fallback_get_rom_info(debug_fallback_rom_info_t *out_info);

/**
 * @brief Populates caller statistics report from Tier-0 Safe-Mode BIOS state.
 *
 * ### For Game Designers & Non-Programmers:
 * Takes a snapshot of Safe-Mode activity (how many problems were caught, whether Safe
 * Mode stepped in, and why playback paused in AWAIT) for diagnostic menus.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap field copy of `s_fb` BIOS registers (`booted`, `await_active`, counters,
 * `last_source`, `last_test_ref`, `last_action_taken`) in O(1) time.
 *
 * @param[out] out_stats Destination Safe-Mode telemetry struct.
 * @return true on success; false on NULL argument.
 */
bool debug_fallback_get_stats(debug_fallback_stats_t *out_stats);

/**
 * @brief Populates caller memory buffer with inspected fallback configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Inspects the emergency debugger settings stored in ROM or static code.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection procedure copying the compile-time fallback template.
 *
 * @param[out] out_config  Caller destination debug_config_t memory buffer.
 * @param[in]  config_size Size verification matching sizeof(debug_config_t).
 * @return true on successful population; false on NULL or invalid byte size.
 */
bool debug_fallback_execute_inspect(debug_config_t *out_config, size_t config_size);

/**
 * @brief Formats a standardized fixed-column ASCII diagnostic card into out_buf.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a readable card displaying emergency debugger defaults, ROM status,
 * and whether Safe Mode is currently waiting in AWAIT.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded buffer formatter returning exact written character length (excluding NUL),
 * formatted strictly to 60-column diagnostic card boundaries.
 *
 * @param[out] out_buf  Destination formatted string buffer.
 * @param[in]  buf_size Capacity of destination string buffer in bytes.
 * @return Number of bytes written excluding null terminator, or 0 on error.
 */
size_t debug_fallback_execute_dump_card(char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

/**
 * @brief Arms the Tier-0 Safe-Mode debugger BIOS with optional config or ROM defaults.
 *
 * ### For Game Designers & Non-Programmers:
 * Powers on the engine's Safe-Mode safety net at startup before full data files load,
 * pre-loading the built-in debugger preset and tripwire playbook if present.
 *
 * ### For Systems Programmers & Engine Architects:
 * Idempotent BIOS initialization validating ROM availability (`s_fb.rom_hydrated`),
 * setting `s_fb.booted = true`, and recording boot telemetry to `log_fallback_record()`
 * with zero heap allocation or dependency on `path_finder`.
 *
 * @param[in] opt_config  Optional debug_config_t pointer (or NULL for ROM/monolith default).
 * @param[in] config_size Byte size of opt_config (ignored when opt_config is NULL).
 * @return true once Tier-0 Safe-Mode debugger BIOS is armed.
 */
bool debug_fallback_boot(const debug_config_t *opt_config, size_t config_size);

/**
 * @brief Clears Tier-0 Safe-Mode BIOS state and fault latches cleanly.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely resets the emergency backup recorder when closing the game or resetting tests.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zeroes `s_fb` `.data`/`.bss` registers cleanly and restores baseline diagnostic messages.
 */
void debug_fallback_shutdown(void);

/**
 * @brief Queries whether Tier-0 Safe-Mode debugger BIOS is currently booted.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells diagnostic checks whether Safe-Mode protection has been turned on.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) inline-equivalent state check reading `s_fb.booted`.
 *
 * @return true if debug_fallback_boot() is active.
 */
bool debug_fallback_is_booted(void);

/**
 * @brief Explicitly initializes the debugger using ROM/monolith emergency defaults.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts the time-machine debugger in Safe Mode using built-in backups when external
 * configuration folders cannot be trusted or read.
 *
 * ### For Systems Programmers & Engine Architects:
 * Hydrates ROM `"default"` (or monolith template if ROM fails) with `loaded_from_data = false`
 * and delegates to `debug_init()`. Boots Tier-0 BIOS state prior to primary setup.
 *
 * @triggers
 * - Primary Caller : Tier-0 diskless boot / Safe-Mode recovery paths
 * - Layer 4 Tests  : `tests/unit/debug/test_debug_fallback_unit.c:Section4`
 *
 * @return true on Safe-Mode debugger initialization; false on critical failure.
 */
bool debug_init_emergency_monolith(void);

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_DEBUG_DEBUG_FALLBACK_H */
