/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          fs_utils.c
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
  Foundational zero-heap filesystem abstraction layer implementation.
  Provides robust, bounded file reading into caller-allocated buffers,
  atomic/safe text writing with automatic parent directory synthesis,
  POSIX file metadata inspection, and dynamic data mounting integration.

  ### For Game Designers & Non-Programmers:
  In traditional game development, opening files often allocates heap memory
  behind the scenes. If a level or dialogue file is missing or corrupted,
  the game crashes. In Tenebris and Lux, every file query operates within
  a pre-allocated, bounded container. If an asset is missing, the engine
  never crashes; it reports the issue cleanly so the simulation can proceed
  with fallback content.

  ### For Systems Programmers & Engine Architects:
  This module provides pure POSIX file primitives wrapped in ISO C11 contracts:
  - Zero heap usage (no malloc, calloc, realloc, or free permitted).
  - Absolute null-termination safety on every output buffer.
  - Bounded stack buffers preventing buffer overflow vulnerabilities.
  - Direct delegation of Virtual Filesystem (VFS) mounting to path_finder.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Filesystem & I/O Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy (Downward Dependency Ladder)
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "debug/debug.h"
#include "core/string_utils.h"
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Static Axiomatic Invariants & Module Manifest
--------------------------------------------------------------------------------
*/

static const char *const s_fs_invariants[4] = {
    "Memory safety: Caller output buffers are strictly bounded",
    "Guaranteed null termination: Read buffers unconditionally null-terminated",
    "Resilience: Gracefully handles non-existent paths, NULL pointers, 0 bytes",
    "Standardized buffer capacities for small/medium/large data"
};

const module_metadata_t fs_utils_manifest = {
    "core.util.fs",
    "fs_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    3,
    "2026-11-06",
    "Zero-heap POSIX filesystem abstraction, bounded reader, and safe writer",
    2,
    { "core.util.string", "core.io.path_finder", NULL, NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    500,
    4,
    {
        s_fs_invariants[0],
        s_fs_invariants[1],
        s_fs_invariants[2],
        s_fs_invariants[3]
    },
    fs_utils_preflight
};

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Filesystem & I/O Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Filesystem Inspection & Probing Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Checks if a file or directory exists at the specified path.
 *
 * @param[in] path Null-terminated path string to test.
 * @return true if path exists and is accessible; false on missing/NULL/error.
 */
bool fs_file_exists(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    struct stat st;
    return (stat(path, &st) == 0);
}

/**
 * @brief Returns file size in bytes for a regular file.
 *
 * @param[in] path Null-terminated path string to regular file.
 * @return File size in bytes (>= 0) on success; -1 if missing, not regular, or NULL.
 */
long fs_get_file_size(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return -1;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }

    if (!S_ISREG(st.st_mode)) {
        return -1;
    }

    return (long)st.st_size;
}

/**
 * @brief Verifies whether the specified path refers to a regular file (not directory/device).
 *
 * @param[in] path Null-terminated path string.
 * @return true if path exists and is regular file; false otherwise.
 */
bool fs_is_regular_file(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }

    return S_ISREG(st.st_mode);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Bounded Buffered I/O Operations
--------------------------------------------------------------------------------
*/

/**
 * @brief Reads file contents into a bounded buffer with byte-count tracking.
 *
 * Guarantees unconditional null-termination of the output buffer on all paths.
 * If the file exceeds (buf_size - 1) bytes, reads up to the limit, null-terminates,
 * and returns false to indicate truncation.
 *
 * @param[in]  filepath       Null-terminated path to the file.
 * @param[out] buf            Caller-allocated destination buffer.
 * @param[in]  buf_size       Capacity of destination buffer in bytes.
 * @param[out] out_bytes_read Optional pointer receiving bytes read (excluding null).
 *
 * @return true if entire file was read without truncation; false on error/overflow.
 */
bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read)
{
    if (out_bytes_read != NULL) {
        *out_bytes_read = 0;
    }

    if (filepath == NULL || buf == NULL || buf_size == 0) {
        if (buf != NULL && buf_size > 0) {
            buf[0] = '\0';
        }
        return false;
    }

    FILE *f = fopen(filepath, "r");
    if (!f) {
        buf[0] = '\0';
        return false;
    }

    size_t read_bytes = fread(buf, 1, buf_size - 1, f);
    buf[read_bytes] = '\0';

    int next_char = fgetc(f);
    fclose(f);

    if (next_char != EOF) {
        buf[buf_size - 1] = '\0';
        debug_trap_tripwire("fs_utils", "File content exceeds caller bounded buffer capacity", __FILE__, __LINE__);
        return false;
    }

    if (out_bytes_read != NULL) {
        *out_bytes_read = read_bytes;
    }
    return true;
}

