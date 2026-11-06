// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          fs_utils.h
// @id            core.util.fs
// @layer         Layer 0: Core Utilities
// @subsystem     Data & Filesystem Subsystem (util/io)
// @version       0.1.0
// @interface_rev Rev 3
// @date          2026-11-05
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Zero-Heap POSIX Filesystem & Buffered I/O Engine
// @role          Bounded file reading, safe writing, existence verification,
//                file size inspection, recursive directory provisioning, and
//                Virtual Filesystem (VFS) data mounting for engine runtimes.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Foundational zero-heap filesystem abstraction layer for the C11 engine.
//   Provides bounded, non-allocating file reading into caller-allocated buffers,
//   atomic/safe text writing with automatic parent directory synthesis,
//   POSIX file metadata inspection, and dynamic data mounting integration.
//   Guarantees that filesystem queries never crash the host application or
//   exceed predetermined memory ceilings.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module in Layer 0).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/caller-supplied buffers).
//   - Concurrency / Safety:  REENTRANT_PURE on distinct caller destination buffers.
//   - Latency Budget:        < 500 microseconds per disk query (I/O bounded).
//
// AXIOMATIC INVARIANTS:
//   1. Zero dynamic heap allocations: malloc, calloc, realloc, and free prohibited.
//   2. Guaranteed null termination: Read buffers unconditionally null-terminated.
//   3. Buffer safety: Strict caller buffer boundary enforcement (FS_PATH_MAX).
//   4. Fault resilience: Gracefully handles non-existent paths, NULL pointers, 0 bytes.
//   5. Standardized capacities: Small (1K), Medium (8K), Large (32K), Max (64K).
//   6. Universal 5-Block Lifecycle Protocol enforced across header and source.
// =============================================================================

#ifndef TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H
#define TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: PRE-CONDITIONS & CANONICAL BUFFER LIMITS ]
// -----------------------------------------------------------------------------

#define FS_BUFFER_SMALL_BYTES   1024   /**< Small data snippets, tokens, and headers. */
#define FS_BUFFER_MEDIUM_BYTES  8192   /**< Standard configuration profiles (< 8 KB). */
#define FS_BUFFER_LARGE_BYTES   32768  /**< Aggregate manifests and data catalogs. */
#define FS_BUFFER_MAX_CONFIG    65536  /**< Maximum supported configuration profile. */
#define FS_PATH_MAX             512    /**< Maximum supported filepath length. */

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.2: STRUCTURAL ENTITY & OBJECT DEFINITIONS ]
// -----------------------------------------------------------------------------

/**
 * @brief Detailed filesystem inspection report object.
 */
typedef struct {
    bool exists;         /**< true if file or directory is accessible on disk. */
    bool is_regular;     /**< true if path is S_ISREG (regular data file). */
    bool is_directory;   /**< true if path is S_ISDIR (directory node). */
    long size_bytes;     /**< Total size in bytes (-1 if inaccessible). */
} fs_file_info_t;

/**
 * @brief Structured result object from a bounded file read procedure.
 */
typedef struct {
    bool success;                   /**< true if entire file was read within buffer bounds. */
    size_t bytes_read;              /**< Actual payload byte count excluding null terminator. */
    char path[FS_PATH_MAX];         /**< Normalized resolved path of file read. */
} fs_read_result_t;

/**
 * @brief Structured result object from a file write procedure.
 */
typedef struct {
    bool success;                   /**< true if all bytes were committed to disk. */
    size_t bytes_written;           /**< Number of bytes successfully written. */
    char path[FS_PATH_MAX];         /**< Target path written to. */
} fs_write_result_t;

/**
 * Manifest record for the Zero-Heap Filesystem Engine.
 */
extern const module_metadata_t fs_utils_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: FILESYSTEM INSPECTION & PROBING COGS ]
// Mission: Non-destructive path probing, accessibility, and size queries.
// -----------------------------------------------------------------------------

/**
 * @brief Tests whether a given file or directory path exists on the filesystem.
 *
 * @param[in] path Null-terminated file or directory path to test.
 * @return true if path exists and is accessible; false otherwise (or if NULL).
 */
bool fs_file_exists(const char *path);

/**
 * @brief Checks if a given path is an accessible regular file (not directory/device).
 *
 * @param[in] path Null-terminated filesystem path to inspect.
 * @return true if path exists and is a regular file; false otherwise.
 */
bool fs_is_regular_file(const char *path);

/**
 * @brief Retrieves the total byte size of a regular file on disk.
 *
 * @param[in] path Null-terminated file path to inspect.
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
 *
 * @param[in]  filepath Null-terminated path to target file.
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of destination buffer (including null terminator).
 *
 * @return true if file opened and completely read within buffer bounds; false otherwise.
 * @post If returning true, buf[buf_size - 1] == '\0' is guaranteed.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size);

/**
 * @brief Reads file text and reports exact count of bytes read into buffer.
 *
 * @param[in]  filepath       Null-terminated path to target file.
 * @param[out] buf            Caller-supplied destination buffer.
 * @param[in]  buf_size       Capacity of destination buffer in bytes.
 * @param[out] out_bytes_read Optional pointer receiving bytes read (excluding null).
 *
 * @return true if read completed without truncation; false otherwise.
 */
bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read);

/**
 * @brief Safely writes or overwrites bounded data to a file on disk.
 *
 * Automatically creates parent directories recursively if they do not exist.
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
 * Mimics POSIX mkdir -p behavior using bounded stack buffer traversal.
 *
 * @param[in] dir_path Null-terminated directory path to create.
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
 * @param[in] root_path Null-terminated path to the mounted data directory.
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
 *
 * @param[in]  rel_path Relative path or filename.
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
 * @param[in]  path     Path to inspect.
 * @param[out] out_info Destination info structure populated by procedure.
 * @return true if path exists and info was captured; false on missing/NULL.
 */
bool fs_inspect_file(const char *path, fs_file_info_t *out_info);

/**
 * @brief Executes a complete bounded read procedure, populating an fs_read_result_t object.
 *
 * @param[in]  path       Filepath to read.
 * @param[out] buf        Destination text buffer.
 * @param[in]  buf_size   Capacity of buffer.
 * @param[out] out_result Detailed operation outcome object.
 * @return true if read completed successfully without truncation; false otherwise.
 */
bool fs_execute_read(const char *path, char *buf, size_t buf_size,
                     fs_read_result_t *out_result);

/**
 * @brief Executes a complete bounded write procedure, populating an fs_write_result_t object.
 *
 * @param[in]  path       Target filepath to write.
 * @param[in]  data       Data bytes to write.
 * @param[in]  len        Length of data.
 * @param[out] out_result Detailed write operation outcome object.
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
 */
void fs_utils_init(void);

/**
 * @brief Tears down the filesystem subsystem lifecycle.
 */
void fs_utils_shutdown(void);

#endif /* TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H */
