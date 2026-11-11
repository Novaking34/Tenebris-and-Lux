/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          log_fallback.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Forensic Telemetry & Flight Recorder Engine
@role          Tier-0 BIOS Safe-Mode Telemetry Code Page & Compiled-In ROM Vault
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined execution path: Never invoked in normal production.
               4. Provenance tracking: loaded_from_data is unconditionally false.
               5. Profile identity explicitly stamped as LOG_FALLBACK_PROFILE_NAME
                  or compact floppy preset LOG_FALLBACK_FLOPPY_PROFILE_NAME.
               6. Compiled-in .rodata telemetry ROM hydrates configs with zero disk I/O.
               7. Universal 5-Block Lifecycle Protocol enforced across header and source.
               8. Strictly ISO C-style comments only (zero double-slash comments).

PURPOSE & ARCHITECTURAL ROLE:
  Tier-0 Safe-Mode BIOS logger, emergency monolith defaults, and compiled-in
  .rodata telemetry ROM vault. Serves two cooperating roles:
  (a) Early-Boot / Crash Capture: Arms before log_init() on static .bss RAM
      buffers so pre-init faults, write-protected/full disks, and shutdown
      events are safely captured and replayed once log_init() mounts disk I/O.
  (b) Boot-Time Compiled-In ROM: Embeds all 10 telemetry JSON definitions into
      immutable .rodata arrays so offline or 1.44 MB floppy deployments can
      parse and hydrate complete profiles via master_loader_load_from_memory().

  ### For Game Designers & Non-Programmers:
  Think of this module as the computer's Safe Mode and BIOS flight recorder.
  Even before the game finds its folders on disk, or if someone deletes config
  files or puts the engine on a locked floppy disk, this backup system keeps
  essential error logs in emergency memory, loads built-in factory copies of
  the telemetry presets, and replays early boot notes once the main logger starts.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time .rodata & .bss: Zero dynamic heap allocation.
  - Unified JSON Setting Parser: Provides log_fallback_parse_config_json()
    shared directly with log.c so Safe-Mode ROM hydration and disk loading
    never duplicate config extraction logic.
  - Explicit provenance tagging: Safe-Mode & ROM configs preserve
    loaded_from_data == false while exposing detailed ROM/sink diagnostics.
================================================================================
*/

#ifndef TENEBRIS_LUX_LOG_FALLBACK_H
#define TENEBRIS_LUX_LOG_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
Mission: Quarantined emergency fallback limits, include ladder, and static
helper predicates for telemetry bootstrapping when data manifests are offline.
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

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Fallback Configuration Constants
--------------------------------------------------------------------------------
Compile-time static defaults for emergency boot and headless testing.
For Designers: Fallback parameters guarantee logs are recorded even if config
files are deleted, saving up to 10 historical 1MB files in the "logs" folder.
For Programmers: Immutable constants residing in .rodata with zero heap allocation.
--------------------------------------------------------------------------------
*/

/** Baseline directory for emergency fallback log output. */
#define LOG_FALLBACK_DIR            "logs"

/** Baseline file name for emergency fallback log file. */
#define LOG_FALLBACK_BASE_FILENAME  "tenebris_lux_fallback.log"

/** Hardcoded byte threshold triggering file rotation under fallback (1 MB). */
#define LOG_FALLBACK_MAX_BYTES      (1024 * 1024)

/** Maximum rotated archive files retained under fallback (10 files). */
#define LOG_FALLBACK_MAX_FILES      10

/** Filter cutoff severity under emergency fallback (INFO). */
#define LOG_FALLBACK_MIN_SEVERITY   LOG_SEV_INFO

/** Categorical log folder routing flag under fallback. */
#define LOG_FALLBACK_SAVE_BY_CAT    true

/** Console mirroring flag under fallback (false to prevent spam). */
#define LOG_FALLBACK_ECHO_CONSOLE   false

/** ANSI color emission flag under fallback (false). */
#define LOG_FALLBACK_USE_COLORS     false

