/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
================================================================================
@project       Tenebris and Lux
@file          engine/src/io/path_finder.c
@id            cog.path_finder
@layer         Layer 0: Core Utilities
@archetype     operational | stateful
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Layered Virtual Filesystem (VFS) Path Router
@role          Runtime Engine & Chassis: Tiered root/alias probe walker, POSIX
               lstat/readlink containment, ROM route hydration, and io.vfs.* probes.
@maturity      Rev 5 Baseline | Shared vfs_routes parser, POSIX symlink depth guard & 4 probes verified.
@chassis       zero-heap | .bss 288 B (s_vfs_state) + .rodata 5,300 B; reads routes from s_io_fb.vfs_routes.
@scale         O(roots * candidates <= 36) deterministic probe; POSIX lstat/readlink depth capped by route table.
@seams         in: vfs_routes.json, rom://io | uses: cog.io.fallback, cog.log, cog.debug | probes: io.vfs.* -> DEBUG_STEP_AWAIT | dag: core.io.path_finder
@invariants    1. Stratum Baseline: ALLOC_ZERO_HEAP, ISO C11 (-std=c11 -Wall -Wextra -Wpedantic, Invariant 18), 5-Block Protocol.
               2. Downward DAG isolation: reads s_io_fb.vfs_routes directly so cog.fs_utils depends cleanly downward.
               3. Reentrancy guards (in_load_routes, in_preflight, PATH_EMIT_LOG) prevent log <-> VFS cycles.
               4. Parent escapes (".."), symlink cap violations, and overflows fail cleanly without crash.

PURPOSE & ARCHITECTURAL ROLE:
  Executes data-driven 3-tier VFS resolution against mounted roots and aliases,
  verifies symlink depth & traversal bounds under _POSIX_C_SOURCE 200809L, and
  wires fault diagnostics into LOG_CAT_IO and 4 registered io.vfs.* probes.

  ### For Game Designers & Non-Programmers:
  Guarantees configs, story assets, fonts, and sprites load predictably from any
  launch directory while blocking escaping path tricks and dropping gracefully to
  sealed built-in ROM routes whenever disk folders are missing or broken.

  ### For Systems Programmers & Engine Architects:
  - Controlled Memory Chassis: Eliminates redundant route buffers by borrowing
    io_fb_vfs_routes_t directly, keeping .bss compact at 288 B + .rodata 5,300 B.
  - Bottleneck & Headroom: Candidate loop short-circuits cleanly on first hit;
    mounted base root caching avoids redundant repo discovery probes on hot paths.
  - Probe & Config Wiring: Shares io_fallback_parse_vfs_routes_json() cleanly and
    escalates containment or route faults across 4 `io.vfs.*` debug probes.
================================================================================
*/

/* Feature test macro exposing POSIX lstat/readlink/S_ISLNK cleanly under -std=c11. */
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
Module:        cog.path_finder
Mission:       Bind deps, log/debug setup, private structs, unified
               .bss/.rodata chassis, and support helpers.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers (+ POSIX lstat/readlink)
Tier 2: External Library Dependencies (None in Layer 0/1)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* --- Tier 2: External Library Dependencies (None in Layer 0/1) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "io/path_finder.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "io/io_fallback.h"
#include "log/log.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Static Configuration Constants
--------------------------------------------------------------------------------
Module constants, logging channel anchors, and safety caps in `.rodata`.
--------------------------------------------------------------------------------
*/

/** Canonical module identifier bound to `core/manifest.h`. */
#define MODULE_ID               PATH_FINDER_MODULE_ID
/** Logging category dedicated to VFS, FS, JSON, and BIOS telemetry. */
#define LOCAL_LOG_CAT           LOG_CAT_IO
/** Short source tag stamped into `LOG_CAT_IO` telemetry records. */
#define LOCAL_MODULE_TAG        "PathFinder"
/** Maximum hop safeguard bound checked alongside `routes->max_symlink_depth`. */
#define VFS_MAX_SYMLINK_CEILING 16

static_assert(
    PATH_FINDER_MAX_PATH >= 256 && PATH_FINDER_MAX_ROOT >= 128 && VFS_MAX_SYMLINK_CEILING >= 4,
    "VFS path, root, and symlink ceiling constants must remain operational"
);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Probe Forward Declarations
--------------------------------------------------------------------------------
Private `.bss` singleton type and Block 4 probe forward declarations.
--------------------------------------------------------------------------------
*/

/**
 * @struct path_finder_vfs_state
 * @brief Private runtime VFS mount, reentrancy guard, and telemetry singleton in `.bss`.
 *
 * ### For Game Designers & Non-Programmers:
 * Tracks mounted game/engine asset folders, security block counts, and BIOS hits.
 *
 * ### For Systems Programmers & Engine Architects:
 * Compact zero-heap singleton (~288 B); borrows route matrices directly from
 * `cog.io.fallback` (`s_io_fb.vfs_routes`) to eliminate redundant `.bss` copies.
 */
typedef struct path_finder_vfs_state {
    char     active_root[PATH_FINDER_MAX_ROOT]; /**< Mounted primary VFS data directory. */
    bool     root_initialized;                  /**< True once root synced with default/env. */
    bool     root_customized;                   /**< True if non-default mount root active. */
    bool     initialized;                       /**< True while `path_finder_init()` active. */
    bool     routes_from_data;                  /**< True when `vfs_routes.json` parsed from disk. */
    bool     probes_registered;                 /**< True after `s_vfs_probes` bound in debugger. */
    bool     in_preflight;                      /**< Suppresses noisy logs during `--audit`. */
    bool     in_log_emit;                       /**< Breaks `LOG_CAT_IO -> path_finder` recursion. */
    bool     in_load_routes;                    /**< Guards against recursive route hydration. */
    uint32_t total_lookups;                     /**< Lifetime resolution attempt count. */
    uint32_t disk_hits;                         /**< Resolutions backed by POSIX disk files. */
    uint32_t rom_hits;                          /**< Resolutions served via `.rodata` ROM vault. */
    uint32_t misses;                            /**< Total failed or rejected resolution queries. */
    uint32_t traversal_blocks;                  /**< Rejected `..` or symlink-depth breaches. */
    uint32_t overflow_rejections;               /**< Rejected path/root buffer overflows. */
} path_finder_vfs_state_t;

