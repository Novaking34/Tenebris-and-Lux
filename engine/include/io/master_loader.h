/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/io/master_loader.h
@id            cog.master_loader
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Master Data & Manifest Loader (Universal 5-Block JSON Envelopes)
@role          Public Contract & Seam: 5-block JSON envelope orchestrator, schema
               contract verifier, and span-isolated block_3_body getter API.
@maturity      Rev 5 Baseline | Contract policy hydration, span-isolated queries & probe escalation live.
@chassis       zero-heap | Caller-owned non-owning master_loader_t slices + caller storage buffer.
@scale         O(N) envelope slice over max_payload_bytes (64 KiB); O(M) span-isolated queries in block_3_body.
@seams         in: engine/config/io/loader_contracts.json | uses: cog.path_finder, cog.fs_utils, cog.json_utils | probes: io.loader.* -> DEBUG_STEP_AWAIT | dag: core.io.master_loader
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Non-owning zero-copy block spans point strictly inside validated caller buffer or .rodata ROM.
               3. All master_loader_get_*() and find_array() calls inspect solely [body_start, body_start + body_len).
               4. Damaged envelopes or contracts reject cleanly, downgrade to ROM defaults, and fire io.loader.* probes.

PURPOSE & ARCHITECTURAL ROLE:
  Declares the top-level Layer 0 JSON envelope ingestion contract unifying VFS
  lookup (`path_finder`), bounded reads (`fs_utils`), span scanning (`json_utils`),
  and contract enforcement (`loader_contracts.json`) for every engine & game load.

  ### For Game Designers & Non-Programmers:
  Acts as the single quality checkpoint for every JSON asset before gameplay sees
  it: malformed files, broken versions, or missing `block_3_body` sections fail
  safely instead of corrupting stats or game state.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Caller supplies storage buffer and owns `master_loader_t`;
    multiple concurrent loaders on distinct buffers are reentrant except counters.
  - Extension Seam: Enforce stricter envelope rules, allowed schema prefixes, or
    legacy block compatibility purely via `loader_contracts.json`.
  - Explicit Non-Goal: Does not allocate or cache parsed C domain structs across
    reloads (each subsystem hydrates its own typed domain table cleanly).
================================================================================
*/

#ifndef CORNERSTONE_IO_MASTER_LOADER_H
#define CORNERSTONE_IO_MASTER_LOADER_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Public Contract Limits, Fallback Link, Ontology & O(1) Pre-Conditions )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.master_loader
Mission:       Declare 5-block envelope caps, downward I/O ladder, handle ontology, and O(1) guards.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "io/io_fallback.h"
#include "io/path_finder.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Structural Constants & Buffer Bounds
--------------------------------------------------------------------------------
Fixed capacities governing envelope header metadata and contract bounds.
--------------------------------------------------------------------------------
*/

/** Maximum length for Block 1/2 header identifier strings (`.rodata`/stack bounded). */
#define LOADER_MAX_TOKEN_LEN            64

/** Maximum length for provenance file strings stored inside `loader_header_t`. */
#define LOADER_MAX_PATH_LEN             256

/** Canonical VFS config relative path for MasterLoader schema/envelope contracts. */
#define MASTER_LOADER_CONTRACT_PATH     "io/loader_contracts.json"

/** Compiled `.rodata` Config ROM fallback URI for MasterLoader contracts. */
#define MASTER_LOADER_ROM_CONTRACT_URI  "rom://io/loader_contracts.json"

/** Maximum structural JSON nesting depth accepted by MasterLoader contract bounds. */
#define MASTER_LOADER_MAX_JSON_DEPTH    32

/** Maximum single config JSON payload ceiling aligned with `FS_BUFFER_MAX_CONFIG`. */
#define MASTER_LOADER_MAX_PAYLOAD_BYTES FS_BUFFER_MAX_CONFIG