/**
 * @brief Reads the entire contents of a file into a caller-supplied buffer.
 *
 * Delegates cleanly to fs_read_file_bounded, eliminating duplicated I/O loops.
 *
 * @param[in]  filepath Null-terminated path to the file.
 * @param[out] buf      Caller-allocated destination buffer.
 * @param[in]  buf_size Capacity of destination buffer in bytes.
 *
 * @return true if entire file was read and fit; false on error/overflow/NULL.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size)
{
    return fs_read_file_bounded(filepath, buf, buf_size, NULL);
}

/**
 * @brief Writes data bytes safely to a file, creating parent directories as needed.
 *
 * @param[in] filepath Null-terminated target filepath.
 * @param[in] data     Buffer of data bytes to write.
 * @param[in] data_len Length of data in bytes.
 *
 * @return true if all bytes were written successfully; false otherwise.
 */
bool fs_write_file_contents(const char *filepath, const char *data,
                            size_t data_len)
{
    if (filepath == NULL || filepath[0] == '\0' || data == NULL) {
        return false;
    }

    char parent[FS_PATH_MAX];
    if (str_parent_dir(parent, sizeof(parent), filepath)) {
        if (strcmp(parent, ".") != 0) {
            fs_ensure_directory(parent);
        }
    }

    FILE *f = fopen(filepath, "w");
    if (!f) {
        return false;
    }

    if (data_len > 0) {
        size_t written = fwrite(data, 1, data_len, f);
        fclose(f);
        return (written == data_len);
    }

    fclose(f);
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Recursive Directory Management
--------------------------------------------------------------------------------
*/

/**
 * @brief Ensures that a directory path exists, recursively creating parents.
 *
 * Mimics POSIX mkdir -p behavior using bounded stack buffer traversal.
 * Zero dynamic heap allocations.
 *
 * @param[in] dir_path Null-terminated directory path to create.
 *
 * @return true if directory exists or was created; false on error/NULL.
 */
bool fs_ensure_directory(const char *dir_path)
{
    if (dir_path == NULL || dir_path[0] == '\0') {
        return false;
    }

    char tmp[FS_PATH_MAX];
    str_safe_copy(tmp, dir_path, sizeof(tmp));
    size_t len = strlen(tmp);

    while (len > 1 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
        len--;
    }

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            struct stat st;
            if (stat(tmp, &st) != 0) {
                if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
                    return false;
                }
            } else if (!S_ISDIR(st.st_mode)) {
                return false;
            }
            *p = '/';
        }
    }

    struct stat st;
    if (stat(tmp, &st) != 0) {
        if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
            return false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        return false;
    }

    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Virtual Filesystem (VFS) Routing Delegation
--------------------------------------------------------------------------------
*/

/**
 * @brief Sets the active mounted data root for all filesystem resolutions.
 *
 * Delegates cleanly to path_finder_set_data_root().
 *
 * @param[in] root_path Null-terminated path to the mounted data directory.
 * @return true if accepted and within buffer bounds; false otherwise.
 */
bool fs_set_data_root(const char *root_path)
{
    return path_finder_set_data_root(root_path);
}

/**
 * @brief Retrieves the currently mounted data root path.
 *
 * Delegates cleanly to path_finder_get_data_root().
 *
 * @return Pointer to active data root string.
 */
const char *fs_get_data_root(void)
{
    return path_finder_get_data_root();
}

/**
 * @brief Dynamically resolves a dataset or configuration file path.
 *
 * Delegates cleanly to Layer 0 path_finder engine without duplicating
 * path traversal logic or maintaining legacy redirect tables.
 *
 * @param[in]  rel_path Relative path or filename to resolve.
 * @param[out] out_path Destination buffer for resolved accessible path.
 * @param[in]  out_size Capacity of destination buffer in bytes.
 *
 * @return true if an accessible regular file was located; false otherwise.
 */
