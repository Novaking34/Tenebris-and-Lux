/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_utils.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Grand Integrated Engine Utilities & Audiovisual Harness
@role          Comprehensive cross-subsystem integration harness verifying
               the collective interlock of ALL Layer 0 Core Utilities and
               Layer 2 Audiovisual Viewport engines (Core, IO, Log, Debug,
               Audio, and Viewport Reel/Layout/Palette) working in harmony
               before higher-level simulation or gameplay logic runs.
@invariants    1. Zero dynamic heap allocations in Layer 0/Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. 100% pass across all utility subsystem self-audits.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Acts as the Grand Test of Engine Utilities. Proves the Big Machine's complete
  mechanical apparatus functions deterministically:
  - VFS path resolution & manifest ingestion
  - Telemetry logging ring buffer
  - State debugger & FNV-1a state hashing
  - Audio bus multi-channel event routing & ducking
  - Viewport Movie Reel 5-screen navigation & depth kinematics
  - High Fantasy Hope color palette & AABB layout bounding
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.utils
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"
#include "core/math_utils.h"
#include "core/geom_utils.h"
#include "core/string_utils.h"
#include "core/time_utils.h"
#include "core/clock_pacer.h"
#include "core/formula_eval.h"
#include "io/fs_utils.h"
#include "io/json_utils.h"
#include "io/path_finder.h"
#include "io/master_loader.h"
#include "log/log.h"
#include "debug/debug.h"
#include "audio/audio_bus.h"
#include "viewport/reel/reel.h"
#include "viewport/styling/gui_palette.h"
#include "viewport/hud/gui_layout.h"
#include "viewport/reel/gui_screens.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s (%s)\n", \
                __FILE__, __LINE__, #cond, msg); \
            exit(1); \
        } \
    } while (0)

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION INTEGRATED TEST SUITES
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL INTEGRATED ENGINE UTILITIES BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_integrated_utils_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Grand Integrated Engine Utilities Nominal Baseline...\n");

    /* 1.1 VFS & Manifest Loading Integration */
    char resolved[PATH_FINDER_MAX_PATH];
    bool found_manifest = path_finder_resolve(NULL, "manifest.json", resolved, sizeof(resolved));
    TEST_ASSERT(found_manifest, "Master manifest path resolved via path_finder");
    TEST_ASSERT(fs_file_exists(resolved), "Master manifest exists on disk");

    /* 1.2 Telemetry Logging Integration */
    log_init(NULL);
    LOG_INFO(LOG_CAT_LOG, "IntegratedUtils", 0.0f, "Engine utilities bootstrapping sequence initialized");

    /* 1.3 State Debugger Integration */
    debug_init(NULL);
    TEST_ASSERT(debug_is_active(), "Debugger initialized and active");

    /* 1.4 Audio Bus Integration */
    audio_bus_init(NULL);
    audio_bus_set_channel_volume(AUDIO_CHANNEL_MASTER, 1.0f);
    audio_bus_set_channel_volume(AUDIO_CHANNEL_BGM, 0.80f);

    audio_event_t bgm_cue;
    memset(&bgm_cue, 0, sizeof(bgm_cue));
    bgm_cue.type = AUDIO_EVENT_PLAY_BGM;
    bgm_cue.channel = AUDIO_CHANNEL_BGM;
    bgm_cue.volume = 0.80f;
    str_safe_copy(bgm_cue.cue_id, "sacyra_meadows_bgm", sizeof(bgm_cue.cue_id));
    bool bgm_ok = audio_bus_post_event(&bgm_cue);
    TEST_ASSERT(bgm_ok, "BGM audio event posted to audio bus");

    /* 1.5 Viewport Palette & High Fantasy Hope Themes Integration */
    gui_theme_catalog_t themes;
    gui_palette_init_catalog_fallback(&themes);
    TEST_ASSERT(strcmp(themes.active_theme_id, "high_fantasy_hope") == 0, "Default theme is high_fantasy_hope");

    gui_palette_t palette;
    gui_palette_init_default(&palette);
    TEST_ASSERT((palette.shield_gold & 0x00FFFFFF) == 0xF0CE78, "Palette active gold matches radiant hope");

    /* 1.6 Viewport Layout Integration (matching layout.json 860x620) */
    gui_layout_t layout;
    gui_layout_get_default(&layout);
    TEST_ASSERT(gui_layout_audit(&layout), "Layout audit passes for desktop presentation");

    /* 1.7 Movie Reel Kinematic Integration */
    reel_t reel;
    reel_init(&reel, 0, 5);
    TEST_ASSERT(reel_get_current_screen(&reel) == 0, "Movie reel initialized at Tactical view");

    /* Push reel to Status Dossier */
    bool push_ok = reel_push(&reel, 1, 0.25f);
    TEST_ASSERT(push_ok, "Movie reel advanced to screen 1 (Status)");

    /* Post stinger audio cue for screen change */
    audio_event_t ui_cue;
    memset(&ui_cue, 0, sizeof(ui_cue));
    ui_cue.type = AUDIO_EVENT_PLAY_SFX;
    ui_cue.channel = AUDIO_CHANNEL_UI;
    ui_cue.volume = 0.90f;
    str_safe_copy(ui_cue.cue_id, "reel_push_swish", sizeof(ui_cue.cue_id));
    audio_bus_post_event(&ui_cue);

    /* Poll audio bus event */
    audio_event_t polled_ev;
    bool polled = audio_bus_poll_event(&polled_ev);
    TEST_ASSERT(polled, "Polled event from audio bus");

    printf("  [PASS-1] Section 1 passed unconditionally (All Subsystems Interlocked).\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (CROSS-SUBSYSTEM FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_integrated_utils_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Cross-Subsystem Fault Hardening)...\n");

    /* 2.1 Audio bus volume under extreme parameters */
    audio_bus_set_channel_volume(AUDIO_CHANNEL_BGM, -0.5f);
    float vol = audio_bus_get_channel_volume(AUDIO_CHANNEL_BGM);
    TEST_ASSERT(vol >= 0.0f && vol <= 1.0f, "Volume clamps safely to [0.0, 1.0]");

    /* 2.2 Corrupt layout recovery */
    gui_layout_t layout;
    gui_layout_update(&layout, -500, -500);
    TEST_ASSERT(gui_layout_audit(&layout), "Layout auto-recovers to valid bounded state");

    /* 2.3 Shading arithmetic on invalid colors */
    unsigned long shaded = gui_palette_shade(0x000000, 2.5f);
    TEST_ASSERT((shaded & 0xFF000000) == 0xFF000000, "Alpha maintained on zero color shading");

    /* 2.4 Reel out-of-bounds protection */
    reel_t reel;
    reel_init(&reel, 0, 5);
    TEST_ASSERT(!reel_navigate(&reel, -1, 0.0f), "Negative navigation rejected");
    TEST_ASSERT(!reel_navigate(&reel, 999, 0.0f), "Excessive navigation rejected");

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (CROSS-ENGINE AXIOMATIC CONTRACTS) ]
--------------------------------------------------------------------------------
*/
static void test_integrated_utils_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Cross-Engine Invariants)...\n");

    /* 3.1 Pacing clock baseline */
    clock_pacer_t pacer;
    clock_pacer_init(&pacer, NULL);
    TEST_ASSERT(pacer.initialized && pacer.config.fixed_step_ms > 0.0,
                "Clock pacer initialized with positive fixed step");

    /* 3.2 Battle formula deterministic calculation */
    formula_eval_t f_eval;
    formula_eval_init(&f_eval, NULL);
    int net = formula_eval_calc_mitigation(&f_eval, 100, 25);
    TEST_ASSERT(net > 0 && net <= 100, "Physical damage formula computes deterministically");

    /* 3.3 All presentation panels remain disjoint during resize */
    gui_layout_t layout;
    gui_layout_update(&layout, 1280, 720);
    TEST_ASSERT(gui_layout_audit(&layout), "1280x720 layout maintains pairwise disjoint AABBs");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (GRAND MULTI-SUBSYSTEM PUMP) ]
