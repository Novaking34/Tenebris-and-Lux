/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          fs_utils.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap POSIX Filesystem & Buffered I/O Engine
@role          Bounded file reading, safe atomic writing, POSIX file inspection,
               recursive directory synthesis, and Virtual Filesystem (VFS) path
               resolution for engine runtimes and data-driven systems.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Guaranteed null-termination on all output string and data buffers.
               4. Hard buffer boundary enforcement (FS_PATH_MAX, canonical buffer limits).
               5. Fault resilience: gracefully handles non-existent paths, NULL pointers, 0 bytes.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Foundational zero-heap filesystem abstraction layer for the C11 engine.
  Provides bounded, non-allocating file reading into caller-allocated buffers,
  atomic/safe text writing with automatic parent directory synthesis,
  POSIX file metadata inspection, and dynamic data mounting integration.

  ### For Game Designers & Non-Programmers:
  In traditional software, loading files often allocates memory whenever
  a file is opened. If a file is too large or corrupted, the program crashes
  or causes lag spikes. In a tactical RPG designed for long campaigns and
  older hardware, we treat file reading like a bank vault: every buffer has
  a fixed, predetermined limit. If a file is missing or damaged, the engine
  never crashes—it safely reports the issue and falls back to baseline defaults.

  ### For Systems Programmers & Engine Architects:
  Layer 0 leaf module. Strictly adheres to ALLOC_ZERO_HEAP: malloc(), calloc(),
  and free() are prohibited. All operations write directly into caller-allocated
  stack or static buffers. Thread-safe on distinct destination buffers (REENTRANT_PURE).
================================================================================
*/

#ifndef TENEBRIS_LUX_IO_FS_UTILS_H
#define TENEBRIS_LUX_IO_FS_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap Filesystem Engine
================================================================================
Mission: Pre-conditions, downward include ladder, bounded buffer capacities,
manifest anchors, filesystem metadata types, and static inline path validators.
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
Sub-Assembly 2.1: Pre-Conditions & Canonical Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for bounded, zero-heap filesystem reading and writing.
For Designers: File sizes are strictly bounded so that corrupted or oversized
data files cannot cause memory bloat or crash low-spec devices.
For Programmers: Guarantees zero heap allocation (ALLOC_ZERO_HEAP) in caller buffers.
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the engine's Loose-Bolts initialization DAG.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap Filesystem Engine. */
#define FS_UTILS_MODULE_ID             "io.fs_utils"

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
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS APIS
Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Filesystem Inspection & Probing Cogs
Mission: Non-destructive path probing, accessibility, and size queries.
--------------------------------------------------------------------------------
*/

/**
 * @brief Tests whether a given file or directory path exists on disk.
 *
 * Checks POSIX accessibility using stat(). Does not open the file or allocate memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether an asset or configuration file actually exists on the computer
 * before trying to open it, preventing file errors.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant POSIX stat() inspection returning boolean accessibility without side effects.
 *
 * @param[in] path Null-terminated file or directory path to test.
 * @return true if path exists and is accessible; false otherwise (or if NULL/empty).
 */
bool fs_file_exists(const char *path);

/**
 * @brief Checks if a given path is an accessible regular file (not directory or device).
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures the path points to an actual data file rather than a folder.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates POSIX S_ISREG(stat.st_mode).
 *
 * @param[in] path Null-terminated filesystem path to inspect.
 * @return true if path exists and is S_ISREG; false if missing, directory, device, or NULL.
 */
bool fs_is_regular_file(const char *path);

/**
 * @brief Retrieves the total byte size of a regular file on disk.
 *
 * Queries POSIX stat.st_size without reading file contents into memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Measures the exact size of a file in bytes to ensure it will fit inside our memory limits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct inspection of stat.st_size without opening file descriptors or allocating heap.
 *
 * @param[in] path Null-terminated file path to inspect.
 * @return Positive byte size if regular file; -1 on missing, directory, or error.
 */
long fs_get_file_size(const char *path);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Buffered I/O & Directory Management Cogs
Mission: Bounded text reading, safe file writing, and parent directory synthesis.
--------------------------------------------------------------------------------
*/

