// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          json_utils.h
// @id            core.util.json
// @layer         Layer 0: Core Utilities
// @subsystem     Data & Filesystem Subsystem (util/io)
// @version       0.1.0
// @interface_rev Rev 3
// @date          2026-11-05
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Zero-Heap JSON Tokenizer & Structured Field Deserializer
// @role          Fast zero-heap JSON scanning, token classification, bounded
//                field extraction, array slicing, and procedure execution for
//                data-driven engine assets.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Zero-heap, memory-safe JSON parser and key/value token extractor.
//   Serves as the universal serialization/deserialization primitive for
//   data-driven profiles, manifests, telemetry channels, combat tables,
//   and world topologies without pulling in heavy third-party dependencies.
//   Guarantees non-allocating, deterministic parsing into caller stack buffers.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module in Layer 0).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (100% stack/caller-supplied buffers).
//   - Concurrency / Safety:  REENTRANT_PURE (Stateless parser on const char*).
//   - Latency Budget:        < 50 microseconds per token extraction (O(N) scan).
//
// AXIOMATIC INVARIANTS:
//   1. Zero dynamic heap allocations: malloc, calloc, realloc, free prohibited.
//   2. Buffer safety: Extracted strings unconditionally enforce null termination.
//   3. Fault resilience: Gracefully handles malformed JSON, missing keys, and NULL.
//   4. Numerical bounds: Safely clamps or validates numerical ranges on conversion.
//   5. Standardized token limits: Predictable bounds for keys, tokens, and nesting.
//   6. Universal 5-Block Lifecycle Protocol enforced across header and source.
// =============================================================================

#ifndef TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H
#define TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

#include "core/manifest.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.1: PRE-CONDITIONS & CANONICAL TOKEN LIMITS ]
// -----------------------------------------------------------------------------

#define JSON_MAX_KEY_LEN     64   /**< Maximum key length in bytes. */
#define JSON_MAX_TOKEN_LEN   128  /**< Maximum token preview string length. */
#define JSON_MAX_NESTING     16   /**< Maximum supported bracket nesting depth. */

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 2.2: STRUCTURAL ENTITY & OBJECT DEFINITIONS ]
// -----------------------------------------------------------------------------

/**
 * @brief Ontological classification of JSON value types.
 */
typedef enum {
    JSON_TYPE_UNKNOWN = 0,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NULL
} json_value_type_t;

/**
 * @brief Detailed token view constructed by json_inspect_token.
 */
typedef struct {
    json_value_type_t type;      /**< Inferred value type. */
    const char *raw_start;       /**< Pointer to start of value payload. */
    size_t raw_len;              /**< Length in bytes of the value slice. */
    bool exists;                 /**< true if key was located in buffer. */
} json_token_view_t;

/**
 * @brief Structured array view constructed by json_inspect_array.
 */
typedef struct {
    const char *arr_start;       /**< Pointer to opening '['. */
    size_t arr_len;              /**< Total array slice length in bytes. */
    size_t count;                /**< Number of top-level elements. */
    bool exists;                 /**< true if array was located. */
} json_array_view_t;

/**
 * @brief High-level extraction result object constructed by execution procedures.
 */
typedef struct {
    bool success;                /**< true if field was successfully extracted. */
    json_value_type_t type;      /**< Extracted token type. */
    char key[JSON_MAX_KEY_LEN];  /**< Target key name. */
    size_t value_len;            /**< Byte length of extracted value. */
} json_extract_result_t;

/**
 * Manifest record for the Zero-Heap JSON Engine.
 */
extern const module_metadata_t json_utils_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.1: JSON TOKEN LOCATOR COGS ]
// Mission: Scan JSON payloads for matching keys while respecting quotes/whitespace.
// -----------------------------------------------------------------------------

/**
 * @brief Scans a JSON buffer for a specific top-level or scoped key token.
 *
 * Core search primitive that locates occurrences of "key" followed by a colon
 * (:), ignoring intervening whitespace. Prevents false positive matches on
 * substring prefixes or keys embedded inside quoted strings.
 *
 * @param[in] json Null-terminated JSON buffer.
 * @param[in] key  Null-terminated key name to search for (without quotes).
 *
 * @return Pointer to value position immediately following the colon (:),
 *         or NULL if key is not found or parameters are invalid.
 */
