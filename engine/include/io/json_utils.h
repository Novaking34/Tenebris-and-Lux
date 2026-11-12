/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/io/json_utils.h
@id            cog.json_utils
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap JSON Tokenizer & Structured Field Deserializer
@role          Public Contract & Seam: Zero-heap JSON token scanner, span-isolated
               field extractors, structural validator, and contract policy hooks.
@maturity      Rev 5 Baseline | Single-pass span APIs, policy hydration & io.json.* probes active.
@chassis       zero-heap | POD token spans & bounded caller buffers; parser caps match loader_contracts.json.
@scale         O(N) single-pass scan bounded by max_json_depth (32) & max_payload_bytes (64 KiB).
@seams         in: engine/config/io/loader_contracts.json | uses: cog.io.fallback, cog.log, cog.debug | probes: io.json.* -> DEBUG_STEP_AWAIT | dag: core.util.json
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Output string extraction unconditionally guarantees NUL termination.
               3. Span-scoped lookups constrain token scans cleanly inside [start, end).
               4. Syntax, depth, and malformed container faults return false cleanly without heap use.

PURPOSE & ARCHITECTURAL ROLE:
  Declares the public zero-heap JSON scanning, token inspection, typed scalar
  extraction, array walking, and span-isolated validation API used across all
  engine configs, manifests, and game data catalogs.

  ### For Game Designers & Non-Programmers:
  Powers fast reading of balance tables, item stats, dialogue presets, and
  diagnostic JSON profiles on constrained systems with zero memory allocation
  stutter and built-in structural safety limits.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Pure span views borrow immutable caller/ROM JSON
    text without allocation; caller owns destination buffers.
  - Extension Seam: Extend supported value types or adjust nesting depth caps via
    `loader_contracts.json` without pulling third-party parser dependencies.
  - Explicit Non-Goal: Does not construct DOM trees or enforce 5-block RPG
    envelope policy rules directly (layered cleanly in `cog.master_loader`).
================================================================================
*/

#ifndef CORNERSTONE_IO_JSON_UTILS_H
#define CORNERSTONE_IO_JSON_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Public Contract Limits, Fallback Link, Ontology & O(1) Pre-Conditions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.json_utils
Mission:       Declare JSON scanner bounds, BIOS link, token views, and span validators.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "io/io_fallback.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Canonical Token Limits & Compile-Time Proofs
--------------------------------------------------------------------------------
Compile-time sizing limits for JSON key lookup, token previews, and nesting.

### For Game Designers & Non-Programmers:
Ensures config JSONs parse cleanly without heap bloat, supporting keys up to 64
bytes and structured nesting up to 32 levels deep.

### For Systems Programmers & Engine Architects:
Guarantees 100% zero-heap allocation (`ALLOC_ZERO_HEAP`) and proves alignment
with `io_fallback.h` JSON limits at compile time.
--------------------------------------------------------------------------------
*/

/** Maximum key length in bytes supported by token extractors. */
#define JSON_MAX_KEY_LEN          64

/** Maximum token preview string length in bytes. */
#define JSON_MAX_TOKEN_LEN        128

/** Maximum supported bracket nesting depth (aligned with loader_contracts.json). */
#define JSON_MAX_NESTING          32

/** Default bounded single-pass JSON token scan ceiling. */
#define JSON_DEFAULT_MAX_TOKENS   1024U

/** Default maximum JSON payload ceiling in bytes (matches io_fallback.h). */
#define JSON_DEFAULT_MAX_PAYLOAD  65536U

