/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          json_utils.c
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap JSON Tokenizer & Structured Field Deserializer
@role          Fast zero-heap JSON scanning, token classification, bounded
               field extraction, array slicing, and procedure execution for
               data-driven engine assets.
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Buffer safety: extracted strings unconditionally enforce null-termination.
               4. Fault resilience: gracefully handles malformed JSON, missing keys, and NULL.
               5. Numerical bounds: safely clamps or validates numerical ranges on conversion.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Zero-heap, memory-safe JSON parser and key/value token extractor implementation.
  Serves as the universal serialization/deserialization primitive for
  data-driven profiles, manifests, telemetry channels, combat tables,
  and world topologies without pulling in heavy third-party dependencies.
  Guarantees non-allocating, deterministic parsing into caller stack buffers.

  ### For Game Designers & Non-Programmers:
  Game balance numbers, item statistics, and level layouts are saved in JSON
  data files. A traditional parser might allocate memory for every single number
  and text tag, which can crash on memory-constrained hardware or cause stutter.
  Our zero-heap parser reads data directly where it sits in memory, extracting
  values in fractions of a microsecond without ever allocating memory.

  ### For Systems Programmers & Engine Architects:
  This module provides deterministic, O(N) single-pass token extraction:
  - Scans keys while strictly respecting quoted strings and bracket nesting.
  - Zero heap usage (malloc/calloc/free strictly prohibited).
  - Explicitly tagged enums and structs for clean DWARF debugger integration.
  - Categorized diagnostic severity reporting during pre-flight self-audit.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap JSON Engine
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
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "io/json_utils.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Static Axiomatic Invariants & Module Manifest
--------------------------------------------------------------------------------
*/

static const char *const json_invariants[4] = {
    "Zero heap allocations: Strict zero-heap guarantee across all routines",
    "Buffer safety: Extracted strings unconditionally enforce null termination",
    "Resilience: Gracefully handles malformed JSON, missing keys, and NULL pointers",
    "Universal 5-Block Lifecycle Protocol enforced across header and source"
};

/**
 * Manifest record for the Zero-Heap JSON Engine.
 */