static_assert(
    MASTER_LOADER_MAX_JSON_DEPTH == JSON_MAX_NESTING &&
    MASTER_LOADER_MAX_PAYLOAD_BYTES == FS_BUFFER_MAX_CONFIG,
    "MasterLoader contract bounds must align cleanly with JSON and FS limits"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest Binding & Pre-Flight Hooks
--------------------------------------------------------------------------------
Binds the Master Loader into the boot-time DAG registry cleanly below Layer 1/2.
--------------------------------------------------------------------------------
*/

/** Canonical subsystem identity aligned across manifests, telemetry, and probes. */
#define MASTER_LOADER_MODULE_ID         "core.io.master_loader"

/** Forward-declared module manifest exported cleanly by `master_loader.c`. */
extern const module_metadata_t master_loader_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Ontology & Typed Definitions
--------------------------------------------------------------------------------
Tagged types for ingestion status codes, 5-block header provenance, non-owning
loader handles, hydrated runtime contracts, and `.bss` telemetry snapshots.
--------------------------------------------------------------------------------
*/

/**
 * @enum loader_status
 * @brief Deterministic ingestion status codes returned by Master Loader cogs.
 *
 * ### For Game Designers & Non-Programmers:
 * Gives diagnostics a clear reason whenever a data file fails to load: missing
 * file, buffer too small, wrong schema version, broken JSON syntax, or absent
 * `block_3_body` payload.
 *
 * ### For Systems Programmers & Engine Architects:
 * Preserves legacy `0x0201..0x0206` numeric values for Layer 0 binary/test
 * stability while `master_loader_status_to_io_error()` maps cleanly onto
 * `engine/config/diagnostics/taxonomy/error_codes.json` (`0x0501..0x0603`).
 */
typedef enum loader_status {
    LOADER_STATUS_OK                   = 0,      /**< Operation succeeded cleanly. */
    LOADER_STATUS_ERR_NOT_FOUND        = 0x0201, /**< File absent across VFS & ROM (`IO_ERR_FILE_NOT_FOUND`). */
    LOADER_STATUS_ERR_BUFFER_OVERFLOW  = 0x0202, /**< Payload exceeds buffer or contract ceiling (`IO_ERR_TRUNCATED_READ`). */
    LOADER_STATUS_ERR_SCHEMA_MISMATCH  = 0x0203, /**< Block 2 `schema_id` failed verification contract. */
    LOADER_STATUS_ERR_SYNTAX_ERROR     = 0x0204, /**< Malformed JSON tokens/escapes/containers (`IO_ERR_JSON_SYNTAX`). */
    LOADER_STATUS_ERR_NULL_PARAM       = 0x0205, /**< Null pointer or zero-capacity contract breach. */
    LOADER_STATUS_ERR_BODY_MISSING     = 0x0206, /**< Required `block_3_body` missing from 5-block envelope. */
    LOADER_STATUS_ERR_VERSION_MISMATCH = 0x0207, /**< Block 1 `version` violates active semver prefix contract. */
    LOADER_STATUS_ERR_CONTRACT_BREACH  = 0x0208  /**< Structural depth or `block_1_metadata` contract violated (`IO_ERR_JSON_DEPTH_EXCEEDED`). */
} loader_status_t;

/**
 * @struct loader_header
 * @brief Extracted OmniCode Block 1 & Block 2 provenance and contract metadata.
 *
 * ### For Game Designers & Non-Programmers:
 * Captures the "title card" at the top of every data file (project name, file
 * ID, version, and data schema tag) plus whether it came from disk or Safe-Mode ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed-layout stack/value struct (zero heap). Existing member order preserved
 * across `project..is_5block`; provenance flags `has_block_1..loaded_from_rom`
 * are zero-initialized on every load.
 */
typedef struct loader_header {
    char project[LOADER_MAX_TOKEN_LEN];  /**< Block 1 project name (`"Tenebris and Lux"`). */
    char file[LOADER_MAX_PATH_LEN];      /**< Block 1 relative file provenance string. */
    char id[LOADER_MAX_TOKEN_LEN];       /**< Block 1 unique dataset or config ID. */
    char layer[LOADER_MAX_TOKEN_LEN];    /**< Block 1 architectural stratum string. */
    char version[32];                    /**< Block 1 semantic version string (e.g. `"0.1.1-dev"`). */
    char schema_id[LOADER_MAX_TOKEN_LEN];/**< Block 2 schema contract ID (`"tenlux.data.*"`). */
    bool is_5block;                      /**< True if 5-block envelope body & metadata checks satisfied. */
    bool has_block_1;                    /**< True if `"block_1_metadata"` object parsed cleanly. */
    bool has_block_2;                    /**< True if `"block_2_opening"` (or legacy `block_2_types`) parsed cleanly. */
    bool loaded_from_rom;                /**< True when payload originated from a `rom://` `.rodata` URI. */
} loader_header_t;

/**
 * @struct master_loader
 * @brief Bounded, non-owning JSON ingestion context with span-isolated body view.
 *
 * ### For Game Designers & Non-Programmers:
 * Holds the open "bookmarked" view of one loaded JSON file so gameplay loaders
 * can pull settings, formulas, and lore tables out of Block 3 cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Non-owning cursor over caller `raw_buf`. Callers must keep backing storage
 * valid while calling `master_loader_get_*()` / `master_loader_find_array()`.
 */
typedef struct master_loader {
    const char     *raw_buf;                             /**< Non-owning pointer to JSON source bytes. */
    size_t          raw_len;                             /**< Byte length of JSON stream. */
    loader_header_t header;                              /**< Parsed Block 1/2 provenance metadata. */
    const char     *body_start;                          /**< Non-owning start pointer of `block_3_body` object. */
    size_t          body_len;                            /**< Exact byte span of `block_3_body` object. */
    loader_status_t status;                              /**< Status code of most recent load/verify operation. */
    char            resolved_path[PATH_FINDER_MAX_PATH]; /**< VFS or explicit path resolved during load. */
    bool            loaded_from_data;                    /**< True when loaded from disk data; false for ROM or memory. */
} master_loader_t;

/**
 * @typedef master_loader_contract_t
 * @brief Single-source MasterLoader verification policy hydrated from `loader_contracts.json`.
 *
 * ### For Game Designers & Non-Programmers:
 * Controls strictness rules such as mandatory 5-block headers, version prefix checks,
 * and whether malformed data files pause in debugger inspection.
 *
 * ### For Systems Programmers & Engine Architects:
 * Direct alias of `io_fb_loader_contract_t` preventing duplicate contract structs
 * across BIOS (`io_fallback.h`) and primary `master_loader.h`.
 */
typedef io_fb_loader_contract_t master_loader_contract_t;

/**
 * @struct master_loader_stats
 * @brief Read-only diagnostic snapshot of MasterLoader telemetry and active contract flags.
 *
 * ### For Game Designers & Non-Programmers:
 * Shows how many files were loaded from disk vs. built-in Safe-Mode ROM and
 * surfaces any broken JSON syntax, missing blocks, or schema mismatches encountered.
 *
 * ### For Systems Programmers & Engine Architects:
 * Value snapshot copied out of static `.bss` storage by `master_loader_get_stats()`.
 */
typedef struct master_loader_stats {
    bool     initialized;                    /**< True after `master_loader_init()` completed. */
    bool     contract_from_data;             /**< True only when `loader_contracts.json` parsed from disk. */
    bool     probes_registered;              /**< True after `io.loader.*` probes bound in debugger. */
    bool     require_block_1_metadata;       /**< Live `require_block_1_metadata` contract toggle. */
    bool     require_5block_envelope;        /**< Live `require_5block_envelope` contract toggle. */
    bool     enforce_schema_id;              /**< Live `enforce_schema_id` contract toggle. */
    bool     require_monotonic_version;      /**< Live `require_monotonic_version` contract toggle. */
    bool     allow_legacy_block2_types;      /**< Live `allow_legacy_block2_types` compatibility toggle. */
    bool     probe_await_on_corrupt;         /**< Live Safe-Mode `DEBUG_STEP_AWAIT` trap toggle. */
    int      max_json_depth;                 /**< Active bracket nesting limit (<= 32). */
    size_t   max_json_tokens;                /**< Active token scan limit (<= 1024). */
    size_t   max_payload_bytes;              /**< Active JSON byte payload ceiling (<= 65536). */
    uint32_t total_loads;                    /**< Lifetime ingestion calls (`disk + rom + memory`). */
    uint32_t disk_loads;                     /**< Loads satisfied cleanly from disk storage. */
    uint32_t rom_loads;                      /**< Loads served cleanly via `rom://` `.rodata` vault. */
    uint32_t memory_loads;                   /**< Direct caller in-memory envelope parses. */
    uint32_t body_queries;                   /**< Total `block_3_body` extraction/lookup queries. */
    uint32_t syntax_errors;                  /**< Rejected malformed JSON payloads (`0x0204`). */
    uint32_t schema_mismatches;              /**< Schema ID verification mismatches (`0x0203`). */
    uint32_t version_mismatches;             /**< Version prefix mismatches (`0x0207`). */
    uint32_t body_missing_errors;            /**< Envelopes lacking `block_3_body` (`0x0206`). */
    uint32_t not_found_errors;               /**< VFS/path misses (`0x0201`). */
    uint32_t overflow_errors;                /**< Buffer or payload size limit breaches (`0x0202`). */
    uint32_t corrupt_contract_downgrades;    /**< Corrupt disk/caller contracts reverted to ROM Safe Mode. */
    char     expected_major_minor_version[16]; /**< Expected version prefix from `loader_contracts.json`. */
} master_loader_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines
--------------------------------------------------------------------------------
Zero-heap pure predicates and taxonomy adapters evaluated in O(1).
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns true if a `loader_status_t` represents clean completion.
 * @param[in] status Loader status enum value.
 * @return true iff `status == LOADER_STATUS_OK`.
 */
static inline bool loader_status_is_ok(loader_status_t status)
{
    return status == LOADER_STATUS_OK;
}

/**
 * @brief Returns true if a `master_loader_t` handle holds an active parsed buffer.
 * @param[in] loader Pointer to `master_loader_t` context.
 * @return true if non-NULL, `status == LOADER_STATUS_OK`, and `raw_buf != NULL`.
 */
static inline bool loader_is_ready(const master_loader_t *loader)
{
    return (loader != NULL) && (loader->status == LOADER_STATUS_OK) && (loader->raw_buf != NULL);
}

/**
 * @brief Returns true if extracted header provenance marks a 5-block envelope.
 * @param[in] hdr Pointer to `loader_header_t`.
 * @return true if non-NULL and `hdr->is_5block == true`.
 */
static inline bool loader_header_is_5block(const loader_header_t *hdr)
{
    return (hdr != NULL) && hdr->is_5block;
}

/**
 * @brief Returns true if `loader` has an isolated non-empty `block_3_body` span.
 * @param[in] loader Pointer to `master_loader_t` context.
 * @return true when ready with `body_start != NULL && body_len >= 2U`.
 */
static inline bool loader_has_valid_body(const master_loader_t *loader)
{
    return loader_is_ready(loader) && (loader->body_start != NULL) && (loader->body_len >= 2U);
}

/**
 * @brief Maps MasterLoader status code onto `error_codes.json` (`0x05xx`/`0x06xx`).
 * @param[in] status Loader status enum value.
 * @return Standardized taxonomy error code, or `0U` when `LOADER_STATUS_OK`.
 */
static inline uint16_t master_loader_status_to_io_error(loader_status_t status)
{
    switch (status) {
        case LOADER_STATUS_OK:                   return 0U;
        case LOADER_STATUS_ERR_NOT_FOUND:        return IO_ERR_FILE_NOT_FOUND;
        case LOADER_STATUS_ERR_BUFFER_OVERFLOW:  return IO_ERR_TRUNCATED_READ;
        case LOADER_STATUS_ERR_SCHEMA_MISMATCH:  return IO_ERR_JSON_KEY_MISSING;
        case LOADER_STATUS_ERR_SYNTAX_ERROR:     return IO_ERR_JSON_SYNTAX;
        case LOADER_STATUS_ERR_NULL_PARAM:       return IO_ERR_READ_FAILURE;
        case LOADER_STATUS_ERR_BODY_MISSING:     return IO_ERR_JSON_KEY_MISSING;
        case LOADER_STATUS_ERR_VERSION_MISMATCH: return IO_ERR_JSON_KEY_MISSING;
        case LOADER_STATUS_ERR_CONTRACT_BREACH:  return IO_ERR_JSON_DEPTH_EXCEEDED;
        default:                                 return IO_ERR_JSON_SYNTAX;
    }
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Public Operational Cogs, State Accessors & Domain APIs )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0 (Core Utilities)
Module:        cog.master_loader
Mission:       5-Block Envelope Ingestion, Scoped Body Accessors & Contracts
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Ingestion Cogs
Mission: Load and verify 5-block JSON envelopes from VFS domains, explicit disk
         or `rom://` paths, and in-memory buffers with `ALLOC_ZERO_HEAP`.

  ### For Game Designers & Non-Programmers:
  Provides the safe entry points for loading game configs and manifests, whether
  they come from mod folders, engine defaults, or built-in ROM backups.

  ### For Systems Programmers & Engine Architects:
  Unifies `path_finder_resolve()`, bounded `fs_read_file_bounded()`, structural
  JSON checks, and Block 1/2/3 boundary slicing into caller-owned `master_loader_t`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Resolves a VFS domain asset (`disk` -> `.rodata` ROM) and ingests its envelope.
 *
 * ### For Game Designers & Non-Programmers:
 * Primary way subsystems load data by logical folder (`"config"`, `"world"`,
 * `"mechanics"`) and relative file path without caring where the game is installed
 * or whether Safe-Mode ROM backup data is active.
 *
 * ### For Systems Programmers & Engine Architects:
 * Calls `path_finder_resolve()` -> `master_loader_load_from_path()`. Detects buffer
 * size exhaustion cleanly, parses Block 1/2 provenance and `block_3_body` span,
 * and marks `loader->loaded_from_data` (`false` when served from `rom://`).
 * @triggers `io.loader.envelope_integrity`, `LOG_CAT_IO` on syntax/contract fault.
 *
 * @param[out] loader   Loader context populated by this call (`memset` on entry).
 * @param[in]  domain   Logical VFS domain (`"config"`, `"mechanics"`, `"world"`, etc.).
 * @param[in]  rel_path Relative file path within the target domain.
 * @param[out] buf      Caller-supplied byte buffer receiving the loaded JSON stream.
 * @param[in]  buf_size Byte capacity of `buf` (must exceed file size for NUL byte).
 * @return true on clean load & envelope parse; false with `loader->status` set otherwise.
 */
bool master_loader_load(master_loader_t *loader,
                        const char *domain,
                        const char *rel_path,
                        char *buf,
                        size_t buf_size);

/**
 * @brief Reads and parses a JSON file from an explicit POSIX path or `"rom://..."` URI.
 *
 * ### For Game Designers & Non-Programmers:
 * Opens an already-resolved path or emergency ROM address and checks its header
 * and data body before handing values to game systems.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks file size (`fs_get_file_size()`) against `buf_size` and active contract
 * `max_payload_bytes`, performs bounded read via `fs_read_file_bounded()`, and
 * delegates envelope validation to the shared internal parser.
 * @triggers `io.loader.envelope_integrity`, `LOG_CAT_IO` on syntax/contract fault.
 *
 * @param[out] loader   Loader context populated by this call.
 * @param[in]  path     Resolved host filesystem path or `"rom://..."` asset URI.
 * @param[out] buf      Caller buffer receiving the NUL-terminated JSON bytes.
 * @param[in]  buf_size Capacity in bytes of `buf`.
 * @return true on valid load & envelope extraction; false otherwise.
 */
bool master_loader_load_from_path(master_loader_t *loader,
                                  const char *path,
                                  char *buf,
                                  size_t buf_size);

/**
 * @brief Parses and verifies a 5-block JSON envelope directly from caller memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets built-in ROM presets and unit test JSON snippets pass through the exact same
 * header and schema safety checks as files loaded from disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-copy non-owning parser (`ALLOC_ZERO_HEAP`, `CONCURRENCY_REENTRANT` read view).
 * Validates structural syntax & nesting depth via `json_validate_span()`, extracts
 * Block 1 (`project..version`) and Block 2 (`schema_id` inside `block_2_opening` or
 * allowed `block_2_types`), and binds `[body_start, body_start + body_len)`.
 * @triggers `io.loader.envelope_integrity`, `LOG_CAT_IO` on envelope fault.
 *
 * @param[out] loader    Loader context populated with zeroed `resolved_path` and spans.
 * @param[in]  json_text Immutable JSON buffer (`raw_buf` binds here; must outlive queries).
 * @param[in]  json_len  Explicit byte length (or `0` to measure with `strlen()`).
 * @return true on clean parse matching active contract; false with diagnostic status otherwise.
 */
bool master_loader_load_from_memory(master_loader_t *loader,
                                    const char *json_text,
                                    size_t json_len);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Scoped `block_3_body` Accessors
Mission: Restrict all payload getters and array locators strictly inside the
         verified `[body_start, body_start + body_len)` span (or named `sub_obj`).

  ### For Game Designers & Non-Programmers:
  Reads gameplay properties cleanly from inside `block_3_body` only, preventing
  file headers or audit sections from ever colliding with gameplay field names.

  ### For Systems Programmers & Engine Architects:
  Delegates scope isolation and typed extraction exclusively to span-bounded
  `json_*_span` APIs with unconditional NUL termination on extracted strings.
--------------------------------------------------------------------------------
*/

/**
 * @brief Extracts a string value strictly within `block_3_body` (or named sub-object).
 *
 * ### For Game Designers & Non-Programmers:
 * Safely reads text properties (character names, formulas, IDs) from the data body
 * without accidentally matching header text outside Block 3.
 *
 * ### For Systems Programmers & Engine Architects:
 * Uses span-bounded `json_find_object_span()` + `json_get_string_span()` bounded to
 * `loader->body_len`. Unconditionally NUL-terminates `out_val` when `out_size > 0`.
 * @triggers `io.json.depth_bounds` if scan breaches `max_json_depth`.
 *
 * @param[in]  loader   Ready `master_loader_t` instance.
 * @param[in]  sub_obj  Optional sub-object key inside `block_3_body` (or NULL for root body).
 * @param[in]  key      Target property key inside resolved body/sub-object span.
 * @param[out] out_val  Caller string buffer.
 * @param[in]  out_size Capacity of `out_val` in bytes.
 * @return true on clean extraction; false if missing, wrong type, or invalid params.
 */
bool master_loader_get_string(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              char *out_val,
                              size_t out_size);

/**
 * @brief Extracts a signed integer strictly within `block_3_body` (or named sub-object).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads whole-number gameplay values (HP caps, percentages, cooldown turns) cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Span-bounded extraction via `json_get_int_span()` with overflow/syntax guards.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Ready `master_loader_t` instance.
 * @param[in]  sub_obj Optional sub-object key inside `block_3_body` (or NULL).
 * @param[in]  key     Target integer key.
 * @param[out] out_val Destination `int`.
 * @return true on valid integer parse; false otherwise.
 */
bool master_loader_get_int(const master_loader_t *loader,
                           const char *sub_obj,
                           const char *key,
                           int *out_val);

/**
 * @brief Extracts a non-negative `size_t` strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads positive capacity counts (ring buffer slots, item stack limits) cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Rejects negative JSON integers safely via `json_get_size_t_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Ready `master_loader_t` instance.
 * @param[in]  sub_obj Optional sub-object key inside `block_3_body` (or NULL).
 * @param[in]  key     Target non-negative integer key.
 * @param[out] out_val Destination `size_t`.
 * @return true on valid unsigned parse; false otherwise.
 */
bool master_loader_get_size_t(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *key,
                              size_t *out_val);

/**
 * @brief Extracts a floating-point number strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads decimal multipliers (damage ratios, audio volume dB, difficulty scales).
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded extraction via `json_get_float_span()`, rejecting NaN/Inf tokens cleanly.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Ready `master_loader_t` instance.
 * @param[in]  sub_obj Optional sub-object key inside `block_3_body` (or NULL).
 * @param[in]  key     Target float key.
 * @param[out] out_val Destination `float`.
 * @return true on valid finite float parse; false otherwise.
 */
bool master_loader_get_float(const master_loader_t *loader,
                             const char *sub_obj,
                             const char *key,
                             float *out_val);

/**
 * @brief Extracts a JSON boolean strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads true/false feature switches from data manifests cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Accepts strict JSON `true`/`false` literals only via `json_get_bool_span()`.
 * @triggers None on expected miss or parse rejection.
 *
 * @param[in]  loader  Ready `master_loader_t` instance.
 * @param[in]  sub_obj Optional sub-object key inside `block_3_body` (or NULL).
 * @param[in]  key     Target boolean key.
 * @param[out] out_val Destination `bool`.
 * @return true on valid boolean literal parse; false otherwise.
 */
bool master_loader_get_bool(const master_loader_t *loader,
                            const char *sub_obj,
                            const char *key,
                            bool *out_val);

/**
 * @brief Locates a JSON array span strictly within `block_3_body` (or `sub_obj`).
 *
 * ### For Game Designers & Non-Programmers:
 * Finds lists inside a file (such as enemy waves, settings menus, or item tables)
 * and counts how many entries exist.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-copy slice via `json_find_array_span()` + `json_array_get_count()`.
 * @triggers `io.json.depth_bounds` if array bracket scan exceeds `max_json_depth`.
 *
 * @param[in]  loader    Ready `master_loader_t` instance.
 * @param[in]  sub_obj   Optional sub-object key inside `block_3_body` (or NULL).
 * @param[in]  array_key Target JSON array key.
 * @param[out] out_start Pointer to `[` delimiter inside loaded JSON buffer.
 * @param[out] out_len   Exact byte length from `[` through matching `]`.
 * @param[out] out_count Optional element count (may be NULL).
 * @return true if array found and well-formed; false otherwise.
 */
bool master_loader_find_array(const master_loader_t *loader,
                              const char *sub_obj,
                              const char *array_key,
                              const char **out_start,
                              size_t *out_len,
                              size_t *out_count);

/**
 * @brief Copies the `index`-th object element out of a located JSON array span.
 *
 * ### For Game Designers & Non-Programmers:
 * Steps through list entries one item at a time so loaders can import catalogs cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded copy via `json_array_get_object()` guaranteeing NUL-termination.
 * @triggers `io.json.depth_bounds` if element object exceeds `max_json_depth`.
 *
 * @param[in]  arr_start Pointer to `[` array boundary from `master_loader_find_array()`.
 * @param[in]  arr_len   Exact byte span of the array.
 * @param[in]  index     Zero-based element index.
 * @param[out] out_obj   Caller destination buffer for the extracted JSON object slice.
 * @param[in]  out_size  Byte capacity of `out_obj`.
 * @return true on valid extraction; false on out-of-bounds index or overflow.
 */
bool master_loader_get_array_object(const char *arr_start,
                                    size_t arr_len,
                                    size_t index,
                                    char *out_obj,
                                    size_t out_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Data-Driven Contract Hydration & Status Formatters
Mission: Synchronize live verification contracts with `loader_contracts.json`
         or `.rodata` Config ROM fallback and format loader statuses cleanly.

  ### For Game Designers & Non-Programmers:
  Loads structural strictness rules and version expectations from project settings,
  dropping safely back to ROM defaults and readable error names on any corruption.

  ### For Systems Programmers & Engine Architects:
  Hydrates `master_loader_contract_t`, triggers `io.loader.contract_bounds` when
  corrupt contracts force ROM downgrade, and maps enum codes to static mnemonics.
--------------------------------------------------------------------------------
*/

/**
 * @brief Hydrates MasterLoader verification contract from disk JSON or `.rodata` ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads `engine/config/io/loader_contracts.json` so file strictness, depth caps,
 * and schema rules are driven by project config instead of magic constants.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves `rel_or_abs_path` (default `MASTER_LOADER_CONTRACT_PATH`), validates
 * structural syntax/envelope + contract bounds, and syncs both `s_loader_state`
 * and `io_fallback_set_loader_contract()`. Corrupt disk files trigger Safe-Mode
 * ROM downgrade (`loaded_from_data = false`) + `io.loader.contract_bounds` probe.
 * @triggers `io.loader.contract_bounds`, `LOG_CAT_IO`.
 *
 * @param[in] rel_or_abs_path Optional override path (NULL uses `"io/loader_contracts.json"`).
 * @return true if hydrated cleanly from disk data; false if Safe-Mode ROM active.
 */
bool master_loader_load_contract(const char *rel_or_abs_path);

/**
 * @brief Copies live hydrated `master_loader_contract_t` into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports which file verification rules and limits are currently enforced.
 *
 * ### For Systems Programmers & Engine Architects:
 * Lazy-arms from Tier-0 BIOS if uninitialized and performs O(1) value copy.
 * @triggers None.
 *
 * @param[out] out_contract Destination contract descriptor.
 * @return true on valid copy; false on NULL output pointer.
 */
bool master_loader_get_contract(master_loader_contract_t *out_contract);

/**
 * @brief Installs a validated `master_loader_contract_t` into runtime state.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets tests or runtime profiles adjust verification strictness safely while blocking
 * nonsensical or unsafe settings.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates bounds via `master_loader_validate_contract()`. Rejecting a corrupt
 * contract downgrades cleanly to `.rodata` ROM defaults (`loaded_from_data = false`),
 * records Tier-0 fault telemetry, and escalates `io.loader.contract_bounds`.
 * @triggers `io.loader.contract_bounds`, `LOG_CAT_IO`.
 *
 * @param[in] contract Contract descriptor to validate and commit.
 * @return true if valid and committed; false after Safe-Mode downgrade on corruption.
 */
bool master_loader_set_contract(const master_loader_contract_t *contract);

/**
 * @brief Returns immutable human-readable `.rodata` mnemonic for `loader_status_t`.
 *
 * ### For Game Designers & Non-Programmers:
 * Translates numeric error codes into clear status names for logs and status cards.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure `O(1)` lookup returning static `.rodata` string literal (never NULL).
 * @triggers None.
 *
 * @param[in] status Loader status enum value.
 * @return NUL-terminated `.rodata` status string.
 */
const char *master_loader_status_to_string(loader_status_t status);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
( Verification Contracts, Debugger Validation Probes & Self-Audit Runners )
================================================================================
Mission: Validate schema/version/contract invariants cleanly, wire `io.loader.*`
         probes into the central debugger (`DEBUG_STEP_AWAIT` Safe-Mode loop),
         and verify operational correctness with zero `.bss` buffer bloat.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Validation Predicates & Debugger Probe Registration
--------------------------------------------------------------------------------
Non-mutating verification predicates plus idempotent `debug_validation_probe_t`
wiring for contract bounds, envelope integrity, and schema enforcement.
--------------------------------------------------------------------------------
*/

/**
 * @brief Validates Block 2 `schema_id` against an expected contract identifier.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures a loaded JSON file is truly the asset kind the caller asked for (for
 * example, rejecting a sound manifest accidentally loaded as combat formulas).
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap exact match on `loader->header.schema_id`. Respects active
 * `contract.enforce_schema_id` only inside `master_loader_validate_envelope()`;
 * direct explicit check always verifies non-empty exact schema identity cleanly.
 *
 * @param[in] loader             Ready `master_loader_t` instance.
 * @param[in] expected_schema_id Required schema contract string (e.g. `"tenlux.data.math_formulas.v1"`).
 * @return true if ready and `schema_id` matches exactly; false otherwise.
 */
bool master_loader_validate_schema(const master_loader_t *loader,
                                   const char *expected_schema_id);

/**
 * @brief Validates Block 1 semantic `version` against an expected version or prefix.
 *
 * ### For Game Designers & Non-Programmers:
 * Prevents outdated or incompatible future data files from silently breaking gameplay.
 *
 * ### For Systems Programmers & Engine Architects:
 * Performs boundary-safe prefix matching (`strcmp` or `'0.1'` prefix delimited by
 * `'\0'`, `'.'`, or `'-'`) so `"0.1"` validates `"0.1.0-dev"` and `"0.1.1-dev"`
 * without falsely matching `"0.10.0"`.
 *
 * @param[in] loader           Ready `master_loader_t` instance.
 * @param[in] required_version Expected exact version or major.minor prefix string.
 * @return true on clean version contract match; false otherwise.
 */
bool master_loader_validate_version(const master_loader_t *loader,
                                    const char *required_version);

/**
 * @brief Performs full active-contract verification over a loaded `master_loader_t`.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks body presence, schema ID, and version compatibility in one sweep using the
 * rules stored in `loader_contracts.json`.
 *
 * ### For Systems Programmers & Engine Architects:
 * Enforces `require_block_1_metadata`, `require_5block_envelope`,
 * `enforce_schema_id` (when `expected_schema_id != NULL`), and monotonic prefix
 * matching against `expected_major_minor_version` when `header.version` is present.
 *
 * @param[in] loader             Ready `master_loader_t` context.
 * @param[in] expected_schema_id Optional required `schema_id` (NULL checks non-empty when enforced).
 * @return true when all active contract checks succeed; false otherwise.
 */
bool master_loader_validate_envelope(const master_loader_t *loader,
                                     const char *expected_schema_id);

/**
 * @brief Validates internal coherence and safety bounds of a `master_loader_contract_t`.
 *
 * ### For Game Designers & Non-Programmers:
 * Rejects tampered contract limits (like negative depth limits or zero payload caps)
 * before they can disable engine safety guards.
 *
 * ### For Systems Programmers & Engine Architects:
 * Verifies non-empty `expected_major_minor_version`, `1 <= max_json_depth <= 32`,
 * `1 <= max_json_tokens <= 1024`, and `IO_FB_SMALL_BYTES <= max_payload_bytes <= 65536`.
 *
 * @param[in] contract Contract descriptor to validate.
 * @return true if all contract fields lie within engine safety bounds; false otherwise.
 */
bool master_loader_validate_contract(const master_loader_contract_t *contract);

/**
 * @brief Idempotently registers `io.loader.*` validation probes with `debug.c`.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects MasterLoader health watches to the interactive debugger tripwire panel.
 *
 * ### For Systems Programmers & Engine Architects:
 * Registers `io.loader.contract_bounds`, `io.loader.envelope_integrity`, and
 * `io.loader.rom_parity` when `debug_is_active()` is true.
 */
void master_loader_register_debug_probes(void);

/**
 * @brief Escalates a MasterLoader fault through Tier-0 BIOS Safe Mode and debugger trap.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches corrupt settings back to safe built-in ROM rules and pauses the debugger
 * (`DEBUG_STEP_AWAIT`) when active so developers can inspect what failed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Maps `error_code` (`IO_ERR_*`) to matching `io.loader.*` probe ID, suppresses recursion
 * during `in_preflight`, invokes `io_fallback_downgrade_safe_mode()`, and calls
 * `debug_trigger_probe()` when `debug_is_active() && contract.probe_await_on_corrupt`.
 *
 * @param[in] context_label Descriptive failure summary for telemetry cards.
 * @param[in] error_code    Taxonomy hex error (`IO_ERR_JSON_SYNTAX`, `IO_ERR_JSON_DEPTH_EXCEEDED`, etc.).
 * @return true if `debug_trigger_probe()` latched debugger inspection; false otherwise.
 */
bool master_loader_trigger_probe(const char *context_label, uint16_t error_code);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Tripwire Runners
--------------------------------------------------------------------------------
Executed at boot (`--audit`) and via unit/system test runners. Zero heap; zero
permanent `.bss` staging buffers.
--------------------------------------------------------------------------------
*/

/**
 * @brief Runs the diagnostic self-verification preflight suite for `master_loader`.
 *
 * ### For Game Designers & Non-Programmers:
 * Verifies formula ingestion, ROM Safe-Mode fallback, malformed JSON rejection,
 * and cross-block isolation before gameplay launches.
 *
 * ### For Systems Programmers & Engine Architects:
 * Preserves and restores `.bss` telemetry/contract state (`in_preflight = true`),
 * uses bounded local stack buffers only (zero `.bss` test arrays), and returns
 * a structured `diag_result_t`.
 *
 * @return `diag_result_t` carrying `SEVERITY_PASS` or first detected tripwire failure.
 */
diag_result_t master_loader_preflight(void);

/**
 * @brief Boolean convenience wrapper running `master_loader_preflight().passed`.
 *
 * @return true when all MasterLoader pre-flight tripwires pass; false otherwise.
 */
bool master_loader_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK
( High-Level Orchestrators, Inspection Cards, Telemetry & Lifecycle )
================================================================================
Mission: Expose turn-key inspect/load-verified orchestrators, structured
         diagnostic cards, live telemetry stats, and clean init/shutdown lifecycle.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Orchestrators, Diagnostic Cards & Runtime Telemetry
--------------------------------------------------------------------------------
Unified one-call inspection/verification cogs and read-only telemetry export.
--------------------------------------------------------------------------------
*/

/**
 * @brief Inspects only the Block 1/2 provenance header of a VFS asset safely.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a file's title card (ID, version, schema, ROM/Disk source) without asking
 * gameplay systems to manage raw read buffers.
 *
 * ### For Systems Programmers & Engine Architects:
 * Inspects cleanly using a transient stack buffer (`FS_BUFFER_LARGE_BYTES`, 32 KB)
 * with zero permanent `.bss` footprint and copies parsed `loader_header_t`.
 *
 * @param[in]  domain     Logical VFS domain (`"config"`, `"mechanics"`, etc.).
 * @param[in]  rel_path   Relative path inside domain.
 * @param[out] out_header Destination header metadata struct.
 * @return true if asset read and parsed cleanly; false otherwise.
 */
bool master_loader_execute_inspect(const char *domain,
                                   const char *rel_path,
                                   loader_header_t *out_header);

/**
 * @brief Formats a 60-column inspection summary card for a parsed `loader_header_t`.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a neat inspector card listing file name, dataset ID, version, schema,
 * and whether the file came from disk or Safe-Mode ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Single bounded `snprintf()` pass aligned with `fs_execute_dump_card()` and
 * `path_finder_execute_dump_card()`.
 *
 * @param[in]  header   Parsed `loader_header_t` (NULL outputs compact fault card).
 * @param[out] out_buf  Destination ASCII text buffer.
 * @param[in]  buf_size Byte capacity of `out_buf`.
 * @return Number of bytes written (excluding NUL), or `0U` on invalid buffer.
 */
size_t master_loader_execute_dump_card(const loader_header_t *header,
                                       char *out_buf,
                                       size_t buf_size);

/**
 * @brief One-call verified VFS ingestion enforcing schema ID, 5-block body, and version.
 *
 * ### For Game Designers & Non-Programmers:
 * Loads a file AND guarantees it conforms to the exact data format expected before
 * returning success.
 *
 * ### For Systems Programmers & Engine Architects:
 * Chains `master_loader_load()` + `master_loader_validate_envelope()`. Sets
 * `out_loader->status = LOADER_STATUS_ERR_SCHEMA_MISMATCH` (`0x0203`) on schema
 * mismatch (triggering telemetry/probe on disk corruption when enabled) or
 * `LOADER_STATUS_ERR_VERSION_MISMATCH` (`0x0207`) on version prefix breach.
 *
 * @param[in]  domain             Logical VFS domain (`"config"`, `"world"`, etc.).
 * @param[in]  rel_path           Relative file path inside domain.
 * @param[in]  expected_schema_id Exact expected Block 2 `schema_id` string.
 * @param[out] buf                Caller read buffer.
 * @param[in]  buf_size           Byte capacity of `buf`.
 * @param[out] out_loader         Verified loader handle populated on success or fault.
 * @return true if loaded, structurally verified, and body isolated; false otherwise.
 */
bool master_loader_execute_load_verified(const char *domain,
                                         const char *rel_path,
                                         const char *expected_schema_id,
                                         char *buf,
                                         size_t buf_size,
                                         master_loader_t *out_loader);

/**
 * @brief Exports a read-only telemetry & active-contract snapshot (`ALLOC_ZERO_HEAP`).
 *
 * ### For Game Designers & Non-Programmers:
 * Lets developer panels display MasterLoader load counts, disk vs. ROM ratios,
 * and active verification strictness toggles.
 *
 * ### For Systems Programmers & Engine Architects:
 * Ensures runtime contract armed and copies `master_loader_stats_t` in O(1).
 *
 * @param[out] out_stats Destination stats snapshot struct.
 * @return true on clean export; false on NULL output pointer.
 */
bool master_loader_get_stats(master_loader_stats_t *out_stats);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Control
--------------------------------------------------------------------------------
Initializes contract hydration from `io/loader_contracts.json` + `.rodata` ROM,
arms debugger probes, and resets lifecycle state cleanly on shutdown.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes MasterLoader, hydrates `loader_contracts.json`, and arms probes.
 *
 * ### For Game Designers & Non-Programmers:
 * Boots the Master Loader at engine startup and prints whether file rules came
 * from external JSON configuration or built-in ROM safety tables.
 *
 * ### For Systems Programmers & Engine Architects:
 * Ensures Tier-0 I/O BIOS is booted, hydrates `io/loader_contracts.json` (with
 * Safe-Mode ROM downgrade if corrupt/absent), registers `io.loader.*` probes
 * when `debug_is_active()`, and emits clean `LOG_CAT_IO` lifecycle telemetry.
 */
void master_loader_init(void);

/**
 * @brief Resets MasterLoader active initialization and probe registration flags cleanly.
 *
 * ### For Game Designers & Non-Programmers:
 * Logs final load counts cleanly when shutting down or restarting subsystems.
 *
 * ### For Systems Programmers & Engine Architects:
 * Emits `LOG_CAT_IO` shutdown summary before resetting `initialized` and
 * `probes_registered` flags safely without heap frees.
 */
void master_loader_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_IO_MASTER_LOADER_H */
