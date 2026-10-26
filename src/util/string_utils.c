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
//   Pure implementation of bounded string copies, in-place whitespace trimming,
//   and ASCII case-insensitive substring searching.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (In-place or caller-supplied stack buffers).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant execution on provided buffers).
//   - Latency Budget:        < 10 microseconds per operation.
//
// AXIOMATIC INVARIANTS:
//   1. Safe buffer bounds: dst[dst_size - 1] is unconditionally guaranteed null.
//   2. Pointer safety: Handles NULL pointers gracefully without segmentation faults.
//   3. Zero dynamic allocations.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
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
// ( String Implementations )
// =============================================================================

void str_trim_inplace(char *str)
{
    if (str == NULL) return;

    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    if (*str == '\0') {
        return;
    }

    char *end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}

bool str_case_eq(const char *a, const char *b)
{
    if (a == NULL || b == NULL) return (a == b);

    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

bool str_contains_nocase(const char *haystack, const char *needle)
{
    if (haystack == NULL || needle == NULL) return false;
    if (*needle == '\0') return true;

    size_t n_len = strlen(needle);
    size_t h_len = strlen(haystack);
    if (n_len > h_len) return false;

    for (size_t i = 0; i <= h_len - n_len; i++) {
        size_t j = 0;
        while (j < n_len && tolower((unsigned char)haystack[i + j]) ==
                            tolower((unsigned char)needle[j])) {
            j++;
        }
        if (j == n_len) return true;
    }
    return false;
}

void str_safe_copy(char *dst, const char *src, size_t dst_size)
{
    if (dst == NULL || dst_size == 0) return;
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }
    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Pre-Flight Loose-Bolts Inspection & Self-Audits )
// =============================================================================

diag_result_t string_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = string_utils_manifest.module_id;
    res.check_name = "Buffer Bounds & NULL Safety";

    /* Test 1: Buffer truncation and null-termination safety */
    char buf[8];
    memset(buf, 'Z', sizeof(buf));
    str_safe_copy(buf, "0123456789ABCDEF", sizeof(buf));
    if (buf[sizeof(buf) - 1] != '\0' || strlen(buf) != 7 || strcmp(buf, "0123456") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_safe_copy failed buffer bounds null-termination.";
        return res;
    }

    /* Test 2: NULL pointer resilience */
    str_safe_copy(NULL, "valid", 10);
    str_safe_copy(buf, NULL, sizeof(buf));
    if (buf[0] != '\0') {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_safe_copy did not initialize destination on NULL source.";
        return res;
    }
    str_trim_inplace(NULL);
    if (!str_case_eq(NULL, NULL) || str_case_eq("a", NULL) || str_case_eq(NULL, "b")) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_case_eq failed NULL pointer comparison contract.";
        return res;
    }
    if (str_contains_nocase(NULL, "test") || str_contains_nocase("test", NULL)) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: str_contains_nocase crashed or faulted on NULL pointer.";
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
        res.details = "Fatal: str_trim_inplace failed on pure whitespace string.";
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

bool string_utils_audit(void)
{
    diag_result_t pf = string_utils_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of string_utils.c
