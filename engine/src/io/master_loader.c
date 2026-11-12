/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/src/io/master_loader.c
@id            cog.master_loader
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master Data & Manifest Loader (Universal 5-Block JSON Envelopes)
@role          Runtime Engine & Chassis: Envelope slicer, contract verifier,
               ROM Safe-Mode fallback hydrator, and transient-stack preflight engine.
@maturity      Rev 5 Baseline | 64 KiB static .bss bloat purged; transient stack & io.loader.* probes active.
@chassis       zero-heap | .bss 112 B (s_loader_state) + .rodata 5,916 B + transient preflight stack frame.
@scale         O(N) envelope verification + single-pass block slicing across 64 KiB ceiling; zero .bss staging bloat.
@seams         in: loader_contracts.json, rom://io | uses: cog.path_finder, cog.fs_utils, cog.json_utils, cog.log, cog.debug | probes: io.loader.* -> DEBUG_STEP_AWAIT | dag: core.io.master_loader
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Runs json_validate_span() before slicing block_1_metadata, block_2_opening/types, and block_3_body.
               3. Zero permanent Block 4 .bss staging buffers; preflight/inspect execute cleanly on transient stack frames.
               4. Contract load or validation faults trigger Safe-Mode ROM downgrade and latch io.loader.* probes.

PURPOSE & ARCHITECTURAL ROLE:
  Executes the unified 5-block JSON loading pipeline: resolves VFS/ROM paths,
  verifies syntax and envelope schemas against loader_contracts.json, isolates
  block_3_body spans from metadata noise, and dispatches diagnostic probes.

  ### For Game Designers & Non-Programmers:
  Stops corrupt quotes, stale schemas, or missing data sections right at the
  door while logging actionable clues—preventing partial config bugs from
  silently altering combat math or progression state.

  ### For Systems Programmers & Engine Architects:
  - Controlled Memory Chassis: Eliminated legacy 64 KiB .bss Block 4 arrays so
    runtime footprint sits at just 112 B .bss (s_loader_state) + 5,916 B .rodata.
  - Bottleneck & Headroom: Pre-computed body_start/body_len pointers confine all
    field getters to pure block_3_body span scans cleanly without re-walking headers.
  - Probe & Config Wiring: Hydrates active rules from loader_contracts.json (disk
    or Safe-Mode ROM) and wires registered `io.loader.*` probes into DEBUG_STEP_AWAIT.
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
Module:        cog.master_loader
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
#include <stdio.h>
#include <string.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "io/master_loader.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Private Constants & Logging Anchors
--------------------------------------------------------------------------------
Fixed logger labels and compile-time contract bounds cleanly in `.rodata`.
--------------------------------------------------------------------------------
*/

/** Canonical module identifier bound in manifests, telemetry, and probes. */
#define MODULE_ID        MASTER_LOADER_MODULE_ID
/** Structured telemetry category for all MasterLoader diagnostics. */
#define LOCAL_LOG_CAT    LOG_CAT_IO
/** Concise logger source tag matching sibling IO modules. */
#define LOCAL_MODULE_TAG "MasterLoader"

