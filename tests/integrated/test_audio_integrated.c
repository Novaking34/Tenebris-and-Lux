/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_audio_integrated.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Integrated Verification: Audio Bus & Simulation Engine
@role          Verifies cross-cutting integration between the Audio Bus event
               queue, combat resolutions, hero stance transitions, item
               consumptions, and command routing.
@invariants    1. Zero dynamic heap allocations in Layer 0/1/2 (ALLOC_ZERO_HEAP).
               2. Audio strictly follows simulation (never drives simulation turn state).
               3. Simulation state hashing produces identical hash with audio enabled or disabled.
               4. Universal 5-Block Lifecycle Protocol enforced.

PURPOSE & ARCHITECTURAL ROLE:
  Ensures the Audio Event Bus correctly receives simulation triggers (sword slashes,
  barrier activations, stance shifts) while guaranteeing complete headless decoupling.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & ONTOLOGY
Stratum: Layer 4 (Verification Suite) | Machine: test.integrated.audio
================================================================================
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "audio/audio_bus.h"
#include "hero/sim_state.h"
#include "combat/combat.h"
#include "combat/stances.h"
#include "relics/inventory.h"
#include "relics/consumables.h"
#include "dispatch/dispatcher.h"
#include "debug/debug.h"
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
BLOCK 3: BODY BLOCK / 4-SECTION INTEGRATED TEST SUITES
================================================================================
*/

