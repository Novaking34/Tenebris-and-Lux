/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_debug_integrated.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 3
@date          2026-11-06
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Interactive State Debugger Layer Integrated Pipeline Verification Harness
@role          Verifies end-to-end integration between path_finder, master_loader,
               the 6 diagnostic debugger JSON assets in system/diagnostics/debugger/,
               and the interactive engine state debugger across the 4 Ontological Sections.
@invariants    1. Zero dynamic heap allocations in Layer 0 execution.
               2. All 6 debugger data files loaded and schema-validated.
               3. Four Ontological Testing Sections strictly executed.
               4. Universal 5-Block Lifecycle Protocol enforced.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / STATIC DATA & PRE-CONDITIONS
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.debug
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

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/string_utils.h"
#include "io/fs_utils.h"
#include "io/path_finder.h"
#include "io/json_utils.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"
#include "debug/debug_fallback.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

static debug_state_fields_t make_state(int turn, const char *hero, int hp, int lux, int ten, int stance, int sway)
{
    debug_state_fields_t s;
    memset(&s, 0, sizeof(s));
    s.turn = turn;
    str_safe_copy(s.hero_name, (hero != NULL) ? hero : "CustomBearer", sizeof(s.hero_name));
    s.hp = hp;
    s.max_hp = 500;
    s.mp = 100;
    s.tp = 50;
    s.shield_power = lux;
    s.sword_edge = ten;
    s.stance = stance;
    s.moral_sway = sway;
    s.location = 1;
    s.lux_barrio_turns = (lux > 50) ? 2 : 0;
    s.charm_turns = (ten > 50) ? 2 : 0;
    s.threat_hp = 100;
    s.foes_vanquished = 0;
    return s;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION INTEGRATED TEST SUITES
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.debug
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL PIPELINE SYNCHRONIZATION) ]
--------------------------------------------------------------------------------
*/

