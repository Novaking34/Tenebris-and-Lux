/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/src/io/json_utils.c
@id            cog.json_utils
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap JSON Tokenizer & Structured Field Deserializer
@role          Runtime Engine & Chassis: Single-source span primitives, escape-aware
               string decoder, numeric/bool coercion, and io.json.* probes.
@maturity      Rev 5 Baseline | Span-isolated parsing, lazy boot safety & contract policy hydration verified.
@chassis       zero-heap | .bss 144 B (s_json_state) + .rodata 5,740 B + zero function-local static buffers.
@scale         O(N) single-pass scan over JSON slice; bracket depth capped at <= 32 on caller stack.
@seams         in: loader_contracts.json, rom://io | uses: cog.io.fallback, cog.log, cog.debug | probes: io.json.* -> DEBUG_STEP_AWAIT | dag: core.util.json
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Container scanners enforce strict [start, end) isolation so sibling blocks never bleed.
               3. Lazy Safe-Mode policy seeding lets log_init() parse JSON before io/ initialization without loops.
               4. Escape decoder writes bounded UTF-8/ASCII cleanly with unconditional NUL termination.

PURPOSE & ARCHITECTURAL ROLE:
  Executes single-pass zero-heap JSON tokenization and extraction in Layer 0,
  isolating object/array spans in Sub-Assembly 2.4 helpers while hydrating active
  depth/token policies from loader_contracts.json or Safe-Mode ROM defaults.

  ### For Game Designers & Non-Programmers:
  Ensures typos, extra spaces, or deeply corrupted JSON brackets fail cleanly
  with clear diagnostic hints rather than misreading neighboring blocks or locking
  up startup before the main logger starts.

  ### For Systems Programmers & Engine Architects:
  - Controlled Memory Chassis: .bss occupies just 144 B (s_json_state) and
    .rodata 5,740 B, keeping parser telemetry featherlight inside Layer 0.
  - Bottleneck & Headroom: Single-pass span helpers avoid quadratic rescans over
    large catalogs; ample budget for optional key-hash indexing if tables grow.
  - Probe & Config Wiring: Hydrates limits lazily on early boot, then wires
    `io.json.*` validation probes cleanly into `debug_trigger_probe()` on init.
================================================================================
*/

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/*
================================================================================
BLOCK 2: OPENING BLOCK
( 3-Tier Includes, Private Ontology, Unified Memory Chassis & Inline Frontier )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.json_utils
Mission:       Bind deps, log/debug setup, private structs, unified
               .bss/.rodata chassis, and support helpers.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Library Dependencies (None in Layer 0/1)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "io/json_utils.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "io/io_fallback.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Compile-Time Constants
--------------------------------------------------------------------------------
*/
/** @brief Canonical manifest identifier registering the zero-heap JSON engine. */
#define MODULE_ID          JSON_UTILS_MODULE_ID
/** @brief Telemetry category channel for JSON parsing diagnostics. */
#define LOCAL_LOG_CAT      LOG_CAT_IO
/** @brief Concise logger source tag stamped into LOG_CAT_IO messages. */
#define LOCAL_MODULE_TAG   "JSON_Utils"