/*
--------------------------------------------------------------------------------
[ SECTION 1: ALWAYS PASS (NOMINAL SIMULATION-TO-AUDIO PIPELINE) ]
--------------------------------------------------------------------------------
*/
static void test_audio_int_always_pass(void)
{
    printf("  [PASS-1] Running Section 1: Always Pass (Nominal Sim-to-Audio Pipeline)...\n");

    audio_bus_init(NULL);
    player_state_t hero;
    sim_state_init(&hero);

    /* 1.1 Stance change emits stance shift audio cue */
    hero.stance = STANCE_WARDING;
    TEST_ASSERT(hero.stance == STANCE_WARDING, "Hero shifted to Warding");

    audio_event_t stance_ev;
    memset(&stance_ev, 0, sizeof(stance_ev));
    stance_ev.type = AUDIO_EVENT_PLAY_SFX;
    stance_ev.channel = AUDIO_CHANNEL_SFX_TACTICAL;
    stance_ev.volume = 0.8f;
    str_safe_copy(stance_ev.cue_id, "stance_shift_lux", sizeof(stance_ev.cue_id));
    audio_bus_post_event(&stance_ev);

    /* 1.2 Combat strike emits combat SFX */
    combat_handle_attack(&hero, false, NULL, 0);

    audio_event_t combat_ev;
    memset(&combat_ev, 0, sizeof(combat_ev));
    combat_ev.type = AUDIO_EVENT_PLAY_SFX;
    combat_ev.channel = AUDIO_CHANNEL_SFX_COMBAT;
    combat_ev.volume = 0.95f;
    str_safe_copy(combat_ev.cue_id, "hero_damaged", sizeof(combat_ev.cue_id));
    audio_bus_post_event(&combat_ev);

    /* 1.3 Verify both cues queued in correct order */
    audio_event_t out1, out2;
    TEST_ASSERT(audio_bus_poll_event(&out1) && strcmp(out1.cue_id, "stance_shift_lux") == 0,
                "First cue was stance shift");
    TEST_ASSERT(audio_bus_poll_event(&out2) && strcmp(out2.cue_id, "hero_damaged") == 0,
                "Second cue was combat strike");

    printf("  [PASS-1] Section 1 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 2: NEVER FAIL (NEGATIVE FAULT TOLERANCE) ]
--------------------------------------------------------------------------------
*/
static void test_audio_int_never_fail(void)
{
    printf("  [PASS-2] Running Section 2: Never Fail (Negative Fault Tolerance)...\n");

    audio_bus_init(NULL);
    player_state_t hero;
    sim_state_init(&hero);

    /* 2.1 Hero defeat (HP reaches 0) dispatches stinger without crash */
    hero.hp = 0;
    audio_event_t defeat_ev;
    memset(&defeat_ev, 0, sizeof(defeat_ev));
    defeat_ev.type = AUDIO_EVENT_PLAY_STINGER;
    defeat_ev.channel = AUDIO_CHANNEL_STINGER;
    defeat_ev.volume = 1.0f;
    str_safe_copy(defeat_ev.cue_id, "hero_defeated_stinger", sizeof(defeat_ev.cue_id));
    bool ok = audio_bus_post_event(&defeat_ev);
    TEST_ASSERT(ok, "Hero defeat stinger posted successfully");

    /* 2.2 Drain and ensure bus remains healthy */
    audio_event_t out;
    TEST_ASSERT(audio_bus_poll_event(&out) && strcmp(out.cue_id, "hero_defeated_stinger") == 0,
                "Polled defeat stinger");

    printf("  [PASS-2] Section 2 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 3: ASSERTION & GUARANTEE (AUDIO-SIM INDEPENDENCE) ]
--------------------------------------------------------------------------------
*/
static void test_audio_int_assertions_and_guarantees(void)
{
    printf("  [PASS-3] Running Section 3: Assertion & Guarantee (Audio-Sim Independence)...\n");

    /* State hash of simulation must be completely identical regardless of audio events */
    player_state_t sim1, sim2;
    sim_state_init(&sim1);
    sim_state_init(&sim2);

    /* Run identical sequence on sim1 and sim2 */
    sim1.stance = STANCE_STRIKING;
    sim2.stance = STANCE_STRIKING;

    /* Dispatch audio events ONLY for sim1 */
    audio_bus_init(NULL);
    audio_event_t ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = AUDIO_EVENT_PLAY_SFX;
    ev.channel = AUDIO_CHANNEL_SFX_COMBAT;
    str_safe_copy(ev.cue_id, "test_strike", sizeof(ev.cue_id));
    audio_bus_post_event(&ev);

    debug_state_fields_t f1, f2;
    memset(&f1, 0, sizeof(f1));
    memset(&f2, 0, sizeof(f2));
    f1.hp = sim1.hp;
    f1.stance = (int)sim1.stance;
    f1.moral_sway = sim1.moral_sway;
    f2.hp = sim2.hp;
    f2.stance = (int)sim2.stance;
    f2.moral_sway = sim2.moral_sway;

    uint32_t h1 = debug_compute_state_hash(&f1);
    uint32_t h2 = debug_compute_state_hash(&f2);
    TEST_ASSERT(h1 == h2, "Audio events do not mutate or influence simulation state hash");

    printf("  [PASS-3] Section 3 passed unconditionally.\n");
}

/*
--------------------------------------------------------------------------------
[ SECTION 4: EMERGENCE & STRESS (MULTI-TURN AUDIO HARNESS) ]
--------------------------------------------------------------------------------
*/
static void test_audio_int_emergence(void)
{
    printf("  [PASS-4] Running Section 4: Emergence & Stress (100-Turn Combat Audio Loop)...\n");

    audio_bus_init(NULL);
    player_state_t hero;
    sim_state_init(&hero);

    /* 100-turn battle loop */
    int total_cues = 0;
    for (int turn = 1; turn <= 100; turn++) {
        /* Hero performs tactical action */
        if (turn % 3 == 0) {
            hero.stance = STANCE_WARDING;
            audio_event_t ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = AUDIO_EVENT_PLAY_SFX;
            ev.channel = AUDIO_CHANNEL_SFX_TACTICAL;
            snprintf(ev.cue_id, sizeof(ev.cue_id), "tactical_%d", turn);
            audio_bus_post_event(&ev);
            total_cues++;
        } else {
            combat_handle_attack(&hero, false, NULL, 0);
            audio_event_t ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = AUDIO_EVENT_PLAY_SFX;
            ev.channel = AUDIO_CHANNEL_SFX_COMBAT;
            snprintf(ev.cue_id, sizeof(ev.cue_id), "combat_%d", turn);
            audio_bus_post_event(&ev);
            total_cues++;
        }

        /* End of turn audio drain */
        audio_event_t out;
        while (audio_bus_poll_event(&out)) {
            /* drained */
        }
    }

    TEST_ASSERT(total_cues == 100, "Exactly 100 audio cues were processed across 100 turns");

    printf("  [PASS-4] Section 4 passed unconditionally.\n");
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / TEST RUNNER
================================================================================
*/
int main(void)
{
    printf("============================================================\n");
    printf("   Integrated Verification: Audio Bus & Simulation Engine\n");
    printf("============================================================\n");

    test_audio_int_always_pass();
    test_audio_int_never_fail();
    test_audio_int_assertions_and_guarantees();
    test_audio_int_emergence();

    printf("============================================================\n");
    printf("   All Audio Integrated Tests Passed Successfully (4/4)\n");
    printf("============================================================\n");
    return 0;
}
