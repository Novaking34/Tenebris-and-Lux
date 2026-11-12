/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/include/io/io_fallback.h
@id            cog.io.fallback
@layer         Layer 0: Core Utilities
@archetype     quarantined_fallback | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Tier-0 BIOS Safe-Mode I/O Engine & Compiled ROM Vault
@role          Public Contract & Seam: Quarantined emergency API across FS,
               JSON, VFS, MasterLoader, read-only floppy latch, and ROM streams.
@maturity      Rev 5 Baseline | Full 4-pillar I/O BIOS, 8 compiled I/O ROMs & shared policy parser ready.
@chassis       zero-heap | Public POD Safe-Mode policies; bounded by IO_FALLBACK_* path & payload limits.
@scale         O(n) linear scan across JSON profile tokens; scales over 8 build-generated I/O config ROMs.
@seams         in: io_config_rom.h | uses: core/manifest.h (quarantined) | probes: io_fb.* | dag: io.fallback_bios
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Quarantined include invariant: never includes primary fs_utils.h, json_utils.h, path_finder.h, or master_loader.h.
               3. Safe-Mode & ROM-hydrated configs preserve loaded_from_data = false unconditionally.
               4. Active read-only floppy latch cleanly blocks disk writes and recursive mkdir calls.

PURPOSE & ARCHITECTURAL ROLE:
  Defines the unified Tier-0 BIOS Safe-Mode contract covering emergency file
  I/O, JSON scanning, VFS routing (`rom://` URIs), and 5-block loader parsing
  so engine startup and fault recovery run cleanly with zero external disk files.

  ### For Game Designers & Non-Programmers:
  Acts as the engine's sealed storage BIOS. Even if config folders vanish or
  the build runs off a locked floppy drive, built-in factory storage rules answer
  reads cleanly and lock out unsafe disk writes.

  ### For Systems Programmers & Engine Architects:
  - Ownership & Reentrancy: Exposes read-only `.rodata` ROM payloads and shares
    `io_fallback_parse_policy_json()` cleanly with primary `io/` loaders.
  - Extension Seam: Add JSON configs under `engine/config/io/` to extend the
    compiled `io_config_rom.h` table automatically via CMake.
  - Explicit Non-Goal: Does not resolve layered `game/` override mounts (owned
    by `cog.path_finder` when disk trees are online).
================================================================================
*/

#ifndef CORNERSTONE_IO_IO_FALLBACK_H
#define CORNERSTONE_IO_IO_FALLBACK_H

/*
================================================================================
BLOCK 2: OPENING BLOCK
( Tier-0 Safe-Mode BIOS Quarantine, ROM Contracts & Standalone Frontier )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.io.fallback
Mission:       Quarantined standalone BIOS interface free of primary headers.
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers (Quarantined) --- */
#include "core/manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, BIOS Constants & Compile-Time Proofs
--------------------------------------------------------------------------------
Immutable buffer ceilings, Safe-Mode profile names, and taxonomy error codes.

### For Game Designers & Non-Programmers:
Keeps every file read and write inside strict, predictable memory bounds backed
by `engine/config/io/fs_buffers.json` and `diagnostics/taxonomy/error_codes.json`.

### For Systems Programmers & Engine Architects:
Immutable `.rodata` constants paired with `static_assert` proofs ensuring
BIOS ceilings match canonical configuration envelope sizes.
--------------------------------------------------------------------------------
*/

/** Standard Safe-Mode profile stamped onto BIOS emergency configurations. */
#define IO_FALLBACK_PROFILE_NAME         "io_bios_fallback"

/** Compact read-only floppy preset stamped during diskless/locked boot. */
#define IO_FALLBACK_FLOPPY_PROFILE_NAME  "floppy_read_only"

/** Default small stack/token buffer ceiling (1,024 bytes). */
#define IO_FB_SMALL_BYTES                1024U

/** Default medium single-config profile ceiling (8,192 bytes). */
#define IO_FB_MEDIUM_BYTES               8192U

/** Default large manifest/catalog ceiling (32,768 bytes). */
#define IO_FB_LARGE_BYTES                32768U

/** Absolute maximum engine config size ceiling (65,536 bytes). */
#define IO_FB_MAX_CONFIG_BYTES           65536U

/** Maximum filesystem path length in bytes (including null terminator). */
#define IO_FB_PATH_MAX                   512U

/** Maximum mounted VFS data root length in bytes. */
#define IO_FB_MAX_ROOT_LEN               256U

/** Maximum JSON bracket/brace recursion depth for Safe-Mode scanning. */
#define IO_FB_MAX_JSON_DEPTH             32

/** Maximum JSON token limit tracked in Safe-Mode contract checks. */
#define IO_FB_MAX_JSON_TOKENS            1024U

/** Fixed capacity of the Tier-0 BIOS I/O fault ring buffer in .bss. */
#define IO_FB_FAULT_RING_CAP             16U

/** Default mounted data root directory used when environment is unset. */
#define IO_FB_DEFAULT_ROOT               "game/data"

/** Virtual ROM URI prefix enabling zero-disk resolution across VFS/FS APIs. */
#define IO_FB_ROM_URI_PREFIX             "rom://"

/** Maximum number of dynamic VFS domain search subdirectories in `.bss`. */
#define IO_FB_MAX_VFS_DOMAIN_DIRS        48U

/** Maximum number of dynamic VFS root probe candidates in `.bss`. */
#define IO_FB_MAX_VFS_CANDIDATES         12U

/** Maximum number of layered config precedence tiers in `.bss`. */
#define IO_FB_MAX_PRECEDENCE_TIERS       8U

/** Maximum number of hydrated asset base directories in `.bss`. */
#define IO_FB_MAX_ASSET_ROOTS            8U

/** Maximum path segment string capacity per hydrated VFS directory entry. */
#define IO_FB_MAX_DIR_SEG_LEN            64U

/** Default POSIX directory creation bitmask matching fs_buffers.json. */
#define IO_FB_DEFAULT_DIR_MODE           0755U

/** Default maximum symlink resolution depth for VFS path expansion. */
#define IO_FB_DEFAULT_SYMLINK_DEPTH      4

/** Standardized error taxonomy hex codes aligned with error_codes.json. */
#define IO_ERR_FILE_NOT_FOUND            0x0501U
#define IO_ERR_READ_FAILURE              0x0502U
#define IO_ERR_WRITE_BLOCKED_RO          0x0504U
#define IO_ERR_PATH_OVERFLOW             0x0505U
#define IO_ERR_TRUNCATED_READ            0x0506U
#define IO_ERR_ROM_MISSING               0x0507U
#define IO_ERR_JSON_SYNTAX               0x0601U
#define IO_ERR_JSON_KEY_MISSING          0x0602U
#define IO_ERR_JSON_DEPTH_EXCEEDED       0x0603U