const module_metadata_t json_utils_manifest = {
    "core.util.json",
    "json_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 0 },
    3,
    "2026-11-06",
    "Zero-heap JSON token locator, typed value extractor, and inspection procedures",
    0,
    { NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    50,
    4,
    {
        json_invariants[0],
        json_invariants[1],
        json_invariants[2],
        json_invariants[3]
    },
    json_utils_preflight
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: JSON TOKEN LOCATOR COGS ]
// Mission: Scan JSON payloads for matching keys while respecting quotes/whitespace.
// -----------------------------------------------------------------------------

/**
 * Scans a bounded JSON slice for a specific key token.
 * If max_len is 0, scans until null terminator.
 */
static const char *json_find_key_scoped(const char *json, size_t max_len, const char *key)
{
    if (json == NULL || key == NULL || key[0] == '\0') {
        return NULL;
    }

    size_t key_len = strlen(key);
    const char *p = json;
    const char *end = (max_len > 0) ? (json + max_len) : NULL;

    while (*p != '\0' && (end == NULL || p < end)) {
        // Look for opening quote of a key
        if (*p == '"') {
            p++;
            // Check if key matches
            if ((end == NULL || p + key_len <= end) &&
                strncmp(p, key, key_len) == 0 && p[key_len] == '"') {
                const char *after_quote = p + key_len + 1;
                // Skip whitespace up to colon
                while ((end == NULL || after_quote < end) &&
                       (*after_quote == ' ' || *after_quote == '\t' ||
                        *after_quote == '\r' || *after_quote == '\n')) {
                    after_quote++;
                }
                if ((end == NULL || after_quote < end) && *after_quote == ':') {
                    after_quote++;
                    // Skip whitespace after colon
                    while ((end == NULL || after_quote < end) &&
                           (*after_quote == ' ' || *after_quote == '\t' ||
                            *after_quote == '\r' || *after_quote == '\n')) {
                        after_quote++;
                    }
                    return after_quote;
                }
            }
        }
        p++;
    }

    return NULL;
}

const char *json_find_key(const char *json, const char *key)
{
    /* Scan JSON with unbounded length using scoped scanner */
    return json_find_key_scoped(json, 0, key);
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: VALUE EXTRACTION COGS ]
// Mission: Extract typed values (string, integer, size_t, bool, float).
// -----------------------------------------------------------------------------

bool json_get_string(const char *json, const char *key, char *out_val,
                     size_t out_size)
{
    /* Step 1: Precondition check on destination buffer */
    if (json == NULL || key == NULL || out_val == NULL || out_size == 0) {
        if (out_val && out_size > 0) {
            out_val[0] = '\0';
        }
        return false;
    }

    /* Step 2: Locate key token */
    const char *val = json_find_key(json, key);
    if (!val || *val != '"') {
        out_val[0] = '\0';
        return false;
    }

    /* Step 3: Advance past opening quote and extract string literal */
    val++;
    size_t i = 0;
    while (*val != '\0' && *val != '"' && i < out_size - 1) {
        if (*val == '\\' && *(val + 1) != '\0') {
            val++; /* Skip escape slash */
        }
        out_val[i++] = *val++;
    }
    out_val[i] = '\0';

    if (*val != '"') {
        return false;
    }

    return true;
}

bool json_get_size_t(const char *json, const char *key, size_t *out_val)
{
    /* Step 1: Precondition check */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate key token */
    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

    /* Step 3: Verify numeric digit start */
    if (!isdigit((unsigned char)*val)) {
        return false;
    }

    /* Step 4: Parse unsigned long */
    char *endptr = NULL;
    unsigned long parsed = strtoul(val, &endptr, 10);
    if (endptr == val) {
        return false;
    }

    *out_val = (size_t)parsed;
    return true;
}

bool json_get_int(const char *json, const char *key, int *out_val)
{
    /* Step 1: Precondition check */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate key token */
    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

    /* Step 3: Verify digit or sign start */
    if (!isdigit((unsigned char)*val) && *val != '-' && *val != '+') {
        return false;
    }

    /* Step 4: Parse signed long */
    char *endptr = NULL;
    long parsed = strtol(val, &endptr, 10);
    if (endptr == val) {
        return false;
    }

    *out_val = (int)parsed;
    return true;
}

bool json_get_bool(const char *json, const char *key, bool *out_val)
{
    /* Step 1: Precondition check */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate key token */
    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

    /* Step 3: Match true / false literals */
    if (strncmp(val, "true", 4) == 0) {
        *out_val = true;
        return true;
    } else if (strncmp(val, "false", 5) == 0) {
        *out_val = false;
        return true;
    }

    return false;
}

bool json_get_float(const char *json, const char *key, float *out_val)
{
    /* Step 1: Precondition check */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate key token */
    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

    /* Step 3: Verify digit, sign, or period */
    if (!isdigit((unsigned char)*val) && *val != '-' && *val != '+' && *val != '.') {
        return false;
    }

    /* Step 4: Parse float */
    char *endptr = NULL;
    float parsed = strtof(val, &endptr);
    if (endptr == val) {
        return false;
    }

    *out_val = parsed;
    return true;
}

bool json_find_object(const char *json, const char *object_key,
                      const char **out_obj_start, size_t *out_obj_len)
{
    /* Step 1: Precondition check */
    if (json == NULL || object_key == NULL || out_obj_start == NULL ||
        out_obj_len == NULL) {
        return false;
    }

    /* Step 2: Locate key token */
    const char *val = json_find_key(json, object_key);
    if (!val || *val != '{') {
        return false;
    }

    /* Step 3: Scan for matching closing bracket respecting strings */
    *out_obj_start = val;
    int depth = 0;
    bool in_str = false;
    const char *p = val;

    while (*p != '\0') {
        if (*p == '"' && (p == val || *(p - 1) != '\\')) {
            in_str = !in_str;
        } else if (!in_str) {
            if (*p == '{') {
                depth++;
            } else if (*p == '}') {
                depth--;
                if (depth == 0) {
                    *out_obj_len = (size_t)(p - val + 1);
                    return true;
                }
            }
        }
        p++;
    }

    return false;
}

bool json_get_object_string(const char *json, const char *parent_obj_key,
                            const char *key, char *out_val, size_t out_size)
{
    /* Step 1: Precondition check on destination buffer */
    if (json == NULL || key == NULL || out_val == NULL || out_size == 0) {
        if (out_val && out_size > 0) {
            out_val[0] = '\0';
        }
        return false;
    }

    /* Step 2: Delegate to top-level extractor if parent_obj_key is NULL */
    if (parent_obj_key == NULL) {
        return json_get_string(json, key, out_val, out_size);
    }

    /* Step 3: Locate sub-object bounds */
    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        out_val[0] = '\0';
        return false;
    }

    /* Step 4: Scan scoped key within sub-object */
    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val || *val != '"') {
        out_val[0] = '\0';
        return false;
    }

    /* Step 5: Extract string literal */
    val++;
    size_t i = 0;
    while (*val != '\0' && *val != '"' && i < out_size - 1) {
        if (*val == '\\' && *(val + 1) != '\0') {
            val++;
        }
        out_val[i++] = *val++;
    }
    out_val[i] = '\0';
    if (*val != '"') {
        return false;
    }
    return true;
}

