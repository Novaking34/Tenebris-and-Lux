/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/src/io/io_fallback.c
@id            cog.io.fallback
@layer         Layer 0: Core Utilities
@archetype     quarantined_fallback | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Tier-0 BIOS Safe-Mode I/O Engine & Compiled ROM Vault
@role          Runtime Engine & Chassis: Emergency FS/JSON/VFS/Loader handlers,
               8 compiled I/O ROM payloads in .rodata, and floppy Safe-Mode state.
@maturity      Rev 5 Baseline | Zero-disk boot, 40.3 KiB I/O ROM vault & shared policy parser verified.
@chassis       zero-heap | .bss 8,312 B (s_io_fb) + .rodata 40,256 B (8 I/O ROMs) + stack <= 1 KiB.
@scale         O(n) POSIX/ROM byte copy & JSON key extraction bounded by max_config_bytes; O(1) ROM index.
@seams         in: io_config_rom.h | uses: cog.log.fallback, cog.debug.fallback | probes: io_fallback.* -> DEBUG_STEP_AWAIT | dag: io.fallback_bios
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Virtual "rom://" streams read strictly from immutable ELF .rodata without filesystem syscalls.
               3. Read-only floppy latch rejects write/mkdir paths while preserving Safe-Mode audit counters.
               4. io_fallback_preflight() verifies all 8 I/O ROM schemas cleanly without clobbering active s_io_fb.

PURPOSE & ARCHITECTURAL ROLE:
  Executes the Tier-0 Safe-Mode I/O BIOS across all four emergency storage
  pillars, serves "rom://" virtual streams from compiled io_config_rom.h data,
  and furnishes the shared canonical policy JSON parser used by primary io/.

  ### For Game Designers & Non-Programmers:
  Protects saves and boot stability when drives are write-locked or folders go
  missing: built-in ROM configs satisfy required reads seamlessly while unsafe
  writes are cleanly blocked instead of corrupting files.

  ### For Systems Programmers & Engine Architects:
  - Controlled Memory Chassis: static singleton s_io_fb measures exactly 8,312 B
    in .bss alongside 40,256 B .rodata housing all 8 embedded engine/config/io/ ROMs.
  - Bottleneck & Headroom: Emergency parser executes zero-heap single-pass token
    extraction cleanly within caller buffers without auxiliary scratch bloat.
  - Probe & Config Wiring: Exposes io_fallback_register_debug_probes() and
    shares io_fallback_parse_policy_json() directly with cog.fs_utils.
================================================================================
*/

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/*
================================================================================
BLOCK 2: OPENING BLOCK
( BIOS Dependencies, Safe-Mode Chassis, Config ROM Vault & O(1) Guards )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.io.fallback
Mission:       Wire BIOS deps and io_config_rom.h vault, house Safe-Mode
               chassis in 2.3, and guard ROM/disk fallback.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None in Layer 0/1)
Tier 3: Internal Engine Foundation Headers & Config ROM Vault
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers & Config ROM Vault --- */
#include "io/io_fallback.h"
#include "core/manifest.h"
#include "io_config_rom.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Limits & Invariant Assertions
--------------------------------------------------------------------------------
*/
/** @brief Canonical module identifier bound in manifests and fault telemetry. */
#define MODULE_ID                IO_FALLBACK_MODULE_ID
/** @brief Default telemetry channel for Tier-0 I/O BIOS diagnostic events. */
#define LOCAL_LOG_CAT            LOG_CAT_IO
/** @brief Human-readable source tag for Tier-0 I/O BIOS messages. */
#define LOCAL_MODULE_TAG         "IO_Fallback"
/** @brief Maximum ROM/JSON config buffer span tolerated by Tier-0 scanners (64 KiB). */
#define IO_FB_JSON_MAX_BYTES     65536U

