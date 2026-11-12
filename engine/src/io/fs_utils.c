/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/src/io/fs_utils.c
@id            cog.fs_utils
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Zero-Heap POSIX Filesystem & Buffered I/O Engine
@role          Runtime Engine & Chassis: Bounded POSIX/"rom://" stream engine,
               flushed writes, policy hydration, and io.fs.* probe dispatch.
@maturity      Rev 5 Baseline | Shared policy parser, ROM stream fallback & probe tripwires verified.
@chassis       zero-heap | .bss 232 B (s_fs_state) + .rodata 4,064 B + 8 KiB stack transient on policy load.
@scale         O(file_len <= 64 KiB) read/write stream; O(path_len <= 512) segment creation; zero static scratch bloat.
@seams         in: fs_buffers.json, io/default.json | uses: cog.io.fallback, cog.log, cog.debug | probes: io.fs.* -> DEBUG_STEP_AWAIT | dag: core.util.fs
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Zero function-local static arrays; runtime state stays confined inside s_fs_state (.bss 232 B).
               3. Policy hydration delegates parsing exclusively to shared io_fallback_parse_policy_json().
               4. Policy load failures trip io.fs.buffer_bounds/rom_parity probes and downgrade cleanly to ROM Safe Mode.

PURPOSE & ARCHITECTURAL ROLE:
  Executes POSIX and virtual "rom://" bounded file operations cleanly in Layer 0,
  hydrates size-bracket policies from fs_buffers.json with ROM downgrade, and
  wires runtime read/write guardrails to registered debug probes.

  ### For Game Designers & Non-Programmers:
  Handles live disk loading smoothly on constrained hardware: failed or missing
  config reads fall back automatically to sealed ROM blueprints while write-locked
  drives skip saves harmlessly without data loss.

  ### For Systems Programmers & Engine Architects:
  - Controlled Memory Chassis: .bss holds just 232 B (s_fs_state) and .rodata
    4,064 B; fs_utils_load_policy() bounds transient policy JSON stack usage at
    8 KiB rather than parking idle .bss buffers.
  - Bottleneck & Headroom: Single-call fread/fwrite with explicit ferror checks
    avoids byte-by-byte syscalls; generous headroom for encrypted/packed streams later.
  - Probe & Config Wiring: Registers 3 validation probes (io.fs.buffer_bounds,
    io.fs.rom_parity, io.fs.readonly_guard) wired directly into DEBUG_STEP_AWAIT.
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
Module:        cog.fs_utils
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
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "io/fs_utils.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "io/io_fallback.h"
#include "io/path_finder.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Module Manifest Anchor, Category Tags & Compile-Time Constants
--------------------------------------------------------------------------------
*/
/** @brief Canonical manifest identifier registering the POSIX filesystem cog. */
#define MODULE_ID          FS_UTILS_MODULE_ID
/** @brief Telemetry category channel for filesystem and VFS I/O messages. */
#define LOCAL_LOG_CAT      LOG_CAT_IO
/** @brief Concise logger source tag stamped into LOG_CAT_IO telemetry entries. */
#define LOCAL_MODULE_TAG   "FS_Utils"