const char *json_find_key(const char *json, const char *key);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: VALUE EXTRACTION COGS ]
// Mission: Extract typed values (string, integer, size_t, bool, float).
// -----------------------------------------------------------------------------

/**
 * @brief Extracts a string literal value for a key into a caller-supplied buffer.
 *
 * Deserializes string properties (e.g. "version": "1.0.0", "name": "Lux").
 * Strips surrounding quotes and guarantees bounded null termination.
 *
 * @param[in]  json     Null-terminated JSON buffer.
 * @param[in]  key      Key name to search for.
 * @param[out] out_val  Destination buffer for extracted string.
 * @param[in]  out_size Capacity of destination buffer in bytes.
 *
 * @return true if key was found and value extracted; false otherwise.
 * @post out_val[out_size - 1] == '\0' is guaranteed if returning true.
 */
bool json_get_string(const char *json, const char *key, char *out_val,
                     size_t out_size);

/**
 * @brief Extracts an unsigned size_t integer value for a given key.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination size_t variable.
 *
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_size_t(const char *json, const char *key, size_t *out_val);

/**
 * @brief Extracts a signed 32-bit int value for a given key.
 *
 * Supports positive and negative integer representations.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination int variable.
 *
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_int(const char *json, const char *key, int *out_val);

/**
 * @brief Extracts a boolean value (true or false) for a given key.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination bool variable.
 *
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_bool(const char *json, const char *key, bool *out_val);

/**
 * @brief Extracts a floating-point float value for a given key.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination float variable.
 *
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_float(const char *json, const char *key, float *out_val);

/**
 * @brief Locates embedded sub-object boundaries ({ ... }) for a given key.
 *
 * @param[in]  json          Null-terminated JSON buffer.
 * @param[in]  object_key    Key name containing the object.
 * @param[out] out_obj_start Pointer to opening '{' of the sub-object.
 * @param[out] out_obj_len   Length in bytes of sub-object including '{' and '}'.
 *
 * @return true if sub-object was located and balanced; false otherwise.
 */
bool json_find_object(const char *json, const char *object_key,
                      const char **out_obj_start, size_t *out_obj_len);

/**
 * @brief Extracts a string value from within an embedded JSON object.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Key name of parent object (or NULL for top-level).
 * @param[in]  key            Key name to extract inside the object.
 * @param[out] out_val        Destination buffer for extracted string.
 * @param[in]  out_size       Capacity of destination buffer in bytes.
 *
 * @return true if parent object and key were found and parsed; false otherwise.
 */
bool json_get_object_string(const char *json, const char *parent_obj_key,
                            const char *key, char *out_val, size_t out_size);

/**
 * @brief Extracts a signed 32-bit int value from within an embedded JSON object.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Key name of parent object (or NULL for top-level).
 * @param[in]  key            Key name to extract inside the object.
 * @param[out] out_val        Pointer to destination int variable.
 *
 * @return true if parent object and key were found and parsed; false otherwise.
 */
bool json_get_object_int(const char *json, const char *parent_obj_key,
                         const char *key, int *out_val);

/**
 * @brief Extracts a float value from within an embedded JSON object.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Key name of parent object (or NULL for top-level).
 * @param[in]  key            Key name to extract inside the object.
 * @param[out] out_val        Pointer to destination float variable.
 *
 * @return true if parent object and key were found and parsed; false otherwise.
 */
bool json_get_object_float(const char *json, const char *parent_obj_key,
                           const char *key, float *out_val);

/**
 * @brief Extracts a boolean value from within an embedded JSON object.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Key name of parent object (or NULL for top-level).
 * @param[in]  key            Key name to extract inside the object.
 * @param[out] out_val        Pointer to destination bool variable.
 *
 * @return true if parent object and key were found and parsed; false otherwise.
 */
bool json_get_object_bool(const char *json, const char *parent_obj_key,
                          const char *key, bool *out_val);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: ARRAY INSPECTION & EXTRACTION COGS ]
// Mission: Scan JSON array boundaries and extract elements without heap usage.
// -----------------------------------------------------------------------------

/**
 * @brief Locates an embedded array [ ... ] for a given key.
 *
 * @param[in]  json          Null-terminated JSON buffer.
 * @param[in]  array_key     Key name containing the array.
 * @param[out] out_arr_start Pointer to the opening '[' of the array.
 * @param[out] out_arr_len   Length in bytes of array including '[' and ']'.
 *
 * @return true if array was located and brackets balanced; false otherwise.
 */