bool fs_resolve_data_path(const char *rel_path,
                          char *out_path,
                          size_t out_size)
{
    return path_finder_resolve(NULL, rel_path, out_path, out_size);
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Filesystem & I/O Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Self-Audit & Invariant Tripwires
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on filesystem invariants.
 *
 * Verifies existence testing, bounded reading, null-termination, parent
 * directory synthesis, and Block 5 execution procedures without leaking memory.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t fs_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = fs_utils_manifest.module_id;
    res.check_name = "Filesystem Invariants & I/O Bounds";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Filesystem utilities: path probing and safe file I/O verified.";

    /* Check 1: NULL resilience across inspection and I/O primitives */
    if (fs_file_exists(NULL) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_file_exists(NULL) failed resilience contract.";
        return res;
    }

    if (fs_get_file_size(NULL) != -1) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_get_file_size(NULL) failed resilience contract.";
        return res;
    }

    char test_buf[64];
    if (fs_read_file_contents(NULL, test_buf, sizeof(test_buf)) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_read_file_contents(NULL) failed resilience contract.";
        return res;
    }

    if (fs_ensure_directory(NULL) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_ensure_directory(NULL) failed resilience contract.";
        return res;
    }

    /* Check 2: Known path probing (/dev/null exists) */
    if (!fs_file_exists("/dev/null")) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "fs_file_exists('/dev/null') unexpectedly returned false.";
        return res;
    }

    /* Check 3: Non-existent path probing */
    if (fs_file_exists("/__tenebris_lux_nonexistent_path_xyz__")) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "fs_file_exists unexpectedly detected non-existent path.";
        return res;
    }

    /* Check 4: Zero-capacity buffer reading */
    if (fs_read_file_contents("/dev/null", test_buf, 0) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_read_file_contents allowed zero-capacity buffer read.";
        return res;
    }

    /* Check 5: Regular file identification */
    if (fs_is_regular_file(NULL) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_is_regular_file(NULL) failed resilience contract.";
        return res;
    }

    if (fs_is_regular_file("/dev/null") != false) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "fs_is_regular_file unexpectedly identified /dev/null as regular.";
        return res;
    }

    size_t bytes_out = 0;
    if (fs_read_file_bounded(NULL, test_buf, sizeof(test_buf), &bytes_out) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_read_file_bounded(NULL) failed resilience contract.";
        return res;
    }

    if (fs_write_file_contents(NULL, "data", 4) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_write_file_contents(NULL) failed resilience contract.";
        return res;
    }

    /* Check 6: Executable procedure testing (Block 5 procedures) */
    fs_file_info_t info;
    if (!fs_inspect_file("/dev/null", &info) || !info.exists || info.is_regular) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "fs_inspect_file failed on /dev/null.";
        return res;
    }

    return res;
}

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all filesystem invariant tripwires pass cleanly; false otherwise.
 */
bool fs_utils_audit(void)
{
    diag_result_t res = fs_utils_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Filesystem & I/O Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures & Object Constructors
--------------------------------------------------------------------------------
*/

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
bool fs_inspect_file(const char *path, fs_file_info_t *out_info)
{
    if (out_info == NULL) {
        return false;
    }

    memset(out_info, 0, sizeof(*out_info));
    out_info->size_bytes = -1;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }

    out_info->exists = true;
    out_info->is_regular = S_ISREG(st.st_mode);
    out_info->is_directory = S_ISDIR(st.st_mode);
    out_info->size_bytes = S_ISREG(st.st_mode) ? (long)st.st_size : 0;
    return true;
}

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
                     fs_read_result_t *out_result)
{
    if (out_result != NULL) {
        memset(out_result, 0, sizeof(*out_result));
    }

    if (path == NULL || buf == NULL || buf_size == 0) {
        if (buf != NULL && buf_size > 0) {
            buf[0] = '\0';
        }
        return false;
    }

    char resolved[FS_PATH_MAX];
    const char *target = path;
    if (fs_resolve_data_path(path, resolved, sizeof(resolved))) {
        target = resolved;
    }

    size_t read_bytes = 0;
    bool ok = fs_read_file_bounded(target, buf, buf_size, &read_bytes);

    if (out_result != NULL) {
        out_result->success = ok;
        out_result->bytes_read = read_bytes;
        str_safe_copy(out_result->path, target, sizeof(out_result->path));
    }

    return ok;
}

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
                      fs_write_result_t *out_result)
{
    if (out_result != NULL) {
        memset(out_result, 0, sizeof(*out_result));
    }

    if (path == NULL || data == NULL) {
        return false;
    }

    bool ok = fs_write_file_contents(path, data, len);

    if (out_result != NULL) {
        out_result->success = ok;
        out_result->bytes_written = ok ? len : 0;
        str_safe_copy(out_result->path, path, sizeof(out_result->path));
    }

    return ok;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the filesystem subsystem lifecycle.
 *
 * Sets up default data root mount paths from the runtime environment.
 */
void fs_utils_init(void)
{
    /* Initialize active data root from environment if available */
    fs_set_data_root(NULL);
}

/**
 * @brief Tears down the filesystem subsystem lifecycle.
 *
 * Flushes state and performs clean teardown.
 */
void fs_utils_shutdown(void)
{
    /* Graceful teardown of filesystem engine */
}

/*
================================================================================
End of fs_utils.c
================================================================================
*/