static void test_debug_integrated_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Synchronized Ingestion & Profiles)...\n");

    log_init(NULL);

    /* 1.1 Ingest debugger manifest via path_finder & master_loader */
    char manifest_path[PATH_FINDER_MAX_PATH];
    bool resolved = path_finder_resolve("system", "diagnostics/debugger/manifest.json",
                                       manifest_path, sizeof(manifest_path));
    TEST_ASSERT(resolved, "Resolved debugger manifest path");

    static char buf[16384];
    master_loader_t loader;
    bool loaded = master_loader_load_from_path(&loader, manifest_path, buf, sizeof(buf));
    TEST_ASSERT(loaded, "Master loader ingested debugger manifest.json");
    TEST_ASSERT(master_loader_validate_schema(&loader, "tenlux.data.debug_menu.v1"),
                "Manifest schema matches tenlux.data.debug_menu.v1");

    char active_profile[32];
    TEST_ASSERT(master_loader_get_string(&loader, NULL, "active_profile", active_profile, sizeof(active_profile)),
                "Active profile key extracted");
    TEST_ASSERT(strcmp(active_profile, "default") == 0, "Active profile is 'default'");

    /* 1.2 Ingest and validate all 3 profiles: default, strict, trace */
    const char *profiles[3] = { "default", "strict", "trace" };
    for (int i = 0; i < 3; i++) {
        debug_config_t cfg;
        bool prof_ok = debug_load_config_profile(profiles[i], &cfg);
        TEST_ASSERT(prof_ok, "Loaded profile successfully");
        TEST_ASSERT(cfg.loaded_from_data == true, "Profile provenance is data-driven");
        TEST_ASSERT(strcmp(cfg.profile_name, profiles[i]) == 0, "Profile name matches");
        TEST_ASSERT(cfg.max_snapshots == 64, "Snapshot capacity invariant == 64");
    }

    /* 1.3 Validate Catalogs: snapshot_policy.json & inspection_cards.json */
    master_loader_t policy_loader;
    static char policy_buf[16384];
    bool policy_ok = master_loader_load(&policy_loader, "system",
                                        "diagnostics/debugger/snapshot_policy.json",
                                        policy_buf, sizeof(policy_buf));
    TEST_ASSERT(policy_ok, "Loaded snapshot_policy.json");
    TEST_ASSERT(master_loader_validate_schema(&policy_loader, "tenlux.data.snapshot_policy.v1"),
                "Schema is tenlux.data.snapshot_policy.v1");

    size_t ring_cap = 0;
    master_loader_get_size_t(&policy_loader, "policy", "ring_buffer_capacity", &ring_cap);
    TEST_ASSERT(ring_cap == 64, "Policy ring_buffer_capacity == 64");

    master_loader_t card_loader;
    static char card_json_buf[16384];
    bool card_load_ok = master_loader_load(&card_loader, "system",
                                          "diagnostics/debugger/inspection_cards.json",
                                          card_json_buf, sizeof(card_json_buf));
    TEST_ASSERT(card_load_ok, "Loaded inspection_cards.json");
    TEST_ASSERT(master_loader_validate_schema(&card_loader, "tenlux.data.inspection_cards.v1"),
                "Schema is tenlux.data.inspection_cards.v1");

    /* 1.4 Boot debugger under 'default' profile and perform end-to-end battle recording */
    debug_config_t default_cfg;
    debug_load_config_profile("default", &default_cfg);
    TEST_ASSERT(debug_init(&default_cfg), "debug_init with default_cfg");
    TEST_ASSERT(debug_is_data_driven(), "Debugger data-driven");

    debug_state_fields_t turn1 = make_state(1, "Casatti", 450, 80, 80, 0, 0);
    uint64_t id1 = debug_record_snapshot(&turn1, "stance", "warding");
    TEST_ASSERT(id1 == 1, "Snapshot 1 recorded");

    debug_state_fields_t turn2 = make_state(2, "Casatti", 400, 95, 70, 1, 10);
    uint64_t id2 = debug_record_snapshot(&turn2, "attack", "cuttatio");
    TEST_ASSERT(id2 == 2, "Snapshot 2 recorded");

    /* Card formatting pipeline */
    char card_buf[1024];
    TEST_ASSERT(debug_execute_dump_card("CARD_HERO_VITALS", &turn2, card_buf, sizeof(card_buf)),
                "Dump hero vitals card");
    TEST_ASSERT(strstr(card_buf, "Casatti") != NULL, "Hero name in card output");

    debug_shutdown();
    printf("  [PASS-1] Section 1 passed: All nominal integrated pipeline checks verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: FAILING TESTS (FAULT-TOLERANT INTEGRATION TRIPWIRES) ]
--------------------------------------------------------------------------------
*/

static void test_debug_integrated_failing(void)
{
    printf("  [FAIL-2] Running Section 2: Failing Tests (Fault-Tolerant Fallback)...\n");

    /* 2.1 Non-existent profile gracefully falls back to monolith config */
    debug_config_t missing_cfg;
    bool fallback_triggered = debug_load_config_profile("non_existent_profile_xyz", &missing_cfg);
    TEST_ASSERT(fallback_triggered, "Gracefully handles missing profile without failing");
    TEST_ASSERT(!missing_cfg.loaded_from_data, "Fallback profile reports loaded_from_data false");
    TEST_ASSERT(strcmp(missing_cfg.profile_name, "monolith_fallback") == 0,
                "Profile name is monolith_fallback");
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_SETTING_MISSING,
                "Tripwire status latched DEBUG_TRIPWIRE_SETTING_MISSING");
    debug_reset_tripwire();

    /* 2.2 Corrupt manifest path falls back to monolith config */
    debug_config_t corrupt_cfg;
    bool mf_ok = debug_load_config_from_manifest("non_existent_manifest.json", &corrupt_cfg);
    TEST_ASSERT(!mf_ok, "Manifest loader reports false for missing manifest");
    TEST_ASSERT(!corrupt_cfg.loaded_from_data, "Fallback config loaded");
    TEST_ASSERT(debug_get_tripwire_status() == DEBUG_TRIPWIRE_MANIFEST_MISSING,
                "Tripwire status latched DEBUG_TRIPWIRE_MANIFEST_MISSING");
    debug_reset_tripwire();

    printf("  [FAIL-2] Section 2 passed: All fault-tolerant integrated tripwires verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (CONTRACTUAL INTEGRATION INVARIANTS) ]
--------------------------------------------------------------------------------
*/

static void test_debug_integrated_assertions_and_guarantees(void)
{
    printf("  [AXIOM-3] Running Section 3: Assertion & Guarantee (Profile Behavior Contracts)...\n");

    /* Axiom 1: 'strict' profile breaks on error and stance changes */
    debug_config_t strict_cfg;
    TEST_ASSERT(debug_load_config_profile("strict", &strict_cfg), "Load strict profile");
    TEST_ASSERT(strict_cfg.break_on_error == true, "Strict profile break_on_error == true");
    TEST_ASSERT(strict_cfg.break_on_stance_change == true, "Strict profile break_on_stance_change == true");

    debug_init(&strict_cfg);
    TEST_ASSERT(strcmp(debug_get_active_profile_name(), "strict") == 0, "Active profile is strict");
    debug_shutdown();

    /* Axiom 2: 'trace' profile enables deep trace channels ("all") */
    debug_config_t trace_cfg;
    TEST_ASSERT(debug_load_config_profile("trace", &trace_cfg), "Load trace profile");
    TEST_ASSERT(strcmp(trace_cfg.trace_channels, "all") == 0, "Trace profile trace_channels == 'all'");

    debug_init(&trace_cfg);
    TEST_ASSERT(strcmp(debug_get_active_profile_name(), "trace") == 0, "Active profile is trace");
    debug_shutdown();

    /* Axiom 3: State hash determinism across profile reboots */
    debug_state_fields_t sample = make_state(10, "Elijah", 400, 75, 75, 1, 0);
    uint32_t h1 = debug_compute_state_hash(&sample);
    uint32_t h2 = debug_compute_state_hash(&sample);
    TEST_ASSERT(h1 == h2, "Hash is bitwise identical across calls");

    printf("  [AXIOM-3] Section 3 passed: All profile behavioral contracts verified.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE (COMPOSITE PIPELINE SIMULATION & REWIND) ]
--------------------------------------------------------------------------------
*/

static void test_debug_integrated_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (End-to-End Campaign Replay & Rewind)...\n");

    debug_init(NULL);

    /* Simulate 50 turns of tactical campaign replay */
    for (int t = 1; t <= 50; t++) {
        int hp = 500 - (t * 4);
        int lux = (t * 5) % 100;
        int ten = (t * 3) % 100;
        int stance = t % 4;
        int sway = (t * 2) - 50;
        debug_state_fields_t turn_state = make_state(t, "Katrina", hp, lux, ten, stance, sway);
        char cmd[32];
        snprintf(cmd, sizeof(cmd), "action_%d", t);
        uint64_t id = debug_record_snapshot(&turn_state, cmd, "tactical");
        TEST_ASSERT(id == (uint64_t)t, "Monotonic snapshot ID");
    }

    TEST_ASSERT(debug_get_snapshot_count() == 50, "Total snapshots recorded is 50");

    /* Rewind to turn 25 */
    debug_state_fields_t restored;
    TEST_ASSERT(debug_rewind_to_snapshot(24, &restored), "Rewind to turn 25 (index 24)");
    TEST_ASSERT(restored.turn == 25, "Restored state turn is 25");
    TEST_ASSERT(restored.hp == (500 - (25 * 4)), "Restored HP matches turn 25 formula");
    TEST_ASSERT(strcmp(restored.hero_name, "Katrina") == 0, "Hero name Katrina preserved");

    /* Verify diff between turn 25 and turn 50 */
    debug_snapshot_t s25, s50;
    TEST_ASSERT(debug_get_snapshot(24, &s25), "Get snap 25");
    TEST_ASSERT(debug_get_snapshot(49, &s50), "Get snap 50");

    char diff[1024];
    size_t dlen = debug_diff_snapshots(&s25, &s50, diff, sizeof(diff));
    TEST_ASSERT(dlen > 0, "Diff generated");
    TEST_ASSERT(strstr(diff, "400 -> 300") != NULL, "HP diff 400 -> 300 present");

    debug_shutdown();
    printf("  [EMERGE-4] Section 4 passed: End-to-end campaign replay and rewind verified.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER HANDSHAKE
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.debug
================================================================================
*/

int main(void)
{
    printf("================================================================================\n");
    printf("   Tenebris and Lux - Interactive State Debugger Integrated Test Suite          \n");
    printf("================================================================================\n");

    test_debug_integrated_always_pass();
    test_debug_integrated_failing();
    test_debug_integrated_assertions_and_guarantees();
    test_debug_integrated_emergence();

    printf("================================================================================\n");
    printf("VERDICT: ALL DEBUGGER INTEGRATED INVARIANTS SATISFIED (4/4 SECTIONS PASS)\n");
    printf("================================================================================\n");

    return 0;
}
