/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          string_utils_fallback.h
@id            core.util.string_fallback
@layer         Layer 0: Core Utilities
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Safe String Manipulation Subsystem (core/string)
@role          Quarantined Monolithic Emergency Default Code Page & String Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Quarantined execution path: Never invoked in normal production.
               4. Provenance tracking: loaded_from_data is unconditionally false.
               5. Canonical lexical limits: 512 max path, 64 max name, 80 column line discipline.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Isolated, monolithic emergency default code page and tripwire fallback for
  the safe string manipulation engine. Quarantined strictly outside main production
  pathways to guarantee that production uses the data-driven pipeline. Provides
  compile-time static lexical limits, fallback bounded string copies, and in-place
  whitespace trimming routines when external assets or data manifests are offline.

  ### For Game Designers & Non-Programmers:
  If the text and dialogue limits fail to load from data files, this code page
  enforces standard text length rules (80-character line discipline, 64-character
  character names) so text boxes never wrap awkwardly or clip.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Safe null-termination guaranteed: `dst[dst_size - 1] = '\0'`.
  - Stack-allocated immutable string_fallback_config_t structure.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_STRING_UTILS_FALLBACK_H
#define TENEBRIS_LUX_CORE_STRING_UTILS_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & STATIC CONFIGURATION CONSTANTS
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
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
Sub-Assembly 2.1: Pre-Conditions & Fallback Lexical Constants
--------------------------------------------------------------------------------
*/

/** Canonical profile identifier for fallback string limits. */
#define STRING_FALLBACK_PROFILE_NAME    "string_canonical_fallback"

/** Canonical maximum filesystem path buffer length in bytes (512). */
#define STRING_FALLBACK_MAX_PATH        512

/** Canonical maximum entity / item / hero identifier length in bytes (64). */
#define STRING_FALLBACK_MAX_NAME        64

/** Canonical maximum interactive terminal command string length (128). */
#define STRING_FALLBACK_MAX_COMMAND     128

/** Canonical maximum terminal / HUD line length for 80-column discipline (80). */
#define STRING_FALLBACK_MAX_LINE        80

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Fallback Lexical Data Structures
--------------------------------------------------------------------------------
*/

/**
 * @brief Self-contained fallback configuration structure for lexical limits.
 */
typedef struct string_fallback_config {
    char   id[32];               /**< Fallback profile identifier. */
    char   display_name[64];     /**< Human-readable descriptive name. */
    size_t max_path_len;         /**< Maximum allowable path length. */
    size_t max_name_len;         /**< Maximum allowable name length. */
    size_t max_command_len;      /**< Maximum allowable CLI command length. */
    size_t max_line_len;         /**< Maximum terminal line length. */
} string_fallback_config_t;

/*
================================================================================
BLOCK 3: BODY BLOCK / EXPORTED FALLBACK COGS
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration & Safe String Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs and returns the static fallback lexical configuration.
 *
 * @return Fully-populated string_fallback_config_t structure on stack.
 */
string_fallback_config_t string_fallback_get_config(void);

/**
 * @brief Zero-dependency safe string copying with guaranteed null-termination.
 *
 * @param[out] dst      Destination character buffer.
 * @param[in]  src      Source null-terminated string (or NULL).
 * @param[in]  dst_size Capacity of destination buffer in bytes.
 */
void string_fallback_copy(char *dst, const char *src, size_t dst_size);

/**
 * @brief In-place ASCII whitespace trimming without engine dependencies.
 *
 * @param[in,out] str Null-terminated string to modify in place.
 */
void string_fallback_trim(char *str);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / TRIPWIRES
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Machine Pre-Flight Tripwire & Self-Audit Procedures
--------------------------------------------------------------------------------
*/

/**
 * @brief Pre-flight diagnostic check evaluating fallback lexical invariants.
 *
 * @return Diagnostic check outcome reporting PASS or FATAL.
 */
diag_result_t string_fallback_preflight(void);

/**
 * @brief Standalone boolean audit wrapper for regression test harnesses.
 *
 * @return True if all lexical fallback tripwires pass cleanly.
 */
bool string_fallback_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / DIAGNOSTIC CARD DUMP & TEARDOWN
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Diagnostic ASCII Status Card Generator
--------------------------------------------------------------------------------
*/

/**
 * @brief Formats an ASCII diagnostic inspection card for fallback lexical state.
 */
bool string_fallback_execute_dump_card(const string_fallback_config_t *cfg,
                                       char *out_buf,
                                       size_t buf_size);

#endif /* TENEBRIS_LUX_CORE_STRING_UTILS_FALLBACK_H */
