/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          json_utils.h
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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

#ifndef TENEBRIS_LUX_IO_JSON_UTILS_H
#define TENEBRIS_LUX_IO_JSON_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap JSON Engine
================================================================================
Mission: Parser limits, include hierarchy, manifest anchors, token ontology,
and pre-conditional static validation inlines for zero-heap JSON extraction.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
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
Compile-time sizing limits for JSON key lookup, token previews, and recursion.
For Designers: Bounds ensure configuration files parse quickly without memory
exhaustion, supporting keys up to 64 letters and nesting up to 16 levels deep.
For Programmers: Guarantees 100% zero-heap allocation (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/** Maximum key length in bytes supported by token extractors. */
#define JSON_MAX_KEY_LEN     64

/** Maximum token preview string length in bytes. */
#define JSON_MAX_TOKEN_LEN   128

/** Maximum supported bracket nesting depth (objects and arrays). */
#define JSON_MAX_NESTING     16

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Zero-Heap JSON parsing engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Zero-Heap JSON Engine. */
#define JSON_UTILS_MODULE_ID           "io.json_utils"

/**
 * Manifest record for the Zero-Heap JSON Engine.
 */
extern const module_metadata_t json_utils_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for token classification, array slices, and query results.
--------------------------------------------------------------------------------
*/

/**
 * @enum json_value_type
 * @brief Ontological classification of JSON value types.
 *
 * For Designers: Identifies whether a value in a JSON data file is text, a number,
 * a true/false switch, a list of items, or a grouped object.
 *
 * For Programmers: Tagged enumeration used for fast DWARF debugging and type dispatch.
 */
typedef enum json_value_type {
    JSON_TYPE_UNKNOWN = 0, /**< Unparseable or invalid token. */
    JSON_TYPE_OBJECT,      /**< Nested JSON dictionary structure ({...}). */
    JSON_TYPE_ARRAY,       /**< Ordered JSON list ([...]). */
    JSON_TYPE_STRING,      /**< Quoted string literal. */
    JSON_TYPE_NUMBER,      /**< Numeric integer or floating-point literal. */
    JSON_TYPE_BOOLEAN,     /**< Boolean true or false keyword. */
    JSON_TYPE_NULL         /**< Null literal keyword. */
} json_value_type_t;

/**
 * @struct json_token_view
 * @brief Detailed token view constructed by json_inspect_token.
 *
 * For Designers: A lens showing where a specific property lives inside the file.
 *
 * For Programmers: Non-owning slice pointing directly into the source buffer.
 */
typedef struct json_token_view {
    json_value_type_t type;      /**< Inferred value type. */
    const char       *raw_start;  /**< Pointer to start of value payload. */
    size_t            raw_len;    /**< Length in bytes of the value slice. */
    bool              exists;     /**< true if key was located in buffer. */
} json_token_view_t;

/**
 * @struct json_array_view
 * @brief Structured array view constructed by json_inspect_array.
 *
 * For Designers: A view over a list in a configuration file (like a list of sounds).
 *
 * For Programmers: Non-owning slice over an array token, with pre-counted elements.
 */
typedef struct json_array_view {
    const char *arr_start;       /**< Pointer to opening '['. */
    size_t      arr_len;         /**< Total array slice length in bytes. */
    size_t      count;           /**< Number of top-level elements. */
    bool        exists;          /**< true if array was located. */
} json_array_view_t;

/**
 * @struct json_extract_result
 * @brief High-level extraction result object constructed by execution procedures.
 *
 * For Designers: A status receipt confirming whether a value was read successfully.
 *
 * For Programmers: Fixed-size stack record reporting extraction success and metadata.
 */
typedef struct json_extract_result {
    bool              success;                /**< true if field was successfully extracted. */
    json_value_type_t type;                   /**< Extracted token type. */
    char              key[JSON_MAX_KEY_LEN];  /**< Target key name. */
    size_t            value_len;              /**< Byte length of extracted value. */
} json_extract_result_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an inferred JSON value type is recognized.
 *
 * @param[in] type Enum value to validate.
 * @return true if type is within [JSON_TYPE_OBJECT .. JSON_TYPE_NULL].
 */
static inline bool json_type_is_valid(json_value_type_t type) {
    return type >= JSON_TYPE_OBJECT && type <= JSON_TYPE_NULL;
}

/**
 * @brief Predicate verifying if an inspected token view found a valid token.
 *
 * @param[in] view Pointer to token view record.
 * @return true if view is non-NULL, exists is true, and raw_start is non-NULL.
 */
static inline bool json_token_exists(const json_token_view_t *view) {
    return (view != NULL) && view->exists && (view->raw_start != NULL);
}

/**
 * @brief Predicate verifying if an inspected array view found a valid array.
 *
 * @param[in] view Pointer to array view record.
 * @return true if view is non-NULL, exists is true, and arr_start is non-NULL.
 */
static inline bool json_array_exists(const json_array_view_t *view) {
    return (view != NULL) && view->exists && (view->arr_start != NULL);
}

/**
 * @brief Predicate verifying if a JSON key length is safely within limits.
 *
 * @param[in] len Length of key string in bytes.
 * @return true if length is greater than 0 and strictly less than JSON_MAX_KEY_LEN.
 */
static inline bool json_key_len_is_valid(size_t len) {
    return len > 0 && len < JSON_MAX_KEY_LEN;
}

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
 * ### For Game Designers & Non-Programmers:
 * Searches through a data file to locate where a property begins (e.g. finding "attack_power:").
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant zero-heap token locator respecting quote escapes and whitespace.
 *
 * @param[in] json Null-terminated JSON buffer.
 * @param[in] key  Null-terminated key name to search for (without quotes).
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
 * ### For Game Designers & Non-Programmers:
 * Copies a piece of text (like a hero name or ability description) out of a JSON file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap extraction unescaping quotes with unconditional null termination at out_val[out_size - 1].
 *
 * @param[in]  json     Null-terminated JSON buffer.
 * @param[in]  key      Key name to search for.
 * @param[out] out_val  Destination buffer for extracted string.
 * @param[in]  out_size Capacity of destination buffer in bytes.
 * @return true if key was found and value extracted; false otherwise.
 * @post out_val[out_size - 1] == '\0' is guaranteed if returning true.
 */
bool json_get_string(const char *json, const char *key, char *out_val,
                     size_t out_size);

/**
 * @brief Extracts an unsigned size_t integer value for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a positive count or byte size setting from a data file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses unsigned integer token into destination size_t variable.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination size_t variable.
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_size_t(const char *json, const char *key, size_t *out_val);

/**
 * @brief Extracts a signed 32-bit int value for a given key.
 *
 * Supports positive and negative integer representations.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a number (such as HP, weapon damage, or moral alignment) from a file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses signed 32-bit integer token with optional leading minus sign into out_val.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination int variable.
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_int(const char *json, const char *key, int *out_val);

/**
 * @brief Extracts a boolean value (true or false) for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a yes/no switch from a data file (e.g. whether a weapon is two-handed).
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks for "true" or "false" tokens and sets out_val accordingly.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination bool variable.
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_bool(const char *json, const char *key, bool *out_val);

/**
 * @brief Extracts a floating-point float value for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a decimal multiplier or rating (such as 1.5 critical hit damage).
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses IEEE floating-point number into destination float variable.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination float variable.
 * @return true if key was found and parsed; false otherwise.
 */
bool json_get_float(const char *json, const char *key, float *out_val);

/**
 * @brief Locates embedded sub-object boundaries ({ ... }) for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Locates an entire nested data grouping (like a character's inventory block or stat table).
 *
 * ### For Systems Programmers & Engine Architects:
 * Scans balanced curly braces within the buffer starting from the key token,
 * returning the pointer to the opening '{' and total byte length without heap usage.
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
 * ### For Game Designers & Non-Programmers:
 * Reads a text field nested inside a specific sub-section of a file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scopes key search within the parent object slice bounds, copying string payload
 * safely with null-termination up to out_size.
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
 * ### For Game Designers & Non-Programmers:
 * Reads an integer stat nested inside an object group (e.g. stats.attack_power).
 *
 * ### For Systems Programmers & Engine Architects:
 * Isolates parent object slice and parses signed 32-bit integer token into out_val.
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
 * ### For Game Designers & Non-Programmers:
 * Reads a decimal multiplier nested inside an object (e.g. elemental.ice_multiplier).
 *
 * ### For Systems Programmers & Engine Architects:
 * Isolates parent object slice and parses floating-point token into out_val.
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
 * ### For Game Designers & Non-Programmers:
 * Reads a true/false flag nested inside an object (e.g. audio.loop).
 *
 * ### For Systems Programmers & Engine Architects:
 * Isolates parent object slice and parses boolean token into out_val.
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
 * ### For Game Designers & Non-Programmers:
 * Locates a list of items (such as party members, spell books, or loot drop tables).
 *
 * ### For Systems Programmers & Engine Architects:
 * Scans balanced square brackets starting from array_key, returning pointer to '['
 * and total slice byte length without heap allocations.
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
 * ### For Game Designers & Non-Programmers:
 * Counts how many items or entries are in a list.
 *
 * ### For Systems Programmers & Engine Architects:
 * Iterates through array slice taking nesting into account to return element count.
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
 * ### For Game Designers & Non-Programmers:
 * Reads a single text item from a list (e.g. reading item #3 from an inventory).
 *
 * ### For Systems Programmers & Engine Architects:
 * Traverses comma separators to index, copies unescaped string payload safely into out_val.
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
 * ### For Game Designers & Non-Programmers:
 * Reads an entire detailed object from a list (e.g. reading character #2's complete sheet).
 *
 * ### For Systems Programmers & Engine Architects:
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
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(JSON syntax contracts, token key matching rules, and numeric parsing limits
are verified via pre-flight diagnostic suites and standalone parsing tripwires.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on JSON parser invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the game's data reader to ensure it reads numbers, text, true/false toggles,
 * and lists from configuration files accurately without crashing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies extraction of strings, integers, negatives, booleans, floats,
 * array slicing, and bounds-checking resilience against truncated inputs.
 *
 * @return diag_result_t reporting categorized severity.
 */
diag_result_t json_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Simple pass/fail test ensuring game data files can be read cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Boolean audit harness validating token extraction, nested object slicing, and malformed JSON resilience.
 *
 * @return true if all JSON invariant tripwires pass cleanly; false otherwise.
 */
bool json_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap JSON Engine
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Inspects a key token, classifies its value type, and constructs a token view.
 *
 * ### For Game Designers & Non-Programmers:
 * Examines a specific property in a file to determine what kind of data it is.
 *
 * ### For Systems Programmers & Engine Architects:
 * Non-owning token parser isolating raw slice pointer and length.
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
 * ### For Game Designers & Non-Programmers:
 * Inspects a list in a data file, checking how many items it has.
 *
 * ### For Systems Programmers & Engine Architects:
 * Analyzes array bounds and pre-counts top-level elements without heap allocation.
 *
 * @param[in]  json      Null-terminated JSON buffer.
 * @param[in]  array_key Array property key name.
 * @param[out] out_view  Constructed array view object.
 *
 * @return true if array was located and view constructed; false otherwise.
 */
bool json_inspect_array(const char *json, const char *array_key, json_array_view_t *out_view);

/**
 * @brief Formats an ASCII diagnostic card summarizing an inspected JSON token view.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing token type, payload length, and validity.
 *
 * ### For Systems Programmers & Engine Architects:
 * Renders a 60-column terminal-friendly status card into out_buf without dynamic heap allocation.
 *
 * @param[in]  view     Populated json_token_view_t record.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return Number of characters written (excluding null terminator), or 0 on error.
 */
size_t json_execute_dump_card(const json_token_view_t *view, char *out_buf, size_t buf_size);

/**
 * @brief High-level execution procedure that extracts a string and constructs a result object.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a text property safely and returns a receipt confirming it was found.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded string extractor with null-termination and json_extract_result_t report.
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
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the zero-heap JSON parsing subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts up the JSON parser when the engine boots.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets internal parse caches and state.
 */
void json_utils_init(void);

/**
 * @brief Tears down the JSON parsing subsystem.
 *
 * ### For Game Designers & Non-Programmers:
 * Shuts down the JSON parser when the game exits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Flushes state and performs clean teardown.
 */
void json_utils_shutdown(void);

#endif /* TENEBRIS_LUX_IO_JSON_UTILS_H */
