// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          fs_utils.c
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
//   Foundational zero-heap filesystem abstraction layer implementation.
//   Provides robust, bounded file reading into caller-allocated buffers,
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

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "core/string_utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: STATIC AXIOMATIC INVARIANTS & MODULE MANIFEST ]
// -----------------------------------------------------------------------------

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
    "2026-11-05",
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.2: LEGACY REDIRECTION & FALLBACK ROUTING TABLES ]
// -----------------------------------------------------------------------------

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
    { "data/player/morality_matrix.json", 33, "mechanics/campaign/morality.json" },
    { "player/morality_matrix.json", 28, "mechanics/campaign/morality.json" },
    { "data/player/inventory_spec.json", 31, "mechanics/campaign/inventory_spec.json" },
    { "player/inventory_spec.json", 26, "mechanics/campaign/inventory_spec.json" },
    { "data/player/manifest.json", 26, "mechanics/campaign/player_manifest.json" },
    { "player/manifest.json", 21, "mechanics/campaign/player_manifest.json" },
    { "data/player/", 12, "mechanics/campaign/" },
    { "player/", 7, "mechanics/campaign/" },
    { "data/world/maps.json", 20, "world/battlefields/maps.json" },
    { "world/maps.json", 15, "world/battlefields/maps.json" },
    { "data/world/map_types.json", 25, "world/battlefields/map_types.json" },
    { "world/map_types.json", 20, "world/battlefields/map_types.json" },
    { "data/world/overworld.json", 25, "world/geography/realms.json" },
    { "world/overworld.json", 20, "world/geography/realms.json" },
    { "data/gui/palette.json", 22, "presentation/visuals/palettes/legacy_palette.json" },
    { "gui/palette.json", 17, "presentation/visuals/palettes/legacy_palette.json" },
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
    "game/data/world/%s",
    "../game/data/world/%s",
    "game/data/entities/%s",
    "../game/data/entities/%s",
    "game/data/mechanics/%s",
    "../game/data/mechanics/%s",
    "game/data/presentation/%s",
    "../game/data/presentation/%s"
};

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: FILESYSTEM INSPECTION & PROBING COGS ]
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: BUFFERED I/O & DIRECTORY MANAGEMENT COGS ]
// -----------------------------------------------------------------------------

bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size)
{
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

    return true;
}

bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read)
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

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: DYNAMIC VFS MOUNTING & PATH RESOLUTION COGS ]
// -----------------------------------------------------------------------------

bool fs_set_data_root(const char *root_path)
{
    return path_finder_set_data_root(root_path);
}

const char *fs_get_data_root(void)
{
    return path_finder_get_data_root();
}

bool fs_resolve_data_path(const char *rel_path,
                          char *out_path,
                          size_t out_size)
{
    if (rel_path == NULL || rel_path[0] == '\0' ||
        out_path == NULL || out_size == 0) {
        return false;
    }

    /* Primary resolution via Layer 0 path_finder engine */
    if (path_finder_resolve(NULL, rel_path, out_path, out_size)) {
        return true;
    }

    /* Fallback candidate 1: Direct file check */
    if (fs_is_regular_file(rel_path)) {
        str_safe_copy(out_path, rel_path, out_size);
        return true;
    }

    /* Fallback candidate 2: Legacy path prefix mapping */
    const char *stripped = rel_path;
    if (strncmp(stripped, "../game/data/", 13) == 0) {
        stripped += 13;
    } else if (strncmp(stripped, "game/data/", 10) == 0) {
        stripped += 10;
    } else if (strncmp(stripped, "../data/", 8) == 0) {
        stripped += 8;
    } else if (strncmp(stripped, "data/", 5) == 0) {
        stripped += 5;
    } else if (strncmp(stripped, "../", 3) == 0) {
        stripped += 3;
    }

    size_t map_count = sizeof(s_legacy_mappings) / sizeof(s_legacy_mappings[0]);
    size_t root_count = sizeof(s_base_data_roots) / sizeof(s_base_data_roots[0]);

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

    /* Fallback candidate 3: Base candidate roots */
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

    /* Fallback candidate 4: Explicit candidate prefixes */
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

    out_path[0] = '\0';
    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

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
        res.severity = SEVERITY_FATAL;
        res.details = "fs_file_exists('/dev/null') unexpectedly returned false.";
        return res;
    }

    /* Check 3: Non-existent path probing */
    if (fs_file_exists("/__tenebris_lux_nonexistent_path_xyz__")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
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

    /* Check 6: Executable procedure testing (Block 5 procedures) */
    fs_file_info_t info;
    if (!fs_inspect_file("/dev/null", &info) || !info.exists || info.is_regular) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_inspect_file failed on /dev/null.";
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
// BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap Filesystem Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: EXECUTABLE PROCEDURES & OBJECT CONSTRUCTORS ]
// -----------------------------------------------------------------------------

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

bool fs_execute_read(const char *path, char *buf, size_t buf_size,
                     fs_read_result_t *out_result)
{
    if (out_result) {
        memset(out_result, 0, sizeof(*out_result));
    }

    if (path == NULL || buf == NULL || buf_size == 0) {
        if (buf && buf_size > 0) {
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

    if (out_result) {
        out_result->success = ok;
        out_result->bytes_read = read_bytes;
        str_safe_copy(out_result->path, target, sizeof(out_result->path));
    }

    return ok;
}

bool fs_execute_write(const char *path, const char *data, size_t len,
                      fs_write_result_t *out_result)
{
    if (out_result) {
        memset(out_result, 0, sizeof(*out_result));
    }

    if (path == NULL || data == NULL) {
        return false;
    }

    bool ok = fs_write_file_contents(path, data, len);

    if (out_result) {
        out_result->success = ok;
        out_result->bytes_written = ok ? len : 0;
        str_safe_copy(out_result->path, path, sizeof(out_result->path));
    }

    return ok;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.2: SUBSYSTEM LIFECYCLE INITIALIZATION & TEARDOWN ]
// -----------------------------------------------------------------------------

void fs_utils_init(void)
{
    /* Initialize active data root from environment if available */
    fs_set_data_root(NULL);
}

void fs_utils_shutdown(void)
{
    /* Graceful teardown of filesystem engine */
}

// =============================================================================
// End of fs_utils.c
// =============================================================================
