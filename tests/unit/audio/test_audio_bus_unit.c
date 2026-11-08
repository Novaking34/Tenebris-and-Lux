/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_audio_bus_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Unit test harness for audio event bus, channels, and ring buffer.
@invariants    1. Zero dynamic heap allocations in tests.
               2. All structs and enums must have explicit non-anonymous tags.
               3. 4-section ontological verification structure.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include "audio/audio_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion '%s' failed: %s\n", \
                    __FILE__, __LINE__, #cond, (msg)); \
            exit(1); \
        } \
    } while (0)

/*
================================================================================
BLOCK 3: BODY BLOCK / 4-SECTION ONTOLOGICAL TEST PROCEDURES
================================================================================
*/

/* --- SECTION 1: ALWAYS PASS (Baseline Defaults & States) --- */
static void test_audio_bus_always_pass(void)
{
    printf("  [TRUTH-1] Running Section 1: Truth (Always Pass)...\n");

    audio_bus_init(NULL);
    TEST_ASSERT(audio_bus_is_initialized(), "bus must be initialized");
    TEST_ASSERT(audio_bus_pending_count() == 0, "queue must start empty");
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_MASTER) == 1.0f,
                "master volume default 1.0f");
    TEST_ASSERT(!audio_bus_is_channel_muted(AUDIO_CHANNEL_MASTER),
                "master not muted");

    audio_event_t ev;
    TEST_ASSERT(!audio_bus_poll_event(&ev), "poll on empty queue returns false");
    TEST_ASSERT(!audio_bus_peek_event(&ev), "peek on empty queue returns false");

    printf("  [TRUTH-1] Section 1 passed: Baseline defaults valid.\n");
}

/* --- SECTION 2: NEVER FAIL (Fault Tolerance & Boundary Guards) --- */
static void test_audio_bus_never_fail(void)
{
    printf("  [SAFETY-2] Running Section 2: Safety (Never Fail)...\n");

    /* Null guards */
    TEST_ASSERT(!audio_bus_post_event(NULL), "null event posting rejected");
    TEST_ASSERT(!audio_bus_post_sfx(NULL, 1.0f, 0.0f), "null SFX cue rejected");
    TEST_ASSERT(!audio_bus_post_bgm(NULL, 1.0f), "null BGM track rejected");
    TEST_ASSERT(!audio_bus_post_ambience(NULL, 1.0f), "null ambience cue rejected");
    TEST_ASSERT(!audio_bus_post_stinger(NULL, 1.0f), "null stinger rejected");

    audio_bus_get_stats(NULL); /* Should not crash */

    /* Out-of-bounds channel requests */
    audio_bus_set_channel_volume((audio_channel_t)99, 0.5f);
    TEST_ASSERT(audio_bus_get_channel_volume((audio_channel_t)99) == 0.0f,
                "invalid channel volume query safe 0.0");
    audio_bus_set_channel_muted((audio_channel_t)99, true);
    TEST_ASSERT(!audio_bus_is_channel_muted((audio_channel_t)99),
                "invalid channel mute query safe false");

    /* Volume clamping */
    audio_bus_set_channel_volume(AUDIO_CHANNEL_BGM, -10.0f);
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_BGM) == 0.0f,
                "volume clamped at 0.0 minimum");
    audio_bus_set_channel_volume(AUDIO_CHANNEL_BGM, 50.0f);
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_BGM) == 1.0f,
                "volume clamped at 1.0 maximum");

    /* Queue capacity overflow */
    audio_bus_clear_events();
    audio_bus_reset_stats();

    for (uint32_t i = 0; i < AUDIO_BUS_QUEUE_CAPACITY; ++i) {
        TEST_ASSERT(audio_bus_post_sfx("sfx_step", 0.5f, 0.0f), "fill queue slot");
    }
    TEST_ASSERT(audio_bus_pending_count() == AUDIO_BUS_QUEUE_CAPACITY, "queue full");

    /* Overflow attempt */
    TEST_ASSERT(!audio_bus_post_sfx("sfx_overflow", 0.5f, 0.0f),
                "overflow event safely rejected");

    audio_bus_stats_t stats;
    audio_bus_get_stats(&stats);
    TEST_ASSERT(stats.dropped_events == 1, "dropped events counter increments");
    TEST_ASSERT(stats.queue_high_watermark == AUDIO_BUS_QUEUE_CAPACITY,
                "high watermark matches capacity");

    audio_bus_clear_events();
    TEST_ASSERT(audio_bus_pending_count() == 0, "queue cleared successfully");

    printf("  [SAFETY-2] Section 2 passed: Fault tolerance bounds verified.\n");
}