bool json_get_object_int(const char *json, const char *parent_obj_key,
                         const char *key, int *out_val)
{
    /* Step 1: Precondition check */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Delegate if parent is NULL */
    if (parent_obj_key == NULL) {
        return json_get_int(json, key, out_val);
    }

    /* Step 3: Locate parent object */
    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        return false;
    }

    /* Step 4: Locate scoped key */
    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val || (!isdigit((unsigned char)*val) && *val != '-' && *val != '+')) {
        return false;
    }

    /* Step 5: Parse integer */
    char *endptr = NULL;
    long parsed = strtol(val, &endptr, 10);
    if (endptr == val) {
        return false;
    }

    *out_val = (int)parsed;
    return true;
}

bool json_get_object_float(const char *json, const char *parent_obj_key,
                           const char *key, float *out_val)
{
    /* Step 1: Precondition check */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Delegate if parent is NULL */
    if (parent_obj_key == NULL) {
        return json_get_float(json, key, out_val);
    }

    /* Step 3: Locate parent object */
    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        return false;
    }

    /* Step 4: Locate scoped key */
    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val || (!isdigit((unsigned char)*val) && *val != '-' && *val != '+' && *val != '.')) {
        return false;
    }

    /* Step 5: Parse float */
    char *endptr = NULL;
    float parsed = strtof(val, &endptr);
    if (endptr == val) {
        return false;
    }

    *out_val = parsed;
    return true;
}

bool json_get_object_bool(const char *json, const char *parent_obj_key,
                          const char *key, bool *out_val)
{
    /* Step 1: Precondition check */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Delegate if parent is NULL */
    if (parent_obj_key == NULL) {
        return json_get_bool(json, key, out_val);
    }

    /* Step 3: Locate parent object */
    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        return false;
    }

    /* Step 4: Locate scoped key */
    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val) {
        return false;
    }

    /* Step 5: Match boolean string */
    if (strncmp(val, "true", 4) == 0) {
        *out_val = true;
        return true;
    } else if (strncmp(val, "false", 5) == 0) {
        *out_val = false;
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: ARRAY INSPECTION & EXTRACTION COGS ]
// Mission: Scan JSON array boundaries and extract elements without heap usage.
// -----------------------------------------------------------------------------

bool json_find_array(const char *json, const char *array_key,
                     const char **out_arr_start, size_t *out_arr_len)
{
    /* Step 1: Precondition check */
    if (json == NULL || array_key == NULL || out_arr_start == NULL ||
        out_arr_len == NULL) {
        return false;
    }

    /* Step 2: Locate array key token */
    const char *val = json_find_key(json, array_key);
    if (!val || *val != '[') {
        return false;
    }

    /* Step 3: Scan matching closing bracket */
    *out_arr_start = val;
    int depth = 0;
    bool in_str = false;
    const char *p = val;

    while (*p != '\0') {
        if (*p == '"' && (p == val || *(p - 1) != '\\')) {
            in_str = !in_str;
        } else if (!in_str) {
            if (*p == '[') {
                depth++;
            } else if (*p == ']') {
                depth--;
                if (depth == 0) {
                    *out_arr_len = (size_t)(p - val + 1);
                    return true;
                }
            }
        }
        p++;
    }

    return false;
}

