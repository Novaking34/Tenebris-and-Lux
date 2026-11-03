// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          fs_utils.c
// @id            core.util.fs
// @layer         Layer 0: Core Utilities
// @subsystem     Data & Filesystem Subsystem (util/data)
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-11-02
// @updated       2026-11-02
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap POSIX filesystem abstraction layer implementation.
//   Provides robust, bounded file reading, existence validation, size checks,
//   and directory creation without heap allocation for all data-driven modules.
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
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

#include "util/data/fs_utils.h"
#include "util/string/string_utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const char *const fs_invariants[] = {
    "Memory safety: Caller output buffers are strictly bounded",
    "Guaranteed null termination: Read buffers are unconditionally null-terminated",
    "Resilience: Gracefully handles non-existent paths, NULL pointers, and 0 bytes",
    "Standardized buffer capacities for small/medium/large data"
};

/**
 * Manifest record for the Zero-Heap Filesystem Engine.
 */
const module_metadata_t fs_utils_manifest = {
    "core.util.fs",
    "fs_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 1, 0 },
    2,
    "2026-11-02",
    "Zero-heap POSIX filesystem abstraction, bounded reader, and safe writer",
    1,
    { "core.util.string" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    500,
    4,
    {
        fs_invariants[0],
        fs_invariants[1],
        fs_invariants[2],
        fs_invariants[3]
    },
    fs_utils_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: FILESYSTEM INSPECTION & PROBING COGS ]
// Mission: Non-destructive path probing and file metadata inspection.
// -----------------------------------------------------------------------------

bool fs_file_exists(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return false;
    }
    struct stat st;
    return (stat(path, &st) == 0);
}

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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: BUFFERED I/O & DIRECTORY MANAGEMENT COGS ]
// Mission: Read file text safely into fixed buffers and ensure directories exist.
// -----------------------------------------------------------------------------

bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size)
{
    if (filepath == NULL || buf == NULL || buf_size == 0) {
        return false;
    }

    FILE *f = fopen(filepath, "r");
    if (!f) {
        buf[0] = '\0';
        return false;
    }

    size_t read_bytes = fread(buf, 1, buf_size - 1, f);
    buf[read_bytes] = '\0';

    // Verify whether entire file fit inside buffer
    int next_char = fgetc(f);
    fclose(f);

    if (next_char != EOF) {
        // File exceeded caller's buffer capacity; truncate safely and report error
        buf[buf_size - 1] = '\0';
        return false;
    }

    return true;
}

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

bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size, size_t *out_bytes_read)
{
    if (out_bytes_read) {
        *out_bytes_read = 0;
    }
    if (filepath == NULL || buf == NULL || buf_size == 0) {
        if (buf && buf_size > 0) {
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
        return false;
    }

    if (out_bytes_read) {
        *out_bytes_read = read_bytes;
    }
    return true;
}

bool fs_write_file_contents(const char *filepath, const char *data, size_t data_len)
{
    if (filepath == NULL || filepath[0] == '\0' || data == NULL) {
        return false;
    }

    // Ensure parent directory exists first
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

bool fs_ensure_directory(const char *dir_path)
{
    if (dir_path == NULL || dir_path[0] == '\0') {
        return false;
    }

    char tmp[512];
    str_safe_copy(tmp, dir_path, sizeof(tmp));
    size_t len = strlen(tmp);

    // Strip trailing slashes
    while (len > 1 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
        len--;
    }

    // Traverse segments creating directories progressively
    for (char *p = tmp + 1; *p != '\0'; p++) {
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

    // Create leaf directory
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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify filesystem accessibility, buffer safety, and existence tests.
// -----------------------------------------------------------------------------

diag_result_t fs_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = fs_utils_manifest.module_id;
    res.check_name = "Filesystem Invariants & I/O Bounds";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Filesystem utilities: path probing and safe file I/O verified.";

    // Check 1: NULL resilience
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

    // Check 2: Known path probing (/dev/null exists)
    if (!fs_file_exists("/dev/null")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_file_exists('/dev/null') unexpectedly returned false.";
        return res;
    }

    // Check 3: Non-existent path probing
    if (fs_file_exists("/__tenebris_lux_nonexistent_path_xyz__")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_file_exists unexpectedly detected non-existent path.";
        return res;
    }

    // Check 4: Zero-capacity buffer reading
    if (fs_read_file_contents("/dev/null", test_buf, 0) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_read_file_contents allowed zero-capacity buffer read.";
        return res;
    }

    // Check 5: Rev 2 NULL resilience & regular file probing
    if (fs_is_regular_file(NULL) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_is_regular_file(NULL) failed resilience contract.";
        return res;
    }

    // /dev/null is a character device, not a regular file
    if (fs_is_regular_file("/dev/null") != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
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

    return res;
}

bool fs_utils_audit(void)
{
    diag_result_t res = fs_utils_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Teardown & Module Close )
// =============================================================================

// End of fs_utils.c