/* Forward declarations binding Sub-Assembly 2.3 manifest and Block 4 probes. */
diag_result_t path_finder_preflight(void);
static diag_result_t vfs_probe_route_bounds(void);
static diag_result_t vfs_probe_root_bounds(void);
static diag_result_t vfs_probe_rom_parity(void);
static diag_result_t vfs_probe_traversal_guard(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State, Manifest & Registered Debug Probes
--------------------------------------------------------------------------------
Compact `.bss` state (~288 bytes), manifest, and `.rodata` probes (`s_vfs_probes`).
--------------------------------------------------------------------------------
*/

/** Immutable engine invariant labels stored in `.rodata`. */
static const char *const s_vfs_invariants[5] = {
    "Memory safety: Strict ALLOC_ZERO_HEAP across all VFS routing and probing",
    "Data-driven route matrix hydrated from engine/config/io/vfs_routes.json or ROM",
    "Bounded 512-byte resolved path output with unconditional NUL termination",
    "Sandbox safety: blocks '..' parent traversal and bounded symlink chain depth",
    "Autonomous Safe-Mode ROM downgrade and io.vfs.* probe -> DEBUG_STEP_AWAIT loop"
};

/** Public module manifest exported for runtime `manifest_register()`. */
const module_metadata_t path_finder_manifest = {
    PATH_FINDER_MODULE_ID,
    "path_finder.h",
    LAYER_0_CORE_UTIL,
    { 0, 1, 1 },
    5,
    "2026-11-11",
    "Layered zero-heap VFS path router, symlink guard, and ROM Safe-Mode resolver",
    1,
    { "core.util.string" },
    ALLOC_ZERO_HEAP,
    CONCURRENCY_REENTRANT,
    1600,
    5,
    {
        s_vfs_invariants[0],
        s_vfs_invariants[1],
        s_vfs_invariants[2],
        s_vfs_invariants[3],
        s_vfs_invariants[4]
    },
    path_finder_preflight
};

/** Private singleton instance in `.bss` (`ALLOC_ZERO_HEAP`). */
static path_finder_vfs_state_t s_vfs_state;

/** Compiled `.rodata` debugger probe descriptors registered by `path_finder_register_debug_probes()`. */
static const debug_validation_probe_t s_vfs_probes[4] = {
    {
        "io.vfs.route_bounds",
        PATH_FINDER_MODULE_ID,
        "path_finder.c:Block3:path_finder_set_routes",
        "tests/unit/io/test_path_finder_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_STATE_CORRUPT,
        DEBUG_REACTION_FALLBACK_MODE,
        vfs_probe_route_bounds,
        true,
        true,
        0U
    },
    {
        "io.vfs.root_bounds",
        PATH_FINDER_MODULE_ID,
        "path_finder.c:Block3:path_finder_set_data_root",
        "tests/unit/io/test_path_finder_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_AUDIT_BREACH,
        DEBUG_REACTION_FALLBACK_MODE,
        vfs_probe_root_bounds,
        true,
        true,
        0U
    },
    {
        "io.vfs.rom_parity",
        PATH_FINDER_MODULE_ID,
        "path_finder.c:Block3:path_finder_load_routes",
        "tests/unit/io/test_path_finder_unit.c:Section3",
        DEBUG_TEST_TIER_AUDIT,
        DEBUG_TRIPWIRE_AUDIT_BREACH,
        DEBUG_REACTION_FALLBACK_MODE,
        vfs_probe_rom_parity,
        true,
        true,
        0U
    },
    {
        "io.vfs.traversal_guard",
        PATH_FINDER_MODULE_ID,
        "path_finder.c:Block3:path_finder_resolve",
        "tests/unit/io/test_path_finder_unit.c:Section2",
        DEBUG_TEST_TIER_UNIT,
        DEBUG_TRIPWIRE_FALLBACK_ENGAGED,
        DEBUG_REACTION_LOG_AND_CONTINUE,
        vfs_probe_traversal_guard,
        false,
        true,
        0U
    }
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Inline Frontier & Logging Guard
--------------------------------------------------------------------------------
Mission: Reentrancy-safe `LOG_CAT_IO` macro and O(1) output-buffer guard.
--------------------------------------------------------------------------------
*/

/** Reentrancy-guarded `LOG_CAT_IO` telemetry emitter active once logger is armed. */
#define PATH_EMIT_LOG(sev_macro, grade, ...)                                   \
    do {                                                                       \
        if (!s_vfs_state.in_log_emit && !s_vfs_state.in_preflight &&           \
            log_is_active()) {                                                 \
            s_vfs_state.in_log_emit = true;                                    \
            (void)sev_macro(LOCAL_LOG_CAT, LOCAL_MODULE_TAG, (grade),          \
                            __VA_ARGS__);                                      \
            s_vfs_state.in_log_emit = false;                                   \
        }                                                                      \
    } while (0)

/** @brief Returns true in O(1) time if a caller path buffer pointer and capacity are valid. */
CS_NODISCARD static inline bool vfs_out_buf_valid(const char *buf, size_t cap)
{
    return (buf != NULL && cap > 0U);
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.5: Monitored Support Infrastructure (Non-Inline Internal)
--------------------------------------------------------------------------------
Support Role:  Parent-traversal and POSIX symlink-chain security guards, prefix
               normalizers, ROM restoration, and unified tier/domain walkers.
Pattern Watch: Houses VFS candidate resolution walkers and POSIX `lstat` chain
               probes; if hot-path VFS lookups need negative-lookup caching or
               non-POSIX platform stat adapters, isolate them cleanly here first.
Linkage Rule:  Strictly static functions only; zero static arrays or .bss storage.
--------------------------------------------------------------------------------
*/

/** Checks whether `path` contains any discrete `..` parent traversal segment. */
static bool vfs_has_parent_traversal(const char *path)
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

/** Records a traversal or symlink rejection cleanly into stats, `LOG_CAT_IO`, and BIOS/debug. */
static void vfs_record_traversal_rejection(const char *op,
                                           const char *msg,
                                           bool update_stats)
{
    if (update_stats) {
        s_vfs_state.traversal_blocks++;
        s_vfs_state.misses++;
    }
    PATH_EMIT_LOG(LOG_WARN, -0.45f, "%s: %s", op ? op : "resolve", msg);
    if (!s_vfs_state.in_preflight) {
        io_fallback_record_fault(PATH_FINDER_MODULE_ID,
                                 op ? op : "resolve",
                                 msg,
                                 IO_ERR_PATH_OVERFLOW);
        if (debug_is_active()) {
            path_finder_register_debug_probes();
            (void)debug_trigger_probe("io.vfs.traversal_guard",
                                      PATH_FINDER_MODULE_ID,
                                      msg,
                                      __FILE__,
                                      __LINE__,
                                      op ? op : "path_finder_resolve");
        }
    }
}

/** Verifies symlink resolution depth along leaf `path` stays <= `max_depth`. */
static bool vfs_verify_symlink_chain(const char *path, int max_depth)
{
    if (path == NULL || path[0] == '\0' || max_depth < 0) {
        return false;
    }

    struct stat lst;
    if (lstat(path, &lst) != 0) {
        return false;
    }
    if (!S_ISLNK(lst.st_mode)) {
        return S_ISREG(lst.st_mode);
    }
    if (max_depth == 0) {
        return false;
    }

    char cur[PATH_FINDER_MAX_PATH];
    char link_target[PATH_FINDER_MAX_PATH];
    char joined[PATH_FINDER_MAX_PATH];
    str_safe_copy(cur, path, sizeof(cur));

    for (int hop = 0; hop < max_depth; hop++) {
        ssize_t rlen = readlink(cur, link_target, sizeof(link_target) - 1U);
        if (rlen <= 0 || (size_t)rlen >= sizeof(link_target)) {
            return false;
        }
        link_target[(size_t)rlen] = '\0';

        if (link_target[0] == '/') {
            str_safe_copy(joined, link_target, sizeof(joined));
        } else {
            char parent[PATH_FINDER_MAX_PATH];
            if (str_parent_dir(parent, sizeof(parent), cur) &&
                strcmp(parent, ".") != 0) {
                int n = snprintf(joined, sizeof(joined), "%s/%s", parent, link_target);
                if (n <= 0 || (size_t)n >= sizeof(joined)) {
                    return false;
                }
            } else {
                str_safe_copy(joined, link_target, sizeof(joined));
            }
        }
        str_safe_copy(cur, joined, sizeof(cur));

        if (lstat(cur, &lst) != 0) {
            return false;
        }
        if (!S_ISLNK(lst.st_mode)) {
            return S_ISREG(lst.st_mode);
        }
    }
    return false;
}

/** Checks regular-file existence and enforces `routes->max_symlink_depth`. */
static bool vfs_probe_regular_candidate(const char *candidate,
                                        const io_fb_vfs_routes_t *routes,
                                        bool *out_symlink_blocked)
{
    if (out_symlink_blocked != NULL) {
        *out_symlink_blocked = false;
    }
    if (candidate == NULL || candidate[0] == '\0' || !io_fallback_fs_is_regular(candidate)) {
        return false;
    }

    int max_depth = (routes != NULL) ? routes->max_symlink_depth : IO_FB_DEFAULT_SYMLINK_DEPTH;
    if (!vfs_verify_symlink_chain(candidate, max_depth)) {
        if (out_symlink_blocked != NULL) {
            *out_symlink_blocked = true;
        }
        return false;
    }
    return true;
}

/** Resolves default active data root from `routes->env_override_var` or `routes->default_data_root`. */
static void vfs_compute_default_root(const io_fb_vfs_routes_t *routes,
                                     char *out_root,
                                     size_t out_size)
{
    const char *env_var = (routes != NULL && routes->env_override_var[0] != '\0')
        ? routes->env_override_var
        : "TL_DATA_ROOT";
    const char *env_val = getenv(env_var);
    const char *fallback_root = (routes != NULL && routes->default_data_root[0] != '\0')
        ? routes->default_data_root
        : IO_FB_DEFAULT_ROOT;

    if (env_val != NULL && env_val[0] != '\0' &&
        strlen(env_val) < out_size &&
        (!routes || !routes->block_path_traversal || !vfs_has_parent_traversal(env_val))) {
        str_safe_copy(out_root, env_val, out_size);
    } else {
        str_safe_copy(out_root, fallback_root, out_size);
    }
}

/** Lazily arms `s_vfs_state.active_root` without mutating explicit custom mounts. */
static void vfs_ensure_root_armed(void)
{
    if (!s_vfs_state.root_initialized) {
        const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
        vfs_compute_default_root(routes, s_vfs_state.active_root, sizeof(s_vfs_state.active_root));
        s_vfs_state.root_initialized = true;
        s_vfs_state.root_customized = false;
        (void)io_fallback_path_set_root(s_vfs_state.active_root);
    }
}

/** Restores `.rodata` ROM default routes cleanly via single-source `io_fallback_parse_vfs_routes_json()`. */
static bool vfs_restore_rom_routes(void)
{
    size_t rom_sz = 0U;
    const char *rom_json = io_fallback_find_rom_asset(PATH_FINDER_ROM_ROUTES_URI, &rom_sz);
    io_fb_vfs_routes_t restored;
    bool parsed = false;

    if (rom_json != NULL && rom_sz > 0U &&
        io_fallback_parse_vfs_routes_json(rom_json, rom_sz, &restored) &&
        path_finder_routes_contract_ok(&restored)) {
        restored.loaded_from_data = false;
        parsed = io_fallback_set_vfs_routes(&restored);
    }
    s_vfs_state.routes_from_data = false;
    return parsed;
}

/** Strips known config/data prefixes cleanly so caller-prefixed paths re-enter precedence lookup. */
static const char *vfs_strip_known_prefixes(const char *path,
                                            const io_fb_vfs_routes_t *routes)
{
    if (path == NULL) {
        return "";
    }
    const char *p = path;
    if (io_fb_is_rom_uri(p)) {
        p += 6;
    }

    static const char *const s_std_prefixes[] = {
        "../engine/config/",
        "engine/config/",
        "../game/config/",
        "game/config/",
        "../config/",
        "config/",
        "../game/data/",
        "game/data/",
        "../data/",
        "data/"
    };
    for (size_t i = 0U; i < sizeof(s_std_prefixes) / sizeof(s_std_prefixes[0]); i++) {
        size_t plen = strlen(s_std_prefixes[i]);
        if (strncmp(p, s_std_prefixes[i], plen) == 0) {
            return p + plen;
        }
    }

    if (routes != NULL) {
        for (size_t i = 0U; i < routes->config_precedence_count; i++) {
            const char *tier = routes->config_precedence[i];
            size_t tlen = strlen(tier);
            if (tlen > 0U && strncmp(p, tier, tlen) == 0) {
                return p + tlen;
            }
        }
    }
    return p;
}

/** Expands `diagnostic_aliases` from hydrated `vfs_routes.json` (`log/`, `debug/`, `io/`). */
static void vfs_build_diagnostic_alias(const char *domain,
                                       const char *clean_path,
                                       const io_fb_vfs_routes_t *routes,
                                       char *out_alias,
                                       size_t out_size)
{
    if (out_alias == NULL || out_size == 0U) {
        return;
    }
    out_alias[0] = '\0';
    if (clean_path == NULL || clean_path[0] == '\0' || routes == NULL) {
        return;
    }

    if (strncmp(clean_path, "log/", 4) == 0 && routes->diag_log_prefix[0] != '\0') {
        (void)snprintf(out_alias, out_size, "%s%s",
                       routes->diag_log_prefix, clean_path + 4);
    } else if (strncmp(clean_path, "debug/", 6) == 0 && routes->diag_debug_prefix[0] != '\0') {
        (void)snprintf(out_alias, out_size, "%s%s",
                       routes->diag_debug_prefix, clean_path + 6);
    } else if (domain != NULL && strcmp(domain, "io") == 0 &&
               routes->diag_io_prefix[0] != '\0') {
        size_t io_len = strlen(routes->diag_io_prefix);
        if (strncmp(clean_path, routes->diag_io_prefix, io_len) != 0) {
            (void)snprintf(out_alias, out_size, "%s%s",
                           routes->diag_io_prefix, clean_path);
        }
    }
}

/** Commits a resolved disk or ROM path into `out_path` with overflow protection and telemetry. */
static bool vfs_commit_resolved(const char *matched_path,
                                bool is_rom,
                                char *out_path,
                                size_t out_size,
                                bool record_telemetry,
                                bool *out_overflowed)
{
    if (out_overflowed != NULL) {
        *out_overflowed = false;
    }
    if (matched_path == NULL || matched_path[0] == '\0' ||
        !vfs_out_buf_valid(out_path, out_size)) {
        return false;
    }

    size_t len = strlen(matched_path);
    if (len >= out_size || len >= (size_t)PATH_FINDER_MAX_PATH) {
        out_path[0] = '\0';
        if (out_overflowed != NULL) {
            *out_overflowed = true;
        }
        if (record_telemetry) {
            s_vfs_state.overflow_rejections++;
            s_vfs_state.misses++;
        }
        PATH_EMIT_LOG(LOG_WARN, -0.45f,
                      "Resolved path length (%zu B) exceeds buffer capacity (%zu B)",
                      len, out_size);
        if (!s_vfs_state.in_preflight) {
            io_fallback_record_fault(PATH_FINDER_MODULE_ID,
                                     "resolve",
                                     "Resolved path exceeds caller output buffer capacity",
                                     IO_ERR_PATH_OVERFLOW);
        }
        return false;
    }

    str_safe_copy(out_path, matched_path, out_size);
    if (record_telemetry) {
        if (is_rom) {
            s_vfs_state.rom_hits++;
        } else {
            s_vfs_state.disk_hits++;
        }
    }
    PATH_EMIT_LOG(LOG_DEBUG, 0.05f, "Resolved [%s] -> '%s'",
                  is_rom ? "ROM" : "DISK", out_path);
    return true;
}

/** Tests one formatted candidate probe path for regular file & symlink compliance. */
static bool vfs_try_candidate(const char *candidate,
                              const io_fb_vfs_routes_t *routes,
                              char *out_path,
                              size_t out_size,
                              bool record_telemetry,
                              bool *out_abort_error)
{
    bool sym_blocked = false;
    if (!vfs_probe_regular_candidate(candidate, routes, &sym_blocked)) {
        if (sym_blocked) {
            vfs_record_traversal_rejection("resolve",
                                           "Blocked symlink exceeding configured max_symlink_depth",
                                           record_telemetry);
            if (out_abort_error != NULL) {
                *out_abort_error = true;
            }
        }
        return false;
    }

    bool overflowed = false;
    bool ok = vfs_commit_resolved(candidate, false, out_path, out_size,
                                  record_telemetry, &overflowed);
    if (overflowed && out_abort_error != NULL) {
        *out_abort_error = true;
    }
    return ok;
}

/** Probes `<base>/<domain>/<subpath>`, `<base>/<domain_search_paths[d]><subpath>`, and aliases. */
static bool vfs_probe_base_with_domains(const char *base_dir,
                                        const char *domain,
                                        const char *clean_path,
                                        const char *aliased_path,
                                        const io_fb_vfs_routes_t *routes,
                                        char *probe_buf,
                                        size_t probe_cap,
                                        char *out_path,
                                        size_t out_size,
                                        bool record_telemetry,
                                        bool *out_abort_error)
{
    if (base_dir == NULL || base_dir[0] == '\0' || routes == NULL) {
        return false;
    }

    /* Prioritize explicit caller `<base>/<domain>/<file>` for custom mounts/hotpatches. */
    if (domain != NULL && domain[0] != '\0') {
        int n = snprintf(probe_buf, probe_cap, "%s/%s/%s", base_dir, domain, clean_path);
        if (n > 0 && (size_t)n < probe_cap &&
            vfs_try_candidate(probe_buf, routes, out_path, out_size,
                              record_telemetry, out_abort_error)) {
            return true;
        }
        if (*out_abort_error) {
            return false;
        }
        if (aliased_path != NULL && aliased_path[0] != '\0') {
            n = snprintf(probe_buf, probe_cap, "%s/%s/%s", base_dir, domain, aliased_path);
            if (n > 0 && (size_t)n < probe_cap &&
                vfs_try_candidate(probe_buf, routes, out_path, out_size,
                                  record_telemetry, out_abort_error)) {
                return true;
            }
            if (*out_abort_error) {
                return false;
            }
        }
    }

    size_t search_cnt = (routes->domain_search_count <= IO_FB_MAX_VFS_DOMAIN_DIRS)
        ? routes->domain_search_count
        : IO_FB_MAX_VFS_DOMAIN_DIRS;

    for (size_t d = 0U; d < search_cnt; d++) {
        int n = snprintf(probe_buf, probe_cap, "%s/%s%s",
                         base_dir, routes->domain_search_paths[d], clean_path);
        if (n > 0 && (size_t)n < probe_cap &&
            vfs_try_candidate(probe_buf, routes, out_path, out_size,
                              record_telemetry, out_abort_error)) {
            return true;
        }
        if (*out_abort_error) {
            return false;
        }
        if (aliased_path != NULL && aliased_path[0] != '\0') {
            n = snprintf(probe_buf, probe_cap, "%s/%s%s",
                             base_dir, routes->domain_search_paths[d], aliased_path);
            if (n > 0 && (size_t)n < probe_cap &&
                vfs_try_candidate(probe_buf, routes, out_path, out_size,
                                  record_telemetry, out_abort_error)) {
                return true;
            }
            if (*out_abort_error) {
                return false;
            }
        }
    }
    return false;
}

/** Probes layered `config_precedence[]` tiers (`game/config/`, `engine/config/`, linkers). */
static bool vfs_probe_config_tiers(const char *clean_path,
                                   const char *aliased_path,
                                   const io_fb_vfs_routes_t *routes,
                                   char *probe_buf,
                                   size_t probe_cap,
                                   char *out_path,
                                   size_t out_size,
                                   bool record_telemetry,
                                   bool *out_abort_error)
{
    if (routes == NULL) {
        return false;
    }

    const char *cfg_sub = (strncmp(clean_path, "config/", 7) == 0)
        ? (clean_path + 7)
        : clean_path;

    size_t tier_cnt = (routes->config_precedence_count <= IO_FB_MAX_PRECEDENCE_TIERS)
        ? routes->config_precedence_count
        : IO_FB_MAX_PRECEDENCE_TIERS;

    static const char *const s_rel_bases[2] = { "", "../" };

    for (size_t b = 0U; b < 2U; b++) {
        for (size_t t = 0U; t < tier_cnt; t++) {
            const char *tier = routes->config_precedence[t];
            if (tier[0] == '\0') {
                continue;
            }

            int n = snprintf(probe_buf, probe_cap, "%s%s%s", s_rel_bases[b], tier, cfg_sub);
            if (n > 0 && (size_t)n < probe_cap &&
                vfs_try_candidate(probe_buf, routes, out_path, out_size,
                                  record_telemetry, out_abort_error)) {
                return true;
            }
            if (*out_abort_error) {
                return false;
            }

            if (aliased_path != NULL && aliased_path[0] != '\0') {
                n = snprintf(probe_buf, probe_cap, "%s%s%s",
                             s_rel_bases[b], tier, aliased_path);
                if (n > 0 && (size_t)n < probe_cap &&
                    vfs_try_candidate(probe_buf, routes, out_path, out_size,
                                      record_telemetry, out_abort_error)) {
                    return true;
                }
                if (*out_abort_error) {
                    return false;
                }
            }
        }
    }
    return false;
}

/** Finds a config JSON file directly on disk (used exclusively by `vfs_hydrate_routes_internal`). */
static bool vfs_locate_disk_routes_file(const char *rel_or_abs_path,
                                        const io_fb_vfs_routes_t *routes,
                                        char *out_disk_path,
                                        size_t out_size)
{
    if (rel_or_abs_path == NULL || rel_or_abs_path[0] == '\0' ||
        out_disk_path == NULL || out_size == 0U || routes == NULL) {
        return false;
    }

    char probe[PATH_FINDER_MAX_PATH * 2U];
    bool abort_err = false;

    if (!io_fb_is_rom_uri(rel_or_abs_path) &&
        vfs_try_candidate(rel_or_abs_path, routes, out_disk_path, out_size, false, &abort_err)) {
        return true;
    }
    if (abort_err) {
        return false;
    }

    const char *clean = vfs_strip_known_prefixes(rel_or_abs_path, routes);
    if (vfs_probe_config_tiers(clean, NULL, routes, probe, sizeof(probe),
                               out_disk_path, out_size, false, &abort_err)) {
        return true;
    }
    return false;
}

/** Shared disk + `.rodata` ROM hydration path eliminating parser duplication. */
static bool vfs_hydrate_routes_internal(const char *rel_path)
{
    if (s_vfs_state.in_load_routes) {
        return s_vfs_state.routes_from_data;
    }
    s_vfs_state.in_load_routes = true;

    const char *target = (rel_path != NULL && rel_path[0] != '\0')
        ? rel_path
        : PATH_FINDER_DEFAULT_ROUTES_PATH;
    const io_fb_vfs_routes_t *cur_routes = io_fallback_get_vfs_routes();

    if (vfs_has_parent_traversal(target) && cur_routes->block_path_traversal) {
        vfs_record_traversal_rejection("load_routes",
                                       "Blocked traversal '..' in vfs_routes target path",
                                       true);
        s_vfs_state.in_load_routes = false;
        return false;
    }

    char resolved_disk[PATH_FINDER_MAX_PATH];
    if (!vfs_locate_disk_routes_file(target, cur_routes, resolved_disk, sizeof(resolved_disk))) {
        (void)vfs_restore_rom_routes();
        PATH_EMIT_LOG(LOG_WARN, -0.15f,
                      "VFS route file '%s' absent on disk; using Safe-Mode ROM vault",
                      target);
        s_vfs_state.in_load_routes = false;
        return false;
    }

    char json_buf[IO_FB_MEDIUM_BYTES];
    size_t read_len = 0U;
    io_fb_vfs_routes_t parsed;
    bool ok = io_fallback_fs_read_bounded(resolved_disk, json_buf, sizeof(json_buf), &read_len) &&
              io_fallback_parse_vfs_routes_json(json_buf, read_len, &parsed) &&
              path_finder_validate_routes_contract(&parsed);

    if (!ok) {
        bool await_flag = cur_routes->probe_await_on_corrupt;
        (void)vfs_restore_rom_routes();
        (void)io_fallback_downgrade_safe_mode("Corrupt vfs_routes.json; reverted to ROM vault",
                                              false);
        io_fallback_record_fault(PATH_FINDER_MODULE_ID,
                                 "load_routes",
                                 "Corrupt disk vfs_routes JSON rejected; downgraded to ROM Safe Mode",
                                 IO_ERR_JSON_SYNTAX);
        PATH_EMIT_LOG(LOG_ERROR, -0.75f,
                      "Corrupt disk VFS routes '%s'; restored .rodata ROM Safe Mode",
                      resolved_disk);
        if (debug_is_active() && await_flag && !s_vfs_state.in_preflight) {
            path_finder_register_debug_probes();
            (void)debug_trigger_probe("io.vfs.rom_parity",
                                      PATH_FINDER_MODULE_ID,
                                      "Corrupt on-disk vfs_routes JSON tripped Safe-Mode ROM parity",
                                      __FILE__,
                                      __LINE__,
                                      "path_finder_load_routes");
        }
        s_vfs_state.in_load_routes = false;
        return false;
    }

    parsed.loaded_from_data = true;
    if (!io_fallback_set_vfs_routes(&parsed)) {
        (void)vfs_restore_rom_routes();
        s_vfs_state.in_load_routes = false;
        return false;
    }

    s_vfs_state.routes_from_data = true;
    if (!s_vfs_state.root_customized) {
        vfs_compute_default_root(&parsed, s_vfs_state.active_root, sizeof(s_vfs_state.active_root));
        s_vfs_state.root_initialized = true;
        (void)io_fallback_path_set_root(s_vfs_state.active_root);
    }

    PATH_EMIT_LOG(LOG_INFO, 0.25f,
                  "Hydrated VFS routes from '%s' (%zu domains, %zu asset roots, %zu config tiers)",
                  resolved_disk,
                  parsed.domain_search_count,
                  parsed.asset_root_count,
                  parsed.config_precedence_count);

    s_vfs_state.in_load_routes = false;
    return true;
}

/*
================================================================================
BLOCK 3: BODY BLOCK
( Operational Business Logic, Span Scopes & Runtime Hydration )
================================================================================
Engine:        Cornerstone Game Engine
Stratum:       Layer 0 (Core Utilities)
Module:        cog.path_finder
Mission:       Multi-Tier VFS Resolution, Alias Expansion & ROM Downgrade Engine
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Root Mount Management & VFS Route Hydration
Mission: Enforce bounded mount roots, `allow_mod_mounts`, `env_override_var`,
         and data-driven `vfs_routes.json` hydration with Safe-Mode downgrade.

  ### For Game Designers & Non-Programmers:
  Keeps custom mod/campaign root folders inside safe memory limits and safely
  replaces broken route files with built-in ROM defaults.

  ### For Systems Programmers & Engine Architects:
  Single canonical parser path via `io_fallback_parse_vfs_routes_json()` + contract
  checking; triggers `io.vfs.root_bounds` / `io.vfs.route_bounds` / `io.vfs.rom_parity`.
  @triggers `io.vfs.root_bounds`, `io.vfs.route_bounds`, `io.vfs.rom_parity`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Commits or resets the active primary root directory searched during VFS resolution.
 *
 * ### For Game Designers & Non-Programmers:
 * Switches which base folder takes highest priority when loading overrides and
 * campaign data, or restores the default root folder cleanly when `root_path` is empty.
 *
 * ### For Systems Programmers & Engine Architects:
 * Computes effective default root via `vfs_compute_default_root()`, rejects lengths
 * failing `path_finder_root_len_is_valid()` via `IO_ERR_PATH_OVERFLOW` and
 * `io.vfs.root_bounds`, blocks `..` escapes, honors `allow_mod_mounts`, and mirrors
 * `s_vfs_state.active_root` into `io_fallback_path_set_root()`.
 * @triggers `io.vfs.root_bounds`, `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in] root_path Candidate directory path, or NULL/empty to restore default root.
 * @return true if valid and committed; false on overflow, traversal, or blocked mod mount.
 */
bool path_finder_set_data_root(const char *root_path)
{
    /* Step 1: Ensure route table and root state are armed before applying override. */
    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
    vfs_ensure_root_armed();

    /* Step 2: NULL or empty path restores default mounted root and clears custom flag. */
    if (root_path == NULL || root_path[0] == '\0') {
        vfs_compute_default_root(routes, s_vfs_state.active_root, sizeof(s_vfs_state.active_root));
        s_vfs_state.root_initialized = true;
        s_vfs_state.root_customized = false;
        (void)io_fallback_path_set_root(s_vfs_state.active_root);
        PATH_EMIT_LOG(LOG_DEBUG, 0.10f, "VFS data root reset to default: '%s'",
                      s_vfs_state.active_root);
        return true;
    }

    /* Step 3: Enforce fixed chassis length ceiling (PATH_FINDER_MAX_ROOT = 256 B). */
    size_t rlen = strlen(root_path);
    if (!path_finder_root_len_is_valid(rlen)) {
        s_vfs_state.overflow_rejections++;
        io_fallback_record_fault(PATH_FINDER_MODULE_ID,
                                 "set_data_root",
                                 "Configured data root exceeds PATH_FINDER_MAX_ROOT (256 B)",
                                 IO_ERR_PATH_OVERFLOW);
        PATH_EMIT_LOG(LOG_ERROR, -0.75f,
                      "Rejected data root mount (%zu B >= %d B)",
                      rlen, PATH_FINDER_MAX_ROOT);
        if (debug_is_active() && !s_vfs_state.in_preflight) {
            path_finder_register_debug_probes();
            (void)debug_trigger_probe("io.vfs.root_bounds",
                                      PATH_FINDER_MODULE_ID,
                                      "Oversized VFS root mount rejected -> Safe Mode",
                                      __FILE__,
                                      __LINE__,
                                      "path_finder_set_data_root");
        }
        return false;
    }

    /* Step 4: Block parent-traversal segments if traversal containment is enabled. */
    if (routes->block_path_traversal && vfs_has_parent_traversal(root_path)) {
        vfs_record_traversal_rejection("set_data_root",
                                       "Blocked parent traversal '..' in custom VFS root mount",
                                       true);
        return false;
    }

    /* Step 5: Enforce `allow_mod_mounts` policy when caller specifies non-default folder. */
    char default_buf[PATH_FINDER_MAX_ROOT];
    vfs_compute_default_root(routes, default_buf, sizeof(default_buf));
    bool is_custom = (strcmp(root_path, default_buf) != 0 &&
                      strcmp(root_path, routes->default_data_root) != 0);

    if (is_custom && !routes->allow_mod_mounts) {
        io_fallback_record_fault(PATH_FINDER_MODULE_ID,
                                 "set_data_root",
                                 "Custom non-default VFS mount blocked by allow_mod_mounts=false",
                                 IO_ERR_WRITE_BLOCKED_RO);
        PATH_EMIT_LOG(LOG_WARN, -0.40f,
                      "Blocked external mod mount '%s' (allow_mod_mounts=false)",
                      root_path);
        return false;
    }

    /* Step 6: Commit validated root into local chassis and BIOS shadow buffer. */
    str_safe_copy(s_vfs_state.active_root, root_path, sizeof(s_vfs_state.active_root));
    s_vfs_state.root_initialized = true;
    s_vfs_state.root_customized = is_custom;
    (void)io_fallback_path_set_root(s_vfs_state.active_root);

    PATH_EMIT_LOG(LOG_DEBUG, 0.10f, "VFS data root configured: '%s'%s",
                  s_vfs_state.active_root, is_custom ? " [CUSTOM MOUNT]" : "");
    return true;
}

/**
 * @brief Retrieves the active mounted VFS root path from `.bss` chassis storage.
 *
 * ### For Game Designers & Non-Programmers:
 * Exposes the exact base directory currently searched first for game data and configs.
 *
 * ### For Systems Programmers & Engine Architects:
 * Invokes `vfs_ensure_root_armed()` lazily so pre-boot queries still receive an
 * initialized null-terminated string inside `s_vfs_state.active_root`.
 * @triggers None.
 *
 * @return Non-NULL pointer to static `.bss` active data root path.
 */
const char *path_finder_get_data_root(void)
{
    /* Step 1: Lazily arm default root on first read before returning chassis buffer. */
    vfs_ensure_root_armed();
    return s_vfs_state.active_root;
}

/**
 * @brief Hydrates VFS route tables from disk JSON or downgrades cleanly to `.rodata` ROM.
 *
 * ### For Game Designers & Non-Programmers:
 * Refreshes search directories, asset paths, and traversal toggles from disk settings
 * without restarting, falling back to factory ROM defaults if the JSON is broken.
 *
 * ### For Systems Programmers & Engine Architects:
 * Arms root state and delegates disk probing, canonical parsing via
 * `io_fallback_parse_vfs_routes_json()`, and ROM parity fallback to
 * `vfs_hydrate_routes_internal()` in Sub-Assembly 2.5.
 * @triggers `io.vfs.rom_parity`, `io.vfs.route_bounds`, `LOG_CAT_IO`.
 *
 * @param[in] rel_path Optional relative/direct route JSON path (NULL uses `"io/vfs_routes.json"`).
 * @return true when loaded validly from disk JSON; false when safely downgraded to ROM.
 */
bool path_finder_load_routes(const char *rel_path)
{
    /* Step 1: Arm root state and run reentrancy-guarded disk-to-ROM hydration helper. */
    vfs_ensure_root_armed();
    return vfs_hydrate_routes_internal(rel_path);
}

/**
 * @brief Validates and commits an explicit VFS route configuration into Tier-0 BIOS state.
 *
 * ### For Game Designers & Non-Programmers:
 * Applies programmatic routing table changes while automatically dropping back to Safe-Mode
 * ROM defaults if the supplied route table is incomplete or corrupted.
 *
 * ### For Systems Programmers & Engine Architects:
 * Runs `path_finder_validate_routes_contract()`. On breach, restores ROM routes via
 * `vfs_restore_rom_routes()`, enters BIOS Safe Mode, logs fault, and fires
 * `io.vfs.route_bounds` when debugging is active outside preflight.
 * @triggers `io.vfs.route_bounds`, `LOG_CAT_IO`.
 *
 * @param[in] routes Candidate VFS route table to commit.
 * @return true if valid and committed; false on contract breach (downgraded to ROM).
 */
bool path_finder_set_routes(const io_fb_vfs_routes_t *routes)
{
    /* Step 1: Guarantee active root baseline before validating incoming contract. */
    vfs_ensure_root_armed();

    /* Step 2: Reject invalid route contracts by downgrading safely to .rodata ROM vault. */
    if (!path_finder_validate_routes_contract(routes)) {
        const io_fb_vfs_routes_t *cur = io_fallback_get_vfs_routes();
        bool await_flag = (cur != NULL) ? cur->probe_await_on_corrupt : true;

        (void)vfs_restore_rom_routes();
        (void)io_fallback_downgrade_safe_mode("Invalid VFS routes contract; reverted to ROM vault",
                                              false);
        io_fallback_record_fault(PATH_FINDER_MODULE_ID,
                                 "set_routes",
                                 "Rejected corrupt io_fb_vfs_routes_t; downgraded to ROM Safe Mode",
                                 IO_ERR_PATH_OVERFLOW);
        PATH_EMIT_LOG(LOG_ERROR, -0.80f,
                      "Invalid io_fb_vfs_routes_t contract; downgraded to Safe-Mode ROM");

        if (debug_is_active() && await_flag && !s_vfs_state.in_preflight) {
            path_finder_register_debug_probes();
            (void)debug_trigger_probe("io.vfs.route_bounds",
                                      PATH_FINDER_MODULE_ID,
                                      "Invalid VFS route table breached bounds -> Safe Mode",
                                      __FILE__,
                                      __LINE__,
                                      "path_finder_set_routes");
        }
        return false;
    }

    /* Step 3: Commit validated route descriptor into shared BIOS storage & sync root. */
    if (!io_fallback_set_vfs_routes(routes)) {
        return false;
    }

    s_vfs_state.routes_from_data = routes->loaded_from_data;
    if (!s_vfs_state.root_customized) {
        vfs_compute_default_root(routes, s_vfs_state.active_root, sizeof(s_vfs_state.active_root));
        (void)io_fallback_path_set_root(s_vfs_state.active_root);
    }

    PATH_EMIT_LOG(LOG_INFO, 0.20f,
                  "Applied VFS routes (%s, %zu domains, %zu candidates, traversal=%s)",
                  routes->loaded_from_data ? "disk" : "rom",
                  routes->domain_search_count,
                  routes->candidate_root_count,
                  routes->block_path_traversal ? "blocked" : "open");
    return true;
}

/**
 * @brief Snapshots the currently active `io_fb_vfs_routes_t` configuration into caller memory.
 *
 * ### For Game Designers & Non-Programmers:
 * Lets UI diagnostics and tests read active search folders, symlink limits, and ROM status.
 *
 * ### For Systems Programmers & Engine Architects:
 * Performs a bounded struct assignment from `io_fallback_get_vfs_routes()` and unifies
 * `s_vfs_state.routes_from_data` with `cur->loaded_from_data`.
 * @triggers None.
 *
 * @param[out] out_routes Caller destination receiving copied route configuration.
 * @return true on success; false when `out_routes` is NULL.
 */
bool path_finder_get_routes(io_fb_vfs_routes_t *out_routes)
{
    /* Step 1: Reject NULL output descriptor silently (expected negative boundary check). */
    if (out_routes == NULL) {
        return false;
    }

    /* Step 2: Copy canonical Tier-0 route descriptor and merge provenance indicator. */
    const io_fb_vfs_routes_t *cur = io_fallback_get_vfs_routes();
    *out_routes = *cur;
    out_routes->loaded_from_data = s_vfs_state.routes_from_data || cur->loaded_from_data;
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Multi-Tier VFS Path & Asset Resolution Cogs
Mission: Execute traversal/symlink checks, mounted root priority, layered config
         precedence, candidate domain matrices, asset roots, and ROM fallbacks.

  ### For Game Designers & Non-Programmers:
  Searches game overrides, engine baselines, asset folders, and built-in ROM
  backups automatically in clean deterministic order.

  ### For Systems Programmers & Engine Architects:
  Consolidates all probing through parameterized helpers (`vfs_probe_base_with_domains`,
  `vfs_probe_config_tiers`, `vfs_try_candidate`) using bounded stack storage only.
  @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
--------------------------------------------------------------------------------
*/

/**
 * @brief Resolves a logical domain and relative path to a readable disk file or `rom://` URI.
 *
 * ### For Game Designers & Non-Programmers:
 * Automatically tracks down config presets, manifests, and game data files across
 * custom mod folders, `game/config/`, `engine/config/`, and built-in ROM backups.
 *
 * ### For Systems Programmers & Engine Architects:
 * Walks Tiers 0..5 deterministically (`rom://` -> direct probe -> `active_root` ->
 * `env_override_var` -> `config_precedence` + `diagnostic_aliases` -> candidate matrix
 * -> `.rodata` ROM fallback), strictly enforcing traversal containment, POSIX symlink
 * depth bounds, and destination capacity checks.
 * @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in]  domain   Optional logical domain hint (`"config"`, `"mechanics"`, etc.) or NULL.
 * @param[in]  rel_path Relative file path or explicit `"rom://..."` virtual URI.
 * @param[out] out_path Caller output buffer (`out_path[0] = '\0'` on failure).
 * @param[in]  out_size Capacity of `out_path` in bytes.
 * @return true if resolved onto disk or ROM within `out_size`; false on miss/error.
 */
bool path_finder_resolve(const char *domain,
                         const char *rel_path,
                         char *out_path,
                         size_t out_size)
{
    /* Step 1: Clear output defensively and validate non-empty inputs cleanly. */
    if (!vfs_out_buf_valid(out_path, out_size)) {
        return false;
    }
    out_path[0] = '\0';
    if (rel_path == NULL || rel_path[0] == '\0') {
        return false;
    }

    vfs_ensure_root_armed();
    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();

    bool track = !s_vfs_state.in_preflight;
    if (track) {
        s_vfs_state.total_lookups++;
    }

    /* Step 2: Enforce '..' parent traversal sandbox guard (allow rom:// URIs cleanly). */
    if (!io_fb_is_rom_uri(rel_path) && routes->block_path_traversal &&
        (vfs_has_parent_traversal(rel_path) || vfs_has_parent_traversal(domain))) {
        vfs_record_traversal_rejection("resolve",
                                       "Blocked parent directory traversal '..' in VFS path",
                                       track);
        return false;
    }

    /* Step 3 (Tier 0): Explicit virtual "rom://..." URI lookup. */
    if (io_fb_is_rom_uri(rel_path)) {
        bool ovf = false;
        if (io_fallback_find_rom_asset(rel_path, NULL) != NULL &&
            vfs_commit_resolved(rel_path, true, out_path, out_size, track, &ovf)) {
            return true;
        }
        if (track && !ovf) {
            s_vfs_state.misses++;
        }
        return false;
    }

    char probe[PATH_FINDER_MAX_PATH * 2U];
    char aliased[PATH_FINDER_MAX_PATH];
    bool abort_err = false;

    /* Step 4 (Tier 1): Direct path accessibility probe (enforcing max_symlink_depth & out_size). */
    if (vfs_try_candidate(rel_path, routes, out_path, out_size, track, &abort_err)) {
        return true;
    }
    if (abort_err) {
        return false;
    }

    const char *clean_path = vfs_strip_known_prefixes(rel_path, routes);
    vfs_build_diagnostic_alias(domain, clean_path, routes, aliased, sizeof(aliased));

    /* Step 5 (Tier 2a): Active mounted root priority (mod overrides & custom roots first). */
    if (vfs_probe_base_with_domains(s_vfs_state.active_root, domain, clean_path,
                                    aliased, routes, probe, sizeof(probe),
                                    out_path, out_size, track, &abort_err)) {
        return true;
    }
    if (abort_err) {
        return false;
    }

    /* Step 6 (Tier 2b): Environment override root (`env_override_var` when mod mounts allowed). */
    if (routes->env_override_var[0] != '\0' && routes->allow_mod_mounts) {
        const char *env_root = getenv(routes->env_override_var);
        if (env_root != NULL && env_root[0] != '\0' &&
            strcmp(env_root, s_vfs_state.active_root) != 0 &&
            (!routes->block_path_traversal || !vfs_has_parent_traversal(env_root))) {
            if (vfs_probe_base_with_domains(env_root, domain, clean_path,
                                            aliased, routes, probe, sizeof(probe),
                                            out_path, out_size, track, &abort_err)) {
                return true;
            }
            if (abort_err) {
                return false;
            }
        }
    }

    /* Step 7 (Tier 3): Layered `config_precedence` tiers & `diagnostic_aliases`. */
    bool is_cfg_query = (domain == NULL) ||
                        (strcmp(domain, "config") == 0) ||
                        (strcmp(domain, "io") == 0) ||
                        (strncmp(rel_path, "config/", 7) == 0) ||
                        (strncmp(rel_path, "engine/config/", 14) == 0) ||
                        (aliased[0] != '\0');
    if (is_cfg_query &&
        vfs_probe_config_tiers(clean_path, aliased, routes, probe, sizeof(probe),
                               out_path, out_size, track, &abort_err)) {
        return true;
    }
    if (abort_err) {
        return false;
    }

    /* Step 8 (Tier 4): Hydrated `fallback_candidate_roots` x `domain_search_paths` matrix. */
    size_t root_cnt = (routes->candidate_root_count <= IO_FB_MAX_VFS_CANDIDATES)
        ? routes->candidate_root_count
        : IO_FB_MAX_VFS_CANDIDATES;
    for (size_t r = 0U; r < root_cnt; r++) {
        const char *cand = routes->fallback_candidate_roots[r];
        if (cand[0] == '\0' || strcmp(cand, s_vfs_state.active_root) == 0) {
            continue;
        }
        if (vfs_probe_base_with_domains(cand, domain, clean_path, aliased, routes,
                                        probe, sizeof(probe), out_path, out_size,
                                        track, &abort_err)) {
            return true;
        }
        if (abort_err) {
            return false;
        }
    }

    /* Step 9 (Tier 5): Zero-disk `.rodata` Config ROM vault fallback (`rom://<clean>`). */
    if (routes->rom_fallback_enabled) {
        const char *rom_match = NULL;
        if (io_fallback_find_rom_asset(clean_path, NULL) != NULL) {
            rom_match = clean_path;
        } else if (aliased[0] != '\0' && io_fallback_find_rom_asset(aliased, NULL) != NULL) {
            rom_match = aliased;
        }
        if (rom_match != NULL) {
            int n = snprintf(probe, sizeof(probe), "%s%s", IO_FB_ROM_URI_PREFIX, rom_match);
            bool ovf = false;
            if (n > 0 && (size_t)n < sizeof(probe) &&
                vfs_commit_resolved(probe, true, out_path, out_size, track, &ovf)) {
                return true;
            }
            if (ovf) {
                return false;
            }
        }
    }

    if (track) {
        s_vfs_state.misses++;
    }
    PATH_EMIT_LOG(LOG_DEBUG, -0.05f, "VFS miss for domain='%s' path='%s'",
                  domain ? domain : "(none)", rel_path);
    return false;
}

/**
 * @brief Resolves a subsystem directory path to its canonical `manifest.json` index.
 *
 * ### For Game Designers & Non-Programmers:
 * Locates the master `manifest.json` anchor for any configuration or domain subtree
 * without worrying about trailing slashes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates traversal containment prior to normalizing trailing slashes, formats
 * `<clean_sub>/manifest.json` within `PATH_FINDER_MAX_PATH`, logs truncation warnings,
 * and delegates resolution to `path_finder_resolve()`.
 * @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in]  subsystem_path Directory fragment naming the subsystem subtree.
 * @param[out] out_path       Caller output buffer receiving resolved manifest path.
 * @param[in]  out_size       Capacity of `out_path` in bytes.
 * @return true if a readable `manifest.json` was located; false otherwise.
 */
bool path_finder_find_manifest(const char *subsystem_path,
                               char *out_path,
                               size_t out_size)
{
    /* Step 1: Clear destination defensively and validate required inputs. */
    if (!vfs_out_buf_valid(out_path, out_size)) {
        return false;
    }
    out_path[0] = '\0';
    if (subsystem_path == NULL || subsystem_path[0] == '\0') {
        return false;
    }

    /* Step 2: Block traversal sequences before trimming trailing slashes. */
    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
    if (routes->block_path_traversal && vfs_has_parent_traversal(subsystem_path)) {
        vfs_record_traversal_rejection("find_manifest",
                                       "Blocked traversal '..' in subsystem manifest path",
                                       !s_vfs_state.in_preflight);
        return false;
    }

    /* Step 3: Trim trailing separators cleanly and compose `<subsystem>/manifest.json`. */
    char clean_sub[PATH_FINDER_MAX_PATH];
    str_safe_copy(clean_sub, subsystem_path, sizeof(clean_sub));
    size_t slen = strlen(clean_sub);
    while (slen > 0U && clean_sub[slen - 1U] == '/') {
        clean_sub[--slen] = '\0';
    }
    if (slen == 0U) {
        return false;
    }

    char target[PATH_FINDER_MAX_PATH];
    int n = snprintf(target, sizeof(target), "%s/manifest.json", clean_sub);
    if (n <= 0 || (size_t)n >= sizeof(target)) {
        if (!s_vfs_state.in_preflight) {
            s_vfs_state.overflow_rejections++;
            PATH_EMIT_LOG(LOG_WARN, -0.35f,
                          "find_manifest: path exceeds PATH_FINDER_MAX_PATH (%d B)",
                          PATH_FINDER_MAX_PATH);
        }
        return false;
    }

    return path_finder_resolve(NULL, target, out_path, out_size);
}

/**
 * @brief Resolves a media or art asset path across active mounts and `asset_roots`.
 *
 * ### For Game Designers & Non-Programmers:
 * Locates fonts, sprites, audio banks, and UI art across mounted override packs and
 * standard asset directories without hardcoding filesystem paths.
 *
 * ### For Systems Programmers & Engine Architects:
 * Guards `asset_category` and `asset_name` against traversal escapes, probes active
 * mount overlays (`<root>/assets/<cat>/<name>` then `<root>/<cat>/<name>`), iterates
 * hydrated `routes->asset_roots[]`, and falls back to `presentation/<cat>/<name>`.
 * @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in]  asset_category Category subfolder (`"fonts"`, `"sprites"`, `"audio"`, etc.).
 * @param[in]  asset_name     Asset filename within category.
 * @param[out] out_path       Caller destination path buffer.
 * @param[in]  out_size       Capacity of `out_path` in bytes.
 * @return true if asset file was located safely; false otherwise.
 */
bool path_finder_resolve_asset(const char *asset_category,
                               const char *asset_name,
                               char *out_path,
                               size_t out_size)
{
    /* Step 1: Clear output defensively and validate category, name, and buffer bounds. */
    if (!vfs_out_buf_valid(out_path, out_size)) {
        return false;
    }
    out_path[0] = '\0';
    if (asset_category == NULL || asset_category[0] == '\0' ||
        asset_name == NULL || asset_name[0] == '\0') {
        return false;
    }

    vfs_ensure_root_armed();
    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
    bool track = !s_vfs_state.in_preflight;

    if (track) {
        s_vfs_state.total_lookups++;
    }

    /* Step 2: Reject parent traversal segments in either category or filename. */
    if (routes->block_path_traversal &&
        (vfs_has_parent_traversal(asset_category) || vfs_has_parent_traversal(asset_name))) {
        vfs_record_traversal_rejection("resolve_asset",
                                       "Blocked traversal '..' in asset category or name",
                                       track);
        return false;
    }

    char probe[PATH_FINDER_MAX_PATH * 2U];
    bool abort_err = false;
    const char *const mount_patterns[2] = { "%s/assets/%s/%s", "%s/%s/%s" };

    /* Step 3: Probe active mounted root overlays (`assets/<cat>` before direct `<cat>`). */
    for (size_t i = 0U; i < 2U; i++) {
        int n = snprintf(probe, sizeof(probe), mount_patterns[i],
                         s_vfs_state.active_root, asset_category, asset_name);
        if (n > 0 && (size_t)n < sizeof(probe) &&
            vfs_try_candidate(probe, routes, out_path, out_size, track, &abort_err)) {
            return true;
        }
        if (abort_err) {
            return false;
        }
    }

    /* Step 4: Probe hydrated `asset_roots[]` table (`vfs_routes.json`). */
    size_t aroot_cnt = (routes->asset_root_count <= IO_FB_MAX_ASSET_ROOTS)
        ? routes->asset_root_count
        : IO_FB_MAX_ASSET_ROOTS;
    for (size_t a = 0U; a < aroot_cnt; a++) {
        if (routes->asset_roots[a][0] == '\0') {
            continue;
        }
        int n = snprintf(probe, sizeof(probe), "%s/%s/%s",
                         routes->asset_roots[a], asset_category, asset_name);
        if (n > 0 && (size_t)n < sizeof(probe) &&
            vfs_try_candidate(probe, routes, out_path, out_size, track, &abort_err)) {
            return true;
        }
        if (abort_err) {
            return false;
        }
    }

    /* Step 5: Fall back through presentation subtree via `path_finder_resolve()`. */
    int n_pres = snprintf(probe, sizeof(probe), "presentation/%s/%s",
                          asset_category, asset_name);
    if (n_pres > 0 && (size_t)n_pres < sizeof(probe) &&
        path_finder_resolve("presentation", probe, out_path, out_size)) {
        return true;
    }

    if (track) {
        s_vfs_state.misses++;
    }
    return false;
}

/**
 * @brief Resolves an asset by checking an explicit path hint prior to category fallbacks.
 *
 * ### For Game Designers & Non-Programmers:
 * Prioritizes an artist's or theme's custom direct file hint before falling back
 * smoothly to standard asset category directories.
 *
 * ### For Systems Programmers & Engine Architects:
 * Guards `direct_path_hint` against traversal escapes, probes `path_finder_resolve()`
 * on non-empty hints, then cascades through `path_finder_resolve_asset()` and generic
 * domain-scoped `path_finder_resolve()` when category/name are present.
 * @triggers `io.vfs.traversal_guard`, `LOG_CAT_IO`.
 *
 * @param[in]  direct_path_hint Preferred direct or relative artist path (nullable).
 * @param[in]  asset_category   Fallback category subfolder (`"sprites"`, `"fonts"`, nullable).
 * @param[in]  asset_name       Fallback filename (nullable when only testing direct hint).
 * @param[out] out_path         Caller destination path buffer.
 * @param[in]  out_size         Capacity of `out_path` in bytes.
 * @return true if any valid target resolved safely; false otherwise.
 */
bool path_finder_resolve_with_hint(const char *direct_path_hint,
                                   const char *asset_category,
                                   const char *asset_name,
                                   char *out_path,
                                   size_t out_size)
{
    /* Step 1: Clear output defensively and validate output buffer bounds. */
    if (!vfs_out_buf_valid(out_path, out_size)) {
        return false;
    }
    out_path[0] = '\0';

    vfs_ensure_root_armed();
    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();

    /* Step 2: Validate and probe direct artist path hint before asset category searches. */
    if (direct_path_hint != NULL && direct_path_hint[0] != '\0') {
        if (routes->block_path_traversal && vfs_has_parent_traversal(direct_path_hint)) {
            vfs_record_traversal_rejection("resolve_with_hint",
                                           "Blocked traversal '..' in direct_path_hint",
                                           !s_vfs_state.in_preflight);
            return false;
        }
        if (path_finder_resolve(NULL, direct_path_hint, out_path, out_size)) {
            return true;
        }
    }

    /* Step 3: Fall back to structured asset category roots when category and name exist. */
    if (asset_category != NULL && asset_category[0] != '\0' &&
        asset_name != NULL && asset_name[0] != '\0') {
        if (path_finder_resolve_asset(asset_category, asset_name, out_path, out_size)) {
            return true;
        }
    }

    /* Step 4: Last-resort generic domain lookup for bare asset filename. */
    if (asset_name != NULL && asset_name[0] != '\0') {
        return path_finder_resolve(asset_category, asset_name, out_path, out_size);
    }

    return false;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / CONTRACT VALIDATION & PRE-FLIGHT
Stratum: Layer 0 (Core Utilities) | Machine: core.io.path_finder
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract Verification & Live Debugger Probe Wiring
Mission: Validate `io_fb_vfs_routes_t` structural invariants and register the
         4 `io.vfs.*` validation probes cleanly into `debug.c`.

  ### For Game Designers & Non-Programmers:
  Verifies all loaded directory tables are structurally healthy and pauses in
  `DEBUG_STEP_AWAIT` immediately if corrupt route data enters the VFS.

  ### For Systems Programmers & Engine Architects:
  All Block 4 probe callbacks inspect current live state (never cumulative hit/miss
  counters) and perform zero heap allocations.
--------------------------------------------------------------------------------
*/

bool path_finder_validate_routes_contract(const io_fb_vfs_routes_t *routes)
{
    const io_fb_vfs_routes_t *target = (routes != NULL)
        ? routes
        : io_fallback_get_vfs_routes();

    if (!path_finder_routes_contract_ok(target)) {
        return false;
    }

    for (size_t t = 0U; t < target->config_precedence_count; t++) {
        if (target->config_precedence[t][0] == '\0') {
            return false;
        }
    }
    for (size_t r = 0U; r < target->candidate_root_count; r++) {
        if (target->fallback_candidate_roots[r][0] == '\0') {
            return false;
        }
    }
    return true;
}

static diag_result_t vfs_probe_route_bounds(void)
{
    diag_result_t res;
    res.subsystem = PATH_FINDER_MODULE_ID;
    res.check_name = "VFS Route Table Structural Bounds";
    res.passed = path_finder_validate_routes_contract(io_fallback_get_vfs_routes());
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "io_fb_vfs_routes_t precedence, roots, domains, and symlink limits valid."
        : "Active io_fb_vfs_routes_t breached structural route bounds contract.";
    return res;
}

static diag_result_t vfs_probe_root_bounds(void)
{
    vfs_ensure_root_armed();
    diag_result_t res;
    res.subsystem = PATH_FINDER_MODULE_ID;
    res.check_name = "Mounted VFS Root Path Bounds";
    size_t len = strlen(s_vfs_state.active_root);
    res.passed = path_finder_root_len_is_valid(len);
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "Mounted active VFS root within PATH_FINDER_MAX_ROOT (256 B)."
        : "Mounted VFS root empty or exceeds PATH_FINDER_MAX_ROOT bounds.";
    return res;
}

static diag_result_t vfs_probe_rom_parity(void)
{
    diag_result_t res;
    res.subsystem = PATH_FINDER_MODULE_ID;
    res.check_name = "Config ROM Vault Parity for io/vfs_routes.json";
    size_t rom_sz = 0U;
    const char *rom_asset = io_fallback_find_rom_asset(PATH_FINDER_ROM_ROUTES_URI, &rom_sz);
    bool contract_ok = path_finder_validate_routes_contract(io_fallback_get_vfs_routes());
    res.passed = (rom_asset != NULL && rom_sz > 0U && contract_ok);
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "Compiled .rodata ROM io/vfs_routes.json & Safe-Mode parity intact."
        : "Missing compiled ROM vfs_routes asset or parity contract breach.";
    return res;
}

static diag_result_t vfs_probe_traversal_guard(void)
{
    diag_result_t res;
    res.subsystem = PATH_FINDER_MODULE_ID;
    res.check_name = "Parent Traversal Sandbox Guard Verification";
    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
    bool bad_detected = vfs_has_parent_traversal("../../etc/passwd") &&
                        vfs_has_parent_traversal("a/../b");
    bool good_allowed = !vfs_has_parent_traversal("runtime/math/formulas.json") &&
                        !vfs_has_parent_traversal("shader..v1.json");
    res.passed = (routes != NULL && bad_detected && good_allowed);
    res.severity = res.passed ? SEVERITY_PASS : SEVERITY_CRITICAL;
    res.details = res.passed
        ? "VFS '..' traversal sandbox matcher active and non-blocking."
        : "VFS traversal sandbox predicate failed structural self-check.";
    return res;
}

void path_finder_register_debug_probes(void)
{
    if (!debug_is_active() || s_vfs_state.probes_registered) {
        return;
    }
    for (size_t i = 0U; i < sizeof(s_vfs_probes) / sizeof(s_vfs_probes[0]); i++) {
        (void)debug_register_validation_probe(&s_vfs_probes[i]);
    }
    s_vfs_state.probes_registered = true;
}

bool path_finder_trigger_routes_probe(const char *context_label)
{
    if (!debug_is_active()) {
        return false;
    }
    path_finder_register_debug_probes();
    return debug_trigger_probe(
        "io.vfs.route_bounds",
        PATH_FINDER_MODULE_ID,
        context_label ? context_label : "VFS route table contract breach detected",
        __FILE__,
        __LINE__,
        "path_finder_trigger_routes_probe"
    );
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Execute non-destructive startup verification of NULL resilience, route
         contracts, ROM fallback, traversal rejection, and Block 5 reporting.

  ### For Game Designers & Non-Programmers:
  Runs during `--audit` to confirm path searching, safety locks, and ROM backups
  work before launching gameplay.

  ### For Systems Programmers & Engine Architects:
  Snapshots `s_vfs_state`, runs checks under `in_preflight = true` without permanent
  telemetry pollution or debugger freezes, and restores exact caller state on exit.
--------------------------------------------------------------------------------
*/

diag_result_t path_finder_preflight(void)
{
    diag_result_t res;
    res.subsystem = path_finder_manifest.module_id;
    res.check_name = "VFS Layered Routing, Traversal Guard & ROM Pre-Flight";
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.details = "VFS Path Finder: Config precedence, traversal guard & ROM parity verified.";

    path_finder_vfs_state_t backup = s_vfs_state;
    s_vfs_state.in_preflight = true;
    vfs_ensure_root_armed();

    /* Check 1: NULL & zero-buffer resilience */
    char probe_buf[PATH_FINDER_MAX_PATH];
    if (path_finder_resolve(NULL, NULL, probe_buf, sizeof(probe_buf)) != false ||
        path_finder_resolve("config", "io/vfs_routes.json", NULL, 16U) != false ||
        path_finder_resolve("config", "io/vfs_routes.json", probe_buf, 0U) != false ||
        path_finder_find_manifest(NULL, probe_buf, sizeof(probe_buf)) != false ||
        path_finder_resolve_asset(NULL, NULL, probe_buf, sizeof(probe_buf)) != false ||
        path_finder_execute_resolve(NULL, NULL, NULL) != false) {
        s_vfs_state = backup;
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "path_finder NULL/zero-capacity resilience contract failed.";
        return res;
    }

    /* Check 2: Active route contract & ROM parity */
    if (!path_finder_validate_routes_contract(NULL) ||
        io_fallback_find_rom_asset(PATH_FINDER_ROM_ROUTES_URI, NULL) == NULL) {
        s_vfs_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Active VFS routes contract or compiled ROM parity asset missing.";
        return res;
    }

    /* Check 3: Explicit ROM lookup & traversal sandbox containment */
    if (!path_finder_resolve(NULL, PATH_FINDER_ROM_ROUTES_URI, probe_buf, sizeof(probe_buf)) ||
        !io_fb_is_rom_uri(probe_buf)) {
        s_vfs_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Virtual rom:// VFS route asset resolution failed.";
        return res;
    }

    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
    if (routes->block_path_traversal &&
        path_finder_resolve(NULL, "../../etc/passwd", probe_buf, sizeof(probe_buf)) != false) {
        s_vfs_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "VFS traversal sandbox failed to block parent escape path.";
        return res;
    }

    /* Check 4: Core config resolution & Block 5 diagnostic card formatter */
    path_resolved_info_t info;
    if (!path_finder_execute_resolve("config", PATH_FINDER_DEFAULT_ROUTES_PATH, &info) ||
        !path_finder_info_exists(&info)) {
        s_vfs_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_execute_resolve failed on io/vfs_routes.json.";
        return res;
    }

    char card_buf[1024];
    if (path_finder_execute_dump_card(&info, card_buf, sizeof(card_buf)) == 0U) {
        s_vfs_state = backup;
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "path_finder_execute_dump_card failed formatting status card.";
        return res;
    }

    s_vfs_state = backup;
    return res;
}

bool path_finder_audit(void)
{
    diag_result_t res = path_finder_preflight();
    return res.passed && res.severity != SEVERITY_FATAL;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES, CARDS & LIFECYCLE
Stratum: Layer 0 (Core Utilities) | Machine: core.io.path_finder
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
Mission: Construct structured `path_resolved_info_t` reports, render 60-column
         ASCII cards, and copy live VFS telemetry into `path_finder_stats_t`.

  ### For Game Designers & Non-Programmers:
  Creates human-readable diagnostic cards and live statistics showing disk hits,
  ROM backups, and security blocks cleanly.

  ### For Systems Programmers & Engine Architects:
  Deterministic bounded formatters (`snprintf`) with strict truncation returns and
  O(1) `.bss` telemetry snapshots.
--------------------------------------------------------------------------------
*/

bool path_finder_execute_resolve(const char *domain,
                                 const char *rel_path,
                                 path_resolved_info_t *out_info)
{
    if (out_info == NULL) {
        return false;
    }

    memset(out_info, 0, sizeof(*out_info));
    str_safe_copy(out_info->domain,
                  (domain != NULL && domain[0] != '\0') ? domain : "none",
                  sizeof(out_info->domain));
    if (rel_path != NULL) {
        str_safe_copy(out_info->rel_path, rel_path, sizeof(out_info->rel_path));
    }

    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
    out_info->routes_from_data = s_vfs_state.routes_from_data || routes->loaded_from_data;

    if (rel_path == NULL || rel_path[0] == '\0') {
        return false;
    }

    if (path_finder_resolve(domain, rel_path,
                            out_info->resolved_path,
                            sizeof(out_info->resolved_path))) {
        out_info->is_rom_virtual = io_fb_is_rom_uri(out_info->resolved_path);
        out_info->exists = true;
        out_info->is_regular = out_info->is_rom_virtual ||
                               io_fallback_fs_is_regular(out_info->resolved_path);
        return true;
    }

    return false;
}

size_t path_finder_execute_dump_card(const path_resolved_info_t *info,
                                     char *out_buf,
                                     size_t buf_size)
{
    if (info == NULL || !vfs_out_buf_valid(out_buf, buf_size)) {
        return 0U;
    }

    const char *node_type = info->is_rom_virtual
        ? "ROM VIRTUAL ASSET"
        : (info->is_regular ? "DISK REGULAR FILE"
                            : (info->exists ? "SPECIAL NODE" : "UNRESOLVED"));

    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "              VFS PATH RESOLUTION STATUS CARD               \n"
        "============================================================\n"
        "Domain:        %s\n"
        "Target Path:   %s\n"
        "Resolved Path: %s\n"
        "Node Type:     %s\n"
        "Routes Source: %s\n"
        "Status:        %s\n"
        "============================================================\n",
        info->domain[0] != '\0' ? info->domain : "none",
        info->rel_path[0] != '\0' ? info->rel_path : "(null)",
        info->resolved_path[0] != '\0' ? info->resolved_path : "(unresolved)",
        node_type,
        info->routes_from_data ? "DISK JSON (vfs_routes.json)" : "SAFE-MODE .rodata ROM",
        info->exists ? "RESOLVED" : "NOT FOUND"
    );

    if (written < 0 || (size_t)written >= buf_size) {
        out_buf[buf_size - 1U] = '\0';
        return 0U;
    }
    return (size_t)written;
}

bool path_finder_get_stats(path_finder_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }

    vfs_ensure_root_armed();
    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();

    memset(out_stats, 0, sizeof(*out_stats));
    out_stats->initialized = s_vfs_state.initialized;
    out_stats->routes_from_data = s_vfs_state.routes_from_data || routes->loaded_from_data;
    out_stats->block_path_traversal = routes->block_path_traversal;
    out_stats->allow_mod_mounts = routes->allow_mod_mounts;
    out_stats->rom_fallback_enabled = routes->rom_fallback_enabled;
    out_stats->max_symlink_depth = routes->max_symlink_depth;
    out_stats->config_tier_count = routes->config_precedence_count;
    out_stats->candidate_root_count = routes->candidate_root_count;
    out_stats->asset_root_count = routes->asset_root_count;
    out_stats->domain_search_count = routes->domain_search_count;
    out_stats->total_lookups = s_vfs_state.total_lookups;
    out_stats->disk_hits = s_vfs_state.disk_hits;
    out_stats->rom_hits = s_vfs_state.rom_hits;
    out_stats->misses = s_vfs_state.misses;
    out_stats->traversal_blocks = s_vfs_state.traversal_blocks;
    out_stats->overflow_rejections = s_vfs_state.overflow_rejections;
    str_safe_copy(out_stats->active_root, s_vfs_state.active_root,
                  sizeof(out_stats->active_root));
    return true;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Arm Tier-0 BIOS, hydrate `io/vfs_routes.json` (with Safe-Mode ROM
         fallback), register `io.vfs.*` probes, and log lifecycle summaries.

  ### For Game Designers & Non-Programmers:
  Cleanly boots the file path finder using disk routes when available and
  prints a final diagnostic summary on exit.

  ### For Systems Programmers & Engine Architects:
  Idempotent BIOS boot -> root init -> `vfs_hydrate_routes_internal(NULL)` ->
  `path_finder_register_debug_probes()`. Teardown resets custom mount state and
  clears `s_vfs_state.initialized`.
--------------------------------------------------------------------------------
*/

void path_finder_init(void)
{
    (void)io_fallback_boot(NULL);

    s_vfs_state.total_lookups = 0U;
    s_vfs_state.disk_hits = 0U;
    s_vfs_state.rom_hits = 0U;
    s_vfs_state.misses = 0U;
    s_vfs_state.traversal_blocks = 0U;
    s_vfs_state.overflow_rejections = 0U;
    s_vfs_state.probes_registered = false;

    if (!s_vfs_state.root_customized) {
        s_vfs_state.root_initialized = false;
    }
    vfs_ensure_root_armed();

    bool disk_loaded = vfs_hydrate_routes_internal(NULL);
    path_finder_register_debug_probes();
    s_vfs_state.initialized = true;

    const io_fb_vfs_routes_t *routes = io_fallback_get_vfs_routes();
    PATH_EMIT_LOG(LOG_INFO, 0.20f,
                  "VFS PathFinder initialized (source=%s, root='%s', domains=%zu, traversal_guard=%s)",
                  disk_loaded ? "DISK" : "ROM_SAFE_MODE",
                  s_vfs_state.active_root,
                  routes->domain_search_count,
                  routes->block_path_traversal ? "ON" : "OFF");
}

void path_finder_shutdown(void)
{
    PATH_EMIT_LOG(LOG_INFO, 0.0f,
                  "VFS PathFinder shutdown (lookups=%u, disk_hits=%u, rom_hits=%u, misses=%u, blocks=%u)",
                  s_vfs_state.total_lookups,
                  s_vfs_state.disk_hits,
                  s_vfs_state.rom_hits,
                  s_vfs_state.misses,
                  s_vfs_state.traversal_blocks);

    s_vfs_state.initialized = false;
    s_vfs_state.probes_registered = false;
    s_vfs_state.root_initialized = false;
    s_vfs_state.root_customized = false;
    vfs_ensure_root_armed();
}