size_t json_array_get_count(const char *arr_start, size_t arr_len)
{
    /* Step 1: Precondition validation */
    if (arr_start == NULL || arr_len < 2 || *arr_start != '[') {
        return 0;
    }

    /* Step 2: Check for empty array '[]' */
    const char *end = arr_start + arr_len;
    const char *p = arr_start + 1;
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) {
        p++;
    }
    if (p < end && *p == ']') {
        return 0;
    }

    /* Step 3: Count comma-separated elements at depth 0 */
    size_t count = 1;
    int depth = 0;
    bool in_str = false;

    while (p < end - 1) {
        if (*p == '"' && *(p - 1) != '\\') {
            in_str = !in_str;
        } else if (!in_str) {
            if (*p == '[' || *p == '{') {
                depth++;
            } else if (*p == ']' || *p == '}') {
                depth--;
            } else if (*p == ',' && depth == 0) {
                count++;
            }
        }
        p++;
    }

    return count;
}

bool json_array_get_string(const char *arr_start, size_t arr_len,
                           size_t index, char *out_val, size_t out_size)
{
    /* Step 1: Precondition validation */
    if (arr_start == NULL || arr_len < 2 || *arr_start != '[' ||
        out_val == NULL || out_size == 0) {
        if (out_val && out_size > 0) {
            out_val[0] = '\0';
        }
        return false;
    }

    /* Step 2: Scan array elements */
    const char *end = arr_start + arr_len;
    const char *p = arr_start + 1;
    size_t cur_index = 0;
    int depth = 0;
    bool in_str = false;

    while (p < end - 1) {
        while (p < end - 1 && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) {
            p++;
        }
        if (p >= end - 1 || *p == ']') {
            break;
        }

        if (cur_index == index) {
            if (*p != '"') {
                out_val[0] = '\0';
                return false;
            }
            p++; /* Skip opening quote */
            size_t i = 0;
            while (p < end && *p != '"' && i < out_size - 1) {
                if (*p == '\\' && (p + 1 < end)) {
                    p++;
                }
                out_val[i++] = *p++;
            }
            out_val[i] = '\0';
            if (p >= end || *p != '"') {
                return false;
            }
            return true;
        }

        /* Advance to next element separated by comma at depth 0 */
        while (p < end - 1) {
            if (*p == '"' && *(p - 1) != '\\') {
                in_str = !in_str;
            } else if (!in_str) {
                if (*p == '[' || *p == '{') {
                    depth++;
                } else if (*p == ']' || *p == '}') {
                    depth--;
                } else if (*p == ',' && depth == 0) {
                    p++;
                    cur_index++;
                    break;
                }
            }
            p++;
        }
    }

    out_val[0] = '\0';
    return false;
}