static_assert(
    JSON_MAX_NESTING == IO_FB_MAX_JSON_DEPTH &&
    JSON_DEFAULT_MAX_PAYLOAD == IO_FB_MAX_CONFIG_BYTES,
    "JSON scanner limits must align identically with Tier-0 BIOS ceilings"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Zero-Heap JSON parsing engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string matching core/manifest.h & engine/config/io/manifest.json. */
#define JSON_UTILS_MODULE_ID           "core.util.json"

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

/**
 * @struct json_policy
 * @brief Runtime JSON parser bounds and escape validation policy hydrated from config/ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Controls safety limits for reading config and lore JSON files (how deep sections
 * can nest, maximum file size, and whether broken escapes trigger debugger stops).
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed-layout zero-heap policy record hydrated from `engine/config/io/loader_contracts.json`
 * and `default.json` (`.rodata` ROM fallback cleanly sets `loaded_from_data = false`).
 */
typedef struct json_policy {
    int    max_json_depth;         /**< Max nested bracket depth (1..JSON_MAX_NESTING). */
    size_t max_json_tokens;        /**< Max single-pass tokens (1..JSON_DEFAULT_MAX_TOKENS). */
    size_t max_payload_bytes;      /**< Hard JSON payload cap (64..JSON_DEFAULT_MAX_PAYLOAD). */
    size_t max_key_len;            /**< Key buffer cap in bytes (1..JSON_MAX_KEY_LEN). */
    size_t max_token_len;          /**< Token preview cap in bytes (1..JSON_MAX_TOKEN_LEN). */
    bool   strict_utf8_escapes;    /**< Reject invalid escape codes / unescaped control chars. */
    bool   reject_duplicate_keys;  /**< Flag duplicate top-level keys during validation. */
    bool   probe_await_on_corrupt; /**< Escalate syntax/depth/policy faults to DEBUG_STEP_AWAIT. */
    char   source_tag[32];         /**< Origin label (`"loader_contracts"`, `"rom_bios"`, etc.). */
    bool   loaded_from_data;       /**< True ONLY when hydrated cleanly from disk JSON config. */
} json_policy_t;

/**
 * @struct json_validation_report
 * @brief Detailed diagnostic output populated by structural JSON span validators.
 *
 * ### For Game Designers & Non-Programmers:
 * Pinpoints the exact spot in a JSON file if a quote, brace, or nested section breaks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap validation transcript carrying taxonomy code (`IO_ERR_JSON_*`),
 * byte offset, observed max depth, and token counter.
 */
typedef struct json_validation_report {
    bool     valid;               /**< True if structural JSON scan passed all active bounds. */
    uint16_t error_code;          /**< 0 on pass; 0x0601 (syntax) or 0x0603 (depth) on fail. */
    size_t   error_offset;        /**< Byte offset from span start where violation surfaced. */
    int      max_depth_observed;  /**< Deepest brace/bracket nesting level encountered. */
    size_t   token_count;         /**< Count of scanned keys/values encountered. */
    char     diagnostic[96];      /**< Concise human-readable diagnostic explanation. */
} json_validation_report_t;

/**
 * @struct json_utils_stats
 * @brief Live telemetry counters and active policy copy for Layer 0 JSON engine.
 *
 * ### For Game Designers & Non-Programmers:
 * Shows how many fields were extracted cleanly and catches depth overflows, syntax
 * mistakes, or short buffer truncations early during development.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copied cleanly from static `.bss` state by `json_utils_get_stats()` (zero heap).
 */
typedef struct json_utils_stats {
    bool          initialized;        /**< True after `json_utils_init()` completes. */
    bool          policy_from_data;   /**< True if active policy loaded from disk JSON. */
    uint64_t      key_queries;        /**< Total scoped/flat key locator queries run. */
    uint64_t      extractions_ok;     /**< Successful scalar, slice, or array extractions. */
    uint64_t      depth_overflows;    /**< Scans aborted for exceeding max_json_depth. */
    uint64_t      syntax_faults;      /**< Unterminated strings/containers or bad tokens. */
    uint64_t      type_mismatches;    /**< Keys found whose value failed target type parse. */
    uint64_t      truncated_strings;  /**< Valid strings clipped cleanly to caller out_size. */
    uint64_t      token_inspections;  /**< Token/array inspection calls completed. */
    json_policy_t active_policy;      /**< Snapshot of currently enforced parser policy. */
} json_utils_stats_t;

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

/**
 * @brief Predicate verifying whether a `json_policy_t` fits Layer 0 safety bounds.
 *
 * ### For Game Designers & Non-Programmers:
 * Prevents invalid or corrupted JSON reader limits from entering active engine memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure O(1) bounds guard checking `0 < max_json_depth <= 32`, token ceiling, payload cap,
 * and key/token length invariants aligned with `loader_contracts.json`.
 *
 * @param[in] policy Candidate policy struct pointer.
 * @return true if policy is non-NULL and all bounds lie inside valid engine limits.
 */
static inline bool json_policy_is_valid(const json_policy_t *policy) {
    return (policy != NULL) &&
           (policy->max_json_depth > 0 && policy->max_json_depth <= JSON_MAX_NESTING) &&
           (policy->max_json_tokens > 0U && policy->max_json_tokens <= JSON_DEFAULT_MAX_TOKENS) &&
           (policy->max_payload_bytes >= 64U &&
            policy->max_payload_bytes <= JSON_DEFAULT_MAX_PAYLOAD) &&
           (policy->max_key_len > 0U && policy->max_key_len <= JSON_MAX_KEY_LEN) &&
           (policy->max_token_len > 0U && policy->max_token_len <= JSON_MAX_TOKEN_LEN);
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Public Operational Cogs, State Accessors & Domain APIs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0 (Core Utilities)
Module:        cog.json_utils
Mission:       Zero-Heap JSON Tokenizer, Span Scopes & Policy Configuration APIs
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: JSON Token Locator Cogs
Mission: Scan flat or span-bounded JSON payloads for matching keys, objects, and
         arrays while respecting escape sequences and nesting limits cleanly.

  ### For Game Designers & Non-Programmers:
  Finds named sections, tables, and lists inside JSON configuration files cleanly
  without mistaking words inside text descriptions for structural headings.

  ### For Systems Programmers & Engine Architects:
  Exposes zero-copy, zero-heap token and balanced-container locators constrained
  to `[json_start, json_start + json_len)` and active `max_json_depth` policy.
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
 * @triggers On nesting limit exceeded -> `IO_ERR_JSON_DEPTH_EXCEEDED` via `io.json.depth_bounds`.
 *
 * @param[in] json Null-terminated JSON buffer.
 * @param[in] key  Null-terminated key name to search for (without quotes).
 * @return Pointer to value position immediately following the colon (:),
 *         or NULL if key is not found or parameters are invalid.
 */
const char *json_find_key(const char *json, const char *key);

/**
 * @brief Span-bounded key locator preventing sibling overrun across sliced buffers.
 *
 * ### For Game Designers & Non-Programmers:
 * Searches strictly inside one specific section of a JSON file so properties from
 * neighboring sections can never bleed in accidentally.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-copy locator bounded strictly to `[json_start, json_start + json_len)`.
 * Properly skips quoted string values (respecting `\\` and `\"` escapes) and
 * enforces active `max_json_depth`.
 *
 * @triggers On nesting overflow -> `IO_ERR_JSON_DEPTH_EXCEEDED` fault + `io.json.depth_bounds`.
 *
 * @param[in] json_start Pointer to JSON slice start.
 * @param[in] json_len   Exact byte length of slice (`0` measures NUL-terminated buffer).
 * @param[in] key        Key name to match.
 * @return Pointer to trimmed value token following ':', or NULL if absent/invalid.
 */
const char *json_find_key_span(const char *json_start, size_t json_len, const char *key);

/**
 * @brief Span-bounded nested JSON object (`{...}`) locator.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a sub-table inside a JSON file (such as `metrics` or `stats`) and
 * fences its boundaries cleanly so fields outside that table cannot leak in.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates to `json_find_delimited_span_internal()` inside `[json_start, json_len)`,
 * matching balanced `{}` braces up to `max_json_depth` with zero allocation.
 *
 * @triggers On depth overflow -> `IO_ERR_JSON_DEPTH_EXCEEDED` fault + `io.json.depth_bounds`.
 *
 * @param[in]  json_start    Pointer to JSON slice start.
 * @param[in]  json_len      Exact byte length of slice (`0` measures NUL-terminated).
 * @param[in]  object_key    Target key whose value must be a JSON object.
 * @param[out] out_obj_start Receives pointer to opening `'{'`.
 * @param[out] out_obj_len   Receives byte length including closing `'}'`.
 * @return true if bounded object slice located cleanly; false otherwise.
 */
bool json_find_object_span(const char *json_start, size_t json_len,
                           const char *object_key,
                           const char **out_obj_start, size_t *out_obj_len);

/**
 * @brief Span-bounded nested JSON array (`[...]`) locator.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a JSON list (such as `"tags"` or `"skills"`) and fences its exact
 * start and end positions so adjacent lists are never read by mistake.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded zero-copy bracket matcher operating strictly within `[json_start, json_len)`.
 * Enforces `max_json_depth` without touching caller memory beyond `json_len`.
 *
 * @triggers On depth overflow -> `IO_ERR_JSON_DEPTH_EXCEEDED` fault + `io.json.depth_bounds`.
 *
 * @param[in]  json_start    Pointer to JSON slice start.
 * @param[in]  json_len      Exact byte length of slice (`0` measures NUL-terminated).
 * @param[in]  array_key     Target key whose value must be a JSON array.
 * @param[out] out_arr_start Receives pointer to opening `'['`.
 * @param[out] out_arr_len   Receives byte length including closing `']'`.
 * @return true if bounded array slice located cleanly; false otherwise.
 */
bool json_find_array_span(const char *json_start, size_t json_len,
                          const char *array_key,
                          const char **out_arr_start, size_t *out_arr_len);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Typed Value & Nested Object Extraction Cogs
Mission: Extract typed values (string, size_t, int, bool, float) and object-scoped
         fields into caller-owned storage without heap allocations.

  ### For Game Designers & Non-Programmers:
  Reads numbers, text labels, true/false switches, and nested stat groups out of
  JSON configuration and gameplay files safely.

  ### For Systems Programmers & Engine Architects:
  Combines key lookup with strict bounds-checked scalar decoding and bounded
  parent-object isolation (`json_find_object` + `json_get_object_*`).
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts a string literal value for a key into a caller-supplied buffer.
 *
 * Deserializes string properties (e.g. `"version": "1.0.0"`, `"mode": "standard"`).
 * Strips surrounding quotes and guarantees bounded null termination.
 *
 * ### For Game Designers & Non-Programmers:
 * Copies a piece of text (like a profile label or ability description) out of a JSON file.
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
Sub-Assembly 3.3: Array Inspection & Indexed Element Extraction Cogs
Mission: Scan JSON array boundaries and extract typed elements without heap usage.

  ### For Game Designers & Non-Programmers:
  Counts entries and pulls individual items or nested tables out of JSON lists
  such as skill sets, dialogue choices, and asset arrays.

  ### For Systems Programmers & Engine Architects:
  Tracks balanced bracket and quote depth inside `[arr_start, arr_start + arr_len)`
  without dynamic allocations or partial buffer leaks.
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
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Span-Bounded Extractors & Runtime Policy Configuration Cogs
Mission: Guarantee strict `[json_start, json_start + json_len)` boundary isolation
         and manage runtime parser policy hydration via `loader_contracts.json` or ROM.

  ### For Game Designers & Non-Programmers:
  Reads values strictly inside fenced JSON blocks (`block_3_body`) and updates
  parser safety limits from disk presets or built-in ROM backups seamlessly.

  ### For Systems Programmers & Engine Architects:
  Isolates key scans to `[json_start, json_start + json_len)`, validates numeric/bool
  delimiters strictly, and wires policy limit violations into `io.json.policy_bounds`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Span-bounded string value extractor with JSON escape decoding (`\"`, `\\`, `\n`, etc.).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads text inside a strictly delimited JSON section so quotes and special characters
 * decode cleanly without leaking into adjacent blocks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Scans `[json_start, json_start + json_len)` with zero allocation; unconditionally
 * null-terminates `out_val` and returns false on buffer truncation.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Slice byte length.
 * @param[in]  key        Key name to resolve inside slice.
 * @param[out] out_val    Destination string buffer (unconditionally null-terminated).
 * @param[in]  out_size   Capacity of `out_val` in bytes.
 * @return true if string value found and decoded cleanly; false otherwise.
 */
bool json_get_string_span(const char *json_start, size_t json_len,
                          const char *key, char *out_val, size_t out_size);

/**
 * @brief Span-bounded strict `int` extractor rejecting trailing garbage and `INT_MIN`/`INT_MAX` overflow.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely reads whole numbers inside a fenced JSON section, rejecting malformed typos like `"42abc"`.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded `[json_start, json_len)` `strtol()` wrapper enforcing delimiter termination and 32-bit range.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Slice byte length (`0` measures NUL-terminated buffer).
 * @param[in]  key        Key name to locate.
 * @param[out] out_val    Destination signed integer (untouched on failure).
 * @return true if strictly valid `int` parsed; false otherwise.
 */
bool json_get_int_span(const char *json_start, size_t json_len,
                       const char *key, int *out_val);

/**
 * @brief Span-bounded strict `size_t` extractor rejecting negative numbers and overflow.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely reads positive sizes and buffer capacities from a fenced JSON section.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded `[json_start, json_len)` `strtoull()` parser rejecting leading `-` signs and `SIZE_MAX` overflow.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Slice byte length (`0` measures NUL-terminated buffer).
 * @param[in]  key        Key name to locate.
 * @param[out] out_val    Destination unsigned size (untouched on failure).
 * @return true if strictly valid non-negative integer parsed; false otherwise.
 */
bool json_get_size_t_span(const char *json_start, size_t json_len,
                          const char *key, size_t *out_val);

/**
 * @brief Span-bounded strict `float` extractor rejecting non-finite or trailing non-numeric text.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads decimal multipliers (such as `"1.25"`) from a fenced JSON block while rejecting NaN/Inf.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded `[json_start, json_len)` `strtod()` wrapper validating `isfinite()` and delimiter boundary.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Slice byte length (`0` measures NUL-terminated buffer).
 * @param[in]  key        Key name to locate.
 * @param[out] out_val    Destination float value (untouched on failure).
 * @return true if valid finite float parsed cleanly; false otherwise.
 */
bool json_get_float_span(const char *json_start, size_t json_len,
                         const char *key, float *out_val);

/**
 * @brief Span-bounded strict `bool` extractor checking delimiter boundaries after `true`/`false`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads on/off toggles inside a fenced JSON section and rejects typos like `truefish`.
 *
 * ### For Systems Programmers & Engine Architects:
 * Exact token matcher verifying whitespace/comma/brace delimiter immediately follows `true`/`false`.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Slice byte length (`0` measures NUL-terminated buffer).
 * @param[in]  key        Key name to locate.
 * @param[out] out_val    Destination bool value (untouched on failure).
 * @return true if exact `true` or `false` token parsed cleanly; false otherwise.
 */
bool json_get_bool_span(const char *json_start, size_t json_len,
                        const char *key, bool *out_val);

/**
 * @brief Hydrates parser policy limits from disk `io/loader_contracts.json` or Tier-0 `.rodata` ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads JSON parser safety rules (depth caps, strict escape validation) automatically
 * from configuration files or falls back to safe built-in ROM rules when files are absent.
 *
 * ### For Systems Programmers & Engine Architects:
 * Synchronizes `io_fallback` active config + loader contract, resolving optional disk path
 * via VFS and falling back cleanly on fault (`loaded_from_data = false`).
 *
 * @triggers On corrupt disk contract -> `io.json.policy_bounds` probe + ROM Safe-Mode downgrade.
 *
 * @param[in] rel_or_abs_path Optional override path (`NULL` resolves `"io/loader_contracts.json"`).
 * @return true if hydrated cleanly from disk (`loaded_from_data = true`); false if Safe-Mode ROM used.
 */
bool json_utils_load_policy(const char *rel_or_abs_path);

/**
 * @brief Copies currently active `json_policy_t` into caller struct.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets developer panels or diagnostics inspect which safety limits the JSON reader is enforcing.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) lazy-armed snapshot copy from Sub-Assembly 2.3 `.bss` `s_json_state.policy`.
 *
 * @param[out] out_policy Destination policy copy.
 * @return true on non-NULL destination; false otherwise.
 */
bool json_utils_get_policy(json_policy_t *out_policy);

/**
 * @brief Validates and installs a candidate parser policy, downgrading on contract breach.
 *
 * ### For Game Designers & Non-Programmers:
 * Updates JSON parser limits at runtime and restores safe ROM defaults if bad limits are passed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks `json_utils_validate_policy_contract()`; on rejection restores Safe-Mode ROM policy,
 * emits `LOG_CAT_IO`, and escalates `io.json.policy_bounds` when `debug_is_active()`.
 *
 * @triggers Invalid policy -> Safe-Mode ROM restore + `IO_ERR_JSON_SYNTAX` + `io.json.policy_bounds`.
 *
 * @param[in] policy Candidate `json_policy_t` struct.
 * @return true if valid and installed; false on invalid bounds (downgrades to ROM Safe Mode).
 */
bool json_utils_set_policy(const json_policy_t *policy);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Zero-Heap JSON Engine
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate structural JSON integrity, register debugger probes, and guard Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Structural Validation, Policy Contracts & Debugger Probes
Mission: Validate balanced braces/quotes/escapes/depth on raw JSON payloads,
         verify policy bounds, and wire `io.json.*` probes into `debug.c`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Verifies policy bounds contract cleanly without mutating global state.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks that custom JSON safety limits stay within safe nesting and buffer caps before use.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure side-effect-free predicate wrapping `json_policy_is_valid()` plus key/token invariants.
 *
 * @param[in] policy Policy descriptor to inspect.
 * @return true if policy passes all depth, token, payload, and key/token bounds.
 */
bool json_utils_validate_policy_contract(const json_policy_t *policy);

/**
 * @brief Performs a pure zero-heap structural syntax and depth scan across `[json_start, json_len)`.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks an entire config file for unclosed quotes, mismatched brackets, or extreme
 * nesting before the game relies on its data.
 *
 * ### For Systems Programmers & Engine Architects:
 * Single-pass O(N) structural validator checking balanced `{}`/`[]`, proper string
 * escape parity, `max_json_depth`, `max_json_tokens`, `strict_utf8_escapes`, and
 * optional top-level duplicate key detection (`reject_duplicate_keys`). Side-effect
 * free report population (does not enter AWAIT directly).
 *
 * @param[in]  json_start Pointer to JSON buffer start.
 * @param[in]  json_len   Exact byte length (`0` measures NUL-terminated buffer).
 * @param[out] out_report Optional structured diagnostic report receiving offset/code.
 * @return true if structurally valid within active policy bounds; false otherwise.
 */
bool json_validate_span(const char *json_start,
                        size_t json_len,
                        json_validation_report_t *out_report);

/**
 * @brief Validates a JSON span and trips `io.json.syntax` / `io.json.depth_bounds` on violation.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs full structural verification on critical configs and immediately alerts the
 * Safe-Mode debugger if brackets, quotes, or nesting limits are broken.
 *
 * ### For Systems Programmers & Engine Architects:
 * Calls `json_validate_span()`; on failure increments fault counters, logs to `LOG_CAT_IO`,
 * records Tier-0 I/O fault, and calls `json_utils_trigger_probe()` (`DEBUG_STEP_AWAIT`).
 *
 * @triggers Structural syntax error -> `IO_ERR_JSON_SYNTAX` -> `io.json.syntax` -> `DEBUG_STEP_AWAIT`.
 * @triggers Depth overflow -> `IO_ERR_JSON_DEPTH_EXCEEDED` -> `io.json.depth_bounds` -> `DEBUG_STEP_AWAIT`.
 *
 * @param[in]  json_start    Pointer to JSON buffer start.
 * @param[in]  json_len      Exact byte length (`0` measures NUL-terminated).
 * @param[in]  context_label Human-readable caller/asset tag for diagnostics.
 * @param[out] out_report    Optional diagnostic report populated before trap escalation.
 * @return true if valid; false after recording telemetry and escalating probe on fault.
 */
bool json_utils_enforce_valid_span(const char *json_start,
                                   size_t json_len,
                                   const char *context_label,
                                   json_validation_report_t *out_report);

/**
 * @brief Idempotently registers `io.json.syntax`, `io.json.depth_bounds`, and `io.json.policy_bounds`
 *        validation probes into the central debugger (`debug.c`).
 *
 * ### For Game Designers & Non-Programmers:
 * Links JSON parsing safeguards directly to the interactive debugger so broken configs
 * stop safely before corrupting game state.
 *
 * ### For Systems Programmers & Engine Architects:
 * Registers Sub-Assembly 2.3 `s_json_probes[3]` via `debug_register_validation_probe()`.
 *
 * @triggers Connects `TW_ENG_JSON_SYNTAX` & `TW_ENG_JSON_DEPTH_EXCEEDED` to `DEBUG_STEP_AWAIT`.
 */
void json_utils_register_debug_probes(void);

/**
 * @brief Manually escalates a JSON parser fault to Tier-0 BIOS + registered `debug` probe.
 *
 * ### For Game Designers & Non-Programmers:
 * Raises an immediate Safe-Mode debugger alarm when corrupted JSON threatens loading.
 *
 * ### For Systems Programmers & Engine Architects:
 * Selects matching probe ID from `error_code`, protects against preflight recursion,
 * downgrades BIOS via `io_fallback_downgrade_safe_mode()`, and invokes `debug_trigger_probe()`.
 *
 * @triggers Fires `io.json.depth_bounds` (0x0603), `io.json.policy_bounds`, or `io.json.syntax` (0x0601).
 *
 * @param[in] context_label Descriptive failure summary.
 * @param[in] error_code    Taxonomy code (`IO_ERR_JSON_SYNTAX` or `IO_ERR_JSON_DEPTH_EXCEEDED`).
 * @return true if `debug_trigger_probe()` processed the escalation; false if suppressed.
 */
bool json_utils_trigger_probe(const char *context_label, uint16_t error_code);

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

/**
 * @brief Copies live JSON parser telemetry counters and active policy into `out_stats`.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets developer menus show live JSON parse counts, truncated fields, and syntax warnings.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) copy of `s_json_state` counters into caller storage under `ALLOC_ZERO_HEAP`.
 *
 * @param[out] out_stats Destination stats record.
 * @return true on non-NULL destination; false otherwise.
 */
bool json_utils_get_stats(json_utils_stats_t *out_stats);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Boot Tier-0 BIOS if needed, hydrate `loader_contracts.json` policy from
disk or `.rodata` ROM vault, register `io.json.*` debugger probes, and log clean
lifecycle telemetry under `ALLOC_ZERO_HEAP` (zero dynamic heap buffers).
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the zero-heap JSON parsing subsystem and hydrates active policy.
 *
 * ### For Game Designers & Non-Programmers:
 * Prepares the data reader at engine startup and loads JSON safety rules from disk or ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Ensures Tier-0 I/O BIOS is booted, hydrates `json_policy_t` from `loader_contracts.json`
 * (or ROM fallback), registers `io.json.*` validation probes, and logs boot telemetry
 * cleanly on `LOG_CAT_IO`.
 *
 * @triggers Registers `io.json.syntax`, `io.json.depth_bounds`, `io.json.policy_bounds`.
 */
void json_utils_init(void);

/**
 * @brief Shuts down the zero-heap JSON parsing subsystem and seals telemetry.
 *
 * ### For Game Designers & Non-Programmers:
 * Cleanly logs final data-parsing health stats when exiting the game.
 *
 * ### For Systems Programmers & Engine Architects:
 * Emits final query/extraction/fault counters to `LOG_CAT_IO` and clears initialized flag.
 */
void json_utils_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_IO_JSON_UTILS_H */
