/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_io_fallback_unit.c
@id            test.unit.io.fallback
@layer         Layer 4: Verification Suite
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-11
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Tier-0 BIOS I/O & Config ROM Vault Unit Verification
@role          Comprehensive 4-Section unit verification covering compiled-in
               .rodata IO config ROM vault lookup/envelopes, zero-disk "rom://"
               URI reads, shared policy parsing, all 4 BIOS fallback pillars
               (Filesystem, JSON, VFS PathFinder, MasterLoader), floppy
               read-only write locks, Safe-Mode downgrades, and DEBUG_STEP_AWAIT
               probe wiring.
@invariants    1. Zero dynamic heap allocations in test execution (ALLOC_ZERO_HEAP).
               2. All 4 Ontological Testing Sections + Block 4 Audit verified.
               3. Fallback/ROM configs keep loaded_from_data == false.
               4. Universal 5-Block Lifecycle Protocol & ISO C comments enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Validates the standalone Tier-0 Safe-Mode BIOS layer (`io/io_fallback.h`)
  guaranteeing that Tenebris and Lux can boot cleanly, read embedded engine I/O
  policies, parse 5-block manifests, block disk writes on read-only media, and
  halt cleanly in DEBUG_STEP_AWAIT on corrupted policies.

  ### For Game Designers & Non-Programmers:
  Verifies that even if config files are missing from disk or run off a
  write-protected floppy, the game boots cleanly using built-in factory ROM
  rules without ever crashing or silently overwriting save data.

  ### For Systems Programmers & Engine Architects:
  Exercises `io_fallback_config_t` construction, `.rodata` ROM integrity across
  all generated `io_config_rom.h` entries, `io_fallback_parse_policy_json()`,
  the 4 Safe-Mode I/O pillars, fault ring recording, 60-col dump cards, and
  `io.fallback.*` validation probe wiring into `debug.h`.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fallback
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
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation & Layer 4 Shared Harness --- */
#include "common/cornerstone_test.h"
#include "core/manifest.h"
#include "core/string_utils.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"
#include "io/io_fallback.h"
#include "log/log.h"
#include "log/log_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Scratch Capacity Constants
--------------------------------------------------------------------------------
*/

/** Scratch buffer size matching default medium single-config cap (8 KB). */
#define TEST_IO_FB_SCRATCH_BYTES   IO_FB_MEDIUM_BYTES

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Test Fixture Metadata Anchor & Counters
--------------------------------------------------------------------------------
*/

/** Logical unit harness identifier emitted during audit reporting. */
#define TEST_IO_FALLBACK_ID        "test.unit.io.fallback"

static int s_tests_run = 0;
static int s_tests_passed = 0;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Scratch Buffer Declarations (.bss, Zero Heap)
--------------------------------------------------------------------------------
*/

/** Static non-heap scratch buffer used for bounded disk/ROM asset loads. */
static char s_test_scratch[TEST_IO_FB_SCRATCH_BYTES];

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Inline Test Fixtures & Helper Predicates
--------------------------------------------------------------------------------
*/

/** Cleanly removes any temporary test file without failing if absent. */
static inline void test_io_fb_cleanup_file(const char *path)
{
    if (path != NULL) {
        (void)remove(path);
    }
}

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION UNIT TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fallback
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL ROM VAULT & ALL 4 BIOS PILLARS) ]
--------------------------------------------------------------------------------
*/

