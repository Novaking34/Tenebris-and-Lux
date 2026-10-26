// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          string_utils.h
// @layer         Core Utilities Layer (Layer 0)
// @version       0.0.1
// @epoch         2026.10
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @role          Safe String Operations, In-Place Trimming & Token Parsing
// @invariants    Rank 1 (< >): Form precedes fullness.
//                1. Memory safety: all operations are null-terminated and bounded.
//                2. Zero heap allocations: in-place mutations only.
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