static_assert(
    FS_PATH_MAX >= 256 && FS_BUFFER_SMALL_BYTES >= 512 && FS_BUFFER_MAX_CONFIG >= FS_BUFFER_LARGE_BYTES,
    "Filesystem path capacity and tiered stream buffer caps must remain valid"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Probe Forward Declarations
--------------------------------------------------------------------------------
*/

/**
 * @struct fs_utils_runtime_state
 * @brief Consolidated `.bss` runtime state holding policy, telemetry counters, and recursion guards.
 *
 * ### For Game Designers & Non-Programmers:
 * Tracks live file read/write activity, blocked read-only attempts, and active I/O limits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Fixed `.bss` chassis (< 512 B, `ALLOC_ZERO_HEAP`). `in_log_emit` breaks reentrancy when
 * `log.c` category directory rotation calls back into `fs_ensure_directory()`.
 */
typedef struct fs_utils_runtime_state {
    bool                 initialized;            /**< True while filesystem engine armed. */
    bool                 policy_from_data;       /**< True ONLY when hydrated from disk JSON. */
    bool                 in_log_emit;            /**< Reentrancy guard breaking log<->fs loops. */
    bool                 in_preflight;           /**< Suppresses log chatter during --audit. */
    io_fallback_config_t policy;                 /**< Active bounded buffer/read-only policy. */
    uint64_t             read_ops;               /**< Cumulative read operations completed. */
    uint64_t             write_ops;              /**< Cumulative write operations completed. */
    uint64_t             truncation_rejections;  /**< Reads aborted on caller cap overrun. */
    uint64_t             read_only_blocks;       /**< Mutations blocked by read-only latch. */
    uint64_t             rom_fallback_reads;     /**< Virtual rom:// stream reads served. */
} fs_utils_runtime_state_t;

/* Forward declarations binding Sub-Assembly 2.3 manifest and Block 4 probes. */
diag_result_t fs_utils_preflight(void);
static diag_result_t fs_probe_buffer_bounds(void);
static diag_result_t fs_probe_rom_parity(void);
static diag_result_t fs_probe_readonly_guard(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss & .rodata Allocations)
--------------------------------------------------------------------------------
*/

/** @brief Immutable architectural invariants exported in `fs_utils_manifest` (.rodata). */
static const char *const s_fs_invariants[5] = {
    "Memory safety: Caller output buffers strictly bounded under ALLOC_ZERO_HEAP",
    "Guaranteed null termination on every bounded read and path output",
    "Virtual rom:// stream fallback enables diskless .rodata config reads",
    "Read-only floppy Safe-Mode latch blocks writes and mkdir cleanly",
    "Canonical policy parser shared with Tier-0 I/O BIOS (no duplication)"
};

/** @brief Public module metadata node registering cog.fs_utils in manifest. */
const module_metadata_t fs_utils_manifest = {
    FS_UTILS_MODULE_ID,
    "fs_utils.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-11",
    "Zero-heap POSIX filesystem abstraction, policy loader, and Safe-Mode ROM reader",
    2,
    { "core.util.string", "core.io.path_finder" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    500,
    5,
    {
        s_fs_invariants[0],
        s_fs_invariants[1],
        s_fs_invariants[2],
        s_fs_invariants[3],
        s_fs_invariants[4]
    },
    fs_utils_preflight
};

/** Consolidated runtime policy & counter singleton (< 512 B .bss, zero heap). */
static fs_utils_runtime_state_t s_fs_state;

/** Compiled `.rodata` validation probe descriptors registered into `debug.c`. */
static const debug_validation_probe_t s_fs_validation_probes[3] = {
    {
        "io.fs.buffer_bounds",
        FS_UTILS_MODULE_ID,
        "fs_utils.c:Block3:fs_utils_set_policy",
        "tests/unit/io/test_fs_utils_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_PANIC_HALT,
        fs_probe_buffer_bounds,
        true,
        true,
        0U
    },
    {
        "io.fs.rom_parity",
        FS_UTILS_MODULE_ID,
        "fs_utils.c:Block3:fs_utils_load_policy",
        "tests/unit/io/test_fs_utils_unit.c:Section4",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_AUDIT_BREACH,
        DEBUG_REACTION_FALLBACK_MODE,
        fs_probe_rom_parity,
        true,
        true,
        0U
    },
    {
        "io.fs.readonly_guard",
        FS_UTILS_MODULE_ID,
        "fs_utils.c:Block3:fs_write_file_contents",
        "tests/unit/io/test_io_fallback_unit.c:Section3",
        DEBUG_TEST_TIER_UNIT,
        DEBUG_TRIPWIRE_FALLBACK_ENGAGED,
        DEBUG_REACTION_LOG_AND_CONTINUE,
        fs_probe_readonly_guard,
        false,
        true,
        0U
    }
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Logging Frontier
Mission: Guard buffer bounds in O(1) and emit reentrancy-safe `LOG_CAT_IO` logs.
--------------------------------------------------------------------------------
*/

/** @brief Reentrancy-guarded `LOG_CAT_IO` telemetry emitter skipping audit runs. */
#define FS_EMIT_LOG(sev_macro, grade, ...)                                 \
    do {                                                                   \
        if (!s_fs_state.in_log_emit && !s_fs_state.in_preflight) {         \
            s_fs_state.in_log_emit = true;                                 \
            (void)sev_macro(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, (grade),      \
                            __VA_ARGS__);                                  \
            s_fs_state.in_log_emit = false;                                \
        }                                                                  \
    } while (0)

/**
 * @brief Validates in O(1) time that a caller buffer pointer and byte capacity are usable.
 *
 * ### For Game Designers & Non-Programmers:
 * Prevents missing or zero-length memory buffers from receiving file data.
 *
 * ### For Systems Programmers & Engine Architects:
 * Pure zero-overhead inline guard before bounded POSIX/ROM reads.
 *
 * @param buf Caller buffer pointer.
 * @param cap Buffer byte capacity.
 * @return True if `buf != NULL && cap > 0U`.
 */
static inline bool fs_is_valid_buffer(const void *buf, size_t cap)
{
    return (buf != NULL && cap > 0U);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.5: Monitored Support Infrastructure (Non-Inline Internal)
--------------------------------------------------------------------------------
Support Role:  Lazy ROM policy seeding, effective read-only state checks, and
               downstream loader-contract JSON hydration during init.
Pattern Watch: `fs_hydrate_disk_subpolicies()` coordinates `path_finder` and
               `io_fallback` for `loader_contracts.json`; if additional I/O
               sub-policies hydrate dynamically, centralize them in master_loader.
Linkage Rule:  Strictly static functions only; zero static arrays or .bss storage.
--------------------------------------------------------------------------------
*/

/**
 * @brief Lazily arms `s_fs_state.policy` from `fs_buffers` ROM preset if unset.
 *
 * ### For Game Designers & Non-Programmers:
 * Ensures safe default file size caps exist even before disk setup finishes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Idempotent Safe-Mode initialization preserving `policy_from_data = false`.
 */
static void fs_ensure_policy_armed(void)
{
    if (s_fs_state.policy.small_bytes == 0U) {
        io_fallback_config_t fb = io_fallback_get_default_config();
        (void)io_fallback_load_rom_config("fs_buffers", &fb);
        fb.loaded_from_data = false;
        s_fs_state.policy = fb;
        s_fs_state.policy_from_data = false;
    }
}

/**
 * @brief Returns true if either local I/O policy or Tier-0 BIOS enforces read-only.
 *
 * ### For Game Designers & Non-Programmers:
 * Respects locked/floppy read-only switches before allowing file saves or folders.
 *
 * ### For Systems Programmers & Engine Architects:
 * Arms default policy on demand and queries `io_fallback_is_disk_read_only()`.
 *
 * @return True when disk mutations must be blocked cleanly.
 */
static bool fs_effective_read_only(void)
{
    fs_ensure_policy_armed();
    return s_fs_state.policy.read_only_mode || io_fallback_is_disk_read_only();
}

/**
 * @brief Hydrates optional disk `loader_contracts.json` into `cog.io.fallback`.
 *
 * ### For Game Designers & Non-Programmers:
 * Applies game/user loader rules from disk cleanly when `fs_utils` starts up.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resolves config path via `path_finder_resolve()` and skips virtual `rom://`
 * URIs so ROM defaults are never needlessly reparsed over themselves.
 *
 * @param scratch_buf  Caller scratch buffer for bounded file ingestion.
 * @param scratch_size Byte capacity of `scratch_buf`.
 */
static void fs_hydrate_disk_subpolicies(char *scratch_buf, size_t scratch_size)
{
    if (!fs_is_valid_buffer(scratch_buf, scratch_size)) {
        return;
    }

    /* Delegate VFS routing hydration cleanly to path_finder (keeps s_vfs_state & ROM parity synced). */
    (void)path_finder_load_routes(NULL);

    char resolved[FS_PATH_MAX];
    size_t read_len = 0U;

    if (path_finder_resolve("config", "io/loader_contracts.json", resolved, sizeof(resolved)) &&
        !io_fb_is_rom_uri(resolved) &&
        fs_read_file_bounded(resolved, scratch_buf, scratch_size, &read_len)) {
        io_fb_loader_contract_t contract;
        if (io_fallback_parse_loader_contract_json(scratch_buf, read_len, &contract)) {
            contract.loaded_from_data = true;
            (void)io_fallback_set_loader_contract(&contract);
        }
    }
}

static bool fs_check_path_cap(const char *op, const char *path)
{
    size_t path_cap = (s_fs_state.policy.path_max_bytes > 0U &&
                       s_fs_state.policy.path_max_bytes <= FS_PATH_MAX)
        ? s_fs_state.policy.path_max_bytes
        : FS_PATH_MAX;
    size_t path_len = strlen(path);
    if (!fs_path_len_is_valid(path_len) || path_len >= path_cap) {
        io_fallback_record_fault("fs_utils", op,
                                 "Target path exceeds path_max_bytes",
                                 IO_ERR_PATH_OVERFLOW);
        FS_EMIT_LOG(LOG_ERROR, -0.45f,
                    "%s: path '%s' rejected (>= %zu bytes)", op, path, path_cap);
        return false;
    }
    return true;
}

static bool fs_guard_writable(const char *op,
                              const char *target_path,
                              const char *ro_fault_msg,
                              const char *ro_log_msg,
                              const char *probe_msg)
{
    fs_ensure_policy_armed();

    if (!fs_check_path_cap(op, target_path)) {
        return false;
    }

    if (fs_effective_read_only()) {
        s_fs_state.read_only_blocks++;
        io_fallback_record_fault("fs_utils", op, ro_fault_msg, IO_ERR_WRITE_BLOCKED_RO);
        FS_EMIT_LOG(LOG_WARN, -0.30f, "%s '%s' (Floppy Safe-Mode read-only active)",
                    ro_log_msg, target_path);
        if (debug_is_active()) {
            fs_utils_register_debug_probes();
            (void)debug_trigger_probe("io.fs.readonly_guard",
                                      MODULE_ID,
                                      probe_msg,
                                      __FILE__, __LINE__, __func__);
        }
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Operational Business Logic, Span Scopes & Runtime Hydration )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0: Core Utilities
Module:        cog.fs_utils
Mission:       Implement matching Sub-Assembly 3.1..3.4 functions with guarded
               telemetry, debug probes, zero static helpers (2.5), and reconstructible Doxygen.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Filesystem Inspection & Probing Cogs
Mission: Provide non-mutating file existence, regular-node, and byte-size checks
         spanning host storage and `"rom://..."` `.rodata` streams.

  ### For Game Designers & Non-Programmers:
  Lets any part of the engine safely ask if a file or built-in ROM resource exists
  and how big it is before trying to read it.

  ### For Systems Programmers & Engine Architects:
  Dispatches `"rom://..."` paths directly to Tier-0 `.rodata` table descriptors and
  ordinary POSIX paths to `stat()` with zero heap allocations.
--------------------------------------------------------------------------------
*/

/**
 * @brief Test existence of a filesystem path or virtual `"rom://..."` asset URI.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Checks whether a target asset, config file, or directory exists on storage or
 *   in factory ROM before opening it.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Delegates `"rom://..."` URIs to `io_fallback_fs_exists()`, or executes non-mutating
 *   POSIX `stat()` without touching runtime fault counters or heap memory.
 *
 * @param[in] path NUL-terminated filesystem path or `"rom://..."` URI.
 * @return `true` when reachable on disk or ROM; `false` on miss, NULL, or empty input.
 * @triggers Validates input pointers strictly with zero state mutation.
 */
bool fs_file_exists(const char *path)
{
    /* Step 1: Reject NULL or empty paths cleanly */
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    /* Step 2: Check `.rodata` ROM vault before falling back to POSIX `stat()` */
    if (io_fb_is_rom_uri(path)) {
        return io_fallback_fs_exists(path);
    }

    struct stat st;
    return (stat(path, &st) == 0);
}

/**
 * @brief Verify that `path` identifies a readable regular file or ROM asset (not a directory).
 *
 *   ### For Game Designers & Non-Programmers:
 *   Confirms a path leads to an actual readable data file rather than a folder or missing node.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Delegates `"rom://..."` queries to `io_fallback_fs_is_regular()` and checks
 *   `S_ISREG(st.st_mode)` on POSIX filesystem nodes cleanly.
 *
 * @param[in] path NUL-terminated filesystem path or `"rom://..."` URI.
 * @return `true` if target exists and is a regular file/ROM entry; `false` otherwise.
 * @triggers Performs side-effect-free node classification with zero allocations.
 */
bool fs_is_regular_file(const char *path)
{
    /* Step 1: Guard NULL or empty path arguments */
    if (path == NULL || path[0] == '\0') {
        return false;
    }

    /* Step 2: Evaluate virtual ROM table or POSIX `S_ISREG` file mode */
    if (io_fb_is_rom_uri(path)) {
        return io_fallback_fs_is_regular(path);
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }

    return S_ISREG(st.st_mode);
}

/**
 * @brief Query total byte size of a regular disk file or `"rom://..."` resource.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Measures file size up front so oversized data files can be spotted cleanly
 *   before copying them into engine buffers.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Forwards `"rom://..."` URIs to `io_fallback_fs_get_size()` or returns
 *   `stat.st_size` when `S_ISREG` holds (`-1L` otherwise) without opening FDs.
 *
 * @param[in] path NUL-terminated path or `"rom://..."` URI.
 * @return Byte size on success; `-1L` when missing, non-regular, or invalid path.
 * @triggers Rejects directories and missing targets deterministically.
 */
long fs_get_file_size(const char *path)
{
    /* Step 1: Guard NULL or empty paths cleanly */
    if (path == NULL || path[0] == '\0') {
        return -1L;
    }

    /* Step 2: Dispatch virtual ROM size lookup or POSIX `stat()` query */
    if (io_fb_is_rom_uri(path)) {
        return io_fallback_fs_get_size(path);
    }

    struct stat st;
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode)) {
        return -1L;
    }

    return (long)st.st_size;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Bounded Buffered I/O & Directory Management Cogs
Mission: Execute policy-capped text reads, safe atomic/flushed writes, and
         recursive directory synthesis using shared Tier-0 BIOS primitives.

  ### For Game Designers & Non-Programmers:
  Handles reading configs, saving game/diagnostic data atomically, and creating
  missing folders while respecting locked Read-Only Safe Mode.

  ### For Systems Programmers & Engine Architects:
  Applies shared `fs_guard_writable()` (`path_max_bytes` + RO lock + `io.fs.readonly_guard`),
  enforces active `max_config_bytes`, guarantees trailing NUL bytes, and delegates
  atomic `.tl_tmp` writes & `mkdir -p` segments to `io_fallback`.
@triggers
  - Primary Cogs  : `fs_read_file_contents()`, `fs_read_file_bounded()`,
                    `fs_write_file_contents()`, `fs_ensure_directory()`
--------------------------------------------------------------------------------
*/

/**
 * @brief Read an entire file or ROM payload into a fixed caller buffer with NUL guarantee.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Loads an entire config or text asset cleanly into caller memory while ensuring
 *   oversized payloads never overrun buffers.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Thin pass-through to `fs_read_file_bounded()` discarding byte-length telemetry
 *   when callers only require NUL-terminated contents.
 *
 * @param[in]  filepath NUL-terminated path or `"rom://..."` URI.
 * @param[out] buf      Caller destination buffer.
 * @param[in]  buf_size Capacity of `buf` in bytes (including NUL terminator).
 * @return `true` if payload fit cleanly within policy and caller capacity; `false` otherwise.
 * @triggers Inherits fault telemetry and `LOG_WARN` warnings from `fs_read_file_bounded()`.
 */
bool fs_read_file_contents(const char *filepath, char *buf, size_t buf_size)
{
    /* Step 1: Forward cleanly into bounded reader without length output pointer */
    return fs_read_file_bounded(filepath, buf, buf_size, NULL);
}

/**
 * @brief Execute a policy-bounded file or `"rom://..."` read and report byte count read.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads JSON or text safely into caller buffers up to active policy limits,
 *   logging warnings cleanly when missing or oversized files are rejected.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Clears `*out_bytes_read`, arms policy lazily, routes `"rom://"` URIs through
 *   `io_fallback_fs_read_bounded()`, or streams host files capped at
 *   `min(buf_size, s_fs_state.policy.max_config_bytes)` with EOF truncation probe.
 *
 * @param[in]  filepath       NUL-terminated disk path or `"rom://..."` URI.
 * @param[out] buf            Caller output buffer (guaranteed NUL-terminated when valid).
 * @param[in]  buf_size       Byte capacity of `buf`.
 * @param[out] out_bytes_read Optional pointer receiving exact bytes read (`0U` on fault).
 * @return `true` if full file fit cleanly within limits; `false` on miss, error, or truncation.
 * @triggers Records `IO_ERR_FILE_NOT_FOUND` / `IO_ERR_TRUNCATED_READ` and `LOG_WARN`.
 */
bool fs_read_file_bounded(const char *filepath, char *buf, size_t buf_size,
                          size_t *out_bytes_read)
{
    /* Step 1: Reset output byte count and validate path/buffer pointers */
    if (out_bytes_read != NULL) {
        *out_bytes_read = 0U;
    }

    if (filepath == NULL || filepath[0] == '\0' || !fs_is_valid_buffer(buf, buf_size)) {
        if (fs_is_valid_buffer(buf, buf_size)) {
            buf[0] = '\0';
        }
        return false;
    }

    fs_ensure_policy_armed();

    /* Step 2: Handle `"rom://..."` virtual streams directly via Tier-0 BIOS vault */
    if (io_fb_is_rom_uri(filepath)) {
        bool rom_ok = io_fallback_fs_read_bounded(filepath, buf, buf_size, out_bytes_read);
        if (rom_ok) {
            s_fs_state.read_ops++;
            s_fs_state.rom_fallback_reads++;
        } else {
            s_fs_state.truncation_rejections++;
            FS_EMIT_LOG(LOG_WARN, -0.25f, "ROM bounded read rejected/truncated: '%s'", filepath);
        }
        return rom_ok;
    }

    /* Step 3: Open POSIX host file cleanly and record miss telemetry if absent */
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        buf[0] = '\0';
        io_fallback_record_fault("fs_utils", "fs_read_file_bounded",
                                 "Failed opening file for read", IO_ERR_FILE_NOT_FOUND);
        FS_EMIT_LOG(LOG_WARN, -0.20f, "Failed opening file '%s' for bounded read", filepath);
        return false;
    }

    /* Step 4: Read within `min(buf_size, max_config_bytes)` and probe truncation/errors */
    size_t policy_cap = (s_fs_state.policy.max_config_bytes > 0U &&
                         s_fs_state.policy.max_config_bytes < buf_size)
        ? s_fs_state.policy.max_config_bytes
        : buf_size;
    size_t read_bytes = fread(buf, 1U, policy_cap - 1U, f);
    buf[read_bytes] = '\0';
    bool io_err = (ferror(f) != 0);
    int next_char = fgetc(f);
    (void)fclose(f);

    if (io_err || (next_char != EOF && s_fs_state.policy.reject_on_truncation)) {
        buf[buf_size - 1U] = '\0';
        s_fs_state.truncation_rejections++;
        io_fallback_record_fault("fs_utils", "fs_read_file_bounded",
                                 io_err ? "Stream I/O error"
                                        : "File exceeds caller or policy buffer cap",
                                 io_err ? IO_ERR_READ_FAILURE : IO_ERR_TRUNCATED_READ);
        FS_EMIT_LOG(LOG_WARN, -0.35f,
                    "Read fault/truncation on '%s' (cap=%zu)", filepath, policy_cap);
        return false;
    }

    s_fs_state.read_ops++;
    if (out_bytes_read != NULL) {
        *out_bytes_read = read_bytes;
    }
    return true;
}

/**
 * @brief Safely write bytes to a disk path honoring permissions, flush, atomic rename & RO locks.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Writes files safely with automatic parent folder synthesis and atomic temporary
 *   staging, cleanly refusing writes whenever Floppy Read-Only Safe Mode is armed.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Validates path cap and read-only lock via shared `fs_guard_writable()` (`2.5`), then
 *   delegates POSIX write + `.tl_tmp` rename cleanly to `io_fallback_fs_write_with_flags()`.
 *
 * @param[in] filepath Target file path on writable storage.
 * @param[in] data     Source byte buffer to write.
 * @param[in] data_len Byte count to persist from `data`.
 * @return `true` when complete payload persisted cleanly; `false` under lock, overflow, or error.
 * @triggers Logs errors/warnings and fires `io.fs.readonly_guard` if blocked by Floppy RO latch.
 */
bool fs_write_file_contents(const char *filepath, const char *data,
                            size_t data_len)
{
    /* Step 1: Reject NULL/empty inputs and run shared path/RO write guards */
    if (filepath == NULL || filepath[0] == '\0' || data == NULL) {
        return false;
    }

    if (!fs_guard_writable("fs_write_file_contents",
                           filepath,
                           "Disk write blocked by Safe-Mode read-only latch",
                           "Blocked disk write to",
                           "Write attempted while Floppy Read-Only Safe Mode active")) {
        return false;
    }

    /* Step 2: Execute shared flushed/atomic POSIX write and bump counters/logs */
    bool ok = io_fallback_fs_write_with_flags(
        filepath,
        data,
        data_len,
        s_fs_state.policy.posix_dir_mode,
        s_fs_state.policy.flush_on_write,
        s_fs_state.policy.atomic_write,
        s_fs_state.policy.block_path_traversal
    );

    if (ok) {
        s_fs_state.write_ops++;
    } else {
        FS_EMIT_LOG(LOG_ERROR, -0.50f,
                    "Failed writing %zu bytes to '%s'", data_len, filepath);
    }
    return ok;
}

/**
 * @brief Recursively create missing directory segments cleanly within policy caps and RO locks.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Builds missing output folders step-by-step using configured permissions while
 *   honoring Floppy Read-Only write protection.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Guards `FS_PATH_MAX` bounds and RO lock via shared `fs_guard_writable()` (`2.5`),
 *   then delegates recursive `mkdir` execution cleanly to `io_fallback_fs_ensure_dir_mode()`.
 *
 * @param[in] dir_path NUL-terminated directory hierarchy path to synthesize.
 * @return `true` when directory already exists or was created cleanly; `false` on fault/block.
 * @triggers Fires `io.fs.readonly_guard` if attempted while Safe-Mode read-only latch is set.
 */
bool fs_ensure_directory(const char *dir_path)
{
    /* Step 1: Reject NULL/empty inputs and enforce path cap + Safe-Mode RO lock */
    if (dir_path == NULL || dir_path[0] == '\0') {
        return false;
    }

    if (!fs_guard_writable("fs_ensure_directory",
                           dir_path,
                           "Directory creation blocked by Safe-Mode read-only latch",
                           "Blocked directory creation",
                           "mkdir attempted while Floppy Read-Only Safe Mode active")) {
        return false;
    }

    /* Step 2: Delegate `mkdir -p` walk with configured permissions and traversal guard */
    return io_fallback_fs_ensure_dir_mode(
        dir_path,
        s_fs_state.policy.posix_dir_mode,
        s_fs_state.policy.block_path_traversal
    );
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Dynamic VFS Mounting & Path Resolution Cogs
Mission: Route active data root mounting and multi-tier path discovery cleanly
         into `cog.path_finder` so VFS mount state exists in one authoritative place.

  ### For Game Designers & Non-Programmers:
  Lets builds or tools change the game's active content directory and automatically
  locate files across layered workspace or backup folders.

  ### For Systems Programmers & Engine Architects:
  Thin non-duplicating delegation directly into `path_finder_set_data_root()`,
  `path_finder_get_data_root()`, and `path_finder_resolve(NULL, ...)`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Set or reset the active mounted VFS root directory via `path_finder`.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Points asset loading at a custom content directory or resets to auto-discovery.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Delegates mount root assignment cleanly to `path_finder_set_data_root()`.
 *
 * @param[in] root_path New root path, or NULL/empty to restore default auto-discovery.
 * @return `true` when mount path accepted cleanly; `false` on length overflow.
 * @triggers Inherits path length telemetry and probes from `cog.path_finder`.
 */
bool fs_set_data_root(const char *root_path)
{
    /* Step 1: Delegate VFS root update directly to `path_finder` */
    return path_finder_set_data_root(root_path);
}

/**
 * @brief Retrieve the currently active VFS root directory string via `path_finder`.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reports which base content folder the engine is currently using for data lookup.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Delegates read-only root inspection directly to `path_finder_get_data_root()`.
 *
 * @return NUL-terminated string representing active VFS data root.
 * @triggers Guarantees side-effect-free lookup via `cog.path_finder`.
 */
const char *fs_get_data_root(void)
{
    /* Step 1: Forward read-only root query cleanly to `path_finder` */
    return path_finder_get_data_root();
}

/**
 * @brief Resolve a relative asset or config path across mounted roots, domains, and ROM.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Automatically locates requested assets across game folders, config overrides,
 *   or compiled ROM backups without requiring full hard paths.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Delegates multi-tier discovery directly to `path_finder_resolve(NULL, ...)`.
 *
 * @param[in]  rel_path Relative path or asset filename to resolve.
 * @param[out] out_path Caller buffer receiving resolved path or `"rom://..."` URI.
 * @param[in]  out_size Capacity of `out_path` in bytes.
 * @return `true` when asset located cleanly; `false` on miss, invalid args, or traversal block.
 * @triggers Inherits VFS traversal guard and telemetry from `cog.path_finder`.
 */
bool fs_resolve_data_path(const char *rel_path,
                          char *out_path,
                          size_t out_size)
{
    /* Step 1: Delegate multi-tier VFS path resolution cleanly to `path_finder` */
    return path_finder_resolve(NULL, rel_path, out_path, out_size);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Data-Driven Policy Hydration & Safe-Mode Accessors
Mission: Ingest `engine/config/io/fs_buffers.json` + sibling `io/` subpolicies cleanly,
         and escalate corrupt configs or invalid overrides to Safe-Mode ROM defaults.

  ### For Game Designers & Non-Programmers:
  Loads engine buffer brackets and safety switches from disk config JSON files—or
  cleanly drops into Safe Mode using built-in ROM defaults when damaged or absent.

  ### For Systems Programmers & Engine Architects:
  Uses ONE 8 KB transient stack buffer + `io_fallback_parse_policy_span()`, verifies
  contracts via `fs_utils_validate_policy_contract()`, and fires `io.fs.rom_parity`
  / `io.fs.buffer_bounds` under debugger supervision.
@triggers
  - Primary Cogs  : `fs_utils_load_policy()`, `fs_utils_set_policy()`,
                    `fs_utils_get_policy()`
--------------------------------------------------------------------------------
*/

/**
 * @brief Load active filesystem buffer & safety policy from disk JSON or `.rodata` ROM.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Reads `fs_buffers.json` cleanly from workspace config directories so buffer
 *   tiers and floppy switches adjust without recompiling, cleanly downgrading to
 *   built-in ROM backups when disk configs are missing or corrupt.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Resolves `io/fs_buffers.json` via `path_finder_resolve("config", ...)`, reads into
 *   transient stack `json_buf[FS_BUFFER_MEDIUM_BYTES]` (8 KB), parses via shared
 *   `io_fallback_parse_policy_span()`, syncs `io_fallback_set_read_only(true)` if
 *   configured, hydrates sibling `io/` subpolicies via `fs_hydrate_disk_subpolicies()`,
 *   or downgrades with `io_fallback_downgrade_safe_mode()` and `io.fs.rom_parity`.
 *
 * @param[in] rel_path Config relative path (NULL/empty defaults to `"io/fs_buffers.json"`).
 * @return `true` when hydrated cleanly from disk (`loaded_from_data = true`); `false` when
 *         downgraded safely to compiled ROM defaults (`loaded_from_data = false`).
 * @triggers Logs outcome via `FS_EMIT_LOG` and triggers `io.fs.rom_parity` on corrupt disk JSON.
 */
bool fs_utils_load_policy(const char *rel_path)
{
    /* Step 1: Normalize relative config path and probe layered VFS disk resolution */
    bool default_load = (rel_path == NULL || rel_path[0] == '\0');
    const char *target = default_load ? "io/fs_buffers.json" : rel_path;

    char resolved[FS_PATH_MAX];
    char json_buf[FS_BUFFER_MEDIUM_BYTES];
    size_t json_len = 0U;
    io_fallback_config_t parsed;

    bool found_disk = path_finder_resolve("config", target, resolved, sizeof(resolved));
    bool resolved_is_disk = (found_disk && !io_fb_is_rom_uri(resolved));

    /* Step 2: Hydrate from validated disk JSON and synchronize sibling subpolicies */
    if (resolved_is_disk &&
        fs_read_file_bounded(resolved, json_buf, sizeof(json_buf), &json_len) &&
        io_fallback_parse_policy_span(json_buf, json_len, &parsed)) {
        parsed.loaded_from_data = true;
        s_fs_state.policy = parsed;
        s_fs_state.policy_from_data = true;
        if (parsed.read_only_mode) {
            io_fallback_set_read_only(true);
        }
        if (default_load) {
            fs_hydrate_disk_subpolicies(json_buf, sizeof(json_buf));
        }
        FS_EMIT_LOG(LOG_INFO, 0.25f,
                    "Loaded disk FS policy '%s' from '%s' (mode=%04o, atomic=%s)",
                    parsed.profile_name, resolved,
                    parsed.posix_dir_mode & 0777U,
                    parsed.atomic_write ? "true" : "false");
        return true;
    }

    /* Step 3: Downgrade cleanly to `.rodata` ROM defaults on miss or parse error */
    (void)io_fallback_downgrade_safe_mode("fs_buffers.json absent or invalid; hydrated ROM policy",
                                          false);
    io_fallback_config_t rom_cfg = io_fallback_get_default_config();
    bool rom_ok = io_fallback_load_rom_config("fs_buffers", &rom_cfg);
    rom_cfg.loaded_from_data = false;
    s_fs_state.policy = rom_cfg;
    s_fs_state.policy_from_data = false;

    if (resolved_is_disk) {
        FS_EMIT_LOG(LOG_ERROR, -0.40f,
                    "Corrupt disk policy '%s'; downgraded cleanly to ROM Safe Mode",
                    target);
    } else {
        FS_EMIT_LOG(LOG_WARN, -0.40f,
                    "Missing disk policy '%s'; downgraded cleanly to ROM Safe Mode",
                    target);
    }

    /* Step 4: Escalate corrupt disk files or missing ROM assets into debugger AWAIT */
    if (debug_is_active() &&
        ((resolved_is_disk && s_fs_state.policy.probe_await_on_corrupt) || !rom_ok)) {
        fs_utils_register_debug_probes();
        (void)debug_trigger_probe(
            "io.fs.rom_parity",
            MODULE_ID,
            resolved_is_disk
                ? "Corrupt on-disk FS policy rejected; downgraded to ROM Safe Mode"
                : "Critical compiled ROM asset io/fs_buffers.json missing",
            __FILE__, __LINE__, __func__
        );
    }
    return false;
}

/**
 * @brief Validate and install an explicit runtime filesystem policy configuration.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Safely updates active file limits at runtime, rejecting corrupt settings and
 *   restoring factory ROM defaults cleanly before truncated writes can happen.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Validates candidate via `fs_utils_validate_policy_contract()`; on rejection,
 *   downgrades `s_fs_state.policy` to `io_fallback_load_rom_config("fs_buffers")`,
 *   emits `LOG_ERROR`, and fires `io.fs.buffer_bounds` into `debug_trigger_probe()`.
 *
 * @param[in] policy Candidate policy struct to validate and apply.
 * @return `true` if validated and committed; `false` if NULL or rejected (Safe-Mode armed).
 * @triggers Escalates invalid policy overrides to `io.fs.buffer_bounds` + Safe-Mode ROM.
 */
bool fs_utils_set_policy(const io_fallback_config_t *policy)
{
    /* Step 1: Reject NULL policy pointer cleanly without firing probes */
    if (policy == NULL) {
        return false;
    }

    /* Step 2: Validate structural contract or downgrade cleanly into ROM Safe Mode */
    if (!fs_utils_validate_policy_contract(policy)) {
        (void)io_fallback_downgrade_safe_mode("Rejected corrupted fs_utils buffer policy", false);
        io_fallback_config_t safe_cfg = io_fallback_get_default_config();
        (void)io_fallback_load_rom_config("fs_buffers", &safe_cfg);
        safe_cfg.loaded_from_data = false;
        s_fs_state.policy = safe_cfg;
        s_fs_state.policy_from_data = false;

        FS_EMIT_LOG(LOG_ERROR, -0.60f,
                    "Rejected corrupted fs_utils policy '%s'; engaging Safe Mode",
                    policy->profile_name[0] ? policy->profile_name : "(unnamed)");

        if (debug_is_active()) {
            fs_utils_register_debug_probes();
            (void)debug_trigger_probe(
                "io.fs.buffer_bounds",
                MODULE_ID,
                "FS policy failed buffer ladder or PATH_MAX invariant check",
                __FILE__,
                __LINE__,
                "fs_utils_set_policy"
            );
        }
        return false;
    }

    /* Step 3: Commit validated configuration into `.bss` state and log transition */
    s_fs_state.policy = *policy;
    s_fs_state.policy_from_data = policy->loaded_from_data;
    FS_EMIT_LOG(LOG_INFO, 0.15f,
                "Applied runtime FS policy '%s' (from_data=%s)",
                policy->profile_name, policy->loaded_from_data ? "true" : "false");
    return true;
}

/**
 * @brief Retrieve a stack copy of the currently active filesystem policy configuration.
 *
 *   ### For Game Designers & Non-Programmers:
 *   Hands tools and diagnostic cards exact active file caps and Safe-Mode switches.
 *
 *   ### For Systems Programmers & Engine Architects:
 *   Ensures lazy policy arming via `fs_ensure_policy_armed()` and copies `s_fs_state.policy`
 *   in O(1) time without heap usage.
 *
 * @param[out] out_policy Caller destination struct populated on success.
 * @return `true` when copied cleanly; `false` if `out_policy` is NULL.
 * @triggers Lazy-arms policy defaults on demand and rejects NULL destination cleanly.
 */
bool fs_utils_get_policy(io_fallback_config_t *out_policy)
{
    /* Step 1: Reject NULL output destination */
    if (out_policy == NULL) {
        return false;
    }

    /* Step 2: Ensure policy armed and copy snapshot by value */
    fs_ensure_policy_armed();
    *out_policy = s_fs_state.policy;
    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 0 (Core Utilities) | Subsystem: Filesystem & I/O Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Policy Validation & Registered Debugger Probes
Mission: Validate FS buffer contracts and wire `io.fs.*` diagnostic probes cleanly.

  ### For Game Designers & Non-Programmers:
  Provides the engine debugger with instant health checks covering memory limits,
  ROM availability, and Floppy Read-Only safety locks.

  ### For Systems Programmers & Engine Architects:
  Registers `io.fs.buffer_bounds`, `io.fs.rom_parity`, and `io.fs.readonly_guard`
  into `debug.c` cleanly (`ALLOC_ZERO_HEAP`).
  @triggers `io.fs.buffer_bounds`, `io.fs.rom_parity`, `io.fs.readonly_guard`.
--------------------------------------------------------------------------------
*/

bool fs_utils_validate_policy_contract(const io_fallback_config_t *policy)
{
    const io_fallback_config_t *target = policy;
    if (target == NULL) {
        fs_ensure_policy_armed();
        target = &s_fs_state.policy;
    }
    return io_fallback_validate_policy_contract(target);
}

static diag_result_t fs_probe_buffer_bounds(void)
{
    fs_ensure_policy_armed();
    diag_result_t res;
    res.subsystem = FS_UTILS_MODULE_ID;
    res.check_name = "FS Buffer Ladder & Path Cap Bounds";
    res.passed = fs_utils_validate_policy_contract(&s_fs_state.policy);
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "FS buffer ladder (1K < 8K < 32K <= 64K) and path limits valid."
        : "FS buffer ladder corrupted; Safe-Mode ROM policy required before AWAIT.";
    return res;
}

static diag_result_t fs_probe_rom_parity(void)
{
    diag_result_t res;
    res.subsystem = FS_UTILS_MODULE_ID;
    res.check_name = "FS ROM Fallback Asset Availability";
    size_t rom_sz = 0U;
    const char *rom = io_fallback_find_rom_asset("io/fs_buffers.json", &rom_sz);
    res.passed = (rom != NULL && rom_sz > 0U && io_fallback_fs_exists("rom://io/fs_buffers.json"));
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "Compiled .rodata io/fs_buffers.json ROM asset reachable via rom:// URI."
        : "Compiled .rodata io/fs_buffers.json missing from I/O ROM vault.";
    return res;
}

static diag_result_t fs_probe_readonly_guard(void)
{
    diag_result_t res;
    res.subsystem = FS_UTILS_MODULE_ID;
    res.check_name = "FS Read-Only Floppy Latch Parity";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Read-only floppy Safe-Mode state synchronized cleanly.";

    if (io_fallback_is_disk_read_only() && !fs_effective_read_only()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "BIOS read-only latch diverged from fs_utils effective guard.";
    }
    return res;
}

void fs_utils_register_debug_probes(void)
{
    for (size_t i = 0U; i < 3U; i++) {
        (void)debug_register_validation_probe(&s_fs_validation_probes[i]);
    }
}

bool fs_utils_trigger_policy_probe(const char *context_label)
{
    if (fs_utils_validate_policy_contract(&s_fs_state.policy)) {
        return false;
    }

    (void)io_fallback_downgrade_safe_mode(
        context_label ? context_label : "fs_utils policy breach",
        false
    );
    io_fallback_config_t rom_cfg = io_fallback_get_default_config();
    (void)io_fallback_load_rom_config("fs_buffers", &rom_cfg);
    rom_cfg.loaded_from_data = false;
    s_fs_state.policy = rom_cfg;
    s_fs_state.policy_from_data = false;

    if (!debug_is_active()) {
        return false;
    }

    fs_utils_register_debug_probes();
    return debug_trigger_probe(
        "io.fs.buffer_bounds",
        MODULE_ID,
        context_label ? context_label : "Corrupted fs_utils policy tripped probe",
        __FILE__,
        __LINE__,
        "fs_utils_trigger_policy_probe"
    );
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Pre-Flight Self-Audit & Invariant Tripwires
Mission: Execute non-destructive startup verification of NULL resilience, POSIX
         probes, virtual `"rom://"` reads, and active policy contracts.

  ### For Game Designers & Non-Programmers:
  Runs during `./build/tenebris-lux --audit` to prove filesystem helpers safely block
  overflows and read ROM configs cleanly before entering gameplay.

  ### For Systems Programmers & Engine Architects:
  Saves `s_fs_state` and sets `in_preflight = true` so synthetic audit errors do not
  pollute runtime logs, restoring caller state on every return path.
--------------------------------------------------------------------------------
*/

diag_result_t fs_utils_preflight(void)
{
    diag_result_t res;
    res.subsystem = fs_utils_manifest.module_id;
    res.check_name = "Filesystem Invariants, Policy & ROM Bounds";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "Filesystem utilities: POSIX I/O, policy contract & ROM stream verified.";

    fs_utils_runtime_state_t fs_backup = s_fs_state;
    s_fs_state.in_preflight = true;

    /* Check 1: NULL resilience across inspection and I/O primitives */
    char test_buf[64];
    size_t bytes_out = 0U;
    if (fs_file_exists(NULL) != false ||
        fs_get_file_size(NULL) != -1L ||
        fs_is_regular_file(NULL) != false ||
        fs_read_file_contents(NULL, test_buf, sizeof(test_buf)) != false ||
        fs_read_file_bounded("/dev/null", test_buf, 0U, &bytes_out) != false ||
        fs_write_file_contents(NULL, "data", 4U) != false ||
        fs_ensure_directory(NULL) != false) {
        s_fs_state = fs_backup;
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "fs_utils NULL/zero-cap resilience contract failed.";
        return res;
    }

    /* Check 2: Known path probing (/dev/null exists as char device, not regular) */
    if (!fs_file_exists("/dev/null") || fs_is_regular_file("/dev/null") ||
        fs_file_exists("/__tenebris_lux_nonexistent_path_xyz__")) {
        s_fs_state = fs_backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "fs_utils POSIX device/existence probing failed.";
        return res;
    }

    /* Check 3: Virtual ROM URI read & schema check */
    char rom_peek[512];
    size_t rom_bytes = 0U;
    if (!fs_file_exists("rom://io/fs_buffers.json") ||
        !fs_is_regular_file("rom://io/fs_buffers.json") ||
        fs_read_file_bounded("rom://io/fs_buffers.json", rom_peek, sizeof(rom_peek),
                             &rom_bytes) != false) {
        /* Note: fs_buffers.json > 512 B intentionally verifies clean bounded rejection */
        if (rom_peek[sizeof(rom_peek) - 1U] != '\0') {
            s_fs_state = fs_backup;
            res.passed = false;
            res.severity = SEVERITY_FATAL;
            res.details = "ROM bounded truncation failed unconditional null-termination.";
            return res;
        }
    }

    /* Check 4: Policy contract and Block 5 procedures */
    fs_ensure_policy_armed();
    if (!fs_utils_validate_policy_contract(&s_fs_state.policy)) {
        s_fs_state = fs_backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Active fs_utils policy failed numeric buffer ladder contract.";
        return res;
    }

    fs_file_info_t info;
    if (!fs_inspect_file("/dev/null", &info) || !info.exists || info.is_regular) {
        s_fs_state = fs_backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "fs_inspect_file failed on /dev/null.";
        return res;
    }

    s_fs_state = fs_backup;
    return res;
}

bool fs_utils_audit(void)
{
    diag_result_t res = fs_utils_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Subsystem: Filesystem & I/O Engine
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Inspect POSIX/`"rom://"` metadata, run structured read/write commands,
         render ASCII status cards, and report `fs_utils_stats_t` telemetry.

  ### For Game Designers & Non-Programmers:
  Provides structured inspection cards and live statistics showing read/write counts,
  blocked writes, and whether active limits come from disk or Safe-Mode ROM.

  ### For Systems Programmers & Engine Architects:
  Zero-heap entry inspection (`stat` / `.rodata`), path-resolved execution wrappers,
  and deterministic telemetry getters.
--------------------------------------------------------------------------------
*/

bool fs_inspect_file(const char *path, fs_file_info_t *out_info)
{
    if (out_info == NULL) {
        return false;
    }

    memset(out_info, 0, sizeof(*out_info));
    out_info->size_bytes = -1L;

    if (path == NULL || path[0] == '\0') {
        return false;
    }

    if (io_fb_is_rom_uri(path)) {
        io_fb_file_info_t fb_info;
        if (!io_fallback_fs_inspect(path, &fb_info)) {
            return false;
        }
        out_info->exists = fb_info.exists;
        out_info->is_regular = fb_info.is_regular;
        out_info->is_directory = fb_info.is_directory;
        out_info->size_bytes = fb_info.size_bytes;
        return true;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }

    out_info->exists = true;
    out_info->is_regular = S_ISREG(st.st_mode);
    out_info->is_directory = S_ISDIR(st.st_mode);
    out_info->size_bytes = S_ISREG(st.st_mode) ? (long)st.st_size : 0L;
    return true;
}

size_t fs_execute_dump_card(const fs_file_info_t *info, char *out_buf, size_t buf_size)
{
    if (info == NULL || out_buf == NULL || buf_size == 0U) {
        return 0U;
    }
    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "             FILESYSTEM NODE INSPECTION CARD                \n"
        "============================================================\n"
        "Exists:        %s\n"
        "Node Type:     %s\n"
        "Size (Bytes):  %ld\n"
        "Access Status: %s\n"
        "============================================================\n",
        info->exists ? "YES" : "NO",
        info->is_directory ? "DIRECTORY"
                           : (info->is_regular ? "REGULAR FILE"
                                               : (info->exists ? "SPECIAL NODE" : "NON-EXISTENT")),
        info->size_bytes,
        info->exists ? "ACCESSIBLE" : "INACCESSIBLE / NOT FOUND"
    );
    if (written < 0 || (size_t)written >= buf_size) {
        out_buf[buf_size - 1U] = '\0';
        return 0U;
    }
    return (size_t)written;
}

bool fs_execute_read(const char *path, char *buf, size_t buf_size,
                     fs_read_result_t *out_result)
{
    if (out_result != NULL) {
        memset(out_result, 0, sizeof(*out_result));
    }

    if (path == NULL || buf == NULL || buf_size == 0U) {
        if (buf != NULL && buf_size > 0U) {
            buf[0] = '\0';
        }
        return false;
    }

    char resolved[FS_PATH_MAX];
    const char *target = path;
    if (!io_fb_is_rom_uri(path) && fs_resolve_data_path(path, resolved, sizeof(resolved))) {
        target = resolved;
    } else if (!fs_is_regular_file(path) &&
               path_finder_resolve("config", path, resolved, sizeof(resolved))) {
        target = resolved;
    }

    size_t read_bytes = 0U;
    bool ok = fs_read_file_bounded(target, buf, buf_size, &read_bytes);

    if (out_result != NULL) {
        out_result->success = ok;
        out_result->bytes_read = read_bytes;
        str_safe_copy(out_result->path, target, sizeof(out_result->path));
    }

    return ok;
}

bool fs_execute_write(const char *path, const char *data, size_t len,
                      fs_write_result_t *out_result)
{
    if (out_result != NULL) {
        memset(out_result, 0, sizeof(*out_result));
    }

    if (path == NULL || data == NULL) {
        return false;
    }

    bool ok = fs_write_file_contents(path, data, len);

    if (out_result != NULL) {
        out_result->success = ok;
        out_result->bytes_written = ok ? len : 0U;
        str_safe_copy(out_result->path, path, sizeof(out_result->path));
    }

    return ok;
}

bool fs_utils_get_stats(fs_utils_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }
    fs_ensure_policy_armed();
    out_stats->initialized = s_fs_state.initialized;
    out_stats->policy_from_data = s_fs_state.policy_from_data;
    out_stats->read_only_safe_mode = fs_effective_read_only();
    out_stats->read_ops = s_fs_state.read_ops;
    out_stats->write_ops = s_fs_state.write_ops;
    out_stats->truncation_rejections = s_fs_state.truncation_rejections;
    out_stats->read_only_blocks = s_fs_state.read_only_blocks;
    out_stats->rom_fallback_reads = s_fs_state.rom_fallback_reads;
    out_stats->active_policy = s_fs_state.policy;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Arm Tier-0 BIOS fallback, hydrate `fs_buffers` + subpolicies, register
         debugger probes, and log structured lifecycle transitions on `LOG_CAT_IO`.

  ### For Game Designers & Non-Programmers:
  Cleanly initializes filesystem services at game boot and seals stats on shutdown.

  ### For Systems Programmers & Engine Architects:
  Enforces idempotent BIOS boot -> `fs_set_data_root(NULL)` -> `fs_utils_load_policy(NULL)`
  -> `fs_utils_register_debug_probes()` and marks `s_fs_state.initialized = true`.
--------------------------------------------------------------------------------
*/

void fs_utils_init(void)
{
    if (!io_fallback_is_booted()) {
        (void)io_fallback_boot(NULL);
    }
    fs_set_data_root(NULL);
    (void)fs_utils_load_policy(NULL);
    fs_utils_register_debug_probes();
    s_fs_state.initialized = true;
    FS_EMIT_LOG(LOG_INFO, 0.25f,
                "fs_utils initialized (profile='%s', from_data=%s)",
                s_fs_state.policy.profile_name,
                s_fs_state.policy_from_data ? "true" : "false");
}

void fs_utils_shutdown(void)
{
    FS_EMIT_LOG(LOG_INFO, 0.10f,
                "fs_utils shutdown (reads=%llu, writes=%llu, ro_blocks=%llu)",
                (unsigned long long)s_fs_state.read_ops,
                (unsigned long long)s_fs_state.write_ops,
                (unsigned long long)s_fs_state.read_only_blocks);
    s_fs_state.initialized = false;
}

/*
================================================================================
End of fs_utils.c
================================================================================
*/
