/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_audio_functional.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Functional Verification: 7-Bus Audio Event Subsystem
@role          Isolates and validates the functional contracts, mixing buses,
               ducking attenuation, volume bounds, and event queue mechanics
               across all 4 ontological testing tiers.
@invariants    1. Zero dynamic heap allocations in audio cue routing (ALLOC_ZERO_HEAP).
               2. Volume percentages strictly bounded between 0.0f and 1.0f.
               3. Audio strictly follows simulation events (never drives simulation).
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Verifies the audio bus event dispatch pipeline, multi-channel concurrency,
  volume bounds, cue FIFO ordering, and fallback profile isolation.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.functional.audio
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "audio/audio_bus.h"
#include "audio/audio_fallback.h"
#include "core/string_utils.h"

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
BLOCK 3: BODY BLOCK / 4-SECTION FUNCTIONAL TEST SUITES
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL AUDIO BASELINE) ]
--------------------------------------------------------------------------------
*/
static void test_audio_func_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Audio Contracts)...\n");

    /* 1.1 Audio bus initialization */
    audio_bus_init(NULL);
    TEST_ASSERT(audio_bus_is_initialized(), "Audio bus initialized");

    /* 1.2 Channel volume setters and getters for all 7 channels */
    for (int ch = 0; ch < AUDIO_CHANNEL_COUNT; ch++) {
        float test_vol = 0.1f * (float)(ch + 1);
        audio_bus_set_channel_volume((audio_channel_t)ch, test_vol);
        float get_vol = audio_bus_get_channel_volume((audio_channel_t)ch);
        TEST_ASSERT(get_vol > test_vol - 0.01f && get_vol < test_vol + 0.01f,
                    "Retrieved channel volume matches set volume");
    }

    /* 1.3 Event posting and FIFO polling across all channels */
    for (int ch = 0; ch < AUDIO_CHANNEL_COUNT; ch++) {
        audio_event_t ev;
        memset(&ev, 0, sizeof(ev));
        ev.type = AUDIO_EVENT_PLAY_SFX;
        ev.channel = (audio_channel_t)ch;
        ev.volume = 0.85f;
        ev.pitch = 1.0f;
        snprintf(ev.cue_id, sizeof(ev.cue_id), "cue_ch_%d", ch);

        bool posted = audio_bus_post_event(&ev);
        TEST_ASSERT(posted, "Audio event posted successfully");

        audio_event_t out_ev;
        bool polled = audio_bus_poll_event(&out_ev);
        TEST_ASSERT(polled, "Audio event polled successfully");
        TEST_ASSERT(out_ev.channel == (audio_channel_t)ch, "Polled event channel matches");
        char expected[AUDIO_MAX_CUE_ID_LEN];
        snprintf(expected, sizeof(expected), "cue_ch_%d", ch);
        TEST_ASSERT(strcmp(out_ev.cue_id, expected) == 0, "Polled event cue ID matches");
    }

    /* 1.4 Channel volume attenuation & muting */
    audio_bus_set_channel_volume(AUDIO_CHANNEL_BGM, 0.30f);
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_BGM) == 0.30f, "Channel volume adjusted");
    audio_bus_set_channel_muted(AUDIO_CHANNEL_BGM, true);
    TEST_ASSERT(audio_bus_is_channel_muted(AUDIO_CHANNEL_BGM), "Channel muted");
    audio_bus_set_channel_muted(AUDIO_CHANNEL_BGM, false);
    TEST_ASSERT(!audio_bus_is_channel_muted(AUDIO_CHANNEL_BGM), "Channel unmuted");

    /* 1.5 Fallback audio profile inspection */
    audio_fallback_profile_t fb;
    audio_fallback_get_profile(&fb);
    TEST_ASSERT(!fb.loaded_from_data, "Fallback profile reports loaded_from_data false");
    TEST_ASSERT(fb.config.master_volume == 1.0f, "Fallback master volume is 1.0f");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_audio_func_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    audio_bus_init(NULL);

    /* 2.1 NULL event pointer */
    TEST_ASSERT(!audio_bus_post_event(NULL), "Posting NULL event returns false");
    TEST_ASSERT(!audio_bus_poll_event(NULL), "Polling to NULL pointer returns false");

    /* 2.2 Invalid channel out-of-bounds */
    audio_event_t bad_ch_ev;
    memset(&bad_ch_ev, 0, sizeof(bad_ch_ev));
    bad_ch_ev.type = AUDIO_EVENT_PLAY_SFX;
    bad_ch_ev.channel = (audio_channel_t)999;
    str_safe_copy(bad_ch_ev.cue_id, "bad", sizeof(bad_ch_ev.cue_id));
    TEST_ASSERT(!audio_bus_post_event(&bad_ch_ev), "Posting to invalid channel returns false");

    audio_bus_set_channel_volume((audio_channel_t)999, 0.5f);
    TEST_ASSERT(audio_bus_get_channel_volume((audio_channel_t)999) == 0.0f, "Getting volume on invalid channel returns 0.0f");

    /* 2.3 Volume clamping on out-of-bounds inputs */
    audio_bus_set_channel_volume(AUDIO_CHANNEL_MASTER, -2.0f);
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_MASTER) == 0.0f, "Negative volume clamped to 0.0f");

    audio_bus_set_channel_volume(AUDIO_CHANNEL_MASTER, 10.0f);
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_MASTER) == 1.0f, "Excessive volume clamped to 1.0f");

    /* 2.4 Drain empty queue safely */
    audio_bus_init(NULL);
    audio_event_t dummy;
    TEST_ASSERT(!audio_bus_poll_event(&dummy), "Polling empty queue safely returns false");

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (ACOUSTIC INVARIANTS) ]
--------------------------------------------------------------------------------
*/
static void test_audio_func_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Acoustic Invariants)...\n");

    audio_bus_init(NULL);

    /* 3.1 FIFO order preservation */
    for (int i = 0; i < 10; i++) {
        audio_event_t ev;
        memset(&ev, 0, sizeof(ev));
        ev.type = AUDIO_EVENT_PLAY_SFX;
        ev.channel = AUDIO_CHANNEL_SFX_COMBAT;
        ev.volume = 1.0f;
        snprintf(ev.cue_id, sizeof(ev.cue_id), "combat_hit_%d", i);
        audio_bus_post_event(&ev);
    }

    for (int i = 0; i < 10; i++) {
        audio_event_t out;
        bool ok = audio_bus_poll_event(&out);
        TEST_ASSERT(ok, "Event polled in sequence");
        char expected[AUDIO_MAX_CUE_ID_LEN];
        snprintf(expected, sizeof(expected), "combat_hit_%d", i);
        TEST_ASSERT(strcmp(out.cue_id, expected) == 0, "FIFO ordering strictly preserved");
    }

    /* 3.2 Subsystem audit verification */
    TEST_ASSERT(audio_bus_preflight_audit(), "audio_bus_preflight_audit passes unconditionally");
    TEST_ASSERT(audio_fallback_audit(), "audio_fallback_audit passes unconditionally");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (RAPID DISPATCH & WRAP-AROUND) ]