--------------------------------------------------------------------------------
*/
static void test_integrated_utils_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (1,000 Integrated Ticks)...\n");

    /* Grand 1,000 tick integrated simulation loop:
       Clock Pacer -> Formula Eval -> Audio Dispatch -> Reel Step -> Shading */
    clock_pacer_t pacer;
    clock_pacer_init(&pacer, NULL);

    reel_t reel;
    reel_init(&reel, 0, 5);

    gui_theme_catalog_t themes;
    gui_palette_init_catalog_fallback(&themes);

    for (int tick = 0; tick < 1000; tick++) {
        /* 1. Accumulate simulated frame time */
        clock_pacer_begin_frame(&pacer, (uint64_t)(time_get_monotonic_us() * 1000.0));

        /* 2. Process audio events */
        if (tick % 10 == 0) {
            audio_event_t ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = AUDIO_EVENT_PLAY_SFX;
            ev.channel = AUDIO_CHANNEL_SFX_TACTICAL;
            ev.volume = 0.5f;
            snprintf(ev.cue_id, sizeof(ev.cue_id), "tactical_tick_%d", tick);
            audio_bus_post_event(&ev);
        }

        /* 3. Update movie reel transition */
        reel_update(&reel, 0.016f, 860);

        /* 4. Flush audio ring */
        audio_event_t out;
        while (audio_bus_poll_event(&out)) {
            /* drained */
        }
    }

    /* Verify all subsystem self-audits pass */
    TEST_ASSERT(gui_palette_audit(), "gui_palette_audit passes");
    TEST_ASSERT(gui_layout_audit(NULL), "gui_layout_audit passes");
    TEST_ASSERT(gui_screens_audit(), "gui_screens_audit passes");
    TEST_ASSERT(clock_pacer_audit(), "clock_pacer_audit passes");

    printf("  [PASS-4] Section 4 passed unconditionally (1,000 Integrated Ticks Verified).\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
================================================================================
*/
int main(void)
{
    printf("============================================================\n");
    printf("   Grand Integrated Engine Utilities & Audiovisual Harness\n");
    printf("   Subsystems: Core, IO, Log, Debug, Audio, Viewport Reel/HUD\n");
    printf("============================================================\n");

    test_integrated_utils_always_pass();
    test_integrated_utils_never_fail();
    test_integrated_utils_assertions_and_guarantees();
    test_integrated_utils_emergence();

    printf("============================================================\n");
    printf("   All Integrated Engine Utilities Tests Passed (100%% Green)\n");
    printf("============================================================\n");
    return 0;
}
