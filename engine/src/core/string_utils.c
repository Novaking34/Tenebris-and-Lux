// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          string_utils.c
// @id            core.util.string
// @layer         Layer 0: Core Utilities
// @version       1.0.0
// @interface_rev Rev 1
// @date          2026-10-26
// @updated       2026-10-26
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Pure implementation of bounded string copies, in-place whitespace
//   trimming, and ASCII case-insensitive substring searching. Guarantees
//   memory safety, zero heap allocations, and NULL resilience for all text.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (In-place or caller buffers).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant buffer operations).
//   - Latency Budget:        < 10 microseconds per operation.
//
// AXIOMATIC INVARIANTS:
//   1. Safe buffer bounds: dst[dst_size - 1] is unconditionally null.
//   2. Pointer safety: Handles NULL pointers gracefully without crashes.
//   3. Zero dynamic allocations.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
// =============================================================================

#include "core/string_utils.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *const string_invariants[] = {
    "Form precedes fullness: Safe null-termination guaranteed",
    "Buffer bounds enforcement: dst[dst_size - 1] == '\\0'",
    "Graceful NULL pointer resilience across all operations",
    "Zero dynamic heap allocations: in-place mutations only",
    "Safe parent directory extraction and bounded path joining"
};

const module_metadata_t string_utils_manifest = {
    "core.util.string",
    "string_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 1, 0 },
    2,                          // interface_rev
    "2026-11-02",               // last_updated
    "Safe string copying, in-place trimming, case-insensitive matching, and path manipulation",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    10,                         // max_budget_us
    5,
    {
        string_invariants[0],
        string_invariants[1],
        string_invariants[2],
        string_invariants[3],
        string_invariants[4]
    },
    string_utils_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: MEMORY-SAFE BUFFER OPERATIONS ]
// Mission: Guardrail string copying and trimming against memory bugs.
// -----------------------------------------------------------------------------

/**
 * @brief Copies a source string into a destination buffer with bounded termination.
 *
 * ### Machine Role & Mission
 *
 * Core text safety primitive replacing dangerous standard `strcpy`/`strncpy`.
 * Guarantees that the destination buffer is always properly null-terminated at
 * `dst[dst_size - 1]`, preventing buffer overruns and memory leaks.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.string (Safe String Engine)
 * - Assembly:   Memory-Safe Buffer Operations
 * - Cog:        Bounded Safe String Copy
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (In-place caller-supplied buffer).
 * - Concurrency:    REENTRANT_PURE (Thread-safe on distinct buffers).
 * - Latency Budget: < 5 microseconds execution time (O(N) bounded).
 *
 * @param[out] dst      Destination memory buffer.
 * @param[in]  src      Source null-terminated string (safe if NULL).
 * @param[in]  dst_size Capacity of destination buffer in bytes.
 *
 * @post `dst[dst_size - 1] == '\0'` if `dst != NULL` and `dst_size > 0`.
 *
 * @note If `src` is NULL or `dst_size` is 0, safely writes empty or no-ops.
 */
void str_safe_copy(char *dst, const char *src, size_t dst_size)
{
    /* Step 1: Precondition validation (ignore invalid destination buffers) */
    if (dst == NULL || dst_size == 0) {
        return;
    }

    /* Step 2: Graceful handling of NULL source string */
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }

    /* Step 3: Bounded copy copying at most dst_size - 1 characters */
    strncpy(dst, src, dst_size - 1);

    /* Step 4: Unconditional terminal null-byte anchor */
    dst[dst_size - 1] = '\0';
}

/**
 * @brief Trims leading and trailing ASCII whitespace in-place within a buffer.
 *
 * ### Machine Role & Mission
 *
 * Cleans user CLI and dialogue inputs before command parsing and keyword
 * dispatch. Compacts string in-place without dynamic heap reallocations.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.string (Safe String Engine)
 * - Assembly:   Memory-Safe Buffer Operations
 * - Cog:        In-Place Whitespace Trimmer
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, in-place memmove).
 * - Concurrency:    REENTRANT_PURE (Thread-safe on caller memory).
 * - Latency Budget: < 5 microseconds execution time (O(N)).
 *
 * @param[in,out] str Null-terminated string buffer to mutate in-place.
 */
void str_trim_inplace(char *str)
{
    /* Step 1: Null pointer check */
    if (str == NULL) {
        return;
    }

    /* Step 2: Scan past leading whitespace characters */
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    /* Step 3: Shift trimmed text to buffer origin if leading space existed */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    /* Step 4: Check if string is now empty */
    if (*str == '\0') {
        return;
    }

    /* Step 5: Scan backward from the end and prune trailing whitespace */
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: CASE-INSENSITIVE TEXT MATCHING COGS ]
// Mission: Parse player command verbs and dialogue responses flexibly.
// -----------------------------------------------------------------------------

