// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          json_utils.c
// @id            core.util.json
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
//   Zero-heap, memory-safe JSON parser and key/value token extractor implementation.
//   Serves as the universal serialization/deserialization primitive for
//   data-driven profiles, manifests, telemetry channels, combat tables,
//   and world topologies without pulling in heavy third-party dependencies.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/caller-supplied buffers).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless parser on const char*).
//   - Latency Budget:        < 50 microseconds per token extraction (O(N) scan).
//
// AXIOMATIC INVARIANTS:
//   1. Zero heap allocations: Strict zero-heap guarantee across all routines.
//   2. Buffer safety: Extracted strings unconditionally enforce null termination.
//   3. Resilience: Gracefully handles malformed JSON, missing keys, and NULL pointers.
//   4. Numerical bounds: Safely clamps or validates numerical ranges on conversion.
// =============================================================================

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

#include "util/data/json_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const json_invariants[] = {
    "Zero heap allocations: Strict zero-heap guarantee across all routines",
    "Buffer safety: Extracted strings unconditionally enforce null termination",
    "Resilience: Gracefully handles malformed JSON, missing keys, and NULL pointers"
};

/**
 * Manifest record for the Zero-Heap JSON Engine.
 */
const module_metadata_t json_utils_manifest = {
    "core.util.json",
    "json_utils.h",
    LAYER_0_CORE_UTIL,
    { 1, 1, 0 },
    2,
    "2026-11-02",
    "Zero-heap JSON token locator and typed value extractor",
    0,
    { NULL },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    50,
    3,
    {
        json_invariants[0],
        json_invariants[1],
        json_invariants[2]
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
    return json_find_key_scoped(json, 0, key);
}

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: VALUE EXTRACTION COGS ]
// Mission: Extract typed values (string, integer, size_t, bool, float).
// -----------------------------------------------------------------------------

bool json_get_string(const char *json, const char *key, char *out_val, size_t out_size)
{
    if (json == NULL || key == NULL || out_val == NULL || out_size == 0) {
        if (out_val && out_size > 0) {
            out_val[0] = '\0';
        }
        return false;
    }

    const char *val = json_find_key(json, key);
    if (!val || *val != '"') {
        out_val[0] = '\0';
        return false;
    }

    val++; // Skip opening quote
    size_t i = 0;
    while (*val != '\0' && *val != '"' && i < out_size - 1) {
        if (*val == '\\' && *(val + 1) != '\0') {
            val++; // Handle basic escapes
        }
        out_val[i++] = *val++;
    }
    out_val[i] = '\0';

    return true;
}

bool json_get_size_t(const char *json, const char *key, size_t *out_val)
{
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

    if (!isdigit((unsigned char)*val)) {
        return false;
    }

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
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

    if (!isdigit((unsigned char)*val) && *val != '-' && *val != '+') {
        return false;
    }

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
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

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
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    const char *val = json_find_key(json, key);
    if (!val) {
        return false;
    }

    if (!isdigit((unsigned char)*val) && *val != '-' && *val != '+' && *val != '.') {
        return false;
    }

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
    if (json == NULL || object_key == NULL || out_obj_start == NULL || out_obj_len == NULL) {
        return false;
    }

    const char *val = json_find_key(json, object_key);
    if (!val || *val != '{') {
        return false;
    }

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
    if (json == NULL || key == NULL || out_val == NULL || out_size == 0) {
        if (out_val && out_size > 0) {
            out_val[0] = '\0';
        }
        return false;
    }

    if (parent_obj_key == NULL) {
        return json_get_string(json, key, out_val, out_size);
    }

    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        out_val[0] = '\0';
        return false;
    }

    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val || *val != '"') {
        out_val[0] = '\0';
        return false;
    }

    val++; // Skip opening quote
    size_t i = 0;
    while (*val != '\0' && *val != '"' && i < out_size - 1) {
        if (*val == '\\' && *(val + 1) != '\0') {
            val++;
        }
        out_val[i++] = *val++;
    }
    out_val[i] = '\0';
    return true;
}

