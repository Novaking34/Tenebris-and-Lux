/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          string_utils.c
@id            core.string
@layer         Layer 0: Core Utilities
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
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

/*
================================================================================
BLOCK 2: OPENING / STATIC DATA BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
Mission: Memory-safe string copying, concatenation, trimming, case-folding,
         FNV-1a deterministic hashing, and bounded path manipulation.
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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/string_utils.h"
#include "core/string_utils_fallback.h"
#include "core/manifest.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Local Module Manifest Anchor, Category Tags & Invariant Vectors
--------------------------------------------------------------------------------
*/

#define MODULE_ID           "core.util.string"
#define LOCAL_LOG_CAT       LOG_CAT_SYSTEM
#define LOCAL_MODULE_TAG    "STRING"

static const char *const string_invariants[] = {
    "Form precedes fullness: Safe null-termination guaranteed",
    "Buffer bounds enforcement: dst[dst_size - 1] == '\\0'",
    "Graceful NULL pointer resilience across all operations",
    "Zero dynamic heap allocations: in-place mutations only",
    "Safe parent directory extraction and bounded path joining"
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/* None. (Stateless string engine / all operations manipulate caller-provided buffers) */

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations) & Manifest Anchor
--------------------------------------------------------------------------------
*/

/* None. (Stateless string engine / zero mutable static variables) */