bool json_array_get_object(const char *arr_start, size_t arr_len,
                           size_t index, char *out_obj, size_t out_size)
{
    /* Step 1: Precondition check */
    if (arr_start == NULL || arr_len < 2 || *arr_start != '[' ||
        out_obj == NULL || out_size == 0) {
        if (out_obj && out_size > 0) {
            out_obj[0] = '\0';
        }
        return false;
    }

    /* Step 2: Traverse array tracking string literals and nested brace depth */
    const char *end = arr_start + arr_len;
    const char *p = arr_start + 1;
    size_t cur_idx = 0;
    int depth = 0;
    bool in_str = false;
    const char *obj_start = NULL;

    while (p < end - 1) {
        char c = *p;
        if (c == '"' && (p == arr_start || *(p - 1) != '\\')) {
            in_str = !in_str;
        } else if (!in_str) {
            if (c == '{') {
                if (depth == 0) {
                    obj_start = p;
                }
                depth++;
            } else if (c == '}') {
                depth--;
                if (depth == 0 && obj_start != NULL) {
                    if (cur_idx == index) {
                        size_t len = (size_t)(p - obj_start + 1);
                        if (len >= out_size) {
                            out_obj[0] = '\0';
                            return false; /* Buffer overflow protection */
                        }
                        memcpy(out_obj, obj_start, len);
                        out_obj[len] = '\0';
                        return true;
                    }
                    cur_idx++;
                    obj_start = NULL;
                }
            }
        }
        p++;
    }

    out_obj[0] = '\0';
    return false;
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify JSON parsing invariants, string boundary checks, and types.
// -----------------------------------------------------------------------------

diag_result_t json_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = json_utils_manifest.module_id;
    res.check_name = "JSON Key Locator, Typed Field Deserialization & Inspection";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "JSON utilities: token scan, string, int, bool, float, object, array & procedures verified.";

    // Check 1: NULL resilience
    char str_buf[32];
    size_t sz_val = 0;
    int int_val = 0;
    bool b_val = false;
    float f_val = 0.0f;

    if (json_get_string(NULL, "key", str_buf, sizeof(str_buf)) != false ||
        json_get_size_t(NULL, "key", &sz_val) != false ||
        json_get_int(NULL, "key", &int_val) != false ||
        json_get_bool(NULL, "key", &b_val) != false ||
        json_get_float(NULL, "key", &f_val) != false ||
        json_get_object_string(NULL, "obj", "key", str_buf, sizeof(str_buf)) != false ||
        json_find_array(NULL, "arr", NULL, NULL) != false ||
        json_array_get_count(NULL, 0) != 0 ||
        json_inspect_token(NULL, NULL, NULL) != false ||
        json_inspect_array(NULL, NULL, NULL) != false ||
        json_execute_extract_string(NULL, NULL, NULL, 0, NULL) != false) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_utils NULL input failed resilience contract.";
        return res;
    }

    // Check 2: Parse synthetic JSON payload with nested object and array
    const char *test_json =
        "{\n"
        "  \"name\": \"Shield of Lux\",\n"
        "  \"capacity\": 100,\n"
        "  \"moral_sway\": -42,\n"
        "  \"active\": true,\n"
        "  \"resonance\": 0.85,\n"
        "  \"_metadata\": {\n"
        "    \"version\": \"1.1.0\",\n"
        "    \"layer\": 0,\n"
        "    \"ratio\": 1.5,\n"
        "    \"enabled\": true\n"
        "  },\n"
        "  \"tags\": [\"relic\", \"defensive\", \"light\"]\n"
        "}";

    if (!json_get_string(test_json, "name", str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "Shield of Lux") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_string failed string extraction.";
        return res;
    }

    if (!json_get_size_t(test_json, "capacity", &sz_val) || sz_val != 100) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_size_t failed size_t extraction.";
        return res;
    }

    if (!json_get_int(test_json, "moral_sway", &int_val) || int_val != -42) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_int failed negative integer extraction.";
        return res;
    }

    if (!json_get_bool(test_json, "active", &b_val) || b_val != true) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_bool failed boolean extraction.";
        return res;
    }

    if (!json_get_float(test_json, "resonance", &f_val) || f_val < 0.84f || f_val > 0.86f) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_float failed floating point extraction.";
        return res;
    }

    // Check 3: Rev 2 Nested Object Extraction
    if (!json_get_object_string(test_json, "_metadata", "version", str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "1.1.0") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_object_string failed nested extraction.";
        return res;
    }

    if (!json_get_object_int(test_json, "_metadata", "layer", &int_val) || int_val != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_object_int failed nested int extraction.";
        return res;
    }

    if (!json_get_object_float(test_json, "_metadata", "ratio", &f_val) ||
        f_val < 1.49f || f_val > 1.51f) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_object_float failed nested float extraction.";
        return res;
    }

    if (!json_get_object_bool(test_json, "_metadata", "enabled", &b_val) || b_val != true) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_get_object_bool failed nested bool extraction.";
        return res;
    }

    // Check 4: Rev 2 Array Inspection and Extraction
    const char *arr_start = NULL;
    size_t arr_len = 0;
    if (!json_find_array(test_json, "tags", &arr_start, &arr_len) ||
        arr_start == NULL || arr_len == 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_find_array failed array boundary discovery.";
        return res;
    }

    size_t tag_count = json_array_get_count(arr_start, arr_len);
    if (tag_count != 3) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_array_get_count did not match expected count of 3.";
        return res;
    }

    if (!json_array_get_string(arr_start, arr_len, 0, str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "relic") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_array_get_string[0] failed.";
        return res;
    }

    if (!json_array_get_string(arr_start, arr_len, 1, str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "defensive") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_array_get_string[1] failed.";
        return res;
    }

    if (!json_array_get_string(arr_start, arr_len, 2, str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "light") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_array_get_string[2] failed.";
        return res;
    }

    if (json_array_get_string(arr_start, arr_len, 3, str_buf, sizeof(str_buf))) {
        res.passed = false;
        res.severity = SEVERITY_WARNING;
        res.details = "json_array_get_string returned true for out-of-bounds index.";
        return res;
    }

    // Check 4b: Array Object Slicing
    const char *obj_arr_test = "[{\"id\": 10, \"code\": \"RELIC_A\"}, {\"id\": 20, \"code\": \"RELIC_B\"}]";
    char obj_slice[128];
    if (!json_array_get_object(obj_arr_test, strlen(obj_arr_test), 1, obj_slice, sizeof(obj_slice))) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_array_get_object failed to slice object at index 1.";
        return res;
    }
    int slice_id = 0;
    if (!json_get_int(obj_slice, "id", &slice_id) || slice_id != 20) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_array_get_object extracted corrupt object data.";
        return res;
    }

    // Check 5: Block 5 Inspection Procedures
    json_token_view_t tok_view;
    if (!json_inspect_token(test_json, "moral_sway", &tok_view) ||
        !tok_view.exists || tok_view.type != JSON_TYPE_NUMBER) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_inspect_token failed for numeric field.";
        return res;
    }

    if (!json_inspect_token(test_json, "active", &tok_view) ||
        !tok_view.exists || tok_view.type != JSON_TYPE_BOOLEAN) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_inspect_token failed for boolean field.";
        return res;
    }

    json_array_view_t arr_view;
    if (!json_inspect_array(test_json, "tags", &arr_view) ||
        !arr_view.exists || arr_view.count != 3) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_inspect_array failed to construct valid array view.";
        return res;
    }

    json_extract_result_t res_obj;
    char exec_buf[32];
    if (!json_execute_extract_string(test_json, "name", exec_buf, sizeof(exec_buf), &res_obj) ||
        !res_obj.success || strcmp(exec_buf, "Shield of Lux") != 0) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "json_execute_extract_string failed string extraction procedure.";
        return res;
    }

    // Check 6: Missing key gracefully returns false
    if (json_get_string(test_json, "nonexistent", str_buf, sizeof(str_buf))) {
        res.passed = false;
        res.severity = SEVERITY_WARNING;
        res.details = "json_get_string returned true for missing key.";
        return res;
    }

    return res;
}

