/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          json_utils.h
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
  Zero-heap, memory-safe JSON parser and key/value token extractor.
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

#ifndef TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H
#define TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Canonical Token Limits
--------------------------------------------------------------------------------
*/

#define JSON_MAX_KEY_LEN     64   /**< Maximum key length in bytes. */
#define JSON_MAX_TOKEN_LEN   128  /**< Maximum token preview string length. */
#define JSON_MAX_NESTING     16   /**< Maximum supported bracket nesting depth. */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
*/

/**
 * @enum json_value_type
 * @brief Ontological classification of JSON value types.
 */
typedef enum json_value_type {
    JSON_TYPE_UNKNOWN = 0,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_BOOLEAN,
    JSON_TYPE_NULL
} json_value_type_t;

/**
 * @struct json_token_view
 * @brief Detailed token view constructed by json_inspect_token.
 */
typedef struct json_token_view {
    json_value_type_t type;      /**< Inferred value type. */
    const char *raw_start;       /**< Pointer to start of value payload. */
    size_t raw_len;              /**< Length in bytes of the value slice. */
    bool exists;                 /**< true if key was located in buffer. */
} json_token_view_t;

/**
 * @struct json_array_view
 * @brief Structured array view constructed by json_inspect_array.
 */
typedef struct json_array_view {
    const char *arr_start;       /**< Pointer to opening '['. */
    size_t arr_len;              /**< Total array slice length in bytes. */
    size_t count;                /**< Number of top-level elements. */
    bool exists;                 /**< true if array was located. */
} json_array_view_t;

/**
 * @struct json_extract_result
 * @brief High-level extraction result object constructed by execution procedures.
 */
typedef struct json_extract_result {
    bool success;                /**< true if field was successfully extracted. */
    json_value_type_t type;      /**< Extracted token type. */
    char key[JSON_MAX_KEY_LEN];  /**< Target key name. */
    size_t value_len;            /**< Byte length of extracted value. */
} json_extract_result_t;

/**
 * Manifest record for the Zero-Heap JSON Engine.
 */
extern const module_metadata_t json_utils_manifest;

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap JSON Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: JSON Token Locator Cogs
Mission: Scan JSON payloads for matching keys while respecting quotes/whitespace.
--------------------------------------------------------------------------------
*/

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

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Value Extraction Cogs
Mission: Extract typed values (string, integer, size_t, bool, float).
--------------------------------------------------------------------------------
*/

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

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Array Inspection & Extraction Cogs
Mission: Scan JSON array boundaries and extract elements without heap usage.
--------------------------------------------------------------------------------
*/

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

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap JSON Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Self-Audit & Pre-Flight Tripwires
Mission: Verify JSON parsing invariants, string boundary checks, and types.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on JSON parser invariants.
 *
 * Verifies extraction of strings, integers, negatives, booleans, floats,
 * array slicing, and bounds-checking resilience against truncated inputs.
 *
 * @return diag_result_t reporting categorized severity.
 */
diag_result_t json_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return true if all JSON invariant tripwires pass cleanly; false otherwise.
 */
bool json_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap JSON Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures & Object Constructors
--------------------------------------------------------------------------------
*/

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

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the zero-heap JSON parsing subsystem.
 */
void json_utils_init(void);

/**
 * @brief Tears down the JSON parsing subsystem.
 */
void json_utils_shutdown(void);

#endif /* TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H */