const module_metadata_t string_utils_manifest = {
    MODULE_ID,
    "string_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,                          /* interface_rev */
    "2026-11-09",               /* last_updated */
    "Safe string copying, concatenation, trimming, case-folding, FNV-1a hashing, and path manipulation",
    0,                          /* dependency_count: leaf foundational cog */
    { NULL },                   /* dependencies */
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    5,                          /* max_budget_us */
    5,
    {
        string_invariants[0],
        string_invariants[1],
        string_invariants[2],
        string_invariants[3],
        string_invariants[4]
    },
    string_utils_preflight
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

/**
 * @brief Fast static inline check for ASCII whitespace character.
 */
static inline bool str_is_ascii_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/*
===============================================================================
BLOCK 3: BODY / CORE STRING LOGIC BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
===============================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Memory-Safe Buffer Operations
--------------------------------------------------------------------------------
*/

void str_safe_copy(char *dst, const char *src, size_t dst_size)
{
    if (dst == NULL || dst_size == 0) {
        return;
    }

    if (src == NULL) {
        dst[0] = '\0';
        return;
    }

    size_t i = 0;
    while (i + 1 < dst_size && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

size_t str_safe_concat(char *dst, const char *src, size_t dst_size)
{
    if (dst == NULL || dst_size == 0) return 0;
    if (src == NULL) return strlen(dst);

    size_t dst_len = strlen(dst);
    size_t src_len = strlen(src);

    if (dst_len >= dst_size) {
        dst[dst_size - 1] = '\0';
        return dst_len + src_len;
    }

    size_t avail = dst_size - dst_len - 1;
    size_t copy_len = (src_len < avail) ? src_len : avail;

    memcpy(dst + dst_len, src, copy_len);
    dst[dst_len + copy_len] = '\0';

    return dst_len + src_len;
}

void str_trim_inplace(char *str)
{
    if (str == NULL || str[0] == '\0') {
        return;
    }

    char *start = str;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        str[0] = '\0';
        return;
    }

    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Case Conversion & Text Matching Cogs
--------------------------------------------------------------------------------
*/

bool str_case_eq(const char *a, const char *b)
{
    if (a == b) return true;
    if (a == NULL || b == NULL) return false;

    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

bool str_contains_nocase(const char *haystack, const char *needle)
{
    if (haystack == NULL || needle == NULL) return false;
    if (*needle == '\0') return true;

    size_t h_len = strlen(haystack);
    size_t n_len = strlen(needle);
    if (n_len > h_len) return false;

    for (size_t i = 0; i <= h_len - n_len; i++) {
        size_t j = 0;
        while (j < n_len &&
               tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j])) {
            j++;
        }
        if (j == n_len) {
            return true;
        }
    }
    return false;
}

bool str_starts_with(const char *str, const char *prefix)
{
    if (str == NULL || prefix == NULL) return false;
    size_t p_len = strlen(prefix);
    return strncmp(str, prefix, p_len) == 0;
}

bool str_ends_with(const char *str, const char *suffix)
{
    if (str == NULL || suffix == NULL) return false;
    size_t str_len = strlen(str);
    size_t s_len = strlen(suffix);
    if (s_len > str_len) return false;
    return strcmp(str + str_len - s_len, suffix) == 0;
}

bool str_to_lower(char *dst, const char *src, size_t dst_size)
{
    if (dst == NULL || src == NULL || dst_size == 0) return false;
    size_t i = 0;
    while (i + 1 < dst_size && src[i] != '\0') {
        dst[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
    return true;
}

bool str_to_upper(char *dst, const char *src, size_t dst_size)
{
    if (dst == NULL || src == NULL || dst_size == 0) return false;
    size_t i = 0;
    while (i + 1 < dst_size && src[i] != '\0') {
        dst[i] = (char)toupper((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Deterministic Hashing (FNV-1a)
--------------------------------------------------------------------------------
*/

uint32_t str_fnv1a32(const char *str)
{
    if (str == NULL) return 0;
    uint32_t hash = 0x811C9DC5;
    while (*str != '\0') {
        hash ^= (uint8_t)*str++;
        hash *= 0x01000193;
    }
    return hash;
}

uint64_t str_fnv1a64(const char *str)
{
    if (str == NULL) return 0;
    uint64_t hash = 0xCBF29CE484222325ULL;
    while (*str != '\0') {
        hash ^= (uint8_t)*str++;
        hash *= 0x100000001B3ULL;
    }
    return hash;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Path & Directory String Manipulation Cogs
--------------------------------------------------------------------------------
*/

bool str_parent_dir(char *out_dir, size_t out_size, const char *filepath)
{
    if (out_dir == NULL || out_size == 0 || filepath == NULL) {
        return false;
    }

    const char *last_slash = strrchr(filepath, '/');
    if (last_slash == NULL) {
        str_safe_copy(out_dir, ".", out_size);
        return true;
    }

    size_t dir_len = (size_t)(last_slash - filepath);
    if (dir_len == 0) {
        str_safe_copy(out_dir, "/", out_size);
        return true;
    }

    if (dir_len >= out_size) {
        debug_trap_tripwire("string", "Parent directory length exceeds buffer capacity", __FILE__, __LINE__);
        return false;
    }

    memcpy(out_dir, filepath, dir_len);
    out_dir[dir_len] = '\0';
    return true;
}

bool str_path_join(char *dest, size_t dest_size, const char *dir, const char *filename)
{
    if (dest == NULL || dest_size == 0) {
        return false;
    }
    dest[0] = '\0';

    if (filename == NULL || filename[0] == '\0') {
        if (dir != NULL) {
            str_safe_copy(dest, dir, dest_size);
            return true;
        }
        return false;
    }

    if (dir == NULL || dir[0] == '\0') {
        str_safe_copy(dest, filename, dest_size);
        return true;
    }

    size_t dir_len = strlen(dir);
    bool needs_slash = (dir_len > 0 && dir[dir_len - 1] != '/');

    int written = snprintf(dest, dest_size, "%s%s%s", dir, needs_slash ? "/" : "", filename);
    if (written < 0 || (size_t)written >= dest_size) {
        debug_trap_tripwire("string", "Joined path length exceeds destination buffer capacity", __FILE__, __LINE__);
        dest[dest_size - 1] = '\0';
        return false;
    }
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Data-Driven Lexical Configuration Cogs
--------------------------------------------------------------------------------
*/

string_config_t string_get_default_config(void)
{
    string_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    str_safe_copy(cfg.id, "default", sizeof(cfg.id));
    str_safe_copy(cfg.display_name, "Standard Continental Lexical Profile", sizeof(cfg.display_name));
    cfg.max_path_length = 512;
    cfg.max_name_length = 64;
    cfg.max_command_length = 128;
    cfg.max_line_length = 80;
    str_safe_copy(cfg.ellipsis, "...", sizeof(cfg.ellipsis));
    cfg.loaded_from_data = false;

    return cfg;
}

bool string_load_config_from_file(const char *profile_path, string_config_t *out_config)
{
    if (out_config == NULL) {
        debug_trap_tripwire("string", "NULL out_config passed to string_load_config_from_file", __FILE__, __LINE__);
        return false;
    }

    *out_config = string_get_default_config();

    const char *target_rel = profile_path;
    if (target_rel == NULL || target_rel[0] == '\0') {
        target_rel = "runtime/string/default.json";
    }

    master_loader_t loader;
    char file_buf[8192];
    if (!master_loader_load(&loader, "config", target_rel, file_buf, sizeof(file_buf))) {
        LOG_RECORD(LOG_SEV_WARNING, LOG_CAT_LOG, "String", -0.2f,
                   "Failed to load string profile '%s', falling back to defaults (GRADE:-020)", target_rel);
        return false;
    }

    master_loader_get_string(&loader, "profile", "id", out_config->id, sizeof(out_config->id));
    master_loader_get_string(&loader, "profile", "display_name", out_config->display_name, sizeof(out_config->display_name));
    master_loader_get_size_t(&loader, "profile", "max_path_length", &out_config->max_path_length);
    master_loader_get_size_t(&loader, "profile", "max_name_length", &out_config->max_name_length);
    master_loader_get_size_t(&loader, "profile", "max_command_length", &out_config->max_command_length);
    master_loader_get_size_t(&loader, "profile", "max_line_length", &out_config->max_line_length);
    master_loader_get_string(&loader, "profile", "ellipsis", out_config->ellipsis, sizeof(out_config->ellipsis));
    out_config->loaded_from_data = true;

    LOG_RECORD(LOG_SEV_DEBUG, LOG_CAT_LOG, "String", 0.0f,
               "Hydrated string profile '%s' (Path: %zu, Line: %zu) (GRADE:+000)",
               out_config->id, out_config->max_path_length, out_config->max_line_length);
    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Machine: Safe String Engine
================================================================================
*/

diag_result_t string_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = string_utils_manifest.module_id;
    res.check_name = "Buffer Bounds, Trimming & Hashing Safety";

    if (!string_utils_audit()) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Fatal: string_utils failed invariant audit.";
        return res;
    }

    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Safe copy, concatenation, trimming, case-folding & FNV-1a verified.";
    return res;
}

bool string_utils_audit(void)
{
    /* Test 1: Bounded copying and null termination */
    char buf[8];
    memset(buf, 'Z', sizeof(buf));
    str_safe_copy(buf, "1234567890", sizeof(buf));
    if (buf[7] != '\0' || strcmp(buf, "1234567") != 0) {
        return false;
    }

    /* Test 2: In-place whitespace trimming */
    char trim_buf[32];
    str_safe_copy(trim_buf, "   Tenebris and Lux   ", sizeof(trim_buf));
    str_trim_inplace(trim_buf);
    if (strcmp(trim_buf, "Tenebris and Lux") != 0) {
        return false;
    }

    /* Test 3: Safe concatenation */
    char cat_buf[16];
    str_safe_copy(cat_buf, "Light", sizeof(cat_buf));
    str_safe_concat(cat_buf, " & Dark", sizeof(cat_buf));
    if (strcmp(cat_buf, "Light & Dark") != 0) return false;

    /* Test 4: Case-insensitive equality and substring search */
    if (!str_case_eq("WARDING", "warding")) return false;
    if (str_case_eq("WARDING", "striking")) return false;
    if (!str_contains_nocase("Raising Shield of Lux", "shield")) return false;
    if (str_contains_nocase("Raising Shield of Lux", "darkness")) return false;

    /* Test 5: FNV-1a deterministic hashing */
    uint32_t h1 = str_fnv1a32("test_string");
    uint32_t h2 = str_fnv1a32("test_string");
    if (h1 == 0 || h1 != h2) return false;

    /* Test 6: Path assembly */
    char joined[64];
    if (!str_path_join(joined, sizeof(joined), "game/data", "manifest.json")) return false;
    if (strcmp(joined, "game/data/manifest.json") != 0) return false;

    char parent[64];
    if (!str_parent_dir(parent, sizeof(parent), "game/data/manifest.json")) return false;
    if (strcmp(parent, "game/data") != 0) return false;

    return true;
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

bool string_execute_dump_card(const string_config_t *config, char *out_buf, size_t buf_size)
{
    if (config == NULL || out_buf == NULL || buf_size < 300) {
        return false;
    }

    int written = snprintf(
        out_buf, buf_size,
        ".----------------------------------------------------------.\n"
        "|              LEXICAL BOUNDS INSPECTION CARD              |\n"
        "|----------------------------------------------------------|\n"
        "| Profile Identifier : %-36s|\n"
        "| Max Path Length    : %4zu bytes                             |\n"
        "| Max Name Length    : %4zu bytes                             |\n"
        "| Max Command Length : %4zu bytes                             |\n"
        "| Max Line Length    : %4zu bytes                             |\n"
        "| Data Hydration     : %-36s|\n"
        "'----------------------------------------------------------'\n",
        config->id, config->max_path_length, config->max_name_length,
        config->max_command_length, config->max_line_length,
        config->loaded_from_data ? "ACTIVE (game/data/system/runtime)" : "FALLBACK (Static Hardcoded)");

    return (written > 0 && (size_t)written < buf_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
None. (Stateless module / pure stack-allocated execution).
--------------------------------------------------------------------------------
*/

/* End of string_utils.c */
