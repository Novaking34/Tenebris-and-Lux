// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          fs_utils.h
// @id            core.util.fs
// @layer         Layer 0: Core Utilities
// @subsystem     Data & Filesystem Subsystem (util/data)
// @version       1.1.0
// @interface_rev Rev 2
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap POSIX filesystem abstraction layer.
//   Provides bounded file reading, existence verification, file size inspection,
//   and path/directory creation primitives for all data-driven subsystems
//   (logging profiles, world maps, dialogue scripts, entity catalogs).
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/caller-supplied buffers).
//   - Concurrency / Safety:  REENTRANT_PURE (Thread-safe on distinct path buffers).
//   - Latency Budget:        < 500 microseconds per disk query (I/O bounded).
//
// AXIOMATIC INVARIANTS:
//   1. Memory safety: Caller-supplied output buffers are strictly bounded.
//   2. Guaranteed null termination: Read buffers are unconditionally null-terminated.
//   3. Resilience: Gracefully handles non-existent paths, NULL pointers, and 0 bytes.
//   4. Zero dynamic heap: Zero malloc/free calls in compliance with Layer 0 rules.
//   5. Predictable buffer sizes: Standardized buffer capacities for small/medium/large data.
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

/**
 * Canonical buffer capacities for data-driven file operations.
 */
#define FS_BUFFER_SMALL_BYTES   1024   /**< Small data snippets and headers. */
#define FS_BUFFER_MEDIUM_BYTES  8192   /**< Standard configuration profiles (< 8 KB). */
#define FS_BUFFER_LARGE_BYTES   32768  /**< Aggregate manifests and data catalogs. */
#define FS_BUFFER_MAX_CONFIG    65536  /**< Maximum supported configuration profile. */
#define FS_PATH_MAX             512    /**< Maximum supported filepath length. */

/**
 * Manifest record for the Zero-Heap Filesystem Engine.
 *
 * Exposes compiled module provenance, zero-heap memory guarantee,
 * reentrancy policy, and filesystem pre-flight audit callback.
 */
extern const module_metadata_t fs_utils_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: FILESYSTEM INSPECTION & PROBING COGS ]
// Mission: Non-destructive path probing and file metadata inspection.
// -----------------------------------------------------------------------------

/**
 * @brief Tests whether a given file or directory path exists on the filesystem.
 *
 * Validates path accessibility prior to read attempts, enabling callers to
 * detect missing data files and trip diagnostic tripwires cleanly without
 * triggering unhandled OS faults.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.fs (Zero-Heap Filesystem Engine)
 * - Assembly:   Filesystem Inspection & Probing
 * - Cog:        Path Existence Prober
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Stateless POSIX stat wrapper).
 * - Concurrency:    REENTRANT_PURE (Thread-safe read-only inspection).
 * - Latency Budget: < 50 microseconds (OS stat call).
 *
 * @param[in] path Null-terminated file or directory path to test.
 *
 * @return true if path exists and is accessible; false otherwise (or if NULL).
 */
bool fs_file_exists(const char *path);

/**
 * @brief Retrieves the total byte size of a regular file on the filesystem.
 *
 * Inspects file length before reading to ensure caller-supplied stack buffers
 * have sufficient capacity, preventing buffer overflow or unnecessary truncation.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.fs (Zero-Heap Filesystem Engine)
 * - Assembly:   Filesystem Inspection & Probing
 * - Cog:        File Byte Size Inspector
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Stateless POSIX stat wrapper).
 * - Concurrency:    REENTRANT_PURE (Thread-safe read-only inspection).
 * - Latency Budget: < 50 microseconds (OS stat call).
 *
 * @param[in] path Null-terminated file path to inspect.
 *
 * @return Positive byte size if path exists and is a regular file;
 *         -1 if file does not exist, is inaccessible, or if path is NULL.
 */
long fs_get_file_size(const char *path);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: BUFFERED I/O & DIRECTORY MANAGEMENT COGS ]
// Mission: Read file text safely into fixed buffers and ensure directories exist.
// -----------------------------------------------------------------------------