/**
 * @brief Compares two null-terminated strings for equality ignoring ASCII case.
 *
 * ### Machine Role & Mission
 *
 * Provides forgiving command verb parsing in the CLI ('WARD' vs 'ward',
 * 'Strike' vs 'strike'), allowing seamless user interaction.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.string (Safe String Engine)
 * - Assembly:   Case-Insensitive Text Matching
 * - Cog:        Case-Insensitive String Comparator
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, CPU registers only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 5 microseconds execution time (O(min(len_a, len_b))).
 *
 * @param[in] a First null-terminated string (safe if NULL).
 * @param[in] b Second null-terminated string (safe if NULL).
 *
 * @return `true` if strings match character-for-character ignoring case.
 */
bool str_case_eq(const char *a, const char *b)
{
    /* Step 1: Pointer identity & NULL safety checks */
    if (a == NULL || b == NULL) {
        return (a == b);
    }

    /* Step 2: Character-by-character case-folded traversal */
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++;
        b++;
    }

    /* Step 3: Confirm simultaneous termination */
    return (*a == '\0' && *b == '\0');
}

/**
 * @brief Searches for a case-insensitive substring match within a haystack string.
 *
 * ### Machine Role & Mission
 *
 * Enables flexible natural language input parsing for command aliases
 * and dialogue keyword evaluation.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.string (Safe String Engine)
 * - Assembly:   Case-Insensitive Text Matching
 * - Cog:        Case-Insensitive Substring Matcher
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, stack indices only).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 10 microseconds execution time (O(N * M)).
 *
 * @param[in] haystack The parent text to scan (safe if NULL).
 * @param[in] needle   The substring pattern to find (safe if NULL).
 *
 * @return `true` if `needle` occurs within `haystack` ignoring case.
 */