static_assert(
    IO_FB_MAX_CONFIG_BYTES >= IO_FB_LARGE_BYTES &&
    IO_FB_LARGE_BYTES > IO_FB_MEDIUM_BYTES &&
    IO_FB_MEDIUM_BYTES > IO_FB_SMALL_BYTES,
    "BIOS buffer bracket thresholds must ascend monotonically"
);
static_assert(
    IO_FB_PATH_MAX >= IO_FB_MAX_ROOT_LEN,
    "BIOS max path length must accommodate mounted data roots"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor registering Tier-0 I/O BIOS capabilities.
--------------------------------------------------------------------------------
*/

/** Module identifier matching subsystems.json and core manifest DAG contracts. */
#define IO_FALLBACK_MODULE_ID            "io.fallback_bios"

/** Module manifest record for the Tier-0 BIOS I/O & Config ROM subsystem. */
extern const module_metadata_t io_fallback_manifest;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Fallback Object Definitions
--------------------------------------------------------------------------------
Tagged types shared across emergency FS, JSON, VFS, and MasterLoader routines.
--------------------------------------------------------------------------------
*/

/**
 * @enum io_fallback_sink
 * @brief Operating medium currently backing Tier-0 I/O fallback operations.
 *
 * ### For Game Designers & Non-Programmers:
 * Shows whether file loading is running normally on disk, locked into read-only
 * floppy mode, or running purely from internal ROM memory.
 *
 * ### For Systems Programmers & Engine Architects:
 * Explicitly tracks Safe-Mode degradation tier inside `io_fallback_state_t`.
 */
typedef enum io_fallback_sink {
    IO_FB_SINK_OFFLINE = 0,   /**< BIOS not yet armed. */
    IO_FB_SINK_ROM_ONLY,      /**< Diskless mode; serves embedded .rodata ROM streams only. */
    IO_FB_SINK_FLOPPY_RO,     /**< Read-only Safe Mode; disk writes and mkdir blocked cleanly. */
    IO_FB_SINK_DISK_ACTIVE    /**< Standard BIOS fallback with writable POSIX disk enabled. */
} io_fallback_sink_t;

/**
 * @enum io_fb_loader_status
 * @brief Status codes returned by Tier-0 MasterLoader BIOS ingestion routines.
 */
typedef enum io_fb_loader_status {
    IO_FB_LOADER_OK = 0,             /**< 5-block envelope parsed and verified cleanly. */
    IO_FB_LOADER_ERR_NULL,           /**< NULL pointer or zero capacity rejected. */
    IO_FB_LOADER_ERR_NOT_FOUND,      /**< Asset unavailable on both disk VFS and .rodata ROM. */
    IO_FB_LOADER_ERR_OVERFLOW,       /**< Payload exceeded bounded caller buffer capacity. */
    IO_FB_LOADER_ERR_SYNTAX,         /**< Malformed JSON syntax detected during scan. */
    IO_FB_LOADER_ERR_BODY_MISSING,   /**< Required `block_3_body` object absent. */
    IO_FB_LOADER_ERR_SCHEMA_MISMATCH,/**< Header `schema_id` failed expected contract. */
    IO_FB_LOADER_ERR_READ_ONLY       /**< Write rejected under read-only floppy latch. */
} io_fb_loader_status_t;

/**
 * @struct io_fallback_config
 * @brief Unified layer-0 I/O runtime policy record shared by ROM & primary loaders.
 *
 * ### For Game Designers & Non-Programmers:
 * Holds the master file-size caps, JSON safety depth, read-only lock, and
 * fallback mode switches configured by `engine/config/io/`.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed-layout zero-heap policy struct hydrated from `.rodata` ROM or disk JSON
 * via `io_fallback_parse_policy_json()`. Safe-Mode & ROM loads always preserve
 * `loaded_from_data == false`.
 */
typedef struct io_fallback_config {
    size_t       small_bytes;           /**< Small buffer cap (default: 1024 B). */
    size_t       medium_bytes;          /**< Medium config buffer cap (default: 8192 B). */
    size_t       large_bytes;           /**< Large manifest buffer cap (default: 32768 B). */
    size_t       max_config_bytes;      /**< Hard upper file ceiling (default: 65536 B). */
    size_t       path_max_bytes;        /**< POSIX path ceiling (default: 512 B). */
    int          max_json_depth;        /**< Max nested bracket/brace scan depth (default: 32). */
    size_t       max_json_tokens;       /**< Bounded single-pass token ceiling (default: 1024). */
    bool         reject_on_truncation;  /**< True to fail reads exceeding buffer capacity. */
    bool         require_5block_envelope; /**< True to mandate `block_3_body` in MasterLoader. */
    bool         enforce_schema_id;     /**< True to verify `schema_id` during verified loads. */
    bool         rom_fallback_enabled;  /**< True to resolve `"rom://..."` when disk is absent. */
    bool         read_only_mode;        /**< True to block disk writes/mkdir (Floppy Safe Mode). */
    bool         probe_await_on_corrupt; /**< True to trigger DEBUG_STEP_AWAIT on policy fault. */
    unsigned int posix_dir_mode;        /**< POSIX directory creation bitmask (default: 0755). */
    bool         flush_on_write;        /**< True to call fflush() before closing written files. */
    bool         atomic_write;          /**< True to write sibling temp file and rename(). */
    bool         block_path_traversal;  /**< True to reject '..' parent traversal components. */
    int          max_symlink_depth;     /**< Bounded VFS symlink resolution depth (default: 4). */
    bool         allow_mod_mounts;      /**< True to permit external mod overlay VFS mounts. */
    bool         strict_utf8_escapes;   /**< Policy flag for strict JSON UTF-8 escape validation. */
    bool         reject_duplicate_keys; /**< Policy flag to disallow duplicate JSON object keys. */
    bool         hot_reload_enabled;    /**< True in dev presets enabling live config reload. */
    char         default_root[64];      /**< Default VFS root directory (`"game/data"`). */
    char         profile_name[32];      /**< Active policy profile identifier. */
    bool         loaded_from_data;      /**< Provenance flag: ALWAYS false in Safe-Mode & ROM. */
} io_fallback_config_t;

/**
 * @struct io_fb_vfs_routes
 * @brief Hydrated VFS root candidates, layered config precedence, and domain search paths.
 *
 * ### For Game Designers & Non-Programmers:
 * Controls every subfolder (`mechanics/`, `world/`, `diagnostics/`, `audio/`, etc.)
 * where the engine searches automatically for data manifests and user config overrides.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed `.bss` table (~4.3 KB) hydrated dynamically from `engine/config/io/vfs_routes.json`
 * (`.rodata` ROM on Tier-0 boot, disk override when mounted). Zero heap allocation.
 */
typedef struct io_fb_vfs_routes {
    char   default_data_root[64];       /**< Baseline data root (`"game/data"`). */
    char   env_override_var[32];        /**< Env var checked for root override (`"TL_DATA_ROOT"`). */
    char   config_precedence[IO_FB_MAX_PRECEDENCE_TIERS][IO_FB_MAX_DIR_SEG_LEN]; /**< Config precedence prefixes. */
    size_t config_precedence_count;     /**< Active precedence prefix count. */
    char   fallback_candidate_roots[IO_FB_MAX_VFS_CANDIDATES][IO_FB_MAX_DIR_SEG_LEN]; /**< Probe base directories. */
    size_t candidate_root_count;        /**< Active candidate root count. */
    char   asset_roots[IO_FB_MAX_ASSET_ROOTS][IO_FB_MAX_DIR_SEG_LEN]; /**< Asset category base directories. */
    size_t asset_root_count;            /**< Active asset root count. */
    char   domain_search_paths[IO_FB_MAX_VFS_DOMAIN_DIRS][IO_FB_MAX_DIR_SEG_LEN]; /**< Relative domain subfolders. */
    size_t domain_search_count;         /**< Active domain subfolder count. */
    char   diag_log_prefix[IO_FB_MAX_DIR_SEG_LEN];   /**< Diagnostic alias target for `log/`. */
    char   diag_debug_prefix[IO_FB_MAX_DIR_SEG_LEN]; /**< Diagnostic alias target for `debug/`. */
    char   diag_io_prefix[IO_FB_MAX_DIR_SEG_LEN];    /**< Diagnostic alias target for `io/`. */
    bool   block_path_traversal;        /**< True to reject '..' traversal segments. */
    int    max_symlink_depth;           /**< Bounded symlink hop cap. */
    bool   allow_mod_mounts;            /**< True to allow external mod VFS layers. */
    bool   rom_fallback_enabled;        /**< True to allow virtual `rom://` resolution fallback. */
    bool   probe_await_on_corrupt;      /**< True to halt in `DEBUG_STEP_AWAIT` on corrupt route policies. */
    bool   loaded_from_data;            /**< True only when hydrated from external disk JSON. */
} io_fb_vfs_routes_t;

/**
 * @struct io_fb_loader_contract
 * @brief Hydrated MasterLoader 5-block envelope and schema verification policy.
 *
 * ### For Game Designers & Non-Programmers:
 * Guarantees every lore file, combat catalog, and settings JSON starts with valid
 * metadata and matches the expected engine version before gameplay touches it.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed-footprint contract descriptor populated from `loader_contracts.json`
 * (`.rodata` ROM on BIOS boot or disk manifest when mounted).
 */
typedef struct io_fb_loader_contract {
    bool   require_block_1_metadata;    /**< True to enforce block_1_metadata header presence. */
    bool   require_5block_envelope;     /**< True to enforce block_3_body extraction. */
    bool   enforce_schema_id;           /**< True to verify expected schema_id strings. */
    bool   require_monotonic_version;   /**< True to enforce compatible engine minor version. */
    bool   allow_legacy_block2_types;   /**< Permit legacy block_2_types schema_id fallback. */
    bool   probe_await_on_corrupt;      /**< Escalate corrupt contracts/envelopes to DEBUG_STEP_AWAIT. */
    bool   strict_utf8_escapes;         /**< Strict JSON escape validation contract. */
    bool   reject_duplicate_keys;       /**< Reject duplicate object keys when scanning. */
    int    max_json_depth;              /**< Maximum nested JSON bracket depth contract (<= 32). */
    size_t max_json_tokens;             /**< Maximum single-pass JSON token scan cap (<= 1024). */
    size_t max_payload_bytes;           /**< Maximum bounded JSON payload size (<= 65536). */
    char   expected_major_minor_version[16]; /**< Expected version prefix (e.g. `"0.1"`). */
    bool   loaded_from_data;            /**< True only when hydrated from external disk JSON. */
} io_fb_loader_contract_t;

/**
 * @struct io_fb_file_info
 * @brief Bounded POSIX / Virtual ROM node inspection descriptor.
 */
typedef struct io_fb_file_info {
    bool exists;                        /**< True if disk node or `.rodata` ROM asset exists. */
    bool is_regular;                    /**< True for POSIX S_ISREG or embedded ROM asset. */
    bool is_directory;                  /**< True for POSIX S_ISDIR directory node. */
    bool is_rom_virtual;                /**< True when resolved from internal `.rodata` ROM. */
    long size_bytes;                    /**< Exact byte size (-1 if inaccessible). */
} io_fb_file_info_t;

/**
 * @struct io_fb_loader_header
 * @brief Parsed OmniCode Block 1/2 metadata header populated by BIOS loader.
 */
typedef struct io_fb_loader_header {
    char project[64];                   /**< Block 1 project name. */
    char file[128];                     /**< Block 1 file provenance path. */
    char id[64];                        /**< Block 1 dataset or config ID. */
    char layer[64];                     /**< Block 1 architectural stratum string. */
    char version[32];                   /**< Semantic version string. */
    char schema_id[64];                 /**< Block 2 schema identifier. */
    bool is_5block;                     /**< True if `block_1_metadata` envelope is present. */
    bool loaded_from_rom;               /**< True if hydrated from compiled `.rodata` ROM. */
} io_fb_loader_header_t;

/**
 * @struct io_fb_loader
 * @brief Zero-heap Tier-0 BIOS MasterLoader context operating on caller or ROM spans.
 */
typedef struct io_fb_loader {
    io_fb_loader_header_t header;                  /**< Extracted envelope metadata header. */
    io_fb_loader_status_t status;                  /**< Last ingestion/validation status code. */
    const char           *raw_buf;                 /**< Non-owning pointer to JSON source bytes. */
    size_t                raw_len;                 /**< Byte length of JSON source stream. */
    const char           *body_start;              /**< Slice pointer to `block_3_body` object. */
    size_t                body_len;                /**< Byte length of `block_3_body` object. */
    char                  resolved_path[IO_FB_PATH_MAX]; /**< Disk path or `"rom://..."` URI. */
} io_fb_loader_t;

/**
 * @struct io_fallback_fault_entry
 * @brief Single static-ring diagnostic breadcrumb recording I/O BIOS fault events.
 */
typedef struct io_fallback_fault_entry {
    uint64_t sequence_id;               /**< Monotonic fault sequence counter. */
    uint16_t error_code;                /**< Taxonomy hex code (e.g., 0x0501, 0x0601). */
    char     subsystem[24];             /**< Reporting I/O cog (`fs_utils`, `json`, etc.). */
    char     operation[32];             /**< Failing operation (`fs_read_file_bounded`, etc.). */
    char     message[88];               /**< Concise human-readable diagnostic summary. */
} io_fallback_fault_entry_t;

/**
 * @struct io_fallback_stats
 * @brief Read-only diagnostic snapshot of live Tier-0 I/O BIOS & ROM vault health.
 *
 * ### For Game Designers & Non-Programmers:
 * Summarizes whether storage Safe Mode is armed, whether disk writes are locked,
 * how many factory ROM files are loaded, and any blocked or fallback I/O counts.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copied cleanly from static `.bss` state by `io_fallback_get_stats()` with zero heap.
 */
typedef struct io_fallback_stats {
    bool                 booted;                 /**< True when BIOS I/O fallback is active. */
    bool                 rom_hydrated;           /**< True when policy parsed from `.rodata` ROM. */
    bool                 disk_read_only;         /**< True when Safe-Mode write latch is active. */
    io_fallback_sink_t   active_sink;            /**< Active fallback medium/mode. */
    size_t               rom_asset_count;        /**< Count of compiled `io_config_rom.h` files. */
    size_t               rom_bytes_total;        /**< Total `.rodata` payload bytes embedded. */
    uint64_t             rom_hits;               /**< Total zero-disk `"rom://"` resolves/reads. */
    uint64_t             fault_count;            /**< Total faults captured in BIOS ring. */
    uint64_t             read_only_blocks;       /**< Write/mkdir attempts blocked safely. */
    uint64_t             safe_downgrades;        /**< Number of Safe-Mode downgrade latches. */
    char                 active_root[IO_FB_MAX_ROOT_LEN]; /**< Mounted VFS root directory. */
    char                 active_profile[32];     /**< Active policy profile name. */
    bool                 loaded_from_data;       /**< Always false for Safe-Mode BIOS reports. */
} io_fallback_stats_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Fast header-only O(1) validators preventing NULL dereferences and out-of-range caps.
--------------------------------------------------------------------------------
*/

/**
 * @brief Validates that a destination buffer pointer and byte capacity are usable.
 * @return true if `buf != NULL` and `cap > 0`.
 */
static inline bool io_fb_buf_is_valid(const void *buf, size_t cap)
{
    return (buf != NULL) && (cap > 0U);
}

/**
 * @brief Validates that a string length fits strictly within `IO_FB_PATH_MAX`.
 * @return true if `len > 0` and `len < IO_FB_PATH_MAX`.
 */
static inline bool io_fb_path_len_valid(size_t len)
{
    return (len > 0U) && (len < IO_FB_PATH_MAX);
}

/**
 * @brief Returns true if `path` uses the virtual `.rodata` ROM URI scheme `"rom://"`.
 */
static inline bool io_fb_is_rom_uri(const char *path)
{
    return (path != NULL) &&
           (path[0] == 'r') && (path[1] == 'o') && (path[2] == 'm') &&
           (path[3] == ':') && (path[4] == '/') && (path[5] == '/');
}

/**
 * @brief Predicate returning true for JSON whitespace bytes.
 */
static inline bool io_fb_is_ws(char c)
{
    return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

/**
 * @brief Pure O(1) invariant predicate verifying ascending buffer ladder ceilings.
 *
 * @param[in] cfg Policy configuration to validate.
 * @return true if non-NULL and `0 < small < medium < large <= max_config <= 65536`
 *         and path/depth bounds sit in safe ranges.
 */
static inline bool io_fb_policy_bounds_valid(const io_fallback_config_t *cfg)
{
    if (cfg == NULL) {
        return false;
    }
    if (cfg->small_bytes == 0U ||
        cfg->small_bytes >= cfg->medium_bytes ||
        cfg->medium_bytes >= cfg->large_bytes ||
        cfg->large_bytes > cfg->max_config_bytes ||
        cfg->max_config_bytes > IO_FB_MAX_CONFIG_BYTES) {
        return false;
    }
    if (cfg->path_max_bytes < 64U || cfg->path_max_bytes > IO_FB_PATH_MAX) {
        return false;
    }
    if (cfg->max_json_depth <= 0 || cfg->max_json_depth > IO_FB_MAX_JSON_DEPTH) {
        return false;
    }
    if (cfg->max_json_tokens == 0U || cfg->max_json_tokens > IO_FB_MAX_JSON_TOKENS) {
        return false;
    }
    if (cfg->posix_dir_mode > 0777U) {
        return false;
    }
    if (cfg->max_symlink_depth < 0 || cfg->max_symlink_depth > 16) {
        return false;
    }
    return true;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Tier-0 Safe-Mode BIOS Cogs, ROM Vault Queries & Fault Recovery )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.io.fallback
Mission:       Declare quarantined BIOS Safe-Mode, .rodata ROM vault, and
               shared fault-recovery APIs with Dual-Audience specs and bounds.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Fallback Policy Constructors & Shared JSON Config Parsers
Mission: Provide hardcoded Safe-Mode/Floppy policies and ONE canonical JSON
parser shared by both `.rodata` ROM hydration and primary disk I/O loaders.

  ### For Game Designers & Non-Programmers:
  Supplies factory-safe storage settings, directory search routes, and loader
  contracts whether external config files exist on disk or only in built-in ROM.

  ### For Systems Programmers & Engine Architects:
  Single-source zero-heap JSON span parsers (`ALLOC_ZERO_HEAP`) ensuring every
  primary `io/` cog shares one parser implementation with `loaded_from_data = false`
  until external disk provenance is explicitly proven.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns immutable compile-time Safe-Mode I/O policy (`loaded_from_data = false`).
 *
 * ### For Game Designers & Non-Programmers:
 * Supplies clean factory buffer sizes and safety rules if no config files exist.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) stack value copy from `.rodata` template `s_io_fallback_template`.
 *
 * @return Valid `io_fallback_config_t` with `loaded_from_data == false`.
 */
io_fallback_config_t io_fallback_get_default_config(void);

/**
 * @brief Returns compact read-only floppy preset (`read_only_mode = true`, `loaded_from_data = false`).
 *
 * ### For Game Designers & Non-Programmers:
 * Locks disk writing off while keeping config loading alive from built-in ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) stack value copy enabling read-only Safe Mode on write-protected media.
 *
 * @return Read-only `io_fallback_config_t` with `loaded_from_data == false`.
 */
io_fallback_config_t io_fallback_get_floppy_config(void);

/**
 * @brief Canonical span-bounded zero-heap JSON -> `io_fallback_config_t` parser.
 *
 * ### For Game Designers & Non-Programmers:
 * Converts raw JSON policy text into engine safety settings directly in place
 * without extra temporary scratch copies.
 *
 * ### For Systems Programmers & Engine Architects:
 * Operates strictly on caller slice `[json_start, json_start + json_len)` with zero
 * secondary 8 KB stack copies. Preserves `out_cfg->loaded_from_data = false` and
 * honors missing optional fields via `s_io_fallback_template`.
 *
 * @param[in]  json_start Start pointer of UTF-8 JSON span.
 * @param[in]  json_len   Exact span byte length.
 * @param[out] out_cfg    Validated output configuration struct.
 * @return true if valid JSON parsed cleanly and satisfies `io_fb_policy_bounds_valid()`.
 */
bool io_fallback_parse_policy_span(const char *json_start,
                                   size_t json_len,
                                   io_fallback_config_t *out_cfg);

/**
 * @brief Canonical shared zero-heap JSON -> `io_fallback_config_t` parser.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads buffer limits, JSON safety depths, and read-only switches out of
 * `engine/config/io/` JSON configs whether loaded from disk or embedded ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Length-tolerant wrapper forwarding `[json_text, json_text + effective_len)` into
 * `io_fallback_parse_policy_span()` so `.rodata` ROM boot and `fs_utils_load_policy()`
 * execute through one unified zero-heap path.
 *
 * @param[in]  json_text Null-terminated or length-bounded JSON buffer.
 * @param[in]  json_len  Payload byte length (`0` allowed to measure `strlen`).
 * @param[out] out_cfg   Destination policy struct hydrated with parsed fields.
 * @return true if `block_3_body` parsed cleanly and bounds are valid; false on fault.
 */
bool io_fallback_parse_policy_json(const char *json_text,
                                   size_t json_len,
                                   io_fallback_config_t *out_cfg);

/**
 * @brief Parses VFS routing, domain directories, and precedence tiers from JSON (`block_3_body`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads `vfs_routes.json` so all 36 data subfolders, override paths, and symlink
 * rules activate dynamically without hardcoding directory names in C.
 *
 * ### For Systems Programmers & Engine Architects:
 * Single-pass span scanner populating `io_fb_vfs_routes_t` bounded by
 * `IO_FB_MAX_VFS_DOMAIN_DIRS` (48) and `IO_FB_MAX_VFS_CANDIDATES` (12). Always leaves
 * `loaded_from_data = false` (caller sets `true` only after authenticated disk load).
 *
 * @param[in]  json_text  Raw JSON text buffer containing `vfs_routes.json`.
 * @param[in]  json_len   Byte length of `json_text`.
 * @param[out] out_routes Hydrated output VFS routing descriptor.
 * @return true when at least one valid domain path or candidate root was hydrated.
 */
bool io_fallback_parse_vfs_routes_json(const char *json_text,
                                       size_t json_len,
                                       io_fb_vfs_routes_t *out_routes);

/**
 * @brief Parses MasterLoader envelope contracts from `loader_contracts.json` (`block_3_body`).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads strict envelope verification toggles (metadata presence, schema ID checks,
 * version prefix) straight from config data or ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Single-pass bounded span scan populating `io_fb_loader_contract_t` with `ALLOC_ZERO_HEAP`.
 *
 * @param[in]  json_text    Raw JSON buffer containing `loader_contracts.json`.
 * @param[in]  json_len     Byte length of `json_text`.
 * @param[out] out_contract Hydrated MasterLoader contract descriptor.
 * @return true on clean contract extraction.
 */
bool io_fallback_parse_loader_contract_json(const char *json_text,
                                            size_t json_len,
                                            io_fb_loader_contract_t *out_contract);

/**
 * @brief Returns read-only pointer to active hydrated VFS routing table in `.bss`.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets higher layers inspect active config search order and domain subfolders cleanly.
 *
 * ### For Systems Programmers & Engine Architects:
 * Auto-arms BIOS if needed and returns immutable pointer into `s_io_fb.vfs_routes`
 * (~4.3 KB static `.bss`), avoiding large struct copies on caller stack frames.
 *
 * @return Non-NULL pointer to active `io_fb_vfs_routes_t`.
 */
const io_fb_vfs_routes_t *io_fallback_get_vfs_routes(void);

/**
 * @brief Updates live `.bss` VFS routing descriptor after contract validation.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets custom builds or tests swap which data folders the emergency I/O searches first.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies validated `io_fb_vfs_routes_t` into Sub-Assembly 2.3 `.bss` `s_io_fb.vfs_routes`.
 *
 * @param[in] routes Validated VFS routing descriptor to install.
 * @return true on valid installation; false on NULL/empty table.
 */
bool io_fallback_set_vfs_routes(const io_fb_vfs_routes_t *routes);

/**
 * @brief Copies live hydrated MasterLoader contract into caller output struct.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports active envelope rules, schema enforcement, and JSON depth bounds to diagnostics.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) stack copy out of `s_io_fb.loader_contract` (hydrates from ROM defaults if unbooted).
 *
 * @param[out] out_contract Destination contract struct.
 * @return true on success; false on NULL output pointer.
 */
bool io_fallback_get_loader_contract(io_fb_loader_contract_t *out_contract);

/**
 * @brief Updates live MasterLoader contract descriptor cleanly.
 *
 * ### For Game Designers & Non-Programmers:
 * Applies updated JSON envelope, depth, and buffer limits to the fallback BIOS registry.
 *
 * ### For Systems Programmers & Engine Architects:
 * Normalizes depth/token/payload bounds and writes into `s_io_fb.loader_contract`.
 *
 * @param[in] contract Contract descriptor to install.
 * @return true on valid installation; false on NULL input.
 */
bool io_fallback_set_loader_contract(const io_fb_loader_contract_t *contract);

/**
 * @brief Copies live Tier-0 BIOS active configuration into caller storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets primary I/O modules inspect active BIOS file buffer caps and read-only switches.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) copy of `s_io_fb.config` (or default ROM template if BIOS is currently unbooted).
 *
 * @param[out] out_cfg Destination configuration struct.
 * @return true on success; false on NULL output pointer.
 */
bool io_fallback_get_active_config(io_fallback_config_t *out_cfg);

/**
 * @brief Enables or clears the Safe-Mode read-only floppy disk write latch.
 *
 * ### For Game Designers & Non-Programmers:
 * Simulates a write-protected floppy or read-only disc so write operations are safely blocked.
 *
 * ### For Systems Programmers & Engine Architects:
 * Sets `s_io_fb.config.read_only_mode` and switches `active_sink` between `DISK_ACTIVE`
 * and `FLOPPY_RO`.
 *
 * @param[in] read_only true to block disk writes & mkdir safely; false to allow disk writes.
 */
void io_fallback_set_read_only(bool read_only);

/**
 * @brief Returns true if Safe-Mode read-only floppy disk lock is currently active.
 *
 * ### For Game Designers & Non-Programmers:
 * Tells callers whether saving files or creating directories is currently blocked.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) predicate consulting `s_io_fb.config.read_only_mode` and `IO_FB_SINK_FLOPPY_RO`.
 *
 * @return true when write-protected Floppy Safe-Mode is active; false otherwise.
 */
bool io_fallback_is_disk_read_only(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Compiled-In .rodata I/O Config ROM Vault Cogs
Mission: Resolve and hydrate embedded `engine/config/io/` JSON payloads from
immutable ELF `.rodata` (`io_config_rom.h`) with zero disk I/O.

  ### For Game Designers & Non-Programmers:
  Lets players and QA tools inspect or reload factory-sealed engine presets baked
  directly into the executable even when disk folders are absent.

  ### For Systems Programmers & Engine Architects:
  Immutable `O(1)` index lookup and bounded `O(R)` name scan over generated
  `s_io_config_rom_table[]` entries with zero heap allocations.
--------------------------------------------------------------------------------
*/

/**
 * @brief Returns total number of embedded JSON assets compiled into `io_config_rom.h`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reports how many built-in I/O safety presets are baked directly into the game executable.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns `IO_CONFIG_ROM_ASSET_COUNT` compile-time ROM table entry count.
 *
 * @return Number of compiled `.rodata` I/O ROM assets.
 */
size_t io_fallback_rom_count(void);

/**
 * @brief Returns null-terminated JSON ROM payload pointer at table index `index`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a built-in JSON configuration directly from read-only memory without touching disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-copy lookup in `s_io_config_rom_table[]`.
 *
 * @param[in]  index    Asset index in `[0, io_fallback_rom_count() - 1]`.
 * @param[out] out_size Optional byte length excluding null terminator.
 * @return Immutable `.rodata` JSON string pointer, or NULL if out of range.
 */
const char *io_fallback_rom_asset_at(size_t index, size_t *out_size);

/**
 * @brief Retrieves asset metadata (`id` stem & relative path) for ROM index `index`.
 *
 * ### For Game Designers & Non-Programmers:
 * Lists the names and paths of built-in emergency config backups for audit menus.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) metadata accessor returning `.rodata` string pointers and exact payload lengths.
 *
 * @param[in]  index        Asset index in `[0, io_fallback_rom_count() - 1]`.
 * @param[out] out_id       Optional pointer receiving ROM stem ID (e.g. `"default"`).
 * @param[out] out_rel_path Optional pointer receiving rel path (e.g. `"io/default.json"`).
 * @param[out] out_size     Optional pointer receiving byte size.
 * @return true if `index` is valid; false otherwise.
 */
bool io_fallback_rom_info_at(size_t index,
                             const char **out_id,
                             const char **out_rel_path,
                             size_t *out_size);

/**
 * @brief Finds an embedded `.rodata` I/O ROM JSON payload by ID stem or path.
 *
 * Accepts `"default"`, `"io/default.json"`, `"rom://io/default.json"`,
 * `"fs_buffers"`, `"vfs_routes"`, `"loader_contracts"`, or `"manifest"`.
 *
 * ### For Game Designers & Non-Programmers:
 * Locates any built-in emergency I/O config by short name or `"rom://..."` virtual path.
 *
 * ### For Systems Programmers & Engine Architects:
 * Normalizes `"rom://"` / `"io/"` prefixes and returns immutable `.rodata` slice pointers.
 *
 * @param[in]  id_or_path Lookup stem, relative path, or `"rom://..."` URI.
 * @param[out] out_size   Optional pointer receiving byte length.
 * @return Immutable `.rodata` pointer, or NULL if not compiled in ROM.
 */
const char *io_fallback_find_rom_asset(const char *id_or_path, size_t *out_size);

/**
 * @brief Hydrates an `io_fallback_config_t` directly from `.rodata` ROM JSON.
 *
 * Preserves `out_cfg->loaded_from_data == false` unconditionally and sets
 * `profile_name` cleanly on success.
 *
 * ### For Game Designers & Non-Programmers:
 * Restores a named safe I/O profile (like `"default"` or `"floppy_safe"`) from built-in ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Shared zero-heap ROM hydrator calling `io_fallback_parse_policy_span()` and enforcing
 * `out_cfg->loaded_from_data = false`.
 *
 * @param[in]  profile_id ROM profile name (e.g. `"default"`, `"fs_buffers"`, or NULL for `"default"`).
 * @param[out] out_cfg    Destination config structure.
 * @return true if ROM asset was located, parsed, and passed bounds checks.
 */
bool io_fallback_load_rom_config(const char *profile_id, io_fallback_config_t *out_cfg);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Unified Safe-Mode Core Cogs (All 4 I/O Pillars) & Fault Ring
Mission: Provide complete zero-heap fallback implementations covering Filesystem,
JSON, VFS PathFinder, and MasterLoader plus fault ring telemetry & Safe-Mode latches.

  ### For Game Designers & Non-Programmers:
  Keeps reading files, searching data directories, checking save permissions,
  and logging storage warnings working seamlessly during Safe Mode or offline play.

  ### For Systems Programmers & Engine Architects:
  Quarantined fallback mirror spanning Filesystem POSIX/ROM streams, span JSON
  helpers, hydrated 36-dir VFS resolution, 5-block memory ingestion, and FIFO fault ring.
--------------------------------------------------------------------------------
*/

/* --- Pillar 1: Filesystem Core BIOS Fallbacks --- */

/**
 * @brief Checks existence of either a real disk path or a virtual `"rom://..."` URI.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether a save file, config file, or built-in factory ROM blueprint exists.
 *
 * ### For Systems Programmers & Engine Architects:
 * Handles `"rom://..."` lookups in zero-disk `O(R)` table scan (~8 entries) before POSIX `stat()`.
 * Never allocates heap memory; returns `false` cleanly on NULL/empty paths.
 *
 * @param[in] path Disk filesystem path or `"rom://..."` virtual URI.
 * @return true if path exists on disk or in compiled `.rodata` ROM table; false otherwise.
 */
bool io_fallback_fs_exists(const char *path);

/**
 * @brief Checks whether `path` is a POSIX regular file OR an embedded `.rodata` ROM asset.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms a target path points to readable file data rather than an empty folder.
 *
 * ### For Systems Programmers & Engine Architects:
 * Evaluates `"rom://"` assets first (`S_ISREG` via `stat()` otherwise); side-effect free.
 *
 * @param[in] path Disk filesystem path or `"rom://..."` virtual URI.
 * @return true if regular readable file or `.rodata` ROM asset; false otherwise.
 */
bool io_fallback_fs_is_regular(const char *path);

/**
 * @brief Returns byte size of a regular disk file or `"rom://..."` asset (`-1` on error).
 *
 * ### For Game Designers & Non-Programmers:
 * Reports exact file size before reading so oversized configs are caught safely.
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounded metadata query via `.rodata` descriptor or POSIX `stat()`. Returns `-1L` on miss.
 *
 * @param[in] path Disk filesystem path or `"rom://..."` virtual URI.
 * @return Non-negative byte size on success; `-1L` if missing or not a regular file.
 */
long io_fallback_fs_get_size(const char *path);

/**
 * @brief Inspects POSIX path or `"rom://..."` asset into `io_fb_file_info_t`.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves whether a file lives in Safe-Mode ROM memory or external storage and checks size.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zero-heap inspection populating `exists`, `is_regular`, `is_directory`, `is_rom_virtual`, and `size_bytes`.
 *
 * @param[in]  path     Disk filesystem path or `"rom://..."` URI.
 * @param[out] out_info Destination metadata struct zero-initialized and populated on hit.
 * @return true when path resolved to an existing file/directory/ROM entry; false otherwise.
 */
bool io_fallback_fs_inspect(const char *path, io_fb_file_info_t *out_info);

/**
 * @brief Bounded read supporting both regular files and `"rom://..."` `.rodata` streams.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads JSON manifests or Safe-Mode defaults into memory while guaranteeing oversized files
 * cannot overflow engine buffers.
 *
 * ### For Systems Programmers & Engine Architects:
 * Always NUL-terminates `buf` when `buf != NULL && buf_size > 0`. Enforces active
 * `max_config_bytes` ceiling (`IO_ERR_TRUNCATED_READ`), resets `*out_bytes_read = 0`
 * on truncation/failure, and records BIOS fault telemetry.
 *
 * @param[in]  path           Disk filesystem path or `"rom://..."` virtual URI.
 * @param[out] buf            Caller destination buffer receiving null-terminated payload.
 * @param[in]  buf_size       Byte capacity of `buf` (at least `2` for non-empty payloads).
 * @param[out] out_bytes_read Optional pointer receiving exact byte count read (`0` on error).
 * @return true if complete payload fit within `buf_size` and active policy cap; false on fault.
 * @triggers Records `LOG_SEV_WARNING`/`LOG_SEV_ERROR` to `LOG_CAT_IO` on read/truncation fault.
 */
bool io_fallback_fs_read_bounded(const char *path,
                                 char *buf,
                                 size_t buf_size,
                                 size_t *out_bytes_read);

/**
 * @brief Parameterized bounded write honoring explicit mode, flush, atomic rename & traversal policy.
 *
 * ### For Game Designers & Non-Programmers:
 * Shared engine save primitive allowing higher layers (like `fs_utils`) to honor active
 * atomic-save and security settings without duplicating disk code.
 *
 * ### For Systems Programmers & Engine Architects:
 * Used by both `io_fallback_fs_write_bounded()` and `fs_utils_write_file_contents()` so
 * atomic `.tl_tmp` sibling write + `rename()`, `fflush()`, and `..` rejection live once.
 *
 * @param[in] path            Target file path.
 * @param[in] data            Source payload pointer (`data_len` bytes).
 * @param[in] data_len        Number of bytes to write from `data`.
 * @param[in] dir_mode        POSIX directory permissions mask if parent creation is needed.
 * @param[in] flush_on_write  true to call `fflush()` prior to `fclose()`.
 * @param[in] atomic_write    true to stage `.tl_tmp` sibling file and atomically `rename()`.
 * @param[in] block_traversal true to reject `..` path traversal sequences.
 * @return true on complete write/rename; false on read-only lock, traversal guard, or I/O error.
 * @triggers Records `IO_ERR_WRITE_BLOCKED_RO` or `IO_ERR_PATH_OVERFLOW` into BIOS fault ring.
 */
bool io_fallback_fs_write_with_flags(const char *path,
                                     const char *data,
                                     size_t data_len,
                                     unsigned int dir_mode,
                                     bool flush_on_write,
                                     bool atomic_write,
                                     bool block_traversal);

/**
 * @brief Bounded write honoring Safe-Mode `read_only_mode`, `flush_on_write`, and `atomic_write`.
 *
 * ### For Game Designers & Non-Programmers:
 * Safely saves files when writable, or cleanly blocks writes during Floppy Read-Only Safe Mode
 * so write-protected media is never harmed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates directly to `io_fallback_fs_write_with_flags()` using live BIOS config flags,
 * blocking `..` traversal and enforcing `IO_ERR_WRITE_BLOCKED_RO` under Floppy RO latch.
 *
 * @param[in] path     Target disk path to write.
 * @param[in] data     Source byte buffer (`data_len` bytes).
 * @param[in] data_len Exact byte length to persist.
 * @return true on successful bounded write; false under RO lock, traversal guard, or I/O fault.
 * @triggers On `read_only_mode` block -> records `IO_ERR_WRITE_BLOCKED_RO` in `log_fallback` + `debug_fallback`.
 */
bool io_fallback_fs_write_bounded(const char *path, const char *data, size_t data_len);

/**
 * @brief Parameterized recursive `mkdir -p` honoring explicit POSIX permissions and traversal guard.
 *
 * ### For Game Designers & Non-Programmers:
 * Shared folder creation helper letting callers apply profile-defined directory permissions.
 *
 * ### For Systems Programmers & Engine Architects:
 * Shared single implementation serving both Safe-Mode BIOS and `fs_utils`.
 *
 * @param[in] dir_path        Directory path hierarchy to ensure on disk.
 * @param[in] posix_mode      POSIX mode mask applied to newly created segments (`0` -> default `0755`).
 * @param[in] block_traversal true to block `..` traversal segments cleanly.
 * @return true if directory hierarchy exists or was created cleanly; false on lock or fault.
 * @triggers Records `IO_ERR_WRITE_BLOCKED_RO` or `IO_ERR_PATH_OVERFLOW` into BIOS fault ring.
 */
bool io_fallback_fs_ensure_dir_mode(const char *dir_path,
                                    unsigned int posix_mode,
                                    bool block_traversal);

/**
 * @brief Bounded recursive `mkdir -p` honoring `IO_FB_PATH_MAX`, `posix_dir_mode`, and `read_only_mode`.
 *
 * ### For Game Designers & Non-Programmers:
 * Creates missing output directories automatically unless running in Read-Only Safe Mode.
 *
 * ### For Systems Programmers & Engine Architects:
 * Delegates cleanly to `io_fallback_fs_ensure_dir_mode()` using active `posix_dir_mode`
 * and `block_path_traversal` policy settings.
 *
 * @param[in] dir_path Directory path hierarchy to ensure on disk.
 * @return true if directory hierarchy exists cleanly; false on RO lock, overflow, or failure.
 * @triggers On `read_only_mode` block -> records `IO_ERR_WRITE_BLOCKED_RO` in `log_fallback` + `debug_fallback`.
 */
bool io_fallback_fs_ensure_dir(const char *dir_path);

/* --- Pillar 2: JSON Core BIOS Fallbacks --- */

/**
 * @brief Scans `json` for key `key` respecting string quotes & escape sequences (`O(N)`).
 *
 * ### For Game Designers & Non-Programmers:
 * Safely finds fields inside game configs even when string descriptions contain commas or colons.
 *
 * ### For Systems Programmers & Engine Architects:
 * Span-backed state machine skipping string literals and escaped quotes without recursion or heap.
 *
 * @param[in] json Null-terminated JSON buffer to scan.
 * @param[in] key  Target JSON key string without enclosing quotes.
 * @return Pointer to start of JSON value token, or NULL if absent/invalid.
 */
const char *io_fallback_json_find_key(const char *json, const char *key);

/**
 * @brief Locates a balanced `{...}` or `[...]` block for `key` within `max_json_depth`.
 *
 * ### For Game Designers & Non-Programmers:
 * Isolates a specific JSON section (such as `block_3_body`) without disturbing outer metadata.
 *
 * ### For Systems Programmers & Engine Architects:
 * Tracks quote escapes and bracket depth up to `IO_FB_MAX_JSON_DEPTH` (32), recording
 * `IO_ERR_JSON_DEPTH_EXCEEDED` into BIOS fault telemetry if depth is exceeded.
 *
 * @param[in]  json      Null-terminated JSON buffer.
 * @param[in]  key       Property key whose delimited object/array value should be located.
 * @param[in]  open_ch   Expected opening delimiter (`'{'` or `'['`).
 * @param[in]  close_ch  Matching closing delimiter (`'}'` or `']'`).
 * @param[out] out_start Receives pointer to `open_ch` on success.
 * @param[out] out_len   Receives total inclusive span byte length on success.
 * @return true when matched and balanced cleanly; false on miss or depth overflow.
 * @triggers On depth overflow -> `IO_ERR_JSON_DEPTH_EXCEEDED` fault into `log_fallback` + `debug_fallback`.
 */
bool io_fallback_json_find_block(const char *json,
                                 const char *key,
                                 char open_ch,
                                 char close_ch,
                                 const char **out_start,
                                 size_t *out_len);

/**
 * @brief Extracts null-terminated string value for `key` within `out_size`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads text fields such as profile names or schema identifiers cleanly from configs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Unescapes standard JSON string escapes into bounded caller storage with guaranteed NUL byte.
 *
 * @param[in]  json     Null-terminated JSON text.
 * @param[in]  key      Target property key name.
 * @param[out] out_buf  Caller buffer receiving decoded NUL-terminated string.
 * @param[in]  out_size Capacity of `out_buf` in bytes.
 * @return true when string field existed and fit within `out_size`; false otherwise.
 */
bool io_fallback_json_get_string(const char *json,
                                 const char *key,
                                 char *out_buf,
                                 size_t out_size);

/**
 * @brief Extracts integer value for `key` cleanly with `strtol` range validation.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads numeric limits (buffer caps, max turns, folder modes) safely from config files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Uses `strtol(..., 10)` checking overflow/non-numeric trailing tokens before writing `*out_val`.
 *
 * @param[in]  json    Null-terminated JSON text.
 * @param[in]  key     Target integer property key.
 * @param[out] out_val Receives parsed `int` value on success.
 * @return true when valid in-range integer existed for `key`; false otherwise.
 */
bool io_fallback_json_get_int(const char *json, const char *key, int *out_val);

/**
 * @brief Extracts boolean value (`true`/`false`) for `key`.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads on/off feature flags cleanly from engine configuration files.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks exact token boundaries so substrings like `true_ish` are never misparsed.
 *
 * @param[in]  json    Null-terminated JSON text.
 * @param[in]  key     Target boolean property key.
 * @param[out] out_val Receives parsed boolean (`true`/`false`) on success.
 * @return true when exact boolean token existed for `key`; false otherwise.
 */
bool io_fallback_json_get_bool(const char *json, const char *key, bool *out_val);

/* --- Pillar 3: VFS / PathFinder Core BIOS Fallbacks --- */

/**
 * @brief Returns currently active Safe-Mode VFS root path.
 *
 * ### For Game Designers & Non-Programmers:
 * Shows which data root folder the engine is currently loading assets from.
 *
 * ### For Systems Programmers & Engine Architects:
 * Auto-boots BIOS if unarmed and returns `.bss` string `s_io_fb.active_root`.
 *
 * @return Non-NULL NUL-terminated pointer to active BIOS VFS root string.
 */
const char *io_fallback_path_get_root(void);

/**
 * @brief Configures Safe-Mode VFS root path cleanly within `IO_FB_MAX_ROOT_LEN`.
 *
 * ### For Game Designers & Non-Programmers:
 * Redirects asset lookup to a custom workspace or reset to `"game/data"` when NULL is passed.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates byte ceiling against `IO_FB_MAX_ROOT_LEN` (`IO_ERR_PATH_OVERFLOW` on overflow).
 *
 * @param[in] root_path New VFS root directory string, or NULL/empty to reset to default.
 * @return true when root updated cleanly; false if path exceeds `IO_FB_MAX_ROOT_LEN`.
 * @triggers Records `IO_ERR_PATH_OVERFLOW` fault when `root_path` exceeds BIOS buffer capacity.
 */
bool io_fallback_path_set_root(const char *root_path);

/**
 * @brief Resolves `rel_path` across hydrated disk roots or falls back to `"rom://..."`.
 *
 * ### For Game Designers & Non-Programmers:
 * Finds config and data files across all engine/game folders regardless of where the
 * game was launched from, seamlessly substituting factory ROM data when offline.
 *
 * ### For Systems Programmers & Engine Architects:
 * Checks env var (`TL_DATA_ROOT`), active root, and all hydrated `candidate_roots x domain_dirs`
 * from `vfs_routes.json` (falling back cleanly to `"rom://..."` virtual URI). Rejects external
 * `..` parent traversal segments when `block_path_traversal == true`.
 *
 * @param[in]  domain   Logical VFS domain hint (`"config"`, `"assets"`, etc., or NULL).
 * @param[in]  rel_path Relative asset path or `"rom://..."` URI.
 * @param[out] out_path Caller buffer receiving resolved filesystem path or `"rom://..."` URI.
 * @param[in]  out_size Capacity of `out_path` in bytes.
 * @return true when target asset resolved on disk or in `.rodata` ROM; false otherwise.
 * @triggers Records `IO_ERR_PATH_OVERFLOW` when `..` parent traversal is intercepted.
 */
bool io_fallback_path_resolve(const char *domain,
                              const char *rel_path,
                              char *out_path,
                              size_t out_size);

/* --- Pillar 4: MasterLoader Core BIOS Fallbacks --- */

/**
 * @brief Parses an OmniCode 5-block JSON buffer directly in memory (`ALLOC_ZERO_HEAP`).
 *
 * ### For Game Designers & Non-Programmers:
 * Inspects any data JSON loaded in RAM to confirm it carries authentic metadata and payload blocks.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates `block_1_metadata` + `block_3_body` presence via single-pass span scanning, recording
 * `IO_ERR_JSON_KEY_MISSING` / `IO_ERR_JSON_SYNTAX` on structural violations.
 *
 * @param[out] loader    Caller-owned fallback loader view populated with body span & header.
 * @param[in]  json_text UTF-8 JSON document buffer in RAM (must outlive `loader` queries).
 * @param[in]  json_len  Exact payload byte count of `json_text`.
 * @return true when `block_3_body` located and `loader` initialized cleanly; false on error.
 * @triggers Records `IO_ERR_JSON_SYNTAX` into BIOS ring on missing `block_3_body`.
 */
bool io_fallback_loader_from_memory(io_fb_loader_t *loader,
                                    const char *json_text,
                                    size_t json_len);

/**
 * @brief Resolves `domain`/`rel_path` (disk OR `.rodata` ROM) and loads 5-block envelope.
 *
 * ### For Game Designers & Non-Programmers:
 * Opens and verifies any game or engine data file automatically from disk or built-in ROM.
 *
 * ### For Systems Programmers & Engine Architects:
 * Executes VFS resolution -> bounded disk/ROM read into `scratch_buf` -> `from_memory()`
 * with zero heap allocation and deterministic provenance (`loaded_from_rom`).
 *
 * @param[out] loader       Caller-owned fallback loader view initialized on success.
 * @param[in]  domain       Logical domain directory hint (`"config"`, `"entities"`, or NULL).
 * @param[in]  rel_path     Relative asset path or `"rom://..."` URI.
 * @param[out] scratch_buf  Caller-owned scratch buffer holding loaded JSON bytes.
 * @param[in]  scratch_size Byte capacity of `scratch_buf`.
 * @return true when resolved, bounded-read, and parsed cleanly; false on miss or error.
 */
bool io_fallback_loader_load(io_fb_loader_t *loader,
                             const char *domain,
                             const char *rel_path,
                             char *scratch_buf,
                             size_t scratch_size);

/**
 * @brief Extracts string from loaded `block_3_body` scope.
 *
 * ### For Game Designers & Non-Programmers:
 * Reads a named string property strictly out of the file's data payload (`block_3_body`).
 *
 * ### For Systems Programmers & Engine Architects:
 * Bounds scanning strictly inside `[body_start, body_start + body_len)` with zero heap copies.
 *
 * @param[in]  loader   Valid parsed `io_fb_loader_t` instance.
 * @param[in]  key      Target property key inside `block_3_body`.
 * @param[out] out_buf  Caller buffer receiving decoded string value.
 * @param[in]  out_size Capacity of `out_buf` in bytes.
 * @return true if property existed inside `block_3_body` and fit in `out_buf`; false otherwise.
 */
bool io_fallback_loader_get_string(const io_fb_loader_t *loader,
                                   const char *key,
                                   char *out_buf,
                                   size_t out_size);

/**
 * @brief Verifies loaded `header.schema_id` against `expected_schema_id`.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms the loaded JSON file actually matches the asset type the caller expects.
 *
 * ### For Systems Programmers & Engine Architects:
 * Exact string comparison on validated `loader->header.schema_id`.
 *
 * @param[in] loader             Valid parsed `io_fb_loader_t` instance.
 * @param[in] expected_schema_id Expected canonical `schema_id` string.
 * @return true when `loader->header.schema_id` equals `expected_schema_id`; false otherwise.
 */
bool io_fallback_loader_validate_schema(const io_fb_loader_t *loader,
                                        const char *expected_schema_id);

/* --- Fault Ring & Safe-Mode Downgrade --- */

/**
 * @brief Records a BIOS I/O diagnostic fault entry into `.bss` ring, `log_fallback`, and `debug_fallback`.
 *
 * ### For Game Designers & Non-Programmers:
 * Logs exactly why an I/O operation failed or was blocked (such as Floppy Read-Only protection)
 * into both the engine flight log and debugger history.
 *
 * ### For Systems Programmers & Engine Architects:
 * Appends `io_fallback_fault_entry_t` into 16-slot FIFO ring and forwards severity-mapped entry
 * to `log_fallback_record(LOG_CAT_IO)` + critical taxonomy codes to `debug_fallback_record_fault()`.
 *
 * @param[in] subsystem  Module tag reporting the fault (e.g. `"io_fallback.fs"`).
 * @param[in] operation  Operation or cog name that encountered the fault.
 * @param[in] message    Concise human-readable diagnostic reason.
 * @param[in] error_code Canonical `IO_ERR_*` hexadecimal code from Sub-Assembly 2.1.
 * @triggers Forwards telemetry to `log_fallback_record(LOG_CAT_IO)` and `debug_fallback_record_fault()`.
 */
void io_fallback_record_fault(const char *subsystem,
                              const char *operation,
                              const char *message,
                              uint16_t error_code);

/**
 * @brief Latches Safe-Mode degradation (`IO_FB_SINK_FLOPPY_RO` or `ROM_ONLY`) and logs reason.
 *
 * ### For Game Designers & Non-Programmers:
 * Smoothly shifts I/O into read-only Safe Mode when disk damage or corrupted limits occur so
 * gameplay completes safely without losing existing saves or crashing.
 *
 * ### For Systems Programmers & Engine Architects:
 * Increments `safe_downgrades`, switches `active_sink`, logs via `LOG_CAT_IO`, and updates BIOS ring.
 *
 * @triggers  When debug probes are active and `reason != NULL`, marks `safe_downgrades`
 *            counter for debugger triage without crashing caller execution.
 *
 * @param[in] reason         Human-readable Safe-Mode trigger reason.
 * @param[in] lock_read_only true to lock `FLOPPY_RO`; false for `ROM_ONLY`.
 * @return true after latching Safe Mode.
 */
bool io_fallback_downgrade_safe_mode(const char *reason, bool lock_read_only);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / CONTRACT VALIDATION & SELF-TEST
Stratum: Layer 0 (Core Utilities) | Subsystem: Tier-0 BIOS I/O & ROM Vault
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Validate policy contracts, verify every compiled `.rodata` ROM envelope,
and wire `io.fallback.*` probes cleanly into the central debugger.
--------------------------------------------------------------------------------
*/

/**
 * @brief Validates structural & numeric bounds of an `io_fallback_config_t` policy.
 *
 * ### For Game Designers & Non-Programmers:
 * Confirms that buffer limits (`small < medium < large <= max`), profile name, and default
 * asset directory are sane before activating a configuration.
 *
 * ### For Systems Programmers & Engine Architects:
 * Side-effect-free contract predicate invoking `io_fb_policy_bounds_valid()`.
 *
 * @param[in] cfg Candidate policy descriptor to validate.
 * @return true if valid (`io_fb_policy_bounds_valid` and non-empty profile/root).
 */
bool io_fallback_validate_policy_contract(const io_fallback_config_t *cfg);

/**
 * @brief Iterates every `.rodata` ROM asset and verifies 5-block envelope & `block_3_body`.
 *
 * ### For Game Designers & Non-Programmers:
 * Audits all built-in emergency I/O config backups at startup to ensure none were damaged.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses all `IO_CONFIG_ROM_ASSET_COUNT` entries via `io_fallback_loader_from_memory()`.
 *
 * @triggers `io.fallback.rom_integrity` (`DEBUG_TRIPWIRE_AUDIT_BREACH` -> `DEBUG_STEP_AWAIT`).
 * @return true if all ROM entries parse cleanly via `io_fallback_loader_from_memory()`.
 */
bool io_fallback_verify_rom_envelopes(void);

/**
 * @brief Registers Tier-0 I/O BIOS validation probes with the central debugger.
 *
 * ### For Game Designers & Non-Programmers:
 * Connects the emergency I/O ROM & floppy locks directly to the live debugger so
 * corrupted buffer profiles pause in Safe Mode (`DEBUG_STEP_AWAIT`) immediately.
 *
 * ### For Systems Programmers & Engine Architects:
 * Idempotent registration of `s_io_fb_probes[2]` (`Sub-Assembly 2.3`) into `debug.c`
 * covering `io.fallback.rom_integrity` and `io.fallback.policy_bounds`.
 */
void io_fallback_register_debug_probes(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Exercise all 4 Safe-Mode pillars, ROM vault hydration, and floppy locks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes comprehensive Tier-0 BIOS pre-flight audit across ROM & 4 I/O pillars.
 *
 * ### For Game Designers & Non-Programmers:
 * Runs automated self-checks across built-in ROM backups, virtual file routing, JSON reader,
 * and write-protected floppy guards before game loading begins.
 *
 * ### For Systems Programmers & Engine Architects:
 * Temporarily saves and restores `s_io_fb` across non-destructive verification checks.
 *
 * @triggers Validates `io.fallback.rom_integrity`, `io.fallback.policy_bounds`, read-only
 *           floppy write blocking, and `"rom://"` zero-disk ingestion cleanly.
 * @return `diag_result_t` with `SEVERITY_PASS` or diagnostic failure details.
 */
diag_result_t io_fallback_preflight(void);

/**
 * @brief High-level boolean audit gate wrapping `io_fallback_preflight()`.
 *
 * ### For Game Designers & Non-Programmers:
 * Returns simple true/false pass status from the BIOS pre-flight diagnostic check.
 *
 * ### For Systems Programmers & Engine Architects:
 * Boolean facade over `io_fallback_preflight().passed`.
 *
 * @return true if pre-flight passed cleanly; false otherwise.
 */
bool io_fallback_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES, CARDS & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Tier-0 BIOS I/O & ROM Vault
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Render terminal inspection cards and retrieve fault/ROM statistics.
--------------------------------------------------------------------------------
*/

/**
 * @brief Copies live Safe-Mode BIOS & ROM telemetry into caller `out_stats`.
 *
 * ### For Game Designers & Non-Programmers:
 * Provides counters of BIOS boots, Safe-Mode fallbacks, ROM hits, and I/O faults for HUDs.
 *
 * ### For Systems Programmers & Engine Architects:
 * O(1) `.bss` telemetry copy out of `s_io_fb`.
 *
 * @param[out] out_stats Destination stats record.
 * @return true on non-NULL `out_stats`; false otherwise.
 */
bool io_fallback_get_stats(io_fallback_stats_t *out_stats);

/**
 * @brief Copies inspected policy configuration into `out_cfg` (or default if unbooted).
 *
 * ### For Game Designers & Non-Programmers:
 * Captures the currently active Safe-Mode file limits for inspection reports.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns active `s_io_fb.config` if booted, else `s_io_fallback_template`.
 *
 * @param[out] out_cfg Destination inspected config record.
 * @return true on non-NULL `out_cfg`; false otherwise.
 */
bool io_fallback_execute_inspect(io_fallback_config_t *out_cfg);

/**
 * @brief Renders a fixed-width 60-column ASCII Safe-Mode I/O BIOS status card.
 *
 * ### For Game Designers & Non-Programmers:
 * Prints a clean summary showing active ROM files, safety buffer caps, and fault counts.
 *
 * ### For Systems Programmers & Engine Architects:
 * Writes formatted diagnostics into caller buffer (`buf_size >= 768` recommended).
 *
 * @param[in]  cfg      Config snapshot to display (`NULL` uses live BIOS state).
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Capacity of `out_buf` in bytes.
 * @return true if card fit cleanly without truncation; false otherwise.
 */
bool io_fallback_execute_dump_card(const io_fallback_config_t *cfg,
                                   char *out_buf,
                                   size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization, Floppy Mode & Teardown
Mission: Manage deterministic BIOS arming, diskless floppy Safe Mode, and cleanup.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes Tier-0 Safe-Mode BIOS boot from explicit config or ROM/hardcoded baseline.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts the emergency I/O system and loads all 36 data search directories and schema contracts
 * from built-in ROM blueprints so the game runs cleanly even without external folders.
 *
 * ### For Systems Programmers & Engine Architects:
 * Hydrates `s_io_fb.config`, `s_io_fb.vfs_routes` (from `rom://io/vfs_routes.json`), and
 * `s_io_fb.loader_contract` (from `rom://io/loader_contracts.json`), guarantees
 * `loaded_from_data = false`, and registers `io.fallback.*` validation probes cleanly.
 *
 * @param[in] opt_cfg Optional baseline configuration (NULL selects ROM/default template).
 * @return true on clean BIOS Safe-Mode boot.
 */
bool io_fallback_boot(const io_fallback_config_t *opt_cfg);

/**
 * @brief Boots into strict read-only Floppy Safe Mode (`disk_read_only = true`, `.rodata` ROM).
 *
 * ### For Game Designers & Non-Programmers:
 * Arms strict write-protected mode so the game can inspect ROM defaults and diagnostics without
 * ever creating folders or writing files to the host disk.
 *
 * ### For Systems Programmers & Engine Architects:
 * Attempts `floppy_safe` ROM hydration before falling back to `s_io_floppy_template`, locks
 * `IO_FB_SINK_FLOPPY_RO`, hydrates ROM VFS routes, and registers `io.fallback.*` probes.
 *
 * @return true on clean Floppy Safe-Mode activation.
 */
bool io_fallback_boot_floppy(void);

/**
 * @brief Tears down Tier-0 BIOS I/O runtime state cleanly.
 *
 * ### For Game Designers & Non-Programmers:
 * Resets emergency I/O metrics and Safe-Mode state cleanly at shutdown or between unit tests.
 *
 * ### For Systems Programmers & Engine Architects:
 * Preserves fault count clearing rules cleanly and marks `s_io_fb.booted = false`.
 */
void io_fallback_teardown(void);

/**
 * @brief Returns true if Tier-0 BIOS I/O Safe Mode is currently armed.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks whether the emergency I/O layer has already been activated.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure predicate reading `s_io_fb.booted`.
 *
 * @return true if `io_fallback_boot()` or `io_fallback_boot_floppy()` is currently active.
 */
bool io_fallback_is_booted(void);

/**
 * @brief Resets internal `.bss` singleton strictly for isolated unit-test fixtures.
 *
 * ### For Game Designers & Non-Programmers:
 * Restores a completely blank BIOS state between automated tests so tests never affect each other.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zeroes `s_io_fb` cleanly via `memset()` without touching ROM `.rodata` assets.
 */
void io_fallback_reset_for_testing(void);

#ifdef __cplusplus
}
#endif

#endif /* CORNERSTONE_IO_IO_FALLBACK_H */

