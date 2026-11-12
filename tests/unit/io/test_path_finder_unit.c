/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_path_finder_unit.c
@id            test.unit.io.path_finder
@layer         Layer 4: Verification Suite
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        VFS Path Finder Unit Verification Suite
@role          Validates multi-tier VFS resolution, `vfs_routes.json` hydration,
               `diagnostic_aliases`, `"rom://"` fallback, traversal/symlink containment,
               `LOG_CAT_IO` telemetry, and `io.vfs.*` probe -> `DEBUG_STEP_AWAIT` recovery.
@invariants    1. Strictly zero dynamic heap allocations inside engine calls under test.
               2. Executes all Four Ontological Testing Sections cleanly.
               3. Cleans all temporary `build/` probes/symlinks and restores default routes.
               4. Universal 5-Block Lifecycle Protocol & strict ISO C11 comments only.

PURPOSE & ARCHITECTURAL ROLE:
  Isolated Layer 4 unit test harness exercising every public contract, boundary check,
  security lock (`block_path_traversal`, `max_symlink_depth`, `allow_mod_mounts`),
  Safe-Mode `.rodata` ROM downgrade, and debugger tripwire in `io/path_finder.h`.

  ### For Game Designers & Non-Programmers:
  Verifies that config overrides load in proper priority, unsafe folder escapes and
  broken route configs freeze safely instead of loading garbage, and ROM backups work.

  ### For Systems Programmers & Engine Architects:
  Asserts `ALLOC_ZERO_HEAP`, 512/256-byte path bounds, `io.fallback` single-parser
  parity (`loaded_from_data`), symlink hop accounting, `LOG_CAT_IO` emissions, and
  deterministic `debug_is_awaiting()` -> `debug_resolve_await()` recovery.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / MODULES, TYPES & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

#define _POSIX_C_SOURCE 200809L

/* --- Tier 1: Standard ISO C11 Library Headers (+ POSIX symlink/unlink for depth tests) --- */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* --- Tier 2: External Library Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation & Layer 4 Shared Harness --- */
#include "common/cornerstone_test.h"
#include "core/manifest.h"
#include "debug/debug.h"
#include "io/io_fallback.h"
#include "io/path_finder.h"
#include "log/log.h"