/**
 * @brief Reads entire text file contents into a bounded, caller-supplied buffer.
 *
 * Universal data loader for external JSON configurations, manifests, balance
 * tables, and chronicles. Guarantees null-termination and prevents buffer
 * overflows by enforcing strict buf_size bounds without heap allocation.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.fs (Zero-Heap Filesystem Engine)
 * - Assembly:   Buffered I/O & Directory Management
 * - Cog:        Bounded File Text Reader
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, writes into caller buffer).
 * - Concurrency:    REENTRANT_PURE (Thread-safe when distinct buffers provided).
 * - Latency Budget: < 500 microseconds for typical data profiles (< 64 KB).
 *
 * @param[in]  filepath Null-terminated path to target file.
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of caller buffer (including null terminator).
 *
 * @return true if file was successfully opened and read within buffer bounds;
 *         false if file cannot be opened, exceeds buffer size, or NULL args.
 *
 * @post If returning true, buf[buf_size - 1] == '\0' is guaranteed.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size);

/**
 * @brief Reads file text and reports the exact count of bytes read into buffer.
 *
 * Bounded file reader variant reporting precise byte count read into caller's
 * buffer, excluding the terminal null byte.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.fs (Zero-Heap Filesystem Engine)
 * - Assembly:   Buffered I/O & Directory Management
 * - Cog:        Bounded Byte-Counting File Reader
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
 * @brief Safely writes or overwrites bounded text/data to a file on disk.
 *
 * Automatically creates parent directories recursively if they do not yet exist.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.fs (Zero-Heap Filesystem Engine)
 * - Assembly:   Buffered I/O & Directory Management
 * - Cog:        Safe File Writer & Directory Synchronizer
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
 * @brief Checks if a given path is an accessible regular file (not dir/device).
 *
 * @param[in] path Null-terminated filesystem path to inspect.
 *
 * @return true if path exists and is S_ISREG; false otherwise.
 */
bool fs_is_regular_file(const char *path);

/**
 * @brief Ensures that a directory path exists, recursively creating parents if needed.
 *
 * Pre-flight directory provisioner for log files, save slots, and diagnostic
 * dumps. Mimics POSIX mkdir -p behavior using in-place stack path traversal.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.fs (Zero-Heap Filesystem Engine)
 * - Assembly:   Buffered I/O & Directory Management
 * - Cog:        Recursive Directory Provisioner
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Fixed stack buffer for traversal).
 * - Concurrency:    REENTRANT_PURE (Thread-safe directory creation).
 * - Latency Budget: < 200 microseconds (POSIX stat/mkdir calls).
 *
 * @param[in] dir_path Null-terminated directory path to create.
 *
 * @return true if directory exists or was successfully created;
 *         false if creation failed or path is NULL.
 */
bool fs_ensure_directory(const char *dir_path);

/**
 * @brief Sets the active mounted data root for all filesystem resolutions.
 *
 * @param[in] root_path Null-terminated path to the mounted data directory.
 * @return true if accepted; false otherwise.
 */
bool fs_set_data_root(const char *root_path);

/**
 * @brief Gets the active mounted data root directory path.
 *
 * @return Null-terminated string representing the mounted data root.
 */
const char *fs_get_data_root(void);

/**
 * @brief Dynamically resolves a dataset or configuration file path.
 *
 * Traverses candidate prefix trees across workspace root, build directory,
 * and the 5 structured data domains (system, world, entities, mechanics,
 * presentation) to locate an accessible file without hardcoded paths.
 *
 * ### Ontological Classification
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.fs (Zero-Heap Filesystem Engine)
 * - Assembly:   Dynamic Data Pathfinding & Resolution
 * - Cog:        Dynamic Data Path Resolver
 *
 * ### Execution & Resource Contract
 * - Memory Model:   ALLOC_ZERO_HEAP (Uses caller-supplied buffer).
 * - Concurrency:    REENTRANT_PURE (Read-only filesystem probing).
 * - Latency Budget: < 50 microseconds across candidate prefix probes.
 *
 * @param[in]  rel_path  Relative path or filename (e.g. "relics.json" or
 *                       "mechanics/items/relics.json").
 * @param[out] out_path  Destination buffer for resolved accessible path.
 * @param[in]  out_size  Capacity of destination buffer in bytes.
 *
 * @return true if an accessible file was dynamically resolved;
 *         false if no candidate path exists or parameters are invalid.
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
 * Verifies NULL resilience, existence checking against known system paths,
 * bounded file reading safety, and directory creation behavior.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t fs_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all filesystem invariant tripwires pass cleanly;
 *         false otherwise.
 */
bool fs_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_UTIL_DATA_FS_UTILS_H */
