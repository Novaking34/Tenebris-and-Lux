/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          fs_utils.h
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
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

  ### For Engine Engineers:
  Layer 0 leaf module. Strictly adheres to ALLOC_ZERO_HEAP: malloc(), calloc(),
  and free() are prohibited. All operations write directly into caller-allocated
  stack or static buffers. Thread-safe on distinct destination buffers (REENTRANT_PURE).
================================================================================
*/

#ifndef TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H
#define TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: HIERARCHICAL INCLUDE LADDER ]
// -----------------------------------------------------------------------------
// The include ladder strictly reflects the dependency hierarchy:
// Tier 1: ISO C11 Standard Library Headers (Language Foundation)
// Tier 2: External Third-Party Headers (None allowed in Layer 0)
// Tier 3: Internal Engine Foundation Headers (Ladder Root / Manifests)
// -----------------------------------------------------------------------------

/* Tier 1: ISO C11 Standard Library Headers */
#include <stdbool.h> /* Boolean primitives (bool, true, false) */
#include <stddef.h>  /* Type definitions (size_t, NULL) */
#include <stdint.h>  /* Exact-width integer types */

/* Tier 2: External Third-Party Library Headers */
/* [ZERO EXTERNAL DEPENDENCIES: Layer 0 contains zero third-party requirements] */

/* Tier 3: Internal Core Engine Headers (Lower Architectural Foundation) */
#include "core/manifest.h" /* Central module registry & diagnostic severity definitions */

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.2: PRE-CONDITIONS & CANONICAL BUFFER LIMITS ]
// -----------------------------------------------------------------------------
// Canonical memory limits define the maximum payload sizes for different types
// of game assets. Callers allocate buffers according to these constants.
// -----------------------------------------------------------------------------

/**
 * @brief Buffer capacity for small data tokens, keys, and short header blocks.
 * @details Sized to 1,024 bytes (1 KB). Ideal for individual formula strings,
 *          subsystem identity manifests, and single-line localized phrases.
 */
#define FS_BUFFER_SMALL_BYTES   1024

/**
 * @brief Standard buffer capacity for single configuration profiles.
 * @details Sized to 8,192 bytes (8 KB). Suitable for complete unit stat blocks,
 *          combat stance definitions, or inventory item catalogs.
 */
#define FS_BUFFER_MEDIUM_BYTES  8192

/**
 * @brief Buffer capacity for aggregate manifests and multi-entry data catalogs.
 * @details Sized to 32,768 bytes (32 KB). Used for root manifests, combined
 *          dialogue scripts, and regional province topological manifests.
 */
#define FS_BUFFER_LARGE_BYTES   32768

/**
 * @brief Maximum supported configuration profile ceiling across the engine.
 * @details Sized to 65,536 bytes (64 KB). Acts as an absolute safety tripwire;
 *          any asset exceeding this threshold is rejected as corrupt or oversized.
 */
#define FS_BUFFER_MAX_CONFIG    65536

/**
 * @brief Maximum permissible length for any filesystem path string in the engine.
 * @details Sized to 512 bytes, matching POSIX PATH_MAX safety constraints for
 *          embedded, Linux, and Windows workspace paths.
 */
#define FS_PATH_MAX             512

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.3: STRUCTURAL ENTITY & OBJECT DEFINITIONS ]
// -----------------------------------------------------------------------------
// Explicitly tagged structures (non-anonymous) ensure Doxygen compatibility,
// clean DWARF debug symbol generation, and forward-declaration support.
// -----------------------------------------------------------------------------

/**
 * @struct fs_file_info
 * @brief Comprehensive filesystem node inspection record.
 *
 * Populated by fs_inspect_file() to provide callers with full metadata
 * about a file or directory node before attempting bounded read/write operations.
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
 * Encapsulates the execution result of fs_execute_read(), recording whether
 * the file was read successfully, the exact payload size, and the resolved path.
 */