bool str_contains_nocase(const char *haystack, const char *needle)
{
    /* Step 1: Precondition check on NULL inputs */
    if (haystack == NULL || needle == NULL) {
        return false;
    }

    /* Step 2: Empty needle is trivially contained in any haystack */
    if (*needle == '\0') {
        return true;
    }

    /* Step 3: Length verification to prevent out-of-bounds scanning */
    size_t n_len = strlen(needle);
    size_t h_len = strlen(haystack);
    if (n_len > h_len) {
        return false;
    }

    /* Step 4: Sliding window substring match */
    for (size_t i = 0; i <= h_len - n_len; i++) {
        size_t j = 0;
        while (j < n_len &&
               tolower((unsigned char)haystack[i + j]) ==
               tolower((unsigned char)needle[j])) {
            j++;
        }
        if (j == n_len) {
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: PATH & DIRECTORY STRING MANIPULATION COGS ]
// Mission: Safe extraction of parent directories and safe path joining.
// -----------------------------------------------------------------------------

/**
 * @brief Extracts the directory component of a filepath into a destination buffer.
 *
 * @param[out] out_dir   Destination buffer.
 * @param[in]  out_size  Capacity of destination buffer in bytes.
 * @param[in]  filepath  Source null-terminated filepath.
 *
 * @return `true` if directory was extracted within bounds; `false` on error/NULL.
 */
bool str_parent_dir(char *out_dir, size_t out_size, const char *filepath)
{
    /* Step 1: Precondition check */
    if (out_dir == NULL || out_size == 0) {
        return false;
    }
    out_dir[0] = '\0';

    if (filepath == NULL || filepath[0] == '\0') {
        return false;
    }

    /* Step 2: Locate last slash separator */
    const char *last_slash = strrchr(filepath, '/');
    if (!last_slash) {
        // No directory component; default to current directory "."
        str_safe_copy(out_dir, ".", out_size);
        return true;
    }

    /* Step 3: Handle root directory case */
    size_t dir_len = (size_t)(last_slash - filepath);
    if (dir_len == 0) {
        // Root directory "/"
        str_safe_copy(out_dir, "/", out_size);
        return true;
    }

    /* Step 4: Capacity check before bounded copy */
    if (dir_len >= out_size) {
        return false;
    }

    memcpy(out_dir, filepath, dir_len);
    out_dir[dir_len] = '\0';
    return true;
}

/**
 * @brief Safely joins a directory path and filename with a single intervening separator.
 *
 * @param[out] dest      Destination memory buffer.
 * @param[in]  dest_size Capacity of destination buffer in bytes.
 * @param[in]  dir       Directory path prefix (e.g. "data/log").
 * @param[in]  filename  File component (e.g. "default.json").
 *
 * @return `true` if joined successfully within bounds; `false` on error/NULL.
 */
bool str_path_join(char *dest, size_t dest_size, const char *dir, const char *filename)
{
    /* Step 1: Precondition check */
    if (dest == NULL || dest_size == 0) {
        return false;
    }
    dest[0] = '\0';

    /* Step 2: Handle NULL or empty filename */
    if (filename == NULL || filename[0] == '\0') {
        if (dir != NULL) {
            str_safe_copy(dest, dir, dest_size);
            return true;
        }
        return false;
    }

    /* Step 3: Handle NULL or empty directory */
    if (dir == NULL || dir[0] == '\0') {
        str_safe_copy(dest, filename, dest_size);
        return true;
    }

    /* Step 4: Format with single separator */
    size_t dir_len = strlen(dir);
    bool has_trailing_slash = (dir[dir_len - 1] == '/');

    int written = 0;
    if (has_trailing_slash) {
        written = snprintf(dest, dest_size, "%s%s", dir, filename);
    } else {
        written = snprintf(dest, dest_size, "%s/%s", dir, filename);
    }

    return (written > 0 && (size_t)written < dest_size);
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.4: DATA-DRIVEN LEXICAL CONFIGURATION & FALLBACK TRIPWIRES ]
// Mission: Load lexical bounds and dictionaries from JSON with hardcoded fallback.
// -----------------------------------------------------------------------------

/**
 * @brief Returns the hardcoded fallback lexical configuration profile.
 *
 * @return Default baseline string_config_t with canonical bounds.
 */
string_config_t string_get_default_config(void)
{
    /* Step 1: Zero-initialize and populate canonical defaults */
    string_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    str_safe_copy(cfg.id, "default", sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Standard Continental Lexical Profile", sizeof(cfg.display_name));
    cfg.max_path_length = 512;
    cfg.max_name_length = 64;
    cfg.max_command_length = 128;
    cfg.max_line_length = 80;
    str_safe_copy(cfg.ellipsis, "...", sizeof(cfg.ellipsis));
    return cfg;
}

/**
 * @brief Loads a lexical configuration profile from a JSON file.
 *
 * @param[in]  filepath   Null-terminated path to profile JSON.
 * @param[out] out_config Destination configuration structure.
 *
 * @return `true` if loaded from file; `false` if fallback tripwire was triggered.
 */
bool string_load_config_from_file(const char *filepath, string_config_t *out_config)
{
    /* Step 1: Safety check on destination */
    if (out_config == NULL) {
        return false;
    }

    /* Step 2: Initialize with default fallback config */
    *out_config = string_get_default_config();

    if (filepath == NULL || filepath[0] == '\0') {
        return false;
    }

    /* Step 3: Read file contents with fallback relative path */
    char resolved[FS_PATH_MAX];
    const char *target = filepath;
    if (fs_resolve_data_path(filepath, resolved, sizeof(resolved))) {
        target = resolved;
    }

    char buf[FS_BUFFER_MEDIUM_BYTES];
    bool loaded = fs_read_file_contents(target, buf, sizeof(buf));
    if (!loaded) {
        char alt_path[FS_PATH_MAX];
        snprintf(alt_path, sizeof(alt_path), "game/%s", filepath);
        loaded = fs_read_file_contents(alt_path, buf, sizeof(buf));
    }
    if (!loaded) {
        char alt_path[FS_PATH_MAX];
        snprintf(alt_path, sizeof(alt_path), "../%s", filepath);
        loaded = fs_read_file_contents(alt_path, buf, sizeof(buf));
    }
    if (!loaded) {
        char alt_path[FS_PATH_MAX];
        snprintf(alt_path, sizeof(alt_path), "../game/%s", filepath);
        loaded = fs_read_file_contents(alt_path, buf, sizeof(buf));
    }

    if (!loaded) {
        return false;
    }

    /* Step 4: Parse JSON fields */
    json_get_object_string(buf, "profile", "id", out_config->id, sizeof(out_config->id));
    json_get_object_string(buf, "profile", "display_name", out_config->display_name, sizeof(out_config->display_name));
    json_get_size_t(buf, "max_path_length", &out_config->max_path_length);
    json_get_size_t(buf, "max_name_length", &out_config->max_name_length);
    json_get_size_t(buf, "max_command_length", &out_config->max_command_length);
    json_get_size_t(buf, "max_line_length", &out_config->max_line_length);
    json_get_object_string(buf, "profile", "ellipsis", out_config->ellipsis, sizeof(out_config->ellipsis));

    return true;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify string buffer safety, NULL resilience, and trimming.
// -----------------------------------------------------------------------------

/**
 * @brief Pre-flight diagnostic check evaluating safe string invariants.
 *
 * ### Machine Role & Mission
 *
 * Invoked during pre-flight boot by the Central Manifest orchestrator.
 * Confirms buffer truncation safety, unconditional null termination,
 * NULL pointer resilience, whitespace trimming, and case-folding.
 *
 * @return Diagnostic check outcome reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t string_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = string_utils_manifest.module_id;
    res.check_name = "Buffer Bounds & NULL Safety";

    /* Test 1: Buffer truncation and null-termination safety */
    char buf[8];
    memset(buf, 'Z', sizeof(buf));
    str_safe_copy(buf, "0123456789ABCDEF", sizeof(buf));
    if (buf[sizeof(buf) - 1] != '\0' || strlen(buf) != 7 ||
        strcmp(buf, "0123456") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_safe_copy failed buffer null-termination.";
        return res;
    }

    /* Test 2: NULL pointer resilience */
    str_safe_copy(NULL, "valid", 10);
    str_safe_copy(buf, NULL, sizeof(buf));
    if (buf[0] != '\0') {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_safe_copy did not initialize on NULL src.";
        return res;
    }
    str_trim_inplace(NULL);
    if (!str_case_eq(NULL, NULL) || str_case_eq("a", NULL) ||
        str_case_eq(NULL, "b")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_case_eq failed NULL comparison contract.";
        return res;
    }
    if (str_contains_nocase(NULL, "test") ||
        str_contains_nocase("test", NULL)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_contains_nocase faulted on NULL pointer.";
        return res;
    }

    /* Test 3: In-place whitespace trimming edge cases */
    char t1[16] = "";
    str_trim_inplace(t1);
    if (t1[0] != '\0') {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_trim_inplace corrupted empty string.";
        return res;
    }

    char t2[16] = "   \t  \n ";
    str_trim_inplace(t2);
    if (t2[0] != '\0') {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_trim_inplace failed on whitespace string.";
        return res;
    }

    char t3[32] = " \t  Shield of Lux \r\n ";
    str_trim_inplace(t3);
    if (strcmp(t3, "Shield of Lux") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_trim_inplace corrupted padded token.";
        return res;
    }

    /* Test 4: Case-insensitive matching */
    if (!str_case_eq("TENEBRIS", "tenebris") ||
        str_case_eq("tenebris", "lux") ||
        !str_contains_nocase("Kingdom of Sacyra", "sacyra") ||
        str_contains_nocase("Sacyra", "Abyss")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Case-insensitive string matching logic error.";
        return res;
    }

    /* Test 5: Path parent and path joining */
    char path_buf[64];
    if (!str_parent_dir(path_buf, sizeof(path_buf), "data/log/manifest.json") ||
        strcmp(path_buf, "data/log") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_parent_dir failed parent directory extraction.";
        return res;
    }

    if (!str_parent_dir(path_buf, sizeof(path_buf), "manifest.json") ||
        strcmp(path_buf, ".") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_parent_dir failed default dot fallback.";
        return res;
    }

    if (!str_path_join(path_buf, sizeof(path_buf), "data/log", "default.json") ||
        strcmp(path_buf, "data/log/default.json") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_path_join failed path concatenation.";
        return res;
    }

    if (!str_path_join(path_buf, sizeof(path_buf), "data/log/", "default.json") ||
        strcmp(path_buf, "data/log/default.json") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_path_join produced duplicate slash.";
        return res;
    }

    /* Test 6: Data-driven configuration profile loading and hardcoded fallback */
    string_config_t default_cfg = string_get_default_config();
    if (default_cfg.max_path_length != 512 || default_cfg.max_line_length != 80 ||
        strcmp(default_cfg.ellipsis, "...") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: string_get_default_config failed canonical invariant.";
        return res;
    }

    string_config_t loaded_cfg;
    if (string_load_config_from_file(NULL, &loaded_cfg) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: string_load_config_from_file(NULL) failed fallback contract.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Buffer safety, NULL resilience, trimming, path joining & config profiles verified.";
    return res;
}

/**
 * @brief Standalone boolean self-audit wrapper for regression test suites.
 *
 * @return `true` if all string invariant tripwires pass cleanly.
 */
bool string_utils_audit(void)
{
    diag_result_t pf = string_utils_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of string_utils.c