--------------------------------------------------------------------------------
*/
static void test_audio_func_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (Rapid Ring Wrap-Around)...\n");

    audio_bus_init(NULL);

    /* 4.1 Post and drain 10,000 events to force ring buffer wrap-around */
    for (int i = 0; i < 10000; i++) {
        audio_event_t ev;
        memset(&ev, 0, sizeof(ev));
        ev.type = AUDIO_EVENT_PLAY_SFX;
        ev.channel = (audio_channel_t)(i % AUDIO_CHANNEL_COUNT);
        ev.volume = 0.5f;
        snprintf(ev.cue_id, sizeof(ev.cue_id), "stress_%d", i);

        bool posted = audio_bus_post_event(&ev);
        TEST_ASSERT(posted, "Stress event posted");

        audio_event_t out;
        bool polled = audio_bus_poll_event(&out);
        TEST_ASSERT(polled, "Stress event polled");
        char expected[AUDIO_MAX_CUE_ID_LEN];
        snprintf(expected, sizeof(expected), "stress_%d", i);
        TEST_ASSERT(strcmp(out.cue_id, expected) == 0, "Wrapped event matched exact cue ID");
    }

    printf("  [PASS-4] Section 4 passed unconditionally (10,000 Events Wrapped).\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST ENTRY POINT
================================================================================
*/
int main(void)
{
    printf("============================================================\n");
    printf("   Functional Verification: 7-Bus Audio Event Subsystem\n");
    printf("============================================================\n");

    test_audio_func_always_pass();
    test_audio_func_never_fail();
    test_audio_func_assertions_and_guarantees();
    test_audio_func_emergence();

    printf("============================================================\n");
    printf("   All Audio Functional Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}