typedef struct fs_read_result {
    bool success;                   /**< true if file was read without truncation or errors. */
    size_t bytes_read;              /**< Actual payload byte count transferred into buffer (excluding null). */
    char path[FS_PATH_MAX];         /**< Normalized or resolved disk path that was read. */
} fs_read_result_t;

/**
 * @struct fs_write_result
 * @brief Structured outcome object constructed by bounded file write procedures.
 *
 * Encapsulates the execution result of fs_execute_write(), recording whether
 * all bytes were flushed to storage, the byte count, and the destination path.
 */
typedef struct fs_write_result {
    bool success;                   /**< true if all bytes were committed to storage successfully. */
    size_t bytes_written;           /**< Exact byte count successfully written to disk. */
    char path[FS_PATH_MAX];         /**< Target destination path that was written to. */
} fs_write_result_t;

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.4: MODULE MANIFEST DECLARATION ]
// -----------------------------------------------------------------------------

/**
 * @brief Manifest record for the Zero-Heap Filesystem Engine.
 *
 * Exposes compiled module provenance, zero-heap memory guarantee,
 * reentrancy policy, latency budgets, and pre-flight audit callback.
 */
extern const module_metadata_t fs_utils_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS APIS
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: FILESYSTEM INSPECTION & PROBING COGS ]
// Mission: Non-destructive path probing, accessibility, and size queries.
// -----------------------------------------------------------------------------

/**
 * @brief Tests whether a given file or directory path exists on disk.
 *
 * Checks POSIX accessibility using stat(). Does not open the file or allocate memory.
 *
 * @param[in] path Null-terminated file or directory path to test.
 *
 * @return true if path exists and is accessible; false otherwise (or if NULL/empty).
 *
 * @note Reentrant and safe to call from any thread with distinct path buffers.
 */
bool fs_file_exists(const char *path);

/**
 * @brief Checks if a given path is an accessible regular file (not directory or device).
 *
 * @param[in] path Null-terminated filesystem path to inspect.
 *
 * @return true if path exists and is S_ISREG; false if missing, directory, device, or NULL.
 */
bool fs_is_regular_file(const char *path);

/**
 * @brief Retrieves the total byte size of a regular file on disk.
 *
 * Queries POSIX stat.st_size without reading file contents into memory.
 *
 * @param[in] path Null-terminated file path to inspect.
 *
 * @return Positive byte size if regular file; -1 on missing, directory, or error.
 */
long fs_get_file_size(const char *path);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: BUFFERED I/O & DIRECTORY MANAGEMENT COGS ]
// Mission: Bounded text reading, safe file writing, and parent directory synthesis.
// -----------------------------------------------------------------------------

/**
 * @brief Reads entire text file contents into a bounded, caller-supplied buffer.
 *
 * Guarantees unconditional null termination and prevents buffer overruns.
 * If the file exceeds `buf_size - 1` bytes, the read is safely rejected
 * (returns false) to prevent partial truncated states.
 *
 * @param[in]  filepath Null-terminated path to target file.
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (including null terminator).
 *
 * @return true if file opened and completely read within buffer bounds; false otherwise.
 *
 * @pre  buf != NULL && buf_size > 0.
 * @post buf[buf_size - 1] == '\0' is unconditionally guaranteed.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size);

/**
 * @brief Reads file text and reports exact count of bytes read into buffer.
 *
 * Similar to fs_read_file_contents(), but additionally writes the exact
 * number of bytes read (excluding null terminator) to `out_bytes_read`.
 *
 * @param[in]  filepath       Null-terminated path to target file.
 * @param[out] buf            Caller-supplied destination buffer.
 * @param[in]  buf_size       Capacity of destination buffer in bytes.
 * @param[out] out_bytes_read Optional pointer receiving bytes read (excluding null).
 *
 * @return true if read completed without truncation; false otherwise.
 *
 * @post buf is unconditionally null-terminated.
 */
bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read);

