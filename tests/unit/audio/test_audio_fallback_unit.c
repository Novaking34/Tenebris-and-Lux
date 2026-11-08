/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          test_audio_fallback_unit.c
@layer         Layer 4: Verification Suite
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Unit test harness for quarantined emergency audio fallback profile.
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

#include "audio/audio_fallback.h"
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
static void test_audio_fallback_always_pass(void)
{
    printf("  [TRUTH-1] Running Section 1: Truth (Always Pass)...\n");

    audio_fallback_profile_t prof;
    audio_fallback_get_profile(&prof);

    TEST_ASSERT(strcmp(prof.profile_name, AUDIO_FALLBACK_PROFILE_NAME) == 0,
                "profile name matches constant");
    TEST_ASSERT(!prof.loaded_from_data, "loaded_from_data is false");
    TEST_ASSERT(prof.config.sample_rate_hz == 48000, "sample rate is 48000");

    printf("  [TRUTH-1] Section 1 passed: Baseline defaults valid.\n");
}

/* --- SECTION 2: NEVER FAIL (Fault Tolerance & Boundary Guards) --- */
static void test_audio_fallback_never_fail(void)
{
    printf("  [SAFETY-2] Running Section 2: Safety (Never Fail)...\n");

    /* Null pointer guard */
    audio_fallback_get_profile(NULL);

    printf("  [SAFETY-2] Section 2 passed: Fault tolerance bounds verified.\n");
}

/* --- SECTION 3: AXIOMS & TRIPWIRES (Volume Bounds & Application) --- */
static void test_audio_fallback_axioms_and_tripwires(void)
{
    printf("  [AXIOM-3] Running Section 3: Axioms (Volume Bounds & Application)...\n");

    audio_fallback_profile_t prof;
    audio_fallback_get_profile(&prof);

    TEST_ASSERT(prof.config.master_volume >= 0.0f && prof.config.master_volume <= 1.0f,
                "master volume bounded");
    TEST_ASSERT(prof.config.bgm_volume >= 0.0f && prof.config.bgm_volume <= 1.0f,
                "bgm volume bounded");
    TEST_ASSERT(prof.config.ambience_volume >= 0.0f && prof.config.ambience_volume <= 1.0f,
                "ambience volume bounded");
    TEST_ASSERT(prof.config.sfx_combat_volume >= 0.0f && prof.config.sfx_combat_volume <= 1.0f,
                "sfx combat volume bounded");
    TEST_ASSERT(prof.config.sfx_tactical_volume >= 0.0f && prof.config.sfx_tactical_volume <= 1.0f,
                "sfx tactical volume bounded");
    TEST_ASSERT(prof.config.ui_volume >= 0.0f && prof.config.ui_volume <= 1.0f,
                "ui volume bounded");
    TEST_ASSERT(prof.config.stinger_volume >= 0.0f && prof.config.stinger_volume <= 1.0f,
                "stinger volume bounded");

    /* Test applying fallback configuration to active bus */
    audio_fallback_apply();
    TEST_ASSERT(audio_bus_is_initialized(), "audio bus initialized by fallback");
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_MASTER) == prof.config.master_volume,
                "master volume applied to bus");
    TEST_ASSERT(audio_bus_get_channel_volume(AUDIO_CHANNEL_BGM) == prof.config.bgm_volume,
                "bgm volume applied to bus");

    printf("  [AXIOM-3] Section 3 passed: Volume bounds & application axioms hold.\n");
}

/* --- SECTION 4: EMERGENCE & SELF-AUDIT --- */
static void test_audio_fallback_emergence(void)
{
    printf("  [EMERGE-4] Running Section 4: Emergence (Self-Audit)...\n");

    TEST_ASSERT(audio_fallback_audit(), "audio_fallback_audit passes");

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
    printf("   Tenebris and Lux - Audio Fallback Profile Unit Test      \n");
    printf("============================================================\n");

    test_audio_fallback_always_pass();
    test_audio_fallback_never_fail();
    test_audio_fallback_axioms_and_tripwires();
    test_audio_fallback_emergence();

    printf("============================================================\n");
    printf("   All Audio Fallback Unit Tests Passed (4/4)               \n");
    printf("============================================================\n");
    return 0;
}

