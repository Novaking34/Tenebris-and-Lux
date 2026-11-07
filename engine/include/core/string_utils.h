/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          string_utils.h
@id            core.util.string
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Safe String Manipulation Subsystem (core/string)
@role          Bounded String Operations, FNV-1a Hashing, Path & Case Primitives
@invariants    1. Safe buffer bounds: dst[dst_size - 1] is unconditionally null-terminated.
               2. Pointer safety: Handles NULL pointers gracefully without crashes or SIGSEGV.
               3. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: in-place or caller buffers).
               4. Reentrancy: Stateless, deterministic buffer transformations.

PURPOSE & ARCHITECTURAL ROLE:
  Memory-safe string copying, concatenation, in-place whitespace trimming, case-
  folding, FNV-1a hashing, and canonical path assembling. Ingests lexical limits from
  game/data/system/runtime/string/ via master_loader.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_STRING_UTILS_H
#define TENEBRIS_LUX_CORE_STRING_UTILS_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

/* --- Tier 1: Standard Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: Internal Core Headers --- */
#include "core/manifest.h"

/**
 * @brief Lexical buffer and token dictionary configuration profile.
 */
typedef struct string_config {
    char id[32];
    char display_name[64];
    size_t max_path_length;
    size_t max_name_length;
    size_t max_command_length;
    size_t max_line_length;
    char ellipsis[8];
    bool loaded_from_data;
} string_config_t;

/**
 * @brief Manifest record for the Safe String Engine.
 */
extern const module_metadata_t string_utils_manifest;

/*
================================================================================
BLOCK 3: BODY / CORE STRING API BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.1: MEMORY-SAFE BUFFER OPERATIONS ]
Mission: Guardrail string copying, concatenation, and trimming against memory bugs.
--------------------------------------------------------------------------------
*/

/**
 * @brief Copies a source string into a destination buffer with bounded termination.
 *
 * Guarantees that dst[dst_size - 1] is '\0' if dst != NULL and dst_size > 0.
 *
 * @param[out] dst      Destination memory buffer.
 * @param[in]  src      Source null-terminated string (safe if NULL).
 * @param[in]  dst_size Capacity of destination buffer in bytes.
 */
void str_safe_copy(char *dst, const char *src, size_t dst_size);

/**
 * @brief Safely appends src to dst, guaranteeing unconditional null termination.
 *
 * @param[in,out] dst      Destination buffer containing existing null-terminated text.
 * @param[in]     src      Source string to append (safe if NULL).
 * @param[in]     dst_size Total capacity of dst buffer in bytes.
 * @return Total combined length of string attempted to be created.
 */
size_t str_safe_concat(char *dst, const char *src, size_t dst_size);

/**
 * @brief Strips leading and trailing whitespace from a string in-place.
 */
void str_trim_inplace(char *str);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.2: CASE CONVERSION & TEXT MATCHING COGS ]
Mission: Case-insensitive matching, case folding, and substring search.
--------------------------------------------------------------------------------
*/

/**
 * @brief Compares two strings for equality ignoring ASCII case differences.
 */
bool str_case_eq(const char *a, const char *b);

/**
 * @brief Searches for a case-insensitive substring occurrence within a haystack.
 */
bool str_contains_nocase(const char *haystack, const char *needle);

/**
 * @brief Checks if a string begins with a specific prefix.
 */
bool str_starts_with(const char *str, const char *prefix);

/**
 * @brief Checks if a string ends with a specific suffix.
 */
bool str_ends_with(const char *str, const char *suffix);

/**
 * @brief Converts ASCII characters to lowercase in destination buffer.
 */
bool str_to_lower(char *dst, const char *src, size_t dst_size);

/**
 * @brief Converts ASCII characters to uppercase in destination buffer.
 */
bool str_to_upper(char *dst, const char *src, size_t dst_size);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.3: DETERMINISTIC HASHING (FNV-1a) ]
Mission: High-speed deterministic 32-bit and 64-bit string hashing.
--------------------------------------------------------------------------------
*/

/**
 * @brief Computes 32-bit FNV-1a hash of a null-terminated string.
 */
uint32_t str_fnv1a32(const char *str);

/**
 * @brief Computes 64-bit FNV-1a hash of a null-terminated string.
 */
uint64_t str_fnv1a64(const char *str);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.4: PATH & DIRECTORY STRING MANIPULATION COGS ]
Mission: Safe extraction of parent directories and safe path joining.
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts the directory component of a filepath into destination buffer.
 */
bool str_parent_dir(char *out_dir, size_t out_size, const char *filepath);

/**
 * @brief Safely joins a directory path and filename with a single intervening separator.
 */
bool str_path_join(char *dest, size_t dest_size, const char *dir, const char *filename);

/*
--------------------------------------------------------------------------------
[ SUB-ASSEMBLY 3.5: DATA-DRIVEN LEXICAL CONFIGURATION COGS ]
Mission: Load lexical bounds from JSON via master_loader with hardcoded fallback.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns the hardcoded fallback lexical configuration profile.
 */
string_config_t string_get_default_config(void);

/**
 * @brief Loads a lexical configuration profile via master_loader.
 *
 * @param[in]  profile_path Relative path to profile JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return true if loaded from file; false if fallback was triggered.
 */
bool string_load_config_from_file(const char *profile_path, string_config_t *out_config);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on string safety invariants.
 */
diag_result_t string_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 */
bool string_utils_audit(void);

/*
================================================================================
BLOCK 5: CLOSING & DIAGNOSTIC CARD DUMP BLOCK
================================================================================
*/

/**
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of string config.
 */
bool string_execute_dump_card(const string_config_t *config, char *out_buf, size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_STRING_UTILS_H */
