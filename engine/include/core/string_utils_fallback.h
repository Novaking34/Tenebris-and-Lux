/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          string_utils_fallback.h
@id            core.util.string_fallback
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
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
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Safe String Engine
================================================================================
Mission: Quarantined emergency fallback limits, include ladder, and static
helper predicates for string operations when lexical config data is offline.
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
Sub-Assembly 2.1: Pre-Conditions & Fallback Lexical Constants
--------------------------------------------------------------------------------
Compile-time static defaults for emergency boot and headless string testing.
For Designers: Boundaries ensure character names (up to 64 chars), file paths
(up to 512 chars), and text lines (up to 80 chars) never overflow their bounds.
For Programmers: Immutable constants residing in .rodata with zero heap allocation.
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
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the fallback code page.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the String Utilities Fallback subsystem. */
#define STRING_UTILS_FALLBACK_MODULE_ID "core.string_utils.fallback"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Fallback Object Definitions
--------------------------------------------------------------------------------
Self-contained fallback profile descriptors and configuration structures.
--------------------------------------------------------------------------------
*/

/**
 * @struct string_fallback_config
 * @brief Self-contained fallback configuration structure for lexical limits.
 *
 * For Designers: An emergency record defining maximum text lengths so dialogues
 * and file paths always fit safely in memory.
 *
 * For Programmers: Invariant loaded_from_data is strictly false.
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
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline inspection predicates for the fallback code page.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a string length fits strictly within buffer capacity.
 *
 * @param[in] len     String length in bytes (excluding null terminator).
 * @param[in] max_len Buffer capacity in bytes.
 * @return true if len is strictly less than max_len.
 */
static inline bool string_fallback_len_is_valid(size_t len, size_t max_len) {
    return len < max_len;
}

/**
 * @brief Predicate verifying if an inspected profile name matches the emergency fallback.
 *
 * @param[in] name Profile name string to compare.
 * @return true if name matches STRING_FALLBACK_PROFILE_NAME; false otherwise.
 */
static inline bool string_fallback_is_profile_name(const char *name) {
    if (name == NULL) {
        return false;
    }
    const char *expected = STRING_FALLBACK_PROFILE_NAME;
    while (*name && *expected) {
        if (*name != *expected) {
            return false;
        }
        name++;
        expected++;
    }
    return *name == *expected;
}

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
Mission: Emergency fallback text buffer bounds, safe copying, and trimming.
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs and returns the static fallback lexical configuration.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides standard fallback character length limits (paths: 512, names: 64, lines: 80)
 * when text configuration manifests are offline.
 *
 * ### For Systems Programmers & Engine Architects:
 * Reentrant, zero-heap initialization of stack-allocated string_fallback_config_t.
 *
 * @return Fully-populated string_fallback_config_t structure on stack.
 */
string_fallback_config_t string_fallback_get_config(void);

/**
 * @brief Zero-dependency safe string copying with guaranteed null-termination.
 *
 * ### For Game Designers & Non-Programmers:
 * Emergency text copy ensuring character names or errors do not overflow their storage.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded copy enforcing unconditional null termination at dst[dst_size - 1].
 *
 * @param[out] dst      Destination character buffer.
 * @param[in]  src      Source null-terminated string (or NULL).
 * @param[in]  dst_size Capacity of destination buffer in bytes.
 */
void string_fallback_copy(char *dst, const char *src, size_t dst_size);

/**
 * @brief In-place ASCII whitespace trimming without engine dependencies.
 *
 * ### For Game Designers & Non-Programmers:
 * Removes accidental spaces at the beginning and end of emergency text strings.
 *
 * ### For Systems Programmers & Engine Architects:
 * In-place memory shift without dynamic memory allocation or external libraries.
 *
 * @param[in,out] str Null-terminated string to modify in place.
 */
void string_fallback_trim(char *str);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Safe String Engine
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
(Fallback static text limits, emergency buffer sizes, and profile identity
invariants are verified via pre-flight diagnostics in Sub-Assembly 4.2.)
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
 * @brief Pre-flight diagnostic check evaluating fallback lexical invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the emergency text backup routines to make sure safe copying and trimming
 * work properly even if configuration files are missing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Asserts zero-heap stack buffer safety, unconditional null-termination,
 * loaded_from_data == false, and profile identifier matching.
 *
 * @return diag_result_t reporting categorized diagnostic outcome.
 */
diag_result_t string_fallback_preflight(void);

/**
 * @brief Standalone boolean audit wrapper for regression test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated check ensuring emergency text backup code is 100% operational.
 *
 * ### For Systems Programmers & Engine Architects:
 * High-level boolean validation audit asserting that all lexical fallback tripwires pass.
 *
 * @return true if all lexical fallback tripwires pass cleanly; false otherwise.
 */
bool string_fallback_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Safe String Engine
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
 * @brief Formats an ASCII diagnostic inspection card for fallback lexical state.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing emergency string buffer limits and path capacities.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats a 60-column terminal-friendly status card into out_buf without dynamic heap allocation.
 *
 * @param[in]  cfg      Fallback configuration pointer, or NULL for static defaults.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return True if inspection card was formatted cleanly; false on error.
 */
bool string_fallback_execute_dump_card(const string_fallback_config_t *cfg,
                                       char *out_buf,
                                       size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_STRING_UTILS_FALLBACK_H */