static_assert(
    JSON_MAX_NESTING <= IO_FB_MAX_JSON_DEPTH,
    "JSON_MAX_NESTING must match or fit inside IO_FB_MAX_JSON_DEPTH"
);
static_assert(
    JSON_DEFAULT_MAX_TOKENS == IO_FB_MAX_JSON_TOKENS,
    "JSON_DEFAULT_MAX_TOKENS must stay synchronized with io_fallback.h"
);
static_assert(
    JSON_DEFAULT_MAX_PAYLOAD == IO_FB_MAX_CONFIG_BYTES,
    "JSON_DEFAULT_MAX_PAYLOAD must stay synchronized with io_fallback.h"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Probe Forward Declarations
--------------------------------------------------------------------------------
*/

/**
 * @struct json_utils_runtime_state
 * @brief Fixed `.bss` telemetry, policy, and reentrancy guard state for `json_utils`.
 *
 * ### For Game Designers & Non-Programmers:
 * Tracks live parse totals, broken syntax alerts, and active parser safety rules.
 *
 * ### For Systems Programmers & Engine Architects:
 * Controlled Memory Chassis singleton (< 256 B `.bss`). `in_log_emit` and
 * `in_preflight` prevent circular logging recursion and `--audit` log noise.
 */
typedef struct json_utils_runtime_state {
    bool          initialized;        /**< True after `json_utils_init()` completes. */
    bool          policy_armed;       /**< True once baseline ROM/Safe policy seeded. */
    bool          policy_from_data;   /**< True ONLY when hydrated from external disk JSON. */
    bool          in_log_emit;        /**< Reentrancy guard breaking circular `LOG_CAT_IO` loops. */
    bool          in_preflight;       /**< True during Block 4 synthetic audit self-checks. */
    json_policy_t policy;             /**< Active depth/token/payload/escape parser policy. */
    uint64_t      key_queries;        /**< Monotonic count of key lookup scans invoked. */
    uint64_t      extractions_ok;     /**< Successful scalar, slice, or array extractions. */
    uint64_t      depth_overflows;    /**< Aborted deep containers exceeding `max_json_depth`. */
    uint64_t      syntax_faults;      /**< Unterminated strings/containers or bad tokens. */
    uint64_t      type_mismatches;    /**< Located values rejecting requested primitive type. */
    uint64_t      truncated_strings;  /**< Valid strings clipped cleanly to caller capacity. */
    uint64_t      token_inspections;  /**< Total inspection/validation calls executed. */
} json_utils_runtime_state_t;

/* Forward declarations binding Sub-Assembly 2.3 manifest and Block 4 probes. */
diag_result_t json_utils_preflight(void);
static diag_result_t json_probe_syntax_health(void);
static diag_result_t json_probe_depth_health(void);
static diag_result_t json_probe_policy_health(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss & .rodata Allocations)
--------------------------------------------------------------------------------
*/

/** @brief Immutable architectural invariants exported in `json_utils_manifest` (.rodata). */
static const char *const s_json_invariants[5] = {
    "Zero heap allocations: Strict zero-heap execution across all JSON routines",
    "Buffer safety: Extracted strings and sliced objects unconditionally NUL-terminate",
    "Quote & backslash parity: String values never spoof outer keys or break delimiters",
    "Depth & syntax guard: Nested depth <= max_json_depth (32) with io.json.* probes",
    "Universal 5-Block Lifecycle Protocol enforced across header and source"
};

/**
 * Manifest record for the Zero-Heap JSON Engine.
 */
const module_metadata_t json_utils_manifest = {
    JSON_UTILS_MODULE_ID,
    "json_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-11",
    "Zero-heap JSON span locator, escape-safe extractor, structural validator & probes",
    1,
    { "core.util.string" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    100,
    5,
    {
        s_json_invariants[0],
        s_json_invariants[1],
        s_json_invariants[2],
        s_json_invariants[3],
        s_json_invariants[4]
    },
    json_utils_preflight
};

/** Consolidated `.bss` state singleton (< 256 B, zero heap allocation). */
static json_utils_runtime_state_t s_json_state;

/** Immutable `.rodata` syntax health probe test payloads (Block 4 Sub-Assembly 4.1). */
static const char *const s_json_probe_ok_sample = "{\"ok\":true,\"esc\":\"a\\\\b\"}";
static const char *const s_json_probe_bad_sample = "{\"bad\":\"unclosed";

/** Compiled `.rodata` validation probe descriptors bound into debugger table. */
static const debug_validation_probe_t s_json_probes[3] = {
    {
        "io.json.syntax",
        JSON_UTILS_MODULE_ID,
        "json_utils.c:Block5:json_inspect_token",
        "tests/unit/io/test_json_utils_unit.c:Section3",
        DEBUG_TEST_TIER_UNIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_FALLBACK_MODE,
        json_probe_syntax_health,
        true,
        true,
        0U
    },
    {
        "io.json.depth_bounds",
        JSON_UTILS_MODULE_ID,
        "json_utils.c:Block2:json_find_delimited_span_internal",
        "tests/unit/io/test_json_utils_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_FALLBACK_MODE,
        json_probe_depth_health,
        true,
        true,
        0U
    },
    {
        "io.json.policy_bounds",
        JSON_UTILS_MODULE_ID,
        "json_utils.c:Block3:json_utils_set_policy",
        "tests/unit/io/test_json_utils_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_PANIC_HALT,
        json_probe_policy_health,
        true,
        true,
        0U
    }
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Canonical Pre-Conditional Inline Predicates & Log Frontier
--------------------------------------------------------------------------------
O(1) character classifiers, span boundary guards, and reentrancy-safe logging.
--------------------------------------------------------------------------------
*/

/** @brief Reentrancy-guarded `LOG_CAT_IO` telemetry emitter skipping audit sweeps. */
#define JSON_EMIT_LOG(sev_macro, grade, ...)                                   \
    do {                                                                       \
        if (!s_json_state.in_log_emit && !s_json_state.in_preflight &&         \
            log_is_active()) {                                                 \
            s_json_state.in_log_emit = true;                                   \
            (void)sev_macro(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, (grade),          \
                            __VA_ARGS__);                                      \
            s_json_state.in_log_emit = false;                                  \
        }                                                                      \
    } while (0)

/** @brief Returns true in O(1) time for ASCII JSON whitespace octets. */
static inline bool json_is_whitespace_char(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

/** @brief Returns true in O(1) time for valid JSON numeric literal characters. */
static inline bool json_is_number_char(char c)
{
    return (isdigit((unsigned char)c) != 0) ||
           c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E';
}

/** @brief Returns true in O(1) time when cursor `p` reaches `end` or NUL terminator. */
static inline bool json_at_end(const char *p, const char *end)
{
    if (p == NULL) {
        return true;
    }
    return (end != NULL) ? (p >= end) : (*p == '\0');
}

/** @brief Returns true in O(1) time when cursor `p` sits on a valid value delimiter. */
static inline bool json_is_value_delimiter(const char *p, const char *end)
{
    if (json_at_end(p, end)) {
        return true;
    }
    char c = *p;
    return json_is_whitespace_char(c) || c == ',' || c == '}' || c == ']';
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.5: Monitored Support Infrastructure (Non-Inline Internal)
--------------------------------------------------------------------------------
Support Role:  Single-source JSON span walkers, escape-aware string decoders,
               numeric span parsers, array indexers, and depth validators.
Pattern Watch: Houses ~940 lines of general JSON span/token scanning primitives;
               if streaming or event-based JSON parsing is later required, split
               these internal walkers into a dedicated json_span_scanner.c unit.
Linkage Rule:  Strictly static functions only; zero static arrays or .bss storage.
--------------------------------------------------------------------------------
*/

static void json_seed_default_policy(json_policy_t *out_policy)
{
    if (out_policy == NULL) {
        return;
    }
    memset(out_policy, 0, sizeof(*out_policy));
    out_policy->max_json_depth = JSON_MAX_NESTING;
    out_policy->max_json_tokens = JSON_DEFAULT_MAX_TOKENS;
    out_policy->max_payload_bytes = JSON_DEFAULT_MAX_PAYLOAD;
    out_policy->max_key_len = JSON_MAX_KEY_LEN;
    out_policy->max_token_len = JSON_MAX_TOKEN_LEN;
    out_policy->strict_utf8_escapes = false;
    out_policy->reject_duplicate_keys = false;
    out_policy->probe_await_on_corrupt = true;
    str_safe_copy(out_policy->source_tag, "rom_safe_seed", sizeof(out_policy->source_tag));
    out_policy->loaded_from_data = false;
}

static void json_ensure_policy_armed(void)
{
    if (!s_json_state.policy_armed) {
        json_seed_default_policy(&s_json_state.policy);
        s_json_state.policy_from_data = false;
        s_json_state.policy_armed = true;
    }
}

static void json_sync_policy_from_fallback(json_policy_t *out_policy)
{
    if (out_policy == NULL) {
        return;
    }
    json_seed_default_policy(out_policy);

    io_fallback_config_t fb_cfg;
    if (io_fallback_get_active_config(&fb_cfg)) {
        if (fb_cfg.max_json_depth > 0 && fb_cfg.max_json_depth <= JSON_MAX_NESTING) {
            out_policy->max_json_depth = fb_cfg.max_json_depth;
        }
        if (fb_cfg.max_json_tokens > 0U && fb_cfg.max_json_tokens <= JSON_DEFAULT_MAX_TOKENS) {
            out_policy->max_json_tokens = fb_cfg.max_json_tokens;
        }
        if (fb_cfg.max_config_bytes >= 64U &&
            fb_cfg.max_config_bytes <= JSON_DEFAULT_MAX_PAYLOAD) {
            out_policy->max_payload_bytes = fb_cfg.max_config_bytes;
        }
        out_policy->strict_utf8_escapes = fb_cfg.strict_utf8_escapes;
        out_policy->reject_duplicate_keys = fb_cfg.reject_duplicate_keys;
        out_policy->probe_await_on_corrupt = fb_cfg.probe_await_on_corrupt;
    }

    io_fb_loader_contract_t contract;
    if (io_fallback_get_loader_contract(&contract)) {
        if (contract.max_json_depth > 0 && contract.max_json_depth <= JSON_MAX_NESTING) {
            out_policy->max_json_depth = contract.max_json_depth;
        }
        if (contract.max_json_tokens > 0U &&
            contract.max_json_tokens <= JSON_DEFAULT_MAX_TOKENS) {
            out_policy->max_json_tokens = contract.max_json_tokens;
        }
        if (contract.max_payload_bytes >= 64U &&
            contract.max_payload_bytes <= JSON_DEFAULT_MAX_PAYLOAD) {
            out_policy->max_payload_bytes = contract.max_payload_bytes;
        }
        out_policy->strict_utf8_escapes = contract.strict_utf8_escapes;
        out_policy->reject_duplicate_keys = contract.reject_duplicate_keys;
        str_safe_copy(out_policy->source_tag, "rom_loader_contracts",
                      sizeof(out_policy->source_tag));
    }
    out_policy->loaded_from_data = false;
}

/** Downgrades runtime JSON policy safely to `.rodata` ROM defaults and records fault telemetry. */
static void json_downgrade_policy_to_rom(const char *op,
                                         const char *fault_msg,
                                         const char *log_detail,
                                         const char *probe_msg)
{
    json_policy_t rom_policy;
    json_sync_policy_from_fallback(&rom_policy);
    s_json_state.policy = rom_policy;
    s_json_state.policy_from_data = false;
    s_json_state.policy_armed = true;
    s_json_state.syntax_faults++;

    if (!s_json_state.in_preflight) {
        io_fallback_record_fault("json_utils",
                                 op ? op : "policy",
                                 fault_msg,
                                 IO_ERR_JSON_SYNTAX);
        JSON_EMIT_LOG(LOG_ERROR, -0.75f, "%s", log_detail);
        (void)json_utils_trigger_probe(probe_msg, IO_ERR_JSON_SYNTAX);
    }
}

static const char *json_skip_ws_span(const char *p, const char *end)
{
    while (!json_at_end(p, end) && json_is_whitespace_char(*p)) {
        p++;
    }
    return p;
}

static const char *json_skip_string_span(const char *open_quote,
                                         const char *end,
                                         bool strict_escapes,
                                         bool *out_escape_error)
{
    if (out_escape_error != NULL) {
        *out_escape_error = false;
    }
    if (json_at_end(open_quote, end) || *open_quote != '"') {
        return NULL;
    }

    const char *p = open_quote + 1;
    while (!json_at_end(p, end)) {
        char c = *p;
        if (c == '\\') {
            p++;
            if (json_at_end(p, end)) {
                return NULL;
            }
            char esc = *p;
            if (strict_escapes) {
                bool ok_esc = (esc == '"' || esc == '\\' || esc == '/' ||
                               esc == 'b' || esc == 'f' || esc == 'n' ||
                               esc == 'r' || esc == 't' || esc == 'u');
                if (!ok_esc) {
                    if (out_escape_error != NULL) {
                        *out_escape_error = true;
                    }
                    return NULL;
                }
                if (esc == 'u') {
                    for (int h = 1; h <= 4; h++) {
                        if (json_at_end(p + h, end) ||
                            !isxdigit((unsigned char)p[h])) {
                            if (out_escape_error != NULL) {
                                *out_escape_error = true;
                            }
                            return NULL;
                        }
                    }
                    p += 4;
                }
            }
            p++;
            continue;
        }
        if (c == '"') {
            return p;
        }
        if (strict_escapes && (unsigned char)c < 0x20U) {
            if (out_escape_error != NULL) {
                *out_escape_error = true;
            }
            return NULL;
        }
        p++;
    }

    return NULL;
}

static void json_record_depth_overflow(const char *op, size_t offset)
{
    s_json_state.depth_overflows++;
    if (!s_json_state.in_preflight) {
        char msg[88];
        snprintf(msg, sizeof(msg),
                 "JSON nesting exceeded max_json_depth (%d) near offset %zu",
                 s_json_state.policy.max_json_depth, offset);
        io_fallback_record_fault("json_utils", op, msg, IO_ERR_JSON_DEPTH_EXCEEDED);
        JSON_EMIT_LOG(LOG_ERROR, -0.75f, "%s", msg);
        (void)json_utils_trigger_probe(msg, IO_ERR_JSON_DEPTH_EXCEEDED);
    }
}

static const char *json_find_key_span_internal(const char *json,
                                               size_t max_len,
                                               const char *key)
{
    json_ensure_policy_armed();
    s_json_state.key_queries++;

    if (json == NULL || key == NULL || key[0] == '\0') {
        return NULL;
    }

    size_t key_len = strlen(key);
    if (key_len >= s_json_state.policy.max_key_len) {
        return NULL;
    }

    const char *end = (max_len > 0U) ? (json + max_len) : NULL;
    const char *first_non_ws = json_skip_ws_span(json, end);
    if (json_at_end(first_non_ws, end)) {
        return NULL;
    }

    bool started_with_object = (*first_non_ws == '{');
    int max_depth = s_json_state.policy.max_json_depth;
    int depth = 0;
    const char *p = first_non_ws;

    while (!json_at_end(p, end)) {
        char c = *p;
        if (c == '"') {
            bool esc_err = false;
            const char *close_q = json_skip_string_span(
                p, end, s_json_state.policy.strict_utf8_escapes, &esc_err);
            if (close_q == NULL) {
                s_json_state.syntax_faults++;
                JSON_EMIT_LOG(LOG_WARN, -0.30f,
                              "Unterminated or malformed string while searching key '%s'", key);
                return NULL;
            }
            size_t tok_len = (size_t)(close_q - (p + 1));
            if (tok_len == key_len && strncmp(p + 1, key, key_len) == 0) {
                const char *after = json_skip_ws_span(close_q + 1, end);
                if (!json_at_end(after, end) && *after == ':') {
                    after = json_skip_ws_span(after + 1, end);
                    if (!json_at_end(after, end)) {
                        return after;
                    }
                }
            }
            p = close_q + 1;
            continue;
        }
        if (c == '{' || c == '[') {
            depth++;
            if (depth > max_depth) {
                json_record_depth_overflow("json_find_key_span", (size_t)(p - json));
                return NULL;
            }
        } else if (c == '}' || c == ']') {
            depth--;
            if ((started_with_object && depth <= 0) ||
                (!started_with_object && depth < 0)) {
                return NULL;
            }
        }
        p++;
    }

    return NULL;
}

static bool json_find_delimited_span_internal(const char *open_pos,
                                              const char *end,
                                              char open_ch,
                                              char close_ch,
                                              size_t *out_len,
                                              int *out_max_depth,
                                              bool *out_depth_exceeded,
                                              bool *out_syntax_err)
{
    if (out_len != NULL) {
        *out_len = 0U;
    }
    if (out_max_depth != NULL) {
        *out_max_depth = 0;
    }
    if (out_depth_exceeded != NULL) {
        *out_depth_exceeded = false;
    }
    if (out_syntax_err != NULL) {
        *out_syntax_err = false;
    }
    if (json_at_end(open_pos, end) || *open_pos != open_ch) {
        if (out_syntax_err != NULL) {
            *out_syntax_err = true;
        }
        return false;
    }

    json_ensure_policy_armed();
    int max_depth = s_json_state.policy.max_json_depth;
    char nest_stack[JSON_MAX_NESTING];
    int stack_top = 0;
    int max_seen = 0;
    const char *p = open_pos;

    while (!json_at_end(p, end)) {
        char c = *p;
        if (c == '"') {
            bool esc_err = false;
            const char *close_q = json_skip_string_span(
                p, end, s_json_state.policy.strict_utf8_escapes, &esc_err);
            if (close_q == NULL) {
                if (out_syntax_err != NULL) {
                    *out_syntax_err = true;
                }
                return false;
            }
            p = close_q + 1;
            continue;
        }
        if (c == '{' || c == '[') {
            if (stack_top >= max_depth) {
                if (out_depth_exceeded != NULL) {
                    *out_depth_exceeded = true;
                }
                return false;
            }
            nest_stack[stack_top++] = c;
            if (stack_top > max_seen) {
                max_seen = stack_top;
            }
        } else if (c == '}' || c == ']') {
            if (stack_top <= 0) {
                if (out_syntax_err != NULL) {
                    *out_syntax_err = true;
                }
                return false;
            }
            char expected = (nest_stack[stack_top - 1] == '{') ? '}' : ']';
            if (c != expected) {
                if (out_syntax_err != NULL) {
                    *out_syntax_err = true;
                }
                return false;
            }
            stack_top--;
            if (stack_top == 0) {
                if (c != close_ch) {
                    if (out_syntax_err != NULL) {
                        *out_syntax_err = true;
                    }
                    return false;
                }
                if (out_len != NULL) {
                    *out_len = (size_t)(p - open_pos + 1);
                }
                if (out_max_depth != NULL) {
                    *out_max_depth = max_seen;
                }
                return true;
            }
        }
        p++;
    }

    if (out_syntax_err != NULL) {
        *out_syntax_err = true;
    }
    return false;
}

static bool json_decode_string_span(const char *open_quote,
                                    const char *end,
                                    char *out_buf,
                                    size_t out_size,
                                    bool *out_truncated)
{
    if (out_truncated != NULL) {
        *out_truncated = false;
    }
    if (out_buf == NULL || out_size == 0U) {
        return false;
    }
    out_buf[0] = '\0';

    json_ensure_policy_armed();
    bool esc_err = false;
    const char *close_q = json_skip_string_span(
        open_quote, end, s_json_state.policy.strict_utf8_escapes, &esc_err);
    if (close_q == NULL) {
        s_json_state.syntax_faults++;
        return false;
    }

    size_t write_idx = 0U;
    size_t max_copy = out_size - 1U;
    bool truncated = false;
    const char *p = open_quote + 1;

    while (p < close_q) {
        char ch = *p;
        if (ch == '\\' && (p + 1) < close_q) {
            p++;
            char esc = *p;
            if (esc == 'n') {
                ch = '\n';
            } else if (esc == 'r') {
                ch = '\r';
            } else if (esc == 't') {
                ch = '\t';
            } else if (esc == 'b') {
                ch = '\b';
            } else if (esc == 'f') {
                ch = '\f';
            } else {
                ch = esc;
            }
        }
        if (write_idx < max_copy) {
            out_buf[write_idx++] = ch;
        } else {
            truncated = true;
            break;
        }
        p++;
    }

    out_buf[write_idx] = '\0';
    if (truncated) {
        s_json_state.truncated_strings++;
        if (out_truncated != NULL) {
            *out_truncated = true;
        }
        return false;
    }
    s_json_state.extractions_ok++;
    return true;
}

static bool json_extract_number_token(const char *val,
                                      const char *end,
                                      char *num_buf,
                                      size_t buf_size,
                                      size_t *out_tok_len)
{
    if (out_tok_len != NULL) {
        *out_tok_len = 0U;
    }
    if (json_at_end(val, end) || num_buf == NULL || buf_size < 4U) {
        return false;
    }
    if (*val != '-' && *val != '+' && isdigit((unsigned char)*val) == 0) {
        return false;
    }

    size_t len = 0U;
    const char *p = val;
    while (!json_at_end(p, end) && !json_is_value_delimiter(p, end)) {
        if (len + 1U >= buf_size) {
            return false;
        }
        num_buf[len++] = *p;
        p++;
    }
    if (len == 0U) {
        return false;
    }
    num_buf[len] = '\0';
    if (out_tok_len != NULL) {
        *out_tok_len = len;
    }
    return true;
}

static bool json_parse_int_span(const char *val, const char *end, int *out_val)
{
    if (out_val == NULL) {
        return false;
    }
    char num_buf[64];
    if (!json_extract_number_token(val, end, num_buf, sizeof(num_buf), NULL)) {
        s_json_state.type_mismatches++;
        return false;
    }

    char *endptr = NULL;
    errno = 0;
    long parsed = strtol(num_buf, &endptr, 10);
    if (endptr == num_buf || *endptr != '\0' || errno == ERANGE ||
        parsed < INT_MIN || parsed > INT_MAX) {
        s_json_state.type_mismatches++;
        return false;
    }

    *out_val = (int)parsed;
    s_json_state.extractions_ok++;
    return true;
}

static bool json_parse_size_t_span(const char *val, const char *end, size_t *out_val)
{
    if (out_val == NULL || json_at_end(val, end) || *val == '-') {
        if (out_val != NULL) {
            s_json_state.type_mismatches++;
        }
        return false;
    }
    char num_buf[64];
    if (!json_extract_number_token(val, end, num_buf, sizeof(num_buf), NULL) ||
        num_buf[0] == '-' || num_buf[0] == '+') {
        s_json_state.type_mismatches++;
        return false;
    }

    char *endptr = NULL;
    errno = 0;
    unsigned long long parsed = strtoull(num_buf, &endptr, 10);
    if (endptr == num_buf || *endptr != '\0' || errno == ERANGE ||
        parsed > (unsigned long long)SIZE_MAX) {
        s_json_state.type_mismatches++;
        return false;
    }

    *out_val = (size_t)parsed;
    s_json_state.extractions_ok++;
    return true;
}

static bool json_parse_float_span(const char *val, const char *end, float *out_val)
{
    if (out_val == NULL) {
        return false;
    }
    char num_buf[64];
    if (!json_extract_number_token(val, end, num_buf, sizeof(num_buf), NULL)) {
        s_json_state.type_mismatches++;
        return false;
    }

    char *endptr = NULL;
    errno = 0;
    float parsed = strtof(num_buf, &endptr);
    if (endptr == num_buf || *endptr != '\0' || errno == ERANGE || !isfinite(parsed)) {
        s_json_state.type_mismatches++;
        return false;
    }

    *out_val = parsed;
    s_json_state.extractions_ok++;
    return true;
}

static bool json_parse_bool_span(const char *val, const char *end, bool *out_val)
{
    if (out_val == NULL || json_at_end(val, end)) {
        return false;
    }

    size_t avail = (end != NULL) ? (size_t)(end - val) : strlen(val);
    if (avail >= 4U && strncmp(val, "true", 4) == 0 &&
        (avail == 4U || json_is_value_delimiter(val + 4, end))) {
        *out_val = true;
        s_json_state.extractions_ok++;
        return true;
    }
    if (avail >= 5U && strncmp(val, "false", 5) == 0 &&
        (avail == 5U || json_is_value_delimiter(val + 5, end))) {
        *out_val = false;
        s_json_state.extractions_ok++;
        return true;
    }

    s_json_state.type_mismatches++;
    return false;
}

static const char *json_resolve_object_value_span(const char *json,
                                                  size_t max_len,
                                                  const char *parent_obj_key,
                                                  const char *key,
                                                  const char **out_obj_end)
{
    if (out_obj_end != NULL) {
        *out_obj_end = NULL;
    }
    if (json == NULL || parent_obj_key == NULL || key == NULL || key[0] == '\0') {
        return NULL;
    }

    const char *obj_start = NULL;
    size_t obj_len = 0U;
    if (!json_find_object_span(json, max_len, parent_obj_key, &obj_start, &obj_len) ||
        obj_start == NULL || obj_len < 2U) {
        return NULL;
    }

    if (out_obj_end != NULL) {
        *out_obj_end = obj_start + obj_len;
    }
    return json_find_key_span_internal(obj_start, obj_len, key);
}

static bool json_array_locate_element(const char *arr_start,
                                      size_t arr_len,
                                      size_t target_idx,
                                      size_t *out_count,
                                      const char **out_elem_start,
                                      const char **out_elem_end)
{
    if (out_count != NULL) {
        *out_count = 0U;
    }
    if (out_elem_start != NULL) {
        *out_elem_start = NULL;
    }
    if (out_elem_end != NULL) {
        *out_elem_end = NULL;
    }

    json_ensure_policy_armed();
    if (arr_start == NULL || arr_len < 2U ||
        arr_start[0] != '[' || arr_start[arr_len - 1U] != ']') {
        return false;
    }

    const char *p = arr_start + 1;
    const char *arr_end = arr_start + arr_len - 1U;
    int max_depth = s_json_state.policy.max_json_depth;
    size_t count = 0U;

    while (p < arr_end) {
        p = json_skip_ws_span(p, arr_end);
        if (p >= arr_end) {
            break;
        }
        if (*p == ',') {
            s_json_state.syntax_faults++;
            return false;
        }

        const char *elem_s = p;
        int depth = 0;

        while (p < arr_end) {
            char c = *p;
            if (c == '"') {
                bool esc_err = false;
                const char *close_q = json_skip_string_span(
                    p, arr_end, s_json_state.policy.strict_utf8_escapes, &esc_err);
                if (close_q == NULL) {
                    s_json_state.syntax_faults++;
                    return false;
                }
                p = close_q + 1;
                continue;
            }
            if (c == '{' || c == '[') {
                depth++;
                if (depth + 1 > max_depth) {
                    json_record_depth_overflow("json_array_locate_element",
                                               (size_t)(p - arr_start));
                    return false;
                }
            } else if (c == '}' || c == ']') {
                depth--;
                if (depth < 0) {
                    s_json_state.syntax_faults++;
                    return false;
                }
            } else if (c == ',' && depth == 0) {
                break;
            }
            p++;
        }

        if (depth != 0) {
            s_json_state.syntax_faults++;
            return false;
        }

        const char *elem_e = p;
        while (elem_e > elem_s && json_is_whitespace_char(*(elem_e - 1))) {
            elem_e--;
        }

        if (out_elem_start != NULL && count == target_idx) {
            *out_elem_start = elem_s;
            if (out_elem_end != NULL) {
                *out_elem_end = elem_e;
            }
            if (out_count == NULL) {
                return (elem_e > elem_s);
            }
        }

        count++;
        if (p < arr_end && *p == ',') {
            p++;
        }
    }

    if (out_count != NULL) {
        *out_count = count;
        return true;
    }
    return false;
}

static bool json_validate_root_span_internal(const char *json_start,
                                             size_t effective_len,
                                             const json_policy_t *policy,
                                             json_validation_report_t *out_report)
{
    json_validation_report_t local_rep;
    memset(&local_rep, 0, sizeof(local_rep));
    local_rep.valid = false;

    if (json_start == NULL || effective_len == 0U || policy == NULL ||
        !json_policy_is_valid(policy)) {
        local_rep.error_code = IO_ERR_JSON_SYNTAX;
        str_safe_copy(local_rep.diagnostic,
                      "NULL buffer, zero length, or invalid parser policy.",
                      sizeof(local_rep.diagnostic));
        if (out_report != NULL) {
            *out_report = local_rep;
        }
        return false;
    }

    if (effective_len > policy->max_payload_bytes) {
        local_rep.error_code = IO_ERR_JSON_SYNTAX;
        local_rep.error_offset = policy->max_payload_bytes;
        snprintf(local_rep.diagnostic, sizeof(local_rep.diagnostic),
                 "JSON payload %zu B exceeds max_payload_bytes (%zu B).",
                 effective_len, policy->max_payload_bytes);
        if (out_report != NULL) {
            *out_report = local_rep;
        }
        return false;
    }

    const char *end = json_start + effective_len;
    const char *p = json_skip_ws_span(json_start, end);
    if (p >= end || (*p != '{' && *p != '[')) {
        local_rep.error_code = IO_ERR_JSON_SYNTAX;
        local_rep.error_offset = (size_t)(p - json_start);
        str_safe_copy(local_rep.diagnostic,
                      "Expected JSON root '{' or '[' token.",
                      sizeof(local_rep.diagnostic));
        if (out_report != NULL) {
            *out_report = local_rep;
        }
        return false;
    }

    char nest_stack[JSON_MAX_NESTING];
    int stack_top = 0;
    uint64_t top_key_hashes[64];
    size_t top_key_count = 0U;
    bool root_closed = false;

    while (p < end) {
        p = json_skip_ws_span(p, end);
        if (p >= end) {
            break;
        }

        if (root_closed) {
            local_rep.error_code = IO_ERR_JSON_SYNTAX;
            local_rep.error_offset = (size_t)(p - json_start);
            str_safe_copy(local_rep.diagnostic,
                          "Unexpected trailing tokens after closed JSON root container.",
                          sizeof(local_rep.diagnostic));
            if (out_report != NULL) {
                *out_report = local_rep;
            }
            return false;
        }

        char c = *p;
        if (c == '{' || c == '[') {
            if (stack_top >= policy->max_json_depth) {
                local_rep.error_code = IO_ERR_JSON_DEPTH_EXCEEDED;
                local_rep.error_offset = (size_t)(p - json_start);
                local_rep.max_depth_observed = stack_top + 1;
                snprintf(local_rep.diagnostic, sizeof(local_rep.diagnostic),
                         "Nesting depth exceeded policy max_json_depth (%d).",
                         policy->max_json_depth);
                if (out_report != NULL) {
                    *out_report = local_rep;
                }
                return false;
            }
            nest_stack[stack_top++] = c;
            if (stack_top > local_rep.max_depth_observed) {
                local_rep.max_depth_observed = stack_top;
            }
            local_rep.token_count++;
            p++;
        } else if (c == '}' || c == ']') {
            if (stack_top <= 0) {
                local_rep.error_code = IO_ERR_JSON_SYNTAX;
                local_rep.error_offset = (size_t)(p - json_start);
                str_safe_copy(local_rep.diagnostic,
                              "Unexpected closing container delimiter.",
                              sizeof(local_rep.diagnostic));
                if (out_report != NULL) {
                    *out_report = local_rep;
                }
                return false;
            }
            char expected = (nest_stack[stack_top - 1] == '{') ? '}' : ']';
            if (c != expected) {
                local_rep.error_code = IO_ERR_JSON_SYNTAX;
                local_rep.error_offset = (size_t)(p - json_start);
                str_safe_copy(local_rep.diagnostic,
                              "Mismatched closing '}' vs ']' delimiter.",
                              sizeof(local_rep.diagnostic));
                if (out_report != NULL) {
                    *out_report = local_rep;
                }
                return false;
            }
            stack_top--;
            if (stack_top == 0) {
                root_closed = true;
            }
            p++;
        } else if (c == '"') {
            bool esc_err = false;
            const char *close_q = json_skip_string_span(
                p, end, policy->strict_utf8_escapes, &esc_err);
            if (close_q == NULL) {
                local_rep.error_code = IO_ERR_JSON_SYNTAX;
                local_rep.error_offset = (size_t)(p - json_start);
                str_safe_copy(local_rep.diagnostic,
                              esc_err
                                  ? "Illegal escape or raw control byte inside JSON string."
                                  : "Unterminated quoted JSON string literal.",
                              sizeof(local_rep.diagnostic));
                if (out_report != NULL) {
                    *out_report = local_rep;
                }
                return false;
            }

            if (policy->reject_duplicate_keys && stack_top == 1 && nest_stack[0] == '{') {
                const char *after = json_skip_ws_span(close_q + 1, end);
                if (after < end && *after == ':') {
                    char key_tmp[JSON_MAX_KEY_LEN];
                    size_t raw_klen = (size_t)(close_q - (p + 1));
                    if (raw_klen >= sizeof(key_tmp)) {
                        raw_klen = sizeof(key_tmp) - 1U;
                    }
                    memcpy(key_tmp, p + 1, raw_klen);
                    key_tmp[raw_klen] = '\0';
                    uint64_t h = str_fnv1a64(key_tmp);
                    for (size_t d = 0U; d < top_key_count; d++) {
                        if (top_key_hashes[d] == h) {
                            local_rep.error_code = IO_ERR_JSON_SYNTAX;
                            local_rep.error_offset = (size_t)(p - json_start);
                            str_safe_copy(local_rep.diagnostic,
                                          "Duplicate top-level object key rejected by policy.",
                                          sizeof(local_rep.diagnostic));
                            if (out_report != NULL) {
                                *out_report = local_rep;
                            }
                            return false;
                        }
                    }
                    if (top_key_count < (sizeof(top_key_hashes) / sizeof(top_key_hashes[0]))) {
                        top_key_hashes[top_key_count++] = h;
                    }
                }
            }

            local_rep.token_count++;
            p = close_q + 1;
        } else if (c == ':' || c == ',') {
            const char *next_tok = json_skip_ws_span(p + 1, end);
            if (next_tok >= end || *next_tok == ',' || *next_tok == ':' ||
                (*next_tok == '}' && c == ':') || (*next_tok == ']' && c == ':')) {
                local_rep.error_code = IO_ERR_JSON_SYNTAX;
                local_rep.error_offset = (size_t)(p - json_start);
                str_safe_copy(local_rep.diagnostic,
                              "Missing value after ':' or trailing/empty ',' delimiter.",
                              sizeof(local_rep.diagnostic));
                if (out_report != NULL) {
                    *out_report = local_rep;
                }
                return false;
            }
            p++;
        } else if (c == '-' || isdigit((unsigned char)c) != 0) {
            char num_buf[64];
            size_t tok_len = 0U;
            if (!json_extract_number_token(p, end, num_buf, sizeof(num_buf), &tok_len)) {
                local_rep.error_code = IO_ERR_JSON_SYNTAX;
                local_rep.error_offset = (size_t)(p - json_start);
                str_safe_copy(local_rep.diagnostic,
                              "Oversized or invalid numeric literal in JSON.",
                              sizeof(local_rep.diagnostic));
                if (out_report != NULL) {
                    *out_report = local_rep;
                }
                return false;
            }
            char *num_end = NULL;
            errno = 0;
            (void)strtod(num_buf, &num_end);
            if (num_end == num_buf || *num_end != '\0' || errno == ERANGE) {
                local_rep.error_code = IO_ERR_JSON_SYNTAX;
                local_rep.error_offset = (size_t)(p - json_start);
                str_safe_copy(local_rep.diagnostic,
                              "Malformed or out-of-range numeric literal in JSON.",
                              sizeof(local_rep.diagnostic));
                if (out_report != NULL) {
                    *out_report = local_rep;
                }
                return false;
            }
            local_rep.token_count++;
            p += tok_len;
        } else if ((size_t)(end - p) >= 4U && strncmp(p, "true", 4) == 0 &&
                   json_is_value_delimiter(p + 4, end)) {
            local_rep.token_count++;
            p += 4;
        } else if ((size_t)(end - p) >= 5U && strncmp(p, "false", 5) == 0 &&
                   json_is_value_delimiter(p + 5, end)) {
            local_rep.token_count++;
            p += 5;
        } else if ((size_t)(end - p) >= 4U && strncmp(p, "null", 4) == 0 &&
                   json_is_value_delimiter(p + 4, end)) {
            local_rep.token_count++;
            p += 4;
        } else {
            local_rep.error_code = IO_ERR_JSON_SYNTAX;
            local_rep.error_offset = (size_t)(p - json_start);
            str_safe_copy(local_rep.diagnostic,
                          "Unexpected unquoted keyword or invalid JSON byte.",
                          sizeof(local_rep.diagnostic));
            if (out_report != NULL) {
                *out_report = local_rep;
            }
            return false;
        }

        if (local_rep.token_count > policy->max_json_tokens) {
            local_rep.error_code = IO_ERR_JSON_SYNTAX;
            local_rep.error_offset = (size_t)(p - json_start);
            snprintf(local_rep.diagnostic, sizeof(local_rep.diagnostic),
                     "JSON token count exceeded policy max_json_tokens (%zu).",
                     policy->max_json_tokens);
            if (out_report != NULL) {
                *out_report = local_rep;
            }
            return false;
        }
    }

    if (!root_closed || stack_top != 0) {
        local_rep.error_code = IO_ERR_JSON_SYNTAX;
        local_rep.error_offset = effective_len;
        str_safe_copy(local_rep.diagnostic,
                      "Unclosed root JSON container at end of stream.",
                      sizeof(local_rep.diagnostic));
        if (out_report != NULL) {
            *out_report = local_rep;
        }
        return false;
    }

    local_rep.valid = true;
    local_rep.error_code = 0U;
    local_rep.error_offset = 0U;
    str_safe_copy(local_rep.diagnostic,
                  "Structural JSON span validation passed cleanly.",
                  sizeof(local_rep.diagnostic));
    if (out_report != NULL) {
        *out_report = local_rep;
    }
    return true;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Operational Business Logic, Span Scopes & Runtime Hydration )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0 (Core Utilities)
Module:        cog.json_utils
Mission:       Bounded JSON Scanners, Typed Extractors & Safe-Mode Policy Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: JSON Token Locator Cogs
Mission: Locate JSON key values, objects, and arrays accurately across flat or
         span-bounded buffers without crossing container boundaries.

  ### For Game Designers & Non-Programmers:
  Finds named properties, nested tables, and lists inside JSON files cleanly while
  ignoring similarly named words hidden inside quoted text descriptions.

  ### For Systems Programmers & Engine Architects:
  Wraps Sub-Assembly 2.5 span scanners (`json_find_key_span_internal`,
  `json_find_delimited_span_internal`) while tracking depth overflows and syntax
  faults cleanly without heap allocations.
  @triggers `io.json.depth_bounds`, `LOG_CAT_IO`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Locates a JSON key in a null-terminated buffer and returns its value token.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds where a named setting starts in a JSON file so its value can be read next.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates to `json_find_key_span_internal(json, 0U, key)` respecting escapes,
 * nesting policy depth, and object-key colon syntax. Zero heap usage.
 * @triggers `io.json.depth_bounds`, `LOG_CAT_IO` on nesting overflow.
 *
 * @param[in] json Null-terminated JSON buffer.
 * @param[in] key  Null-terminated property name (without quotes).
 * @return Pointer to first non-whitespace value byte after ':', or NULL if absent/invalid.
 */
const char *json_find_key(const char *json, const char *key)
{
    /* Step 1: Delegate full null-terminated buffer scan to canonical span locator. */
    return json_find_key_span_internal(json, 0U, key);
}

/**
 * @brief Locates a JSON key strictly inside a caller-delimited byte span.
 *
 * ### For Game Designers & Non-Programmers:
 * Searches only within one isolated section of a JSON file so sibling sections can
 * never cause false matches.
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes effective span length when `json_len == 0` and invokes
 * `json_find_key_span_internal()` clamped to `[json_start, json_start + eff_len)`.
 * @triggers `io.json.depth_bounds`, `LOG_CAT_IO` on nesting overflow.
 *
 * @param[in] json_start Pointer to JSON slice start.
 * @param[in] json_len   Exact byte length of slice (`0` measures NUL-terminated buffer).
 * @param[in] key        Null-terminated key name to locate.
 * @return Pointer to value token following ':' within slice, or NULL if absent/invalid.
 */
const char *json_find_key_span(const char *json_start, size_t json_len, const char *key)
{
    /* Step 1: Reject NULL pointers and resolve NUL-terminated fallback span length. */
    if (json_start == NULL || key == NULL) {
        return NULL;
    }
    size_t eff_len = (json_len > 0U) ? json_len : strlen(json_start);
    if (eff_len == 0U) {
        return NULL;
    }

    /* Step 2: Execute span-isolated key scan under active depth bounds. */
    return json_find_key_span_internal(json_start, eff_len, key);
}

/**
 * @brief Locates a nested `{...}` object span for a key strictly inside a slice.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a sub-table inside one JSON section and fences its exact start and end
 * braces so nothing outside that sub-table can interfere.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears output bounds defensively, resolves `object_key` via
 * `json_find_key_span_internal()`, verifies opening `'{'`, measures balanced braces
 * via `json_find_delimited_span_internal()`, and records depth or syntax telemetry.
 * @triggers `io.json.depth_bounds`, `LOG_CAT_IO` on depth overflow.
 *
 * @param[in]  json_start    Pointer to JSON slice start.
 * @param[in]  json_len      Exact byte length of slice (`0` measures NUL-terminated).
 * @param[in]  object_key    Key whose value must open a `{...}` object.
 * @param[out] out_obj_start Receives pointer to opening `'{'` (NULL on failure).
 * @param[out] out_obj_len   Receives total byte span including `'}'` (`0` on failure).
 * @return true if balanced object span resolved cleanly; false otherwise.
 */
bool json_find_object_span(const char *json_start, size_t json_len,
                           const char *object_key,
                           const char **out_obj_start, size_t *out_obj_len)
{
    /* Step 1: Initialize caller output span descriptors defensively. */
    if (out_obj_start == NULL || out_obj_len == NULL) {
        return false;
    }
    *out_obj_start = NULL;
    *out_obj_len = 0U;

    if (json_start == NULL || object_key == NULL) {
        return false;
    }

    /* Step 2: Locate object key inside slice and verify opening brace token. */
    const char *end = (json_len > 0U) ? (json_start + json_len) : NULL;
    const char *val = json_find_key_span_internal(json_start, json_len, object_key);
    if (val == NULL || json_at_end(val, end) || *val != '{') {
        return false;
    }

    /* Step 3: Walk balanced `{...}` delimiters while guarding nesting & syntax errors. */
    size_t obj_len = 0U;
    bool depth_exceeded = false;
    bool syntax_err = false;
    if (!json_find_delimited_span_internal(val, end, '{', '}',
                                           &obj_len, NULL,
                                           &depth_exceeded, &syntax_err)) {
        if (depth_exceeded) {
            json_record_depth_overflow("json_find_object", (size_t)(val - json_start));
        } else if (syntax_err) {
            s_json_state.syntax_faults++;
        }
        return false;
    }

    *out_obj_start = val;
    *out_obj_len = obj_len;
    s_json_state.extractions_ok++;
    return true;
}

/**
 * @brief Locates a nested `[...]` array span for a key strictly inside a slice.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a list inside a bounded JSON section and locks down its exact boundaries
 * so adjacent lists cannot be read by accident.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears output parameters defensively, locates `array_key` within `[json_start, end)`,
 * verifies opening `'['`, walks balanced brackets via
 * `json_find_delimited_span_internal()`, and records overflow or syntax faults.
 * @triggers `io.json.depth_bounds`, `LOG_CAT_IO` on depth overflow.
 *
 * @param[in]  json_start    Pointer to JSON slice start.
 * @param[in]  json_len      Exact byte length of slice (`0` measures NUL-terminated).
 * @param[in]  array_key     Key whose value must open a `[...]` array.
 * @param[out] out_arr_start Receives pointer to opening `'['` (NULL on failure).
 * @param[out] out_arr_len   Receives total byte span including `']'` (`0` on failure).
 * @return true if balanced array span resolved cleanly; false otherwise.
 */
bool json_find_array_span(const char *json_start, size_t json_len,
                          const char *array_key,
                          const char **out_arr_start, size_t *out_arr_len)
{
    /* Step 1: Initialize caller output span descriptors defensively. */
    if (out_arr_start == NULL || out_arr_len == NULL) {
        return false;
    }
    *out_arr_start = NULL;
    *out_arr_len = 0U;

    if (json_start == NULL || array_key == NULL) {
        return false;
    }

    /* Step 2: Locate array key inside slice and verify opening square bracket. */
    const char *end = (json_len > 0U) ? (json_start + json_len) : NULL;
    const char *val = json_find_key_span_internal(json_start, json_len, array_key);
    if (val == NULL || json_at_end(val, end) || *val != '[') {
        return false;
    }

    /* Step 3: Walk balanced `[...]` delimiters while enforcing active depth limits. */
    size_t arr_len = 0U;
    bool depth_exceeded = false;
    bool syntax_err = false;
    if (!json_find_delimited_span_internal(val, end, '[', ']',
                                           &arr_len, NULL,
                                           &depth_exceeded, &syntax_err)) {
        if (depth_exceeded) {
            json_record_depth_overflow("json_find_array", (size_t)(val - json_start));
        } else if (syntax_err) {
            s_json_state.syntax_faults++;
        }
        return false;
    }

    *out_arr_start = val;
    *out_arr_len = arr_len;
    s_json_state.extractions_ok++;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Typed Value & Nested Object Extraction Cogs
Mission: Delegate all string, integer, unsigned size, boolean, float, and
         object-scoped lookups through canonical bounds-checked decoders with
         unconditional NUL-termination and type-mismatch counters.

  ### For Game Designers & Non-Programmers:
  Reads text, whole numbers, multipliers, switches, and nested stat groups out of
  JSON payloads safely without allocating temporary memory.

  ### For Systems Programmers & Engine Architects:
  Combines key/parent lookup (`json_find_key_span_internal` /
  `json_resolve_object_value_span`) with Sub-Assembly 2.5 scalar decoders cleanly.
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts a quoted JSON string value into a bounded NUL-terminated buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Copies a text value out of a JSON file cleanly, decoding escaped quotes and
 * preventing partial overflows if the destination buffer is too small.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears `out_val[0]`, verifies non-string type mismatches in telemetry, and invokes
 * `json_decode_string_span()` with strict capacity checks and guaranteed NUL byte.
 * @triggers None on expected miss; depth overflow paths delegate to locator probe.
 *
 * @param[in]  json     Null-terminated JSON buffer.
 * @param[in]  key      Key name to look up.
 * @param[out] out_val  Destination buffer receiving unescaped string.
 * @param[in]  out_size Capacity of `out_val` in bytes.
 * @return true if key held a valid string that fit within `out_size`; false otherwise.
 */
bool json_get_string(const char *json, const char *key, char *out_val,
                     size_t out_size)
{
    /* Step 1: Validate caller buffer and initialize leading NUL terminator. */
    if (out_val == NULL || out_size == 0U) {
        return false;
    }
    out_val[0] = '\0';

    /* Step 2: Locate key token, count wrong-type values, and decode bounded string. */
    const char *val = json_find_key_span_internal(json, 0U, key);
    if (val == NULL || *val != '"') {
        if (val != NULL) {
            s_json_state.type_mismatches++;
        }
        return false;
    }
    return json_decode_string_span(val, NULL, out_val, out_size, NULL);
}

/**
 * @brief Extracts an unsigned `size_t` integer value for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a non-negative count or byte cap from JSON while rejecting negative or
 * corrupted numbers safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `key` via `json_find_key_span_internal()` and delegates delimiter-strict
 * `strtoull` conversion to `json_parse_size_t_span()`. Leaves `*out_val` untouched on error.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to look up.
 * @param[out] out_val Destination `size_t` storage.
 * @return true if key parsed cleanly as a non-negative integer; false otherwise.
 */
bool json_get_size_t(const char *json, const char *key, size_t *out_val)
{
    /* Step 1: Validate destination pointer and resolve key token. */
    if (out_val == NULL) {
        return false;
    }
    const char *val = json_find_key_span_internal(json, 0U, key);
    if (val == NULL) {
        return false;
    }

    /* Step 2: Parse unsigned integer with delimiter & overflow checks. */
    return json_parse_size_t_span(val, NULL, out_val);
}

/**
 * @brief Extracts a signed 32-bit `int` value for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a positive or negative whole number (like alignment or stat modifiers) from JSON.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `key` via `json_find_key_span_internal()` and invokes `json_parse_int_span()`
 * with `INT_MIN`..`INT_MAX` clamping and delimiter validation.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to look up.
 * @param[out] out_val Destination `int` storage.
 * @return true if key parsed cleanly as a valid `int`; false otherwise.
 */
bool json_get_int(const char *json, const char *key, int *out_val)
{
    /* Step 1: Validate destination pointer and resolve key token. */
    if (out_val == NULL) {
        return false;
    }
    const char *val = json_find_key_span_internal(json, 0U, key);
    if (val == NULL) {
        return false;
    }

    /* Step 2: Parse signed 32-bit integer token cleanly. */
    return json_parse_int_span(val, NULL, out_val);
}

/**
 * @brief Extracts a strict JSON boolean (`true` or `false`) for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads an on/off toggle setting safely without mistaking partial words or numbers
 * for true/false switches.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `key` via `json_find_key_span_internal()` and calls `json_parse_bool_span()`,
 * requiring valid JSON delimiter termination immediately after the literal.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to look up.
 * @param[out] out_val Destination `bool` storage.
 * @return true if literal matched `true` or `false`; false otherwise.
 */
bool json_get_bool(const char *json, const char *key, bool *out_val)
{
    /* Step 1: Validate destination pointer and resolve key token. */
    if (out_val == NULL) {
        return false;
    }
    const char *val = json_find_key_span_internal(json, 0U, key);
    if (val == NULL) {
        return false;
    }

    /* Step 2: Parse delimiter-verified JSON boolean literal. */
    return json_parse_bool_span(val, NULL, out_val);
}

/**
 * @brief Extracts a finite floating-point `float` value for a given key.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads decimal scaling multipliers and floating settings safely while rejecting
 * NaN/Inf or malformed numeric strings.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `key` via `json_find_key_span_internal()` and delegates finite `strtod`
 * parsing + cast range verification to `json_parse_float_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json    Null-terminated JSON buffer.
 * @param[in]  key     Key name to look up.
 * @param[out] out_val Destination `float` storage.
 * @return true if valid finite float parsed cleanly; false otherwise.
 */
bool json_get_float(const char *json, const char *key, float *out_val)
{
    /* Step 1: Validate destination pointer and resolve key token. */
    if (out_val == NULL) {
        return false;
    }
    const char *val = json_find_key_span_internal(json, 0U, key);
    if (val == NULL) {
        return false;
    }

    /* Step 2: Parse delimiter-verified finite float literal. */
    return json_parse_float_span(val, NULL, out_val);
}

/**
 * @brief Locates balanced `{...}` boundaries for a key in a null-terminated JSON buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a sub-table anywhere in a null-terminated JSON payload so subsequent lookups
 * stay confined to that section.
 *
 * ### For Systems Programmers & Engine Architects:
 * Thin null-terminated wrapper forwarding `(json, 0U, ...)` to `json_find_object_span()`.
 * @triggers `io.json.depth_bounds`, `LOG_CAT_IO` on depth overflow.
 *
 * @param[in]  json          Null-terminated JSON buffer.
 * @param[in]  object_key    Key naming the `{...}` sub-object.
 * @param[out] out_obj_start Receives pointer to opening `'{'`.
 * @param[out] out_obj_len   Receives total byte span including closing `'}'`.
 * @return true if sub-object was located and balanced; false otherwise.
 */
bool json_find_object(const char *json, const char *object_key,
                      const char **out_obj_start, size_t *out_obj_len)
{
    /* Step 1: Forward null-terminated buffer to span-isolated object locator. */
    return json_find_object_span(json, 0U, object_key, out_obj_start, out_obj_len);
}

/**
 * @brief Extracts a string field strictly scoped inside an optional parent JSON object.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a text label nested inside a named section (e.g. `metadata.version`) without
 * accidentally picking up a same-named property elsewhere in the file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Isolates parent object slice via `json_resolve_object_value_span()`, records
 * non-string type mismatches in telemetry, and decodes bounded UTF-8/ASCII escapes.
 * @triggers `io.json.depth_bounds` if container scan exceeds `max_json_depth`.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Parent object key (or NULL for top-level scope).
 * @param[in]  key            Target property key inside parent object.
 * @param[out] out_val        Destination buffer receiving decoded string.
 * @param[in]  out_size       Capacity of `out_val` in bytes.
 * @return true if parent object and string field resolved and fit in `out_size`; false otherwise.
 */
bool json_get_object_string(const char *json, const char *parent_obj_key,
                            const char *key, char *out_val, size_t out_size)
{
    /* Step 1: Initialize destination buffer defensively before container resolution. */
    if (out_val == NULL || out_size == 0U) {
        return false;
    }
    out_val[0] = '\0';

    /* Step 2: Resolve scoped field token inside parent object and decode string value. */
    const char *obj_end = NULL;
    const char *val = json_resolve_object_value_span(json, 0U, parent_obj_key, key, &obj_end);
    if (val == NULL || json_at_end(val, obj_end) || *val != '"') {
        if (val != NULL) {
            s_json_state.type_mismatches++;
        }
        return false;
    }
    return json_decode_string_span(val, obj_end, out_val, out_size, NULL);
}

/**
 * @brief Extracts a signed `int` field strictly scoped inside an optional parent object.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a whole-number setting tucked inside a named subsection without colliding with
 * matching field names in other subsections.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounds key scan to `[parent_start, obj_end)` via `json_resolve_object_value_span()`
 * and parses signed 32-bit value via `json_parse_int_span()`.
 * @triggers `io.json.depth_bounds` if container scan exceeds `max_json_depth`.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Parent object key (or NULL for top-level scope).
 * @param[in]  key            Target integer key inside parent object.
 * @param[out] out_val        Destination signed integer storage.
 * @return true if parent object and valid `int` were resolved; false otherwise.
 */
bool json_get_object_int(const char *json, const char *parent_obj_key,
                         const char *key, int *out_val)
{
    /* Step 1: Validate output storage and locate scoped value inside parent slice. */
    if (out_val == NULL) {
        return false;
    }
    const char *obj_end = NULL;
    const char *val = json_resolve_object_value_span(json, 0U, parent_obj_key, key, &obj_end);
    if (val == NULL) {
        return false;
    }

    /* Step 2: Parse signed integer within strict parent object end bound. */
    return json_parse_int_span(val, obj_end, out_val);
}

/**
 * @brief Extracts a finite `float` field strictly scoped inside an optional parent object.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a decimal multiplier nested inside a specific section without picking up fields
 * from sibling sections.
 *
 * ### For Systems Programmers & Engine Architects:
 * Isolates parent slice via `json_resolve_object_value_span()` and delegates finite
 * decimal conversion to `json_parse_float_span()` bounded by `obj_end`.
 * @triggers `io.json.depth_bounds` if container scan exceeds `max_json_depth`.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Parent object key (or NULL for top-level scope).
 * @param[in]  key            Target float key inside parent object.
 * @param[out] out_val        Destination finite float storage.
 * @return true if parent object and finite float resolved cleanly; false otherwise.
 */
bool json_get_object_float(const char *json, const char *parent_obj_key,
                           const char *key, float *out_val)
{
    /* Step 1: Validate output storage and locate scoped value inside parent slice. */
    if (out_val == NULL) {
        return false;
    }
    const char *obj_end = NULL;
    const char *val = json_resolve_object_value_span(json, 0U, parent_obj_key, key, &obj_end);
    if (val == NULL) {
        return false;
    }

    /* Step 2: Parse finite float bounded to parent object end address. */
    return json_parse_float_span(val, obj_end, out_val);
}

/**
 * @brief Extracts a `bool` field strictly scoped inside an optional parent object.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a true/false flag inside one specific sub-table cleanly without cross-talk from
 * adjacent sections.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounds token resolution via `json_resolve_object_value_span()` and enforces strict
 * boolean token delimiting within `[val, obj_end)`.
 * @triggers `io.json.depth_bounds` if container scan exceeds `max_json_depth`.
 *
 * @param[in]  json           Null-terminated JSON buffer.
 * @param[in]  parent_obj_key Parent object key (or NULL for top-level scope).
 * @param[in]  key            Target boolean key inside parent object.
 * @param[out] out_val        Destination `bool` storage.
 * @return true if parent object and boolean literal resolved cleanly; false otherwise.
 */
bool json_get_object_bool(const char *json, const char *parent_obj_key,
                          const char *key, bool *out_val)
{
    /* Step 1: Validate output storage and locate scoped value inside parent slice. */
    if (out_val == NULL) {
        return false;
    }
    const char *obj_end = NULL;
    const char *val = json_resolve_object_value_span(json, 0U, parent_obj_key, key, &obj_end);
    if (val == NULL) {
        return false;
    }

    /* Step 2: Parse boolean literal bounded by parent object end address. */
    return json_parse_bool_span(val, obj_end, out_val);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Array Inspection & Indexed Element Extraction Cogs
Mission: Locate JSON `[...]` blocks safely and extract element count, string,
         and object elements using ONE unified zero-heap iterator
         (`json_array_locate_element`).

  ### For Game Designers & Non-Programmers:
  Counts how many items exist in a JSON list and extracts individual text labels or
  entire sub-tables cleanly by position.

  ### For Systems Programmers & Engine Architects:
  Centralizes comma/bracket walking inside `json_array_locate_element()` (2.5) so
  array count, string decode, and nested object copy share identical escaping rules.
  @triggers `io.json.depth_bounds`, `LOG_CAT_IO`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Locates balanced `[...]` boundaries for an array key in null-terminated JSON.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds a list inside a JSON file (such as folder routes or tag lists) so elements
 * can be counted and extracted safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Thin null-terminated wrapper forwarding `(json, 0U, ...)` to `json_find_array_span()`.
 * @triggers `io.json.depth_bounds`, `LOG_CAT_IO` on depth overflow.
 *
 * @param[in]  json          Null-terminated JSON buffer.
 * @param[in]  array_key     Key naming the `[...]` array.
 * @param[out] out_arr_start Receives pointer to opening `'['`.
 * @param[out] out_arr_len   Receives total slice byte length including `']'`.
 * @return true if array located and brackets balanced; false otherwise.
 */
bool json_find_array(const char *json, const char *array_key,
                     const char **out_arr_start, size_t *out_arr_len)
{
    /* Step 1: Forward null-terminated buffer to span-isolated array locator. */
    return json_find_array_span(json, 0U, array_key, out_arr_start, out_arr_len);
}

/**
 * @brief Counts top-level comma-separated elements inside a JSON `[...]` array slice.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports how many entries are listed in a JSON array, returning 0 for empty `[]` lists.
 *
 * ### For Systems Programmers & Engine Architects:
 * Drives `json_array_locate_element()` with target index `SIZE_MAX` so the iterator
 * walks nested containers/strings accurately without heap allocations.
 * @triggers None on clean count or malformed input.
 *
 * @param[in] arr_start Pointer to opening `'['` of JSON array.
 * @param[in] arr_len   Total byte length of array slice.
 * @return Number of elements in array (`0` if empty or invalid slice).
 */
size_t json_array_get_count(const char *arr_start, size_t arr_len)
{
    /* Step 1: Walk entire array span using canonical iterator to obtain element count. */
    size_t count = 0U;
    if (!json_array_locate_element(arr_start, arr_len, SIZE_MAX, &count, NULL, NULL)) {
        return 0U;
    }
    return count;
}

/**
 * @brief Extracts the i-th string element from a bounded JSON `[...]` array slice.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a single text entry at a given 0-based position from a list without needing
 * temporary memory allocations.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears `out_buf[0]`, queries `json_array_locate_element()` for `(elem_s, elem_e)`,
 * validates quoted string literal start, and decodes escapes via `json_decode_string_span()`.
 * @triggers None on out-of-range index or type mismatch.
 *
 * @param[in]  arr_start Pointer to opening `'['` of JSON array.
 * @param[in]  arr_len   Total byte length of array slice.
 * @param[in]  index     0-based element index.
 * @param[out] out_buf   Caller string destination buffer (NUL-terminated).
 * @param[in]  out_size  Capacity of `out_buf` in bytes.
 * @return true if index existed as a string and decoded within `out_size`; false otherwise.
 */
bool json_array_get_string(const char *arr_start, size_t arr_len,
                           size_t index, char *out_buf, size_t out_size)
{
    /* Step 1: Validate caller buffer and clear leading byte defensively. */
    if (out_buf == NULL || out_size == 0U) {
        return false;
    }
    out_buf[0] = '\0';

    /* Step 2: Locate element slice by index and verify string literal delimiter. */
    const char *elem_s = NULL;
    const char *elem_e = NULL;
    if (!json_array_locate_element(arr_start, arr_len, index, NULL, &elem_s, &elem_e) ||
        elem_s == NULL || elem_e <= elem_s || *elem_s != '"') {
        return false;
    }

    /* Step 3: Decode bounded JSON string escapes into caller storage. */
    return json_decode_string_span(elem_s, elem_e, out_buf, out_size, NULL);
}

/**
 * @brief Copies the i-th `{...}` object slice from a bounded JSON array into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Extracts an entire nested entry (like one route rule or item definition) out of a
 * list so other extractors can inspect its fields cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Locates element slice via `json_array_locate_element()`, re-validates balanced braces
 * and policy depth via `json_find_delimited_span_internal()`, checks `obj_len < out_size`,
 * copies bytes verbatim, and guarantees trailing `'\0'`.
 * @triggers `io.json.depth_bounds` if object nesting exceeds `max_json_depth`.
 *
 * @param[in]  arr_start Pointer to opening `'['` of JSON array.
 * @param[in]  arr_len   Total byte length of array slice.
 * @param[in]  index     0-based element index.
 * @param[out] out_obj   Destination buffer receiving NUL-terminated object slice.
 * @param[in]  out_size  Capacity of `out_obj` in bytes.
 * @return true if nested object existed, balanced safely, and fit inside `out_size`; false otherwise.
 */
bool json_array_get_object(const char *arr_start, size_t arr_len,
                           size_t index, char *out_obj, size_t out_size)
{
    /* Step 1: Validate output buffer and clear leading byte defensively. */
    if (out_obj == NULL || out_size == 0U) {
        return false;
    }
    out_obj[0] = '\0';

    /* Step 2: Locate indexed element and confirm opening brace delimiter. */
    const char *elem_s = NULL;
    const char *elem_e = NULL;
    if (!json_array_locate_element(arr_start, arr_len, index, NULL, &elem_s, &elem_e) ||
        elem_s == NULL || elem_e <= elem_s || *elem_s != '{') {
        return false;
    }

    /* Step 3: Measure balanced object delimiters and enforce active depth policy. */
    size_t obj_len = 0U;
    bool depth_exceeded = false;
    bool syntax_err = false;
    if (!json_find_delimited_span_internal(elem_s, elem_e, '{', '}',
                                           &obj_len, NULL,
                                           &depth_exceeded, &syntax_err)) {
        if (depth_exceeded) {
            json_record_depth_overflow("json_array_get_object", (size_t)(elem_s - arr_start));
        } else if (syntax_err) {
            s_json_state.syntax_faults++;
        }
        return false;
    }

    /* Step 4: Enforce caller buffer headroom and copy NUL-terminated object slice. */
    if (obj_len >= out_size) {
        return false;
    }

    memcpy(out_obj, elem_s, obj_len);
    out_obj[obj_len] = '\0';
    s_json_state.extractions_ok++;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Span-Bounded Extractors & Runtime Policy Configuration Cogs
Mission: Guarantee strict `[json_start, json_start + json_len)` boundary containment
         and manage live policy hydration from `io/loader_contracts.json` or `.rodata` ROM.

  ### For Game Designers & Non-Programmers:
  Reads text, numbers, and switches strictly inside fenced JSON blocks (`block_3_body`),
  while keeping parser depth and token safety limits synchronized with disk config files.

  ### For Systems Programmers & Engine Architects:
  Exposes span-hardened extractors plus policy hydration backed cleanly by
  `json_downgrade_policy_to_rom()` and `io.json.policy_bounds` probe escalation.
  @triggers `io.json.policy_bounds`, `io.json.depth_bounds`, `LOG_CAT_IO`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts a quoted string strictly within `[json_start, json_start + json_len)`.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely reads text values strictly from an isolated section (such as `block_3_body`)
 * so identical key names in metadata or audit blocks can never leak in.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies non-empty span bounds, locates `key` via `json_find_key_span_internal()`,
 * counts type mismatches cleanly, and invokes `json_decode_string_span()` clamped to `end`.
 * @triggers `io.json.depth_bounds` if scan breaches `max_json_depth`.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Exact byte length of slice (`> 0`).
 * @param[in]  key        Property key to resolve inside slice.
 * @param[out] out_val    Destination string buffer (unconditionally null-terminated).
 * @param[in]  out_size   Capacity of `out_val` in bytes.
 * @return true if key held a valid string that decoded within `out_size`; false otherwise.
 */
bool json_get_string_span(const char *json_start, size_t json_len,
                          const char *key, char *out_val, size_t out_size)
{
    /* Step 1: Validate caller buffer and non-zero slice boundaries. */
    if (out_val == NULL || out_size == 0U) {
        return false;
    }
    out_val[0] = '\0';
    if (json_start == NULL || json_len == 0U || key == NULL) {
        return false;
    }

    /* Step 2: Locate key inside exact [json_start, end) slice and decode string. */
    const char *end = json_start + json_len;
    const char *val = json_find_key_span_internal(json_start, json_len, key);
    if (val == NULL || val >= end || *val != '"') {
        if (val != NULL) {
            s_json_state.type_mismatches++;
        }
        return false;
    }
    return json_decode_string_span(val, end, out_val, out_size, NULL);
}

/**
 * @brief Extracts a strict signed `int` strictly within `[json_start, json_start + json_len)`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads whole numbers strictly inside one delimited JSON section while rejecting typos
 * such as trailing letters or out-of-range integers.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates non-zero span pointers, locates `key` inside `[json_start, json_len)`, and
 * invokes `json_parse_int_span()` bounded by `json_start + json_len`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Exact byte length of slice (`> 0`).
 * @param[in]  key        Property key to resolve inside slice.
 * @param[out] out_val    Destination `int` storage (untouched on failure).
 * @return true if strictly valid `int` parsed cleanly; false otherwise.
 */
bool json_get_int_span(const char *json_start, size_t json_len,
                       const char *key, int *out_val)
{
    /* Step 1: Validate slice pointers and destination storage. */
    if (json_start == NULL || json_len == 0U || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate value token within slice and parse strict signed int. */
    const char *val = json_find_key_span_internal(json_start, json_len, key);
    if (val == NULL) {
        return false;
    }
    return json_parse_int_span(val, json_start + json_len, out_val);
}

/**
 * @brief Extracts a strict `size_t` strictly within `[json_start, json_start + json_len)`.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely reads non-negative counts and byte caps inside an isolated JSON block while
 * rejecting negative signs or oversized values.
 *
 * ### For Systems Programmers & Engine Architects:
 * Locates `key` strictly within `[json_start, json_len)` and forwards end bound to
 * `json_parse_size_t_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Exact byte length of slice (`> 0`).
 * @param[in]  key        Property key to resolve inside slice.
 * @param[out] out_val    Destination `size_t` storage (untouched on failure).
 * @return true if strictly valid unsigned integer parsed cleanly; false otherwise.
 */
bool json_get_size_t_span(const char *json_start, size_t json_len,
                          const char *key, size_t *out_val)
{
    /* Step 1: Validate slice pointers and destination storage. */
    if (json_start == NULL || json_len == 0U || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate value token within slice and parse strict unsigned size_t. */
    const char *val = json_find_key_span_internal(json_start, json_len, key);
    if (val == NULL) {
        return false;
    }
    return json_parse_size_t_span(val, json_start + json_len, out_val);
}

/**
 * @brief Extracts a finite `float` strictly within `[json_start, json_start + json_len)`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads decimal multipliers inside an isolated JSON block while rejecting malformed or
 * non-finite numbers (`NaN`/`Inf`).
 *
 * ### For Systems Programmers & Engine Architects:
 * Locates `key` strictly inside `[json_start, json_len)` and forwards slice end bound
 * to `json_parse_float_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Exact byte length of slice (`> 0`).
 * @param[in]  key        Property key to resolve inside slice.
 * @param[out] out_val    Destination `float` storage (untouched on failure).
 * @return true if finite float literal parsed cleanly; false otherwise.
 */
bool json_get_float_span(const char *json_start, size_t json_len,
                         const char *key, float *out_val)
{
    /* Step 1: Validate slice pointers and destination storage. */
    if (json_start == NULL || json_len == 0U || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate value token within slice and parse finite float. */
    const char *val = json_find_key_span_internal(json_start, json_len, key);
    if (val == NULL) {
        return false;
    }
    return json_parse_float_span(val, json_start + json_len, out_val);
}

/**
 * @brief Extracts a strict `bool` strictly within `[json_start, json_start + json_len)`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads true/false flags strictly within one fenced JSON section while rejecting
 * partial identifier prefixes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Locates `key` strictly within `[json_start, json_len)` and invokes
 * `json_parse_bool_span()` bounded by `json_start + json_len`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  json_start Slice start pointer.
 * @param[in]  json_len   Exact byte length of slice (`> 0`).
 * @param[in]  key        Property key to resolve inside slice.
 * @param[out] out_val    Destination `bool` storage (untouched on failure).
 * @return true if delimiter-terminated `true` or `false` matched cleanly; false otherwise.
 */
bool json_get_bool_span(const char *json_start, size_t json_len,
                        const char *key, bool *out_val)
{
    /* Step 1: Validate slice pointers and destination storage. */
    if (json_start == NULL || json_len == 0U || key == NULL || out_val == NULL) {
        return false;
    }

    /* Step 2: Locate value token within slice and parse delimiter-safe bool. */
    const char *val = json_find_key_span_internal(json_start, json_len, key);
    if (val == NULL) {
        return false;
    }
    return json_parse_bool_span(val, json_start + json_len, out_val);
}

/**
 * @brief Hydrates parser policy limits from `loader_contracts.json` or Tier-0 `.rodata` ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Activates JSON parser depth, token, and strict-escape safety limits from disk config
 * files, safely dropping back to built-in ROM defaults if the disk file is missing or corrupt.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves disk or `rom://` path via `io_fallback_path_resolve()`, reads bounded bytes,
 * parses contract via `io_fallback_parse_loader_contract_json()`, applies `block_3_body`
 * limit overrides prior to `json_policy_is_valid()`, and downgrades corrupt policies
 * cleanly via `json_downgrade_policy_to_rom()` + `io.json.policy_bounds`.
 * @triggers `io.json.policy_bounds`, `LOG_CAT_IO` on unreadable/corrupt disk policy.
 *
 * @param[in] rel_or_abs_path Optional override path (NULL resolves `"io/loader_contracts.json"`).
 * @return true if loaded cleanly from disk (`loaded_from_data = true`); false on ROM fallback.
 */
bool json_utils_load_policy(const char *rel_or_abs_path)
{
    /* Step 1: Ensure policy chassis is armed and prepare ROM fallback snapshot. */
    json_ensure_policy_armed();

    json_policy_t rom_policy;
    json_sync_policy_from_fallback(&rom_policy);

    /* Step 2: Resolve explicit disk file or default `"io/loader_contracts.json"` route. */
    char resolved_path[IO_FB_PATH_MAX];
    const char *target = NULL;

    if (rel_or_abs_path != NULL && rel_or_abs_path[0] != '\0') {
        if (io_fallback_fs_is_regular(rel_or_abs_path)) {
            target = rel_or_abs_path;
        } else if (io_fallback_path_resolve("config", rel_or_abs_path,
                                            resolved_path, sizeof(resolved_path))) {
            target = resolved_path;
        }
    } else if (io_fallback_path_resolve("config", "io/loader_contracts.json",
                                        resolved_path, sizeof(resolved_path))) {
        target = resolved_path;
    }

    if (target == NULL || io_fb_is_rom_uri(target)) {
        s_json_state.policy = rom_policy;
        s_json_state.policy_from_data = false;
        s_json_state.policy_armed = true;
        return false;
    }

    /* Step 3: Read disk JSON into stack-bounded buffer, downgrading cleanly if unreadable. */
    char cfg_buf[IO_FB_MEDIUM_BYTES];
    size_t read_bytes = 0U;
    if (!io_fallback_fs_read_bounded(target, cfg_buf, sizeof(cfg_buf), &read_bytes)) {
        s_json_state.policy = rom_policy;
        s_json_state.policy_from_data = false;
        s_json_state.policy_armed = true;
        JSON_EMIT_LOG(LOG_WARN, -0.35f,
                      "Disk loader_contracts unreadable at '%s'; using ROM Safe-Mode policy",
                      target);
        return false;
    }

    /* Step 4: Parse base loader contract and downgrade safely if syntax/envelope corrupt. */
    io_fb_loader_contract_t parsed_contract;
    if (!io_fallback_parse_loader_contract_json(cfg_buf, read_bytes, &parsed_contract)) {
        char log_msg[IO_FB_PATH_MAX + 96U];
        snprintf(log_msg, sizeof(log_msg),
                 "Corrupt JSON policy at '%s'; degraded to Safe-Mode ROM policy",
                 target);
        json_downgrade_policy_to_rom("json_utils_load_policy",
                                     "Corrupt loader_contracts JSON on disk; restored ROM policy",
                                     log_msg,
                                     "Corrupt loader_contracts JSON on disk");
        return false;
    }

    /* Step 5: Seed disk policy and overlay raw `block_3_body` bounds for contract enforcement. */
    json_policy_t disk_policy = rom_policy;
    if (parsed_contract.max_json_depth > 0 &&
        parsed_contract.max_json_depth <= JSON_MAX_NESTING) {
        disk_policy.max_json_depth = parsed_contract.max_json_depth;
    }
    if (parsed_contract.max_json_tokens > 0U &&
        parsed_contract.max_json_tokens <= JSON_DEFAULT_MAX_TOKENS) {
        disk_policy.max_json_tokens = parsed_contract.max_json_tokens;
    }
    if (parsed_contract.max_payload_bytes >= 64U &&
        parsed_contract.max_payload_bytes <= JSON_DEFAULT_MAX_PAYLOAD) {
        disk_policy.max_payload_bytes = parsed_contract.max_payload_bytes;
    }
    disk_policy.strict_utf8_escapes = parsed_contract.strict_utf8_escapes;
    disk_policy.reject_duplicate_keys = parsed_contract.reject_duplicate_keys;
    disk_policy.probe_await_on_corrupt = parsed_contract.probe_await_on_corrupt;

    const char *body_s = cfg_buf;
    size_t body_l = read_bytes;
    (void)json_find_object_span(cfg_buf, read_bytes, "block_3_body", &body_s, &body_l);

    int raw_depth = 0;
    if (json_get_int_span(body_s, body_l, "max_json_depth", &raw_depth)) {
        disk_policy.max_json_depth = raw_depth;
    }
    int raw_tokens = 0;
    if (json_get_int_span(body_s, body_l, "max_json_tokens", &raw_tokens)) {
        disk_policy.max_json_tokens = (raw_tokens > 0) ? (size_t)raw_tokens : 0U;
    }
    int raw_key_len = 0;
    if (json_get_int_span(body_s, body_l, "max_key_len", &raw_key_len)) {
        disk_policy.max_key_len = (raw_key_len > 0) ? (size_t)raw_key_len : 0U;
    }
    int raw_tok_len = 0;
    if (json_get_int_span(body_s, body_l, "max_token_len", &raw_tok_len)) {
        disk_policy.max_token_len = (raw_tok_len > 0) ? (size_t)raw_tok_len : 0U;
    }

    if (!json_policy_is_valid(&disk_policy)) {
        char log_msg[IO_FB_PATH_MAX + 96U];
        snprintf(log_msg, sizeof(log_msg),
                 "Invalid policy bounds in '%s'; restored ROM Safe-Mode policy",
                 target);
        json_downgrade_policy_to_rom("json_utils_load_policy",
                                     "Disk JSON policy failed ladder/depth bounds; restored ROM",
                                     log_msg,
                                     "Disk JSON policy violated parser bounds");
        return false;
    }

    /* Step 6: Commit validated disk policy to BIOS contract and local chassis. */
    parsed_contract.loaded_from_data = true;
    (void)io_fallback_set_loader_contract(&parsed_contract);
    str_safe_copy(disk_policy.source_tag, "disk_loader_contracts",
                  sizeof(disk_policy.source_tag));
    disk_policy.loaded_from_data = true;
    s_json_state.policy = disk_policy;
    s_json_state.policy_from_data = true;
    s_json_state.policy_armed = true;

    JSON_EMIT_LOG(LOG_INFO, 0.25f,
                  "Hydrated JSON policy from '%s' (max_depth=%d, max_tokens=%zu, strict_utf8=%s)",
                  target,
                  s_json_state.policy.max_json_depth,
                  s_json_state.policy.max_json_tokens,
                  s_json_state.policy.strict_utf8_escapes ? "true" : "false");
    return true;
}

/**
 * @brief Snapshots the currently active `json_policy_t` into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets diagnostics and tests inspect the active depth, token, and escape limits currently
 * enforced by the JSON parser.
 *
 * ### For Systems Programmers & Engine Architects:
 * Arms default Safe-Mode policy lazily if uninitialized and performs an O(1) copy of
 * `s_json_state.policy`.
 * @triggers None.
 *
 * @param[out] out_policy Caller struct receiving active policy descriptor.
 * @return true on non-NULL `out_policy`; false when `out_policy` is NULL.
 */
bool json_utils_get_policy(json_policy_t *out_policy)
{
    /* Step 1: Reject NULL output descriptor silently and arm policy lazily before copy. */
    if (out_policy == NULL) {
        return false;
    }
    json_ensure_policy_armed();
    *out_policy = s_json_state.policy;
    return true;
}

/**
 * @brief Validates and commits an explicit `json_policy_t` contract into runtime state.
 *
 * ### For Game Designers & Non-Programmers:
 * Applies programmatic parser safety limits while automatically restoring safe built-in
 * ROM defaults if out-of-range limits are supplied.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks `json_utils_validate_policy_contract(policy)`. On breach, downgrades cleanly
 * via `json_downgrade_policy_to_rom()` (`IO_ERR_JSON_SYNTAX` -> `io.json.policy_bounds`).
 * Otherwise installs `*policy` and synchronizes `policy_from_data`.
 * @triggers `io.json.policy_bounds`, `LOG_CAT_IO` on invalid contract rejection.
 *
 * @param[in] policy Candidate `json_policy_t` descriptor to enforce.
 * @return true if valid and committed; false on contract breach (downgrades to ROM).
 */
bool json_utils_set_policy(const json_policy_t *policy)
{
    /* Step 1: Arm baseline policy and reject invalid candidate contracts via Safe-Mode ROM. */
    json_ensure_policy_armed();
    if (!json_utils_validate_policy_contract(policy)) {
        json_downgrade_policy_to_rom("json_utils_set_policy",
                                     "Rejected out-of-bounds json_policy_t; restored ROM Safe Mode",
                                     "Rejected invalid json_policy_t; downgraded cleanly to Safe-Mode ROM",
                                     "Rejected corrupted json_policy_t contract");
        return false;
    }

    /* Step 2: Commit validated runtime parser policy and provenance flag. */
    s_json_state.policy = *policy;
    s_json_state.policy_from_data = policy->loaded_from_data;
    s_json_state.policy_armed = true;
    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / CONTRACT VALIDATION & PROBES
Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
================================================================================
Mission: Verify structural JSON payloads, enforce active policy contracts, register
`io.json.*` probes into `debug.c`, and run non-destructive pre-flight diagnostics.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Structural Validation, Policy Contracts & Debugger Probes
Mission: Validate arbitrary JSON slices with precise byte offset/code diagnostics
         and connect `TW_ENG_JSON_SYNTAX` / `TW_ENG_JSON_DEPTH_EXCEEDED` probes.

  ### For Game Designers & Non-Programmers:
  Catches broken quotes, bad nesting depth, and corrupted JSON parser rules right
  when they happen and pauses the debugger safely instead of loading garbage data.

  ### For Systems Programmers & Engine Architects:
  Registers `io.json.syntax`, `io.json.depth_bounds`, and `io.json.policy_bounds`
  idempotently into `debug.c` (`DEBUG_STEP_AWAIT` armed when `debug_is_active()`).
  @triggers `io.json.syntax`, `io.json.depth_bounds`, `io.json.policy_bounds`.
--------------------------------------------------------------------------------
*/

bool json_utils_validate_policy_contract(const json_policy_t *policy)
{
    if (!json_policy_is_valid(policy)) {
        return false;
    }
    if (policy->max_json_depth <= 0 || policy->max_json_depth > IO_FB_MAX_JSON_DEPTH) {
        return false;
    }
    return true;
}

bool json_validate_span(const char *json_start,
                        size_t json_len,
                        json_validation_report_t *out_report)
{
    json_ensure_policy_armed();
    s_json_state.token_inspections++;

    size_t effective_len = 0U;
    if (json_start != NULL) {
        effective_len = (json_len > 0U) ? json_len : strlen(json_start);
    }
    return json_validate_root_span_internal(json_start, effective_len,
                                            &s_json_state.policy, out_report);
}

bool json_utils_enforce_valid_span(const char *json_start,
                                   size_t json_len,
                                   const char *context_label,
                                   json_validation_report_t *out_report)
{
    json_validation_report_t rep;
    bool ok = json_validate_span(json_start, json_len, &rep);
    if (out_report != NULL) {
        *out_report = rep;
    }
    if (ok) {
        return true;
    }

    const char *ctx = (context_label != NULL && context_label[0] != '\0')
                          ? context_label
                          : "json_span";
    uint16_t err_code = (rep.error_code == IO_ERR_JSON_DEPTH_EXCEEDED)
                            ? IO_ERR_JSON_DEPTH_EXCEEDED
                            : IO_ERR_JSON_SYNTAX;
    if (err_code == IO_ERR_JSON_DEPTH_EXCEEDED) {
        s_json_state.depth_overflows++;
    } else {
        s_json_state.syntax_faults++;
    }

    char msg[88];
    snprintf(msg, sizeof(msg), "%.20s @%zu: %.52s",
             ctx, rep.error_offset, rep.diagnostic);
    io_fallback_record_fault("json_utils", "json_enforce_valid", msg, err_code);
    JSON_EMIT_LOG(LOG_ERROR, -0.75f, "JSON validation breach [0x%04X] %s",
                  (unsigned int)err_code, msg);
    (void)json_utils_trigger_probe(msg, err_code);
    return false;
}

static diag_result_t json_probe_syntax_health(void)
{
    json_ensure_policy_armed();
    diag_result_t res;
    res.subsystem = JSON_UTILS_MODULE_ID;
    res.check_name = "JSON Zero-Heap Syntax & Quote Parity";

    size_t rom_sz = 0U;
    const char *rom = io_fallback_find_rom_asset("io/loader_contracts.json", &rom_sz);
    bool ok_valid = json_validate_root_span_internal(
        s_json_probe_ok_sample, strlen(s_json_probe_ok_sample), &s_json_state.policy, NULL);
    bool bad_rejected = !json_validate_root_span_internal(
        s_json_probe_bad_sample, strlen(s_json_probe_bad_sample), &s_json_state.policy, NULL);

    res.passed = json_utils_validate_policy_contract(&s_json_state.policy) &&
                 (rom != NULL) && (rom_sz > 0U) && ok_valid && bad_rejected;
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "JSON zero-heap tokenizer, escape parity, and ROM manifest verified."
        : "JSON syntax engine or ROM parity compromised -> DEBUG_STEP_AWAIT.";
    return res;
}

static diag_result_t json_probe_depth_health(void)
{
    json_ensure_policy_armed();
    diag_result_t res;
    res.subsystem = JSON_UTILS_MODULE_ID;
    res.check_name = "JSON Nesting Depth Ceiling Guard";

    io_fb_loader_contract_t fb_contract;
    bool got_fb = io_fallback_get_loader_contract(&fb_contract);
    bool policy_depth_ok = (s_json_state.policy.max_json_depth > 0) &&
                           (s_json_state.policy.max_json_depth <= JSON_MAX_NESTING) &&
                           (s_json_state.policy.max_json_tokens >= 64U);
    bool fb_depth_ok = got_fb &&
                       (fb_contract.max_json_depth > 0) &&
                       (fb_contract.max_json_depth <= JSON_MAX_NESTING);
    bool depth_guard_enforced = false;
    if (policy_depth_ok) {
        size_t target_d = (size_t)s_json_state.policy.max_json_depth + 1U;
        char depth_probe[(JSON_MAX_NESTING + 2) * 2];
        size_t total_len = target_d * 2U;
        if (total_len < sizeof(depth_probe)) {
            for (size_t i = 0U; i < target_d; i++) {
                depth_probe[i] = '[';
                depth_probe[total_len - 1U - i] = ']';
            }
            depth_probe[total_len] = '\0';
            json_validation_report_t drep;
            bool accepted = json_validate_root_span_internal(
                depth_probe, total_len, &s_json_state.policy, &drep);
            depth_guard_enforced = (!accepted && drep.error_code == IO_ERR_JSON_DEPTH_EXCEEDED);
        }
    }

    res.passed = policy_depth_ok && fb_depth_ok && depth_guard_enforced;
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "JSON nesting depth ceiling actively enforced within max_json_depth bounds."
        : "JSON depth ceiling or Safe-Mode BIOS contract diverged.";
    return res;
}

static diag_result_t json_probe_policy_health(void)
{
    json_ensure_policy_armed();
    diag_result_t res;
    res.subsystem = JSON_UTILS_MODULE_ID;
    res.check_name = "JSON Runtime Policy Contract Bounds";
    res.passed = json_utils_validate_policy_contract(&s_json_state.policy);
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_FATAL;
    res.details = res.passed
        ? "Active json_policy_t depth/token/payload bounds verified."
        : "Active json_policy_t corrupted; downgraded to Safe-Mode ROM policy.";
    return res;
}

void json_utils_register_debug_probes(void)
{
    for (size_t i = 0U; i < 3U; i++) {
        (void)debug_register_validation_probe(&s_json_probes[i]);
    }
}

bool json_utils_trigger_probe(const char *context_label, uint16_t error_code)
{
    json_ensure_policy_armed();
    if (s_json_state.in_preflight) {
        return false;
    }

    const char *probe_id = "io.json.syntax";
    bool policy_corrupt = !json_utils_validate_policy_contract(&s_json_state.policy);
    if (error_code == IO_ERR_JSON_DEPTH_EXCEEDED) {
        probe_id = "io.json.depth_bounds";
    } else if (policy_corrupt) {
        probe_id = "io.json.policy_bounds";
        json_sync_policy_from_fallback(&s_json_state.policy);
        s_json_state.policy_from_data = false;
    }

    if (!debug_is_active()) {
        if (policy_corrupt) {
            (void)io_fallback_downgrade_safe_mode(
                (context_label != NULL) ? context_label : "json_utils policy contract downgraded",
                false
            );
        }
        return false;
    }

    json_utils_register_debug_probes();
    (void)io_fallback_downgrade_safe_mode(
        (context_label != NULL) ? context_label : "json_utils tripwire escalation",
        false
    );

    if (!s_json_state.policy.probe_await_on_corrupt) {
        return false;
    }

    return debug_trigger_probe(
        probe_id,
        MODULE_ID,
        (context_label != NULL) ? context_label : "JSON structural/policy anomaly",
        __FILE__,
        __LINE__,
        "json_utils_trigger_probe"
    );
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Non-destructive verification of scalar getters, escape/backslash parity,
spoof-key resistance, bool boundaries, depth bounds, and structural validation.

  ### For Game Designers & Non-Programmers:
  Runs automatically during `./build/tenebris-lux --audit` to prove JSON reading
  handles tricky characters, nesting limits, and broken syntax without crashing.

  ### For Systems Programmers & Engine Architects:
  Saves `s_json_state`, runs checks with `in_preflight = true` (zero log spam),
  and restores caller counters/policy on every return path.
--------------------------------------------------------------------------------
*/

diag_result_t json_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = json_utils_manifest.module_id;
    res.check_name = "Zero-Heap JSON Parsing, Escapes & Depth Bounds";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "JSON utilities: span bounds, escape parity, and depth guard verified.";

    json_utils_runtime_state_t backup = s_json_state;
    json_ensure_policy_armed();
    s_json_state.in_preflight = true;

    /* Check 1: NULL pointer & empty-buffer resilience */
    char scratch[64];
    int iv = 0;
    size_t szv = 0U;
    float fv = 0.0f;
    bool bv = false;
    if (json_find_key(NULL, "key") != NULL ||
        json_find_key("{\"k\":1}", NULL) != NULL ||
        json_get_string(NULL, "key", scratch, sizeof(scratch)) != false ||
        json_get_string("{\"key\":\"v\"}", "key", scratch, 0U) != false ||
        json_get_int("{\"k\":1}", "k", NULL) != false ||
        json_get_size_t("{\"k\":1}", "k", NULL) != false ||
        json_get_bool("{\"k\":true}", "k", NULL) != false ||
        json_get_float("{\"k\":1.0}", "k", NULL) != false ||
        json_inspect_token(NULL, NULL, NULL) != false ||
        json_inspect_array(NULL, NULL, NULL) != false ||
        json_execute_extract_string(NULL, NULL, NULL, 0U, NULL) != false) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "NULL / zero-capacity guard failed pre-flight.";
        return res;
    }

    /* Check 2: Core typed extraction, quote-spoof immunity, trailing backslash & bool boundary */
    const char *test_json =
        "{"
        "\"spoof\": \"\\\"name\\\": \\\"Evil\\\"\", "
        "\"path\": \"C:\\\\\", "
        "\"name\": \"Elijah Ro-enheart\", "
        "\"moral_sway\": -45, "
        "\"bad_bool\": truefish, "
        "\"active\": true, "
        "\"hp_ratio\": 0.75, "
        "\"tags\": [\"lux\", \"ten\\\"ebris\", \"sacyra\\\\\"], "
        "\"nested\": {\"inner\": 42, \"extra\": \"ok\"}, "
        "\"sibling\": 999"
        "}";

    if (!json_get_string(test_json, "name", scratch, sizeof(scratch)) ||
        strcmp(scratch, "Elijah Ro-enheart") != 0) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "String lookup failed or matched spoofed escaped quote key.";
        return res;
    }

    if (!json_get_string(test_json, "path", scratch, sizeof(scratch)) ||
        strcmp(scratch, "C:\\") != 0) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Escaped trailing backslash parsing failed.";
        return res;
    }

    if (!json_get_int(test_json, "moral_sway", &iv) || iv != -45 ||
        json_get_size_t(test_json, "moral_sway", &szv) != false ||
        json_get_bool(test_json, "bad_bool", &bv) != false ||
        !json_get_bool(test_json, "active", &bv) || !bv ||
        !json_get_float(test_json, "hp_ratio", &fv) || fv < 0.74f || fv > 0.76f) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Typed scalar bounds/delimiter checks failed.";
        return res;
    }

    /* Check 3: Object slice isolation & array count/elements */
    const char *obj_s = NULL;
    size_t obj_l = 0U;
    int sib_check = 0;
    if (!json_find_object(test_json, "nested", &obj_s, &obj_l) ||
        json_get_int(obj_s, "sibling", &sib_check) != false ||
        !json_get_object_int(test_json, "nested", "inner", &iv) || iv != 42) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Object slice boundary isolation failed.";
        return res;
    }

    const char *arr_s = NULL;
    size_t arr_l = 0U;
    if (!json_find_array(test_json, "tags", &arr_s, &arr_l) ||
        json_array_get_count(arr_s, arr_l) != 3U ||
        !json_array_get_string(arr_s, arr_l, 2U, scratch, sizeof(scratch)) ||
        strcmp(scratch, "sacyra\\") != 0) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Array backslash-escaped element extraction failed.";
        return res;
    }

    /* Check 4: Structural validator & depth overflow enforcement */
    const char *strict_valid_json =
        "{\"id\": \"tenlux.audit\", \"nested\": {\"arr\": [1, true, false, null]}}";
    char deep_json[96];
    for (int d = 0; d < 35; d++) {
        deep_json[d] = '[';
        deep_json[70 - 1 - d] = ']';
    }
    deep_json[70] = '\0';

    json_validation_report_t rep;
    if (!json_validate_span(strict_valid_json, 0U, &rep) || !rep.valid ||
        json_validate_span(test_json, 0U, &rep) != false ||
        rep.error_code != IO_ERR_JSON_SYNTAX ||
        json_validate_span(deep_json, 70U, &rep) != false ||
        rep.error_code != IO_ERR_JSON_DEPTH_EXCEEDED) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Structural validator failed depth overflow / syntax contract.";
        return res;
    }

    /* Check 5: Token inspection & execution card invariants */
    json_token_view_t tok_view;
    json_array_view_t arr_view;
    json_extract_result_t ext_res;
    if (!json_inspect_token(test_json, "moral_sway", &tok_view) ||
        !tok_view.exists || tok_view.type != JSON_TYPE_NUMBER ||
        !json_inspect_array(test_json, "tags", &arr_view) ||
        !arr_view.exists || arr_view.count != 3U ||
        !json_execute_extract_string(test_json, "name", scratch, sizeof(scratch), &ext_res) ||
        !ext_res.success) {
        s_json_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Block 5 token inspection / extraction procedure failed.";
        return res;
    }

    s_json_state = backup;
    return res;
}

bool json_utils_audit(void)
{
    diag_result_t res = json_utils_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Machine: Zero-Heap JSON Engine
================================================================================
Mission: Construct token/array inspection views, format diagnostic ASCII cards,
report `.bss` parser telemetry, and manage subsystem boot/teardown lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Classify value tokens via canonical Block 2 primitives and format concise
inspection views & telemetry counters with zero heap allocations.

  @triggers `json_inspect_token()` raises `IO_ERR_JSON_SYNTAX` -> `io.json.syntax`
  on unterminated quotes/containers or `IO_ERR_JSON_DEPTH_EXCEEDED` -> `io.json.depth_bounds`.
--------------------------------------------------------------------------------
*/

bool json_inspect_token(const char *json, const char *key, json_token_view_t *out_view)
{
    if (out_view == NULL) {
        return false;
    }
    memset(out_view, 0, sizeof(*out_view));
    out_view->type = JSON_TYPE_UNKNOWN;

    if (json == NULL || key == NULL || key[0] == '\0') {
        return false;
    }

    json_ensure_policy_armed();
    s_json_state.token_inspections++;

    const char *val = json_find_key_span_internal(json, 0U, key);
    if (val == NULL) {
        return false;
    }

    out_view->exists = true;
    out_view->raw_start = val;

    char c = *val;
    if (c == '{' || c == '[') {
        out_view->type = (c == '{') ? JSON_TYPE_OBJECT : JSON_TYPE_ARRAY;
        size_t span_len = 0U;
        bool depth_exceeded = false;
        bool syntax_err = false;
        if (!json_find_delimited_span_internal(val, NULL, c, (c == '{') ? '}' : ']',
                                               &span_len, NULL,
                                               &depth_exceeded, &syntax_err)) {
            if (depth_exceeded) {
                json_record_depth_overflow("json_inspect_token", (size_t)(val - json));
            } else if (syntax_err) {
                s_json_state.syntax_faults++;
                io_fallback_record_fault("json_utils", "json_inspect_token",
                                         "Unclosed or mismatched container in token inspection",
                                         IO_ERR_JSON_SYNTAX);
                JSON_EMIT_LOG(LOG_ERROR, -0.75f,
                              "Unclosed or mismatched container for key '%s' in token inspection",
                              key);
                (void)json_utils_trigger_probe("Unclosed JSON container in json_inspect_token",
                                               IO_ERR_JSON_SYNTAX);
            }
            return false;
        }
        out_view->raw_len = span_len;
        return true;
    }

    if (c == '"') {
        out_view->type = JSON_TYPE_STRING;
        bool esc_err = false;
        const char *close_q = json_skip_string_span(
            val, NULL, s_json_state.policy.strict_utf8_escapes, &esc_err);
        if (close_q == NULL) {
            s_json_state.syntax_faults++;
            io_fallback_record_fault("json_utils", "json_inspect_token",
                                     "Unterminated string literal in token inspection",
                                     IO_ERR_JSON_SYNTAX);
            JSON_EMIT_LOG(LOG_ERROR, -0.75f,
                          "Unterminated string literal for key '%s' in token inspection", key);
            (void)json_utils_trigger_probe("Unterminated string literal in json_inspect_token",
                                           IO_ERR_JSON_SYNTAX);
            return false;
        }
        out_view->raw_len = (size_t)(close_q - val + 1);
        return true;
    }

    if (strncmp(val, "true", 4) == 0 && json_is_value_delimiter(val + 4, NULL)) {
        out_view->type = JSON_TYPE_BOOLEAN;
        out_view->raw_len = 4U;
        return true;
    }
    if (strncmp(val, "false", 5) == 0 && json_is_value_delimiter(val + 5, NULL)) {
        out_view->type = JSON_TYPE_BOOLEAN;
        out_view->raw_len = 5U;
        return true;
    }
    if (strncmp(val, "null", 4) == 0 && json_is_value_delimiter(val + 4, NULL)) {
        out_view->type = JSON_TYPE_NULL;
        out_view->raw_len = 4U;
        return true;
    }
    if (isdigit((unsigned char)c) != 0 || c == '-' || c == '+') {
        out_view->type = JSON_TYPE_NUMBER;
        const char *p = val;
        while (*p != '\0' && json_is_number_char(*p)) {
            p++;
        }
        if (!json_is_value_delimiter(p, NULL)) {
            s_json_state.syntax_faults++;
            out_view->type = JSON_TYPE_UNKNOWN;
            return false;
        }
        out_view->raw_len = (size_t)(p - val);
        return true;
    }

    s_json_state.syntax_faults++;
    return false;
}

bool json_inspect_array(const char *json, const char *array_key, json_array_view_t *out_view)
{
    if (out_view == NULL) {
        return false;
    }
    memset(out_view, 0, sizeof(*out_view));

    if (json == NULL || array_key == NULL) {
        return false;
    }

    s_json_state.token_inspections++;
    const char *arr_start = NULL;
    size_t arr_len = 0U;
    if (!json_find_array(json, array_key, &arr_start, &arr_len)) {
        return false;
    }

    out_view->exists = true;
    out_view->arr_start = arr_start;
    out_view->arr_len = arr_len;
    out_view->count = json_array_get_count(arr_start, arr_len);
    return true;
}

size_t json_execute_dump_card(const json_token_view_t *view, char *out_buf, size_t buf_size)
{
    if (view == NULL || out_buf == NULL || buf_size == 0U) {
        return 0U;
    }
    const char *type_name = "UNKNOWN";
    switch (view->type) {
        case JSON_TYPE_OBJECT:  type_name = "OBJECT"; break;
        case JSON_TYPE_ARRAY:   type_name = "ARRAY"; break;
        case JSON_TYPE_STRING:  type_name = "STRING"; break;
        case JSON_TYPE_NUMBER:  type_name = "NUMBER"; break;
        case JSON_TYPE_BOOLEAN: type_name = "BOOLEAN"; break;
        case JSON_TYPE_NULL:    type_name = "NULL"; break;
        default:                type_name = "UNKNOWN"; break;
    }
    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "                JSON TOKEN INSPECTION CARD                  \n"
        "============================================================\n"
        "Exists:        %s\n"
        "Token Type:    %s\n"
        "Payload Bytes: %zu\n"
        "Payload Slice: %s\n"
        "============================================================\n",
        view->exists ? "YES" : "NO",
        type_name,
        view->raw_len,
        (view->exists && view->raw_start != NULL) ? "VALID RAW POINTER" : "NULL / UNLOCATED"
    );
    if (written < 0 || (size_t)written >= buf_size) {
        out_buf[buf_size - 1U] = '\0';
        return 0U;
    }
    return (size_t)written;
}

bool json_execute_extract_string(const char *json, const char *key,
                                 char *buf, size_t buf_size,
                                 json_extract_result_t *out_result)
{
    if (out_result != NULL) {
        memset(out_result, 0, sizeof(*out_result));
        if (key != NULL) {
            str_safe_copy(out_result->key, key, sizeof(out_result->key));
        }
    }

    if (json == NULL || key == NULL || buf == NULL || buf_size == 0U) {
        if (buf != NULL && buf_size > 0U) {
            buf[0] = '\0';
        }
        return false;
    }

    bool ok = json_get_string(json, key, buf, buf_size);
    if (out_result != NULL) {
        out_result->success = ok;
        out_result->type = ok ? JSON_TYPE_STRING : JSON_TYPE_UNKNOWN;
        out_result->value_len = ok ? strlen(buf) : 0U;
    }

    return ok;
}

bool json_utils_get_stats(json_utils_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }
    json_ensure_policy_armed();
    out_stats->initialized = s_json_state.initialized;
    out_stats->policy_from_data = s_json_state.policy_from_data;
    out_stats->key_queries = s_json_state.key_queries;
    out_stats->extractions_ok = s_json_state.extractions_ok;
    out_stats->depth_overflows = s_json_state.depth_overflows;
    out_stats->syntax_faults = s_json_state.syntax_faults;
    out_stats->type_mismatches = s_json_state.type_mismatches;
    out_stats->truncated_strings = s_json_state.truncated_strings;
    out_stats->token_inspections = s_json_state.token_inspections;
    out_stats->active_policy = s_json_state.policy;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Arm Tier-0 BIOS fallback, hydrate `loader_contracts.json` policy from
disk or `.rodata` ROM vault, register `io.json.*` probes, and log lifecycle state.

  ### For Game Designers & Non-Programmers:
  Boots the JSON config reader reliably at startup and logs parsing health stats on exit.

  ### For Systems Programmers & Engine Architects:
  Idempotent initialization sequencing: `io_fallback_boot()` -> `json_utils_load_policy(NULL)`
  -> `json_utils_register_debug_probes()` -> `LOG_INFO(LOG_CAT_IO)`.
--------------------------------------------------------------------------------
*/

void json_utils_init(void)
{
    if (!io_fallback_is_booted()) {
        (void)io_fallback_boot(NULL);
    }
    (void)json_utils_load_policy(NULL);
    json_utils_register_debug_probes();
    s_json_state.initialized = true;

    JSON_EMIT_LOG(LOG_INFO, 0.25f,
                  "json_utils initialized (source='%s', from_data=%s, max_depth=%d)",
                  s_json_state.policy.source_tag,
                  s_json_state.policy_from_data ? "true" : "false",
                  s_json_state.policy.max_json_depth);
}

void json_utils_shutdown(void)
{
    JSON_EMIT_LOG(LOG_INFO, 0.10f,
                  "json_utils shutdown (queries=%llu, ok=%llu, depth_ovf=%llu, syntax_err=%llu)",
                  (unsigned long long)s_json_state.key_queries,
                  (unsigned long long)s_json_state.extractions_ok,
                  (unsigned long long)s_json_state.depth_overflows,
                  (unsigned long long)s_json_state.syntax_faults);
    s_json_state.initialized = false;
}

/*
================================================================================
End of json_utils.c
================================================================================
*/