static_assert(
    IO_FB_FAULT_RING_CAP >= 4U && IO_FB_FAULT_RING_CAP <= 128U,
    "IO_FB_FAULT_RING_CAP must remain bounded inside Tier-0 .bss chassis"
);
static_assert(
    IO_FB_JSON_MAX_BYTES >= IO_FB_MAX_CONFIG_BYTES,
    "IO_FB_JSON_MAX_BYTES must house canonical Safe-Mode IO config payloads"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Probe Forward Declarations
--------------------------------------------------------------------------------
*/

/**
 * @struct io_fallback_state
 * @brief Consolidated Layer 0 BIOS singleton storing policy, VFS root, and faults.
 *
 * ### For Game Designers & Non-Programmers:
 * Holds the active safety limits, floppy read-only switch, and recent file fault notes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed `.bss` layout (< 4 KB total), eliminating scattered module globals and
 * keeping runtime buffers bounded for Controlled Memory Chassis telemetry.
 */
typedef struct io_fallback_state {
    bool                      booted;                              /**< True once BIOS boot sequence ran. */
    bool                      rom_hydrated;                        /**< True if .rodata ROM preset loaded. */
    bool                      in_fault_record;                     /**< Reentrancy guard inside fault push. */
    bool                      in_preflight;                        /**< Audit suppression guard flag. */
    io_fallback_sink_t        active_sink;                         /**< Active emergency output mode. */
    io_fallback_config_t      config;                              /**< Active I/O Safe-Mode policy copy. */
    io_fb_vfs_routes_t        vfs_routes;                          /**< Active VFS route search matrix. */
    io_fb_loader_contract_t   loader_contract;                     /**< Active 5-block loader policy copy. */
    char                      active_root[IO_FB_MAX_ROOT_LEN];     /**< Active resolved VFS mount root. */
    uint64_t                  fault_sequence;                      /**< Monotonic BIOS fault sequence ID. */
    uint64_t                  rom_hits;                            /**< Cumulative compiled ROM reads. */
    uint64_t                  read_only_blocks;                    /**< Blocked writes in read-only mode. */
    uint64_t                  safe_downgrades;                     /**< Total Safe-Mode downgrades tracked. */
    size_t                    fault_head;                          /**< Circular FIFO fault ring head. */
    size_t                    fault_count;                         /**< Valid faults stored in ring. */
    io_fallback_fault_entry_t faults[IO_FB_FAULT_RING_CAP];        /**< Fixed pre-init BIOS fault ring. */
} io_fallback_state_t;

/* Forward declarations binding Sub-Assembly 2.3 manifest and Block 4 probes. */
diag_result_t io_fallback_preflight(void);
static diag_result_t io_fb_probe_rom_integrity(void);
static diag_result_t io_fb_probe_policy_bounds(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.rodata & .bss Allocations)
Mission: Keep fixed zero-heap chassis buffers, manifest, and probes in 2.3.
--------------------------------------------------------------------------------
*/

/** @brief Immutable architectural invariants registered in engine manifest (.rodata). */
static const char *const s_io_fb_invariants[5] = {
    "Zero dynamic heap allocation across all BIOS I/O pillars (ALLOC_ZERO_HEAP)",
    "Zero-disk boot & virtual rom:// reads from compiled .rodata io_config_rom.h",
    "Safe-Mode & ROM configs preserve loaded_from_data == false unconditionally",
    "Read-only floppy latch blocks destructive disk writes cleanly",
    "Canonical shared policy parser eliminates duplicate JSON extraction paths"
};

/** @brief Public module metadata node registering Safe-Mode I/O BIOS in manifest. */
const module_metadata_t io_fallback_manifest = {
    IO_FALLBACK_MODULE_ID,
    "io_fallback.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-11",
    "Tier-0 BIOS Safe-Mode I/O engine and compiled .rodata Config ROM vault",
    1,
    { "core.util.string" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    20,
    5,
    {
        s_io_fb_invariants[0],
        s_io_fb_invariants[1],
        s_io_fb_invariants[2],
        s_io_fb_invariants[3],
        s_io_fb_invariants[4]
    },
    io_fallback_preflight
};

/** @brief Immutable baseline Safe-Mode I/O policy template (.rodata). */
static const io_fallback_config_t s_io_fallback_template = {
    .small_bytes = IO_FB_SMALL_BYTES,
    .medium_bytes = IO_FB_MEDIUM_BYTES,
    .large_bytes = IO_FB_LARGE_BYTES,
    .max_config_bytes = IO_FB_MAX_CONFIG_BYTES,
    .path_max_bytes = IO_FB_PATH_MAX,
    .max_json_depth = IO_FB_MAX_JSON_DEPTH,
    .max_json_tokens = IO_FB_MAX_JSON_TOKENS,
    .reject_on_truncation = true,
    .require_5block_envelope = true,
    .enforce_schema_id = true,
    .rom_fallback_enabled = true,
    .read_only_mode = false,
    .probe_await_on_corrupt = true,
    .posix_dir_mode = IO_FB_DEFAULT_DIR_MODE,
    .flush_on_write = true,
    .atomic_write = false,
    .block_path_traversal = true,
    .max_symlink_depth = IO_FB_DEFAULT_SYMLINK_DEPTH,
    .allow_mod_mounts = true,
    .strict_utf8_escapes = false,
    .reject_duplicate_keys = false,
    .hot_reload_enabled = false,
    .default_root = IO_FB_DEFAULT_ROOT,
    .profile_name = IO_FALLBACK_PROFILE_NAME,
    .loaded_from_data = false
};

static const io_fallback_config_t s_io_floppy_template = {
    .small_bytes = 512U,
    .medium_bytes = 4096U,
    .large_bytes = 16384U,
    .max_config_bytes = 32768U,
    .path_max_bytes = 256U,
    .max_json_depth = 16,
    .max_json_tokens = 512U,
    .reject_on_truncation = true,
    .require_5block_envelope = true,
    .enforce_schema_id = true,
    .rom_fallback_enabled = true,
    .read_only_mode = true,
    .probe_await_on_corrupt = false,
    .posix_dir_mode = 0555U,
    .flush_on_write = false,
    .atomic_write = false,
    .block_path_traversal = true,
    .max_symlink_depth = 1,
    .allow_mod_mounts = false,
    .strict_utf8_escapes = false,
    .reject_duplicate_keys = false,
    .hot_reload_enabled = false,
    .default_root = IO_FB_DEFAULT_ROOT,
    .profile_name = IO_FALLBACK_FLOPPY_PROFILE_NAME,
    .loaded_from_data = false
};

/* Minimal zero-ROM emergency seed routes used only if io/vfs_routes.json ROM is absent */
static const char *const s_io_fb_seed_candidates[4] = {
    "game/data",
    "../game/data",
    "engine/config",
    "../engine/config"
};

static const char *const s_io_fb_seed_domains[6] = {
    "",
    "io/",
    "runtime/",
    "diagnostics/",
    "mechanics/",
    "world/"
};

/** Consolidated Layer 0 I/O BIOS state singleton in .bss (~7.5 KB zero-heap). */
static io_fallback_state_t s_io_fb;

/** Compiled `.rodata` validation probe descriptors bound cleanly in Block 4. */
static const debug_validation_probe_t s_io_fb_probes[2] = {
    {
        "io.fallback.rom_integrity",
        IO_FALLBACK_MODULE_ID,
        "io_fallback.c:Block4:io_fallback_verify_rom_envelopes",
        "tests/unit/io/test_io_fallback_unit.c:Section4",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_AUDIT_BREACH,
        DEBUG_REACTION_PANIC_HALT,
        io_fb_probe_rom_integrity,
        true,
        true,
        0U
    },
    {
        "io.fallback.policy_bounds",
        IO_FALLBACK_MODULE_ID,
        "io_fallback.c:Block4:io_fallback_validate_policy_contract",
        "tests/unit/io/test_io_fallback_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_PANIC_HALT,
        io_fb_probe_policy_bounds,
        true,
        true,
        0U
    }
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional BIOS Inline Predicates & Guards
--------------------------------------------------------------------------------
Pure O(1) inline range predicates operating safely before Block 3 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Validates in O(1) time that `[start, end)` defines a non-empty bounded span.
 *
 * ### For Game Designers & Non-Programmers:
 * Guards emergency settings scanners against empty or reversed memory slices.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure non-mutating pointer-pair check enforcing strict `[start, end)` bounds.
 *
 * @param start Inclusive span start pointer.
 * @param end   Exclusive span end pointer.
 * @return True if both pointers are non-NULL and `start < end`.
 */
CS_NODISCARD static inline bool io_fb_span_has_bytes(const char *start, const char *end)
{
    return (start != NULL && end != NULL && start < end);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.5: Monitored BIOS Support Infrastructure (Non-Inline Internal)
--------------------------------------------------------------------------------
Support Role:  Tier-0 JSON span scanners, octal parser, traversal guards, and
               Safe-Mode VFS/Loader table seeders shared by ROM and disk paths.
Pattern Watch: Quarantined BIOS scanners mirror simplified json_utils.c span logic
               to preserve zero downward coupling; never leak BIOS parsers upward,
               and elevate any general non-BIOS path rules if shared across modules.
Linkage Rule:  Strictly static functions only; zero static arrays or .bss storage.
--------------------------------------------------------------------------------
*/

static const char *io_fb_skip_ws(const char *p, const char *end)
{
    while (io_fb_span_has_bytes(p, end) && io_fb_is_ws(*p)) {
        p++;
    }
    return p;
}

static bool io_fb_is_unescaped_quote(const char *span_start, const char *p)
{
    if (p == NULL || *p != '"') {
        return false;
    }
    size_t backslashes = 0U;
    const char *cursor = p;
    while (cursor > span_start && *(cursor - 1) == '\\') {
        backslashes++;
        cursor--;
    }
    return (backslashes % 2U) == 0U;
}

static bool io_fb_has_parent_traversal(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return false;
    }
    const char *p = path;
    while (*p != '\0') {
        if ((p == path || *(p - 1) == '/' || *(p - 1) == '\\') &&
            p[0] == '.' && p[1] == '.' &&
            (p[2] == '\0' || p[2] == '/' || p[2] == '\\')) {
            return true;
        }
        p++;
    }
    return false;
}

static bool io_fb_parse_octal_mode(const char *oct_str, unsigned int *out_mode)
{
    if (oct_str == NULL || oct_str[0] == '\0' || out_mode == NULL) {
        return false;
    }
    char *endptr = NULL;
    errno = 0;
    unsigned long parsed = strtoul(oct_str, &endptr, 8);
    if (endptr == oct_str || *endptr != '\0' || errno == ERANGE || parsed > 0777UL) {
        return false;
    }
    *out_mode = (unsigned int)parsed;
    return true;
}

static const char *io_fb_strip_path_prefixes(const char *path)
{
    const char *p = path;
    if (p == NULL) {
        return "";
    }
    if (io_fb_is_rom_uri(p)) {
        p += 6;
    }
    if (strncmp(p, "../engine/config/", 17) == 0) {
        p += 17;
    } else if (strncmp(p, "engine/config/", 14) == 0) {
        p += 14;
    } else if (strncmp(p, "../config/", 10) == 0) {
        p += 10;
    } else if (strncmp(p, "config/", 7) == 0) {
        p += 7;
    } else if (strncmp(p, "../game/data/", 13) == 0) {
        p += 13;
    } else if (strncmp(p, "game/data/", 10) == 0) {
        p += 10;
    }
    return p;
}

static const char *io_fb_find_key_span(const char *span_start,
                                       const char *span_end,
                                       const char *key)
{
    if (!io_fb_span_has_bytes(span_start, span_end) ||
        key == NULL || key[0] == '\0') {
        return NULL;
    }

    size_t key_len = strlen(key);
    const char *p = span_start;
    bool in_str = false;

    while (p < span_end) {
        if (*p == '"' && io_fb_is_unescaped_quote(span_start, p)) {
            if (!in_str) {
                const char *cand = p + 1;
                if ((size_t)(span_end - cand) > key_len &&
                    strncmp(cand, key, key_len) == 0 && cand[key_len] == '"') {
                    const char *after = io_fb_skip_ws(cand + key_len + 1U, span_end);
                    if (after < span_end && *after == ':') {
                        after = io_fb_skip_ws(after + 1, span_end);
                        if (after < span_end) {
                            return after;
                        }
                    }
                }
            }
            in_str = !in_str;
        }
        p++;
    }
    return NULL;
}

static bool io_fb_find_block_span(const char *span_start,
                                  const char *span_end,
                                  const char *key,
                                  char open_ch,
                                  char close_ch,
                                  const char **out_start,
                                  size_t *out_len)
{
    if (out_start == NULL || out_len == NULL) {
        return false;
    }
    *out_start = NULL;
    *out_len = 0U;

    const char *val = io_fb_find_key_span(span_start, span_end, key);
    if (val == NULL || val >= span_end || *val != open_ch) {
        return false;
    }

    int depth = 0;
    bool in_str = false;
    const char *p = val;
    while (p < span_end) {
        if (*p == '"' && io_fb_is_unescaped_quote(val, p)) {
            in_str = !in_str;
        } else if (!in_str) {
            if (*p == open_ch) {
                depth++;
                if (depth > IO_FB_MAX_JSON_DEPTH) {
                    io_fallback_record_fault("io_fallback.json", "find_block_span",
                                             "Exceeded max JSON scan depth",
                                             IO_ERR_JSON_DEPTH_EXCEEDED);
                    return false;
                }
            } else if (*p == close_ch) {
                depth--;
                if (depth == 0) {
                    *out_start = val;
                    *out_len = (size_t)(p - val + 1);
                    return true;
                }
            }
        }
        p++;
    }
    return false;
}

static const char *io_fb_decode_string_token(const char *quote_start,
                                             const char *span_end,
                                             char *out_buf,
                                             size_t out_size)
{
    if (quote_start == NULL || span_end == NULL || quote_start >= span_end ||
        *quote_start != '"' || !io_fb_buf_is_valid(out_buf, out_size)) {
        return NULL;
    }

    const char *p = quote_start + 1;
    size_t idx = 0U;
    while (p < span_end) {
        if (*p == '\\' && (p + 1) < span_end) {
            p++;
            char esc = *p;
            char decoded = esc;
            if (esc == 'n') {
                decoded = '\n';
            } else if (esc == 'r') {
                decoded = '\r';
            } else if (esc == 't') {
                decoded = '\t';
            } else if (esc == 'b') {
                decoded = '\b';
            } else if (esc == 'f') {
                decoded = '\f';
            }
            if (idx < out_size - 1U) {
                out_buf[idx++] = decoded;
            } else {
                out_buf[out_size - 1U] = '\0';
                return NULL;
            }
        } else if (*p == '"') {
            out_buf[idx] = '\0';
            return p + 1;
        } else {
            if (idx < out_size - 1U) {
                out_buf[idx++] = *p;
            } else {
                out_buf[out_size - 1U] = '\0';
                return NULL;
            }
        }
        p++;
    }
    out_buf[idx] = '\0';
    return NULL;
}

static bool io_fb_get_string_span(const char *span_start,
                                  const char *span_end,
                                  const char *key,
                                  char *out_buf,
                                  size_t out_size)
{
    if (io_fb_buf_is_valid(out_buf, out_size)) {
        out_buf[0] = '\0';
    }
    if (!io_fb_buf_is_valid(out_buf, out_size)) {
        return false;
    }
    const char *val = io_fb_find_key_span(span_start, span_end, key);
    if (val == NULL || val >= span_end || *val != '"') {
        return false;
    }
    return io_fb_decode_string_token(val, span_end, out_buf, out_size) != NULL;
}

static bool io_fb_get_int_span(const char *span_start,
                               const char *span_end,
                               const char *key,
                               int *out_val)
{
    if (out_val == NULL) {
        return false;
    }
    const char *val = io_fb_find_key_span(span_start, span_end, key);
    if (val == NULL || val >= span_end) {
        return false;
    }

    char num_buf[32];
    size_t copy_len = (size_t)(span_end - val);
    if (copy_len >= sizeof(num_buf)) {
        copy_len = sizeof(num_buf) - 1U;
    }
    memcpy(num_buf, val, copy_len);
    num_buf[copy_len] = '\0';

    char *endptr = NULL;
    errno = 0;
    long parsed = strtol(num_buf, &endptr, 10);
    if (endptr == num_buf || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
        return false;
    }
    *out_val = (int)parsed;
    return true;
}

static bool io_fb_get_bool_span(const char *span_start,
                                const char *span_end,
                                const char *key,
                                bool *out_val)
{
    if (out_val == NULL) {
        return false;
    }
    const char *val = io_fb_find_key_span(span_start, span_end, key);
    if (val == NULL || val >= span_end) {
        return false;
    }
    size_t avail = (size_t)(span_end - val);
    if (avail >= 4U && strncmp(val, "true", 4) == 0 &&
        (avail == 4U || (!isalnum((unsigned char)val[4]) && val[4] != '_'))) {
        *out_val = true;
        return true;
    }
    if (avail >= 5U && strncmp(val, "false", 5) == 0 &&
        (avail == 5U || (!isalnum((unsigned char)val[5]) && val[5] != '_'))) {
        *out_val = false;
        return true;
    }
    return false;
}

static bool io_fb_get_string_array_span(const char *span_start,
                                        const char *span_end,
                                        const char *key,
                                        char out_items[][IO_FB_MAX_DIR_SEG_LEN],
                                        size_t max_items,
                                        size_t *out_count)
{
    if (out_count != NULL) {
        *out_count = 0U;
    }
    if (out_items == NULL || max_items == 0U || out_count == NULL) {
        return false;
    }

    const char *arr_start = NULL;
    size_t arr_len = 0U;
    if (!io_fb_find_block_span(span_start, span_end, key, '[', ']', &arr_start, &arr_len) ||
        arr_start == NULL || arr_len < 2U) {
        return false;
    }

    const char *p = arr_start + 1;
    const char *arr_end = arr_start + arr_len - 1U;
    size_t count = 0U;

    while (p < arr_end && count < max_items) {
        p = io_fb_skip_ws(p, arr_end);
        if (p >= arr_end) {
            break;
        }
        if (*p == ',') {
            p++;
            continue;
        }
        if (*p == '"') {
            const char *next = io_fb_decode_string_token(p, arr_end,
                                                         out_items[count],
                                                         IO_FB_MAX_DIR_SEG_LEN);
            if (next == NULL) {
                break;
            }
            count++;
            p = next;
        } else {
            p++;
        }
    }

    *out_count = count;
    return (count > 0U);
}

static void io_fb_seed_default_vfs_routes(io_fb_vfs_routes_t *out_routes)
{
    if (out_routes == NULL) {
        return;
    }
    memset(out_routes, 0, sizeof(*out_routes));
    str_safe_copy(out_routes->default_data_root, IO_FB_DEFAULT_ROOT,
                  sizeof(out_routes->default_data_root));
    str_safe_copy(out_routes->env_override_var, "TL_DATA_ROOT",
                  sizeof(out_routes->env_override_var));

    str_safe_copy(out_routes->config_precedence[0], "game/config/", IO_FB_MAX_DIR_SEG_LEN);
    str_safe_copy(out_routes->config_precedence[1], "engine/config/", IO_FB_MAX_DIR_SEG_LEN);
    str_safe_copy(out_routes->config_precedence[2], "game/data/presentation/config/",
                  IO_FB_MAX_DIR_SEG_LEN);
    out_routes->config_precedence_count = 3U;

    for (size_t i = 0U; i < 4U; i++) {
        str_safe_copy(out_routes->fallback_candidate_roots[i],
                      s_io_fb_seed_candidates[i], IO_FB_MAX_DIR_SEG_LEN);
    }
    out_routes->candidate_root_count = 4U;

    str_safe_copy(out_routes->asset_roots[0], "game/assets", IO_FB_MAX_DIR_SEG_LEN);
    str_safe_copy(out_routes->asset_roots[1], "../game/assets", IO_FB_MAX_DIR_SEG_LEN);
    str_safe_copy(out_routes->asset_roots[2], "assets", IO_FB_MAX_DIR_SEG_LEN);
    str_safe_copy(out_routes->asset_roots[3], "../assets", IO_FB_MAX_DIR_SEG_LEN);
    out_routes->asset_root_count = 4U;

    for (size_t i = 0U; i < 6U; i++) {
        str_safe_copy(out_routes->domain_search_paths[i],
                      s_io_fb_seed_domains[i], IO_FB_MAX_DIR_SEG_LEN);
    }
    out_routes->domain_search_count = 6U;
    str_safe_copy(out_routes->diag_log_prefix, "diagnostics/telemetry/", IO_FB_MAX_DIR_SEG_LEN);
    str_safe_copy(out_routes->diag_debug_prefix, "diagnostics/debugger/", IO_FB_MAX_DIR_SEG_LEN);
    str_safe_copy(out_routes->diag_io_prefix, "io/", IO_FB_MAX_DIR_SEG_LEN);
    out_routes->block_path_traversal = true;
    out_routes->max_symlink_depth = IO_FB_DEFAULT_SYMLINK_DEPTH;
    out_routes->allow_mod_mounts = true;
    out_routes->rom_fallback_enabled = true;
    out_routes->probe_await_on_corrupt = true;
    out_routes->loaded_from_data = false;
}

static void io_fb_seed_default_loader_contract(io_fb_loader_contract_t *out_contract)
{
    if (out_contract == NULL) {
        return;
    }
    memset(out_contract, 0, sizeof(*out_contract));
    out_contract->require_block_1_metadata = true;
    out_contract->require_5block_envelope = true;
    out_contract->enforce_schema_id = true;
    out_contract->require_monotonic_version = true;
    out_contract->allow_legacy_block2_types = true;
    out_contract->probe_await_on_corrupt = true;
    out_contract->strict_utf8_escapes = false;
    out_contract->reject_duplicate_keys = false;
    out_contract->max_json_depth = IO_FB_MAX_JSON_DEPTH;
    out_contract->max_json_tokens = IO_FB_MAX_JSON_TOKENS;
    out_contract->max_payload_bytes = IO_FB_MAX_CONFIG_BYTES;
    str_safe_copy(out_contract->expected_major_minor_version, "0.1",
                  sizeof(out_contract->expected_major_minor_version));
    out_contract->loaded_from_data = false;
}

static void io_fb_hydrate_subpolicies(void)
{
    io_fb_seed_default_vfs_routes(&s_io_fb.vfs_routes);
    io_fb_seed_default_loader_contract(&s_io_fb.loader_contract);

    size_t vfs_sz = 0U;
    const char *vfs_json = io_fallback_find_rom_asset("io/vfs_routes.json", &vfs_sz);
    if (vfs_json != NULL && vfs_sz > 0U) {
        (void)io_fallback_parse_vfs_routes_json(vfs_json, vfs_sz, &s_io_fb.vfs_routes);
    }

    size_t ldr_sz = 0U;
    const char *ldr_json = io_fallback_find_rom_asset("io/loader_contracts.json", &ldr_sz);
    if (ldr_json != NULL && ldr_sz > 0U) {
        (void)io_fallback_parse_loader_contract_json(ldr_json, ldr_sz,
                                                    &s_io_fb.loader_contract);
    }
}

static bool io_fb_resolve_body_span(const char *json_text,
                                    size_t json_len,
                                    bool allow_strlen,
                                    const char **out_body_start,
                                    const char **out_body_end)
{
    if (json_text == NULL || out_body_start == NULL || out_body_end == NULL) {
        return false;
    }

    size_t effective_len = (json_len > 0U) ? json_len : (allow_strlen ? strlen(json_text) : 0U);
    if (effective_len == 0U || effective_len > IO_FB_JSON_MAX_BYTES) {
        return false;
    }

    const char *json_end = json_text + effective_len;
    const char *body_start = NULL;
    size_t body_len = 0U;
    if (!io_fb_find_block_span(json_text, json_end, "block_3_body", '{', '}',
                               &body_start, &body_len) ||
        body_start == NULL || body_len == 0U) {
        return false;
    }

    *out_body_start = body_start;
    *out_body_end = body_start + body_len;
    return true;
}

static bool io_fb_guard_disk_mutation(const char *op,
                                      const char *path,
                                      bool block_traversal,
                                      const char *overflow_msg,
                                      const char *traversal_msg,
                                      const char *readonly_msg)
{
    size_t path_cap = (s_io_fb.config.path_max_bytes > 0U &&
                       s_io_fb.config.path_max_bytes <= IO_FB_PATH_MAX)
        ? s_io_fb.config.path_max_bytes
        : IO_FB_PATH_MAX;
    size_t path_len = strlen(path);
    if (path_len == 0U || path_len >= path_cap) {
        io_fallback_record_fault("io_fallback.fs", op, overflow_msg, IO_ERR_PATH_OVERFLOW);
        return false;
    }

    if (block_traversal && io_fb_has_parent_traversal(path)) {
        io_fallback_record_fault("io_fallback.fs", op, traversal_msg, IO_ERR_PATH_OVERFLOW);
        return false;
    }

    if (io_fallback_is_disk_read_only()) {
        s_io_fb.read_only_blocks++;
        io_fallback_record_fault("io_fallback.fs", op, readonly_msg, IO_ERR_WRITE_BLOCKED_RO);
        return false;
    }

    return true;
}

static void io_fb_auto_ensure_booted(void)
{
    if (!s_io_fb.booted) {
        s_io_fb.config = s_io_fallback_template;
        io_fb_hydrate_subpolicies();
        str_safe_copy(s_io_fb.active_root, IO_FB_DEFAULT_ROOT, sizeof(s_io_fb.active_root));
        s_io_fb.active_sink = IO_FB_SINK_DISK_ACTIVE;
        s_io_fb.booted = true;
    }
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Safe-Mode BIOS Implementation, .rodata ROM Hydration & Fault Recording )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.io.fallback
Mission:       Implement BIOS Sub-Assemblies 3.1..3.3 with loaded_from_data = false,
               zero static helpers inside Block 3 (2.5), and reconstructible Doxygen.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Policy Constructors & Shared JSON Config Parsers
Mission: Provide hardcoded Safe-Mode/Floppy templates plus ONE shared span-bounded
         parser serving ROM vault hydration and primary io/ config ingestion.

  ### For Game Designers & Non-Programmers:
  Turns built-in or disk JSON rulebooks into active buffer ceilings, write-lock
  switches, VFS folder search paths, and schema validation contracts safely.

  ### For Systems Programmers & Engine Architects:
  Executes single-pass in-place scans over caller spans via `Sub-Assembly 2.5`
  `io_fb_resolve_body_span()`, enforcing `loaded_from_data = false` across all
  Safe-Mode/ROM loads with zero heap allocation or auxiliary 8 KB copies.
--------------------------------------------------------------------------------
*/

/**
 * @brief Return an immutable copy of the compile-time default Safe-Mode I/O policy.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Hands any caller baseline safe file sizes and read/write defaults even before
 *   the engine opens external settings files.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Returns `s_io_fallback_template` by value (`loaded_from_data == false`) in O(1)
 *   time with zero `.bss` mutation.
 *
 * @return Default Safe-Mode `io_fallback_config_t` preset (`loaded_from_data == false`).
 * @triggers Guarantees pure read-only value semantics with zero state side effects.
 */
io_fallback_config_t io_fallback_get_default_config(void)
{
    /* Step 1: Return immutable .rodata Safe-Mode template struct by value */
    return s_io_fallback_template;
}

/**
 * @brief Return a compact read-only floppy preset locking all disk mutations off.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Supplies tight buffer limits and forces read-only Safe Mode when running from
 *   write-protected media or stripped recovery disks.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Returns `s_io_floppy_template` by value (`read_only_mode == true`,
 *   `loaded_from_data == false`) in O(1) time.
 *
 * @return Read-only floppy `io_fallback_config_t` preset (`loaded_from_data == false`).
 * @triggers Guarantees pure read-only value semantics without touching `.bss`.
 */
io_fallback_config_t io_fallback_get_floppy_config(void)
{
    /* Step 1: Return immutable .rodata read-only floppy template struct */
    return s_io_floppy_template;
}

/**
 * @brief Parse `block_3_body` from an exact caller JSON slice into `io_fallback_config_t`.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads file size caps, JSON safety depths, atomic-write toggles, and directory
 *   permissions out of `engine/config/io/` profiles without duplicating buffers.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Resolves `block_3_body` via `io_fb_resolve_body_span()` directly in caller slice
 *   `[json_start, json_start + json_len)`, seeds missing fields from
 *   `s_io_fallback_template`, checks `io_fb_policy_bounds_valid()`, and keeps
 *   `loaded_from_data = false` unconditionally.
 *
 * @param[in]  json_start Start pointer of UTF-8 JSON span.
 * @param[in]  json_len   Exact byte length of slice (`(0, IO_FB_JSON_MAX_BYTES]`).
 * @param[out] out_cfg    Validated output policy struct initialized on success.
 * @return `true` when `block_3_body` parsed cleanly and bounds passed; `false` on fault.
 * @triggers Validates strict policy bounds and forces `loaded_from_data = false`.
 */
bool io_fallback_parse_policy_span(const char *json_start,
                                   size_t json_len,
                                   io_fallback_config_t *out_cfg)
{
    /* Step 1: Resolve `block_3_body` slice directly without temporary copies */
    const char *body_start = NULL;
    const char *body_end = NULL;
    if (out_cfg == NULL ||
        !io_fb_resolve_body_span(json_start, json_len, false, &body_start, &body_end)) {
        return false;
    }

    /* Step 2: Seed defaults and hydrate string/numeric/boolean fields from body span */
    io_fallback_config_t cfg = s_io_fallback_template;
    int val_int = 0;
    bool val_bool = false;
    char val_str[64];

    if (io_fb_get_string_span(body_start, body_end, "profile_name", val_str, sizeof(val_str)) &&
        val_str[0] != '\0') {
        str_safe_copy(cfg.profile_name, val_str, sizeof(cfg.profile_name));
    }
    if (io_fb_get_string_span(body_start, body_end, "default_root", val_str, sizeof(val_str)) &&
        val_str[0] != '\0') {
        str_safe_copy(cfg.default_root, val_str, sizeof(cfg.default_root));
    }

    if (io_fb_get_int_span(body_start, body_end, "small_bytes", &val_int)) {
        cfg.small_bytes = (val_int > 0) ? (size_t)val_int : 0U;
    }
    if (io_fb_get_int_span(body_start, body_end, "medium_bytes", &val_int)) {
        cfg.medium_bytes = (val_int > 0) ? (size_t)val_int : 0U;
    }
    if (io_fb_get_int_span(body_start, body_end, "large_bytes", &val_int)) {
        cfg.large_bytes = (val_int > 0) ? (size_t)val_int : 0U;
    }
    if (io_fb_get_int_span(body_start, body_end, "max_config_bytes", &val_int)) {
        cfg.max_config_bytes = (val_int > 0) ? (size_t)val_int : 0U;
    }
    if (io_fb_get_int_span(body_start, body_end, "path_max_bytes", &val_int)) {
        cfg.path_max_bytes = (val_int > 0) ? (size_t)val_int : 0U;
    }
    if (io_fb_get_int_span(body_start, body_end, "max_json_depth", &val_int)) {
        cfg.max_json_depth = val_int;
    }
    if (io_fb_get_int_span(body_start, body_end, "max_json_tokens", &val_int)) {
        cfg.max_json_tokens = (val_int > 0) ? (size_t)val_int : 0U;
    }

    if (io_fb_get_string_span(body_start, body_end, "posix_dir_mode_octal",
                              val_str, sizeof(val_str))) {
        unsigned int parsed_mode = IO_FB_DEFAULT_DIR_MODE;
        if (io_fb_parse_octal_mode(val_str, &parsed_mode)) {
            cfg.posix_dir_mode = parsed_mode;
        } else {
            cfg.posix_dir_mode = 0xFFFFU;
        }
    } else if (io_fb_get_int_span(body_start, body_end, "posix_dir_mode", &val_int)) {
        cfg.posix_dir_mode = (val_int >= 0) ? (unsigned int)val_int : 0xFFFFU;
    }

    if (io_fb_get_bool_span(body_start, body_end, "reject_on_truncation", &val_bool)) {
        cfg.reject_on_truncation = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "require_5block_envelope", &val_bool)) {
        cfg.require_5block_envelope = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "enforce_schema_id", &val_bool)) {
        cfg.enforce_schema_id = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "rom_fallback_enabled", &val_bool)) {
        cfg.rom_fallback_enabled = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "read_only_mode", &val_bool)) {
        cfg.read_only_mode = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "probe_await_on_corrupt_policy", &val_bool) ||
        io_fb_get_bool_span(body_start, body_end, "probe_await_on_corrupt", &val_bool)) {
        cfg.probe_await_on_corrupt = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "flush_on_write", &val_bool)) {
        cfg.flush_on_write = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "atomic_write", &val_bool)) {
        cfg.atomic_write = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "block_path_traversal", &val_bool)) {
        cfg.block_path_traversal = val_bool;
    }
    if (io_fb_get_int_span(body_start, body_end, "max_symlink_depth", &val_int)) {
        cfg.max_symlink_depth = val_int;
    }
    if (io_fb_get_bool_span(body_start, body_end, "allow_mod_mounts", &val_bool)) {
        cfg.allow_mod_mounts = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "strict_utf8_escapes", &val_bool)) {
        cfg.strict_utf8_escapes = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "reject_duplicate_keys", &val_bool)) {
        cfg.reject_duplicate_keys = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "hot_reload_enabled", &val_bool)) {
        cfg.hot_reload_enabled = val_bool;
    }

    /* Step 3: Validate parsed buffer-ladder and depth bounds before committing output */
    if (!io_fb_policy_bounds_valid(&cfg)) {
        return false;
    }

    cfg.loaded_from_data = false;
    *out_cfg = cfg;
    return true;
}