/** Profile identifier explicitly identifying the emergency fallback profile. */
#define LOG_FALLBACK_PROFILE_NAME   "monolith_fallback"

/** Compact 1.44 MB Floppy Safe-Mode directory identifier. */
#define LOG_FALLBACK_FLOPPY_DIR          "logs"

/** Compact 1.44 MB Floppy Safe-Mode active file name. */
#define LOG_FALLBACK_FLOPPY_FILENAME     "tenlux_safe.log"

/** Compact Floppy byte threshold triggering log rotation (64 KB). */
#define LOG_FALLBACK_FLOPPY_MAX_BYTES    (64U * 1024U)

/** Compact Floppy maximum archive count (2 rotated files). */
#define LOG_FALLBACK_FLOPPY_MAX_FILES    2U

/** Compact Floppy Safe-Mode profile identifier string. */
#define LOG_FALLBACK_FLOPPY_PROFILE_NAME "floppy_compact"

/** Fixed Tier-0 .bss ring capacity for pre-init / Safe-Mode capture. */
#define LOG_FALLBACK_RING_CAPACITY       32U

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the fallback code page.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Logging Fallback subsystem. */
#define LOG_FALLBACK_MODULE_ID         "log.fallback"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Fallback Object Definitions
--------------------------------------------------------------------------------
Self-contained fallback profile descriptors, sink modes, ROM vault descriptors,
and Safe-Mode runtime statistics structures.
--------------------------------------------------------------------------------
*/

/**
 * @enum log_fallback_sink
 * @brief Active emergency telemetry routing target for Tier-0 Safe Mode.
 *
 * For Designers: Shows where Safe Mode is currently writing notes (memory-only
 * black box, console stderr alerts, or a bounded Safe-Mode file on disk).
 *
 * For Programmers: Updated dynamically on boot and on hardware disk faults.
 */
typedef enum log_fallback_sink {
    LOG_FALLBACK_SINK_UNARMED = 0,     /**< Safe-Mode BIOS ring not yet armed. */
    LOG_FALLBACK_SINK_RING_ONLY,       /**< Capturing in static .bss ring only. */
    LOG_FALLBACK_SINK_RING_AND_STDERR, /**< Disk unavailable/alert mode: ring + stderr. */
    LOG_FALLBACK_SINK_DISK_AND_RING    /**< Compact Safe-Mode file + .bss ring active. */
} log_fallback_sink_t;

/**
 * @struct log_fallback_profile
 * @brief Self-contained fallback configuration record for emergency telemetry.
 *
 * For Designers: An emergency backup record holding factory defaults for the
 * logging system so error recording never fails.
 *
 * For Programmers: Invariant loaded_from_data is strictly false.
 */
typedef struct log_fallback_profile {
    const char   *profile_name;     /**< Fixed profile identifier string. */
    bool          loaded_from_data; /**< Invariant: unconditionally false. */
    log_config_t  config;           /**< Baseline static configuration. */
} log_fallback_profile_t;

/**
 * @struct log_fallback_rom_info
 * @brief Summary telemetry descriptor for the compiled-in .rodata ROM vault.
 *
 * For Designers: Reports how many backup JSON files are built right inside the
 * game executable so telemetry boots even without disk folders.
 *
 * For Programmers: Populated cleanly from generated log_telemetry_rom.h metadata.
 */
typedef struct log_fallback_rom_info {
    size_t asset_count;             /**< Number of embedded JSON assets in .rodata. */
    size_t total_rom_bytes;         /**< Total uncompressed JSON bytes in .rodata. */
    char   active_rom_profile[32];  /**< Most recently hydrated ROM setting ID. */
} log_fallback_rom_info_t;

/**
 * @struct log_fallback_stats
 * @brief Runtime telemetry report for Tier-0 BIOS Safe-Mode capture and replay.
 *
 * For Designers: Status sheet showing how many early-boot messages were caught,
 * whether a floppy/disk write error forced console fallback, and ROM health.
 *
 * For Programmers: Zero-heap status struct populated by log_fallback_get_stats().
 */
