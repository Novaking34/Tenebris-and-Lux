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

#include "io/fs_utils.h"
#include "core/string_utils.h"

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

/**
 * @brief Tests whether a given file or directory path exists on the filesystem.
 *
 * @param[in] path Null-terminated file or directory path to test.
 * @return true if path exists and is accessible; false otherwise (or if NULL).
 */
bool fs_file_exists(const char *path)
{
    /* Step 1: Validate input parameter resilience */
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    /* Step 2: Query POSIX stat metadata */
    struct stat st;
    return (stat(path, &st) == 0);
}

/**
 * @brief Retrieves the total byte size of a regular file on the filesystem.
 *
 * @param[in] path Null-terminated file path to inspect.
 * @return Positive byte size if regular file exists; -1 on error or non-regular.
 */
long fs_get_file_size(const char *path)
{
    /* Step 1: Validate input parameter */
    if (path == NULL || path[0] == '\0') {
        return -1;
    }

    /* Step 2: Query file metadata */
    struct stat st;
    if (stat(path, &st) != 0) {
        return -1;
    }

    /* Step 3: Verify path refers to regular file */
    if (!S_ISREG(st.st_mode)) {
        return -1;
    }

    return (long)st.st_size;
}

/**
 * @brief Checks if a given path is an accessible regular file.
 *
 * @param[in] path Null-terminated path to check.
 * @return true if path exists and is S_ISREG; false otherwise.
 */
bool fs_is_regular_file(const char *path)
{
    /* Step 1: Precondition check */
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    /* Step 2: Query stat and evaluate S_ISREG */
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode);
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: BUFFERED I/O & DIRECTORY MANAGEMENT COGS ]
// Mission: Read file text safely into fixed buffers and ensure directories exist.
// -----------------------------------------------------------------------------

/**
 * @brief Reads entire text file contents into a bounded, caller-supplied buffer.
 *
 * @param[in]  filepath Null-terminated path to target file.
 * @param[out] buf      Caller-supplied memory destination buffer.
 * @param[in]  buf_size Byte capacity of caller buffer (including null terminator).
 * @return true if read succeeded within bounds; false on error/overflow.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size)
{
    /* Step 1: Precondition validation */
    if (filepath == NULL || buf == NULL || buf_size == 0) {
        return false;
    }

    /* Step 2: Open file in read mode */
    FILE *f = fopen(filepath, "r");
    if (!f) {
        buf[0] = '\0';
        return false;
    }

    /* Step 3: Read up to buf_size - 1 bytes */
    size_t read_bytes = fread(buf, 1, buf_size - 1, f);
    buf[read_bytes] = '\0';

    /* Step 4: Verify whether entire file fit inside buffer */
    int next_char = fgetc(f);
    fclose(f);

    if (next_char != EOF) {
        /* File exceeded caller's buffer capacity; enforce null termination */
        buf[buf_size - 1] = '\0';
        return false;
    }

    return true;
}

/**
 * @brief Reads file text and reports the exact count of bytes read into buffer.
 *
 * @param[in]  filepath       Null-terminated path to target file.
 * @param[out] buf            Caller-supplied destination buffer.
 * @param[in]  buf_size       Capacity of destination buffer in bytes.
 * @param[out] out_bytes_read Optional pointer receiving bytes read.
 * @return true if read completed without truncation; false otherwise.
 */
bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read)
{
    /* Step 1: Initialize output counter if provided */
    if (out_bytes_read) {
        *out_bytes_read = 0;
    }

    /* Step 2: Precondition validation */
    if (filepath == NULL || buf == NULL || buf_size == 0) {
        if (buf && buf_size > 0) {
            buf[0] = '\0';
        }
        return false;
    }

    /* Step 3: Open file stream */
    FILE *f = fopen(filepath, "r");
    if (!f) {
        buf[0] = '\0';
        return false;
    }

    /* Step 4: Read bounded content */
    size_t read_bytes = fread(buf, 1, buf_size - 1, f);
    buf[read_bytes] = '\0';

    /* Step 5: Check for file truncation */
    int next_char = fgetc(f);
    fclose(f);

    if (next_char != EOF) {
        buf[buf_size - 1] = '\0';
        return false;
    }

    /* Step 6: Populate output byte count */
    if (out_bytes_read) {
        *out_bytes_read = read_bytes;
    }
    return true;
}

/**
 * @brief Safely writes or overwrites bounded text/data to a file on disk.
 *
 * @param[in] filepath Null-terminated target filepath.
 * @param[in] data     Buffer of data to write.
 * @param[in] data_len Number of bytes to write.
 * @return true if all bytes were written successfully; false on error/NULL.
 */