/**
 * @brief Parse a NUL-terminated or length-bounded JSON buffer into `io_fallback_config_t`.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Canonical single shared preset reader used by both built-in ROM profiles and
 *   external configuration loaders so limits behave identically everywhere.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Computes `strlen(json_text)` only when `json_len == 0U` and forwards cleanly into
 *   `io_fallback_parse_policy_span()`, preserving `ALLOC_ZERO_HEAP`.
 *
 * @param[in]  json_text JSON string buffer to parse.
 * @param[in]  json_len  Payload byte count, or `0U` to measure length via `strlen()`.
 * @param[out] out_cfg   Destination validated policy structure.
 * @return `true` if valid JSON parsed and satisfied policy invariants; `false` otherwise.
 * @triggers Enforces null pointer guards cleanly under non-awaiting negative policy.
 */
bool io_fallback_parse_policy_json(const char *json_text,
                                   size_t json_len,
                                   io_fallback_config_t *out_cfg)
{
    /* Step 1: Reject null pointers before measuring NUL-terminated length fallback */
    if (json_text == NULL || out_cfg == NULL) {
        return false;
    }

    /* Step 2: Forward bounded span directly into single-source span parser */
    size_t effective_len = (json_len > 0U) ? json_len : strlen(json_text);
    return io_fallback_parse_policy_span(json_text, effective_len, out_cfg);
}

/**
 * @brief Hydrate VFS routing roots, 36+ domain paths, and symlink/traversal rules from JSON.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Loads `vfs_routes.json` so asset folders, mod override precedence tiers, and
 *   diagnostic aliases stay configurable in data files rather than hardcoded C arrays.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Resolves `block_3_body` via `io_fb_resolve_body_span()`, seeds defaults with
 *   `io_fb_seed_default_vfs_routes()`, extracts precedence/root/domain arrays within
 *   fixed `.bss` caps, and forces `out_routes->loaded_from_data = false`.
 *
 * @param[in]  json_text  Raw `vfs_routes.json` text buffer.
 * @param[in]  json_len   Byte length of `json_text` (`0U` for `strlen()`).
 * @param[out] out_routes Hydrated output VFS routing descriptor.
 * @return `true` when body block parsed cleanly with non-empty root and domain lists.
 * @triggers Forces `loaded_from_data = false` so caller proves external disk provenance explicitly.
 */
bool io_fallback_parse_vfs_routes_json(const char *json_text,
                                       size_t json_len,
                                       io_fb_vfs_routes_t *out_routes)
{
    /* Step 1: Resolve `block_3_body` span and seed safe default routes */
    const char *body_start = NULL;
    const char *body_end = NULL;
    if (out_routes == NULL ||
        !io_fb_resolve_body_span(json_text, json_len, true, &body_start, &body_end)) {
        return false;
    }

    io_fb_seed_default_vfs_routes(out_routes);

    /* Step 2: Hydrate string identifiers and precedence/search directory arrays */
    char val_str[64];
    int val_int = 0;
    bool val_bool = false;

    if (io_fb_get_string_span(body_start, body_end, "default_data_root",
                              val_str, sizeof(val_str)) && val_str[0] != '\0') {
        str_safe_copy(out_routes->default_data_root, val_str,
                      sizeof(out_routes->default_data_root));
    }
    if (io_fb_get_string_span(body_start, body_end, "env_override_var",
                              val_str, sizeof(val_str)) && val_str[0] != '\0') {
        str_safe_copy(out_routes->env_override_var, val_str,
                      sizeof(out_routes->env_override_var));
    }

    (void)io_fb_get_string_array_span(body_start, body_end, "config_precedence",
                                      out_routes->config_precedence,
                                      IO_FB_MAX_PRECEDENCE_TIERS,
                                      &out_routes->config_precedence_count);
    (void)io_fb_get_string_array_span(body_start, body_end, "fallback_candidate_roots",
                                      out_routes->fallback_candidate_roots,
                                      IO_FB_MAX_VFS_CANDIDATES,
                                      &out_routes->candidate_root_count);
    (void)io_fb_get_string_array_span(body_start, body_end, "asset_roots",
                                      out_routes->asset_roots,
                                      IO_FB_MAX_ASSET_ROOTS,
                                      &out_routes->asset_root_count);
    (void)io_fb_get_string_array_span(body_start, body_end, "domain_search_paths",
                                      out_routes->domain_search_paths,
                                      IO_FB_MAX_VFS_DOMAIN_DIRS,
                                      &out_routes->domain_search_count);

    /* Step 3: Hydrate nested diagnostic domain alias prefixes if supplied */
    const char *alias_start = NULL;
    size_t alias_len = 0U;
    if (io_fb_find_block_span(body_start, body_end, "diagnostic_aliases", '{', '}',
                              &alias_start, &alias_len) &&
        alias_start != NULL && alias_len > 2U) {
        const char *alias_end = alias_start + alias_len;
        if (io_fb_get_string_span(alias_start, alias_end, "log_prefix",
                                  val_str, sizeof(val_str)) && val_str[0] != '\0') {
            str_safe_copy(out_routes->diag_log_prefix, val_str,
                          sizeof(out_routes->diag_log_prefix));
        }
        if (io_fb_get_string_span(alias_start, alias_end, "debug_prefix",
                                  val_str, sizeof(val_str)) && val_str[0] != '\0') {
            str_safe_copy(out_routes->diag_debug_prefix, val_str,
                          sizeof(out_routes->diag_debug_prefix));
        }
        if (io_fb_get_string_span(alias_start, alias_end, "io_prefix",
                                  val_str, sizeof(val_str)) && val_str[0] != '\0') {
            str_safe_copy(out_routes->diag_io_prefix, val_str,
                          sizeof(out_routes->diag_io_prefix));
        }
    }

    /* Step 4: Extract boolean security switches and verify non-empty route tables */
    if (io_fb_get_bool_span(body_start, body_end, "block_path_traversal", &val_bool)) {
        out_routes->block_path_traversal = val_bool;
    }
    if (io_fb_get_int_span(body_start, body_end, "max_symlink_depth", &val_int) &&
        val_int >= 0 && val_int <= 16) {
        out_routes->max_symlink_depth = val_int;
    }
    if (io_fb_get_bool_span(body_start, body_end, "allow_mod_mounts", &val_bool)) {
        out_routes->allow_mod_mounts = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "rom_fallback_enabled", &val_bool)) {
        out_routes->rom_fallback_enabled = val_bool;
    }
    if (io_fb_get_bool_span(body_start, body_end, "probe_await_on_corrupt", &val_bool)) {
        out_routes->probe_await_on_corrupt = val_bool;
    }

    out_routes->loaded_from_data = false;
    return (out_routes->domain_search_count > 0U && out_routes->candidate_root_count > 0U);
}

/**
 * @brief Parse `loader_contracts.json` (`block_3_body`) into `io_fb_loader_contract_t`.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Loads schema checks, version rules, and JSON depth caps from one authoritative
 *   contract table shared across BIOS recovery, JSON scanning, and MasterLoader.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Extracts `block_3_body` via `io_fb_resolve_body_span()`, applies table-driven
 *   boolean overrides on seeded defaults, validates integer depth/token/payload bounds
 *   strictly when present (recording `IO_ERR_JSON_SYNTAX` on corrupt contract bounds),
 *   and preserves `out_contract->loaded_from_data = false`.
 *
 * @param[in]  json_text    Raw `loader_contracts.json` text buffer.
 * @param[in]  json_len     Byte count of `json_text` (`0U` to measure via `strlen()`).
 * @param[out] out_contract Validated destination MasterLoader contract structure.
 * @return `true` when `block_3_body` parsed cleanly within contract limits; `false` on fault.
 * @triggers Records `IO_ERR_JSON_SYNTAX` into BIOS ring when explicit JSON limits violate bounds.
 */
bool io_fallback_parse_loader_contract_json(const char *json_text,
                                            size_t json_len,
                                            io_fb_loader_contract_t *out_contract)
{
    /* Step 1: Resolve `block_3_body` span cleanly via shared Sub-Assembly 2.5 helper */
    const char *body_start = NULL;
    const char *body_end = NULL;
    if (out_contract == NULL ||
        !io_fb_resolve_body_span(json_text, json_len, true, &body_start, &body_end)) {
        return false;
    }

    /* Step 2: Seed defaults and apply uniform table-driven boolean contract overrides */
    io_fb_loader_contract_t parsed;
    io_fb_seed_default_loader_contract(&parsed);

    const struct {
        const char *key;
        bool       *field;
    } k_bool_fields[8] = {
        { "require_block_1_metadata",  &parsed.require_block_1_metadata },
        { "require_5block_envelope",   &parsed.require_5block_envelope },
        { "enforce_schema_id",         &parsed.enforce_schema_id },
        { "require_monotonic_version", &parsed.require_monotonic_version },
        { "allow_legacy_block2_types", &parsed.allow_legacy_block2_types },
        { "probe_await_on_corrupt",    &parsed.probe_await_on_corrupt },
        { "strict_utf8_escapes",       &parsed.strict_utf8_escapes },
        { "reject_duplicate_keys",     &parsed.reject_duplicate_keys }
    };

    for (size_t i = 0U; i < 8U; i++) {
        bool val_bool = false;
        if (io_fb_get_bool_span(body_start, body_end, k_bool_fields[i].key, &val_bool)) {
            *(k_bool_fields[i].field) = val_bool;
        }
    }

    /* Step 3: Validate numeric contract ceilings strictly if explicitly supplied in JSON */
    int val_int = 0;
    if (io_fb_get_int_span(body_start, body_end, "max_json_depth", &val_int)) {
        if (val_int <= 0 || val_int > IO_FB_MAX_JSON_DEPTH) {
            io_fallback_record_fault("io_fallback.json", "parse_loader_contract_json",
                                     "Contract max_json_depth outside [1, 32]",
                                     IO_ERR_JSON_SYNTAX);
            return false;
        }
        parsed.max_json_depth = val_int;
    }
    if (io_fb_get_int_span(body_start, body_end, "max_json_tokens", &val_int)) {
        if (val_int <= 0 || (size_t)val_int > IO_FB_MAX_JSON_TOKENS) {
            io_fallback_record_fault("io_fallback.json", "parse_loader_contract_json",
                                     "Contract max_json_tokens outside [1, 1024]",
                                     IO_ERR_JSON_SYNTAX);
            return false;
        }
        parsed.max_json_tokens = (size_t)val_int;
    }
    if (io_fb_get_int_span(body_start, body_end, "max_payload_bytes", &val_int)) {
        if (val_int < (int)IO_FB_SMALL_BYTES || (size_t)val_int > IO_FB_MAX_CONFIG_BYTES) {
            io_fallback_record_fault("io_fallback.json", "parse_loader_contract_json",
                                     "Contract max_payload_bytes outside [1024, 65536]",
                                     IO_ERR_JSON_SYNTAX);
            return false;
        }
        parsed.max_payload_bytes = (size_t)val_int;
    }

    /* Step 4: Hydrate required version prefix and commit unauthenticated BIOS result */
    char val_str[16];
    if (io_fb_get_string_span(body_start, body_end, "expected_major_minor_version",
                              val_str, sizeof(val_str))) {
        if (val_str[0] == '\0') {
            io_fallback_record_fault("io_fallback.json", "parse_loader_contract_json",
                                     "Contract expected_major_minor_version is empty",
                                     IO_ERR_JSON_SYNTAX);
            return false;
        }
        str_safe_copy(parsed.expected_major_minor_version, val_str,
                      sizeof(parsed.expected_major_minor_version));
    }

    parsed.loaded_from_data = false;
    *out_contract = parsed;
    return true;
}