/* --- SECTION 3: AXIOMS & TRIPWIRES (Routing & Order) --- */
static void test_audio_bus_axioms_and_tripwires(void)
{
    printf("  [AXIOM-3] Running Section 3: Axioms (Routing & Order)...\n");

    audio_bus_init(NULL);

    /* 3.1 Post SFX, BGM, Ambience, Stinger */
    TEST_ASSERT(audio_bus_post_sfx("sfx_ward_lux_barrio", 0.9f, -0.5f), "post sfx");
    TEST_ASSERT(audio_bus_post_bgm("bgm_tactical_skirmish", 2.0f), "post bgm");
    TEST_ASSERT(audio_bus_post_ambience("amb_sacyra_meadow", 1.5f), "post ambience");
    TEST_ASSERT(audio_bus_post_stinger("stinger_victory_fanfare", 1.0f), "post stinger");

    TEST_ASSERT(audio_bus_pending_count() == 4, "pending count is 4");

    /* 3.2 Verify FIFO queue ordering */
    audio_event_t ev;

    /* First: SFX */
    TEST_ASSERT(audio_bus_peek_event(&ev), "peek sfx");
    TEST_ASSERT(ev.type == AUDIO_EVENT_PLAY_SFX, "first event is SFX");
    TEST_ASSERT(strcmp(ev.cue_id, "sfx_ward_lux_barrio") == 0, "cue id matches");
    TEST_ASSERT(ev.channel == AUDIO_CHANNEL_SFX_COMBAT, "channel matches");
    TEST_ASSERT(audio_bus_poll_event(&ev), "pop sfx");

    /* Second: BGM */
    TEST_ASSERT(audio_bus_poll_event(&ev), "pop bgm");
    TEST_ASSERT(ev.type == AUDIO_EVENT_CROSSFADE_BGM, "second event is BGM crossfade");
    TEST_ASSERT(strcmp(ev.cue_id, "bgm_tactical_skirmish") == 0, "track id matches");
    TEST_ASSERT(ev.channel == AUDIO_CHANNEL_BGM, "channel matches");

    /* Third: Ambience */
    TEST_ASSERT(audio_bus_poll_event(&ev), "pop ambience");
    TEST_ASSERT(ev.type == AUDIO_EVENT_CROSSFADE_AMBIENCE, "third event is Ambience crossfade");
    TEST_ASSERT(strcmp(ev.cue_id, "amb_sacyra_meadow") == 0, "ambience id matches");

    /* Fourth: Stinger */
    TEST_ASSERT(audio_bus_poll_event(&ev), "pop stinger");
    TEST_ASSERT(ev.type == AUDIO_EVENT_PLAY_STINGER, "fourth event is Stinger");
    TEST_ASSERT(strcmp(ev.cue_id, "stinger_victory_fanfare") == 0, "stinger id matches");

    TEST_ASSERT(audio_bus_pending_count() == 0, "queue drained");

    /* 3.3 Channel Mute toggles */
    audio_bus_set_channel_muted(AUDIO_CHANNEL_UI, true);
    TEST_ASSERT(audio_bus_is_channel_muted(AUDIO_CHANNEL_UI), "UI channel muted");
    audio_bus_set_channel_muted(AUDIO_CHANNEL_UI, false);
    TEST_ASSERT(!audio_bus_is_channel_muted(AUDIO_CHANNEL_UI), "UI channel unmuted");

    printf("  [AXIOM-3] Section 3 passed: Routing & ordering axioms hold.\n");
}

/* --- SECTION 4: EMERGENCE & SELF-AUDIT --- */
static void test_audio_bus_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Self-Audit)...\n");

    TEST_ASSERT(audio_bus_preflight_audit(), "audio_bus_preflight_audit passes");

    printf("  [EMERGE-4] Section 4 passed: Preflight audit clean.\n");
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING / ENTRY POINT BLOCK
================================================================================
*/

int main(void)
{
    printf("============================================================\n");
    printf("   Tenebris and Lux - Audio Event Bus Unit Test             \n");
    printf("============================================================\n");

    test_audio_bus_always_pass();
    test_audio_bus_never_fail();
    test_audio_bus_axioms_and_tripwires();
    test_audio_bus_emergence();

    printf("============================================================\n");
    printf("   All Audio Event Bus Unit Tests Passed (4/4)              \n");
    printf("============================================================\n");
    return 0;
}