typedef struct log_fallback_stats {
    bool                booted;             /**< True once log_fallback_boot() arms BIOS ring. */
    bool                disk_fault;         /**< True if disk stream failed (read-only/full). */
    bool                rom_hydrated;       /**< True if a ROM setting was loaded cleanly. */
    log_fallback_sink_t active_sink;        /**< Active emergency output target. */
    uint64_t            total_recorded;     /**< Lifetime entries captured by fallback. */
    uint64_t            total_replayed;     /**< Early-boot entries drained into primary log. */
    uint64_t            disk_bytes_written; /**< Total bytes written to fallback disk stream. */
    size_t              ring_count;         /**< Buffered entries currently queued for replay. */
    size_t              ring_capacity;      /**< Static ring limit (LOG_FALLBACK_RING_CAPACITY). */
    size_t              rom_asset_count;    /**< Embedded .rodata telemetry JSON file count. */
    size_t              rom_total_bytes;    /**< Embedded .rodata telemetry JSON byte footprint. */
    char                active_profile[32]; /**< Active fallback or ROM profile identifier. */
} log_fallback_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline inspection predicates for the fallback code page.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an active configuration was derived from fallback.
 *
 * @param[in] cfg Pointer to logging configuration to inspect.
 * @return true if the configuration was generated from this emergency fallback.
 */
static inline bool log_fallback_is_active(const log_config_t *cfg) {
    return (cfg != NULL) && (!cfg->loaded_from_data);
}

/**
 * @brief Predicate verifying whether a Safe-Mode sink includes writable disk output.
 *
 * @param[in] sink Sink mode enum to inspect.
 * @return true if sink equals LOG_FALLBACK_SINK_DISK_AND_RING.
 */