static_assert(
    MASTER_LOADER_MAX_JSON_DEPTH >= 8 && MASTER_LOADER_MAX_PAYLOAD_BYTES >= 4096U,
    "MasterLoader nesting depth and config payload caps must remain operational"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Probe Forward Declarations
--------------------------------------------------------------------------------
Consolidated runtime `.bss` layout and Block 4 diagnostic forward declarations.
--------------------------------------------------------------------------------
*/

/**
 * @struct master_loader_runtime_state
 * @brief Private singleton state in `.bss` (`ALLOC_ZERO_HEAP`, zero heap).
 *
 * ### For Game Designers & Non-Programmers:
 * Tracks how many 5-block config files loaded cleanly from disk or built-in
 * ROM backups and counts formatting or version mismatches.
 *
 * ### For Systems Programmers & Engine Architects:
 * Slim `.bss` chassis (< 256 B) housing active ingestion contract bounds,
 * recursion latches (`in_preflight`, `in_log_emit`, `in_load_contract`), and metrics.
 */
typedef struct master_loader_runtime_state {
    bool                     initialized;                 /**< True during active lifecycle. */
    bool                     contract_armed;              /**< True once baseline ROM/disk contract seeded. */
    bool                     contract_from_data;          /**< True ONLY when disk JSON parsed cleanly. */
    bool                     probes_registered;           /**< True after `io.loader.*` probes registered. */
    bool                     in_preflight;                /**< Suppresses probe await & log spam during audit. */
    bool                     in_log_emit;                 /**< Breaks `LOG_CAT_IO` recursion loops cleanly. */
    bool                     in_load_contract;            /**< Guards recursive contract hydration. */
    master_loader_contract_t contract;                    /**< Active verification contract bounds/toggles. */
    uint32_t                 total_loads;                 /**< Lifetime ingestion attempts cleanly tracked. */
    uint32_t                 disk_loads;                  /**< Disk-backed ingestion successes. */
    uint32_t                 rom_loads;                   /**< `.rodata` ROM virtual URI ingestion successes. */
    uint32_t                 memory_loads;                /**< Direct caller memory-span ingestion parses. */
    uint32_t                 body_queries;                /**< Scoped `block_3_body` extraction queries. */
    uint32_t                 syntax_errors;               /**< Rejected syntax/container violations (`0x0204`). */
    uint32_t                 schema_mismatches;           /**< Explicit schema mismatch count (`0x0203`). */
    uint32_t                 version_mismatches;          /**< Semver prefix mismatch count (`0x0207`). */
    uint32_t                 body_missing_errors;         /**< Envelopes lacking `block_3_body` (`0x0206`). */
    uint32_t                 not_found_errors;            /**< Missing disk/ROM file path misses (`0x0201`). */
    uint32_t                 overflow_errors;             /**< Buffer or payload size breaches (`0x0202`). */
    uint32_t                 corrupt_contract_downgrades; /**< Corrupt contract downgrades to Safe-Mode ROM. */
} master_loader_runtime_state_t;

/* Forward declarations binding Sub-Assembly 2.3 manifest and Block 4 probes. */
diag_result_t master_loader_preflight(void);
static diag_result_t loader_probe_contract_bounds(void);
static diag_result_t loader_probe_envelope_integrity(void);
static diag_result_t loader_probe_rom_parity(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State, Manifest & Debugger Probes (.bss & .rodata)
--------------------------------------------------------------------------------
Controlled Memory Chassis: slim `.bss` singleton (< 256 B), manifest node,
probe payload samples, and compiled `.rodata` validation probe descriptors.
--------------------------------------------------------------------------------
*/

/** @brief Immutable architectural invariants exported in `master_loader_manifest` (.rodata). */
static const char *const s_master_loader_invariants[4] = {
    "ALLOC_ZERO_HEAP enforced on all 5-block parsing and body lookups",
    "block_3_body spans strictly bounded preventing Block 1/2/4/5 key bleed",
    "loader_contracts.json hydrated from VFS with .rodata ROM Safe-Mode parity",
    "Corrupt contracts or syntax faults escalate io.loader.* probes -> DEBUG_STEP_AWAIT"
};

/** @brief Public module metadata node registering cog.master_loader in manifest. */
const module_metadata_t master_loader_manifest = {
    MASTER_LOADER_MODULE_ID,
    "master_loader.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-11",
    "Zero-heap universal 5-block JSON envelope ingestion and contract orchestrator",
    3,
    { "core.io.path_finder", "core.util.json", "core.util.fs" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    500,
    4,
    {
        s_master_loader_invariants[0],
        s_master_loader_invariants[1],
        s_master_loader_invariants[2],
        s_master_loader_invariants[3]
    },
    master_loader_preflight
};

/** Private `.bss` singleton (< 256 bytes). */
static master_loader_runtime_state_t s_loader_state;

/** Immutable `.rodata` health sample payloads verified by `io.loader.envelope_integrity`. */
static const char *const s_probe_valid_doc =
    "{"
    "  \"block_1_metadata\": {"
    "    \"project\": \"Tenebris and Lux\","
    "    \"version\": \"0.1.1-dev\""
    "  },"
    "  \"block_2_opening\": {"
    "    \"static_declarations\": {"
    "      \"schema_id\": \"tenlux.probe.master_loader.v1\""
    "    }"
    "  },"
    "  \"block_3_body\": {"
    "    \"probe_key\": \"ok\","
    "    \"bleed_check\": 42"
    "  },"
    "  \"block_4_audit\": {"
    "    \"bleed_check\": 999,"
    "    \"outside_only\": \"isolated\""
    "  }"
    "}";

/** Immutable `.rodata` malformed envelope sample verifying probe failure detection. */
static const char *const s_probe_corrupt_doc =
    "{"
    "  \"block_1_metadata\": {\"project\": \"Tenebris and Lux\"},"
    "  \"block_3_body\": {\"broken\": \"unclosed}"
    "}";

/** Immutable `.rodata` debugger probe descriptors registered by `master_loader_register_debug_probes()`. */
static const debug_validation_probe_t s_loader_probes[3] = {
    {
        "io.loader.contract_bounds",
        MASTER_LOADER_MODULE_ID,
        "master_loader.c:Block3:master_loader_set_contract",
        "tests/unit/io/test_master_loader_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_FALLBACK_MODE,
        loader_probe_contract_bounds,
        true,
        true,
        0U
    },
    {
        "io.loader.envelope_integrity",
        MASTER_LOADER_MODULE_ID,
        "master_loader.c:Block3:master_loader_load_from_memory",
        "tests/unit/io/test_master_loader_unit.c:Section2",
        DEBUG_TEST_TIER_UNIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_FALLBACK_MODE,
        loader_probe_envelope_integrity,
        true,
        true,
        0U
    },
    {
        "io.loader.rom_parity",
        MASTER_LOADER_MODULE_ID,
        "master_loader.c:Block3:master_loader_load_contract",
        "tests/unit/io/test_master_loader_unit.c:Section4",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_AUDIT_BREACH,
        DEBUG_REACTION_FALLBACK_MODE,
        loader_probe_rom_parity,
        true,
        true,
        0U
    }
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Canonical Pre-Conditional Log Frontier & Inline Predicates
--------------------------------------------------------------------------------
Reentrancy-safe `LOG_CAT_IO` wrapper and O(1) span readiness predicates.
--------------------------------------------------------------------------------
*/

/** @brief Reentrancy-guarded `LOG_CAT_IO` telemetry emitter skipping audit sweeps. */
#define LOADER_EMIT_LOG(sev_macro, grade, ...)                                 \
    do {                                                                       \
        if (!s_loader_state.in_log_emit && !s_loader_state.in_preflight &&     \
            log_is_active()) {                                                 \
            s_loader_state.in_log_emit = true;                                 \
            (void)sev_macro(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, (grade),          \
                            __VA_ARGS__);                                      \
            s_loader_state.in_log_emit = false;                                \
        }                                                                      \
    } while (0)

/** @brief Returns true in O(1) time if an envelope pointer and span length are non-empty. */
CS_NODISCARD static inline bool loader_span_nonempty(const char *span, size_t len)
{
    return (span != NULL && len > 0U);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.5: Monitored Support Infrastructure (Non-Inline Internal)
--------------------------------------------------------------------------------
Support Role:  ROM contract seeding, fault telemetry bridging, semver prefix checks,
               envelope Block 1/2/3/4 extraction, and isolated `block_3_body` scoping.
Pattern Watch: `master_loader_parse_envelope_internal()` extracts 5-block header and
               body spans via `cog.json_utils`; if future mod-pack or save-state
               loaders add multi-version schema migration passes, split those
               migration tables into a dedicated `schema_migrator.c` orchestrator.
Linkage Rule:  Strictly static functions only; zero static arrays or .bss storage.
--------------------------------------------------------------------------------
*/

/** Single-source helper seeding `out_contract` from `.rodata` ROM (or BIOS defaults). */
static bool loader_build_rom_contract(master_loader_contract_t *out_contract)
{
    if (out_contract == NULL) {
        return false;
    }

    size_t rom_len = 0U;
    const char *rom_json = io_fallback_find_rom_asset(MASTER_LOADER_CONTRACT_PATH, &rom_len);
    if (rom_json != NULL && rom_len > 0U &&
        io_fallback_parse_loader_contract_json(rom_json, rom_len, out_contract) &&
        master_loader_validate_contract(out_contract)) {
        out_contract->loaded_from_data = false;
        return true;
    }

    /* Last-resort Tier-0 static baseline if external ROM generator absent */
    memset(out_contract, 0, sizeof(*out_contract));
    out_contract->require_block_1_metadata = true;
    out_contract->require_5block_envelope = true;
    out_contract->enforce_schema_id = true;
    out_contract->require_monotonic_version = true;
    out_contract->allow_legacy_block2_types = true;
    out_contract->probe_await_on_corrupt = true;
    out_contract->strict_utf8_escapes = false;
    out_contract->reject_duplicate_keys = false;
    out_contract->max_json_depth = MASTER_LOADER_MAX_JSON_DEPTH;
    out_contract->max_json_tokens = JSON_DEFAULT_MAX_TOKENS;
    out_contract->max_payload_bytes = MASTER_LOADER_MAX_PAYLOAD_BYTES;
    str_safe_copy(out_contract->expected_major_minor_version, "0.1",
                  sizeof(out_contract->expected_major_minor_version));
    out_contract->loaded_from_data = false;
    return false;
}

/** Lazy-arms live contract from BIOS or `.rodata` ROM on first use. */
static void master_loader_ensure_contract_armed(void)
{
    if (s_loader_state.contract_armed) {
        return;
    }

    master_loader_contract_t fb_contract;
    if (io_fallback_get_loader_contract(&fb_contract) &&
        master_loader_validate_contract(&fb_contract)) {
        s_loader_state.contract = fb_contract;
        s_loader_state.contract_from_data = fb_contract.loaded_from_data;
    } else {
        (void)loader_build_rom_contract(&s_loader_state.contract);
        s_loader_state.contract_from_data = false;
    }
    s_loader_state.contract_armed = true;
}

/** Records a non-fatal or escalating fault cleanly across `io_fallback` + `LOG_CAT_IO`. */
static void loader_record_fault_telemetry(const char *func_tag,
                                          const char *reason,
                                          uint16_t err_code,
                                          bool escalate_probe)
{
    if (s_loader_state.in_preflight) {
        return;
    }

    io_fallback_record_fault(MASTER_LOADER_MODULE_ID,
                             (func_tag != NULL) ? func_tag : "master_loader",
                             (reason != NULL) ? reason : "MasterLoader envelope fault",
                             err_code);

    if (escalate_probe) {
        (void)master_loader_trigger_probe(reason, err_code);
    }
}

/** Downgrades runtime contract cleanly to `.rodata` ROM defaults with optional probe telemetry. */
static void loader_apply_rom_contract_downgrade(const master_loader_contract_t *rom_contract,
                                                const char *func_tag,
                                                const char *safe_reason,
                                                const char *fault_reason,
                                                const char *log_detail,
                                                uint16_t err_code,
                                                bool escalate_probe,
                                                const char *probe_msg)
{
    master_loader_contract_t safe_c;
    if (rom_contract != NULL) {
        safe_c = *rom_contract;
    } else {
        (void)loader_build_rom_contract(&safe_c);
    }

    safe_c.loaded_from_data = false;
    (void)io_fallback_set_loader_contract(&safe_c);
    s_loader_state.contract = safe_c;
    s_loader_state.contract_from_data = false;
    s_loader_state.contract_armed = true;

    if (!s_loader_state.in_preflight) {
        s_loader_state.corrupt_contract_downgrades++;
    }

    if (fault_reason != NULL) {
        (void)io_fallback_downgrade_safe_mode(safe_reason, false);
        io_fallback_record_fault(MASTER_LOADER_MODULE_ID,
                                 (func_tag != NULL) ? func_tag : "master_loader",
                                 fault_reason,
                                 err_code);
        LOADER_EMIT_LOG(LOG_ERROR, -0.75f, "%s",
                        (log_detail != NULL) ? log_detail : fault_reason);

        if (escalate_probe && debug_is_active() && !s_loader_state.in_preflight) {
            master_loader_register_debug_probes();
            (void)debug_trigger_probe(
                "io.loader.contract_bounds",
                MODULE_ID,
                (probe_msg != NULL) ? probe_msg : fault_reason,
                __FILE__, __LINE__,
                (func_tag != NULL) ? func_tag : __func__
            );
        }
    }
}

/** Single-source boundary-safe version prefix matcher (`"0.1"` matches `"0.1.1-dev"`). */
static bool loader_version_matches_prefix(const char *actual_ver,
                                          const char *required_prefix)
{
    if (actual_ver == NULL || required_prefix == NULL ||
        actual_ver[0] == '\0' || required_prefix[0] == '\0') {
        return false;
    }

    size_t req_len = strlen(required_prefix);
    if (strncmp(actual_ver, required_prefix, req_len) != 0) {
        return false;
    }

    char tail = actual_ver[req_len];
    return (tail == '\0' || tail == '.' || tail == '-');
}

/** Extracts Block 1 & Block 2 provenance cleanly within dedicated block object spans. */
static void loader_extract_header_spans(const char *json_text,
                                        size_t json_len,
                                        bool allow_legacy_block2,
                                        loader_header_t *out_hdr)
{
    const char *b1_s = NULL;
    size_t b1_l = 0U;
    if (json_find_object_span(json_text, json_len, "block_1_metadata", &b1_s, &b1_l) &&
        b1_s != NULL && b1_l >= 2U) {
        out_hdr->has_block_1 = true;
        (void)json_get_string_span(b1_s, b1_l, "project",
                                   out_hdr->project, sizeof(out_hdr->project));
        (void)json_get_string_span(b1_s, b1_l, "file",
                                   out_hdr->file, sizeof(out_hdr->file));
        (void)json_get_string_span(b1_s, b1_l, "id",
                                   out_hdr->id, sizeof(out_hdr->id));
        (void)json_get_string_span(b1_s, b1_l, "layer",
                                   out_hdr->layer, sizeof(out_hdr->layer));
        (void)json_get_string_span(b1_s, b1_l, "version",
                                   out_hdr->version, sizeof(out_hdr->version));
    }

    const char *b2_s = NULL;
    size_t b2_l = 0U;
    if (json_find_object_span(json_text, json_len, "block_2_opening", &b2_s, &b2_l) &&
        b2_s != NULL && b2_l >= 2U) {
        out_hdr->has_block_2 = true;
        const char *decl_s = NULL;
        size_t decl_l = 0U;
        if (json_find_object_span(b2_s, b2_l, "static_declarations", &decl_s, &decl_l) &&
            decl_s != NULL && decl_l >= 2U) {
            (void)json_get_string_span(decl_s, decl_l, "schema_id",
                                       out_hdr->schema_id, sizeof(out_hdr->schema_id));
        }
        if (out_hdr->schema_id[0] == '\0') {
            (void)json_get_string_span(b2_s, b2_l, "schema_id",
                                       out_hdr->schema_id, sizeof(out_hdr->schema_id));
        }
    } else if (allow_legacy_block2 &&
               json_find_object_span(json_text, json_len, "block_2_types", &b2_s, &b2_l) &&
               b2_s != NULL && b2_l >= 2U) {
        out_hdr->has_block_2 = true;
        (void)json_get_string_span(b2_s, b2_l, "schema_id",
                                   out_hdr->schema_id, sizeof(out_hdr->schema_id));
    }
}

/** Shared single-source JSON validation + envelope slicer preserving VFS provenance. */
static bool master_loader_parse_envelope_internal(master_loader_t *loader,
                                                  const char *json_text,
                                                  size_t json_len,
                                                  bool is_rom,
                                                  bool is_disk,
                                                  bool clear_resolved_path)
{
    if (loader == NULL) {
        return false;
    }

    char preserved_path[PATH_FINDER_MAX_PATH];
    preserved_path[0] = '\0';
    if (!clear_resolved_path) {
        str_safe_copy(preserved_path, loader->resolved_path, sizeof(preserved_path));
    }

    memset(loader, 0, sizeof(*loader));
    if (!clear_resolved_path) {
        str_safe_copy(loader->resolved_path, preserved_path, sizeof(loader->resolved_path));
    }
    loader->header.loaded_from_rom = is_rom;
    loader->loaded_from_data = is_disk && !is_rom;

    master_loader_ensure_contract_armed();

    if (json_text == NULL) {
        loader->status = LOADER_STATUS_ERR_NULL_PARAM;
        LOADER_EMIT_LOG(LOG_WARN, -0.25f, "Rejected NULL JSON source text in parser");
        return false;
    }

    size_t effective_len = (json_len > 0U) ? json_len : strlen(json_text);
    if (effective_len == 0U) {
        loader->status = LOADER_STATUS_ERR_SYNTAX_ERROR;
        if (!s_loader_state.in_preflight) {
            s_loader_state.syntax_errors++;
        }
        LOADER_EMIT_LOG(LOG_ERROR, -0.60f,
                        "Rejected empty JSON payload (%s)",
                        loader->resolved_path[0] ? loader->resolved_path : "memory");
        loader_record_fault_telemetry("master_loader_parse_envelope",
                                      "Empty JSON payload passed to MasterLoader",
                                      IO_ERR_JSON_SYNTAX, true);
        return false;
    }

    if (effective_len > s_loader_state.contract.max_payload_bytes) {
        loader->raw_buf = json_text;
        loader->raw_len = effective_len;
        loader->status = LOADER_STATUS_ERR_BUFFER_OVERFLOW;
        if (!s_loader_state.in_preflight) {
            s_loader_state.overflow_errors++;
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.45f,
                        "Payload size (%zu B) exceeded contract max_payload_bytes (%zu B)",
                        effective_len, s_loader_state.contract.max_payload_bytes);
        return false;
    }

    loader->raw_buf = json_text;
    loader->raw_len = effective_len;

    json_validation_report_t vrep;
    if (!json_validate_span(json_text, effective_len, &vrep)) {
        bool is_depth = (vrep.error_code == IO_ERR_JSON_DEPTH_EXCEEDED);
        loader->status = is_depth ? LOADER_STATUS_ERR_CONTRACT_BREACH
                                  : LOADER_STATUS_ERR_SYNTAX_ERROR;
        if (!s_loader_state.in_preflight) {
            s_loader_state.syntax_errors++;
        }
        LOADER_EMIT_LOG(LOG_ERROR, -0.70f,
                        "JSON structural failure at offset %zu (%s): %s",
                        vrep.error_offset,
                        loader->resolved_path[0] ? loader->resolved_path : "memory",
                        vrep.diagnostic);
        loader_record_fault_telemetry("master_loader_parse_envelope",
                                      vrep.diagnostic[0] ? vrep.diagnostic
                                                         : "Malformed JSON syntax rejected",
                                      (vrep.error_code != 0U) ? vrep.error_code : IO_ERR_JSON_SYNTAX,
                                      true);
        return false;
    }

    if (vrep.max_depth_observed > s_loader_state.contract.max_json_depth ||
        vrep.token_count > s_loader_state.contract.max_json_tokens) {
        loader->status = LOADER_STATUS_ERR_CONTRACT_BREACH;
        if (!s_loader_state.in_preflight) {
            s_loader_state.syntax_errors++;
        }
        LOADER_EMIT_LOG(LOG_ERROR, -0.70f,
                        "JSON depth/tokens (%d / %zu) exceeded contract (%d / %zu)",
                        vrep.max_depth_observed, vrep.token_count,
                        s_loader_state.contract.max_json_depth,
                        s_loader_state.contract.max_json_tokens);
        loader_record_fault_telemetry("master_loader_parse_envelope",
                                      "JSON nesting depth or token count exceeded contract",
                                      IO_ERR_JSON_DEPTH_EXCEEDED, true);
        return false;
    }

    loader_extract_header_spans(json_text,
                                effective_len,
                                s_loader_state.contract.allow_legacy_block2_types,
                                &loader->header);

    if (s_loader_state.contract.require_block_1_metadata && !loader->header.has_block_1) {
        loader->status = LOADER_STATUS_ERR_CONTRACT_BREACH;
        if (!s_loader_state.in_preflight) {
            s_loader_state.syntax_errors++;
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.50f,
                        "Envelope missing mandatory block_1_metadata (%s)",
                        loader->resolved_path[0] ? loader->resolved_path : "memory");
        return false;
    }

    const char *body_s = NULL;
    size_t body_l = 0U;
    if (json_find_object_span(json_text, effective_len, "block_3_body", &body_s, &body_l) &&
        body_s != NULL && body_l >= 2U) {
        loader->body_start = body_s;
        loader->body_len = body_l;
        loader->header.is_5block = s_loader_state.contract.require_block_1_metadata
                                       ? loader->header.has_block_1
                                       : true;
    } else if (s_loader_state.contract.require_5block_envelope) {
        loader->status = LOADER_STATUS_ERR_BODY_MISSING;
        if (!s_loader_state.in_preflight) {
            s_loader_state.body_missing_errors++;
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.45f,
                        "5-block envelope missing block_3_body (%s)",
                        loader->resolved_path[0] ? loader->resolved_path : "memory");
        return false;
    } else {
        loader->body_start = json_text;
        loader->body_len = effective_len;
        loader->header.is_5block = false;
    }

    loader->status = LOADER_STATUS_OK;
    return true;
}

/** Single-source span resolver isolating all body & sub-object lookups to `[body_start, body_len)`. */
static bool master_loader_resolve_body_scope(const master_loader_t *loader,
                                             const char *sub_obj,
                                             const char **out_span,
                                             size_t *out_len)
{
    if (!loader_is_ready(loader) || out_span == NULL || out_len == NULL) {
        return false;
    }

    const bool has_body = loader_span_nonempty(loader->body_start, loader->body_len);
    const char *base_span = has_body ? loader->body_start : loader->raw_buf;
    size_t base_len = has_body ? loader->body_len : loader->raw_len;
    if (!loader_span_nonempty(base_span, base_len)) {
        return false;
    }

    if (!s_loader_state.in_preflight) {
        s_loader_state.body_queries++;
    }

    if (sub_obj != NULL && sub_obj[0] != '\0') {
        const char *sub_s = NULL;
        size_t sub_l = 0U;
        if (!json_find_object_span(base_span, base_len, sub_obj, &sub_s, &sub_l) ||
            sub_s == NULL || sub_l < 2U) {
            return false;
        }
        *out_span = sub_s;
        *out_len = sub_l;
        return true;
    }

    *out_span = base_span;
    *out_len = base_len;
    return true;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Operational Business Logic, Span Scopes & Runtime Hydration )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0 (Core Utilities)
Module:        cog.master_loader
Mission:       5-Block Envelope Parser, Body Span Isolation & Contract Hydration
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Ingestion Cogs
Mission: Resolve domain assets or parse memory envelopes, enforce structural
         JSON & size bounds, and delegate envelope verification (`2.5`).

  ### For Game Designers & Non-Programmers:
  Provides safe zero-allocation entry points that fetch configs and story files
  from disk or ROM backups and validate their 5-block headers before use.

  ### For Systems Programmers & Engine Architects:
  Combines `path_finder_resolve()`, `fs_read_file_bounded()`, and
  `master_loader_parse_envelope_internal()` cleanly while tracking load counts
  and downgrading broken files safely into diagnostic error states.
  @triggers `io.loader.envelope_integrity`, `LOG_CAT_IO`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Resolves a VFS domain asset and ingests its 5-block JSON envelope.
 *
 * ### For Game Designers & Non-Programmers:
 * Looks up a file by logical category (`"config"`, `"mechanics"`, `"world"`) and
 * relative path across game overrides, engine defaults, and built-in ROM backups.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears `*loader`, arms contract defaults lazily, rejects empty arguments with
 * `LOADER_STATUS_ERR_NULL_PARAM`, resolves VFS path via `path_finder_resolve()`,
 * and delegates disk/ROM reading and envelope slicing to `master_loader_load_from_path()`.
 * @triggers `io.loader.envelope_integrity`, `LOG_CAT_IO` on miss or syntax fault.
 *
 * @param[out] loader   Caller-owned loader state initialized by this call.
 * @param[in]  domain   Logical VFS domain (`"config"`, `"mechanics"`, `"world"`, or NULL).
 * @param[in]  rel_path Relative file path within the target domain.
 * @param[out] buf      Caller-supplied byte buffer receiving file payload.
 * @param[in]  buf_size Capacity of `buf` in bytes (must accommodate trailing NUL).
 * @return true on clean VFS resolve, read, and envelope parse; false otherwise.
 */
bool master_loader_load(master_loader_t *loader,
                        const char *domain,
                        const char *rel_path,
                        char *buf,
                        size_t buf_size)
{
    /* Step 1: Reject NULL loader defensively and clear context before arming contract. */
    if (loader == NULL) {
        return false;
    }

    memset(loader, 0, sizeof(*loader));
    master_loader_ensure_contract_armed();

    /* Step 2: Validate required path and caller buffer capacity bounds. */
    if (rel_path == NULL || rel_path[0] == '\0' || buf == NULL || buf_size == 0U) {
        loader->status = LOADER_STATUS_ERR_NULL_PARAM;
        LOADER_EMIT_LOG(LOG_WARN, -0.25f, "master_loader_load rejected invalid argument");
        return false;
    }

    if (!s_loader_state.in_preflight) {
        s_loader_state.total_loads++;
    }

    /* Step 3: Resolve concrete disk path or rom:// URI and delegate payload load. */
    char resolved[PATH_FINDER_MAX_PATH];
    if (!path_finder_resolve(domain, rel_path, resolved, sizeof(resolved))) {
        loader->status = LOADER_STATUS_ERR_NOT_FOUND;
        if (!s_loader_state.in_preflight) {
            s_loader_state.not_found_errors++;
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.20f,
                        "VFS resolve miss for domain='%s' rel='%s'",
                        (domain != NULL) ? domain : "(default)", rel_path);
        return false;
    }

    return master_loader_load_from_path(loader, resolved, buf, buf_size);
}

/**
 * @brief Reads a JSON payload from an explicit disk path or `"rom://..."` URI and parses it.
 *
 * ### For Game Designers & Non-Programmers:
 * Opens a concrete file path or virtual ROM backup address, ensures it fits within
 * memory safety limits, and validates its 5-block header and body structure.
 *
 * ### For Systems Programmers & Engine Architects:
 * Detects `"rom://"` provenance cleanly, verifies file existence and payload cap
 * against `min(buf_size, max_payload_bytes)`, performs bounded read into `buf`,
 * and dispatches `master_loader_parse_envelope_internal()`.
 * @triggers `io.loader.envelope_integrity`, `LOG_CAT_IO` on read/envelope fault.
 *
 * @param[out] loader   Caller-owned loader state populated on return.
 * @param[in]  path     Resolved POSIX path or `"rom://..."` virtual asset URI.
 * @param[out] buf      Caller buffer receiving NUL-terminated JSON payload.
 * @param[in]  buf_size Byte capacity of `buf`.
 * @return true if file/ROM asset read cleanly and passed contract checks; false otherwise.
 */
bool master_loader_load_from_path(master_loader_t *loader,
                                  const char *path,
                                  char *buf,
                                  size_t buf_size)
{
    /* Step 1: Reject NULL loader defensively and zero-initialize loader provenance fields. */
    if (loader == NULL) {
        return false;
    }

    memset(loader, 0, sizeof(*loader));
    master_loader_ensure_contract_armed();

    /* Step 2: Validate path and caller buffer bounds prior to filesystem access. */
    if (path == NULL || path[0] == '\0' || buf == NULL || buf_size == 0U) {
        loader->status = LOADER_STATUS_ERR_NULL_PARAM;
        LOADER_EMIT_LOG(LOG_WARN, -0.25f, "master_loader_load_from_path rejected invalid parameter");
        return false;
    }

    str_safe_copy(loader->resolved_path, path, sizeof(loader->resolved_path));
    bool is_rom = io_fb_is_rom_uri(path);
    loader->header.loaded_from_rom = is_rom;
    loader->loaded_from_data = !is_rom;

    /* Step 3: Verify resource presence and pre-read byte size against chassis ceiling. */
    if (!fs_file_exists(path)) {
        loader->status = LOADER_STATUS_ERR_NOT_FOUND;
        if (!s_loader_state.in_preflight) {
            s_loader_state.not_found_errors++;
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.20f, "Source file absent at '%s'", path);
        return false;
    }

    long fsize = fs_get_file_size(path);
    if (fsize < 0L) {
        loader->status = LOADER_STATUS_ERR_NOT_FOUND;
        if (!s_loader_state.in_preflight) {
            s_loader_state.not_found_errors++;
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.25f, "Unable to stat payload size at '%s'", path);
        return false;
    }

    if ((size_t)fsize >= buf_size ||
        (size_t)fsize > s_loader_state.contract.max_payload_bytes) {
        loader->status = LOADER_STATUS_ERR_BUFFER_OVERFLOW;
        if (!s_loader_state.in_preflight) {
            s_loader_state.overflow_errors++;
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.45f,
                        "File '%s' (%ld B) exceeds buffer/contract limit (%zu / %zu B)",
                        path, fsize, buf_size, s_loader_state.contract.max_payload_bytes);
        return false;
    }

    /* Step 4: Read bounded payload into caller buffer and classify short/overflow aborts. */
    size_t bytes_read = 0U;
    if (!fs_read_file_bounded(path, buf, buf_size, &bytes_read) || bytes_read == 0U) {
        long recheck = fs_get_file_size(path);
        bool too_large = (recheck >= 0L && (size_t)recheck >= buf_size);
        loader->status = too_large ? LOADER_STATUS_ERR_BUFFER_OVERFLOW
                                   : LOADER_STATUS_ERR_NOT_FOUND;
        if (!s_loader_state.in_preflight) {
            if (too_large) {
                s_loader_state.overflow_errors++;
            } else {
                s_loader_state.not_found_errors++;
            }
        }
        LOADER_EMIT_LOG(LOG_WARN, -0.35f,
                        "Bounded read aborted at '%s' (%s)",
                        path, too_large ? "buffer overflow" : "unreadable/empty");
        return false;
    }

    /* Step 5: Verify 5-block JSON envelope and update disk vs ROM telemetry counters. */
    bool ok = master_loader_parse_envelope_internal(loader, buf, bytes_read, is_rom, !is_rom, false);
    if (ok && !s_loader_state.in_preflight) {
        if (is_rom) {
            s_loader_state.rom_loads++;
        } else {
            s_loader_state.disk_loads++;
        }
    }
    return ok;
}

/**
 * @brief Parses and verifies a 5-block JSON envelope directly from an in-memory buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Validates embedded presets and test snippets with the same header, version, and
 * `block_3_body` rules as files loaded from disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Non-owning zero-copy parse path: arms contract policy, bumps load/memory telemetry
 * outside preflight, and invokes `master_loader_parse_envelope_internal()` cleanly.
 * @triggers `io.loader.envelope_integrity`, `LOG_CAT_IO` on malformed JSON/envelope.
 *
 * @param[out] loader    Caller-owned loader context populated with extracted spans.
 * @param[in]  json_text Immutable JSON text remaining valid while `loader` is queried.
 * @param[in]  json_len  Byte length (`0` measures with `strlen()`).
 * @return true on clean envelope verification; false with diagnostic status otherwise.
 */
bool master_loader_load_from_memory(master_loader_t *loader,
                                    const char *json_text,
                                    size_t json_len)
{
    /* Step 1: Ensure contract defaults are armed and record non-preflight memory telemetry. */
    master_loader_ensure_contract_armed();
    if (!s_loader_state.in_preflight && loader != NULL && json_text != NULL) {
        s_loader_state.total_loads++;
        s_loader_state.memory_loads++;
    }

    /* Step 2: Run shared zero-copy envelope validator and body span extractor. */
    return master_loader_parse_envelope_internal(loader, json_text, json_len, false, false, true);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Scoped `block_3_body` Accessors
Mission: Confine all typed field extractions and array slicing strictly to
         `[body_start, body_start + body_len)` or named `sub_obj`, blocking
         cross-block header collisions completely.

  ### For Game Designers & Non-Programmers:
  Reads text, integers, decimals, booleans, and lists cleanly from `block_3_body`
  alone so metadata headers or audit sections can never interfere with gameplay data.

  ### For Systems Programmers & Engine Architects:
  Resolves active body scope via `master_loader_resolve_body_scope()` (2.5) before
  forwarding strictly bounded spans to `cog.json_utils` extractors.
  @triggers `io.json.depth_bounds` if nested lookup breaches depth bounds.
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts a bounded string field strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Copies a text value out of the loaded file's data body safely with NUL termination.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears `out_val[0]`, validates key/capacity, isolates span via
 * `master_loader_resolve_body_scope()`, and delegates decoding to `json_get_string_span()`.
 * @triggers `io.json.depth_bounds` on nesting violation inside body scope.
 *
 * @param[in]  loader   Loaded `master_loader_t` handle.
 * @param[in]  sub_obj  Optional nested sub-object key inside `block_3_body` (nullable).
 * @param[in]  key      Target string property key.
 * @param[out] out_val  Caller string buffer receiving decoded text.
 * @param[in]  out_size Capacity of `out_val` in bytes.
 * @return true on extraction within `out_size`; false otherwise.
 */
bool master_loader_get_string(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              char *out_val,
                              size_t out_size)
{
    /* Step 1: Validate caller buffer and key, clearing destination defensively. */
    if (out_val == NULL || out_size == 0U) {
        return false;
    }
    out_val[0] = '\0';

    if (key == NULL || key[0] == '\0') {
        return false;
    }

    /* Step 2: Isolate body/sub_obj byte slice and decode bounded string. */
    const char *scope = NULL;
    size_t scope_len = 0U;
    if (!master_loader_resolve_body_scope(loader, sub_obj, &scope, &scope_len)) {
        return false;
    }

    return json_get_string_span(scope, scope_len, key, out_val, out_size);
}

/**
 * @brief Extracts a strict signed `int` strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads whole-number gameplay stats or policy limits solely inside the body payload.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves isolated span via `master_loader_resolve_body_scope()` and parses strict
 * 32-bit integer value via `json_get_int_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Loaded `master_loader_t` handle.
 * @param[in]  sub_obj Optional nested sub-object key inside `block_3_body` (nullable).
 * @param[in]  key     Target integer key.
 * @param[out] out_val Destination signed integer storage.
 * @return true on clean integer parse; false otherwise.
 */
bool master_loader_get_int(const master_loader_t *loader,
                           const char *sub_obj,
                           const char *key,
                           int *out_val)
{
    /* Step 1: Validate key and destination pointer before isolating body span. */
    if (key == NULL || key[0] == '\0' || out_val == NULL) {
        return false;
    }

    /* Step 2: Resolve isolated span and parse strict signed integer. */
    const char *scope = NULL;
    size_t scope_len = 0U;
    if (!master_loader_resolve_body_scope(loader, sub_obj, &scope, &scope_len)) {
        return false;
    }

    return json_get_int_span(scope, scope_len, key, out_val);
}

/**
 * @brief Extracts a strict unsigned `size_t` strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads non-negative capacities or slot counts cleanly from the data payload body.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves isolated span via `master_loader_resolve_body_scope()` and rejects negative
 * or overflowing integers via `json_get_size_t_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Loaded `master_loader_t` handle.
 * @param[in]  sub_obj Optional nested sub-object key inside `block_3_body` (nullable).
 * @param[in]  key     Target unsigned size key.
 * @param[out] out_val Destination `size_t` storage.
 * @return true on clean unsigned integer parse; false otherwise.
 */
bool master_loader_get_size_t(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              size_t *out_val)
{
    /* Step 1: Validate key and destination pointer before isolating body span. */
    if (key == NULL || key[0] == '\0' || out_val == NULL) {
        return false;
    }

    /* Step 2: Resolve isolated span and parse strict unsigned size_t. */
    const char *scope = NULL;
    size_t scope_len = 0U;
    if (!master_loader_resolve_body_scope(loader, sub_obj, &scope, &scope_len)) {
        return false;
    }

    return json_get_size_t_span(scope, scope_len, key, out_val);
}

/**
 * @brief Extracts a finite `float` strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads decimal multipliers and tuning ratios solely inside the data body while
 * blocking non-finite or malformed numeric tokens.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves isolated span via `master_loader_resolve_body_scope()` and delegates
 * finite conversion to `json_get_float_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Loaded `master_loader_t` handle.
 * @param[in]  sub_obj Optional nested sub-object key inside `block_3_body` (nullable).
 * @param[in]  key     Target float key.
 * @param[out] out_val Destination `float` storage.
 * @return true on valid finite float parse; false otherwise.
 */
bool master_loader_get_float(const master_loader_t *loader,
                             const char *sub_obj,
                             const char *key,
                             float *out_val)
{
    /* Step 1: Validate key and destination pointer before isolating body span. */
    if (key == NULL || key[0] == '\0' || out_val == NULL) {
        return false;
    }

    /* Step 2: Resolve isolated span and parse finite decimal float. */
    const char *scope = NULL;
    size_t scope_len = 0U;
    if (!master_loader_resolve_body_scope(loader, sub_obj, &scope, &scope_len)) {
        return false;
    }

    return json_get_float_span(scope, scope_len, key, out_val);
}

/**
 * @brief Extracts a strict JSON boolean strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads on/off toggles cleanly inside `block_3_body` without touching surrounding blocks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves isolated span via `master_loader_resolve_body_scope()` and delegates exact
 * `true`/`false` literal parsing to `json_get_bool_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Loaded `master_loader_t` handle.
 * @param[in]  sub_obj Optional nested sub-object key inside `block_3_body` (nullable).
 * @param[in]  key     Target boolean key.
 * @param[out] out_val Destination `bool` storage.
 * @return true on valid JSON boolean literal parse; false otherwise.
 */
bool master_loader_get_bool(const master_loader_t *loader,
                            const char *sub_obj,
                            const char *key,
                            bool *out_val)
{
    /* Step 1: Validate key and destination pointer before isolating body span. */
    if (key == NULL || key[0] == '\0' || out_val == NULL) {
        return false;
    }

    /* Step 2: Resolve isolated span and parse delimiter-safe JSON bool. */
    const char *scope = NULL;
    size_t scope_len = 0U;
    if (!master_loader_resolve_body_scope(loader, sub_obj, &scope, &scope_len)) {
        return false;
    }

    return json_get_bool_span(scope, scope_len, key, out_val);
}

/**
 * @brief Locates a JSON `[...]` array span and element count inside `block_3_body`.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds tables and lists inside a data manifest (such as item entries or encounter
 * definitions) and measures their item count without allocating extra memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Clears output pointers defensively, isolates `[scope, scope + scope_len)`, finds
 * balanced `[...]` via `json_find_array_span()`, and optionally populates `*out_count`
 * via `json_array_get_count()`.
 * @triggers `io.json.depth_bounds` if array bracket scan exceeds `max_json_depth`.
 *
 * @param[in]  loader    Loaded `master_loader_t` handle.
 * @param[in]  sub_obj   Optional nested sub-object key inside `block_3_body` (nullable).
 * @param[in]  array_key Target array key name.
 * @param[out] out_start Receives pointer to `'['` inside `loader->raw_buf`.
 * @param[out] out_len   Receives byte length from `'['` through `']'`.
 * @param[out] out_count Optional destination for array element count (nullable).
 * @return true if array located cleanly within body scope; false otherwise.
 */
bool master_loader_find_array(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *array_key,
                              const char **out_start,
                              size_t *out_len,
                              size_t *out_count)
{
    /* Step 1: Initialize caller outputs defensively and validate required arguments. */
    if (out_start != NULL) {
        *out_start = NULL;
    }
    if (out_len != NULL) {
        *out_len = 0U;
    }
    if (out_count != NULL) {
        *out_count = 0U;
    }

    if (array_key == NULL || array_key[0] == '\0' || out_start == NULL || out_len == NULL) {
        return false;
    }

    /* Step 2: Isolate body scope, locate array bounds, and measure element count. */
    const char *scope = NULL;
    size_t scope_len = 0U;
    if (!master_loader_resolve_body_scope(loader, sub_obj, &scope, &scope_len)) {
        return false;
    }

    if (!json_find_array_span(scope, scope_len, array_key, out_start, out_len)) {
        return false;
    }

    if (out_count != NULL) {
        *out_count = json_array_get_count(*out_start, *out_len);
    }
    return true;
}

/**
 * @brief Extracts the i-th object element from a located array span into caller memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Pulls one complete catalog entry at a time from a list located by
 * `master_loader_find_array()` so subsystem loaders can inspect its fields cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates `[arr_start, arr_len)` and caller buffer headroom defensively before
 * forwarding cleanly to zero-heap `json_array_get_object()`.
 * @triggers `io.json.depth_bounds` if nested object breaches depth bounds.
 *
 * @param[in]  arr_start Pointer to `'['` array boundary from `master_loader_find_array()`.
 * @param[in]  arr_len   Exact byte length of array span.
 * @param[in]  index     0-based array element index.
 * @param[out] out_obj   Caller destination buffer receiving NUL-terminated object slice.
 * @param[in]  out_size  Capacity of `out_obj` in bytes.
 * @return true on clean copy; false on out-of-range index, wrong type, or overflow.
 */
bool master_loader_get_array_object(const char *arr_start,
                                    size_t arr_len,
                                    size_t index,
                                    char *out_obj,
                                    size_t out_size)
{
    /* Step 1: Validate array slice bounds and defensively NUL-terminate caller buffer. */
    if (arr_start == NULL || arr_len < 2U || out_obj == NULL || out_size == 0U) {
        if (out_obj != NULL && out_size > 0U) {
            out_obj[0] = '\0';
        }
        return false;
    }

    /* Step 2: Extract bounded object slice via zero-heap array iterator. */
    return json_array_get_object(arr_start, arr_len, index, out_obj, out_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Data-Driven Contract Hydration & Status Formatters
Mission: Synchronize live envelope verification rules with `loader_contracts.json`
         or Safe-Mode `.rodata` ROM defaults via unified downgrade helpers and
         format `loader_status_t` diagnostics cleanly.

  ### For Game Designers & Non-Programmers:
  Loads structural strictness rules and version expectations from project settings,
  recovering automatically to Safe-Mode ROM rules and readable error names on corruption.

  ### For Systems Programmers & Engine Architects:
  Consolidates ROM contract restoration and `io.loader.contract_bounds` probe
  escalation inside `loader_apply_rom_contract_downgrade()` (Sub-Assembly 2.5).
  @triggers `io.loader.contract_bounds`, `LOG_CAT_IO`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Hydrates MasterLoader verification policy from disk JSON or Safe-Mode ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads `engine/config/io/loader_contracts.json` so file strictness, version prefix
 * checks, and payload limits can be adjusted without recompiling, dropping safely back
 * to built-in ROM defaults if disk files are damaged.
 *
 * ### For Systems Programmers & Engine Architects:
 * Guarded by `in_load_contract` against reentrant VFS/contract loops. Validates disk
 * payload syntax + raw `block_3_body` bounds prior to `master_loader_validate_contract()`.
 * Corrupt disk files trigger `loader_apply_rom_contract_downgrade()` + `io.loader.contract_bounds`.
 * @triggers `io.loader.contract_bounds`, `LOG_CAT_IO`.
 *
 * @param[in] rel_or_abs_path Optional override path (NULL resolves `"io/loader_contracts.json"`).
 * @return true if loaded cleanly from disk (`loaded_from_data = true`); false if ROM active.
 */
bool master_loader_load_contract(const char *rel_or_abs_path)
{
    /* Step 1: Prevent reentrant contract reload loops during VFS resolution. */
    if (s_loader_state.in_load_contract) {
        return s_loader_state.contract_from_data;
    }

    s_loader_state.in_load_contract = true;
    master_loader_ensure_contract_armed();

    /* Step 2: Prepare Safe-Mode ROM baseline before attempting disk resolution. */
    master_loader_contract_t rom_contract;
    bool rom_ok = loader_build_rom_contract(&rom_contract);

    const char *target_arg = (rel_or_abs_path != NULL && rel_or_abs_path[0] != '\0')
                                 ? rel_or_abs_path
                                 : MASTER_LOADER_CONTRACT_PATH;
    char resolved[PATH_FINDER_MAX_PATH];
    resolved[0] = '\0';
    bool resolved_ok = false;

    if (fs_file_exists(target_arg) && !io_fb_is_rom_uri(target_arg)) {
        str_safe_copy(resolved, target_arg, sizeof(resolved));
        resolved_ok = true;
    } else {
        resolved_ok = path_finder_resolve("config", target_arg, resolved, sizeof(resolved));
    }

    bool resolved_is_disk = resolved_ok && !io_fb_is_rom_uri(resolved);
    char cfg_buf[IO_FB_MEDIUM_BYTES];
    size_t read_bytes = 0U;

    /* Step 3: Parse and enforce raw disk bounds before committing live contract state. */
    if (resolved_is_disk &&
        fs_read_file_bounded(resolved, cfg_buf, sizeof(cfg_buf), &read_bytes) &&
        read_bytes > 0U) {
        json_validation_report_t vrep;
        master_loader_contract_t disk_contract;
        if (json_validate_span(cfg_buf, read_bytes, &vrep) &&
            io_fallback_parse_loader_contract_json(cfg_buf, read_bytes, &disk_contract)) {
            const char *body_s = cfg_buf;
            size_t body_l = read_bytes;
            (void)json_find_object_span(cfg_buf, read_bytes, "block_3_body", &body_s, &body_l);

            int raw_depth = 0;
            if (json_get_int_span(body_s, body_l, "max_json_depth", &raw_depth)) {
                disk_contract.max_json_depth = raw_depth;
            }
            int raw_tokens = 0;
            if (json_get_int_span(body_s, body_l, "max_json_tokens", &raw_tokens)) {
                disk_contract.max_json_tokens = (raw_tokens > 0) ? (size_t)raw_tokens : 0U;
            }
            int raw_payload = 0;
            if (json_get_int_span(body_s, body_l, "max_payload_bytes", &raw_payload)) {
                disk_contract.max_payload_bytes = (raw_payload > 0) ? (size_t)raw_payload : 0U;
            }

            if (master_loader_validate_contract(&disk_contract)) {
                disk_contract.loaded_from_data = true;
                (void)io_fallback_set_loader_contract(&disk_contract);
                s_loader_state.contract = disk_contract;
                s_loader_state.contract_from_data = true;
                s_loader_state.contract_armed = true;
                s_loader_state.in_load_contract = false;

                LOADER_EMIT_LOG(LOG_INFO, 0.25f,
                                "Hydrated MasterLoader contract from '%s' (ver='%s.*', 5block=%s, legacy_b2=%s)",
                                resolved,
                                s_loader_state.contract.expected_major_minor_version,
                                s_loader_state.contract.require_5block_envelope ? "true" : "false",
                                s_loader_state.contract.allow_legacy_block2_types ? "true" : "false");
                return true;
            }
        }
    }

    /* Step 4: Restore ROM contract cleanly and escalate probes on corrupt disk files or missing ROM. */
    s_loader_state.in_load_contract = false;

    if (resolved_is_disk || !rom_ok) {
        char log_detail[PATH_FINDER_MAX_PATH + 96U];
        snprintf(log_detail, sizeof(log_detail),
                 "Corrupt contract at '%s'; downgraded to Safe-Mode ROM contract",
                 resolved[0] ? resolved : target_arg);
        loader_apply_rom_contract_downgrade(
            &rom_contract,
            "master_loader_load_contract",
            resolved_is_disk
                ? "Corrupt on-disk loader_contracts.json rejected; restored ROM contract"
                : "Critical compiled ROM asset io/loader_contracts.json missing",
            "Corrupt or out-of-bounds loader_contracts JSON; downgraded to ROM",
            log_detail,
            IO_ERR_JSON_SYNTAX,
            (rom_contract.probe_await_on_corrupt || !rom_ok),
            "Corrupt on-disk loader_contracts.json rejected -> ROM Safe Mode"
        );
    } else {
        loader_apply_rom_contract_downgrade(&rom_contract, NULL, NULL, NULL, NULL,
                                            0U, false, NULL);
        LOADER_EMIT_LOG(LOG_WARN, -0.35f,
                        "Disk loader_contracts absent at '%s'; using ROM Safe-Mode contract",
                        target_arg);
    }

    return false;
}

/**
 * @brief Copies the currently enforced `master_loader_contract_t` into caller memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Exposes active verification switches, version expectations, and size caps to tools
 * and diagnostics panels cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Rejects NULL output silently, arms default ROM contract lazily via
 * `master_loader_ensure_contract_armed()`, and copies `s_loader_state.contract`.
 * @triggers None.
 *
 * @param[out] out_contract Caller struct receiving copied verification contract.
 * @return true on success; false when `out_contract` is NULL.
 */
bool master_loader_get_contract(master_loader_contract_t *out_contract)
{
    /* Step 1: Reject NULL output descriptor silently and arm contract lazily before copy. */
    if (out_contract == NULL) {
        return false;
    }
    master_loader_ensure_contract_armed();
    *out_contract = s_loader_state.contract;
    return true;
}

/**
 * @brief Validates and commits an explicit `master_loader_contract_t` into runtime state.
 *
 * ### For Game Designers & Non-Programmers:
 * Updates schema strictness at runtime while safely reverting to built-in ROM defaults
 * if corrupt limits are supplied.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks bounds via `master_loader_validate_contract()`. On rejection, invokes
 * `loader_apply_rom_contract_downgrade()` (`IO_ERR_JSON_DEPTH_EXCEEDED` +
 * `io.loader.contract_bounds`); on pass, updates `s_loader_state` and Tier-0 BIOS.
 * @triggers `io.loader.contract_bounds`, `LOG_CAT_IO` on contract rejection.
 *
 * @param[in] contract Candidate `master_loader_contract_t` descriptor.
 * @return true if valid and installed; false after Safe-Mode downgrade on corruption.
 */
bool master_loader_set_contract(const master_loader_contract_t *contract)
{
    /* Step 1: Reject NULL pointer cleanly and arm baseline contract state. */
    if (contract == NULL) {
        return false;
    }

    master_loader_ensure_contract_armed();

    /* Step 2: Downgrade to Safe-Mode ROM and trigger probe if candidate breaches bounds. */
    if (!master_loader_validate_contract(contract)) {
        char log_detail[160];
        snprintf(log_detail, sizeof(log_detail),
                 "Rejected corrupt MasterLoader contract (depth=%d, tok=%zu, bytes=%zu)",
                 contract->max_json_depth,
                 contract->max_json_tokens,
                 contract->max_payload_bytes);
        loader_apply_rom_contract_downgrade(
            NULL,
            "master_loader_set_contract",
            "Rejected corrupted master_loader_contract_t; engaged ROM Safe Mode",
            "Rejected invalid master_loader_contract_t bounds; restored ROM",
            log_detail,
            IO_ERR_JSON_DEPTH_EXCEEDED,
            true,
            "Invalid master_loader_contract_t rejected -> Safe-Mode ROM downgrade"
        );
        return false;
    }

    /* Step 3: Commit validated contract into local chassis and shared Tier-0 BIOS. */
    s_loader_state.contract = *contract;
    s_loader_state.contract_from_data = contract->loaded_from_data;
    s_loader_state.contract_armed = true;
    (void)io_fallback_set_loader_contract(contract);

    LOADER_EMIT_LOG(LOG_INFO, 0.20f,
                    "Updated MasterLoader contract (ver='%s.*', max_depth=%d, await=%s)",
                    s_loader_state.contract.expected_major_minor_version,
                    s_loader_state.contract.max_json_depth,
                    s_loader_state.contract.probe_await_on_corrupt ? "true" : "false");
    return true;
}

/**
 * @brief Translates a `loader_status_t` enum code into a static `.rodata` mnemonic string.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts numeric loader status codes into readable labels for logs, status cards,
 * and debugging reports.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure `O(1)` switch returning immutable `.rodata` string literals (falls back cleanly
 * to `"LOADER_STATUS_UNKNOWN"` on unrecognized values). Zero allocations.
 * @triggers None.
 *
 * @param[in] status Loader status enum value.
 * @return Non-NULL pointer to static `.rodata` NUL-terminated status mnemonic.
 */
const char *master_loader_status_to_string(loader_status_t status)
{
    /* Step 1: Map every enumerated loader outcome to its immutable .rodata mnemonic. */
    switch (status) {
        case LOADER_STATUS_OK:                   return "LOADER_STATUS_OK";
        case LOADER_STATUS_ERR_NOT_FOUND:        return "LOADER_STATUS_ERR_NOT_FOUND";
        case LOADER_STATUS_ERR_BUFFER_OVERFLOW:  return "LOADER_STATUS_ERR_BUFFER_OVERFLOW";
        case LOADER_STATUS_ERR_SCHEMA_MISMATCH:  return "LOADER_STATUS_ERR_SCHEMA_MISMATCH";
        case LOADER_STATUS_ERR_SYNTAX_ERROR:     return "LOADER_STATUS_ERR_SYNTAX_ERROR";
        case LOADER_STATUS_ERR_NULL_PARAM:       return "LOADER_STATUS_ERR_NULL_PARAM";
        case LOADER_STATUS_ERR_BODY_MISSING:     return "LOADER_STATUS_ERR_BODY_MISSING";
        case LOADER_STATUS_ERR_VERSION_MISMATCH: return "LOADER_STATUS_ERR_VERSION_MISMATCH";
        case LOADER_STATUS_ERR_CONTRACT_BREACH:  return "LOADER_STATUS_ERR_CONTRACT_BREACH";
        default:                                 return "LOADER_STATUS_UNKNOWN";
    }
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Validation Predicates, Debugger Probes & Non-Destructive Preflight )
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract Predicates & Debugger Probe Escalation
--------------------------------------------------------------------------------
*/

bool master_loader_validate_schema(const master_loader_t *loader,
                                   const char *expected_schema_id)
{
    if (!loader_is_ready(loader) ||
        expected_schema_id == NULL || expected_schema_id[0] == '\0') {
        return false;
    }

    return strcmp(loader->header.schema_id, expected_schema_id) == 0;
}

bool master_loader_validate_version(const master_loader_t *loader,
                                    const char *required_version)
{
    if (!loader_is_ready(loader) ||
        required_version == NULL || required_version[0] == '\0') {
        return false;
    }

    return loader_version_matches_prefix(loader->header.version, required_version);
}

bool master_loader_validate_envelope(const master_loader_t *loader,
                                     const char *expected_schema_id)
{
    if (!loader_is_ready(loader)) {
        return false;
    }

    master_loader_ensure_contract_armed();

    if (s_loader_state.contract.require_block_1_metadata && !loader->header.has_block_1) {
        return false;
    }
    if (s_loader_state.contract.require_5block_envelope && !loader_has_valid_body(loader)) {
        return false;
    }

    if (expected_schema_id != NULL && expected_schema_id[0] != '\0') {
        if (strcmp(loader->header.schema_id, expected_schema_id) != 0) {
            return false;
        }
    } else if (s_loader_state.contract.enforce_schema_id &&
               loader->header.schema_id[0] == '\0') {
        return false;
    }

    if (s_loader_state.contract.require_monotonic_version &&
        loader->header.version[0] != '\0' &&
        !loader_version_matches_prefix(
            loader->header.version,
            s_loader_state.contract.expected_major_minor_version)) {
        return false;
    }

    return true;
}

bool master_loader_validate_contract(const master_loader_contract_t *contract)
{
    if (contract == NULL || contract->expected_major_minor_version[0] == '\0') {
        return false;
    }

    if (contract->max_json_depth <= 0 ||
        contract->max_json_depth > MASTER_LOADER_MAX_JSON_DEPTH) {
        return false;
    }

    if (contract->max_json_tokens == 0U ||
        contract->max_json_tokens > JSON_DEFAULT_MAX_TOKENS) {
        return false;
    }

    if (contract->max_payload_bytes < IO_FB_SMALL_BYTES ||
        contract->max_payload_bytes > MASTER_LOADER_MAX_PAYLOAD_BYTES) {
        return false;
    }

    return true;
}

static diag_result_t loader_probe_contract_bounds(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "master_loader",
        "io.loader.contract_bounds",
        "Live MasterLoader contract bounds verified within engine safety limits."
    };

    master_loader_ensure_contract_armed();
    if (!master_loader_validate_contract(&s_loader_state.contract)) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Active MasterLoader contract violates depth/token/payload invariants.";
    }
    return res;
}

static diag_result_t loader_probe_envelope_integrity(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "master_loader",
        "io.loader.envelope_integrity",
        "5-block envelope validation, syntax trap, and Block 3 span isolation healthy."
    };

    bool saved_pf = s_loader_state.in_preflight;
    s_loader_state.in_preflight = true;

    master_loader_t valid_l;
    master_loader_t bad_l;
    char probe_val[16];
    int bleed_val = 0;

    bool ok_valid = master_loader_load_from_memory(
        &valid_l, s_probe_valid_doc, strlen(s_probe_valid_doc));
    bool ok_schema = ok_valid && master_loader_validate_envelope(
        &valid_l, "tenlux.probe.master_loader.v1");
    bool ok_get = ok_schema && master_loader_get_string(
        &valid_l, NULL, "probe_key", probe_val, sizeof(probe_val)) &&
        strcmp(probe_val, "ok") == 0;
    bool ok_bleed = ok_get &&
        master_loader_get_int(&valid_l, NULL, "bleed_check", &bleed_val) &&
        bleed_val == 42 &&
        !master_loader_get_string(&valid_l, NULL, "outside_only", probe_val, sizeof(probe_val));
    bool bad_rejected = !master_loader_load_from_memory(
        &bad_l, s_probe_corrupt_doc, strlen(s_probe_corrupt_doc)) &&
        bad_l.status == LOADER_STATUS_ERR_SYNTAX_ERROR;

    s_loader_state.in_preflight = saved_pf;

    if (!ok_bleed || !bad_rejected) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "MasterLoader 5-block envelope isolation or syntax trap failed self-check.";
    }
    return res;
}

static diag_result_t loader_probe_rom_parity(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "master_loader",
        "io.loader.rom_parity",
        "Compiled .rodata Config ROM io/loader_contracts.json verified valid."
    };

    size_t rom_sz = 0U;
    const char *rom_json = io_fallback_find_rom_asset(MASTER_LOADER_CONTRACT_PATH, &rom_sz);
    master_loader_contract_t rom_c;

    if (rom_json == NULL || rom_sz == 0U ||
        !io_fallback_parse_loader_contract_json(rom_json, rom_sz, &rom_c) ||
        !master_loader_validate_contract(&rom_c)) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Compiled .rodata ROM io/loader_contracts.json failed parity check.";
    }
    return res;
}

void master_loader_register_debug_probes(void)
{
    if (!debug_is_active()) {
        return;
    }

    for (size_t i = 0U; i < (sizeof(s_loader_probes) / sizeof(s_loader_probes[0])); i++) {
        (void)debug_register_validation_probe(&s_loader_probes[i]);
    }
    s_loader_state.probes_registered = true;
}

bool master_loader_trigger_probe(const char *context_label, uint16_t error_code)
{
    master_loader_ensure_contract_armed();
    if (s_loader_state.in_preflight) {
        return false;
    }

    const char *probe_id = "io.loader.envelope_integrity";
    bool contract_corrupt = !master_loader_validate_contract(&s_loader_state.contract);
    if (contract_corrupt) {
        probe_id = "io.loader.contract_bounds";
        (void)loader_build_rom_contract(&s_loader_state.contract);
        s_loader_state.contract_from_data = false;
        (void)io_fallback_set_loader_contract(&s_loader_state.contract);
    } else if (error_code == IO_ERR_ROM_MISSING) {
        probe_id = "io.loader.rom_parity";
    }

    const char *reason = (context_label != NULL) ? context_label : "MasterLoader envelope anomaly";
    if (!debug_is_active()) {
        if (contract_corrupt) {
            (void)io_fallback_downgrade_safe_mode(reason, false);
        }
        return false;
    }

    master_loader_register_debug_probes();
    (void)io_fallback_downgrade_safe_mode(reason, false);

    if (!s_loader_state.contract.probe_await_on_corrupt) {
        return false;
    }

    return debug_trigger_probe(
        probe_id,
        MODULE_ID,
        reason,
        __FILE__,
        __LINE__,
        "master_loader_trigger_probe"
    );
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Tripwires
--------------------------------------------------------------------------------
*/

diag_result_t master_loader_preflight(void)
{
    diag_result_t res = {
        true,
        SEVERITY_PASS,
        "master_loader",
        "master_loader_preflight",
        "Zero-heap 5-block ingestion, span isolation, ROM parity, and contract checks verified."
    };

    /* Snapshot live state cleanly so --audit leaves caller telemetry intact */
    master_loader_runtime_state_t saved_state = s_loader_state;
    s_loader_state.in_preflight = true;
    master_loader_ensure_contract_armed();

    /* Step 1: Verify .rodata ROM contract parity & active contract bounds */
    diag_result_t rom_res = loader_probe_rom_parity();
    if (!rom_res.passed) {
        s_loader_state = saved_state;
        return rom_res;
    }

    diag_result_t bounds_res = loader_probe_contract_bounds();
    if (!bounds_res.passed) {
        s_loader_state = saved_state;
        return bounds_res;
    }

    /* Step 2: Run isolated synthetic envelope + cross-block bleed + syntax checks */
    diag_result_t env_res = loader_probe_envelope_integrity();
    if (!env_res.passed) {
        s_loader_state = saved_state;
        return env_res;
    }

    /* Step 3: Load & verify canonical math formulas via transient stack scratch (0 B .bss) */
    char file_buf[FS_BUFFER_MEDIUM_BYTES];
    master_loader_t loader;
    if (!master_loader_load(&loader, "config", "runtime/math/formulas.json",
                            file_buf, sizeof(file_buf))) {
        s_loader_state = saved_state;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Preflight failed loading runtime/math/formulas.json cleanly.";
        return res;
    }

    if (!master_loader_validate_envelope(&loader, "tenlux.data.math_formulas.v1")) {
        s_loader_state = saved_state;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "MasterLoader envelope/schema verification failed on formulas.json.";
        return res;
    }

    int mit_cap = 0;
    if (!master_loader_get_int(&loader, "physical_mitigation", "mitigation_cap_pct", &mit_cap) ||
        mit_cap <= 0 || mit_cap > 100) {
        s_loader_state = saved_state;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Scoped block_3_body extraction failed on mitigation_cap_pct.";
        return res;
    }

    /* Step 4: Verify negative guards & ROM URI ingestion path cleanly */
    master_loader_t bad_loader;
    if (master_loader_load(&bad_loader, "config", "nonexistent/bogus_manifest_404.json",
                           file_buf, sizeof(file_buf)) ||
        bad_loader.status != LOADER_STATUS_ERR_NOT_FOUND) {
        s_loader_state = saved_state;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Negative missing-file trap failed returning LOADER_STATUS_ERR_NOT_FOUND.";
        return res;
    }

    master_loader_t rom_loader;
    if (!master_loader_load_from_path(&rom_loader, MASTER_LOADER_ROM_CONTRACT_URI,
                                      file_buf, sizeof(file_buf)) ||
        !rom_loader.header.loaded_from_rom ||
        !master_loader_validate_schema(&rom_loader, "tenlux.data.io_loader_contracts.v1")) {
        s_loader_state = saved_state;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Compiled .rodata ROM ingestion failed on rom://io/loader_contracts.json.";
        return res;
    }

    s_loader_state = saved_state;
    return res;
}

bool master_loader_audit(void)
{
    return master_loader_preflight().passed;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( Orchestrators, Inspection Cards, Runtime Telemetry & Lifecycle )
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: High-Level Orchestrators & Telemetry
--------------------------------------------------------------------------------
*/

bool master_loader_execute_inspect(const char *domain,
                                   const char *rel_path,
                                   loader_header_t *out_header)
{
    if (rel_path == NULL || out_header == NULL) {
        return false;
    }

    memset(out_header, 0, sizeof(*out_header));

    /* Transient stack scratch handles configs up to 32 KB with zero .bss retention */
    char inspect_buf[FS_BUFFER_LARGE_BYTES];
    master_loader_t loader;
    if (!master_loader_load(&loader, domain, rel_path, inspect_buf, sizeof(inspect_buf))) {
        return false;
    }

    *out_header = loader.header;
    return true;
}

size_t master_loader_execute_dump_card(const loader_header_t *header,
                                       char *out_buf,
                                       size_t buf_size)
{
    if (out_buf == NULL || buf_size == 0U) {
        return 0U;
    }

    if (header == NULL) {
        int err_n = snprintf(
            out_buf, buf_size,
            "+--- [ MASTER LOADER CARD ] -------------------------------+\n"
            "| STATUS : NULL HEADER REFERENCE                           |\n"
            "+----------------------------------------------------------+\n"
        );
        return (err_n > 0) ? (((size_t)err_n < buf_size) ? (size_t)err_n : buf_size - 1U) : 0U;
    }

    const char *source_label = header->loaded_from_rom ? "ROM (.rodata)" : "Disk / Memory";
    int n = snprintf(
        out_buf, buf_size,
        "+--- [ MASTER LOADER ENVELOPE CARD ] ----------------------+\n"
        "| File   : %-47s |\n"
        "| ID     : %-47s |\n"
        "| Ver    : %-15s   Schema: %-21s |\n"
        "| Source : %-16s  5-Block: %-18s |\n"
        "+----------------------------------------------------------+\n",
        header->file[0]      ? header->file      : "(unknown)",
        header->id[0]        ? header->id        : "(unknown)",
        header->version[0]   ? header->version   : "(none)",
        header->schema_id[0] ? header->schema_id : "(none)",
        source_label,
        header->is_5block    ? "VALIDATED"       : "UNVERIFIED"
    );

    if (n < 0) {
        out_buf[0] = '\0';
        return 0U;
    }
    return ((size_t)n < buf_size) ? (size_t)n : (buf_size - 1U);
}

bool master_loader_execute_load_verified(const char *domain,
                                         const char *rel_path,
                                         const char *expected_schema_id,
                                         char *buf,
                                         size_t buf_size,
                                         master_loader_t *out_loader)
{
    if (out_loader == NULL) {
        return false;
    }

    master_loader_ensure_contract_armed();

    if (!master_loader_load(out_loader, domain, rel_path, buf, buf_size)) {
        return false;
    }

    if (expected_schema_id != NULL && expected_schema_id[0] != '\0') {
        if (s_loader_state.contract.enforce_schema_id &&
            !master_loader_validate_schema(out_loader, expected_schema_id)) {
            out_loader->status = LOADER_STATUS_ERR_SCHEMA_MISMATCH;
            if (!s_loader_state.in_preflight) {
                s_loader_state.schema_mismatches++;
            }
            LOADER_EMIT_LOG(LOG_ERROR, -0.60f,
                            "Schema mismatch on '%s': expected '%s', got '%s'",
                            out_loader->resolved_path,
                            expected_schema_id,
                            out_loader->header.schema_id[0]
                                ? out_loader->header.schema_id
                                : "(none)");
            io_fallback_record_fault(MASTER_LOADER_MODULE_ID,
                                     "master_loader_execute_load_verified",
                                     "Block 2 schema_id mismatch on verified load",
                                     IO_ERR_JSON_KEY_MISSING);
            return false;
        }
    }

    if (s_loader_state.contract.require_5block_envelope &&
        !loader_has_valid_body(out_loader)) {
        out_loader->status = LOADER_STATUS_ERR_BODY_MISSING;
        if (!s_loader_state.in_preflight) {
            s_loader_state.body_missing_errors++;
        }
        return false;
    }

    /* Enforce semver prefix cleanly whenever dataset carries a Block 1 version string */
    if (s_loader_state.contract.require_monotonic_version &&
        out_loader->header.version[0] != '\0' &&
        !loader_version_matches_prefix(
            out_loader->header.version,
            s_loader_state.contract.expected_major_minor_version)) {
        out_loader->status = LOADER_STATUS_ERR_VERSION_MISMATCH;
        if (!s_loader_state.in_preflight) {
            s_loader_state.version_mismatches++;
        }
        LOADER_EMIT_LOG(LOG_ERROR, -0.55f,
                        "Version contract breach on '%s': got '%s', expected '%s.*'",
                        out_loader->resolved_path,
                        out_loader->header.version,
                        s_loader_state.contract.expected_major_minor_version);
        io_fallback_record_fault(MASTER_LOADER_MODULE_ID,
                                 "master_loader_execute_load_verified",
                                 "Dataset semantic version incompatible with loader contract",
                                 IO_ERR_JSON_KEY_MISSING);
        return false;
    }

    return true;
}

bool master_loader_get_stats(master_loader_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }

    master_loader_ensure_contract_armed();
    memset(out_stats, 0, sizeof(*out_stats));

    out_stats->initialized = s_loader_state.initialized;
    out_stats->contract_from_data = s_loader_state.contract_from_data;
    out_stats->probes_registered = s_loader_state.probes_registered;
    out_stats->require_block_1_metadata = s_loader_state.contract.require_block_1_metadata;
    out_stats->require_5block_envelope = s_loader_state.contract.require_5block_envelope;
    out_stats->enforce_schema_id = s_loader_state.contract.enforce_schema_id;
    out_stats->require_monotonic_version = s_loader_state.contract.require_monotonic_version;
    out_stats->allow_legacy_block2_types = s_loader_state.contract.allow_legacy_block2_types;
    out_stats->probe_await_on_corrupt = s_loader_state.contract.probe_await_on_corrupt;
    out_stats->max_json_depth = s_loader_state.contract.max_json_depth;
    out_stats->max_json_tokens = s_loader_state.contract.max_json_tokens;
    out_stats->max_payload_bytes = s_loader_state.contract.max_payload_bytes;
    out_stats->total_loads = s_loader_state.total_loads;
    out_stats->disk_loads = s_loader_state.disk_loads;
    out_stats->rom_loads = s_loader_state.rom_loads;
    out_stats->memory_loads = s_loader_state.memory_loads;
    out_stats->body_queries = s_loader_state.body_queries;
    out_stats->syntax_errors = s_loader_state.syntax_errors;
    out_stats->schema_mismatches = s_loader_state.schema_mismatches;
    out_stats->version_mismatches = s_loader_state.version_mismatches;
    out_stats->body_missing_errors = s_loader_state.body_missing_errors;
    out_stats->not_found_errors = s_loader_state.not_found_errors;
    out_stats->overflow_errors = s_loader_state.overflow_errors;
    out_stats->corrupt_contract_downgrades = s_loader_state.corrupt_contract_downgrades;
    str_safe_copy(out_stats->expected_major_minor_version,
                  s_loader_state.contract.expected_major_minor_version,
                  sizeof(out_stats->expected_major_minor_version));
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Control
--------------------------------------------------------------------------------
*/

void master_loader_init(void)
{
    if (!io_fallback_is_booted()) {
        (void)io_fallback_boot(NULL);
    }
    s_loader_state.initialized = true;
    s_loader_state.probes_registered = false;

    bool disk_contract_ok = master_loader_load_contract(NULL);
    master_loader_register_debug_probes();

    LOADER_EMIT_LOG(LOG_INFO, 0.30f,
                    "MasterLoader initialized (contract=%s, ver='%s.*', depth<=%d, await=%s)",
                    disk_contract_ok ? "DISK" : "ROM_SAFE_MODE",
                    s_loader_state.contract.expected_major_minor_version,
                    s_loader_state.contract.max_json_depth,
                    s_loader_state.contract.probe_await_on_corrupt ? "true" : "false");
}

void master_loader_shutdown(void)
{
    LOADER_EMIT_LOG(LOG_INFO, 0.0f,
                    "MasterLoader shutting down cleanly (loads=%u [disk=%u, rom=%u, mem=%u], queries=%u)",
                    s_loader_state.total_loads,
                    s_loader_state.disk_loads,
                    s_loader_state.rom_loads,
                    s_loader_state.memory_loads,
                    s_loader_state.body_queries);

    s_loader_state.initialized = false;
    s_loader_state.probes_registered = false;
}
