// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          json_utils.h
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
//   Zero-heap, memory-safe JSON parser and key/value token extractor.
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

#ifndef TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H
#define TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Manifest record for the Zero-Heap JSON Engine.
 *
 * Exposes compiled module provenance, zero-heap memory guarantee,
 * reentrancy policy, and parser pre-flight audit callback.
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
 * Scans a JSON buffer for a specific top-level or scoped key token.
 *
 * ### Machine Role & Mission
 *
 * Core search primitive that locates occurrences of `"key"` followed by a colon
 * (`:`), ignoring intervening whitespace. Prevents false positive matches on
 * substring prefixes or keys embedded inside quoted strings.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.json (Zero-Heap JSON Engine)
 * - Assembly:   JSON Token Locator
 * - Cog:        Key Pointer Scanner
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Stateless pointer scanner).
 * - Concurrency:    REENTRANT_PURE (Thread-safe read-only inspection).
 * - Latency Budget: < 20 microseconds (O(N) linear scan).
 *
 * @param[in] json Null-terminated JSON buffer.
 * @param[in] key  Null-terminated key name to search for (without quotes).
 *
 * @return Pointer to value position immediately following the colon (`:`),
 *         or `NULL` if key is not found or parameters are invalid.
 */
const char *json_find_key(const char *json, const char *key);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: VALUE EXTRACTION COGS ]
// Mission: Extract typed values (string, integer, size_t, bool, float).
// -----------------------------------------------------------------------------

/**
 * Extracts a string literal value for a given key into a caller-supplied buffer.
 *
 * ### Machine Role & Mission
 *
 * Deserializes string properties (e.g. `"version": "1.0.0"`, `"name": "Lux"`).
 * Strips surrounding quotes and guarantees bounded null termination.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.json (Zero-Heap JSON Engine)
 * - Assembly:   Value Extraction
 * - Cog:        Bounded String Field Extractor
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, writes to caller buffer).
 * - Concurrency:    REENTRANT_PURE (Thread-safe on distinct destination buffers).
 * - Latency Budget: < 30 microseconds.
 *
 * @param[in]  json     Null-terminated JSON buffer.
 * @param[in]  key      Key name to search for.
 * @param[out] out_val  Destination buffer for extracted string.
 * @param[in]  out_size Capacity of destination buffer in bytes.
 *
 * @return `true` if key was found and value extracted; `false` otherwise.
 *
 * @post `out_val[out_size - 1] == '\0'` is guaranteed if returning `true`.
 */
bool json_get_string(const char *json, const char *key, char *out_val, size_t out_size);

/**
 * Extracts an unsigned `size_t` integer value for a given key.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination `size_t` variable.
 *
 * @return `true` if key was found and parsed; `false` otherwise.
 */
bool json_get_size_t(const char *json, const char *key, size_t *out_val);

/**
 * Extracts a signed 32-bit `int` value for a given key (supports negative values).
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination `int` variable.
 *
 * @return `true` if key was found and parsed; `false` otherwise.
 */
bool json_get_int(const char *json, const char *key, int *out_val);

/**
 * Extracts a boolean value (`true` or `false`) for a given key.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination `bool` variable.
 *
 * @return `true` if key was found and parsed; `false` otherwise.
 */
bool json_get_bool(const char *json, const char *key, bool *out_val);

/**
 * Extracts a floating-point `float` value for a given key.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to search for.
 * @param[out] out_val Pointer to destination `float` variable.
 *
 * @return `true` if key was found and parsed; `false` otherwise.
 */
bool json_get_float(const char *json, const char *key, float *out_val);

/**
 * Locates an embedded sub-object boundaries (`{ ... }`) for a given key.
 *
 * @param[in]  json          Null-terminated JSON buffer.
 * @param[in]  object_key    Key name containing the object.
 * @param[out] out_obj_start Pointer to the opening `{` of the sub-object.
 * @param[out] out_obj_len   Length in bytes of the sub-object including `{` and `}`.
 *
 * @return `true` if sub-object was located and balanced; `false` otherwise.
 */
bool json_find_object(const char *json, const char *object_key,
                      const char **out_obj_start, size_t *out_obj_len);

/**
 * Extracts a string value from within an embedded JSON object.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Key name of parent object (or NULL for top-level).
 * @param[in]  key            Key name to extract inside the object.
 * @param[out] out_val        Destination buffer for extracted string.
 * @param[in]  out_size       Capacity of destination buffer in bytes.
 *
 * @return `true` if parent object and key were found and parsed; `false` otherwise.
 */
bool json_get_object_string(const char *json, const char *parent_obj_key,
                            const char *key, char *out_val, size_t out_size);

/**
 * Extracts a signed 32-bit int value from within an embedded JSON object.
 */
bool json_get_object_int(const char *json, const char *parent_obj_key,
                         const char *key, int *out_val);

/**
 * Extracts a float value from within an embedded JSON object.
 */
bool json_get_object_float(const char *json, const char *parent_obj_key,
                           const char *key, float *out_val);

/**
 * Extracts a boolean value from within an embedded JSON object.
 */
bool json_get_object_bool(const char *json, const char *parent_obj_key,
                          const char *key, bool *out_val);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.3: ARRAY INSPECTION & EXTRACTION COGS ]
// Mission: Scan JSON array boundaries and extract elements without heap usage.
// -----------------------------------------------------------------------------

/**
 * Locates an embedded array `[ ... ]` for a given key.
 *
 * @param[in]  json          Null-terminated JSON buffer.
 * @param[in]  array_key     Key name containing the array.
 * @param[out] out_arr_start Pointer to the opening `[` of the array.
 * @param[out] out_arr_len   Length in bytes of the array including `[` and `]`.
 *
 * @return `true` if array was located and brackets balanced; `false` otherwise.
 */
bool json_find_array(const char *json, const char *array_key,
                     const char **out_arr_start, size_t *out_arr_len);

/**
 * Returns the number of comma-separated elements in a JSON array.
 *
 * @param[in] arr_start Pointer to opening `[` of array.
 * @param[in] arr_len   Length of array in bytes.
 *
 * @return Count of elements in the array.
 */
size_t json_array_get_count(const char *arr_start, size_t arr_len);

/**
 * Extracts the i-th string element from a JSON array of strings into caller buffer.
 *
 * @param[in]  arr_start Pointer to opening `[` of array.
 * @param[in]  arr_len   Length of array in bytes.
 * @param[in]  index     0-based element index.
 * @param[out] out_val   Destination buffer for extracted string.
 * @param[in]  out_size  Capacity of destination buffer in bytes.
 *
 * @return `true` if element exists and was extracted; `false` otherwise.
 */
bool json_array_get_string(const char *arr_start, size_t arr_len,
                           size_t index, char *out_val, size_t out_size);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify JSON parsing invariants, string boundary checks, and types.
// -----------------------------------------------------------------------------

/**
 * Executes machine pre-flight diagnostic checks on JSON parser invariants.
 *
 * Verifies extraction of strings, integers, negatives, booleans, floats,
 * and bounds-checking resilience against truncated or malformed inputs.
 *
 * @return `diag_result_t` reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t json_utils_preflight(void);

/**
 * High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return `true` if all JSON invariant tripwires pass cleanly;
 *         `false` otherwise.
 */
bool json_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_UTIL_DATA_JSON_UTILS_H */