/**
 * @brief Return immutable pointer to the currently active BIOS VFS routing table.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Gives diagnostics and loaders direct read-only visibility into active search roots
 *   and configured asset folders without copying large routing tables on the stack.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Arms Safe-Mode BIOS on demand via `io_fb_auto_ensure_booted()` and returns `&s_io_fb.vfs_routes`
 *   in O(1) time.
 *
 * @return Non-NULL read-only pointer into `.bss` `s_io_fb.vfs_routes`.
 * @triggers Lazy-arms BIOS subpolicies from compiled ROM when invoked before explicit boot.
 */
const io_fb_vfs_routes_t *io_fallback_get_vfs_routes(void)
{
    /* Step 1: Ensure BIOS ROM defaults are hydrated and expose .bss table pointer */
    io_fb_auto_ensure_booted();
    return &s_io_fb.vfs_routes;
}

/**
 * @brief Validate and install a replacement VFS routing table into active `.bss` state.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Permits hot-swapping active directory search lists and precedence rules safely
 *   after validating bounds so corrupted mounts never disrupt asset lookup.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Checks non-empty `default_data_root`, candidate/domain/precedence/asset array caps,
 *   and symlink ceiling `[0, 16]` before copying `routes` into `s_io_fb.vfs_routes`.
 *
 * @param[in] routes Validated VFS routing descriptor to install.
 * @return `true` when installed cleanly; `false` on null pointer or out-of-range table bounds.
 * @triggers Enforces structural capacity bounds strictly prior to mutating `.bss` state.
 */
bool io_fallback_set_vfs_routes(const io_fb_vfs_routes_t *routes)
{
    /* Step 1: Reject null pointers, empty required tables, or capacity violations */
    if (routes == NULL ||
        routes->default_data_root[0] == '\0' ||
        routes->domain_search_count == 0U ||
        routes->domain_search_count > IO_FB_MAX_VFS_DOMAIN_DIRS ||
        routes->candidate_root_count == 0U ||
        routes->candidate_root_count > IO_FB_MAX_VFS_CANDIDATES ||
        routes->config_precedence_count > IO_FB_MAX_PRECEDENCE_TIERS ||
        routes->asset_root_count > IO_FB_MAX_ASSET_ROOTS ||
        routes->max_symlink_depth < 0 ||
        routes->max_symlink_depth > 16) {
        return false;
    }

    /* Step 2: Ensure BIOS armed and store validated routing table */
    io_fb_auto_ensure_booted();
    s_io_fb.vfs_routes = *routes;
    return true;
}

/**
 * @brief Copy live hydrated MasterLoader contract out of active BIOS state.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Lets diagnostics inspect active envelope requirements and JSON safety limits.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Ensures BIOS initialization via `io_fb_auto_ensure_booted()` and copies
 *   `s_io_fb.loader_contract` into caller storage in O(1) time.
 *
 * @param[out] out_contract Destination contract structure populated on success.
 * @return `true` on success; `false` if `out_contract` is NULL.
 * @triggers Lazy-arms BIOS on first call and enforces null guard cleanly.
 */
bool io_fallback_get_loader_contract(io_fb_loader_contract_t *out_contract)
{
    /* Step 1: Validate output pointer */
    if (out_contract == NULL) {
        return false;
    }

    /* Step 2: Ensure BIOS armed and copy active loader contract by value */
    io_fb_auto_ensure_booted();
    *out_contract = s_io_fb.loader_contract;
    return true;
}

/**
 * @brief Install and clamp a MasterLoader contract descriptor in active BIOS state.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Applies updated JSON nesting, token, and version rules safely while automatically
 *   clamping oversized limits back inside hard chassis ceilings.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Rejects NULL or empty version prefixes, arms BIOS, copies `contract`, and normalizes
 *   `max_json_depth`, `max_json_tokens`, and `max_payload_bytes` to compile-time caps.
 *
 * @param[in] contract Contract descriptor to apply.
 * @return `true` when installed; `false` if NULL or `expected_major_minor_version` is empty.
 * @triggers Normalizes structural numeric ceilings cleanly inside hardware limits.
 */
bool io_fallback_set_loader_contract(const io_fb_loader_contract_t *contract)
{
    /* Step 1: Reject NULL contract or missing major.minor version requirement */
    if (contract == NULL || contract->expected_major_minor_version[0] == '\0') {
        return false;
    }

    /* Step 2: Copy contract and clamp numeric limits within BIOS chassis ceilings */
    io_fb_auto_ensure_booted();
    s_io_fb.loader_contract = *contract;
    if (s_io_fb.loader_contract.max_json_depth <= 0 ||
        s_io_fb.loader_contract.max_json_depth > IO_FB_MAX_JSON_DEPTH) {
        s_io_fb.loader_contract.max_json_depth = IO_FB_MAX_JSON_DEPTH;
    }
    if (s_io_fb.loader_contract.max_json_tokens == 0U ||
        s_io_fb.loader_contract.max_json_tokens > IO_FB_MAX_JSON_TOKENS) {
        s_io_fb.loader_contract.max_json_tokens = IO_FB_MAX_JSON_TOKENS;
    }
    if (s_io_fb.loader_contract.max_payload_bytes < IO_FB_SMALL_BYTES ||
        s_io_fb.loader_contract.max_payload_bytes > IO_FB_MAX_CONFIG_BYTES) {
        s_io_fb.loader_contract.max_payload_bytes = IO_FB_MAX_CONFIG_BYTES;
    }
    return true;
}

/**
 * @brief Copy the active Tier-0 BIOS configuration policy into caller storage.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Lets primary storage helpers check active safety caps, traversal locks, and
 *   read-only switches before reading or saving game files.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Auto-boots BIOS if unarmed and copies `s_io_fb.config` in O(1) time.
 *
 * @param[out] out_cfg Destination configuration struct populated on success.
 * @return `true` on success; `false` if `out_cfg` is NULL.
 * @triggers Lazy-arms BIOS on first invocation without mutating parsed policy flags.
 */
bool io_fallback_get_active_config(io_fallback_config_t *out_cfg)
{
    /* Step 1: Validate caller output pointer */
    if (out_cfg == NULL) {
        return false;
    }

    /* Step 2: Ensure BIOS armed and copy active policy snapshot */
    io_fb_auto_ensure_booted();
    *out_cfg = s_io_fb.config;
    return true;
}

/**
 * @brief Arm or clear the Safe-Mode floppy write-protection latch dynamically.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Flips write protection on or off—useful for testing locked-floppy survival mode
 *   or recovering safely when external media turns read-only.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Updates `s_io_fb.config.read_only_mode` and transitions `s_io_fb.active_sink`
 *   between `IO_FB_SINK_FLOPPY_RO` and `IO_FB_SINK_DISK_ACTIVE`.
 *
 * @param[in] read_only `true` to engage read-only write blocking; `false` to re-enable writes.
 * @triggers Transitions active BIOS sink mode and guards downstream write paths.
 */
void io_fallback_set_read_only(bool read_only)
{
    /* Step 1: Arm BIOS and switch read-only flag and sink cleanly */
    io_fb_auto_ensure_booted();
    s_io_fb.config.read_only_mode = read_only;
    if (read_only) {
        s_io_fb.active_sink = IO_FB_SINK_FLOPPY_RO;
    } else if (s_io_fb.active_sink == IO_FB_SINK_FLOPPY_RO) {
        s_io_fb.active_sink = IO_FB_SINK_DISK_ACTIVE;
    }
}

/**
 * @brief Query whether the Safe-Mode floppy read-only latch is currently active.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Quick check telling callers whether saving files or creating directories will be
 *   blocked cleanly for safety.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Returns `false` immediately before boot; otherwise reads `s_io_fb.config.read_only_mode`
 *   in O(1) time without modifying `.bss` state.
 *
 * @return `true` when BIOS is booted with `read_only_mode == true`; `false` otherwise.
 * @triggers Guarantees lock-free O(1) read-only state predicate.
 */
bool io_fallback_is_disk_read_only(void)
{
    /* Step 1: Check booted flag and read-only configuration lock */
    if (!s_io_fb.booted) {
        return false;
    }
    return s_io_fb.config.read_only_mode;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Compiled-In .rodata I/O Config ROM Vault Cogs
Mission: Enumerate, verify, and hydrate factory-sealed .rodata JSON blueprints.

  ### For Game Designers & Non-Programmers:
  Provides direct read access to built-in backup copies of engine I/O profiles,
  VFS folder lists, and loader contracts even on a completely blank drive.

  ### For Systems Programmers & Engine Architects:
  Indexes `s_io_config_rom_table[]` (generated via CMake GenerateConfigRom) in
  O(1)/O(R) time cleanly with zero heap allocation or `.bss` mutation.
--------------------------------------------------------------------------------
*/

/**
 * @brief Return the total count of JSON config payloads compiled into `.rodata` ROM.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Tells diagnostic menus how many factory-sealed I/O configuration blueprints
 *   live inside the game binary itself.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Evaluates compile-time macro `IO_CONFIG_ROM_ASSET_COUNT` cleanly in O(1) time.
 *
 * @return Number of `.rodata` I/O ROM assets embedded in `s_io_config_rom_table[]`.
 * @triggers Guarantees zero disk access and zero mutable state reads.
 */
size_t io_fallback_rom_count(void)
{
    /* Step 1: Return compile-time generated ROM asset table entry count */
    return (size_t)IO_CONFIG_ROM_ASSET_COUNT;
}

/**
 * @brief Return immutable NUL-terminated JSON payload pointer for a ROM table index.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads built-in JSON settings text directly from read-only executable memory
 *   when external folders are absent or damaged.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Bounds-checks `index` against `IO_CONFIG_ROM_ASSET_COUNT`, zeros `*out_size` on
 *   out-of-range lookups, and returns `&s_io_config_rom_table[index].data` directly.
 *
 * @param[in]  index    ROM asset index in `[0, io_fallback_rom_count() - 1]`.
 * @param[out] out_size Optional pointer receiving byte count excluding NUL terminator.
 * @return Immutable `.rodata` JSON string pointer, or NULL when `index` is out of bounds.
 * @triggers Bounds-checks `index` safely under non-awaiting negative lookup policy.
 */
const char *io_fallback_rom_asset_at(size_t index, size_t *out_size)
{
    /* Step 1: Guard out-of-range table index cleanly */
    if (index >= (size_t)IO_CONFIG_ROM_ASSET_COUNT) {
        if (out_size != NULL) {
            *out_size = 0U;
        }
        return NULL;
    }

    /* Step 2: Expose immutable `.rodata` payload slice and byte length */
    const io_rom_asset_t *asset = &s_io_config_rom_table[index];
    if (out_size != NULL) {
        *out_size = asset->size;
    }
    return (const char *)asset->data;
}

/**
 * @brief Query metadata fields (`id` stem, relative path, and byte size) for a ROM slot.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Supplies clean human-readable names and filenames of embedded emergency presets
 *   for startup diagnostics and ROM inventory cards.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Retrieves optional `.rodata` metadata pointers (`out_id`, `out_rel_path`, `out_size`)
 *   for `s_io_config_rom_table[index]` in O(1) time.
 *
 * @param[in]  index        ROM table slot index in `[0, io_fallback_rom_count() - 1]`.
 * @param[out] out_id       Optional output receiving ROM identifier stem (e.g. `"default"`).
 * @param[out] out_rel_path Optional output receiving relative path (e.g. `"io/default.json"`).
 * @param[out] out_size     Optional output receiving payload byte length.
 * @return `true` if slot existed and requested fields were written; `false` out of bounds.
 * @triggers Performs bounded O(1) index guard without heap allocation.
 */
bool io_fallback_rom_info_at(size_t index,
                             const char **out_id,
                             const char **out_rel_path,
                             size_t *out_size)
{
    /* Step 1: Reject out-of-range slot index */
    if (index >= (size_t)IO_CONFIG_ROM_ASSET_COUNT) {
        return false;
    }

    /* Step 2: Populate requested optional metadata pointers from `.rodata` entry */
    const io_rom_asset_t *asset = &s_io_config_rom_table[index];
    if (out_id != NULL) {
        *out_id = asset->id;
    }
    if (out_rel_path != NULL) {
        *out_rel_path = asset->rel_path;
    }
    if (out_size != NULL) {
        *out_size = asset->size;
    }
    return true;
}

/**
 * @brief Locate an embedded `.rodata` I/O ROM JSON payload by ID stem, relative path, or leaf filename.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Finds built-in presets flexibly whether callers ask for `"default"`, `"io/default.json"`,
 *   or `"rom://io/default.json"`.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Strips VFS prefixes via `io_fb_strip_path_prefixes()` and executes case-insensitive
 *   table match across `asset->id`, `asset->rel_path`, and basename leaf (`O(R)`).
 *
 * @param[in]  id_or_path Lookup stem, config relative path, or `"rom://..."` URI.
 * @param[out] out_size   Optional output receiving byte length (`0` on miss/invalid input).
 * @return Immutable `.rodata` payload pointer on match, or NULL if absent.
 * @triggers Normalizes lookup string cleanly before scanning immutable ROM table.
 */
const char *io_fallback_find_rom_asset(const char *id_or_path, size_t *out_size)
{
    /* Step 1: Guard null/empty keys and initialize optional byte counter */
    if (id_or_path == NULL || id_or_path[0] == '\0') {
        if (out_size != NULL) {
            *out_size = 0U;
        }
        return NULL;
    }

    /* Step 2: Normalize virtual prefixes and scan embedded ROM table */
    const char *needle = io_fb_strip_path_prefixes(id_or_path);
    for (size_t i = 0U; i < (size_t)IO_CONFIG_ROM_ASSET_COUNT; i++) {
        const io_rom_asset_t *asset = &s_io_config_rom_table[i];
        if (str_case_eq(asset->id, needle) || str_case_eq(asset->rel_path, needle)) {
            if (out_size != NULL) {
                *out_size = asset->size;
            }
            return (const char *)asset->data;
        }
        /* Step 3: Fall back to matching trailing leaf filename */
        const char *slash = strrchr(asset->rel_path, '/');
        const char *leaf = (slash != NULL) ? (slash + 1) : asset->rel_path;
        if (str_case_eq(leaf, needle)) {
            if (out_size != NULL) {
                *out_size = asset->size;
            }
            return (const char *)asset->data;
        }
    }

    if (out_size != NULL) {
        *out_size = 0U;
    }
    return NULL;
}

/**
 * @brief Hydrate an `io_fallback_config_t` directly from a named compiled-in `.rodata` ROM preset.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Restores factory-certified buffer limits and safety locks (`"default"`, `"floppy_safe"`,
 *   `"fs_buffers"`, etc.) directly from ROM when disk configuration cannot be trusted.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Resolves ROM slice via `io_fallback_find_rom_asset()`, delegates to
 *   `io_fallback_parse_policy_span()`, enforces `out_cfg->loaded_from_data = false`, and
 *   records `IO_ERR_ROM_MISSING` + probe escalation (`io.fallback.rom_integrity`) if a
 *   compiled ROM asset fails policy integrity bounds.
 *
 * @param[in]  profile_id ROM profile ID/filename (NULL/empty defaults to `"default"`).
 * @param[out] out_cfg    Destination policy struct hydrated with validated ROM defaults.
 * @return `true` when ROM located and parsed within bounds; `false` on miss or corrupt ROM.
 * @triggers Escalates ROM corruption via `io_fallback_record_fault()` and `debug_trigger_probe()`.
 */
bool io_fallback_load_rom_config(const char *profile_id, io_fallback_config_t *out_cfg)
{
    /* Step 1: Reject NULL output struct */
    if (out_cfg == NULL) {
        return false;
    }

    /* Step 2: Locate matching compiled `.rodata` ROM JSON slice */
    const char *target = (profile_id != NULL && profile_id[0] != '\0') ? profile_id : "default";
    size_t rom_size = 0U;
    const char *rom_json = io_fallback_find_rom_asset(target, &rom_size);
    if (rom_json == NULL || rom_size == 0U) {
        return false;
    }

    /* Step 3: Hydrate policy span and escalate corrupt ROM payloads into Safe Mode */
    if (!io_fallback_parse_policy_span(rom_json, rom_size, out_cfg)) {
        io_fallback_record_fault("io_fallback.rom", "load_rom_config",
                                 "Embedded ROM config payload failed policy bounds or schema parse",
                                 IO_ERR_ROM_MISSING);
        if (debug_is_active() && s_io_fb.config.probe_await_on_corrupt) {
            io_fallback_register_debug_probes();
            (void)debug_trigger_probe("io.fallback.rom_integrity",
                                      IO_FALLBACK_MODULE_ID,
                                      "Compiled .rodata I/O ROM config failed policy verification",
                                      __FILE__, __LINE__, __func__);
        }
        return false;
    }

    out_cfg->loaded_from_data = false;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Unified Safe-Mode Core Cogs (All 4 I/O Pillars) & Fault Ring
Mission: Deliver complete Safe-Mode implementations of Filesystem, JSON, VFS,
         MasterLoader, and BIOS fault forwarding to log_fallback & debug_fallback.

  ### For Game Designers & Non-Programmers:
  Guarantees file reads, directory safety, path searching, and save blocking work
  predictably together even when external folders or primary loaders fail.

  ### For Systems Programmers & Engine Architects:
  Houses shared write/mkdir mutation guards (`io_fb_guard_disk_mutation`),
  single-source span scanners, hydrated 36-dir VFS routing, 5-block memory
  ingestion, and FIFO fault ring bridges into `log_fallback` and `debug_fallback`.
@triggers
  - Primary Cogs  : `io_fallback_fs_*`, `io_fallback_json_*`, `io_fallback_path_*`,
                    `io_fallback_loader_*`, `io_fallback_record_fault()`,
                    `io_fallback_downgrade_safe_mode()`
--------------------------------------------------------------------------------
*/

/* --- Pillar 1: Filesystem Core BIOS Fallbacks --- */

/**
 * @brief Test existence of either a POSIX filesystem path or an embedded `"rom://..."` URI.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Checks whether a save file, config blueprint, or folder exists cleanly on
 *   external disk or inside factory ROM memory.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Checks `"rom://"` URIs via `io_fallback_find_rom_asset()` first without syscalls,
 *   then delegates disk queries to POSIX `stat()`.
 *
 * @param[in] path Disk path or `"rom://..."` virtual URI.
 * @return `true` if path resolves to an existing file, directory, or ROM entry; `false` otherwise.
 * @triggers Enforces null/empty guards safely with zero heap allocations.
 */
bool io_fallback_fs_exists(const char *path)
{
    /* Step 1: Reject NULL or empty path strings */
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    /* Step 2: Check `.rodata` ROM vault before issuing POSIX `stat()` */
    if (io_fb_is_rom_uri(path)) {
        return io_fallback_find_rom_asset(path, NULL) != NULL;
    }
    struct stat st;
    return (stat(path, &st) == 0);
}

/**
 * @brief Verify that `path` points to a readable regular file or embedded `.rodata` ROM asset.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Ensures the target path is an actual readable file rather than a directory or
 *   missing entry before opening.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Matches `"rom://"` assets via `io_fallback_find_rom_asset()`, or verifies `S_ISREG(st.st_mode)`
 *   on POSIX targets cleanly.
 *
 * @param[in] path Disk path or `"rom://..."` virtual URI.
 * @return `true` if regular readable file or `.rodata` asset; `false` otherwise.
 * @triggers Executes side-effect-free inspection without disturbing fault counters.
 */
bool io_fallback_fs_is_regular(const char *path)
{
    /* Step 1: Reject NULL or empty paths */
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    /* Step 2: Resolve ROM URI or confirm POSIX regular-file mode */
    if (io_fb_is_rom_uri(path)) {
        return io_fallback_find_rom_asset(path, NULL) != NULL;
    }
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode);
}

/**
 * @brief Measure byte size of a POSIX regular file or embedded `"rom://..."` asset.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Checks how big a config or data file is before loading so oversized inputs cannot
 *   clip silently or overflow static buffers.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Queries `.rodata` ROM table length or POSIX `stat().st_size`, returning `-1L`
 *   when absent or non-regular.
 *
 * @param[in] path Disk path or `"rom://..."` virtual URI.
 * @return Non-negative byte size on hit; `-1L` if missing, invalid, or non-regular.
 * @triggers Validates input pointer cleanly prior to metadata query.
 */
long io_fallback_fs_get_size(const char *path)
{
    /* Step 1: Reject NULL or empty paths */
    if (path == NULL || path[0] == '\0') {
        return -1L;
    }

    /* Step 2: Query ROM descriptor length directly if prefixed with `"rom://"` */
    if (io_fb_is_rom_uri(path)) {
        size_t sz = 0U;
        if (io_fallback_find_rom_asset(path, &sz) == NULL) {
            return -1L;
        }
        return (long)sz;
    }

    /* Step 3: Query POSIX regular file byte count */
    struct stat st;
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) {
        return -1L;
    }
    return (long)st.st_size;
}