/**
 * @brief Safely writes or overwrites bounded data to a file on disk.
 *
 * Automatically creates parent directories recursively if they do not exist
 * using fs_ensure_directory(). Flushes data cleanly to disk.
 *
 * @param[in] filepath Null-terminated target filepath.
 * @param[in] data     Buffer of data bytes to write.
 * @param[in] data_len Number of bytes to write.
 *
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
 * @param[in] dir_path Null-terminated directory path to create.
 *
 * @return true if directory exists or was created; false on error/NULL.
 */
bool fs_ensure_directory(const char *dir_path);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: DYNAMIC VFS MOUNTING & PATH RESOLUTION COGS ]
// Mission: Configurable game data mounting and multi-tier path discovery.
// -----------------------------------------------------------------------------

/**
 * @brief Sets the active mounted data root for all filesystem resolutions.
 *
 * Configures the base directory for data queries (e.g. "game/data",
 * "/opt/tenebris-lux/data", or custom mod folders).
 *
 * @param[in] root_path Null-terminated path to the mounted data directory.
 *                      If NULL or empty, resets to auto-discovery.
 *
 * @return true if accepted and within buffer bounds; false otherwise.
 */
bool fs_set_data_root(const char *root_path);

/**
 * @brief Gets the active mounted data root directory path.
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
 * @param[in]  rel_path Relative path or filename to resolve.
 * @param[out] out_path Destination buffer for resolved accessible path.
 * @param[in]  out_size Capacity of destination buffer in bytes.
 *
 * @return true if an accessible file was dynamically resolved; false otherwise.
 */
bool fs_resolve_data_path(const char *rel_path,
                          char *out_path,
                          size_t out_size);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify filesystem accessibility, buffer safety, and existence tests.
// -----------------------------------------------------------------------------

/**
 * @brief Executes machine pre-flight diagnostic checks on filesystem invariants.
 *
 * Verifies existence testing, bounded reading, null-termination, parent
 * directory synthesis, and Block 5 execution procedures without leaking memory.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t fs_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all filesystem invariant tripwires pass cleanly; false otherwise.
 */
bool fs_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: EXECUTABLE PROCEDURES & OBJECT CONSTRUCTORS ]
// Mission: Higher-level execution pipelines that run procedures and create objects.
// -----------------------------------------------------------------------------

/**
 * @brief Executes a comprehensive inspection procedure, constructing an fs_file_info_t object.
 *
 * Gathers existence, regular file status, directory status, and byte size
 * in a single atomic POSIX stat inspection.
 *
 * @param[in]  path     Path to inspect.
 * @param[out] out_info Destination info structure populated by procedure.
 *
 * @return true if path exists and info was captured; false on missing/NULL.
 */
bool fs_inspect_file(const char *path, fs_file_info_t *out_info);

/**
 * @brief Executes a complete bounded read procedure, populating an fs_read_result_t object.
 *
 * Resolves path dynamically via fs_resolve_data_path(), loads the file into
 * `buf` with zero heap allocation, and constructs the result object.
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
 * @param[in]  path       Target filepath to write.
 * @param[in]  data       Data bytes to write.
 * @param[in]  len        Length of data in bytes.
 * @param[out] out_result Detailed write operation outcome object.
 *
 * @return true if write succeeded; false otherwise.
 */
bool fs_execute_write(const char *path, const char *data, size_t len,
                      fs_write_result_t *out_result);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.2: SUBSYSTEM LIFECYCLE INITIALIZATION & TEARDOWN ]
// Mission: Initialize static invariants and perform clean graceful teardown.
// -----------------------------------------------------------------------------

/**
 * @brief Initializes the filesystem subsystem lifecycle.
 *
 * Sets up default data root mount paths from the runtime environment.
 */
void fs_utils_init(void);

/**
 * @brief Tears down the filesystem subsystem lifecycle.
 *
 * Flushes state and performs clean teardown.
 */
void fs_utils_shutdown(void);

#endif /* TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H */
