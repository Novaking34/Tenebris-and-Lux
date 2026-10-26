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
//   Serves as the text safety layer preventing buffer overruns, unterminated
//   strings, and NULL pointer faults across CLI and dialogue subsystems.
//
// DEPENDENCY & RESOURCE CONTRACTS:
//   - Upstream Dependencies: None (Foundational leaf module).
//   - Memory Allocation:     ALLOC_ZERO_HEAP (In-place or caller-supplied stack buffers).
//   - Concurrency / Safety:  REENTRANT_PURE (Pure reentrant execution on provided buffers).
//   - Latency Budget:        < 10 microseconds per operation.
//
// AXIOMATIC INVARIANTS:
//   1. Memory safety: dst[dst_size - 1] is unconditionally guaranteed null-terminated.
//   2. Resilience: Gracefully handles NULL pointers across all parameters without crashing.
//   3. Zero heap allocations: All mutations operate in-place or into caller buffers.
// =============================================================================

#ifndef TENEBRIS_LUX_STRING_UTILS_H
#define TENEBRIS_LUX_STRING_UTILS_H

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES BLOCK
// =============================================================================

#include "util/manifest.h"
#include <stdbool.h>
#include <stddef.h>

extern const module_metadata_t string_utils_manifest;

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Function Declarations )
// =============================================================================

void str_trim_inplace(char *str);
bool str_case_eq(const char *a, const char *b);
bool str_contains_nocase(const char *haystack, const char *needle);
void str_safe_copy(char *dst, const char *src, size_t dst_size);

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================

diag_result_t string_utils_preflight(void);
bool string_utils_audit(void);

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================

#endif /* TENEBRIS_LUX_STRING_UTILS_H */