bool json_find_array(const char *json, const char *array_key,
                     const char **out_arr_start, size_t *out_arr_len);

/**
 * @brief Returns the number of comma-separated elements in a JSON array.
 *
 * @param[in] arr_start Pointer to opening '[' of array.
 * @param[in] arr_len   Length of array in bytes.
 *
 * @return Count of elements in the array.
 */
size_t json_array_get_count(const char *arr_start, size_t arr_len);

/**
 * @brief Extracts i-th string element from JSON array into caller buffer.
 *
 * @param[in]  arr_start Pointer to opening '[' of array.
 * @param[in]  arr_len   Length of array in bytes.
 * @param[in]  index     0-based element index.
 * @param[out] out_val   Destination buffer for extracted string.
 * @param[in]  out_size  Capacity of destination buffer in bytes.
 *
 * @return true if element exists and was extracted; false otherwise.
 */
bool json_array_get_string(const char *arr_start, size_t arr_len,
                           size_t index, char *out_val, size_t out_size);

/**
 * @brief Extracts i-th object slice { ... } from JSON array into caller buffer.
 *
 * Scans nested braces and quotes without dynamic heap allocations, returning
 * the complete bounded object string including opening and closing braces.
 *
 * @param[in]  arr_start Pointer to opening '[' of array.
 * @param[in]  arr_len   Length of array in bytes.
 * @param[in]  index     0-based element index.
 * @param[out] out_obj   Destination buffer for extracted object string.
 * @param[in]  out_size  Capacity of destination buffer in bytes.
 *
 * @return true if object exists and was extracted within buffer bounds; false otherwise.
 */
bool json_array_get_object(const char *arr_start, size_t arr_len,
                           size_t index, char *out_obj, size_t out_size);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify JSON parsing invariants, string boundary checks, and types.
// -----------------------------------------------------------------------------

/**
 * @brief Executes machine pre-flight diagnostic checks on JSON parser invariants.
 *
 * Verifies extraction of strings, integers, negatives, booleans, floats,
 * array slicing, and bounds-checking resilience against truncated inputs.
 *
 * @return diag_result_t reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t json_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all JSON invariant tripwires pass cleanly; false otherwise.
 */
bool json_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.1: EXECUTABLE PROCEDURES & OBJECT CONSTRUCTORS ]
// -----------------------------------------------------------------------------

/**
 * @brief Inspects a key token, classifies its value type, and constructs a token view.
 *
 * @param[in]  json     Null-terminated JSON buffer.
 * @param[in]  key      Key name to inspect.
 * @param[out] out_view Constructed token view object.
 *
 * @return true if key was located and view constructed; false otherwise.
 */
bool json_inspect_token(const char *json, const char *key, json_token_view_t *out_view);

/**
 * @brief Inspects an array, measures slice length, counts elements, and constructs view.
 *
 * @param[in]  json      Null-terminated JSON buffer.
 * @param[in]  array_key Array property key name.
 * @param[out] out_view  Constructed array view object.
 *
 * @return true if array was located and view constructed; false otherwise.
 */
bool json_inspect_array(const char *json, const char *array_key, json_array_view_t *out_view);

/**
 * @brief High-level execution procedure that extracts a string and constructs a result object.
 *
 * @param[in]  json       Null-terminated JSON buffer.
 * @param[in]  key        Key name to extract.
 * @param[out] buf        Caller destination buffer for extracted string.
 * @param[in]  buf_size   Capacity of destination buffer.
 * @param[out] out_result Detailed extraction result object.
 *
 * @return true if string was extracted within buffer limits; false otherwise.
 */
bool json_execute_extract_string(const char *json, const char *key,
                                 char *buf, size_t buf_size,
                                 json_extract_result_t *out_result);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 5.2: SUBSYSTEM LIFECYCLE INITIALIZATION & TEARDOWN ]
// -----------------------------------------------------------------------------

/**
 * @brief Initializes the zero-heap JSON parsing subsystem.
 */
void json_utils_init(void);

/**
 * @brief Tears down the JSON parsing subsystem.
 */
void json_utils_shutdown(void);

#endif /* TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H */