/**
 * @brief Populate `io_fb_file_info_t` metadata for a disk target or virtual ROM URI.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Inspects whether an asset lives in factory ROM memory or storage folders, and
 *   distinguishes files from directories safely.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Zero-initializes `out_info` with `size_bytes = -1L` and populates `exists`,
 *   `is_regular`, `is_directory`, `is_rom_virtual`, and `size_bytes` without allocation.
 *
 * @param[in]  path     Disk path or `"rom://..."` virtual URI.
 * @param[out] out_info Caller metadata struct populated on success.
 * @return `true` when path exists cleanly on disk or ROM; `false` on error/miss.
 * @triggers Resets output struct deterministically even when target path does not exist.
 */
bool io_fallback_fs_inspect(const char *path, io_fb_file_info_t *out_info)
{
    /* Step 1: Reject NULL destination and zero-initialize metadata record */
    if (out_info == NULL) {
        return false;
    }
    memset(out_info, 0, sizeof(*out_info));
    out_info->size_bytes = -1L;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    /* Step 2: Populate `.rodata` ROM metadata when prefixed with `"rom://"` */
    if (io_fb_is_rom_uri(path)) {
        size_t sz = 0U;
        if (io_fallback_find_rom_asset(path, &sz) == NULL) {
            return false;
        }
        out_info->exists = true;
        out_info->is_regular = true;
        out_info->is_directory = false;
        out_info->is_rom_virtual = true;
        out_info->size_bytes = (long)sz;
        return true;
    }

    /* Step 3: Execute POSIX `stat()` on physical path */
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }

    out_info->exists = true;
    out_info->is_regular = S_ISREG(st.st_mode);
    out_info->is_directory = S_ISDIR(st.st_mode);
    out_info->is_rom_virtual = false;
    out_info->size_bytes = S_ISREG(st.st_mode) ? (long)st.st_size : 0L;
    return true;
}

/**
 * @brief Read a disk file or `"rom://..."` asset strictly within caller and BIOS byte ceilings.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads saved configs cleanly from disk or built-in ROM while guaranteeing large or
 *   corrupt files never overrun caller memory.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Enforces active `s_io_fb.config.max_config_bytes` (`IO_ERR_TRUNCATED_READ`), always
 *   NUL-terminates `buf`, copies `.rodata` ROM streams directly on virtual URIs (`rom_hits++`),
 *   and logs `IO_ERR_ROM_MISSING` / `IO_ERR_READ_FAILURE` / `IO_ERR_TRUNCATED_READ` faults.
 *
 * @param[in]  path           Disk path or `"rom://..."` virtual URI.
 * @param[out] buf            Caller output buffer receiving NUL-terminated text.
 * @param[in]  buf_size       Byte capacity of `buf`.
 * @param[out] out_bytes_read Optional pointer receiving exact byte count (`0U` on fault).
 * @return `true` when complete payload fit inside buffer and policy cap; `false` on fault.
 * @triggers Records BIOS faults via `io_fallback_record_fault()` on ROM miss, read error, or clipping.
 */
bool io_fallback_fs_read_bounded(const char *path,
                                 char *buf,
                                 size_t buf_size,
                                 size_t *out_bytes_read)
{
    /* Step 1: Clear output counter and enforce non-empty path + buffer validity */
    if (out_bytes_read != NULL) {
        *out_bytes_read = 0U;
    }
    if (path == NULL || path[0] == '\0' || !io_fb_buf_is_valid(buf, buf_size)) {
        if (io_fb_buf_is_valid(buf, buf_size)) {
            buf[0] = '\0';
        }
        return false;
    }

    /* Step 2: Arm BIOS and compute effective read limit (`min(buf_size, max_config_bytes)`) */
    io_fb_auto_ensure_booted();

    size_t policy_cap = (s_io_fb.config.max_config_bytes > 0U &&
                         s_io_fb.config.max_config_bytes < buf_size)
        ? s_io_fb.config.max_config_bytes
        : buf_size;

    /* Step 3: Serve virtual `"rom://..."` payloads directly from `.rodata` */
    if (io_fb_is_rom_uri(path)) {
        size_t rom_size = 0U;
        const char *rom_data = io_fallback_find_rom_asset(path, &rom_size);
        if (rom_data == NULL) {
            buf[0] = '\0';
            io_fallback_record_fault("io_fallback.fs", "fs_read_bounded",
                                     "ROM asset not found", IO_ERR_ROM_MISSING);
            return false;
        }
        if (rom_size >= buf_size || rom_size > s_io_fb.config.max_config_bytes) {
            size_t copy_len = buf_size - 1U;
            if (copy_len > 0U) {
                memcpy(buf, rom_data, copy_len);
            }
            buf[copy_len] = '\0';
            io_fallback_record_fault("io_fallback.fs", "fs_read_bounded",
                                     "ROM payload exceeds caller or policy cap",
                                     IO_ERR_TRUNCATED_READ);
            return false;
        }
        memcpy(buf, rom_data, rom_size);
        buf[rom_size] = '\0';
        s_io_fb.rom_hits++;
        if (out_bytes_read != NULL) {
            *out_bytes_read = rom_size;
        }
        return true;
    }

    /* Step 4: Stream regular disk file with truncation probe and NUL termination */
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        buf[0] = '\0';
        return false;
    }

    size_t max_read = policy_cap - 1U;
    size_t read_bytes = fread(buf, 1U, max_read, f);
    buf[read_bytes] = '\0';
    bool io_err = (ferror(f) != 0);
    int next_char = fgetc(f);
    (void)fclose(f);

    if (io_err || (next_char != EOF && s_io_fb.config.reject_on_truncation)) {
        buf[buf_size - 1U] = '\0';
        io_fallback_record_fault("io_fallback.fs", "fs_read_bounded",
                                 io_err ? "Disk stream failed read"
                                        : "Disk file exceeds bounded buffer or max_config_bytes",
                                 io_err ? IO_ERR_READ_FAILURE : IO_ERR_TRUNCATED_READ);
        return false;
    }

    if (out_bytes_read != NULL) {
        *out_bytes_read = read_bytes;
    }
    return true;
}

/**
 * @brief Persist a buffer to disk honoring explicit permissions, flush, atomic rename & safety locks.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Shared core save routine guaranteeing parent folders exist, write-protected Safe Mode
 *   cannot be bypassed, and atomic staging prevents half-written saves on power loss.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Delegates path ceiling, traversal (`..`), and `read_only_mode` checks cleanly to
 *   `io_fb_guard_disk_mutation()`, ensures parent directories via
 *   `io_fallback_fs_ensure_dir_mode()`, writes atomically to `.tl_tmp` sibling + `rename()`,
 *   and honors `flush_on_write` cleanly without heap allocations.
 *
 * @param[in] path            Target file path.
 * @param[in] data            Source payload pointer (`data_len` bytes).
 * @param[in] data_len        Exact byte length to write.
 * @param[in] dir_mode        POSIX directory permissions mask when parent mkdir is needed.
 * @param[in] flush_on_write  `true` to call `fflush()` before closing handle.
 * @param[in] atomic_write    `true` to stage `.tl_tmp` sibling and atomically `rename()`.
 * @param[in] block_traversal `true` to reject paths containing `..` traversal segments.
 * @return `true` when complete write and rename succeeded; `false` on guard block or I/O error.
 * @triggers Enforces `io_fb_guard_disk_mutation()` and logs I/O write/rename faults.
 */
bool io_fallback_fs_write_with_flags(const char *path,
                                     const char *data,
                                     size_t data_len,
                                     unsigned int dir_mode,
                                     bool flush_on_write,
                                     bool atomic_write,
                                     bool block_traversal)
{
    /* Step 1: Guard NULL inputs and enforce BIOS path/traversal/read-only locks */
    if (path == NULL || path[0] == '\0' || data == NULL) {
        return false;
    }

    io_fb_auto_ensure_booted();

    if (!io_fb_guard_disk_mutation("fs_write_with_flags",
                                   path,
                                   block_traversal,
                                   "Target write path exceeds path_max_bytes",
                                   "Blocked parent traversal '..' on write target",
                                   "Write blocked under read-only floppy latch")) {
        return false;
    }

    /* Step 2: Materialize missing parent directory hierarchy safely */
    char parent[IO_FB_PATH_MAX];
    if (str_parent_dir(parent, sizeof(parent), path) && strcmp(parent, ".") != 0) {
        if (!io_fallback_fs_ensure_dir_mode(parent, dir_mode, block_traversal)) {
            return false;
        }
    }

    /* Step 3: Stage into temporary sibling when atomic_write is armed */
    char tmp_path[IO_FB_PATH_MAX + 16U];
    const char *open_target = path;
    bool using_atomic = false;
    if (atomic_write) {
        int n = snprintf(tmp_path, sizeof(tmp_path), "%s.tl_tmp", path);
        if (n > 0 && (size_t)n < sizeof(tmp_path)) {
            open_target = tmp_path;
            using_atomic = true;
        }
    }

    /* Step 4: Write bounded payload, flush if requested, close, and commit rename */
    FILE *f = fopen(open_target, "w");
    if (f == NULL) {
        io_fallback_record_fault("io_fallback.fs", "fs_write_with_flags",
                                 "Failed opening target file for write", IO_ERR_READ_FAILURE);
        return false;
    }

    bool ok = true;
    if (data_len > 0U) {
        size_t written = fwrite(data, 1U, data_len, f);
        if (written != data_len) {
            ok = false;
        }
    }
    if (flush_on_write && fflush(f) != 0) {
        ok = false;
    }
    if (fclose(f) != 0) {
        ok = false;
    }

    if (using_atomic) {
        if (!ok || rename(tmp_path, path) != 0) {
            (void)remove(tmp_path);
            ok = false;
        }
    }

    if (!ok) {
        io_fallback_record_fault("io_fallback.fs", "fs_write_with_flags",
                                 "Incomplete write, flush, or atomic rename failure",
                                 IO_ERR_READ_FAILURE);
    }
    return ok;
}

/**
 * @brief Write a buffer to disk using active BIOS policy flags and Safe-Mode locks.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Convenience safe-write wrapper applying the active I/O profile's atomic-save,
 *   flush, directory permissions, and floppy read-only settings automatically.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Arms BIOS on demand and forwards `s_io_fb.config` parameters directly into
 *   `io_fallback_fs_write_with_flags()`.
 *
 * @param[in] path     Target filesystem path.
 * @param[in] data     Source byte buffer (`data_len` bytes).
 * @param[in] data_len Exact byte count to write.
 * @return `true` when write succeeded cleanly; `false` on guard block or failure.
 * @triggers Forwards all Safe-Mode policy enforcement to `io_fallback_fs_write_with_flags()`.
 */
bool io_fallback_fs_write_bounded(const char *path, const char *data, size_t data_len)
{
    /* Step 1: Ensure BIOS armed and delegate with live policy toggles */
    io_fb_auto_ensure_booted();
    return io_fallback_fs_write_with_flags(
        path,
        data,
        data_len,
        s_io_fb.config.posix_dir_mode,
        s_io_fb.config.flush_on_write,
        s_io_fb.config.atomic_write,
        s_io_fb.config.block_path_traversal
    );
}

