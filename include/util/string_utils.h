// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          string_utils.h
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
//   Safe string manipulation, bounded copying, and case-insensitive matching.
//   Serves as text safety layer preventing buffer overruns, unterminated
//   strings, and NULL pointer faults across CLI and dialogue subsystems.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (Caller-supplied stack buffers).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant buffer operations).
//   - Latency Budget:        < 10 microseconds per operation.
//
// AXIOMATIC INVARIANTS:
//   1. Memory safety: dst[dst_size - 1] is unconditionally null-terminated.
//   2. Resilience: Gracefully handles NULL pointers across all parameters.
//   3. Zero heap allocations: All mutations operate in-place or into buffers.
// =============================================================================

#ifndef TENEBRIS_LUX_STRING_UTILS_H
#define TENEBRIS_LUX_STRING_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Manifest record for the Safe String Engine.
 *
 * Exposes compiled module provenance, zero-heap memory guarantee,
 * reentrancy policy, and string safety pre-flight audit callback.
 */
extern const module_metadata_t string_utils_manifest;

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
void str_safe_copy(char *dst, const char *src, size_t dst_size);

/**
 * Strips leading and trailing whitespace from a string in-place.
 *
 * ### Machine Role & Mission
 *
 * Cleans user command inputs, dialogue script lines, and configuration tokens.
 * Shifts trimmed content using memory-safe `memmove` without heap allocation.
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
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, in-place mutation).
 * - Concurrency:    REENTRANT_PURE (Thread-safe on distinct buffers).
 * - Latency Budget: < 5 microseconds execution time (O(N) bounded).
 *
 * @param[in,out] str Pointer to mutable null-terminated string buffer.
 *
 * @note Gracefully no-ops if `str` is NULL or empty string.
 */
void str_trim_inplace(char *str);

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 3.2: CASE-INSENSITIVE TEXT MATCHING COGS ]
// Mission: Parse player command verbs and dialogue responses flexibly.
// -----------------------------------------------------------------------------

/**
 * Compares two strings for equality ignoring ASCII case differences.
 *
 * ### Machine Role & Mission
 *
 * Evaluates player CLI verbs and action commands (e.g. "ward", "STRIKE",
 * "inspect"). Returns exact boolean equivalence without mutating either string.
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.string (Safe String Engine)
 * - Assembly:   Case-Insensitive Text Matching
 * - Cog:        ASCII Case-Insensitive Equality Matcher
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, read-only pointers).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 5 microseconds execution time (O(N) bounded).
 *
 * @param[in] a First string to compare.
 * @param[in] b Second string to compare.
 *
 * @return `true` if strings match character-for-character ignoring case.
 *
 * @note If both are NULL, returns `true`; if only one is NULL, returns `false`.
 */
bool str_case_eq(const char *a, const char *b);

/**
 * Searches for a case-insensitive substring occurrence within a haystack.
 *
 * ### Machine Role & Mission
 *
 * Provides flexible player command parsing (e.g. detecting "sword" in
 * "draw sword", or "lux" in "raise the shield of lux").
 *
 * ### Ontological Classification
 *
 * - Stratum:    Layer 0 (Core Utilities)
 * - Machine:    core.util.string (Safe String Engine)
 * - Assembly:   Case-Insensitive Text Matching
 * - Cog:        ASCII Case-Insensitive Substring Finder
 *
 * ### Execution & Resource Contract
 *
 * - Memory Model:   ALLOC_ZERO_HEAP (Zero dynamic heap, read-only pointers).
 * - Concurrency:    REENTRANT_PURE (Stateless, deterministic, thread-safe).
 * - Latency Budget: < 10 microseconds execution time (O(N*M) bounded).
 *
 * @param[in] haystack The null-terminated text to search within.
 * @param[in] needle   The null-terminated pattern to search for.
 *
 * @return `true` if `needle` is found within `haystack`; `false` otherwise.
 *
 * @note An empty `needle` ("") matches any non-NULL haystack.
 */
bool str_contains_nocase(const char *haystack, const char *needle);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
// =============================================================================

// -----------------------------------------------------------------------------
// [ SUB-ASSEMBLY 4.1: MACHINE SELF-AUDIT & PRE-FLIGHT TRIPWIRES ]
// Mission: Verify string buffer safety, NULL resilience, and trimming.
// -----------------------------------------------------------------------------

/**
 * Executes machine pre-flight diagnostic checks on string safety invariants.
 *
 * Verifies bounded copying, null termination enforcement, in-place trimming,
 * and case-insensitive comparison correctness.
 *
 * @return `diag_result_t` reporting SEVERITY_PASS or SEVERITY_FATAL.
 */
diag_result_t string_utils_preflight(void);

/**
 * High-level boolean audit wrapper for standalone test harnesses.
 *
 * @return `true` if all string invariant tripwires pass cleanly.
 */
bool string_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Include Guard Teardown & Header Close )
// =============================================================================

#endif /* TENEBRIS_LUX_STRING_UTILS_H */
