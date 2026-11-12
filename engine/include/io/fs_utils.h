/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/io/fs_utils.h
@id            cog.fs_utils
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap POSIX Filesystem & Buffered I/O Engine
@role          Public Contract & Seam: Bounded POSIX/"rom://" reads, flushed
               writes, stat queries, recursive mkdir, and buffer-policy loading.
@maturity      Rev 5 Baseline | Policy-driven I/O limits, ROM virtual reads & probe integration active.
@chassis       zero-heap | Public POD fs_utils_policy_t & fs_stat_t; FS_PATH_MAX (512) & configurable ceiling.
@scale         O(file_len) bounded copy up to policy max_config_bytes (64 KiB); O(path_len) recursive mkdir.
@seams         in: engine/config/io/fs_buffers.json | uses: cog.io.fallback, cog.log, cog.debug | probes: io.fs.* -> DEBUG_STEP_AWAIT | dag: core.util.fs
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Every string/read output guarantees NUL termination within caller buffer bounds.
               3. Read-only Safe-Mode floppy latch blocks disk writes and directory synthesis cleanly.
               4. Policy limits enforce canonical size brackets via fs_buffers.json or Safe-Mode ROM.

PURPOSE & ARCHITECTURAL ROLE:
  Defines the Layer 0 zero-heap filesystem contract uniting bounded POSIX disk
  operations and virtual "rom://" stream inspection under policy-configured
  size ceilings and Tier-0 BIOS Safe-Mode write protection.

  ### For Game Designers & Non-Programmers:
  Protects the game from corrupted or oversized data files by clamping reads
  inside strict locked vault brackets (1 KB, 8 KB, 32 KB, up to 64 KB) while
  blocking accidental writes on locked or floppy drives.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Callers supply all destination byte buffers and own
    lifetime; read/stat helpers are reentrant aside from telemetry counters.
  - Extension Seam: Tune buffer tier limits or enable stricter audit thresholds
    in engine/config/io/<profile>.json without modifying C callers.
  - Explicit Non-Goal: Does not resolve multi-tier VFS domains (handled by
    cog.path_finder) or parse structured envelopes (cog.json_utils / cog.master_loader).
================================================================================
*/

#ifndef CORNERSTONE_IO_FS_UTILS_H
#define CORNERSTONE_IO_FS_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Public Contract Limits, Fallback Link, Ontology & O(1) Pre-Conditions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.fs_utils
Mission:       Declare FS buffer ceilings, BIOS fallback link, metadata types, and O(1) path guards.
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
#include "io/io_fallback.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Buffer Limits & Compile-Time Proofs
--------------------------------------------------------------------------------
Compile-time sizing limits for bounded, zero-heap filesystem reading and writing.

### For Game Designers & Non-Programmers:
File sizes stay bounded inside strict preset tiers (1 KiB to 64 KiB) so corrupt
or oversized files cannot bloat memory on constrained hardware.

### For Systems Programmers & Engine Architects:
Guarantees zero heap allocation (`ALLOC_ZERO_HEAP`) and proves exact parity
against Tier-0 BIOS limits in `io_fallback.h`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Buffer capacity for small data tokens, keys, and short header blocks.
 * Sized to 1,024 bytes (1 KB). Ideal for individual formula strings,
 * subsystem identity manifests, and single-line localized phrases.
 */
#define FS_BUFFER_SMALL_BYTES   1024

/**
 * @brief Standard buffer capacity for single configuration profiles.
 * Sized to 8,192 bytes (8 KB). Suitable for complete unit stat blocks,
 * combat stance definitions, or inventory item catalogs.
 */
#define FS_BUFFER_MEDIUM_BYTES  8192

/**
 * @brief Buffer capacity for aggregate manifests and multi-entry data catalogs.
 * Sized to 32,768 bytes (32 KB). Used for root manifests, combined
 * dialogue scripts, and regional province topological manifests.
 */
#define FS_BUFFER_LARGE_BYTES   32768