static void test_io_fb_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (ROM Vault & 4 BIOS Pillars)...\n");

    io_fallback_reset_for_testing();
    TEST_ASSERT(io_fallback_boot(NULL), "io_fallback_boot(NULL) arms BIOS cleanly");
    TEST_ASSERT(io_fallback_is_booted(), "io_fallback_is_booted returns true after boot");

    /* 1.1 Compiled .rodata ROM vault inventory and lookup */
    size_t rom_count = io_fallback_rom_count();
    TEST_ASSERT(rom_count >= 5U, "ROM vault embeds all >= 5 engine/config/io JSON files");

    for (size_t i = 0U; i < rom_count; i++) {
        const char *id = NULL;
        const char *rel = NULL;
        size_t sz = 0U;
        const char *payload = io_fallback_rom_asset_at(i, &sz);
        TEST_ASSERT(payload != NULL && sz > 0U, "ROM payload pointer and byte size valid");
        TEST_ASSERT(io_fallback_rom_info_at(i, &id, &rel, &sz) && id != NULL && rel != NULL,
                    "ROM info metadata populated cleanly");
    }

    size_t rom_sz = 0U;
    TEST_ASSERT(io_fallback_find_rom_asset("default", &rom_sz) != NULL && rom_sz > 0U,
                "ROM lookup resolves stem 'default'");
    TEST_ASSERT(io_fallback_find_rom_asset("fs_buffers", &rom_sz) != NULL && rom_sz > 0U,
                "ROM lookup resolves stem 'fs_buffers'");
    TEST_ASSERT(io_fallback_find_rom_asset("vfs_routes", &rom_sz) != NULL && rom_sz > 0U,
                "ROM lookup resolves stem 'vfs_routes'");
    TEST_ASSERT(io_fallback_find_rom_asset("loader_contracts", &rom_sz) != NULL && rom_sz > 0U,
                "ROM lookup resolves stem 'loader_contracts'");
    TEST_ASSERT(io_fallback_find_rom_asset("manifest", &rom_sz) != NULL && rom_sz > 0U,
                "ROM lookup resolves stem 'manifest'");
    TEST_ASSERT(io_fallback_find_rom_asset("rom://io/default.json", &rom_sz) != NULL && rom_sz > 0U,
                "ROM lookup resolves virtual URI 'rom://io/default.json'");

    /* 1.2 Policy constructors and shared JSON parser */
    io_fallback_config_t def_cfg = io_fallback_get_default_config();
    io_fallback_config_t flp_cfg = io_fallback_get_floppy_config();
    TEST_ASSERT(io_fallback_validate_policy_contract(&def_cfg), "Default BIOS template passes bounds");
    TEST_ASSERT(io_fallback_validate_policy_contract(&flp_cfg), "Floppy BIOS template passes bounds");
    TEST_ASSERT(flp_cfg.read_only_mode, "Floppy preset enables read_only_mode=true");

    io_fallback_config_t rom_fs_cfg;
    TEST_ASSERT(io_fallback_load_rom_config("fs_buffers", &rom_fs_cfg),
                "Hydrated fs_buffers profile directly from .rodata ROM");
    TEST_ASSERT(rom_fs_cfg.small_bytes == 1024U &&
                rom_fs_cfg.medium_bytes == 8192U &&
                rom_fs_cfg.large_bytes == 32768U &&
                rom_fs_cfg.max_config_bytes == 65536U,
                "ROM-parsed fs_buffers ladder matches 1K/8K/32K/64K contract");

    /* 1.3 Pillar 1: Filesystem Core BIOS (zero-disk rom:// & POSIX disk R/W) */
    TEST_ASSERT(io_fallback_fs_exists("rom://io/default.json"), "Virtual ROM URI exists");
    TEST_ASSERT(io_fallback_fs_is_regular("rom://io/default.json"), "Virtual ROM URI reports regular");
    TEST_ASSERT(io_fallback_fs_get_size("rom://io/default.json") > 0L, "Virtual ROM byte size positive");

    io_fb_file_info_t fb_info;
    TEST_ASSERT(io_fallback_fs_inspect("rom://io/default.json", &fb_info) &&
                fb_info.exists && fb_info.is_regular && fb_info.is_rom_virtual &&
                !fb_info.is_directory && fb_info.size_bytes > 0L,
                "io_fallback_fs_inspect identifies virtual .rodata ROM stream");

    size_t read_len = 0U;
    TEST_ASSERT(io_fallback_fs_read_bounded("rom://io/fs_buffers.json",
                                             s_test_scratch,
                                             sizeof(s_test_scratch),
                                             &read_len),
                "Zero-disk bounded read from rom://io/fs_buffers.json succeeded");
    TEST_ASSERT(read_len > 0U && s_test_scratch[read_len] == '\0',
                "ROM bounded read preserves exact length & null terminator");

    const char *tmp_file = "build/unit_io_fb_write.tmp";
    const char *tmp_payload = "{\"status\":\"bios_ok\"}";
    test_io_fb_cleanup_file(tmp_file);
    TEST_ASSERT(io_fallback_fs_write_bounded(tmp_file, tmp_payload, strlen(tmp_payload)),
                "Writable BIOS fallback mode commits bounded file write cleanly");
    char verify_buf[64];
    size_t verify_len = 0U;
    TEST_ASSERT(io_fallback_fs_read_bounded(tmp_file, verify_buf, sizeof(verify_buf), &verify_len) &&
                verify_len == strlen(tmp_payload) &&
                strcmp(verify_buf, tmp_payload) == 0,
                "Written disk payload round-trips accurately via BIOS reader");
    test_io_fb_cleanup_file(tmp_file);

    /* 1.4 Pillar 2: JSON Core BIOS (key, block, string, int, bool) */
    const char *body_ptr = NULL;
    size_t body_len = 0U;
    TEST_ASSERT(io_fallback_json_find_key(s_test_scratch, "block_3_body") != NULL,
                "JSON BIOS locates block_3_body key");
    TEST_ASSERT(io_fallback_json_find_block(s_test_scratch, "block_3_body", '{', '}',
                                            &body_ptr, &body_len) &&
                body_ptr != NULL && body_len > 0U,
                "JSON BIOS extracts balanced block_3_body span");

    char parsed_str[64];
    int parsed_int = 0;
    bool parsed_bool = false;
    TEST_ASSERT(io_fallback_json_get_string(s_test_scratch, "profile_name",
                                            parsed_str, sizeof(parsed_str)) &&
                strcmp(parsed_str, "fs_buffers") == 0,
                "JSON BIOS reads string profile_name='fs_buffers'");
    TEST_ASSERT(io_fallback_json_get_int(s_test_scratch, "medium_bytes", &parsed_int) &&
                parsed_int == 8192,
                "JSON BIOS reads int medium_bytes=8192");
    TEST_ASSERT(io_fallback_json_get_bool(s_test_scratch, "reject_on_truncation", &parsed_bool) &&
                parsed_bool == true,
                "JSON BIOS reads bool reject_on_truncation=true");

    /* 1.5 Pillar 3: VFS PathFinder Core BIOS (root & zero-disk ROM URI fallback) */
    TEST_ASSERT(io_fallback_path_get_root() != NULL && strlen(io_fallback_path_get_root()) > 0U,
                "Active VFS BIOS root is initialized non-empty");
    TEST_ASSERT(io_fallback_path_set_root("game/data"), "Set Safe-Mode VFS root cleanly");

    char resolved_path[IO_FB_PATH_MAX];
    TEST_ASSERT(io_fallback_path_resolve("config", "io/default.json",
                                         resolved_path, sizeof(resolved_path)),
                "PathFinder BIOS resolves 'io/default.json' via layered VFS");
    TEST_ASSERT(io_fallback_path_resolve("config", "rom://io/default.json",
                                         resolved_path, sizeof(resolved_path)) &&
                strcmp(resolved_path, "rom://io/default.json") == 0,
                "PathFinder BIOS preserves explicit rom:// virtual URI resolution");
    TEST_ASSERT(io_fallback_path_resolve(NULL, "loader_contracts",
                                         resolved_path, sizeof(resolved_path)) &&
                strcmp(resolved_path, "rom://loader_contracts") == 0,
                "PathFinder BIOS falls back to rom:// URI for disk-absent ROM stems");

    /* 1.6 Pillar 4: MasterLoader Core BIOS (zero-disk envelope & schema validation) */
    io_fb_loader_t loader;
    TEST_ASSERT(io_fallback_loader_load(&loader, "config", "rom://io/loader_contracts.json",
                                        s_test_scratch, sizeof(s_test_scratch)),
                "MasterLoader BIOS loads 5-block ROM envelope with zero disk I/O");
    TEST_ASSERT(loader.status == IO_FB_LOADER_OK, "Loader status reports IO_FB_LOADER_OK");
    TEST_ASSERT(loader.header.is_5block, "Loader recognized 5-block metadata envelope");
    TEST_ASSERT(loader.header.loaded_from_rom, "Loader marked loaded_from_rom=true");
    TEST_ASSERT(io_fallback_loader_validate_schema(&loader, "tenlux.data.io_loader_contracts.v1"),
                "MasterLoader BIOS validates schema_id='tenlux.data.io_loader_contracts.v1'");

    char ver_str[32];
    TEST_ASSERT(io_fallback_loader_get_string(&loader, "expected_major_minor_version",
                                              ver_str, sizeof(ver_str)) &&
                strcmp(ver_str, "0.1") == 0,
                "MasterLoader BIOS queries scoped string inside block_3_body cleanly");

    /* 1.8 Rev 5: Expanded 8-ROM vault, policy profiles, VFS 36-dir + asset roots hydration & atomic write */
    TEST_ASSERT(io_fallback_rom_count() == 8U,
                "ROM vault embeds all 8 engine/config/io JSON manifests and presets");
    {
        io_fallback_config_t p_floppy;
        io_fallback_config_t p_strict;
        io_fallback_config_t p_dev;
        TEST_ASSERT(io_fallback_load_rom_config("floppy_safe", &p_floppy) &&
                    !p_floppy.loaded_from_data && p_floppy.read_only_mode &&
                    !p_floppy.atomic_write && !p_floppy.flush_on_write &&
                    p_floppy.posix_dir_mode == 0555U && p_floppy.block_path_traversal,
                    "floppy_safe ROM profile hydrates read-only atomic policy cleanly");
        TEST_ASSERT(io_fallback_load_rom_config("strict_audit", &p_strict) &&
                    !p_strict.loaded_from_data && p_strict.reject_on_truncation &&
                    p_strict.strict_utf8_escapes && p_strict.reject_duplicate_keys &&
                    p_strict.probe_await_on_corrupt,
                    "strict_audit ROM profile hydrates forensic schema guards cleanly");
        TEST_ASSERT(io_fallback_load_rom_config("dev_hotreload", &p_dev) &&
                    !p_dev.loaded_from_data && p_dev.hot_reload_enabled &&
                    p_dev.allow_mod_mounts && p_dev.posix_dir_mode == 0755U,
                    "dev_hotreload ROM profile hydrates hot-reload & POSIX mode cleanly");
    }
    {
        const io_fb_vfs_routes_t *vfs = io_fallback_get_vfs_routes();
        io_fb_loader_contract_t lc;
        TEST_ASSERT(vfs != NULL && !vfs->loaded_from_data &&
                    (vfs->domain_search_count >= 1U) &&
                    (vfs->domain_search_count <= IO_FB_MAX_VFS_DOMAIN_DIRS) &&
                    vfs->asset_root_count >= 4U &&
                    strcmp(vfs->diag_log_prefix, "diagnostics/telemetry/") == 0 &&
                    strcmp(vfs->env_override_var, "TL_DATA_ROOT") == 0,
                    "Booted BIOS hydrates bounded VFS domain paths, asset roots, aliases & TL_DATA_ROOT from ROM");
        TEST_ASSERT(io_fallback_get_loader_contract(&lc) && !lc.loaded_from_data &&
                    strcmp(lc.expected_major_minor_version, "0.1") == 0 &&
                    lc.require_5block_envelope,
                    "Booted BIOS hydrates MasterLoader contract schema rules from ROM");
    }
    {
        static const char s_atom_payload[] = "{\"ok\":true}";
        char read_back[64];
        size_t rb_len = 0U;
        TEST_ASSERT(io_fallback_fs_write_with_flags("build/test_io_fb_atomic.tmp",
                                                    s_atom_payload,
                                                    sizeof(s_atom_payload) - 1U,
                                                    0755U, true, true, true),
                    "io_fallback_fs_write_with_flags performs atomic temp-rename write cleanly");
        TEST_ASSERT(io_fallback_fs_read_bounded("build/test_io_fb_atomic.tmp",
                                                read_back, sizeof(read_back), &rb_len) &&
                    rb_len == sizeof(s_atom_payload) - 1U &&
                    strcmp(read_back, s_atom_payload) == 0,
                    "Atomic BIOS write persists exact bytes verified via bounded read");
        (void)remove("build/test_io_fb_atomic.tmp");
    }

    io_fallback_teardown();
    printf("  [PASS-1] Section 1 passed: ROM Vault & all 4 BIOS fallback pillars verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (FAULT TOLERANCE, SYNTAX ERRORS & FLOPPY READ-ONLY LATCH) ]
--------------------------------------------------------------------------------
*/

static void test_io_fb_never_fail(void)
{
    printf("  [FAIL-2] Running Section 2: Never Fail (Fault Tolerance & Floppy RO Latch)...\n");

    io_fallback_reset_for_testing();

    /* 2.1 NULL and zero-capacity resilience */
    io_fallback_config_t cfg;
    char tiny_buf[8];
    size_t out_len = 99U;
    io_fb_loader_t bad_ldr;
    io_fb_file_info_t info;

    TEST_ASSERT(!io_fallback_get_active_config(NULL), "get_active_config(NULL) returns false");
    TEST_ASSERT(!io_fallback_execute_inspect(NULL), "execute_inspect(NULL) returns false");
    TEST_ASSERT(!io_fallback_execute_dump_card(NULL, tiny_buf, sizeof(tiny_buf)),
                "execute_dump_card rejects undersized buffer cleanly");
    TEST_ASSERT(!io_fallback_parse_policy_json(NULL, 0U, &cfg),
                "parse_policy_json(NULL) rejects safely");
    TEST_ASSERT(!io_fallback_load_rom_config("nonexistent_io_profile", &cfg),
                "load_rom_config rejects unknown ROM profile");
    TEST_ASSERT(!io_fallback_fs_exists(NULL), "fs_exists(NULL) returns false");
    TEST_ASSERT(!io_fallback_fs_is_regular(NULL), "fs_is_regular(NULL) returns false");
    TEST_ASSERT(io_fallback_fs_get_size(NULL) == -1L, "fs_get_size(NULL) returns -1");
    TEST_ASSERT(!io_fallback_fs_inspect(NULL, &info), "fs_inspect(NULL) returns false");
    TEST_ASSERT(!io_fallback_fs_read_bounded("rom://io/default.json", tiny_buf,
                                             sizeof(tiny_buf), &out_len) &&
                out_len == 0U && tiny_buf[sizeof(tiny_buf) - 1U] == '\0',
                "Truncated ROM read resets out_bytes=0 and null-terminates safely");
    TEST_ASSERT(!io_fallback_fs_write_bounded(NULL, "abc", 3U), "fs_write(NULL) returns false");
    TEST_ASSERT(!io_fallback_fs_ensure_dir(NULL), "fs_ensure_dir(NULL) returns false");
    TEST_ASSERT(io_fallback_path_set_root(NULL) &&
                strcmp(io_fallback_path_get_root(), IO_FB_DEFAULT_ROOT) == 0,
                "path_set_root(NULL) restores default VFS root cleanly");
    char oversized_root[IO_FB_MAX_ROOT_LEN + 16U];
    memset(oversized_root, 'x', sizeof(oversized_root) - 1U);
    oversized_root[sizeof(oversized_root) - 1U] = '\0';
    TEST_ASSERT(!io_fallback_path_set_root(oversized_root),
                "path_set_root rejects oversized root exceeding IO_FB_MAX_ROOT_LEN");
    TEST_ASSERT(!io_fallback_path_resolve("config", NULL, tiny_buf, sizeof(tiny_buf)),
                "path_resolve(NULL rel) returns false");

    /* 2.2 Malformed JSON and missing block_3_body rejection */
    const char *no_body_json = "{\"block_1_metadata\":{\"schema_id\":\"tenlux.test.v1\"}}";
    TEST_ASSERT(!io_fallback_loader_from_memory(&bad_ldr, no_body_json, strlen(no_body_json)) &&
                bad_ldr.status == IO_FB_LOADER_ERR_BODY_MISSING,
                "MasterLoader BIOS flags missing block_3_body deterministically");

    const char *broken_ladder =
        "{\"block_3_body\":{\"small_bytes\":9000,\"medium_bytes\":1024,\"large_bytes\":512}}";
    TEST_ASSERT(!io_fallback_parse_policy_json(broken_ladder, strlen(broken_ladder), &cfg),
                "Policy parser rejects inverted buffer ladder small > medium > large");

    /* 2.3 Floppy Safe-Mode read-only disk lock enforcement */
    const char *ro_file = "build/unit_io_fb_ro_must_not_exist.tmp";
    const char *ro_dir = "build/unit_io_fb_ro_dir_must_not_exist/sub";
    test_io_fb_cleanup_file(ro_file);

    TEST_ASSERT(io_fallback_boot_floppy(), "Booted Floppy Read-Only Safe Mode cleanly");
    TEST_ASSERT(io_fallback_is_disk_read_only(), "Floppy Safe Mode enables disk_read_only latch");
    TEST_ASSERT(!io_fallback_fs_write_bounded(ro_file, "blocked", 7U),
                "Floppy Safe Mode blocks disk file creation safely");
    TEST_ASSERT(!io_fallback_fs_ensure_dir(ro_dir),
                "Floppy Safe Mode blocks directory creation safely");
    TEST_ASSERT(!io_fallback_fs_exists(ro_file),
                "Blocked file was never created on disk");

    io_fallback_stats_t ro_stats;
    TEST_ASSERT(io_fallback_get_stats(&ro_stats) &&
                ro_stats.disk_read_only &&
                ro_stats.active_sink == IO_FB_SINK_FLOPPY_RO &&
                ro_stats.read_only_blocks >= 2U &&
                !ro_stats.loaded_from_data,
                "Stats accurately count read_only_blocks under IO_FB_SINK_FLOPPY_RO");

    /* Toggling latch off restores normal write behavior before teardown */
    io_fallback_set_read_only(false);
    TEST_ASSERT(!io_fallback_is_disk_read_only(), "Clearing RO latch restores writable status");
    io_fallback_teardown();

    printf("  [FAIL-2] Section 2 passed: Fault resilience & Floppy Safe-Mode locks verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTIONS & GUARANTEES (PROVENANCE, QUARANTINE & BOUNDS) ]
--------------------------------------------------------------------------------
*/

static void test_io_fb_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertions & Guarantees (Provenance & Bounds)...\n");

    io_fallback_reset_for_testing();
    TEST_ASSERT(io_fallback_boot(NULL), "Armed BIOS for axiom verification");

    /* 3.1 Provenance Invariant: loaded_from_data is ALWAYS false in BIOS/ROM */
    io_fallback_config_t def_cfg = io_fallback_get_default_config();
    io_fallback_config_t flp_cfg = io_fallback_get_floppy_config();
    io_fallback_config_t rom_cfg;
    io_fallback_config_t act_cfg;
    io_fallback_stats_t stats;

    TEST_ASSERT(io_fallback_load_rom_config("default", &rom_cfg), "Loaded default ROM config");
    TEST_ASSERT(io_fallback_get_active_config(&act_cfg), "Queried live active config");
    TEST_ASSERT(io_fallback_get_stats(&stats), "Queried BIOS stats");

    TEST_ASSERT(!def_cfg.loaded_from_data, "Default template keeps loaded_from_data=false");
    TEST_ASSERT(!flp_cfg.loaded_from_data, "Floppy template keeps loaded_from_data=false");
    TEST_ASSERT(!rom_cfg.loaded_from_data, "ROM-loaded config keeps loaded_from_data=false");
    TEST_ASSERT(!act_cfg.loaded_from_data, "Active BIOS config keeps loaded_from_data=false");
    TEST_ASSERT(!stats.loaded_from_data, "BIOS stats keep loaded_from_data=false");

    /* 3.2 Manifest & Quarantine Contracts */
    TEST_ASSERT(io_fallback_manifest.alloc_policy == ALLOC_ZERO_HEAP,
                "io_fallback_manifest mandates ALLOC_ZERO_HEAP");
    TEST_ASSERT(strcmp(io_fallback_manifest.module_id, IO_FALLBACK_MODULE_ID) == 0,
                "Manifest module_id matches IO_FALLBACK_MODULE_ID ('io.fallback_bios')");

    /* 3.3 Numeric bounds & buffer ladder enforcement */
    io_fallback_config_t corrupt = def_cfg;
    corrupt.small_bytes = 0U;
    TEST_ASSERT(!io_fallback_validate_policy_contract(&corrupt),
                "Policy validator rejects small_bytes == 0");

    corrupt = def_cfg;
    corrupt.max_config_bytes = IO_FB_MAX_CONFIG_BYTES + 1U;
    TEST_ASSERT(!io_fallback_validate_policy_contract(&corrupt),
                "Policy validator rejects max_config_bytes > 65536");

    corrupt = def_cfg;
    corrupt.path_max_bytes = 32U;
    TEST_ASSERT(!io_fallback_validate_policy_contract(&corrupt),
                "Policy validator rejects undersized path_max_bytes < 64");

    corrupt = def_cfg;
    corrupt.profile_name[0] = '\0';
    TEST_ASSERT(!io_fallback_validate_policy_contract(&corrupt),
                "Policy validator rejects empty profile_name");

    /* 3.4 All compiled .rodata ROM envelopes verify cleanly */
    TEST_ASSERT(io_fallback_verify_rom_envelopes(),
                "Every embedded .rodata I/O ROM envelope validates cleanly");

    io_fallback_teardown();
    printf("  [AXIOM-3] Section 3 passed: Provenance, zero-heap, and ladder axioms verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (BOOT -> SAFE DOWNGRADE -> CARD -> DEBUGGER AWAIT) ]
--------------------------------------------------------------------------------
*/

static void test_io_fb_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Safe-Mode, Cards & Debugger AWAIT)...\n");

    /* 4.1 Full lifecycle boot -> safe downgrade -> dump card */
    io_fallback_reset_for_testing();
    TEST_ASSERT(!io_fallback_is_booted(), "BIOS starts offline cleanly after reset");
    TEST_ASSERT(io_fallback_boot(NULL), "Cold boot BIOS succeeds cleanly");
    TEST_ASSERT(io_fallback_downgrade_safe_mode("Simulated missing external config dir", true),
                "Safe-Mode downgrade latches read-only recovery cleanly");

    io_fallback_stats_t st;
    TEST_ASSERT(io_fallback_get_stats(&st) &&
                st.safe_downgrades >= 1U &&
                st.fault_count >= 1U &&
                st.disk_read_only &&
                st.active_sink == IO_FB_SINK_FLOPPY_RO,
                "Safe-Mode downgrade increments safe_downgrades, records fault & locks RO");

    io_fallback_config_t act;
    TEST_ASSERT(io_fallback_get_active_config(&act), "Fetched active downgraded config");

    char small_card[256];
    char full_card[768];
    TEST_ASSERT(!io_fallback_execute_dump_card(&act, small_card, sizeof(small_card)),
                "Dump card rejects undersized 256-byte buffer safely");
    TEST_ASSERT(io_fallback_execute_dump_card(&act, full_card, sizeof(full_card)) &&
                strstr(full_card, "TIER-0 BIOS I/O") != NULL &&
                strstr(full_card, "FLOPPY_READ_ONLY") != NULL,
                "Dump card renders 60-col Safe-Mode BIOS summary into 768-byte buffer");

    /* 4.2 Debugger Validation Probe -> Safe-Mode -> DEBUG_STEP_AWAIT handshake */
    debug_init(NULL);
    io_fallback_set_read_only(false);
    io_fallback_register_debug_probes();
    {
        cornerstone_test_probe_check_t rom_chk =
            cornerstone_test_check_probe("io.fallback.rom_integrity", IO_FALLBACK_MODULE_ID);
        cornerstone_test_probe_check_t pol_chk =
            cornerstone_test_check_probe("io.fallback.policy_bounds", IO_FALLBACK_MODULE_ID);
        TEST_ASSERT(rom_chk.found && rom_chk.passed && pol_chk.found && pol_chk.passed,
                    "io.fallback.* validation probes register cleanly and pass non-destructively");
    }

    TEST_ASSERT(debug_check_tripwires(NULL) == 0U && !debug_is_awaiting(),
                "Healthy IO BIOS probes pass cleanly with zero tripwires fired");

    TEST_ASSERT(DEBUG_TRIGGER_PROBE("io.fallback.policy_bounds",
                                    IO_FALLBACK_MODULE_ID,
                                    "Synthetic IO BIOS policy bounds fault"),
                "Triggering io.fallback.policy_bounds probe via DEBUG_TRIGGER_PROBE succeeds");
    TEST_ASSERT(debug_is_awaiting() &&
                debug_get_step_mode() == DEBUG_STEP_AWAIT &&
                debug_get_tripwire_status() == DEBUG_TRIPWIRE_STATE_CORRUPT &&
                !debug_can_step(),
                "Probe escalation latches DEBUG_STEP_AWAIT & pauses stepping");

    TEST_ASSERT(debug_resolve_await(NULL, DEBUG_STEP_RUN, "IO BIOS bounds verified repaired") &&
                !debug_is_awaiting() &&
                debug_can_step() &&
                debug_get_step_mode() == DEBUG_STEP_RUN,
                "Clean BIOS state resolves DEBUG_STEP_AWAIT back to DEBUG_STEP_RUN");

    debug_shutdown();
    /* 4.5 Rev 5: Parent traversal rejection & cross-BIOS fault telemetry propagation */
    {
        io_fallback_stats_t trav_before;
        TEST_ASSERT(io_fallback_get_stats(&trav_before), "Captured baseline stats pre-traversal");
        char trav_path[IO_FB_PATH_MAX];
        trav_path[0] = 'X';
        TEST_ASSERT(!io_fallback_path_resolve("config", "../secret_escape.json",
                                              trav_path, sizeof(trav_path)) &&
                    trav_path[0] == '\0',
                    "io_fallback_path_resolve blocks parent traversal (..)");
        TEST_ASSERT(!io_fallback_fs_write_with_flags("../escaped_io_fb.txt", "no", 2U,
                                                     0755U, true, true, true),
                    "io_fallback_fs_write_with_flags rejects parent traversal when guarded");
        TEST_ASSERT(!io_fallback_fs_ensure_dir_mode("build/../escaped_dir", 0755U, true),
                    "io_fallback_fs_ensure_dir_mode rejects parent traversal when guarded");
        io_fallback_stats_t trav_stats;
        TEST_ASSERT(io_fallback_get_stats(&trav_stats) &&
                    trav_stats.fault_count >= trav_before.fault_count + 3U,
                    "Traversal denials increment BIOS fault telemetry cleanly");
    }
    {
        log_fallback_shutdown();
        debug_fallback_shutdown();
        TEST_ASSERT(log_fallback_boot(NULL) && debug_fallback_boot(NULL, 0U),
                    "Booted Tier-0 log & debug BIOS receivers for cross-BIOS wiring test");
        log_fallback_stats_t l_before;
        debug_fallback_stats_t d_before;
        TEST_ASSERT(log_fallback_get_stats(&l_before) && debug_fallback_get_stats(&d_before),
                    "Captured baseline Tier-0 BIOS stats");

        io_fallback_record_fault("fs_test", "write",
                                 "Simulated RO media write lock",
                                 IO_ERR_WRITE_BLOCKED_RO);

        log_fallback_stats_t l_after;
        debug_fallback_stats_t d_after;
        TEST_ASSERT(log_fallback_get_stats(&l_after) && l_after.total_recorded > l_before.total_recorded,
                    "io_fallback_record_fault forwards fault telemetry to log_fallback BIOS ring");
        TEST_ASSERT(debug_fallback_get_stats(&d_after) &&
                    d_after.fault_count > d_before.fault_count,
                    "io_fallback_record_fault forwards escalated fault to debug_fallback ring");

        log_entry_t drained[8];
        size_t got = log_fallback_drain_entries(drained, 8U);
        bool saw_io_cat = false;
        for (size_t i = 0U; i < got; i++) {
            if (drained[i].category == LOG_CAT_IO) {
                saw_io_cat = true;
            }
        }
        TEST_ASSERT(saw_io_cat, "Forwarded BIOS log entry carries LOG_CAT_IO category");
        debug_fallback_shutdown();
        log_fallback_shutdown();
    }

    io_fallback_teardown();

    printf("  [EMERGE-4] Section 4 passed: Lifecycle, Safe-Mode, cards & AWAIT verified.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fallback
================================================================================
*/

static void test_io_fb_preflight_audit(void)
{
    printf("[AUDIT] Running Pre-Flight Audit on Tier-0 BIOS I/O & ROM Vault...\n");
    diag_result_t res = io_fallback_preflight();
    TEST_ASSERT(res.passed && res.severity == SEVERITY_PASS,
                "io_fallback_preflight passed with SEVERITY_PASS");
    TEST_ASSERT(io_fallback_audit(), "io_fallback_audit returned true");
    printf("[AUDIT] io_fallback preflight: %s\n", res.details);
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
Stratum: Layer 4 (Verification Suite) | Machine: test.unit.io.fallback
================================================================================
*/

int main(void)
{
    cornerstone_test_begin("Tier-0 BIOS I/O Fallback Unit Suite", "UNIT");

    test_io_fb_preflight_audit();
    test_io_fb_always_pass();
    test_io_fb_never_fail();
    test_io_fb_assertions_and_guarantees();
    test_io_fb_emergence();

    cornerstone_test_summary("io_fallback Unit Suite", s_tests_run, s_tests_passed);
    return 0;
}