static int s_tests_run = 0;
static int s_tests_passed = 0;

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION ONTOLOGICAL TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN BASELINE) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal VFS Baseline)...\n");

    path_finder_init();

    /* 1.1 Hydrated vfs_routes.json verification & stats */
    io_fb_vfs_routes_t routes;
    path_finder_stats_t stats;
    TEST_ASSERT(path_finder_get_routes(&routes) && routes.loaded_from_data &&
                path_finder_validate_routes_contract(&routes) &&
                (routes.domain_search_count >= 1U) &&
                (routes.domain_search_count <= IO_FB_MAX_VFS_DOMAIN_DIRS) &&
                routes.asset_root_count >= 4U &&
                routes.config_precedence_count == 3U &&
                strcmp(routes.diag_log_prefix, "diagnostics/telemetry/") == 0 &&
                strcmp(routes.diag_debug_prefix, "diagnostics/debugger/") == 0 &&
                strcmp(routes.diag_io_prefix, "io/") == 0,
                "path_finder_init hydrates bounded VFS domain dirs, asset roots & diagnostic aliases from disk");
    TEST_ASSERT(path_finder_get_stats(&stats) && stats.initialized &&
                stats.routes_from_data && stats.block_path_traversal &&
                stats.max_symlink_depth == 4 && stats.allow_mod_mounts,
                "path_finder_get_stats reflects initialized state and hydrated route policy");

    /* 1.2 Default root inspection */
    const char *root = path_finder_get_data_root();
    TEST_ASSERT(root != NULL && strlen(root) > 0U, "Default data root is non-empty");

    /* 1.3 Direct resolution relative to repository working tree */
    char out_path[PATH_FINDER_MAX_PATH];
    bool r_direct = path_finder_resolve(NULL, "CMakeLists.txt", out_path, sizeof(out_path));
    TEST_ASSERT(r_direct && strstr(out_path, "CMakeLists.txt") != NULL,
                "Resolves CMakeLists.txt via direct file check");

    /* 1.4 Config precedence & canonical redundant prefix stripping */
    bool r_sys = path_finder_resolve("config", "runtime/math/formulas.json",
                                     out_path, sizeof(out_path));
    TEST_ASSERT(r_sys && strstr(out_path, "formulas.json") != NULL,
                "Resolves config/runtime/math/formulas.json via config_precedence");

    char norm_path[PATH_FINDER_MAX_PATH];
    bool r_norm = path_finder_resolve(NULL, "engine/config/runtime/math/formulas.json",
                                      norm_path, sizeof(norm_path));
    TEST_ASSERT(r_norm && strcmp(norm_path, out_path) == 0,
                "Redundant 'engine/config/' prefix normalizes identically to domain='config'");

    /* 1.5 Diagnostic aliases (`log/`, `debug/`, `io/`) driven by vfs_routes.json */
    TEST_ASSERT(path_finder_resolve("config", "log/default.json", out_path, sizeof(out_path)) &&
                strstr(out_path, "diagnostics/telemetry/default.json") != NULL,
                "Resolves 'log/' alias -> diagnostics/telemetry/");
    TEST_ASSERT(path_finder_resolve("config", "debug/default.json", out_path, sizeof(out_path)) &&
                strstr(out_path, "diagnostics/debugger/default.json") != NULL,
                "Resolves 'debug/' alias -> diagnostics/debugger/");
    TEST_ASSERT(path_finder_resolve("io", "vfs_routes.json", out_path, sizeof(out_path)) &&
                strstr(out_path, "io/vfs_routes.json") != NULL,
                "Resolves domain='io' + diag_io_prefix -> io/vfs_routes.json");

    /* 1.6 Virtual `rom://` URI lookup and manifest/asset resolution */
    TEST_ASSERT(path_finder_resolve(NULL, PATH_FINDER_ROM_ROUTES_URI, out_path, sizeof(out_path)) &&
                strcmp(out_path, PATH_FINDER_ROM_ROUTES_URI) == 0,
                "Resolves explicit virtual rom://io/vfs_routes.json URI cleanly");

    char manifest_path[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(path_finder_find_manifest("config/diagnostics/telemetry",
                                          manifest_path, sizeof(manifest_path)) &&
                strstr(manifest_path, "manifest.json") != NULL,
                "Resolves telemetry subsystem manifest cleanly");

    char asset_path[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(path_finder_resolve_asset("fonts", "Cinzel-Bold.ttf",
                                          asset_path, sizeof(asset_path)) &&
                strstr(asset_path, "Cinzel-Bold.ttf") != NULL,
                "Resolves game font asset via hydrated asset_roots");

    /* 1.7 Structured execution & 60-col ASCII diagnostic dump card */
    path_resolved_info_t info;
    TEST_ASSERT(path_finder_execute_resolve("config", "runtime/math/formulas.json", &info) &&
                path_finder_info_exists(&info) && !info.is_rom_virtual && info.routes_from_data,
                "path_finder_execute_resolve populates structured report accurately");

    char card[768];
    size_t clen = path_finder_execute_dump_card(&info, card, sizeof(card));
    TEST_ASSERT(clen > 0U && strstr(card, "VFS PATH RESOLUTION STATUS CARD") != NULL &&
                strstr(card, "DISK REGULAR FILE") != NULL,
                "path_finder_execute_dump_card prints 60-column framed VFS status card");

    TEST_ASSERT(path_finder_get_stats(&stats) && stats.disk_hits > 0U && stats.rom_hits > 0U,
                "VFS telemetry counts both disk_hits and rom_hits accurately");

    path_finder_shutdown();
    printf("  [PASS-1] Section 1 passed: Nominal VFS lookups, aliases, assets & ROM verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (INVALID INPUTS, TRAVERSAL & BOUNDARY HARDENING) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault-Tolerant Negative Tests)...\n");

    path_finder_init();
    char out_path[PATH_FINDER_MAX_PATH];

    /* 2.1 NULL pointers & zero buffer capacity */
    TEST_ASSERT(!path_finder_resolve(NULL, NULL, out_path, sizeof(out_path)),
                "path_finder_resolve(NULL rel_path)");
    TEST_ASSERT(!path_finder_resolve("config", "formulas.json", NULL, sizeof(out_path)),
                "path_finder_resolve(NULL out_path)");
    TEST_ASSERT(!path_finder_resolve("config", "formulas.json", out_path, 0U),
                "path_finder_resolve(0 size)");
    TEST_ASSERT(!path_finder_find_manifest(NULL, out_path, sizeof(out_path)),
                "path_finder_find_manifest(NULL path)");
    TEST_ASSERT(!path_finder_find_manifest("config/", NULL, sizeof(out_path)),
                "path_finder_find_manifest(NULL out)");
    TEST_ASSERT(!path_finder_find_manifest("", out_path, sizeof(out_path)),
                "path_finder_find_manifest(empty path)");
    TEST_ASSERT(!path_finder_resolve_asset(NULL, "Cinzel-Bold.ttf", out_path, sizeof(out_path)),
                "path_finder_resolve_asset(NULL cat)");
    TEST_ASSERT(!path_finder_resolve_asset("fonts", NULL, out_path, sizeof(out_path)),
                "path_finder_resolve_asset(NULL name)");
    TEST_ASSERT(!path_finder_resolve_asset("fonts", "Cinzel-Bold.ttf", out_path, 0U),
                "path_finder_resolve_asset(0 size)");
    TEST_ASSERT(!path_finder_resolve_with_hint(NULL, "fonts", NULL, out_path, sizeof(out_path)),
                "path_finder_resolve_with_hint(NULL hint & name)");
    TEST_ASSERT(!path_finder_get_routes(NULL) && !path_finder_get_stats(NULL),
                "Getters reject NULL pointers cleanly");

    /* 2.2 Safe root reset & nonexistent probes */
    TEST_ASSERT(path_finder_set_data_root(NULL), "path_finder_set_data_root(NULL) resets safely");
    TEST_ASSERT(strlen(path_finder_get_data_root()) > 0U, "Reset data root is non-empty");
    TEST_ASSERT(!path_finder_resolve("config", "non_existent_file_xyz_999.json",
                                     out_path, sizeof(out_path)),
                "Missing file returns false cleanly");
    TEST_ASSERT(!path_finder_find_manifest("non_existent_subsystem_dir_xyz",
                                           out_path, sizeof(out_path)),
                "Missing subsystem manifest returns false without root manifest false match");
    TEST_ASSERT(!path_finder_resolve_asset("visuals", "non_existent_sprite_999.png",
                                           out_path, sizeof(out_path)),
                "Missing asset returns false cleanly");

    /* 2.3 Traversal containment (`..`) across resolve, manifest, asset, hint & mount */
    path_finder_stats_t before_stats;
    path_finder_stats_t after_stats;
    TEST_ASSERT(path_finder_get_stats(&before_stats), "Captured baseline traversal stats");

    TEST_ASSERT(!path_finder_resolve(NULL, "../../etc/passwd", out_path, sizeof(out_path)),
                "Blocks external '../..' escape path");
    TEST_ASSERT(!path_finder_resolve("config", "runtime/../../secret.json", out_path, sizeof(out_path)),
                "Blocks embedded '/..' parent traversal segment");
    TEST_ASSERT(!path_finder_find_manifest("../config", out_path, sizeof(out_path)),
                "Blocks '..' traversal in find_manifest");
    TEST_ASSERT(!path_finder_resolve_asset("..", "CMakeLists.txt", out_path, sizeof(out_path)),
                "Blocks '..' traversal in asset category");
    TEST_ASSERT(!path_finder_resolve_with_hint("../CMakeLists.txt", NULL, NULL, out_path, sizeof(out_path)),
                "Blocks '..' traversal in direct_path_hint");
    TEST_ASSERT(!path_finder_set_data_root("../outside_root"),
                "Blocks '..' traversal in set_data_root");

    TEST_ASSERT(path_finder_get_stats(&after_stats) &&
                after_stats.traversal_blocks >= before_stats.traversal_blocks + 6U,
                "All 6 traversal attempts recorded in traversal_blocks counter");

    /* 2.4 Non-matching Execute & Dump NULL safety */
    path_resolved_info_t info;
    TEST_ASSERT(!path_finder_execute_resolve("config", "formulas.json", NULL),
                "execute_resolve(NULL info) rejected");
    TEST_ASSERT(!path_finder_execute_resolve(NULL, NULL, &info) && !info.exists,
                "execute_resolve(NULL rel_path) clears info cleanly");
    TEST_ASSERT(path_finder_execute_dump_card(NULL, out_path, sizeof(out_path)) == 0U,
                "execute_dump_card(NULL info) returns 0");

    path_finder_shutdown();
    printf("  [FAIL-2] Section 2 passed: All invalid inputs and traversal escapes rejected cleanly.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (CONTRACTS, SYMLINKS & PROBE -> AWAIT) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Contracts, Symlinks & Probes)...\n");

    path_finder_init();

    /* 3.1 Strict buffer ceiling, overflow accounting & NUL termination */
    char tiny_buf[8];
    tiny_buf[0] = 'X';
    bool r_tiny = path_finder_resolve("config", "runtime/math/formulas.json",
                                      tiny_buf, sizeof(tiny_buf));
    TEST_ASSERT(!r_tiny && tiny_buf[0] == '\0',
                "Tiny 8-byte output buffer fails cleanly with NUL termination");

    char small_card[16];
    path_resolved_info_t ok_info;
    TEST_ASSERT(path_finder_execute_resolve("config", "runtime/math/formulas.json", &ok_info),
                "Prepared resolved info for truncation card test");
    TEST_ASSERT(path_finder_execute_dump_card(&ok_info, small_card, sizeof(small_card)) == 0U &&
                small_card[sizeof(small_card) - 1U] == '\0',
                "Truncated dump card returns 0 and unconditionally NUL-terminates");

    /* Oversized relative query exceeding PATH_FINDER_MAX_PATH */
    char long_path[600];
    memset(long_path, 'a', sizeof(long_path) - 1U);
    long_path[sizeof(long_path) - 1U] = '\0';
    char out_buf[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(!path_finder_resolve("config", long_path, out_buf, sizeof(out_buf)),
                "Query exceeding PATH_FINDER_MAX_PATH safely rejected");

    /* Manifest invariants */
    TEST_ASSERT(path_finder_manifest.alloc_policy == ALLOC_ZERO_HEAP,
                "Manifest enforces ALLOC_ZERO_HEAP");
    TEST_ASSERT(path_finder_manifest.concurrency == CONCURRENCY_REENTRANT,
                "Manifest specifies CONCURRENCY_REENTRANT");

    /* 3.2 Zero-disk ROM fallback tier when disk candidates are disconnected */
    io_fb_vfs_routes_t saved_routes;
    TEST_ASSERT(path_finder_get_routes(&saved_routes), "Saved disk VFS routes");
    io_fb_vfs_routes_t rom_only_routes = saved_routes;
    rom_only_routes.config_precedence_count = 0U;
    rom_only_routes.candidate_root_count = 1U;
    snprintf(rom_only_routes.fallback_candidate_roots[0],
             sizeof(rom_only_routes.fallback_candidate_roots[0]),
             "%s", "build/nonexistent_vfs_root_dir");
    rom_only_routes.rom_fallback_enabled = true;
    TEST_ASSERT(path_finder_set_routes(&rom_only_routes), "Applied diskless-simulation VFS routes");

    path_resolved_info_t rom_info;
    TEST_ASSERT(path_finder_execute_resolve("config", "io/vfs_routes.json", &rom_info) &&
                rom_info.exists && rom_info.is_regular && rom_info.is_rom_virtual &&
                strncmp(rom_info.resolved_path, "rom://", 6U) == 0,
                "Absent disk configs automatically degrade cleanly to virtual rom:// URI fallback");
    TEST_ASSERT(path_finder_set_routes(&saved_routes), "Restored full disk VFS routes");

    /* 3.3 `max_symlink_depth` enforcement using bounded symlink chain in build/ */
    const char *sym_target = "build/test_pf_sym_target.txt";
    const char *sym_hop1 = "build/test_pf_sym1.txt";
    const char *sym_hop2 = "build/test_pf_sym2.txt";
    (void)unlink(sym_hop2);
    (void)unlink(sym_hop1);
    (void)unlink(sym_target);
    TEST_ASSERT(io_fallback_fs_write_with_flags(sym_target, "{\"sym\":true}", 12U,
                                                0755U, false, false, true),
                "Created temporary symlink target in build/");
    if (symlink("test_pf_sym_target.txt", sym_hop1) == 0 &&
        symlink("test_pf_sym1.txt", sym_hop2) == 0) {
        io_fb_vfs_routes_t sym_routes = saved_routes;
        sym_routes.max_symlink_depth = 2;
        TEST_ASSERT(path_finder_set_routes(&sym_routes), "Configured max_symlink_depth = 2");
        TEST_ASSERT(path_finder_resolve(NULL, sym_hop2, out_buf, sizeof(out_buf)),
                    "2-hop symlink resolves cleanly when max_symlink_depth == 2");

        sym_routes.max_symlink_depth = 1;
        TEST_ASSERT(path_finder_set_routes(&sym_routes), "Configured max_symlink_depth = 1");
        TEST_ASSERT(!path_finder_resolve(NULL, sym_hop2, out_buf, sizeof(out_buf)),
                    "2-hop symlink blocked cleanly when max_symlink_depth == 1");
        TEST_ASSERT(path_finder_set_routes(&saved_routes), "Restored original symlink depth limit");
    }
    (void)unlink(sym_hop2);
    (void)unlink(sym_hop1);
    (void)unlink(sym_target);

    /* 3.4 `allow_mod_mounts = false` lock enforcement */
    io_fb_vfs_routes_t locked_routes = saved_routes;
    locked_routes.allow_mod_mounts = false;
    TEST_ASSERT(path_finder_set_routes(&locked_routes), "Applied allow_mod_mounts=false policy");
    TEST_ASSERT(!path_finder_set_data_root("build/blocked_mod_dir"),
                "Custom external mod mount rejected when allow_mod_mounts=false");
    TEST_ASSERT(path_finder_set_data_root(NULL),
                "Default root reset still permitted when allow_mod_mounts=false");
    TEST_ASSERT(path_finder_set_routes(&saved_routes), "Restored allow_mod_mounts=true");

    /* 3.5 Rev 5 Probes, Safe-Mode ROM downgrade, DEBUG_STEP_AWAIT & LOG_CAT_IO telemetry */
    {
        log_config_t lcfg = log_get_monolith_fallback_config();
        lcfg.echo_to_console = false;
        lcfg.save_by_category = false;
        lcfg.min_severity = LOG_SEV_INFO;
        snprintf(lcfg.log_dir, sizeof(lcfg.log_dir), "%s", "build/logs");
        TEST_ASSERT(log_init(&lcfg), "Initialized telemetry ring for VFS LOG_CAT_IO verification");
        TEST_ASSERT(debug_init(NULL), "Initialized debugger for io.vfs.* probe verification");
        path_finder_register_debug_probes();
        {
            cornerstone_test_probe_check_t rb_chk =
                cornerstone_test_check_probe("io.vfs.route_bounds", PATH_FINDER_MODULE_ID);
            cornerstone_test_probe_check_t root_chk =
                cornerstone_test_check_probe("io.vfs.root_bounds", PATH_FINDER_MODULE_ID);
            TEST_ASSERT(rb_chk.found && rb_chk.passed && root_chk.found && root_chk.passed,
                        "io.vfs.* probes registered and pass non-destructively");
        }

        /* (a) Nominal tripwire sweep must pass zero failures */
        TEST_ASSERT(debug_check_tripwires(NULL) == 0U && !debug_is_awaiting(),
                    "Healthy VFS probes pass debug_check_tripwires() cleanly");

        /* (b) Non-awaiting io.vfs.traversal_guard reports telemetry without stalling */
        TEST_ASSERT(!path_finder_resolve(NULL, "../../etc/shadow", out_buf, sizeof(out_buf)),
                    "Traversal blocked under active debugger");
        debug_tripwire_report_t rep;
        TEST_ASSERT(!debug_is_awaiting() &&
                    debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.vfs.traversal_guard") == 0,
                    "io.vfs.traversal_guard logs cleanly without locking DEBUG_STEP_AWAIT");

        /* (c) Corrupt io_fb_vfs_routes_t fires io.vfs.route_bounds -> ROM Safe Mode -> AWAIT */
        io_fb_vfs_routes_t bad_routes = saved_routes;
        bad_routes.candidate_root_count = 0U;
        bad_routes.max_symlink_depth = 99;
        TEST_ASSERT(!path_finder_set_routes(&bad_routes),
                    "path_finder_set_routes rejects corrupt VFS route bounds");
        io_fb_vfs_routes_t downgraded;
        TEST_ASSERT(path_finder_get_routes(&downgraded) && !downgraded.loaded_from_data &&
                    path_finder_validate_routes_contract(&downgraded),
                    "Rejected route table downgraded to Safe-Mode ROM (loaded_from_data=false)");
        TEST_ASSERT(debug_is_awaiting() &&
                    debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.vfs.route_bounds") == 0,
                    "Corrupt VFS routes halted debugger in DEBUG_STEP_AWAIT via io.vfs.route_bounds");
        TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                        "VFS route table downgraded cleanly to ROM Safe Mode") &&
                    !debug_is_awaiting(),
                    "debug_resolve_await cleared io.vfs.route_bounds AWAIT cleanly");

        /* (d) Oversized mount root fires io.vfs.root_bounds -> DEBUG_STEP_AWAIT */
        char oversized_root[320];
        memset(oversized_root, 'r', sizeof(oversized_root) - 1U);
        oversized_root[sizeof(oversized_root) - 1U] = '\0';
        TEST_ASSERT(!path_finder_set_data_root(oversized_root),
                    "path_finder_set_data_root rejects >= 256 byte root mount");
        TEST_ASSERT(debug_is_awaiting() &&
                    debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.vfs.root_bounds") == 0,
                    "Oversized mount root halted debugger via io.vfs.root_bounds");
        TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                        "Preserved prior bounded VFS mount root") &&
                    !debug_is_awaiting(),
                    "debug_resolve_await cleared io.vfs.root_bounds AWAIT cleanly");

        /* (e) Corrupt disk vfs_routes.json fires io.vfs.rom_parity -> ROM Safe Mode -> AWAIT */
        const char *corrupt_path = "build/test_corrupt_vfs_routes.json";
        TEST_ASSERT(io_fallback_fs_write_with_flags(corrupt_path, "{ invalid_vfs_json ", 19U,
                                                    0755U, false, false, true),
                    "Wrote temporary corrupt vfs_routes JSON file in build/");
        TEST_ASSERT(!path_finder_load_routes(corrupt_path),
                    "Corrupt on-disk vfs_routes JSON rejected safely");
        (void)remove(corrupt_path);
        TEST_ASSERT(path_finder_get_routes(&downgraded) && !downgraded.loaded_from_data &&
                    debug_is_awaiting() &&
                    debug_get_tripwire_report(&rep) &&
                    strcmp(rep.probe_id, "io.vfs.rom_parity") == 0,
                    "Corrupt disk vfs_routes triggers io.vfs.rom_parity -> ROM -> DEBUG_STEP_AWAIT");
        TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN,
                                        "Removed corrupt vfs_routes disk file; ROM active") &&
                    !debug_is_awaiting(),
                    "debug_resolve_await cleared io.vfs.rom_parity AWAIT cleanly");

        /* (f) Verify LOG_CAT_IO entries were emitted into telemetry ring */
        log_entry_t lentries[32];
        size_t lcnt = log_get_recent_entries(lentries, 32U);
        bool saw_io_log = false;
        for (size_t i = 0U; i < lcnt; i++) {
            if (lentries[i].category == LOG_CAT_IO) {
                saw_io_log = true;
            }
        }
        TEST_ASSERT(saw_io_log, "path_finder emitted LOG_CAT_IO telemetry entries");

        debug_shutdown();
        log_shutdown();
    }

    TEST_ASSERT(path_finder_load_routes(NULL), "Reloaded canonical disk vfs_routes.json");
    path_finder_shutdown();
    printf("  [AXIOM-3] Section 3 passed: Contracts, symlinks, ROM parity & probes strictly satisfied.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (ENVIRONMENT OVERRIDES & MOUNT HIERARCHY) ]
--------------------------------------------------------------------------------
*/

static void test_path_finder_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Dynamic Mount Re-routing)...\n");

    path_finder_init();

    /* Dynamically mount a custom root, verify domain-specific & engine config fallthrough */
    const char *custom_mount = "build/test_mount_root";
    TEST_ASSERT(io_fallback_fs_write_with_flags("build/test_mount_root/mod/override.json",
                                                "{\"mod\":true}", 12U, 0755U, false, false, true),
                "Prepared custom VFS mod mount payload in build/");
    bool set_ok = path_finder_set_data_root(custom_mount);
    TEST_ASSERT(set_ok, "Dynamically mounted custom build VFS data root");
    TEST_ASSERT(strcmp(path_finder_get_data_root(), custom_mount) == 0,
                "Mounted data root matches configuration");

    char resolved[PATH_FINDER_MAX_PATH];
    TEST_ASSERT(path_finder_resolve("mod", "override.json", resolved, sizeof(resolved)) &&
                strstr(resolved, "build/test_mount_root/mod/override.json") != NULL,
                "Custom mount resolves domain-specific override file with priority");
    (void)remove("build/test_mount_root/mod/override.json");

    /* Confirm engine config resolution falls through config_precedence cleanly */
    bool r_ok = path_finder_resolve(NULL, "engine/config/runtime/math/formulas.json",
                                    resolved, sizeof(resolved));
    TEST_ASSERT(r_ok, "Core config fallback remains operational during custom VFS mount");

    /* Execute full resolved reporting across custom mount */
    path_resolved_info_t report;
    bool exec_ok = path_finder_execute_resolve("config", "runtime/math/formulas.json", &report);
    TEST_ASSERT(exec_ok && report.exists && report.is_regular,
                "execute_resolve integrates seamlessly with dynamic root shifts");

    /* Reset cleanly */
    path_finder_set_data_root(NULL);
    path_finder_shutdown();

    printf("  [EMERGE-4] Section 4 passed: Custom mount routing and engine fallthrough converged.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK / SELF-TEST VERIFICATION
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

static void test_path_finder_preflight_audit(void)
{
    printf("[AUDIT] Executing path_finder Machine Pre-Flight Audit...\n");
    diag_result_t res = path_finder_preflight();
    TEST_ASSERT(res.passed && res.severity == SEVERITY_PASS, "path_finder_preflight passed");
    TEST_ASSERT(path_finder_audit(), "path_finder_audit passed");
    printf("[AUDIT] path_finder preflight audit: %s\n", res.details);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.path_finder
================================================================================
*/

int main(void)
{
    cornerstone_test_begin("path_finder Subsystem Unit Test Suite", "UNIT");

    test_path_finder_preflight_audit();
    test_path_finder_always_pass();
    test_path_finder_never_fail();
    test_path_finder_assertions_and_guarantees();
    test_path_finder_emergence();

    cornerstone_test_summary("path_finder Unit Suite", s_tests_run, s_tests_passed);
    return 0;
}
