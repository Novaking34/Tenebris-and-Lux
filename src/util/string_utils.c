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

#include "util/string_utils.h"

#include <ctype.h>
#include <string.h>

static const char *const string_invariants[] = {
    "Form precedes fullness: Safe null-termination guaranteed",
    "Buffer bounds enforcement: dst[dst_size - 1] == '\\0'",
    "Graceful NULL pointer resilience across all operations",
    "Zero dynamic heap allocations: in-place mutations only"
};

const module_metadata_t string_utils_manifest = {
    "core.util.string",
    "string_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 0, 0 },
    1,                          // interface_rev
    "2026-10-26",               // last_updated
    "Safe string copying, in-place trimming and case-insensitive matching",
    0,                          // dependency_count
    { NULL },                   // dependencies
    ALLOC_ZERO_HEAP,            // alloc_policy
    CONCURRENCY_REENTRANT,      // concurrency
    10,                         // max_budget_us
    4,
    {
        string_invariants[0],
        string_invariants[1],
        string_invariants[2],
        string_invariants[3]
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
 * Copies a source string into a destination buffer with bounded termination.
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
 * Trims leading and trailing ASCII whitespace in-place within a buffer.
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
 * Compares two null-terminated strings for equality ignoring ASCII case.
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
 * Searches for a case-insensitive substring match within a haystack string.
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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify string buffer safety, NULL resilience, and trimming.
// -----------------------------------------------------------------------------

/**
 * Pre-flight diagnostic check evaluating safe string invariants.
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
        str_contains_nocase("Sacyra", "Malakor")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: Case-insensitive string matching logic error.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Buffer safety, NULL resilience & trimming verified.";
    return res;
}

/**
 * Standalone boolean self-audit wrapper for regression test suites.
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
