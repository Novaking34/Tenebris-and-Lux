// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          string_utils.c
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Implementation of String Trimming, Matching & Safe Copy
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Safe buffer bounds: dst[dst_size - 1] is always guaranteed null.
//                2. Pointer safety: handles NULL pointers gracefully without faults.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / STATIC DATA
// =============================================================================

#include "util/string_utils.h"

#include <ctype.h>
#include <string.h>

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
// =============================================================================

bool string_utils_audit(void)
{
    char buf[64];

    str_safe_copy(buf, "   hello world   ", sizeof(buf));
    str_trim_inplace(buf);
    if (strcmp(buf, "hello world") != 0) return false;

    str_safe_copy(buf, "     ", sizeof(buf));
    str_trim_inplace(buf);
    if (strcmp(buf, "") != 0) return false;

    if (!str_case_eq("WARD", "ward")) return false;
    if (str_case_eq("ward", "strike")) return false;

    if (!str_contains_nocase("stance striking", "strik")) return false;
    if (!str_contains_nocase("STANCE WARDING", "ward")) return false;
    if (str_contains_nocase("ward", "strike")) return false;

    return true;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

// End of string_utils.c