/**
 * @brief Maximum supported configuration profile ceiling across the engine.
 * Sized to 65,536 bytes (64 KB). Acts as an absolute safety tripwire;
 * any asset exceeding this threshold is rejected as corrupt or oversized.
 */
#define FS_BUFFER_MAX_CONFIG    65536

/**
 * @brief Maximum permissible length for any filesystem path string in the engine.
 * Sized to 512 bytes, matching POSIX PATH_MAX safety constraints for
 * embedded, Linux, and Windows workspace paths.
 */
#define FS_PATH_MAX             512

static_assert(
    FS_BUFFER_MAX_CONFIG == IO_FB_MAX_CONFIG_BYTES &&
    FS_PATH_MAX == IO_FB_PATH_MAX,
    "FS public limits must match Tier-0 BIOS io_fallback bounds identically"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the engine's Loose-Bolts initialization DAG.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap Filesystem Engine. */
#define FS_UTILS_MODULE_ID             "core.util.fs"

/**
 * Manifest record for the Zero-Heap Filesystem Engine.
 */
extern const module_metadata_t fs_utils_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological structures for file node metadata, read results, and write outcomes.
All structs and enums MUST have explicit non-anonymous tags for DWARF symbols.
--------------------------------------------------------------------------------
*/

/**
 * @struct fs_file_info
 * @brief Comprehensive filesystem node inspection record.
 *
 * For Designers: A checklist of facts about a file on disk: does it exist,
 * is it a file or folder, and how many bytes does it contain?
 *
 * For Programmers: Populated by fs_inspect_file() via stat() before attempting
 * bounded read/write operations.
 */
typedef struct fs_file_info {
    bool exists;         /**< true if the file or directory exists and is accessible. */
    bool is_regular;     /**< true if the path is a regular data file (S_ISREG). */
    bool is_directory;   /**< true if the path is a directory node (S_ISDIR). */
    long size_bytes;     /**< Total size in bytes (-1 if non-existent or inaccessible). */
} fs_file_info_t;

/**
 * @struct fs_read_result
 * @brief Structured outcome object constructed by bounded file read procedures.
 *
 * For Designers: The receipt confirming whether a file was read successfully,
 * how many bytes were retrieved, and which disk path was used.
 *
 * For Programmers: Encapsulates fs_execute_read() results with null-termination guarantee.
 */
typedef struct fs_read_result {
    bool success;                   /**< true if file was read without truncation or errors. */
    size_t bytes_read;              /**< Actual payload byte count transferred into buffer. */
    char path[FS_PATH_MAX];         /**< Normalized or resolved disk path that was read. */
} fs_read_result_t;

/**
 * @struct fs_write_result
 * @brief Structured outcome object constructed by bounded file write procedures.
 *
 * For Designers: The receipt confirming that saving text or configuration data to disk
 * succeeded without data loss or partial writes.
 *
 * For Programmers: Encapsulates fs_execute_write() outcomes with directory synthesis.
 */
typedef struct fs_write_result {
    bool success;                   /**< true if all bytes were committed to storage successfully. */
    size_t bytes_written;           /**< Exact byte count successfully written to disk. */
    char path[FS_PATH_MAX];         /**< Target destination path that was written to. */
} fs_write_result_t;

/**
 * @struct fs_utils_stats
 * @brief Runtime telemetry and active policy provenance snapshot for `fs_utils`.
 *
 * ### For Game Designers & Non-Programmers:
 * Shows how many files were safely read/written, whether any oversized reads or
 * read-only floppy blocks happened, and whether settings came from disk or ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant stack copy of Sub-Assembly 2.3 runtime `.bss` counters (`s_fs_state`).
 */
typedef struct fs_utils_stats {
    bool                 initialized;           /**< True if `fs_utils_init()` completed. */
    bool                 policy_from_data;      /**< True only if hydrated from disk JSON. */
    bool                 read_only_safe_mode;   /**< True when writes/mkdir are locked off. */
    uint64_t             read_ops;              /**< Successful bounded file/ROM reads. */
    uint64_t             write_ops;             /**< Successful flushed disk writes. */
    uint64_t             truncation_rejections; /**< Bounded reads rejected due to cap. */
    uint64_t             read_only_blocks;      /**< Write/mkdir ops stopped by RO latch. */
    uint64_t             rom_fallback_reads;    /**< Reads served from `.rodata` ROM URIs. */
    io_fallback_config_t active_policy;         /**< Active policy limits & flags copy. */
} fs_utils_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an inspected file exists.
 *
 * @param[in] info Pointer to file inspection info record.
 * @return true if record is non-NULL and exists flag is true.
 */
static inline bool fs_file_info_exists(const fs_file_info_t *info) {
    return (info != NULL) && info->exists;
}

/**
 * @brief Predicate verifying if an inspected path is a regular data file.
 *
 * @param[in] info Pointer to file inspection info record.
 * @return true if record is non-NULL, exists, and is a regular file.
 */
static inline bool fs_file_is_regular(const fs_file_info_t *info) {
    return (info != NULL) && info->exists && info->is_regular;
}

/**
 * @brief Predicate verifying if an inspected path is a directory.
 *
 * @param[in] info Pointer to file inspection info record.
 * @return true if record is non-NULL, exists, and is a directory.
 */
static inline bool fs_file_is_directory(const fs_file_info_t *info) {
    return (info != NULL) && info->exists && info->is_directory;
}

/**
 * @brief Predicate verifying whether a path length is safely within bounds.
 *
 * @param[in] len Length of path string in bytes.
 * @return true if length is greater than 0 and strictly less than FS_PATH_MAX.
 */
static inline bool fs_path_len_is_valid(size_t len) {
    return len > 0 && len < FS_PATH_MAX;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Public Operational Cogs, State Accessors & Domain APIs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.fs_utils
Mission:       Declare bounded POSIX/"rom://" probing, buffered I/O, directory
               creation, VFS mounting delegation, and policy hydration APIs.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Filesystem Inspection & Probing Cogs
Mission: Non-destructive path probing, accessibility, and size queries.

  ### For Game Designers & Non-Programmers:
  Checks safely whether data files or built-in ROM backups exist and measures
  their byte size before reading them into fixed engine buffers.

  ### For Systems Programmers & Engine Architects:
  Reentrant POSIX `stat()` and `.rodata` ROM metadata queries operating strictly
  without heap allocations or file descriptor consumption.
--------------------------------------------------------------------------------
*/

/**
 * @brief Tests whether a given file or directory path exists on disk or ROM.
 *
 * Checks POSIX accessibility using `stat()` or `"rom://..."` lookup. Does not
 * open the file or allocate memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether an asset or configuration file actually exists on storage or ROM
 * before trying to open it, preventing missing-file errors.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant POSIX `stat()` and ROM descriptor inspection returning boolean existence.
 *
 * @param[in] path Null-terminated file, directory path, or `"rom://..."` URI to test.
 * @return true if path exists and is accessible; false otherwise (or if NULL/empty).
 * @triggers Validates input pointers without modifying telemetry counters.
 */
bool fs_file_exists(const char *path);

/**
 * @brief Checks if a given path is an accessible regular file (not directory or device).
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures the path points to an actual readable data file rather than a folder.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates `"rom://..."` existence or POSIX `S_ISREG(stat.st_mode)`.
 *
 * @param[in] path Null-terminated filesystem path or `"rom://..."` URI to inspect.
 * @return true if path exists and is a regular file/ROM entry; false otherwise.
 * @triggers Guarantees side-effect-free path predicate checks.
 */
bool fs_is_regular_file(const char *path);

/**
 * @brief Retrieves the total byte size of a regular file on disk or ROM.
 *
 * Queries POSIX `stat.st_size` or `.rodata` ROM descriptor size without reading contents.
 *
 * ### For Game Designers & Non-Programmers:
 * Measures the exact size of a file in bytes to ensure it fits inside engine limits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct inspection of `stat.st_size` or `.rodata` table entry size (`ALLOC_ZERO_HEAP`).
 *
 * @param[in] path Null-terminated file path or `"rom://..."` URI to inspect.
 * @return Non-negative byte size if regular file/ROM; -1 on missing, directory, or error.
 * @triggers Rejects missing or non-regular targets cleanly with `-1L`.
 */
long fs_get_file_size(const char *path);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Bounded Buffered I/O & Directory Management Cogs
Mission: Bounded text reading, safe file writing, and parent directory synthesis.

  ### For Game Designers & Non-Programmers:
  Loads configuration and story files safely within memory caps and saves outputs
  atomically while blocking mutations during Read-Only Safe Mode.

  ### For Systems Programmers & Engine Architects:
  Enforces active policy ceilings (`max_config_bytes`, `path_max_bytes`), NUL
  termination, Safe-Mode RO protection, and shared Tier-0 BIOS atomic write/mkdir.
--------------------------------------------------------------------------------
*/

/**
 * @brief Reads entire text file contents into a bounded, caller-supplied buffer.
 *
 * Guarantees unconditional null termination and prevents buffer overruns.
 * If the file exceeds `buf_size - 1` or `max_config_bytes`, the read is safely rejected.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads an entire text file safely into memory, guaranteeing it never overflows or crashes the engine.
 *
 * ### For Systems Programmers & Engine Architects:
 * Convenience wrapper delegating cleanly into `fs_read_file_bounded()` without length output.
 *
 * @param[in]  filepath Null-terminated path or `"rom://..."` URI to target file.
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (including null terminator).
 * @return true if file opened and completely read within buffer bounds; false otherwise.
 * @triggers Inherits truncation fault and warning log behavior from `fs_read_file_bounded()`.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size);

/**
 * @brief Reads file or `"rom://..."` text and reports exact count of bytes read.
 *
 * Similar to `fs_read_file_contents()`, and writes the exact number of bytes read
 * (excluding NUL terminator) into optional `out_bytes_read`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a text file and tells callers the exact character byte length loaded.
 *
 * ### For Systems Programmers & Engine Architects:
 * Enforces `min(buf_size, max_config_bytes)` cap, increments `read_ops`/`truncation_rejections`,
 * and records BIOS faults (`IO_ERR_FILE_NOT_FOUND`/`IO_ERR_TRUNCATED_READ`).
 *
 * @param[in]  filepath       Null-terminated path or `"rom://..."` URI to target file.
 * @param[out] buf            Caller-supplied destination buffer.
 * @param[in]  buf_size       Capacity of destination buffer in bytes.
 * @param[out] out_bytes_read Optional pointer receiving bytes read (excluding null).
 * @return true if read completed without truncation; false otherwise.
 * @triggers Emits `LOG_WARN` and records BIOS fault on open error or policy truncation.
 */
bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read);

/**
 * @brief Safely writes or overwrites bounded data to a file on disk.
 *
 * Automatically creates parent directories recursively using configured permissions
 * and flushes/renames atomically when enabled in active policy.
 *
 * ### For Game Designers & Non-Programmers:
 * Saves files safely with automatic folder creation and read-only lock protection.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates path cap and Floppy RO lock (`io.fs.readonly_guard`), then delegates POSIX
 * write/flush/atomic rename to `io_fallback_fs_write_with_flags()`.
 *
 * @param[in] filepath Null-terminated target filepath.
 * @param[in] data     Buffer of data bytes to write.
 * @param[in] data_len Number of bytes to write.
 * @return true if all bytes were written successfully; false on guard block or error.
 * @triggers Triggers `io.fs.readonly_guard` when write attempted under read-only Safe Mode.
 */
bool fs_write_file_contents(const char *filepath, const char *data,
                            size_t data_len);

/**
 * @brief Ensures that a directory path exists, recursively creating missing parents.
 *
 * Mimics POSIX `mkdir -p` behavior using bounded stack buffers with zero heap allocations.
 *
 * ### For Game Designers & Non-Programmers:
 * Automatically creates folders needed for logs or outputs unless write-locked.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates `FS_PATH_MAX` cap and RO latch (`io.fs.readonly_guard`), then delegates to
 * `io_fallback_fs_ensure_dir_mode()`.
 *
 * @param[in] dir_path Null-terminated directory path to create.
 * @return true if directory exists or was created; false on RO lock, overflow, or error.
 * @triggers Triggers `io.fs.readonly_guard` when directory creation is blocked under RO latch.
 */
bool fs_ensure_directory(const char *dir_path);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Dynamic VFS Mounting & Path Resolution Cogs
Mission: Configurable data root mounting and multi-tier path discovery delegation.

  ### For Game Designers & Non-Programmers:
  Lets builds or mods switch the primary data root folder and automatically find
  files across layered config and content directories.

  ### For Systems Programmers & Engine Architects:
  Clean thin delegation into `cog.path_finder` so VFS mount state lives in one place.
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the active mounted data root for all filesystem resolutions.
 *
 * Configures the base directory for data queries (e.g. `"game/data"`,
 * `"/opt/cornerstone/data"`, or custom workspace folders).
 *
 * ### For Game Designers & Non-Programmers:
 * Sets the folder where the engine searches for content, supporting mods cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates VFS root updates to `path_finder_set_data_root()`.
 *
 * @param[in] root_path Null-terminated path to the mounted data directory.
 *                      If NULL or empty, resets to auto-discovery.
 * @return true if accepted and within buffer bounds; false otherwise.
 * @triggers Validates mount path ceiling via `path_finder`.
 */
bool fs_set_data_root(const char *root_path);

/**
 * @brief Gets the active mounted data root directory path.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns the currently active data folder path used for content discovery.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates read-only access directly to `path_finder_get_data_root()`.
 *
 * @return Null-terminated string representing the active mounted data root.
 * @triggers Queries VFS state with zero side effects.
 */
const char *fs_get_data_root(void);

/**
 * @brief Dynamically resolves a dataset or configuration file path.
 *
 * Probes across mounted data roots, domain folders, and workspace sandboxes.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a data file across standard engine/workspace folders automatically.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates multi-tier VFS probing to `path_finder_resolve(NULL, ...)`.
 *
 * @param[in]  rel_path Relative path or filename to resolve.
 * @param[out] out_path Destination buffer for resolved accessible path.
 * @param[in]  out_size Capacity of destination buffer in bytes.
 * @return true if an accessible file or ROM asset resolved; false otherwise.
 * @triggers Honors `path_finder` traversal guards and candidate search tiers cleanly.
 */
bool fs_resolve_data_path(const char *rel_path,
                          char *out_path,
                          size_t out_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Data-Driven Policy Hydration & Safe-Mode Accessors
Mission: Load `engine/config/io/fs_buffers.json` with automatic ROM downgrade.

  ### For Game Designers & Non-Programmers:
  Loads active file buffer brackets and safety switches from disk configs, or cleanly
  restores built-in Safe-Mode ROM settings if config files are missing or damaged.

  ### For Systems Programmers & Engine Architects:
  Uses transient 8 KB stack buffer + shared `io_fallback_parse_policy_span()` and
  validates overrides via `fs_utils_validate_policy_contract()` before committing `.bss`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Hydrates active filesystem buffer & safety policy from disk or ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads buffer limits and write-lock switches from `engine/config/io/fs_buffers.json`.
 * If the config file is deleted or broken, it cleanly drops to built-in ROM defaults.
 *
 * ### For Systems Programmers & Engine Architects:
 * Shares `io_fallback_parse_policy_span()` so zero parser duplication exists.
 * Marks `loaded_from_data = true` exclusively on valid disk parse; downgrades to
 * `io_fallback_load_rom_config("fs_buffers")` (`loaded_from_data = false`) on fault.
 * Uses 8 KB transient stack buffer; O(N) time; zero heap allocations.
 *
 * @param[in] rel_path Config relative path (NULL defaults to `"io/fs_buffers.json"`).
 * @return true if loaded from disk JSON; false if downgraded safely to ROM defaults.
 * @triggers  `io.fs.buffer_bounds`, `io.fs.rom_parity`.
 */
bool fs_utils_load_policy(const char *rel_path);

/**
 * @brief Applies an explicit filesystem policy after verifying structural bounds.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely updates active file-size limits, rejecting invalid settings before damage occurs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates via `fs_utils_validate_policy_contract()`; if invalid, triggers
 * `io.fs.buffer_bounds` Safe-Mode downgrade + `DEBUG_STEP_AWAIT` (when active).
 *
 * @param[in] policy Candidate configuration record.
 * @return true if valid and committed; false if rejected and Safe-Mode engaged.
 * @triggers `io.fs.buffer_bounds`.
 */
bool fs_utils_set_policy(const io_fallback_config_t *policy);

/**
 * @brief Copies the currently active filesystem policy into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets diagnostics or tools inspect the active file size limits and Safe-Mode switches.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) lazy-armed struct copy out of Sub-Assembly 2.3 `.bss` `s_fs_state.policy`.
 *
 * @param[out] out_policy Destination configuration struct.
 * @return true on success; false on NULL pointer.
 * @triggers Lazy-arms policy defaults on demand and rejects NULL destination cleanly.
 */
bool fs_utils_get_policy(io_fallback_config_t *out_policy);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap Filesystem Engine
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, policy contracts, and data provenance;
register `io.fs.*` validation probes with the central debugger.
--------------------------------------------------------------------------------
*/

/**
 * @brief Validates numeric buffer ladder ordering and path/read-only invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Double-checks that small < medium < large <= max file buffer limits stay healthy.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure O(1) contract predicate wrapping `io_fb_policy_bounds_valid()`.
 * @triggers `io.fs.buffer_bounds`.
 *
 * @param[in] policy Policy record to validate (NULL checks active live state).
 * @return true if all contracts hold; false on breach.
 */
bool fs_utils_validate_policy_contract(const io_fallback_config_t *policy);

/**
 * @brief Registers filesystem validation probes (`io.fs.*`) with the debugger.
 *
 * ### For Game Designers & Non-Programmers:
 * Hooks filesystem safety checks directly into the debugger so corrupted file
 * limits automatically freeze playback safely instead of causing silent corruption.
 *
 * ### For Systems Programmers & Engine Architects:
 * Idempotently registers `s_fs_validation_probes[3]` (`io.fs.buffer_bounds`,
 * `io.fs.rom_parity`, `io.fs.readonly_guard`) via `debug_register_validation_probe()`.
 */
void fs_utils_register_debug_probes(void);

/**
 * @brief Evaluates active FS policy probes and triggers Safe-Mode -> AWAIT if breached.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether active file limits are valid and freezes the debugger safely if damaged.
 *
 * ### For Systems Programmers & Engine Architects:
 * Registers `io.fs.*` probes, validates `s_fs_state.policy`, restores Safe-Mode ROM
 * bounds on breach, and invokes `debug_trigger_probe("io.fs.buffer_bounds", ...)`.
 *
 * @triggers `io.fs.buffer_bounds` -> `io_fallback_downgrade_safe_mode()` -> `DEBUG_STEP_AWAIT`.
 * @param[in] context_label Caller diagnostic label recorded into debugger tripwire report.
 * @return true if tripwire/probe fired; false if active policy is clean.
 */
bool fs_utils_trigger_policy_probe(const char *context_label);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on filesystem invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks that the game can safely read files, create folders, and check paths
 * without crashing or getting stuck before loading begins.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies existence testing, bounded reading, null-termination, parent
 * directory synthesis, and Block 5 execution procedures without leaking memory.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t fs_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Simple pass/fail health check verifying file saving and loading reliability.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive boolean audit wrapper exercising all filesystem invariant tripwires.
 *
 * @return true if all filesystem invariant tripwires pass cleanly; false otherwise.
 */
bool fs_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap Filesystem Engine
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
 * @brief Executes a comprehensive inspection procedure, constructing an fs_file_info_t object.
 *
 * Gathers existence, regular file status, directory status, and byte size
 * in a single atomic POSIX stat inspection.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks a file's details (whether it exists, if it is a normal file or folder,
 * and how big it is) in a single quick check.
 *
 * ### For Systems Programmers & Engine Architects:
 * Issues atomic POSIX stat inspection and populates fs_file_info_t without heap allocation.
 *
 * @param[in]  path     Path to inspect.
 * @param[out] out_info Destination info structure populated by procedure.
 *
 * @return true if path exists and info was captured; false on missing/NULL.
 */
bool fs_inspect_file(const char *path, fs_file_info_t *out_info);

/**
 * @brief Formats an ASCII diagnostic card summarizing inspected file metadata.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing file size, path, and whether the file is valid.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a 60-column terminal-friendly status card into out_buf without heap allocation.
 *
 * @param[in]  info     Populated fs_file_info_t record.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return Number of characters written (excluding null terminator), or 0 on error.
 */
size_t fs_execute_dump_card(const fs_file_info_t *info, char *out_buf, size_t buf_size);

/**
 * @brief Executes a complete bounded read procedure, populating an fs_read_result_t object.
 *
 * Resolves path dynamically via fs_resolve_data_path(), loads the file into
 * `buf` with zero heap allocation, and constructs the result object.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a file's text contents into memory while recording whether it loaded completely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant read procedure verifying bounds and returning populated fs_read_result_t.
 *
 * @param[in]  path       Filepath to read.
 * @param[out] buf        Destination text buffer.
 * @param[in]  buf_size   Capacity of buffer.
 * @param[out] out_result Detailed operation outcome object.
 *
 * @return true if read completed successfully without truncation; false otherwise.
 */
bool fs_execute_read(const char *path, char *buf, size_t buf_size,
                     fs_read_result_t *out_result);

/**
 * @brief Executes a complete bounded write procedure, populating an fs_write_result_t object.
 *
 * Creates parent directories recursively, commits data, and constructs
 * the structured write outcome report object.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely saves text or data to disk, ensuring folders are created and checking for write errors.
 *
 * ### For Systems Programmers & Engine Architects:
 * Atomic-like write execution creating parent directories and returning populated fs_write_result_t.
 *
 * @param[in]  path       Target filepath to write.
 * @param[in]  data       Data bytes to write.
 * @param[in]  len        Length of data in bytes.
 * @param[out] out_result Detailed write operation outcome object.
 *
 * @return true if write succeeded; false otherwise.
 */
bool fs_execute_write(const char *path, const char *data, size_t len,
                      fs_write_result_t *out_result);

/**
 * @brief Copies live runtime telemetry counters and policy provenance into `out_stats`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads counters showing files loaded, saved, or blocked by Safe Mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) zero-heap snapshot copy from Sub-Assembly 2.3 `.bss` `s_fs_state`.
 *
 * @param[out] out_stats Destination statistics object.
 * @return true on success; false on NULL argument.
 */
bool fs_utils_get_stats(fs_utils_stats_t *out_stats);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the filesystem subsystem lifecycle.
 *
 * Sets up default data root mount paths from the runtime environment.
 *
 * ### For Game Designers & Non-Programmers:
 * Prepares the storage and file-finding systems when the game launches.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets VFS mount anchors and path caches for zero-heap operation.
 */
void fs_utils_init(void);

/**
 * @brief Tears down the filesystem subsystem lifecycle.
 *
 * Flushes state and performs clean teardown.
 *
 * ### For Game Designers & Non-Programmers:
 * Closes storage connections and clears file paths when the game exits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets VFS state registers and clears active mount pointers.
 */
void fs_utils_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_IO_FS_UTILS_H */