bool fs_write_file_contents(const char *filepath, const char *data,
                            size_t data_len)
{
    /* Step 1: Precondition check */
    if (filepath == NULL || filepath[0] == '\0' || data == NULL) {
        return false;
    }

    /* Step 2: Ensure parent directory exists first */
    char parent[FS_PATH_MAX];
    if (str_parent_dir(parent, sizeof(parent), filepath)) {
        if (strcmp(parent, ".") != 0) {
            fs_ensure_directory(parent);
        }
    }

    /* Step 3: Open file for writing */
    FILE *f = fopen(filepath, "w");
    if (!f) {
        return false;
    }

    /* Step 4: Write payload if data_len > 0 */
    if (data_len > 0) {
        size_t written = fwrite(data, 1, data_len, f);
        fclose(f);
        return (written == data_len);
    }

    fclose(f);
    return true;
}

/**
 * @brief Ensures that a directory path exists, recursively creating parents.
 *
 * @param[in] dir_path Null-terminated directory path to create.
 * @return true if directory exists or was created; false on error.
 */
bool fs_ensure_directory(const char *dir_path)
{
    /* Step 1: Precondition check */
    if (dir_path == NULL || dir_path[0] == '\0') {
        return false;
    }

    /* Step 2: Copy to mutable buffer */
    char tmp[512];
    str_safe_copy(tmp, dir_path, sizeof(tmp));
    size_t len = strlen(tmp);

    /* Step 3: Strip trailing slashes */
    while (len > 1 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
        len--;
    }

    /* Step 4: Traverse segments creating directories progressively */
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

    /* Step 5: Create leaf directory */
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

typedef struct {
    const char *prefix;
    size_t prefix_len;
    const char *replacement;
} legacy_path_map_t;

static const legacy_path_map_t s_legacy_mappings[] = {
    { "data/math/", 10, "system/runtime/math/" },
    { "math/", 5, "system/runtime/math/" },
    { "data/time/", 10, "system/runtime/time/" },
    { "time/", 5, "system/runtime/time/" },
    { "data/string/", 12, "system/runtime/string/" },
    { "string/", 7, "system/runtime/string/" },
    { "data/log/", 9, "system/diagnostics/telemetry/" },
    { "log/", 4, "system/diagnostics/telemetry/" },
    { "data/debug/", 11, "system/diagnostics/debugger/" },
    { "debug/", 6, "system/diagnostics/debugger/" },
    { "data/taxonomy/", 14, "system/diagnostics/taxonomy/" },
    { "taxonomy/", 9, "system/diagnostics/taxonomy/" },
    { "data/formats/", 13, "system/diagnostics/formats/" },
    { "formats/", 8, "system/diagnostics/formats/" },
    { "data/items/", 11, "mechanics/items/" },
    { "items/", 6, "mechanics/items/" },
    { "data/combat/", 12, "mechanics/combat/" },
    { "combat/", 7, "mechanics/combat/" },
    { "data/tactical/terrains.json", 27, "world/terrains/default.json" },
    { "tactical/terrains.json", 22, "world/terrains/default.json" },
    { "data/tactical/", 14, "mechanics/tactics/" },
    { "tactical/", 9, "mechanics/tactics/" },
    { "data/actors/", 12, "entities/roster/" },
    { "actors/", 7, "entities/roster/" },
    { "data/classes/", 13, "entities/classes/" },
    { "classes/", 8, "entities/classes/" },
    {
        "data/player/morality_matrix.json", 33,
        "mechanics/campaign/morality.json"
    },
    {
        "player/morality_matrix.json", 28,
        "mechanics/campaign/morality.json"
    },
    {
        "data/player/inventory_spec.json", 31,
        "mechanics/campaign/inventory_spec.json"
    },
    {
        "player/inventory_spec.json", 26,
        "mechanics/campaign/inventory_spec.json"
    },
    {
        "data/player/manifest.json", 26,
        "mechanics/campaign/player_manifest.json"
    },
    {
        "player/manifest.json", 21,
        "mechanics/campaign/player_manifest.json"
    },
    { "data/player/", 12, "mechanics/campaign/" },
    { "player/", 7, "mechanics/campaign/" },
    { "data/world/maps.json", 20, "world/battlefields/maps.json" },
    { "world/maps.json", 15, "world/battlefields/maps.json" },
    { "data/world/map_types.json", 25, "world/battlefields/map_types.json" },
    { "world/map_types.json", 20, "world/battlefields/map_types.json" },
    { "data/world/overworld.json", 25, "world/geography/realms.json" },
    { "world/overworld.json", 20, "world/geography/realms.json" },
    {
        "data/gui/palette.json", 22,
        "presentation/visuals/palettes/legacy_palette.json"
    },
    {
        "gui/palette.json", 17,
        "presentation/visuals/palettes/legacy_palette.json"
    },
    { "data/gui/assets.json", 21, "presentation/visuals/assets.json" },
    { "gui/assets.json", 16, "presentation/visuals/assets.json" },
    { "data/gui/", 9, "presentation/ui/" },
    { "gui/", 4, "presentation/ui/" },
    { "data/audio/", 11, "presentation/audio/" },
    { "audio/", 6, "presentation/audio/" },
    { "data/fonts/", 11, "presentation/ui/fonts/" },
    { "fonts/", 6, "presentation/ui/fonts/" },
    { "data/textures/", 14, "presentation/visuals/" },
    { "textures/", 9, "presentation/visuals/" },
    { "data/palettes/", 14, "presentation/visuals/palettes/" },
    { "palettes/", 9, "presentation/visuals/palettes/" },
    { "data/skills/manifest.json", 25, "mechanics/combat/skills.json" },
    { "skills/manifest.json", 20, "mechanics/combat/skills.json" },
    { "data/states/manifest.json", 25, "mechanics/combat/states.json" },
    { "states/manifest.json", 20, "mechanics/combat/states.json" },
    { "data/dispatch/", 14, "mechanics/campaign/dispatch/" },
    { "dispatch/", 9, "mechanics/campaign/dispatch/" }
};

static const char *s_base_data_roots[] = {
    "game/data/%s",
    "../game/data/%s",
    "data/%s",
    "../data/%s"
};

static const char *s_data_candidate_prefixes[] = {
    "%s",
    "game/%s",
    "../game/%s",
    "../%s",
    "game/data/%s",
    "../game/data/%s",
    "data/%s",
    "../data/%s",
    /* 1. System Domain */
    "game/data/system/%s",
    "../game/data/system/%s",
    "game/data/system/runtime/%s",
    "../game/data/system/runtime/%s",
    "game/data/system/runtime/math/%s",
    "../game/data/system/runtime/math/%s",
    "game/data/system/runtime/time/%s",
    "../game/data/system/runtime/time/%s",
    "game/data/system/runtime/string/%s",
    "../game/data/system/runtime/string/%s",
    "game/data/system/diagnostics/%s",
    "../game/data/system/diagnostics/%s",
    "game/data/system/diagnostics/telemetry/%s",
    "../game/data/system/diagnostics/telemetry/%s",
    "game/data/system/diagnostics/debugger/%s",
    "../game/data/system/diagnostics/debugger/%s",
    "game/data/system/diagnostics/taxonomy/%s",
    "../game/data/system/diagnostics/taxonomy/%s",
    "game/data/system/diagnostics/formats/%s",
    "../game/data/system/diagnostics/formats/%s",
    /* 2. World Domain */
    "game/data/world/%s",
    "../game/data/world/%s",
    "game/data/world/geography/%s",
    "../game/data/world/geography/%s",
    "game/data/world/battlefields/%s",
    "../game/data/world/battlefields/%s",
    "game/data/world/terrains/%s",
    "../game/data/world/terrains/%s",
    /* 3. Entities Domain */
    "game/data/entities/%s",
    "../game/data/entities/%s",
    "game/data/entities/roster/%s",
    "../game/data/entities/roster/%s",
    "game/data/entities/roster/protagonists/%s",
    "../game/data/entities/roster/protagonists/%s",
    "game/data/entities/roster/companions/%s",
    "../game/data/entities/roster/companions/%s",
    "game/data/entities/roster/enemies/%s",
    "../game/data/entities/roster/enemies/%s",
    "game/data/entities/classes/%s",
    "../game/data/entities/classes/%s",
    "game/data/entities/classes/basic/%s",
    "../game/data/entities/classes/basic/%s",
    "game/data/entities/classes/upgraded/%s",
    "../game/data/entities/classes/upgraded/%s",
    "game/data/entities/classes/advanced/%s",
    "../game/data/entities/classes/advanced/%s",
    "game/data/entities/classes/summon/%s",
    "../game/data/entities/classes/summon/%s",
    "game/data/entities/classes/npc/%s",
    "../game/data/entities/classes/npc/%s",
    /* 4. Mechanics Domain */
    "game/data/mechanics/%s",
    "../game/data/mechanics/%s",
    "game/data/mechanics/tactics/%s",
    "../game/data/mechanics/tactics/%s",
    "game/data/mechanics/combat/%s",
    "../game/data/mechanics/combat/%s",
    "game/data/mechanics/items/%s",
    "../game/data/mechanics/items/%s",
    "game/data/mechanics/campaign/%s",
    "../game/data/mechanics/campaign/%s",
    "game/data/mechanics/campaign/dispatch/%s",
    "../game/data/mechanics/campaign/dispatch/%s",
    /* 5. Presentation Domain */
    "game/data/presentation/%s",
    "../game/data/presentation/%s",
    "game/data/presentation/ui/%s",
    "../game/data/presentation/ui/%s",
    "game/data/presentation/ui/fonts/%s",
    "../game/data/presentation/ui/fonts/%s",
    "game/data/presentation/audio/%s",
    "../game/data/presentation/audio/%s",
    "game/data/presentation/visuals/%s",
    "../game/data/presentation/visuals/%s",
    "game/data/presentation/visuals/palettes/%s",
    "../game/data/presentation/visuals/palettes/%s"
};

bool fs_resolve_data_path(const char *rel_path,
                          char *out_path,
                          size_t out_size)
{
    if (rel_path == NULL || rel_path[0] == '\0' ||
        out_path == NULL || out_size == 0) {
        return false;
    }

    /* Step 1: Direct existence check */
    if (fs_is_regular_file(rel_path)) {
        str_safe_copy(out_path, rel_path, out_size);
        return true;
    }

    /* Step 2: Strip leading workspace/data prefixes */
    const char *stripped = rel_path;
    if (strncmp(stripped, "../game/data/", 13) == 0) {
        stripped += 13;
    } else if (strncmp(stripped, "game/data/", 10) == 0) {
        stripped += 10;
    } else if (strncmp(stripped, "../data/", 8) == 0) {
        stripped += 8;
    } else if (strncmp(stripped, "../game/", 8) == 0) {
        stripped += 8;
    } else if (strncmp(stripped, "game/", 5) == 0) {
        stripped += 5;
    } else if (strncmp(stripped, "../", 3) == 0) {
        stripped += 3;
    }

    /* Step 3: Check legacy mappings */
    size_t map_count = sizeof(s_legacy_mappings) / sizeof(s_legacy_mappings[0]);
    size_t root_count = sizeof(s_base_data_roots) /
                        sizeof(s_base_data_roots[0]);
    for (size_t m = 0; m < map_count; m++) {
        const legacy_path_map_t *entry = &s_legacy_mappings[m];
        if (strncmp(stripped, entry->prefix, entry->prefix_len) == 0) {
            char mapped[512];
            snprintf(mapped, sizeof(mapped), "%s%s", entry->replacement,
                     stripped + entry->prefix_len);
            for (size_t r = 0; r < root_count; r++) {
                char candidate[512];
                int n = snprintf(candidate, sizeof(candidate),
                                 s_base_data_roots[r], mapped);
                if (n > 0 && (size_t)n < sizeof(candidate)) {
                    if (fs_is_regular_file(candidate)) {
                        str_safe_copy(out_path, candidate, out_size);
                        return true;
                    }
                }
            }
        }
    }

    /* Step 4: Check stripped path against base roots */
    for (size_t r = 0; r < root_count; r++) {
        char candidate[512];
        int n = snprintf(candidate, sizeof(candidate),
                         s_base_data_roots[r], stripped);
        if (n > 0 && (size_t)n < sizeof(candidate)) {
            if (fs_is_regular_file(candidate)) {
                str_safe_copy(out_path, candidate, out_size);
                return true;
            }
        }
    }

    /* Step 5: Check all domain candidate prefixes */
    size_t count = sizeof(s_data_candidate_prefixes) /
                   sizeof(s_data_candidate_prefixes[0]);
    for (size_t i = 0; i < count; i++) {
        char candidate[512];
        int written = snprintf(candidate, sizeof(candidate),
                               s_data_candidate_prefixes[i], stripped);
        if (written > 0 && (size_t)written < sizeof(candidate)) {
            if (fs_is_regular_file(candidate)) {
                str_safe_copy(out_path, candidate, out_size);
                return true;
            }
        }
    }

    /* Fallback test with unstripped path */
    if (stripped != rel_path) {
        for (size_t i = 0; i < count; i++) {
            char candidate[512];
            int written = snprintf(candidate, sizeof(candidate),
                                   s_data_candidate_prefixes[i], rel_path);
            if (written > 0 && (size_t)written < sizeof(candidate)) {
                if (fs_is_regular_file(candidate)) {
                    str_safe_copy(out_path, candidate, out_size);
                    return true;
                }
            }
        }
    }

    return false;
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
