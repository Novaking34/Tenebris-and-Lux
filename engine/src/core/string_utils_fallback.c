/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          string_utils_fallback.c
@id            core.string.fallback
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Safe String Manipulation Subsystem (core/string)
@role          Implementation of Quarantined Monolithic Emergency Fallback
@invariants    1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. Monolith code page is strictly isolated from normal production initialization.
               4. Profile identifier is explicitly stamped as STRING_FALLBACK_PROFILE_NAME.
               5. loaded_from_data is unconditionally false.
               6. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the monolithic default code page and tripwire fallback
  for the safe string manipulation engine. Serves as an isolated, compile-time
  fallback baseline quarantined outside main production pathways to guarantee
  that production uses the data-driven pipeline.

  ### For Game Designers & Non-Programmers:
  If the text and dialogue limits fail to load from data files, this code page
  enforces standard text length rules (80-character line discipline, 64-character
  character names) so text boxes never wrap awkwardly or clip.

  ### For Systems Programmers & Engine Architects:
  - 100% Static Compile-Time Constants: Zero dynamic heap allocation.
  - Safe null-termination guaranteed: dst[dst_size - 1] = '\0'.
  - Stack-allocated immutable string_fallback_config_t structure.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & DEPENDENCIES
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
Mission: Quarantined fallback configuration and compile-time string manipulation safeguards.
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Local Module Manifest Anchor, Category Tags & Invariant Vectors
--------------------------------------------------------------------------------
*/

#define MODULE_ID           "core.util.string.fallback"
#define LOCAL_LOG_CAT       LOG_CAT_SYSTEM
#define LOCAL_MODULE_TAG    "STRING_FALLBACK"

/*
Invariants:
1. Zero dynamic heap allocations in Layer 0 (ALLOC_ZERO_HEAP)
2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic)
3. Monolith code page strictly quarantined from production pipeline
4. Profile identifier explicitly stamped as STRING_FALLBACK_PROFILE_NAME
5. loaded_from_data is unconditionally false
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/* None. (Fallback types declared in core/string_utils_fallback.h) */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations) & Manifest Anchor
--------------------------------------------------------------------------------
*/

/* None. (Stateless module / zero mutable static variables) */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

/**
 * @brief Fast static inline check for non-zero buffer capacity.
 */
static inline bool string_fallback_is_valid_size(size_t size)
{
    return size > 0U;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CODE PAGE IMPLEMENTATION
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Configuration & Safe String Implementation
--------------------------------------------------------------------------------
*/

string_fallback_config_t string_fallback_get_config(void)
{
    /* Step 1: Zero-initialize value struct on stack */
    string_fallback_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    /* Step 2: Assign fallback profile metadata */
    strncpy(cfg.id, STRING_FALLBACK_PROFILE_NAME, sizeof(cfg.id) - 1);
    strncpy(cfg.display_name, "Canonical Lexical Limits", sizeof(cfg.display_name) - 1);

    /* Step 3: Populate canonical lexical thresholds */
    cfg.max_path_len = STRING_FALLBACK_MAX_PATH;
    cfg.max_name_len = STRING_FALLBACK_MAX_NAME;
    cfg.max_command_len = STRING_FALLBACK_MAX_COMMAND;
    cfg.max_line_len = STRING_FALLBACK_MAX_LINE;

    /* Step 4: Return immutable stack configuration */
    return cfg;
}

void string_fallback_copy(char *dst, const char *src, size_t dst_size)
{
    /* Step 1: Reject NULL destination or degenerate size */
    if (dst == NULL || dst_size == 0) return;

    /* Step 2: Handle NULL source pointer safely */
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }

    /* Step 3: Copy bounded characters into destination */
    size_t i = 0;
    while (i + 1 < dst_size && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }

    /* Step 4: Guarantee unconditional null-termination */
    dst[i] = '\0';
}

void string_fallback_trim(char *str)
{
    /* Step 1: Reject NULL or empty string */
    if (str == NULL || str[0] == '\0') return;

    /* Step 2: Trim leading ASCII whitespace */
    char *start = str;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    /* Step 3: Shift trimmed text to buffer origin if necessary */
    if (start != str) {
        size_t len = strlen(start);
        memmove(str, start, len + 1);
    }

    /* Step 4: Trim trailing whitespace */
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

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

diag_result_t string_fallback_preflight(void)
{
    diag_result_t res;
    res.subsystem = "core.util.string_fallback";
    res.check_name = "String Emergency Fallback Invariant Integrity";

    /* Test 1: Validate static boundaries */
    string_fallback_config_t cfg = string_fallback_get_config();
    if (cfg.max_path_len != 512 || cfg.max_name_len != 64 ||
        cfg.max_command_len != 128 || cfg.max_line_len != 80) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: String fallback limits violate canonical 80-column discipline.";
        return res;
    }

    /* Test 2: Bounded copy and null-termination tripwire */
    char buf[8];
    string_fallback_copy(buf, "1234567890", sizeof(buf));
    if (strlen(buf) != 7 || buf[7] != '\0') {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: String fallback copy failed bounded null-termination.";
        return res;
    }

    /* Test 3: In-place trimming */
    char trim_buf[32];
    string_fallback_copy(trim_buf, "   hello   ", sizeof(trim_buf));
    string_fallback_trim(trim_buf);
    if (strcmp(trim_buf, "hello") != 0) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: String fallback trim failed whitespace stripping.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "String fallback constants, bounded copy and trim verified.";
    return res;
}

bool string_fallback_audit(void)
{
    diag_result_t pf = string_fallback_preflight();
    return pf.passed && (pf.severity != SEVERITY_FATAL);
}

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
--------------------------------------------------------------------------------
*/

bool string_fallback_execute_dump_card(const string_fallback_config_t *cfg,
                                       char *out_buf,
                                       size_t buf_size)
{
    if (cfg == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|             FALLBACK STRING INVARIANT METRICS            |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Max Path Length    : %4zu bytes                             |\n"
        "| Max Name Length    : %4zu bytes                             |\n"
        "| Max Command Length : %4zu bytes                             |\n"
        "| Max Line Length    : %4zu bytes (80-column)                 |\n"
        "'----------------------------------------------------------'\n",
        cfg->id, cfg->max_path_len, cfg->max_name_len,
        cfg->max_command_len, cfg->max_line_len);

    return (written > 0 && (size_t)written < buf_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

/* End of string_utils_fallback.c */
