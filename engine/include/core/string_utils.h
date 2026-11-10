/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          string_utils.h
@id            core.util.string
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Safe String Manipulation Subsystem (core/string)
@role          Bounded String Operations, FNV-1a Hashing, Path & Case Primitives
@invariants    1. Safe buffer bounds: dst[dst_size - 1] is unconditionally null-terminated.
               2. Pointer safety: Handles NULL pointers gracefully without crashes or SIGSEGV.
               3. Zero dynamic heap allocations (ALLOC_ZERO_HEAP: in-place or caller buffers).
               4. Reentrancy: Stateless, deterministic buffer transformations.
               5. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Memory-safe string copying, concatenation, in-place whitespace trimming, case-
  folding, FNV-1a hashing, and canonical path assembling. Ingests lexical limits from
  engine/config/runtime/string/ via master_loader.

  ### For Game Designers & Non-Programmers:
  Handles all text operations in the game (character names, dialogue lines,
  and inventory titles) safely. It guarantees that long text lines are cleanly
  bounded rather than spilling outside memory or crashing the game.

  ### For Systems Programmers & Engine Architects:
  - 100% Zero-Heap Memory Model (ALLOC_ZERO_HEAP): Operates entirely on caller stack.
  - Hard null-termination invariant: destination buffers are unconditionally
    null-terminated even upon truncation.
  - Deterministic 32-bit FNV-1a string hashing for O(1) identifier comparisons.
  - In-place whitespace trimming and ASCII case-folding routines.
================================================================================
*/