/**
 * @brief Reads entire text file contents into a bounded, caller-supplied buffer.
 *
 * Guarantees unconditional null termination and prevents buffer overruns.
 * If the file exceeds `buf_size - 1` bytes, the read is safely rejected
 * (returns false) to prevent partial truncated states.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads an entire text file safely into memory, guaranteeing it never overflows or crashes the game.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reads via POSIX open()/read() into caller-provided buffer, enforcing null termination at dst[buf_size - 1].
 *
 * @param[in]  filepath Null-terminated path to target file.
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (including null terminator).
 * @return true if file opened and completely read within buffer bounds; false otherwise.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size);

/**
 * @brief Reads file text and reports exact count of bytes read into buffer.
 *
 * Similar to fs_read_file_contents(), but additionally writes the exact
 * number of bytes read (excluding null terminator) to `out_bytes_read`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a text file and tells you exactly how many letters and symbols were loaded.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded read returning transferred byte count in out_bytes_read.
 *
 * @param[in]  filepath       Null-terminated path to target file.
 * @param[out] buf            Caller-supplied destination buffer.
 * @param[in]  buf_size       Capacity of destination buffer in bytes.
 * @param[out] out_bytes_read Optional pointer receiving bytes read (excluding null).
 * @return true if read completed without truncation; false otherwise.
 */
bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read);

/**
 * @brief Safely writes or overwrites bounded data to a file on disk.
 *
 * Automatically creates parent directories recursively if they do not exist
 * using fs_ensure_directory(). Flushes data cleanly to disk.
 *
 * ### For Game Designers & Non-Programmers:
 * Saves game files to disk safely, automatically creating missing folders as needed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Synthesizes parent path, calls mkdir -p, and writes bytes with fsync/flush guarantees.
 *
 * @param[in] filepath Null-terminated target filepath.
 * @param[in] data     Buffer of data bytes to write.
 * @param[in] data_len Number of bytes to write.
 * @return true if all bytes were written successfully; false on error/NULL.
 */
bool fs_write_file_contents(const char *filepath, const char *data,
                            size_t data_len);

/**
 * @brief Ensures that a directory path exists, recursively creating parents.
 *
 * Mimics POSIX `mkdir -p` behavior using bounded stack buffer traversal.
 * Does not dynamically allocate heap memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Automatically creates any folders needed to store save games, screenshots, or logs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant directory creation walking path segments with stack buffers and POSIX mkdir(0755).
 *
 * @param[in] dir_path Null-terminated directory path to create.
 * @return true if directory exists or was created; false on error/NULL.
 */
bool fs_ensure_directory(const char *dir_path);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Dynamic VFS Mounting & Path Resolution Cogs
Mission: Configurable game data mounting and multi-tier path discovery.
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the active mounted data root for all filesystem resolutions.
 *
 * Configures the base directory for data queries (e.g. "game/data",
 * "/opt/tenebris-lux/data", or custom mod folders).
 *
 * ### For Game Designers & Non-Programmers:
 * Sets the folder where the game looks for content, making modding and custom campaigns easy.
 *
 * ### For Systems Programmers & Engine Architects:
 * Configures VFS mount point delegating to path_finder.
 *
 * @param[in] root_path Null-terminated path to the mounted data directory.
 *                      If NULL or empty, resets to auto-discovery.
 * @return true if accepted and within buffer bounds; false otherwise.
 */
bool fs_set_data_root(const char *root_path);

/**
 * @brief Gets the active mounted data root directory path.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns the currently active data folder path.
 *
 * ### For Systems Programmers & Engine Architects:
 * Read-only accessor for active VFS data root mount.
 *
 * @return Null-terminated string representing the active mounted data root.
 */
const char *fs_get_data_root(void);

/**
 * @brief Dynamically resolves a dataset or configuration file path.
 *
 * Probes across mounted data roots, domain folders, and workspace sandboxes.
 * Bridges legacy asset paths to modern domain structures.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a data file across standard game directories automatically.
 *
 * ### For Systems Programmers & Engine Architects:
 * Multi-tier path probing across game/config, engine/config, and mounted VFS roots.
 *
 * @param[in]  rel_path Relative path or filename to resolve.
 * @param[out] out_path Destination buffer for resolved accessible path.
 * @param[in]  out_size Capacity of destination buffer in bytes.
 * @return true if an accessible file was dynamically resolved; false otherwise.
 */
bool fs_resolve_data_path(const char *rel_path,
                          char *out_path,
                          size_t out_size);

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
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(Filesystem path safety, buffer capacity contracts, and discrete existence
invariants are verified via pre-flight assertions and standalone audit suites.)
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

#endif /* TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H */