static inline bool log_fallback_sink_uses_disk(log_fallback_sink_t sink) {
    return sink == LOG_FALLBACK_SINK_DISK_AND_RING;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / MONOLITHIC FALLBACK API DECLARATIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration & Shared Parser Cogs
--------------------------------------------------------------------------------
@triggers
  - Block 4 Audits : log_fallback_preflight() [Steps 1-2],
                     log_fallback_validate_boot_contract()
  - Probe Mapping  : "log.setting_profile", "log.fallback_bios" (when debug active)
  - Layer 4 Tests  :
      * tests/unit/log/test_log_fallback_unit.c
        [Section 1: NOMINAL GOLDEN BASELINE / test_fallback_unit_always_pass]
        [Section 2: QUARANTINE DEFENSE & GRACEFUL DEGRADATION / test_fallback_unit_never_fail]
        [Section 3: INVARIANTS & CONSTRAINTS / test_fallback_unit_assertions_and_guarantees]
      * tests/functional/test_log_functional.c
        [Section 2: NEVER FAIL / GRACEFUL DEGRADATION / test_log_functional_never_fail]
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs and returns the static fallback configuration code page.
 *
 * Provides a guaranteed, valid, compile-time log configuration structure when
 * external JSON data manifests or profile files are missing, corrupted, or
 * unreadable. Quarantined in this dedicated module to ensure that normal
 * production code never conceals a data pipeline failure.
 *
 * Contractual Guarantees:
 * - Returns a fully-populated log_config_t with bounds-compliant limits.
 * - The returned structure unconditionally sets loaded_from_data == false.
 * - Profile name is explicitly stamped as LOG_FALLBACK_PROFILE_NAME ("monolith_fallback").
 *
 * ### For Game Designers & Non-Programmers:
 * Factory default settings used only if the game's telemetry JSON files cannot be loaded.
 *
 * ### For Systems Programmers & Engine Architects:
 * Builds immutable zero-heap log_config_t populated directly from .rodata fallback constants.
 *
 * @return Fully-formed static fallback log_config_t structure.
 */
log_config_t log_get_monolith_fallback_config(void);

/**
 * @brief Returns an ultra-compact Safe-Mode profile sized for 1.44 MB floppy media.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides tiny, storage-conscious log settings (64 KB file cap, max 2 files, no
 * per-category folders) designed for running directly from minimal or floppy disks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns an immutable .rodata compact log_config_t with loaded_from_data == false
 * and profile_name stamped as LOG_FALLBACK_FLOPPY_PROFILE_NAME ("floppy_compact").
 *
 * @return Compact bounds-checked log_config_t configuration.
 */
log_config_t log_fallback_get_floppy_config(void);

/**
 * @brief Checks if a Safe-Mode / fallback configuration carries recognized provenance.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures a backup or ROM-loaded profile came from a real Safe-Mode preset rather
 * than uninitialized garbage memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates Safe-Mode identity against monolith_fallback, floppy_compact, or an
 * embedded ROM profile ID with loaded_from_data == false.
 *
 * @param[in] cfg Configuration struct to inspect.
 * @return true if cfg represents a valid recognized Safe-Mode/ROM config.
 */
bool log_fallback_is_recognized_profile(const log_config_t *cfg);

/**
 * @brief Parses a severity token string case-insensitively using canonical & alias tables.
 *
 * ### For Game Designers & Non-Programmers:
 * Understands severity words like "TRACE", "INFO", "WARN", or "WARNING" in configs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure table lookup backed by .rodata severity descriptors; shared between
 * log_fallback ROM hydration and primary log.c config loading.
 *
 * @param[in]  str     Case-insensitive severity token string.
 * @param[out] out_sev Populated severity enum rank.
 * @return true if recognized; false otherwise.
 */
bool log_fallback_severity_from_string(const char *str, log_severity_t *out_sev);

/**
 * @brief Parses a JSON "setting" payload into a validated log_config_t.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads log folder, filename, file cap, severity cutoff, and flags out of JSON text.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap pure JSON setting extractor shared cleanly by both compiled-in ROM
 * hydration and primary disk configuration loading (eliminating duplicate parsers).
 * Leaves loaded_from_data false; callers set true only after live disk ingestion.
 *
 * @param[in]  scope        Pointer into JSON payload ("setting" block or body).
 * @param[out] out_config   Populated configuration structure.
 * @param[out] out_tripwire Optional failure tripwire enum code (may be NULL).
 * @param[out] err_buf      Optional human-readable error buffer (may be NULL).
 * @param[in]  err_cap      Capacity of err_buf in bytes.
 * @return true if attributes parsed and validated within bounds; false on violation.
 */
bool log_fallback_parse_config_json(
    const char *scope,
    log_config_t *out_config,
    log_tripwire_t *out_tripwire,
    char *err_buf,
    size_t err_cap
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Compiled-In Telemetry ROM Vault Cogs
--------------------------------------------------------------------------------
@triggers
  - Block 4 Audits : log_fallback_preflight() [Steps 3-4: 10 ROM envelopes & hydration],
                     log_fallback_validate_boot_contract()
  - Probe Mapping  : "log.manifest_pipeline", "log.fallback_bios" (when debug active)
  - Layer 4 Tests  :
      * tests/unit/log/test_log_fallback_unit.c
        [Section 1: NOMINAL GOLDEN BASELINE / test_fallback_unit_always_pass]
        [Section 3: INVARIANTS & CONSTRAINTS / test_fallback_unit_assertions_and_guarantees]
      * tests/integrated/test_log_integrated.c
        [Section 3: TELEMETRY ASSET AUDIT / test_log_integrated_assertions_and_guarantees]
--------------------------------------------------------------------------------
*/

/**
 * @brief Retrieves an immutable compiled-in telemetry JSON payload from .rodata ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Pulls a factory-built telemetry config file straight out of engine ROM when the
 * disk folder cannot be found.
 *
 * ### For Systems Programmers & Engine Architects:
 * Performs O(N) lookup across the CMake-generated s_log_telemetry_rom_table[] by
 * asset ID (e.g. "default", "combat_focus", "manifest") or relative VFS path
 * (e.g. "diagnostics/telemetry/default.json"). Zero heap, zero disk I/O.
 *
 * @param[in]  id_or_rel_path Asset ID or relative path inside diagnostics/telemetry/.
 * @param[out] out_json       Optional pointer receiving immutable NUL-terminated JSON pointer.
 * @param[out] out_len        Optional pointer receiving JSON byte count excluding NUL.
 * @return true if matching compiled-in ROM asset exists; false otherwise.
 */
bool log_fallback_get_rom_asset(
    const char *id_or_rel_path,
    const char **out_json,
    size_t *out_len
);

/**
 * @brief Retrieves summary metadata about the compiled-in .rodata telemetry ROM vault.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells diagnostic screens how many built-in telemetry configs are embedded and
 * how much space they use.
 *
 * ### For Systems Programmers & Engine Architects:
 * Populates caller log_fallback_rom_info_t with generated asset count and bytes.
 *
 * @param[out] out_info Destination ROM metadata structure.
 * @return true on success; false on NULL pointer.
 */
bool log_fallback_get_rom_info(log_fallback_rom_info_t *out_info);

/**
 * @brief Hydrates a log_config_t directly from compiled-in .rodata ROM assets.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets the engine boot using the full, official config presets ("default", "quiet",
 * "combat_focus", etc.) straight out of the executable's ROM even with no config files on disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Feeds embedded ROM JSON into master_loader_load_from_memory() and parses via
 * log_fallback_parse_config_json(). If profile_name is NULL or empty, resolves the
 * ROM manifest's active_setting / default_setting automatically. Strictly keeps
 * out_config->loaded_from_data == false to flag offline ROM provenance.
 *
 * @param[in]  profile_name Profile ID (e.g. "combat_focus") or NULL for ROM manifest default.
 * @param[out] out_config   Populated configuration structure.
 * @return true if ROM manifest/profile parsed cleanly; false on missing/invalid ROM asset.
 */
bool log_fallback_load_rom_profile(const char *profile_name, log_config_t *out_config);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Tier-0 Safe-Mode Capture, Replay & Emergency Ignition
--------------------------------------------------------------------------------
@triggers
  - Block 4 Audits : log_fallback_validate_boot_contract() [disk_fault & boot invariants]
  - Tripwire Chain : log_fb_rotate_disk_if_needed() / log_fallback_record() on I/O fault
                     -> debug_fallback_record_fault("log.fallback", ...)
                     -> debug_fallback_finish_with_fallback(...) -> Safe-Mode AWAIT
  - Layer 4 Tests  :
      * tests/unit/log/test_log_fallback_unit.c
        [Section 4: EMERGENCY DISASTER RECOVERY BOOT / test_fallback_unit_emergence]
      * tests/integrated/test_log_integrated.c
        [Section 2: FAILURE DEGRADATION & PIPELINE RECOVERY / test_log_integrated_never_fail]
        [Section 4: CATEGORICAL DISK ROUTING & HUD RING BUFFER / test_log_integrated_emergence]
      * tests/system/test_log_system.c
        [Section 2: CASCADE ROTATION & RETENTION CEILING / test_log_system_never_fail]
        [Section 4: 10,000-ENTRY CHAOS STRESS & RING BUFFER DYNAMICS / test_log_system_emergence]
--------------------------------------------------------------------------------
*/

/**
 * @brief Captures a telemetry message inside the Tier-0 BIOS Safe-Mode black box.
 *
 * ### For Game Designers & Non-Programmers:
 * Records critical startup messages, warnings, or errors into an emergency
 * memory bank even before the main logging system starts up or if the disk locks up.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap emergency writer. Self-arms Tier-0 RAM ring if not yet
 * booted, formats bounded log_entry_t, pushes to .bss circular FIFO, mirrors high-
 * severity or diskless alerts to stderr, and writes to Safe-Mode file when armed.
 *
 * @param[in] severity Log urgency level.
 * @param[in] category Thematic log channel.
 * @param[in] entity   Entity or module tag.
 * @param[in] grade    Normalized moral/alignment gradient [-1.0f .. +1.0f].
 * @param[in] file     Source file anchor (__FILE__).
 * @param[in] line     Source line anchor (__LINE__).
 * @param[in] func     Source function anchor (__func__).
 * @param[in] fmt      Printf format string.
 * @return Non-zero Safe-Mode sequence ID if recorded, or 0 if filtered/reentrant.
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
);

/**
 * @brief Non-destructively inspects buffered Tier-0 Safe-Mode entries in FIFO order.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets diagnostics read early-boot backup logs without erasing them.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies up to max_entries from the static .bss circular buffer (oldest first).
 *
 * @param[out] out_entries Caller array receiving copies of captured entries.
 * @param[in]  max_entries Maximum capacity of out_entries.
 * @return Number of entries copied.
 */
size_t log_fallback_peek_entries(log_entry_t *out_entries, size_t max_entries);

/**
 * @brief Drains buffered Tier-0 Safe-Mode entries in FIFO order for primary handoff.
 *
 * ### For Game Designers & Non-Programmers:
 * Hands off early boot messages stored in emergency memory to the main flight
 * recorder once disk logs come online.
 *
 * ### For Systems Programmers & Engine Architects:
 * Consumes up to max_entries from the .bss queue cleanly so log_init() can replay
 * early boot history into s_log.file without duplicating items.
 *
 * @param[out] out_entries Caller array receiving entries (or NULL to discard cleanly).
 * @param[in]  max_entries Maximum capacity of out_entries.
 * @return Number of entries drained.
 */
size_t log_fallback_drain_entries(log_entry_t *out_entries, size_t max_entries);

/**
 * @brief Explicitly initializes the logging engine using the emergency monolithic code page.
 *
 * Designed for catastrophic disaster recovery, early kernel panic logging,
 * and isolated offline unit tests where the filesystem data hierarchy is
 * unavailable or intentionally excluded.
 *
 * ### For Game Designers & Non-Programmers:
 * Emergency backup starter for the logging system during catastrophic boot errors.
 *
 * ### For Systems Programmers & Engine Architects:
 * Attempts ROM Safe-Mode / static monolith handoff to log_init(), falling back to
 * Tier-0 RAM Ring + stderr Safe-Mode capture cleanly if writable disk is unavailable.
 *
 * @return true on successful primary or Safe-Mode activation; false only on internal fault.
 */
bool log_init_emergency_monolith(void);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Forensic Telemetry Engine
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
(Static compile-time contracts and provenance invariants are verified via
pre-flight diagnostics and dedicated audit harnesses in Sub-Assembly 4.2.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic verification routine evaluating fallback parameters.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the emergency backup logging settings to verify all backup directories,
 * file sizes, and limits are valid before the game relies on them.
 *
 * ### For Systems Programmers & Engine Architects:
 * Asserts that all string buffers are non-empty, limits reside within
 * Layer 0 invariants, profile identifier matches LOG_FALLBACK_PROFILE_NAME,
 * and loaded_from_data is strictly false.
 *
 * @return diag_result_t reporting categorized diagnostic outcome.
 */
diag_result_t log_fallback_preflight(void);

/**
 * @brief Diagnostic verification routine asserting the integrity of the fallback code page.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated check that passes only if the emergency backup code is 100% operational.
 *
 * ### For Systems Programmers & Engine Architects:
 * High-level boolean validation audit asserting that all static fallback invariants hold cleanly.
 *
 * @return true if all static fallback invariants hold cleanly; false otherwise.
 */
bool log_fallback_audit(void);

/**
 * @brief Validates Tier-0 BIOS Safe-Mode boot contract, ROM table, and disk health.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms that Safe-Mode logging loaded cleanly with intact built-in configs and
 * zero unrecovered storage hardware faults.
 *
 * ### For Systems Programmers & Engine Architects:
 * Composes log_fallback_preflight(), compiled-in .rodata ROM asset verification
 * via log_fallback_get_rom_info(), and live Safe-Mode disk_fault state inspection.
 *
 * @triggers
 * - Block 4 Audit : Runs inside primary log validation probe "log.fallback_bios"
 *                   and debug_check_tripwires() Safe-Mode health verification.
 * - Layer 4 Tests :
 *     * tests/unit/log/test_log_fallback_unit.c
 *       [Section 1: NOMINAL GOLDEN BASELINE / test_fallback_unit_always_pass]
 *       [Section 4: EMERGENCY DISASTER RECOVERY BOOT / test_fallback_unit_emergence]
 *     * tests/integrated/test_log_integrated.c
 *       [Section 3: TELEMETRY ASSET AUDIT / test_log_integrated_assertions_and_guarantees]
 *
 * @return diag_result_t reporting SEVERITY_PASS on healthy Safe-Mode contract or
 *         SEVERITY_WARNING / SEVERITY_FATAL on ROM or disk-fault breach.
 */
diag_result_t log_fallback_validate_boot_contract(void);

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

/**
 * @brief Populates caller statistics report from live Tier-0 Safe-Mode state.
 *
 * ### For Game Designers & Non-Programmers:
 * Shows whether Safe Mode is armed, if disk writes hit read-only errors, and how
 * many emergency logs or built-in ROM configs are active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap inspection procedure copying static s_fb telemetry counters.
 *
 * @param[out] out_stats Destination Safe-Mode statistics report object.
 * @return true on success; false on NULL argument.
 */
bool log_fallback_get_stats(log_fallback_stats_t *out_stats);

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
bool log_fallback_execute_inspect(log_config_t *out_cfg);

/**
 * @brief Formats an ASCII diagnostic inspection card for logging fallback parameters.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text summary of the emergency fallback logging configuration.
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
bool log_fallback_execute_dump_card(const log_config_t *cfg, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage Tier-0 BIOS Safe-Mode lifecycle, arm/close optional Safe-Mode
disk streams, and tear down static black-box capture buffers cleanly.
--------------------------------------------------------------------------------
*/

/**
 * @brief Arms the Tier-0 BIOS Safe-Mode logger before CLI flags or disk VFS boot.
 *
 * ### For Game Designers & Non-Programmers:
 * Turns on the emergency black box right at program startup so any early error is
 * safely captured, even before folders or config files are loaded.
 *
 * ### For Systems Programmers & Engine Architects:
 * When `opt_cfg` is NULL, arms purely in RAM Ring + stderr mode (zero root dir
 * pollution) ready for handoff replay into log_init(). When `opt_cfg` is supplied,
 * opens bounded Safe-Mode disk output (honoring build/logs sandbox when in repo
 * build layout) and degrades cleanly to RAM Ring + stderr on read-only/full disks.
 *
 * @param[in] opt_cfg Optional Safe-Mode config (NULL for clean pre-init BIOS ring).
 * @return true if BIOS Safe Mode armed cleanly (even if disk fault forced RAM fallback).
 */
bool log_fallback_boot(const log_config_t *opt_cfg);

/**
 * @brief Flushes any Safe-Mode file stream and resets Tier-0 BIOS runtime state.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely seals the emergency backup file and clears temporary Safe-Mode memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Flushes/closes Safe-Mode FILE*, clears capture ring counters and reentrancy guard.
 */
void log_fallback_shutdown(void);

/**
 * @brief Queries whether the Tier-0 BIOS Safe-Mode logger is currently armed.
 *
 * @return true if log_fallback_boot() (or auto-arm) is currently active.
 */
bool log_fallback_is_booted(void);

#endif /* TENEBRIS_LUX_LOG_FALLBACK_H */