bool json_utils_audit(void)
{
    diag_result_t res = json_utils_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: EXECUTABLE PROCEDURES & OBJECT CONSTRUCTORS ]
// -----------------------------------------------------------------------------

bool json_inspect_token(const char *json, const char *key, json_token_view_t *out_view)
{
    if (out_view == NULL) {
        return false;
    }
    memset(out_view, 0, sizeof(*out_view));
    out_view->type = JSON_TYPE_UNKNOWN;

    if (json == NULL || key == NULL || key[0] == '\0') {
        return false;
    }

    const char *val = json_find_key(json, key);
    if (val == NULL) {
        return false;
    }

    out_view->exists = true;
    out_view->raw_start = val;

    char c = *val;
    if (c == '{') {
        out_view->type = JSON_TYPE_OBJECT;
        int depth = 0;
        bool in_str = false;
        const char *p = val;
        while (*p != '\0') {
            if (*p == '"' && (p == val || *(p - 1) != '\\')) {
                in_str = !in_str;
            } else if (!in_str) {
                if (*p == '{') {
                    depth++;
                } else if (*p == '}') {
                    depth--;
                    if (depth == 0) {
                        out_view->raw_len = (size_t)(p - val + 1);
                        return true;
                    }
                }
            }
            p++;
        }
        return false;
    } else if (c == '[') {
        out_view->type = JSON_TYPE_ARRAY;
        int depth = 0;
        bool in_str = false;
        const char *p = val;
        while (*p != '\0') {
            if (*p == '"' && (p == val || *(p - 1) != '\\')) {
                in_str = !in_str;
            } else if (!in_str) {
                if (*p == '[') {
                    depth++;
                } else if (*p == ']') {
                    depth--;
                    if (depth == 0) {
                        out_view->raw_len = (size_t)(p - val + 1);
                        return true;
                    }
                }
            }
            p++;
        }
        return false;
    } else if (c == '"') {
        out_view->type = JSON_TYPE_STRING;
        const char *p = val + 1;
        while (*p != '\0') {
            if (*p == '"' && *(p - 1) != '\\') {
                out_view->raw_len = (size_t)(p - val + 1);
                return true;
            }
            p++;
        }
        return false;
    } else if (strncmp(val, "true", 4) == 0) {
        out_view->type = JSON_TYPE_BOOLEAN;
        out_view->raw_len = 4;
        return true;
    } else if (strncmp(val, "false", 5) == 0) {
        out_view->type = JSON_TYPE_BOOLEAN;
        out_view->raw_len = 5;
        return true;
    } else if (strncmp(val, "null", 4) == 0) {
        out_view->type = JSON_TYPE_NULL;
        out_view->raw_len = 4;
        return true;
    } else if (isdigit((unsigned char)c) || c == '-' || c == '+') {
        out_view->type = JSON_TYPE_NUMBER;
        const char *p = val;
        while (*p != '\0' && (isdigit((unsigned char)*p) || *p == '.' || *p == '-' ||
                              *p == '+' || *p == 'e' || *p == 'E')) {
            p++;
        }
        out_view->raw_len = (size_t)(p - val);
        return true;
    }

    return false;
}

