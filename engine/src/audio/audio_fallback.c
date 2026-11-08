/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          audio_fallback.c
@layer         Layer 0 / Layer 2: Audio Presentation Subsystem
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Quarantined Monolithic Emergency Default Profile Implementation
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP).
               2. All enums and structs must have explicit non-anonymous tags.
               3. Provenance tracking: loaded_from_data is unconditionally false.
               4. Volume levels clamped within [0.0f, 1.0f].
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include "audio/audio_fallback.h"
#include "audio/audio_bus.h"
#include <string.h>

static const audio_bus_config_t s_fallback_config = {
    .master_volume = 1.0f,
    .bgm_volume = 0.70f,
    .ambience_volume = 0.50f,
    .sfx_combat_volume = 0.85f,
    .sfx_tactical_volume = 0.75f,
    .ui_volume = 0.80f,
    .stinger_volume = 0.90f,
    .sample_rate_hz = 48000,
    .limiter_enabled = true
};

/*
================================================================================
BLOCK 3: BODY BLOCK / IMPLEMENTATION
================================================================================
*/

void audio_fallback_get_profile(audio_fallback_profile_t *out_profile)
{
    if (out_profile == NULL) {
        return;
    }

    out_profile->profile_name = AUDIO_FALLBACK_PROFILE_NAME;
    out_profile->loaded_from_data = false;
    out_profile->config = s_fallback_config;
}

void audio_fallback_apply(void)
{
    audio_bus_init(&s_fallback_config);
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

bool audio_fallback_audit(void)
{
    audio_fallback_profile_t prof;
    audio_fallback_get_profile(&prof);

    if (prof.loaded_from_data) {
        return false;
    }

    if (strcmp(prof.profile_name, AUDIO_FALLBACK_PROFILE_NAME) != 0) {
        return false;
    }

    if (prof.config.master_volume < 0.0f || prof.config.master_volume > 1.0f ||
        prof.config.bgm_volume < 0.0f || prof.config.bgm_volume > 1.0f ||
        prof.config.ambience_volume < 0.0f || prof.config.ambience_volume > 1.0f ||
        prof.config.sfx_combat_volume < 0.0f || prof.config.sfx_combat_volume > 1.0f ||
        prof.config.sfx_tactical_volume < 0.0f || prof.config.sfx_tactical_volume > 1.0f ||
        prof.config.ui_volume < 0.0f || prof.config.ui_volume > 1.0f ||
        prof.config.stinger_volume < 0.0f || prof.config.stinger_volume > 1.0f) {
        return false;
    }

    if (prof.config.sample_rate_hz != 48000) {
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