/**
 * @brief Recursively create a directory hierarchy honoring explicit permissions & traversal policy.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Builds missing folders step-by-step (like `mkdir -p`) while enforcing workspace
 *   sandbox protections and Safe-Mode read-only locks.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Guards bounds/traversal/read-only state via `io_fb_guard_disk_mutation()`, strips
 *   trailing slashes in stack buffer `tmp[IO_FB_PATH_MAX]`, and creates missing intermediate
 *   segments atomically with `safe_mode`.
 *
 * @param[in] dir_path        Directory path hierarchy to materialize.
 * @param[in] posix_mode      POSIX creation mode mask (`0` falls back to `0755`).
 * @param[in] block_traversal `true` to reject paths containing `..` traversal segments.
 * @return `true` when hierarchy exists cleanly; `false` on RO lock, guard fault, or collision.
 * @triggers Enforces `io_fb_guard_disk_mutation()` prior to touching filesystem state.
 */
bool io_fallback_fs_ensure_dir_mode(const char *dir_path,
                                    unsigned int posix_mode,
                                    bool block_traversal)
{
    /* Step 1: Reject NULL/empty inputs and run shared disk mutation guards */
    if (dir_path == NULL || dir_path[0] == '\0') {
        return false;
    }

    io_fb_auto_ensure_booted();

    if (!io_fb_guard_disk_mutation("fs_ensure_dir_mode",
                                   dir_path,
                                   block_traversal,
                                   "Directory path exceeds path_max_bytes",
                                   "Blocked parent traversal '..' on mkdir target",
                                   "mkdir blocked under read-only floppy latch")) {
        return false;
    }

    /* Step 2: Normalize POSIX permissions mask and trim trailing slashes in stack copy */
    unsigned int safe_mode = (posix_mode != 0U && posix_mode <= 0777U)
        ? posix_mode
        : IO_FB_DEFAULT_DIR_MODE;

    char tmp[IO_FB_PATH_MAX];
    str_safe_copy(tmp, dir_path, sizeof(tmp));
    size_t len = strlen(tmp);

    while (len > 1U && tmp[len - 1U] == '/') {
        tmp[len - 1U] = '\0';
        len--;
    }

    /* Step 3: Walk path prefixes cleanly and create missing parent directories */
    for (char *p = tmp + 1; *p != '\0'; p++) {
        if (*p == '/') {
            *p = '\0';
            struct stat st;
            if (stat(tmp, &st) != 0) {
                if (mkdir(tmp, (mode_t)safe_mode) != 0 && errno != EEXIST) {
                    return false;
                }
            } else if (!S_ISDIR(st.st_mode)) {
                return false;
            }
            *p = '/';
        }
    }

    /* Step 4: Materialize leaf directory and verify final mode is a directory */
    struct stat st;
    if (stat(tmp, &st) != 0) {
        if (mkdir(tmp, (mode_t)safe_mode) != 0 && errno != EEXIST) {
            return false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        return false;
    }
    return true;
}

/**
 * @brief Recursively ensure a directory hierarchy exists under active BIOS policy defaults.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Creates missing save or diagnostic folders using active engine safety settings.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Arms BIOS on demand and forwards `s_io_fb.config.posix_dir_mode` and
 *   `block_path_traversal` into `io_fallback_fs_ensure_dir_mode()`.
 *
 * @param[in] dir_path Directory hierarchy path to create if absent.
 * @return `true` when directory exists cleanly; `false` on RO lock, guard fault, or error.
 * @triggers Applies active BIOS permissions and read-only lock via `io_fallback_fs_ensure_dir_mode()`.
 */
bool io_fallback_fs_ensure_dir(const char *dir_path)
{
    /* Step 1: Arm BIOS and delegate mkdir hierarchy with live policy toggles */
    io_fb_auto_ensure_booted();
    return io_fallback_fs_ensure_dir_mode(
        dir_path,
        s_io_fb.config.posix_dir_mode,
        s_io_fb.config.block_path_traversal
    );
}

/* --- Pillar 2: JSON Core BIOS Fallbacks --- */

/**
 * @brief Locate the value token start for `key` inside a NUL-terminated JSON buffer.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Safe emergency key finder used during startup recovery so quoted descriptions
 *   containing colons or braces never fool the fallback config reader.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Rejects NULL or empty inputs cleanly and delegates `[json, json + strlen(json))`
 *   linear scanning to `Sub-Assembly 2.5` `io_fb_find_key_span()`.
 *
 * @param[in] json NUL-terminated UTF-8 JSON buffer.
 * @param[in] key  Target JSON property key without quotes.
 * @return Pointer into `json` at value token start, or NULL if absent/invalid.
 * @triggers Validates inputs deterministically with zero `.bss` mutation.
 */
const char *io_fallback_json_find_key(const char *json, const char *key)
{
    /* Step 1: Reject NULL or empty arguments cleanly */
    if (json == NULL || key == NULL || key[0] == '\0') {
        return NULL;
    }

    /* Step 2: Scan whole document span via quote-aware BIOS scanner */
    return io_fb_find_key_span(json, json + strlen(json), key);
}

/**
 * @brief Isolate a balanced `{...}` or `[...]` block matching `key` within JSON depth caps.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Extracts named nested blocks (such as `block_3_body` or `diagnostic_aliases`)
 *   cleanly while stopping runaway deeply nested payloads.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Validates output pointers and forwards to `io_fb_find_block_span()`, which tracks
 *   escape sequences and emits `IO_ERR_JSON_DEPTH_EXCEEDED` above `IO_FB_MAX_JSON_DEPTH`.
 *
 * @param[in]  json      NUL-terminated UTF-8 JSON buffer.
 * @param[in]  key       Property key whose delimited value block to isolate.
 * @param[in]  open_ch   Opening delimiter (`'{'` or `'['`).
 * @param[in]  close_ch  Matching closing delimiter (`'}'` or `']'`).
 * @param[out] out_start Receives pointer to `open_ch` on success.
 * @param[out] out_len   Receives balanced slice length including outer delimiters.
 * @return `true` if block found and balanced cleanly; `false` otherwise.
 * @triggers Emits `IO_ERR_JSON_DEPTH_EXCEEDED` fault if nesting exceeds 32 levels.
 */
bool io_fallback_json_find_block(const char *json,
                                 const char *key,
                                 char open_ch,
                                 char close_ch,
                                 const char **out_start,
                                 size_t *out_len)
{
    /* Step 1: Enforce valid input/output pointers prior to span scan */
    if (json == NULL || key == NULL || out_start == NULL || out_len == NULL) {
        return false;
    }

    /* Step 2: Delegate balanced bracket tracing cleanly to 2.5 helper */
    return io_fb_find_block_span(json, json + strlen(json), key,
                                 open_ch, close_ch, out_start, out_len);
}

/**
 * @brief Extract and unescape a JSON string field cleanly into caller buffer `out_buf`.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Safely reads config text tokens (such as profile names, folder paths, or versions)
 *   into fixed storage without risk of unterminated strings.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Pre-terminates `out_buf[0] = '\0'` whenever valid, then forwards to
 *   `io_fb_get_string_span()` with zero heap allocations.
 *
 * @param[in]  json     NUL-terminated UTF-8 JSON buffer.
 * @param[in]  key      Target property key name.
 * @param[out] out_buf  Caller destination buffer receiving NUL-terminated decoded string.
 * @param[in]  out_size Capacity of `out_buf` in bytes.
 * @return `true` if string existed and fit cleanly inside `out_size`; `false` otherwise.
 * @triggers Clears destination buffer deterministically before parsing.
 */
bool io_fallback_json_get_string(const char *json,
                                 const char *key,
                                 char *out_buf,
                                 size_t out_size)
{
    /* Step 1: Guarantee clean empty NUL-terminated buffer on early failure */
    if (io_fb_buf_is_valid(out_buf, out_size)) {
        out_buf[0] = '\0';
    }
    if (json == NULL || key == NULL || !io_fb_buf_is_valid(out_buf, out_size)) {
        return false;
    }

    /* Step 2: Decode matched JSON string field bounded by `out_size` */
    return io_fb_get_string_span(json, json + strlen(json), key, out_buf, out_size);
}

/**
 * @brief Extract a signed base-10 integer value for `key` with range verification.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads numeric settings (buffer caps, nesting limits, symlink depths) from JSON
 *   safely without misinterpreting malformed text as zero.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Delegates span parsing to `io_fb_get_int_span()` using bounded `strtol(..., 10)`
 *   overflow and range (`[INT_MIN, INT_MAX]`) validation.
 *
 * @param[in]  json    NUL-terminated UTF-8 JSON buffer.
 * @param[in]  key     Target integer property key.
 * @param[out] out_val Destination pointer receiving parsed `int` value.
 * @return `true` when key held a valid base-10 integer; `false` otherwise.
 * @triggers Enforces null pointer guards strictly without modifying `*out_val` on error.
 */
bool io_fallback_json_get_int(const char *json, const char *key, int *out_val)
{
    /* Step 1: Reject NULL JSON/key/output pointers */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Parse range-checked signed integer value cleanly */
    return io_fb_get_int_span(json, json + strlen(json), key, out_val);
}

/**
 * @brief Extract an exact boolean (`true` or `false`) token for `key`.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads on/off feature toggles cleanly while rejecting accidental partial text matches.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Forwards to `io_fb_get_bool_span()` checking delimiter boundaries after `true`/`false`.
 *
 * @param[in]  json    NUL-terminated UTF-8 JSON buffer.
 * @param[in]  key     Target boolean property key.
 * @param[out] out_val Destination pointer receiving parsed boolean value.
 * @return `true` when exact boolean literal existed; `false` otherwise.
 * @triggers Enforces exact token delimiter validation with zero side effects.
 */
bool io_fallback_json_get_bool(const char *json, const char *key, bool *out_val)
{
    /* Step 1: Reject NULL inputs prior to token scan */
    if (json == NULL || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Extract boundary-checked boolean literal */
    return io_fb_get_bool_span(json, json + strlen(json), key, out_val);
}

/* --- Pillar 3: VFS / PathFinder Core BIOS Fallbacks --- */

/**
 * @brief Return the currently active BIOS virtual filesystem root directory string.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Shows which base data directory the emergency VFS locator checks first.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Arms BIOS lazily if needed and returns immutable pointer to `.bss` `s_io_fb.active_root`.
 *
 * @return Non-NULL NUL-terminated active VFS root string.
 * @triggers Lazy-arms BIOS defaults on first access.
 */
const char *io_fallback_path_get_root(void)
{
    /* Step 1: Arm BIOS and return active root path from static storage */
    io_fb_auto_ensure_booted();
    return s_io_fb.active_root;
}

/**
 * @brief Set or reset the Safe-Mode BIOS VFS root directory path within length limits.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Switches where emergency file lookup searches for data, or resets to `"game/data"`
 *   when given NULL or an empty string.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Rejects paths exceeding `sizeof(s_io_fb.active_root)` while logging
 *   `IO_ERR_PATH_OVERFLOW` into BIOS fault telemetry cleanly.
 *
 * @param[in] root_path New VFS root directory, or NULL/empty to restore `IO_FB_DEFAULT_ROOT`.
 * @return `true` when updated or reset cleanly; `false` when `root_path` overflows limit.
 * @triggers Logs `IO_ERR_PATH_OVERFLOW` via `io_fallback_record_fault()` on capacity failure.
 */
bool io_fallback_path_set_root(const char *root_path)
{
    /* Step 1: Arm BIOS and reset to default root if NULL or empty */
    io_fb_auto_ensure_booted();
    if (root_path == NULL || root_path[0] == '\0') {
        str_safe_copy(s_io_fb.active_root, IO_FB_DEFAULT_ROOT, sizeof(s_io_fb.active_root));
        return true;
    }

    /* Step 2: Enforce `IO_FB_MAX_ROOT_LEN` capacity and record overflow telemetry */
    if (strlen(root_path) >= sizeof(s_io_fb.active_root)) {
        io_fallback_record_fault("io_fallback.vfs", "path_set_root",
                                 "Configured root exceeds IO_FB_MAX_ROOT_LEN",
                                 IO_ERR_PATH_OVERFLOW);
        return false;
    }
    str_safe_copy(s_io_fb.active_root, root_path, sizeof(s_io_fb.active_root));
    return true;
}

/**
 * @brief Resolve a domain/relative asset query to disk or fallback `"rom://..."` URI.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Locates requested config, save, or story files regardless of launch working directory,
 *   honoring workspace overrides and automatically falling back to built-in ROM assets
 *   when running offline or diskless.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Enforces sandbox parent traversal guard (`..` -> `IO_ERR_PATH_OVERFLOW`), checks direct
 *   `"rom://"` / cwd paths, hydrated `env_override_var` (`TL_DATA_ROOT`), `config_precedence`
 *   tiers (`game/config/`, `engine/config/`, `game/data/presentation/config/`),
 *   `candidate_roots x domain_search_paths`, and `.rodata` ROM fallback cleanly without heap.
 *
 * @param[in]  domain   Logical domain hint (e.g. `"config"`, `"entities"`, or NULL).
 * @param[in]  rel_path Relative asset path or `"rom://..."` virtual URI.
 * @param[out] out_path Caller output buffer populated on match.
 * @param[in]  out_size Byte capacity of `out_path`.
 * @return `true` when matched on disk or `.rodata` ROM; `false` on miss, invalid args, or traversal block.
 * @triggers Logs `IO_ERR_PATH_OVERFLOW` into BIOS ring on blocked `..` traversal attempts.
 */
bool io_fallback_path_resolve(const char *domain,
                              const char *rel_path,
                              char *out_path,
                              size_t out_size)
{
    /* Step 1: Pre-clear output buffer and guard invalid inputs */
    if (io_fb_buf_is_valid(out_path, out_size)) {
        out_path[0] = '\0';
    }
    if (rel_path == NULL || rel_path[0] == '\0' || !io_fb_buf_is_valid(out_path, out_size)) {
        return false;
    }

    io_fb_auto_ensure_booted();

    /* Step 2: Block external parent traversal ('..') when sandbox guard enabled */
    if (!io_fb_is_rom_uri(rel_path) &&
        (s_io_fb.config.block_path_traversal || s_io_fb.vfs_routes.block_path_traversal) &&
        io_fb_has_parent_traversal(rel_path)) {
        io_fallback_record_fault("io_fallback.vfs", "path_resolve",
                                 "Blocked parent directory traversal '..' in VFS query",
                                 IO_ERR_PATH_OVERFLOW);
        return false;
    }

    /* Step 3: Resolve direct `"rom://..."` URIs or direct working-directory paths */
    if (io_fb_is_rom_uri(rel_path)) {
        if (io_fallback_find_rom_asset(rel_path, NULL) != NULL && strlen(rel_path) < out_size) {
            str_safe_copy(out_path, rel_path, out_size);
            return true;
        }
        return false;
    }
    if (io_fallback_fs_is_regular(rel_path) && strlen(rel_path) < out_size) {
        str_safe_copy(out_path, rel_path, out_size);
        return true;
    }

    const char *clean = io_fb_strip_path_prefixes(rel_path);
    char probe[IO_FB_PATH_MAX * 2U];

    /* Step 4: Check configured environment override root (`TL_DATA_ROOT`) */
    if (s_io_fb.vfs_routes.env_override_var[0] != '\0') {
        const char *env_root = getenv(s_io_fb.vfs_routes.env_override_var);
        if (env_root != NULL && env_root[0] != '\0') {
            int n = snprintf(probe, sizeof(probe), "%s/%s", env_root, clean);
            if (n > 0 && (size_t)n < sizeof(probe) && (size_t)n < out_size &&
                io_fallback_fs_is_regular(probe)) {
                str_safe_copy(out_path, probe, out_size);
                return true;
            }
            for (size_t d = 0U; d < s_io_fb.vfs_routes.domain_search_count; d++) {
                n = snprintf(probe, sizeof(probe), "%s/%s%s",
                             env_root, s_io_fb.vfs_routes.domain_search_paths[d], clean);
                if (n > 0 && (size_t)n < sizeof(probe) && (size_t)n < out_size &&
                    io_fallback_fs_is_regular(probe)) {
                    str_safe_copy(out_path, probe, out_size);
                    return true;
                }
            }
        }
    }

    /* Step 5: Probe layered config precedence tiers when config query detected */
    bool is_cfg = (domain != NULL && strcmp(domain, "config") == 0) ||
                  (strncmp(rel_path, "config/", 7) == 0) ||
                  (strncmp(rel_path, "engine/config/", 14) == 0) ||
                  (strncmp(clean, "io/", 3) == 0);
    if (is_cfg) {
        const char *cfg_sub = (strncmp(clean, "config/", 7) == 0) ? (clean + 7) : clean;
        for (size_t t = 0U; t < s_io_fb.vfs_routes.config_precedence_count; t++) {
            const char *tier = s_io_fb.vfs_routes.config_precedence[t];
            int n = snprintf(probe, sizeof(probe), "%s%s", tier, cfg_sub);
            if (n > 0 && (size_t)n < sizeof(probe) && (size_t)n < out_size &&
                io_fallback_fs_is_regular(probe)) {
                str_safe_copy(out_path, probe, out_size);
                return true;
            }
            n = snprintf(probe, sizeof(probe), "../%s%s", tier, cfg_sub);
            if (n > 0 && (size_t)n < sizeof(probe) && (size_t)n < out_size &&
                io_fallback_fs_is_regular(probe)) {
                str_safe_copy(out_path, probe, out_size);
                return true;
            }
        }
    }

    /* Step 6: Probe active root and hydrated candidate_roots x domain_search_paths */
    int n_root = snprintf(probe, sizeof(probe), "%s/%s", s_io_fb.active_root, clean);
    if (n_root > 0 && (size_t)n_root < sizeof(probe) && (size_t)n_root < out_size &&
        io_fallback_fs_is_regular(probe)) {
        str_safe_copy(out_path, probe, out_size);
        return true;
    }

    for (size_t r = 0U; r < s_io_fb.vfs_routes.candidate_root_count; r++) {
        for (size_t d = 0U; d < s_io_fb.vfs_routes.domain_search_count; d++) {
            int n = snprintf(probe, sizeof(probe), "%s/%s%s",
                             s_io_fb.vfs_routes.fallback_candidate_roots[r],
                             s_io_fb.vfs_routes.domain_search_paths[d], clean);
            if (n > 0 && (size_t)n < sizeof(probe) && (size_t)n < out_size &&
                io_fallback_fs_is_regular(probe)) {
                str_safe_copy(out_path, probe, out_size);
                return true;
            }
        }
    }

    /* Step 7: Synthesize `"rom://..."` URI if compiled ROM matches `clean` */
    if (s_io_fb.config.rom_fallback_enabled && io_fallback_find_rom_asset(clean, NULL) != NULL) {
        int n = snprintf(probe, sizeof(probe), "%s%s", IO_FB_ROM_URI_PREFIX, clean);
        if (n > 0 && (size_t)n < sizeof(probe) && (size_t)n < out_size) {
            str_safe_copy(out_path, probe, out_size);
            s_io_fb.rom_hits++;
            return true;
        }
    }

    return false;
}

/* --- Pillar 4: MasterLoader Core BIOS Fallbacks --- */

/**
 * @brief Parse an in-memory JSON payload using the Tier-0 BIOS 5-block loader envelope.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Verifies that loaded JSON documents actually include a valid metadata block and
 *   payload section (`block_3_body`) before reading game values from them.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Scans `block_1_metadata`, top-level `schema_id`, and `block_3_body` bounds in place
 *   without allocation, recording `IO_ERR_JSON_SYNTAX` on missing `block_3_body`.
 *
 * @param[out] loader    Caller-owned fallback loader descriptor initialized on success.
 * @param[in]  json_text In-memory UTF-8 JSON buffer (must remain valid for `loader` lifetime).
 * @param[in]  json_len  Exact payload length in bytes.
 * @return `true` when `block_3_body` isolated cleanly; `false` on null inputs or missing body.
 * @triggers Records `IO_ERR_JSON_SYNTAX` via `io_fallback_record_fault()` on invalid envelopes.
 */
bool io_fallback_loader_from_memory(io_fb_loader_t *loader,
                                    const char *json_text,
                                    size_t json_len)
{
    /* Step 1: Guard null/empty payload and initialize loader error status */
    if (loader == NULL || json_text == NULL || json_len == 0U) {
        if (loader != NULL) {
            loader->status = IO_FB_LOADER_ERR_NULL;
        }
        return false;
    }

    const char *json_end = json_text + json_len;
    loader->raw_buf = json_text;
    loader->raw_len = json_len;
    loader->body_start = NULL;
    loader->body_len = 0U;
    loader->header.schema_id[0] = '\0';

    /* Step 2: Hydrate `block_1_metadata` provenance fields if present */
    const char *meta_start = NULL;
    size_t meta_len = 0U;
    if (io_fb_find_block_span(json_text, json_end, "block_1_metadata", '{', '}',
                              &meta_start, &meta_len) &&
        meta_start != NULL && meta_len > 0U) {
        const char *meta_end = meta_start + meta_len;
        loader->header.is_5block = true;
        io_fb_get_string_span(meta_start, meta_end, "project", loader->header.project,
                              sizeof(loader->header.project));
        io_fb_get_string_span(meta_start, meta_end, "file", loader->header.file,
                              sizeof(loader->header.file));
        io_fb_get_string_span(meta_start, meta_end, "id", loader->header.id,
                              sizeof(loader->header.id));
        io_fb_get_string_span(meta_start, meta_end, "layer", loader->header.layer,
                              sizeof(loader->header.layer));
        io_fb_get_string_span(meta_start, meta_end, "version", loader->header.version,
                              sizeof(loader->header.version));
    } else {
        loader->header.is_5block = false;
        loader->header.project[0] = '\0';
        loader->header.file[0] = '\0';
        loader->header.id[0] = '\0';
        loader->header.layer[0] = '\0';
        loader->header.version[0] = '\0';
    }

    (void)io_fb_get_string_span(json_text, json_end, "schema_id", loader->header.schema_id,
                                sizeof(loader->header.schema_id));

    /* Step 3: Isolate mandatory `block_3_body` payload span or record syntax fault */
    if (!io_fb_find_block_span(json_text, json_end, "block_3_body", '{', '}',
                               &loader->body_start, &loader->body_len)) {
        loader->status = IO_FB_LOADER_ERR_BODY_MISSING;
        io_fallback_record_fault("io_fallback.loader", "loader_from_memory",
                                 "Missing block_3_body in envelope", IO_ERR_JSON_SYNTAX);
        return false;
    }

    loader->status = IO_FB_LOADER_OK;
    return true;
}

/**
 * @brief Resolve, bounded-read, and parse a 5-block JSON asset via Safe-Mode BIOS pillars.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Single emergency loader function that locates any JSON file across disk or ROM,
 *   safely reads it into caller scratch space, and unlocks `block_3_body` fields.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Chains `io_fallback_path_resolve()` -> `io_fallback_fs_read_bounded()` ->
 *   `io_fallback_loader_from_memory()` using caller-supplied `scratch_buf`, stamping
 *   `loaded_from_rom` cleanly and mapping error statuses on failure.
 *
 * @param[out] loader       Caller-owned fallback loader handle initialized cleanly.
 * @param[in]  domain       Logical domain hint (`"config"`, `"entities"`, or NULL).
 * @param[in]  rel_path     Relative asset path or `"rom://..."` URI.
 * @param[out] scratch_buf  Caller-owned scratch buffer receiving file payload bytes.
 * @param[in]  scratch_size Capacity of `scratch_buf` in bytes.
 * @return `true` if asset resolved, fit cleanly in scratch, and parsed; `false` otherwise.
 * @triggers Sets typed `IO_FB_LOADER_ERR_*` status on `loader` cleanly across every failure seam.
 */
bool io_fallback_loader_load(io_fb_loader_t *loader,
                             const char *domain,
                             const char *rel_path,
                             char *scratch_buf,
                             size_t scratch_size)
{
    /* Step 1: Reject null pointers or invalid scratch buffer bounds */
    if (loader == NULL || rel_path == NULL || rel_path[0] == '\0' ||
        !io_fb_buf_is_valid(scratch_buf, scratch_size)) {
        if (loader != NULL) {
            loader->status = IO_FB_LOADER_ERR_NULL;
        }
        return false;
    }

    /* Step 2: Zero handle and resolve target via VFS (disk or `"rom://"`) */
    memset(loader, 0, sizeof(*loader));
    if (!io_fallback_path_resolve(domain, rel_path, loader->resolved_path,
                                  sizeof(loader->resolved_path))) {
        loader->status = IO_FB_LOADER_ERR_NOT_FOUND;
        return false;
    }

    /* Step 3: Stream resolved file safely into caller-owned scratch buffer */
    size_t read_len = 0U;
    if (!io_fallback_fs_read_bounded(loader->resolved_path, scratch_buf,
                                     scratch_size, &read_len)) {
        loader->status = IO_FB_LOADER_ERR_OVERFLOW;
        return false;
    }

    /* Step 4: Mark ROM provenance and parse 5-block envelope in place */
    loader->header.loaded_from_rom = io_fb_is_rom_uri(loader->resolved_path);
    return io_fallback_loader_from_memory(loader, scratch_buf, read_len);
}

/**
 * @brief Extract a string field strictly from a parsed loader's `block_3_body` payload.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads text fields exclusively from the asset's payload data so matching keys in
 *   file headers never leak through accidentally.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Bounds scanning strictly to `[loader->body_start, loader->body_start + loader->body_len)`
 *   and pre-terminates `out_buf` on valid output pointers.
 *
 * @param[in]  loader   Valid parsed `io_fb_loader_t` handle.
 * @param[in]  key      Target property key inside `block_3_body`.
 * @param[out] out_buf  Destination buffer receiving NUL-terminated decoded text.
 * @param[in]  out_size Capacity of `out_buf` in bytes.
 * @return `true` if found inside `block_3_body` and fit cleanly; `false` otherwise.
 * @triggers Enforces scoped `body_start..body_len` boundaries strictly.
 */
bool io_fallback_loader_get_string(const io_fb_loader_t *loader,
                                   const char *key,
                                   char *out_buf,
                                   size_t out_size)
{
    /* Step 1: Pre-clear destination buffer and guard unloaded handles */
    if (io_fb_buf_is_valid(out_buf, out_size)) {
        out_buf[0] = '\0';
    }
    if (loader == NULL || loader->body_start == NULL || loader->body_len == 0U ||
        key == NULL || !io_fb_buf_is_valid(out_buf, out_size)) {
        return false;
    }

    /* Step 2: Query key exclusively within scoped `block_3_body` range */
    return io_fb_get_string_span(loader->body_start, loader->body_start + loader->body_len,
                                 key, out_buf, out_size);
}

/**
 * @brief Verify that a parsed loader's `schema_id` matches the caller's expected contract.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Prevents loading the wrong JSON preset into the wrong system (such as feeding a
 *   sound file into the I/O buffer table).
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Compares `loader->header.schema_id` against non-empty `expected_schema_id` via `strcmp`.
 *
 * @param[in] loader             Valid parsed `io_fb_loader_t` handle.
 * @param[in] expected_schema_id Required schema identifier string.
 * @return `true` on exact match; `false` on missing schema or mismatch.
 * @triggers Rejects empty/NULL schema arguments safely.
 */
bool io_fallback_loader_validate_schema(const io_fb_loader_t *loader,
                                        const char *expected_schema_id)
{
    /* Step 1: Reject invalid loader, empty expected ID, or absent header schema */
    if (loader == NULL || expected_schema_id == NULL || expected_schema_id[0] == '\0') {
        return false;
    }
    if (loader->header.schema_id[0] == '\0') {
        return false;
    }

    /* Step 2: Confirm exact schema identifier parity */
    return strcmp(loader->header.schema_id, expected_schema_id) == 0;
}

/* --- Fault Ring & Safe-Mode Downgrade --- */

/**
 * @brief Record a structured I/O fault in the BIOS ring and forward to log & debugger BIOSes.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Captures storage warnings and errors cleanly inside built-in emergency flight logs
 *   and tripwire ledgers so root causes stay visible even without working disk logs.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Reentrancy-guarded (`s_io_fb.in_fault_record`) FIFO writer appending to 16-slot
 *   `s_io_fb.faults[]`, mapping `IO_ERR_*` codes to `log_severity_t`, and forwarding
 *   outside `in_preflight` to `log_fallback_record()` and `debug_fallback_record_fault()`.
 *
 * @param[in] subsystem  Originating module identifier tag (e.g. `"io_fallback.fs"`).
 * @param[in] operation  Operation name encountering the anomaly.
 * @param[in] message    Concise human-readable fault message.
 * @param[in] error_code Canonical `IO_ERR_*` hexadecimal status code.
 * @triggers Writes BIOS FIFO slot and forwards telemetry into `log_fallback` and `debug_fallback`.
 */
void io_fallback_record_fault(const char *subsystem,
                              const char *operation,
                              const char *message,
                              uint16_t error_code)
{
    /* Step 1: Enforce reentrancy guard and ensure BIOS boot state is armed */
    if (s_io_fb.in_fault_record) {
        return;
    }
    s_io_fb.in_fault_record = true;
    io_fb_auto_ensure_booted();

    /* Step 2: Append sequence-stamped fault record into 16-slot circular ring */
    size_t slot = s_io_fb.fault_head;
    io_fallback_fault_entry_t *entry = &s_io_fb.faults[slot];
    s_io_fb.fault_sequence++;
    entry->sequence_id = s_io_fb.fault_sequence;
    entry->error_code = error_code;
    str_safe_copy(entry->subsystem, subsystem ? subsystem : "io", sizeof(entry->subsystem));
    str_safe_copy(entry->operation, operation ? operation : "op", sizeof(entry->operation));
    str_safe_copy(entry->message, message ? message : "I/O fault", sizeof(entry->message));

    s_io_fb.fault_head = (slot + 1U) % IO_FB_FAULT_RING_CAP;
    if (s_io_fb.fault_count < IO_FB_FAULT_RING_CAP) {
        s_io_fb.fault_count++;
    }

    /* Step 3: Map hexadecimal error code to log severity and debugger escalation flag */
    log_severity_t log_sev = LOG_SEV_WARNING;
    bool notify_debugger = false;
    switch (error_code) {
        case IO_ERR_READ_FAILURE:
        case IO_ERR_JSON_SYNTAX:
        case IO_ERR_JSON_DEPTH_EXCEEDED:
        case IO_ERR_PATH_OVERFLOW:
        case IO_ERR_ROM_MISSING:
            log_sev = LOG_SEV_ERROR;
            notify_debugger = true;
            break;
        case IO_ERR_WRITE_BLOCKED_RO:
            log_sev = LOG_SEV_WARNING;
            notify_debugger = true;
            break;
        default:
            log_sev = LOG_SEV_WARNING;
            notify_debugger = false;
            break;
    }

    /* Step 4: Forward outside non-destructive preflight into BIOS log & debug rings */
    if (!s_io_fb.in_preflight) {
        (void)log_fallback_record(
            log_sev,
            LOG_CAT_IO,
            entry->subsystem,
            -0.35f,
            __FILE__,
            __LINE__,
            __func__,
            "[0x%04X] %s: %s",
            (unsigned int)error_code,
            entry->operation,
            entry->message
        );

        if (notify_debugger) {
            const char *tripwire_ref = "TW_ENG_IO_BUFFER_BOUNDS";
            if (error_code == IO_ERR_WRITE_BLOCKED_RO) {
                tripwire_ref = "TW_ENG_IO_READONLY_GUARD";
            } else if (error_code == IO_ERR_JSON_SYNTAX) {
                tripwire_ref = "TW_ENG_JSON_SYNTAX";
            } else if (error_code == IO_ERR_JSON_DEPTH_EXCEEDED) {
                tripwire_ref = "TW_ENG_JSON_DEPTH_EXCEEDED";
            } else if (error_code == IO_ERR_ROM_MISSING) {
                tripwire_ref = "TW_ENG_IO_ROM_INTEGRITY";
            }
            (void)debug_fallback_record_fault(
                entry->subsystem,
                entry->message,
                __FILE__,
                __LINE__,
                tripwire_ref
            );
        }
    }

    s_io_fb.in_fault_record = false;
}

/**
 * @brief Downgrade the BIOS I/O subsystem into Safe Mode (`FLOPPY_RO` or `ROM_ONLY`).
 *
 *   ### For Game Designers & Non-Programmers:
 *   Safely protects storage whenever external folders fail or policies break: locks out
 *   unsafe writes if requested, continues serving factory ROM data, and logs the event.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Arms BIOS, increments `s_io_fb.safe_downgrades`, switches `active_sink` to
 *   `IO_FB_SINK_FLOPPY_RO` or `IO_FB_SINK_ROM_ONLY`, and forwards telemetry into both
 *   `io_fallback_record_fault()` and `debug_fallback_record_fault()`.
 *
 * @param[in] reason         Concise Safe-Mode transition reason, or NULL for generic default.
 * @param[in] lock_read_only `true` to engage read-only floppy latch; `false` for `ROM_ONLY`.
 * @return `true` after Safe-Mode transition and telemetry recording complete.
 * @triggers Increments `safe_downgrades`, updates `active_sink`, and notifies debug BIOS.
 */
bool io_fallback_downgrade_safe_mode(const char *reason, bool lock_read_only)
{
    /* Step 1: Arm BIOS, count Safe-Mode downgrade, and transition active sink */
    io_fb_auto_ensure_booted();
    s_io_fb.safe_downgrades++;
    if (lock_read_only) {
        s_io_fb.config.read_only_mode = true;
        s_io_fb.active_sink = IO_FB_SINK_FLOPPY_RO;
    } else if (s_io_fb.active_sink == IO_FB_SINK_OFFLINE) {
        s_io_fb.active_sink = IO_FB_SINK_ROM_ONLY;
    }

    /* Step 2: Emit dual fault telemetry across I/O BIOS ring and debug BIOS ring */
    const char *msg = (reason != NULL) ? reason : "Engaged Safe-Mode I/O BIOS";
    io_fallback_record_fault("io_fallback", "downgrade_safe_mode", msg, IO_ERR_FILE_NOT_FOUND);
    (void)debug_fallback_record_fault("io_fallback", msg, __FILE__, __LINE__,
                                      "TW_ENG_IO_ROM_INTEGRITY");
    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / CONTRACT VALIDATION & PROBES
Stratum: Layer 0 (Core Utilities) | Subsystem: Tier-0 BIOS I/O & ROM Vault
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Policy & ROM Verification Cogs + Probe Wiring
Mission: Enforce policy ladder bounds, verify all .rodata 5-block ROMs, and
         wire `io.fallback.*` validation probes into the central debugger.

  ### For Game Designers & Non-Programmers:
  Stops broken buffer settings or corrupted built-in ROM assets before they cause
  silent data corruption during gameplay.

  ### For Systems Programmers & Engine Architects:
  Idempotently registers `io.fallback.rom_integrity` and `io.fallback.policy_bounds`
  into `debug.c` (`DEBUG_STEP_AWAIT` tripwire escalation when active).
  @triggers `io.fallback.rom_integrity`, `io.fallback.policy_bounds`.
--------------------------------------------------------------------------------
*/

bool io_fallback_validate_policy_contract(const io_fallback_config_t *cfg)
{
    if (!io_fb_policy_bounds_valid(cfg)) {
        return false;
    }
    if (cfg->profile_name[0] == '\0' || cfg->default_root[0] == '\0') {
        return false;
    }
    return true;
}

bool io_fallback_verify_rom_envelopes(void)
{
    size_t count = io_fallback_rom_count();
    if (count == 0U) {
        return false;
    }

    for (size_t i = 0U; i < count; i++) {
        size_t sz = 0U;
        const char *payload = io_fallback_rom_asset_at(i, &sz);
        io_fb_loader_t loader;
        memset(&loader, 0, sizeof(loader));
        if (!io_fallback_loader_from_memory(&loader, payload, sz)) {
            return false;
        }
        if (!loader.header.is_5block || loader.header.schema_id[0] == '\0') {
            return false;
        }
    }
    return true;
}

static diag_result_t io_fb_probe_rom_integrity(void)
{
    diag_result_t res;
    res.subsystem = IO_FALLBACK_MODULE_ID;
    res.check_name = "IO BIOS ROM Envelope Integrity";
    res.passed = io_fallback_verify_rom_envelopes();
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "All embedded io_config_rom.h 5-block envelopes verified."
        : "Corrupted .rodata IO ROM asset detected; engaging BIOS Safe Mode.";
    return res;
}

static diag_result_t io_fb_probe_policy_bounds(void)
{
    io_fb_auto_ensure_booted();
    diag_result_t res;
    res.subsystem = IO_FALLBACK_MODULE_ID;
    res.check_name = "IO BIOS Policy Bounds & Precedence";
    res.passed = io_fallback_validate_policy_contract(&s_io_fb.config);
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_FATAL;
    res.details = res.passed
        ? "Active BIOS I/O policy buffer ladder and limits verified."
        : "Active BIOS I/O policy corrupted; restoring ROM defaults before AWAIT.";
    return res;
}

void io_fallback_register_debug_probes(void)
{
    for (size_t i = 0U; i < 2U; i++) {
        (void)debug_register_validation_probe(&s_io_fb_probes[i]);
    }
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Run deterministic non-destructive self-checks over BIOS ROM, VFS
         hydration (>=36 domains), MasterLoader contracts, and Floppy RO lock.

  ### For Game Designers & Non-Programmers:
  Runs automatically under `./build/tenebris-lux --audit` to prove emergency I/O
  safeties still work properly before release or playtesting.

  ### For Systems Programmers & Engine Architects:
  Snapshots `s_io_fb` across audit checks and restores caller state before returning
  `diag_result_t` (`SEVERITY_PASS` / `CRITICAL` / `FATAL`).
  @triggers Exercises `io_fallback_boot()`, `io_fallback_verify_rom_envelopes()`,
            `io_fallback_loader_load("rom://io/fs_buffers.json")`, and Floppy RO lock.
--------------------------------------------------------------------------------
*/

diag_result_t io_fallback_preflight(void)
{
    diag_result_t res;
    res.subsystem = io_fallback_manifest.module_id;
    res.check_name = "Tier-0 BIOS I/O ROM & 4-Pillar Safe-Mode Check";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "I/O BIOS: ROM vault, 4-pillar fallbacks, floppy lock, and policy bounds verified.";

    /* Save caller singleton cleanly so preflight never alters live state */
    io_fallback_state_t backup = s_io_fb;
    s_io_fb.in_preflight = true;

    /* Check 1: Default & floppy template provenance contracts */
    io_fallback_config_t def_cfg = io_fallback_get_default_config();
    io_fallback_config_t flp_cfg = io_fallback_get_floppy_config();
    if (def_cfg.loaded_from_data || flp_cfg.loaded_from_data || !flp_cfg.read_only_mode ||
        !io_fallback_validate_policy_contract(&def_cfg) ||
        !io_fallback_validate_policy_contract(&flp_cfg)) {
        s_io_fb = backup;
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "BIOS fallback template violated provenance or bounds contract.";
        return res;
    }

    /* Check 2: Embedded .rodata ROM vault integrity across all entries */
    if (io_fallback_rom_count() < 7U || !io_fallback_verify_rom_envelopes()) {
        s_io_fb = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "io_config_rom.h asset count or 5-block envelope verification failed.";
        return res;
    }

    /* Check 3: Zero-disk virtual rom:// resolution, hydrated VFS routes & loader contract */
    if (!io_fallback_boot(NULL) ||
        s_io_fb.vfs_routes.domain_search_count < 36U ||
        s_io_fb.loader_contract.expected_major_minor_version[0] == '\0') {
        s_io_fb = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Tier-0 BIOS boot failed hydrating all VFS routes or loader contract from ROM.";
        return res;
    }

    io_fallback_config_t rom_cfg;
    if (!io_fallback_load_rom_config("default", &rom_cfg) || rom_cfg.loaded_from_data) {
        s_io_fb = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "io_fallback_load_rom_config('default') failed zero-disk hydration.";
        return res;
    }

    io_fb_loader_t loader;
    char rom_buf[IO_FB_MEDIUM_BYTES];
    if (!io_fallback_loader_load(&loader, "config", "rom://io/fs_buffers.json",
                                 rom_buf, sizeof(rom_buf)) ||
        !io_fallback_loader_validate_schema(&loader, "tenlux.data.io_fs_profile.v1")) {
        s_io_fb = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Zero-disk rom:// MasterLoader fallback failed schema validation.";
        return res;
    }

    /* Check 4: Floppy read-only write latch */
    io_fallback_boot_floppy();
    if (!io_fallback_is_disk_read_only() ||
        io_fallback_fs_write_bounded("build/should_block_ro.tmp", "x", 1U) != false) {
        s_io_fb = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Floppy Safe-Mode read-only write latch failed to block disk write.";
        return res;
    }

    s_io_fb = backup;
    return res;
}

bool io_fallback_audit(void)
{
    diag_result_t res = io_fallback_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES, CARDS & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Tier-0 BIOS I/O & ROM Vault
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Expose Safe-Mode fault/counter telemetry and ASCII status cards.

  ### For Game Designers & Non-Programmers:
  Formats clean diagnostic summary cards showing which I/O profile is running,
  folder mode permissions, and whether any saves were blocked by Safe Mode.

  ### For Systems Programmers & Engine Architects:
  Reads `.bss` atomic counters (`rom_hits`, `read_only_blocks`, `safe_downgrades`)
  with zero heap allocations and guarantees `loaded_from_data == false`.
--------------------------------------------------------------------------------
*/

bool io_fallback_get_stats(io_fallback_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }
    memset(out_stats, 0, sizeof(*out_stats));
    io_fb_auto_ensure_booted();

    out_stats->booted = s_io_fb.booted;
    out_stats->rom_hydrated = s_io_fb.rom_hydrated;
    out_stats->disk_read_only = s_io_fb.config.read_only_mode;
    out_stats->active_sink = s_io_fb.active_sink;
    out_stats->rom_asset_count = (size_t)IO_CONFIG_ROM_ASSET_COUNT;
    out_stats->rom_bytes_total = (size_t)IO_CONFIG_ROM_TOTAL_BYTES;
    out_stats->rom_hits = s_io_fb.rom_hits;
    out_stats->fault_count = s_io_fb.fault_sequence;
    out_stats->read_only_blocks = s_io_fb.read_only_blocks;
    out_stats->safe_downgrades = s_io_fb.safe_downgrades;
    str_safe_copy(out_stats->active_root, s_io_fb.active_root, sizeof(out_stats->active_root));
    str_safe_copy(out_stats->active_profile, s_io_fb.config.profile_name,
                  sizeof(out_stats->active_profile));
    out_stats->loaded_from_data = false;
    return true;
}

bool io_fallback_execute_inspect(io_fallback_config_t *out_cfg)
{
    if (out_cfg == NULL) {
        return false;
    }
    if (s_io_fb.booted) {
        *out_cfg = s_io_fb.config;
    } else {
        *out_cfg = io_fallback_get_default_config();
    }
    out_cfg->loaded_from_data = false;
    return true;
}

bool io_fallback_execute_dump_card(const io_fallback_config_t *cfg,
                                   char *out_buf,
                                   size_t buf_size)
{
    if (!io_fb_buf_is_valid(out_buf, buf_size)) {
        return false;
    }

    io_fallback_config_t active = (cfg != NULL) ? *cfg : s_io_fallback_template;
    const char *sink_label = "DISK_ACTIVE";
    if (active.read_only_mode) {
        sink_label = "FLOPPY_READ_ONLY";
    } else if (s_io_fb.booted && s_io_fb.active_sink == IO_FB_SINK_ROM_ONLY) {
        sink_label = "ROM_ONLY";
    }

    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "           TIER-0 BIOS I/O & ROM VAULT STATUS CARD          \n"
        "============================================================\n"
        "Profile:         %s\n"
        "Sink Mode:       %s\n"
        "Loaded From:     %s (BIOS SAFE-MODE)\n"
        "ROM Assets:      %zu files (%zu bytes .rodata)\n"
        "Buffer Ladder:   %zu / %zu / %zu / %zu B\n"
        "Max Path / JSON: %zu B / depth %d\n"
        "DirMode /Atom/RO:%04o / %s / %s\n"
        "VFS Dirs/Loader: %zu domains / ver %s\n"
        "Faults / RO Blk: %llu / %llu\n"
        "============================================================\n",
        active.profile_name[0] ? active.profile_name : IO_FALLBACK_PROFILE_NAME,
        sink_label,
        active.loaded_from_data ? "DISK (Unexpected)" : "FALSE (COMPILED ROM)",
        (size_t)IO_CONFIG_ROM_ASSET_COUNT,
        (size_t)IO_CONFIG_ROM_TOTAL_BYTES,
        active.small_bytes,
        active.medium_bytes,
        active.large_bytes,
        active.max_config_bytes,
        active.path_max_bytes,
        active.max_json_depth,
        active.posix_dir_mode & 0777U,
        active.atomic_write ? "YES" : "NO",
        active.read_only_mode ? "YES" : "NO",
        s_io_fb.vfs_routes.domain_search_count,
        s_io_fb.loader_contract.expected_major_minor_version[0]
            ? s_io_fb.loader_contract.expected_major_minor_version
            : "0.1",
        (unsigned long long)s_io_fb.fault_sequence,
        (unsigned long long)s_io_fb.read_only_blocks
    );

    if (written < 0 || (size_t)written >= buf_size) {
        out_buf[buf_size - 1U] = '\0';
        return false;
    }
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization, Floppy Mode & Teardown
Mission: Arm Tier-0 Safe-Mode BIOS (nominal or Floppy RO), hydrate ROM subpolicies,
         register debugger probes, and provide clean test reset/teardown hooks.

  ### For Game Designers & Non-Programmers:
  Turns on emergency I/O services safely when booting on disk, CD/DVD, or locked floppy.

  ### For Systems Programmers & Engine Architects:
  Hydrates `s_io_fb.config`, `vfs_routes`, and `loader_contract` from compiled
  `.rodata` ROMs first (`loaded_from_data = false`), configures `IO_FB_SINK_*`, and
  calls `io_fallback_register_debug_probes()`.
--------------------------------------------------------------------------------
*/

bool io_fallback_boot(const io_fallback_config_t *opt_cfg)
{
    io_fallback_config_t selected = s_io_fallback_template;
    bool hydrated = false;

    if (opt_cfg != NULL && io_fallback_validate_policy_contract(opt_cfg)) {
        selected = *opt_cfg;
        selected.loaded_from_data = false;
    } else if (io_fallback_load_rom_config("default", &selected)) {
        hydrated = true;
    }

    s_io_fb.config = selected;
    s_io_fb.rom_hydrated = hydrated;
    io_fb_hydrate_subpolicies();
    str_safe_copy(s_io_fb.active_root,
                  selected.default_root[0] ? selected.default_root : IO_FB_DEFAULT_ROOT,
                  sizeof(s_io_fb.active_root));

    if (selected.read_only_mode) {
        s_io_fb.active_sink = IO_FB_SINK_FLOPPY_RO;
    } else {
        s_io_fb.active_sink = IO_FB_SINK_DISK_ACTIVE;
    }

    s_io_fb.booted = true;
    io_fallback_register_debug_probes();
    return true;
}

bool io_fallback_boot_floppy(void)
{
    io_fallback_config_t floppy = s_io_floppy_template;
    if (io_fallback_load_rom_config("floppy_safe", &floppy) ||
        io_fallback_load_rom_config("default", &floppy)) {
        s_io_fb.rom_hydrated = true;
    } else {
        s_io_fb.rom_hydrated = false;
    }
    floppy.read_only_mode = true;
    floppy.loaded_from_data = false;
    if (floppy.profile_name[0] == '\0' || strcmp(floppy.profile_name, "default") == 0) {
        str_safe_copy(floppy.profile_name, IO_FALLBACK_FLOPPY_PROFILE_NAME,
                      sizeof(floppy.profile_name));
    }

    s_io_fb.config = floppy;
    io_fb_hydrate_subpolicies();
    s_io_fb.active_sink = IO_FB_SINK_FLOPPY_RO;
    str_safe_copy(s_io_fb.active_root, IO_FB_DEFAULT_ROOT, sizeof(s_io_fb.active_root));
    s_io_fb.booted = true;
    io_fallback_register_debug_probes();
    return true;
}

void io_fallback_teardown(void)
{
    memset(&s_io_fb, 0, sizeof(s_io_fb));
    s_io_fb.active_sink = IO_FB_SINK_OFFLINE;
}

bool io_fallback_is_booted(void)
{
    return s_io_fb.booted;
}

void io_fallback_reset_for_testing(void)
{
    io_fallback_teardown();
}

/*
================================================================================
End of io_fallback.c
================================================================================
*/