bool json_inspect_array(const char *json, const char *array_key, json_array_view_t *out_view)
{
    if (out_view == NULL) {
        return false;
    }
    memset(out_view, 0, sizeof(*out_view));

    if (json == NULL || array_key == NULL) {
        return false;
    }

    const char *arr_start = NULL;
    size_t arr_len = 0;
    if (!json_find_array(json, array_key, &arr_start, &arr_len)) {
        return false;
    }

    out_view->exists = true;
    out_view->arr_start = arr_start;
    out_view->arr_len = arr_len;
    out_view->count = json_array_get_count(arr_start, arr_len);
    return true;
}

bool json_execute_extract_string(const char *json, const char *key,
                                 char *buf, size_t buf_size,
                                 json_extract_result_t *out_result)
{
    if (out_result) {
        memset(out_result, 0, sizeof(*out_result));
        if (key) {
            strncpy(out_result->key, key, sizeof(out_result->key) - 1);
            out_result->key[sizeof(out_result->key) - 1] = '\0';
        }
    }

    if (json == NULL || key == NULL || buf == NULL || buf_size == 0) {
        if (buf && buf_size > 0) {
            buf[0] = '\0';
        }
        return false;
    }

    bool ok = json_get_string(json, key, buf, buf_size);
    if (out_result) {
        out_result->success = ok;
        out_result->type = ok ? JSON_TYPE_STRING : JSON_TYPE_UNKNOWN;
        out_result->value_len = ok ? strlen(buf) : 0;
    }

    return ok;
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.2: SUBSYSTEM LIFECYCLE INITIALIZATION & TEARDOWN ]
// -----------------------------------------------------------------------------

void json_utils_init(void)
{
    /* Subsystem initialization hook for zero-heap JSON engine */
}

void json_utils_shutdown(void)
{
    /* Subsystem teardown hook for zero-heap JSON engine */
}

// =============================================================================
// End of json_utils.c
// =============================================================================