#ifndef TENEBRIS_LUX_CORE_STRING_UTILS_H
#define TENEBRIS_LUX_CORE_STRING_UTILS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 (Core Utilities) | Subsystem: Safe String Engine
================================================================================
Mission: Lexical buffer bounds, include ladder, fallback safety integration,
configuration ontology, and pre-conditional static validation inlines.
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
#include "core/string_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Lexical Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for text buffers and terminal commands.
For Designers: Boundaries ensure dialogue lines, character names, and console
commands never overflow their memory slots or crash the game.
For Programmers: Guarantees zero dynamic heap allocations (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Safe String Engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Safe String Engine. */
#define STRING_UTILS_MODULE_ID         "core.string_utils"

/**
 * Manifest record for the Safe String Engine.
 */
extern const module_metadata_t string_utils_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for lexical configuration profiles.
--------------------------------------------------------------------------------
*/

/**
 * @struct string_config
 * @brief Lexical buffer and token dictionary configuration profile.
 *
 * For Designers: Configuration file controlling maximum character lengths for
 * hero names, paths, and dialogue lines loaded from JSON.
 *
 * For Programmers: Loaded dynamically from engine/config/runtime/string/limits.json.
 */
typedef struct string_config {
    char   id[32];                 /**< Profile identifier string. */
    char   display_name[64];       /**< Human-readable descriptive name. */
    size_t max_path_length;        /**< Maximum filesystem path length (512). */
    size_t max_name_length;        /**< Maximum character name length (64). */
    size_t max_command_length;     /**< Maximum interactive CLI command length (128). */
    size_t max_line_length;        /**< Maximum terminal line length (80). */
    char   ellipsis[8];            /**< Truncation ellipsis string (e.g. "..."). */
    bool   loaded_from_data;       /**< true if dynamically loaded from JSON. */
} string_config_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if a string is NULL or empty ("").
 *
 * @param[in] str String pointer to inspect.
 * @return true if str is NULL or points to null terminator '\0'.
 */
static inline bool string_is_empty(const char *str) {
    return (str == NULL) || (str[0] == '\0');
}

/**
 * @brief Predicate verifying if a lexical configuration profile was loaded from JSON.
 *
 * @param[in] cfg Pointer to string configuration to inspect.
 * @return true if cfg is non-NULL and loaded_from_data is true.
 */
static inline bool string_config_is_loaded(const string_config_t *cfg) {
    return (cfg != NULL) && cfg->loaded_from_data;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE STRING API BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Memory-Safe Buffer Operations
--------------------------------------------------------------------------------
Mission: Guardrail string copying, concatenation, and trimming against memory bugs.
--------------------------------------------------------------------------------
*/

/**
 * @brief Copies a source string into a destination buffer with bounded termination.
 *
 * Guarantees that dst[dst_size - 1] is '\0' if dst != NULL and dst_size > 0.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely copies text (like player names or weapon descriptions) without overflowing memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounds-checked copy guaranteeing unconditional null termination at dst[dst_size - 1].
 *
 * @param[out] dst      Destination memory buffer.
 * @param[in]  src      Source null-terminated string (safe if NULL).
 * @param[in]  dst_size Capacity of destination buffer in bytes.
 */
void str_safe_copy(char *dst, const char *src, size_t dst_size);

/**
 * @brief Safely appends src to dst, guaranteeing unconditional null termination.
 *
 * ### For Game Designers & Non-Programmers:
 * Combines two strings together safely without crashing if the combined text is too long.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded append returning total attempted length, preventing buffer overrun.
 *
 * @param[in,out] dst      Destination buffer containing existing null-terminated text.
 * @param[in]     src      Source string to append (safe if NULL).
 * @param[in]     dst_size Total capacity of dst buffer in bytes.
 * @return Total combined length of string attempted to be created.
 */
size_t str_safe_concat(char *dst, const char *src, size_t dst_size);

/**
 * @brief Strips leading and trailing whitespace from a string in-place.
 *
 * ### For Game Designers & Non-Programmers:
 * Cleans up player input and dialogue lines by removing accidental leading or trailing spaces.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(N) in-place shift using isspace() checks with zero dynamic memory allocation.
 *
 * @param[in,out] str Null-terminated string to trim in place.
 */
void str_trim_inplace(char *str);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Case Conversion & Text Matching Cogs
--------------------------------------------------------------------------------
Mission: Case-insensitive matching, case folding, and substring search.
--------------------------------------------------------------------------------
*/

/**
 * @brief Compares two strings for equality ignoring ASCII case differences.
 *
 * ### For Game Designers & Non-Programmers:
 * Compares two words to see if they match regardless of uppercase or lowercase (e.g. "attack" matches "ATTACK").
 *
 * ### For Systems Programmers & Engine Architects:
 * Case-insensitive ASCII string comparison using tolower().
 *
 * @param[in] a First string to compare.
 * @param[in] b Second string to compare.
 * @return true if strings match case-insensitively.
 */
bool str_case_eq(const char *a, const char *b);

/**
 * @brief Searches for a case-insensitive substring occurrence within a haystack.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if a sentence or command contains a keyword regardless of casing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Substring search with case-folding matching.
 *
 * @param[in] haystack String to search within.
 * @param[in] needle   Substring to find.
 * @return true if needle occurs in haystack.
 */
bool str_contains_nocase(const char *haystack, const char *needle);

/**
 * @brief Checks if a string begins with a specific prefix.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if a line begins with a specific prefix (e.g. checking if a command begins with "/").
 *
 * ### For Systems Programmers & Engine Architects:
 * Prefix matching against null-terminated string.
 *
 * @param[in] str    String to inspect.
 * @param[in] prefix Prefix to look for.
 * @return true if str begins with prefix.
 */
bool str_starts_with(const char *str, const char *prefix);

/**
 * @brief Checks if a string ends with a specific suffix.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if a filename or text ends with an expected extension (e.g. ".json").
 *
 * ### For Systems Programmers & Engine Architects:
 * Suffix inspection comparing tail characters.
 *
 * @param[in] str    String to inspect.
 * @param[in] suffix Suffix to look for.
 * @return true if str ends with suffix.
 */
bool str_ends_with(const char *str, const char *suffix);

/**
 * @brief Converts ASCII characters to lowercase in destination buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts text to all lowercase letters for consistent command parsing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded character-by-character tolower() copy into destination buffer.
 *
 * @param[out] dst      Destination memory buffer.
 * @param[in]  src      Source string to convert.
 * @param[in]  dst_size Capacity of destination buffer in bytes.
 * @return true if converted without truncation; false if truncated or error.
 */
bool str_to_lower(char *dst, const char *src, size_t dst_size);

/**
 * @brief Converts ASCII characters to uppercase in destination buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts text to uppercase for title banners and warning displays.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded character-by-character toupper() copy into destination buffer.
 *
 * @param[out] dst      Destination memory buffer.
 * @param[in]  src      Source string to convert.
 * @param[in]  dst_size Capacity of destination buffer in bytes.
 * @return true if converted without truncation; false if truncated or error.
 */
bool str_to_upper(char *dst, const char *src, size_t dst_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Deterministic Hashing (FNV-1a)
--------------------------------------------------------------------------------
Mission: High-speed deterministic 32-bit and 64-bit string hashing.
--------------------------------------------------------------------------------
*/

/**
 * @brief Computes 32-bit FNV-1a hash of a null-terminated string.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts any text ID into a unique numeric fingerprint for lightning-fast lookup.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fowler-Noll-Vo 32-bit FNV-1a hash algorithm with zero heap allocation.
 *
 * @param[in] str Null-terminated string to hash.
 * @return 32-bit FNV-1a hash.
 */
uint32_t str_fnv1a32(const char *str);

/**
 * @brief Computes 64-bit FNV-1a hash of a null-terminated string.
 *
 * ### For Game Designers & Non-Programmers:
 * 64-bit fingerprint for collision-proof identity comparison across large asset catalogs.
 *
 * ### For Systems Programmers & Engine Architects:
 * 64-bit FNV-1a hash with 64-bit prime and offset basis.
 *
 * @param[in] str Null-terminated string to hash.
 * @return 64-bit FNV-1a hash.
 */
uint64_t str_fnv1a64(const char *str);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Path & Directory String Manipulation Cogs
--------------------------------------------------------------------------------
Mission: Safe extraction of parent directories and safe path joining.
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts the directory component of a filepath into destination buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds the folder that contains a given file path.
 *
 * ### For Systems Programmers & Engine Architects:
 * Extracts parent directory path by searching for the last '/' or '\\' separator.
 *
 * @param[out] out_dir  Destination buffer for directory path.
 * @param[in]  out_size Capacity of out_dir in bytes.
 * @param[in]  filepath Source file path string.
 * @return true if parent directory extracted successfully.
 */
bool str_parent_dir(char *out_dir, size_t out_size, const char *filepath);

/**
 * @brief Safely joins a directory path and filename with a single intervening separator.
 *
 * ### For Game Designers & Non-Programmers:
 * Joins a folder path and file name together with the correct slash between them.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded path joining inserting exactly one separator if not already present.
 *
 * @param[out] dest      Destination buffer for combined path.
 * @param[in]  dest_size Capacity of dest in bytes.
 * @param[in]  dir       Directory path.
 * @param[in]  filename  File or relative child path.
 * @return true if joined cleanly without truncation.
 */
bool str_path_join(char *dest, size_t dest_size, const char *dir, const char *filename);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Data-Driven Lexical Configuration Cogs
--------------------------------------------------------------------------------
Mission: Load lexical bounds from JSON via master_loader with hardcoded fallback.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns the hardcoded fallback lexical configuration profile.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides fallback limits for text buffer sizes when configuration files are absent.
 *
 * ### For Systems Programmers & Engine Architects:
 * Static stack-allocated string_config_t with loaded_from_data=false.
 *
 * @return Default fallback string_config_t.
 */
string_config_t string_get_default_config(void);

/**
 * @brief Loads a lexical configuration profile via master_loader.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads text length limits from data files so designers can tune dialogue constraints.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses JSON lexical limits via master_loader with fallback on error.
 *
 * @param[in]  profile_path Relative path to profile JSON (or NULL for default).
 * @param[out] out_config   Destination configuration structure.
 * @return true if loaded from file; false if fallback was triggered.
 */
bool string_load_config_from_file(const char *profile_path, string_config_t *out_config);

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
(Buffer capacity limits, unconditional null-termination contracts, and token
parsing bounds are verified via pre-flight suites and standalone unit cogs.)
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
 * @brief Executes machine pre-flight diagnostic checks on string safety invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the game's text engine to ensure strings never overflow buffers, truncate
 * improperly, or cause crashes during dialogue and name display.
 *
 * ### For Systems Programmers & Engine Architects:
 * Asserts guaranteed null-termination, bounded truncation, safe joining,
 * case-insensitive matching, path manipulation, and fallback profile loading.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t string_utils_preflight(void);

/**
 * @brief High-level boolean audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Automated pass/fail health check verifying text manipulation stability.
 *
 * ### For Systems Programmers & Engine Architects:
 * Comprehensive validation harness exercising all string safety invariant tripwires.
 *
 * @return true if all string safety invariant tripwires pass cleanly; false otherwise.
 */
bool string_utils_audit(void);

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
 * @brief Renders a 60-column terminal-friendly ASCII inspection card of string config.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing string buffer limits, maximum path lengths, and safety caps.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats a 60-column terminal-friendly status card into out_buf without dynamic heap allocation.
 *
 * @param[in]  config   Populated string_config_t pointer, or NULL for active defaults.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return True if formatted cleanly; false on error.
 */
bool string_execute_dump_card(const string_config_t *config, char *out_buf, size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

#endif /* TENEBRIS_LUX_CORE_STRING_UTILS_H */