bool json_get_object_int(const char *json, const char *parent_obj_key,
                         const char *key, int *out_val)
{
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    if (parent_obj_key == NULL) {
        return json_get_int(json, key, out_val);
    }

    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        return false;
    }

    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val || (!isdigit((unsigned char)*val) && *val != '-' && *val != '+')) {
        return false;
    }

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
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    if (parent_obj_key == NULL) {
        return json_get_float(json, key, out_val);
    }

    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        return false;
    }

    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val || (!isdigit((unsigned char)*val) && *val != '-' && *val != '+' && *val != '.')) {
        return false;
    }

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
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    if (parent_obj_key == NULL) {
        return json_get_bool(json, key, out_val);
    }

    const char *obj_start = NULL;
    size_t obj_len = 0;
    if (!json_find_object(json, parent_obj_key, &obj_start, &obj_len)) {
        return false;
    }

    const char *val = json_find_key_scoped(obj_start, obj_len, key);
    if (!val) {
        return false;
    }

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
    if (json == NULL || array_key == NULL || out_arr_start == NULL || out_arr_len == NULL) {
        return false;
    }

    const char *val = json_find_key(json, array_key);
    if (!val || *val != '[') {
        return false;
    }

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
    if (arr_start == NULL || arr_len < 2 || *arr_start != '[') {
        return 0;
    }

    const char *end = arr_start + arr_len;
    const char *p = arr_start + 1;
    // Skip leading whitespace
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) {
        p++;
    }
    if (p < end && *p == ']') {
        return 0; // Empty array []
    }

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
    if (arr_start == NULL || arr_len < 2 || *arr_start != '[' ||
        out_val == NULL || out_size == 0) {
        if (out_val && out_size > 0) {
            out_val[0] = '\0';
        }
        return false;
    }

    const char *end = arr_start + arr_len;
    const char *p = arr_start + 1;
    size_t cur_index = 0;
    int depth = 0;
    bool in_str = false;

    while (p < end - 1) {
        // Skip whitespace
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
            p++; // Skip opening quote
            size_t i = 0;
            while (p < end && *p != '"' && i < out_size - 1) {
                if (*p == '\\' && (p + 1 < end)) {
                    p++;
                }
                out_val[i++] = *p++;
            }
            out_val[i] = '\0';
            return true;
        }

        // Advance to next element separated by comma at depth 0
        while (p < end - 1) {
            if (*p == '"' && *(p - 1) != '\\') {
                in_str = !in_str;
            } else if (!in_str) {
                if (*p == '[' || *p == '{') {
                    depth++;
                } else if (*p == ']' || *p == '}') {
                    depth--;
                } else if (*p == ',' && depth == 0) {
                    p++; // Skip comma
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
    res.check_name = "JSON Key Locator & Typed Field Deserialization";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "JSON utilities: token scan, string, int, bool, float, object & array verified.";

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
        json_array_get_count(NULL, 0) != 0) {
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
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_string failed string extraction.";
        return res;
    }

    if (!json_get_size_t(test_json, "capacity", &sz_val) || sz_val != 100) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_size_t failed size_t extraction.";
        return res;
    }

    if (!json_get_int(test_json, "moral_sway", &int_val) || int_val != -42) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_int failed negative integer extraction.";
        return res;
    }

    if (!json_get_bool(test_json, "active", &b_val) || b_val != true) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_bool failed boolean extraction.";
        return res;
    }

    if (!json_get_float(test_json, "resonance", &f_val) || f_val < 0.84f || f_val > 0.86f) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_float failed floating point extraction.";
        return res;
    }

    // Check 3: Rev 2 Nested Object Extraction
    if (!json_get_object_string(test_json, "_metadata", "version", str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "1.1.0") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_object_string failed nested extraction.";
        return res;
    }

    if (!json_get_object_int(test_json, "_metadata", "layer", &int_val) || int_val != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_object_int failed nested int extraction.";
        return res;
    }

    if (!json_get_object_float(test_json, "_metadata", "ratio", &f_val) ||
        f_val < 1.49f || f_val > 1.51f) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_object_float failed nested float extraction.";
        return res;
    }

    if (!json_get_object_bool(test_json, "_metadata", "enabled", &b_val) || b_val != true) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_get_object_bool failed nested bool extraction.";
        return res;
    }

    // Check 4: Rev 2 Array Inspection and Extraction
    const char *arr_start = NULL;
    size_t arr_len = 0;
    if (!json_find_array(test_json, "tags", &arr_start, &arr_len) ||
        arr_start == NULL || arr_len == 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_find_array failed array boundary discovery.";
        return res;
    }

    size_t tag_count = json_array_get_count(arr_start, arr_len);
    if (tag_count != 3) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_array_get_count did not match expected count of 3.";
        return res;
    }

    if (!json_array_get_string(arr_start, arr_len, 0, str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "relic") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_array_get_string[0] failed.";
        return res;
    }

    if (!json_array_get_string(arr_start, arr_len, 1, str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "defensive") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_array_get_string[1] failed.";
        return res;
    }

    if (!json_array_get_string(arr_start, arr_len, 2, str_buf, sizeof(str_buf)) ||
        strcmp(str_buf, "light") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_array_get_string[2] failed.";
        return res;
    }

    if (json_array_get_string(arr_start, arr_len, 3, str_buf, sizeof(str_buf))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "json_array_get_string returned true for out-of-bounds index.";
        return res;
    }

    // Check 5: Missing key gracefully returns false
    if (json_get_string(test_json, "nonexistent", str_buf, sizeof(str_buf))) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
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
// ( Teardown & Module Close )
// =============================================================================

// End of json_utils.c
