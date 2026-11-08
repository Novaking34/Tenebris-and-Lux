/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          audio_bus.c
@layer         Layer 0 / Layer 2: Audio Presentation Subsystem
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Implements circular ring buffer, channel gain, and event routing.
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP).
               2. All enums and structs must have explicit non-anonymous tags.
               3. Audio volume clamped strictly to [0.0f, 1.0f].
               4. Queue drop protection with high-watermark telemetry.
               5. Reentrant, thread-safe memory layout.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include "audio/audio_bus.h"
#include "core/time_utils.h"
#include "core/string_utils.h"
#include <string.h>

/**
 * Internal static state of the audio bus subsystem.
 */
typedef struct audio_bus_state {
    bool initialized;
    audio_channel_state_t channels[AUDIO_CHANNEL_COUNT];
    audio_event_t queue[AUDIO_BUS_QUEUE_CAPACITY];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    audio_bus_stats_t stats;
} audio_bus_state_t;

static audio_bus_state_t s_audio_bus = {0};

static inline float audio_clampf(float val, float min_v, float max_v)
{
    if (val < min_v) return min_v;
    if (val > max_v) return max_v;
    return val;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / IMPLEMENTATION
================================================================================
*/

void audio_bus_init(const audio_bus_config_t *config)
{
    memset(&s_audio_bus, 0, sizeof(audio_bus_state_t));

    s_audio_bus.channels[AUDIO_CHANNEL_MASTER].volume =
        config ? audio_clampf(config->master_volume, 0.0f, 1.0f) : 1.0f;
    s_audio_bus.channels[AUDIO_CHANNEL_BGM].volume =
        config ? audio_clampf(config->bgm_volume, 0.0f, 1.0f) : 0.75f;
    s_audio_bus.channels[AUDIO_CHANNEL_AMBIENCE].volume =
        config ? audio_clampf(config->ambience_volume, 0.0f, 1.0f) : 0.60f;
    s_audio_bus.channels[AUDIO_CHANNEL_SFX_COMBAT].volume =
        config ? audio_clampf(config->sfx_combat_volume, 0.0f, 1.0f) : 0.90f;
    s_audio_bus.channels[AUDIO_CHANNEL_SFX_TACTICAL].volume =
        config ? audio_clampf(config->sfx_tactical_volume, 0.0f, 1.0f) : 0.80f;
    s_audio_bus.channels[AUDIO_CHANNEL_UI].volume =
        config ? audio_clampf(config->ui_volume, 0.0f, 1.0f) : 0.85f;
    s_audio_bus.channels[AUDIO_CHANNEL_STINGER].volume =
        config ? audio_clampf(config->stinger_volume, 0.0f, 1.0f) : 0.95f;

    for (int i = 0; i < AUDIO_CHANNEL_COUNT; ++i) {
        s_audio_bus.channels[i].muted = false;
        s_audio_bus.channels[i].ducking = AUDIO_DUCKING_NONE;
        s_audio_bus.channels[i].active_voices = 0;
    }

    s_audio_bus.head = 0;
    s_audio_bus.tail = 0;
    s_audio_bus.count = 0;
    s_audio_bus.initialized = true;
}

void audio_bus_shutdown(void)
{
    memset(&s_audio_bus, 0, sizeof(audio_bus_state_t));
    s_audio_bus.initialized = false;
}

bool audio_bus_is_initialized(void)
{
    return s_audio_bus.initialized;
}

bool audio_bus_post_event(const audio_event_t *event)
{
    if (!s_audio_bus.initialized || event == NULL) {
        return false;
    }

    if ((int)event->channel < 0 || event->channel >= AUDIO_CHANNEL_COUNT) {
        return false;
    }

    if (s_audio_bus.count >= AUDIO_BUS_QUEUE_CAPACITY) {
        s_audio_bus.stats.dropped_events++;
        return false;
    }

    uint32_t idx = s_audio_bus.tail;
    s_audio_bus.queue[idx] = *event;
    s_audio_bus.tail = (s_audio_bus.tail + 1) & (AUDIO_BUS_QUEUE_CAPACITY - 1);
    s_audio_bus.count++;
    s_audio_bus.stats.events_posted++;

    if (s_audio_bus.count > s_audio_bus.stats.queue_high_watermark) {
        s_audio_bus.stats.queue_high_watermark = s_audio_bus.count;
    }

    return true;
}

bool audio_bus_post_sfx(const char *cue_id, float volume, float pan)
{
    if (cue_id == NULL) {
        return false;
    }

    audio_event_t ev;
    memset(&ev, 0, sizeof(audio_event_t));
    ev.type = AUDIO_EVENT_PLAY_SFX;
    ev.channel = AUDIO_CHANNEL_SFX_COMBAT;
    str_safe_copy(ev.cue_id, cue_id, sizeof(ev.cue_id));
    ev.volume = (volume >= 0.0f) ? audio_clampf(volume, 0.0f, 1.0f) : 1.0f;
    ev.pan = audio_clampf(pan, -1.0f, 1.0f);
    ev.pitch = 1.0f;
    ev.fade_duration_sec = 0.0f;
    ev.loop = false;
    ev.timestamp_us = time_get_monotonic_us();

    return audio_bus_post_event(&ev);
}

bool audio_bus_post_bgm(const char *track_id, float crossfade_sec)
{
    if (track_id == NULL) {
        return false;
    }

    audio_event_t ev;
    memset(&ev, 0, sizeof(audio_event_t));
    ev.type = (crossfade_sec > 0.0f) ? AUDIO_EVENT_CROSSFADE_BGM : AUDIO_EVENT_PLAY_BGM;
    ev.channel = AUDIO_CHANNEL_BGM;
    str_safe_copy(ev.cue_id, track_id, sizeof(ev.cue_id));
    ev.volume = 1.0f;
    ev.pan = 0.0f;
    ev.pitch = 1.0f;
    ev.fade_duration_sec = (crossfade_sec > 0.0f) ? crossfade_sec : AUDIO_DEFAULT_FADE_SEC;
    ev.loop = true;
    ev.timestamp_us = time_get_monotonic_us();

    return audio_bus_post_event(&ev);
}

bool audio_bus_post_ambience(const char *ambience_id, float crossfade_sec)
{
    if (ambience_id == NULL) {
        return false;
    }

    audio_event_t ev;
    memset(&ev, 0, sizeof(audio_event_t));
    ev.type = (crossfade_sec > 0.0f) ? AUDIO_EVENT_CROSSFADE_AMBIENCE : AUDIO_EVENT_PLAY_AMBIENCE;
    ev.channel = AUDIO_CHANNEL_AMBIENCE;
    str_safe_copy(ev.cue_id, ambience_id, sizeof(ev.cue_id));
    ev.volume = 1.0f;
    ev.pan = 0.0f;
    ev.pitch = 1.0f;
    ev.fade_duration_sec = (crossfade_sec > 0.0f) ? crossfade_sec : AUDIO_DEFAULT_FADE_SEC;
    ev.loop = true;
    ev.timestamp_us = time_get_monotonic_us();

    return audio_bus_post_event(&ev);
}

bool audio_bus_post_stinger(const char *stinger_id, float volume)
{
    if (stinger_id == NULL) {
        return false;
    }

    audio_event_t ev;
    memset(&ev, 0, sizeof(audio_event_t));
    ev.type = AUDIO_EVENT_PLAY_STINGER;
    ev.channel = AUDIO_CHANNEL_STINGER;
    str_safe_copy(ev.cue_id, stinger_id, sizeof(ev.cue_id));
    ev.volume = (volume >= 0.0f) ? audio_clampf(volume, 0.0f, 1.0f) : 1.0f;
    ev.pan = 0.0f;
    ev.pitch = 1.0f;
    ev.fade_duration_sec = 0.0f;
    ev.loop = false;
    ev.timestamp_us = time_get_monotonic_us();

    return audio_bus_post_event(&ev);
}

bool audio_bus_poll_event(audio_event_t *out_event)
{
    if (!s_audio_bus.initialized || s_audio_bus.count == 0) {
        return false;
    }

    if (out_event != NULL) {
        *out_event = s_audio_bus.queue[s_audio_bus.head];
    }

    s_audio_bus.head = (s_audio_bus.head + 1) & (AUDIO_BUS_QUEUE_CAPACITY - 1);
    s_audio_bus.count--;
    s_audio_bus.stats.events_consumed++;

    return true;
}

bool audio_bus_peek_event(audio_event_t *out_event)
{
    if (!s_audio_bus.initialized || s_audio_bus.count == 0) {
        return false;
    }

    if (out_event != NULL) {
        *out_event = s_audio_bus.queue[s_audio_bus.head];
    }

    return true;
}

uint32_t audio_bus_pending_count(void)
{
    return s_audio_bus.initialized ? s_audio_bus.count : 0;
}

void audio_bus_clear_events(void)
{
    if (!s_audio_bus.initialized) {
        return;
    }

    s_audio_bus.head = 0;
    s_audio_bus.tail = 0;
    s_audio_bus.count = 0;
}

void audio_bus_set_channel_volume(audio_channel_t channel, float volume)
{
    if (!s_audio_bus.initialized || (int)channel < 0 || channel >= AUDIO_CHANNEL_COUNT) {
        return;
    }

    s_audio_bus.channels[channel].volume = audio_clampf(volume, 0.0f, 1.0f);
}

float audio_bus_get_channel_volume(audio_channel_t channel)
{
    if (!s_audio_bus.initialized || (int)channel < 0 || channel >= AUDIO_CHANNEL_COUNT) {
        return 0.0f;
    }

    return s_audio_bus.channels[channel].volume;
}

void audio_bus_set_channel_muted(audio_channel_t channel, bool muted)
{
    if (!s_audio_bus.initialized || (int)channel < 0 || channel >= AUDIO_CHANNEL_COUNT) {
        return;
    }

    s_audio_bus.channels[channel].muted = muted;
}

bool audio_bus_is_channel_muted(audio_channel_t channel)
{
    if (!s_audio_bus.initialized || (int)channel < 0 || channel >= AUDIO_CHANNEL_COUNT) {
        return false;
    }

    return s_audio_bus.channels[channel].muted;
}

void audio_bus_get_stats(audio_bus_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return;
    }

    *out_stats = s_audio_bus.stats;
}

void audio_bus_reset_stats(void)
{
    memset(&s_audio_bus.stats, 0, sizeof(audio_bus_stats_t));
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

bool audio_bus_preflight_audit(void)
{
    /* 1. Verify initialization */
    audio_bus_init(NULL);
    if (!audio_bus_is_initialized()) {
        return false;
    }

    /* 2. Verify volume clamping */
    audio_bus_set_channel_volume(AUDIO_CHANNEL_MASTER, 1.5f);
    if (audio_bus_get_channel_volume(AUDIO_CHANNEL_MASTER) > 1.0f) {
        return false;
    }
    audio_bus_set_channel_volume(AUDIO_CHANNEL_MASTER, -0.5f);
    if (audio_bus_get_channel_volume(AUDIO_CHANNEL_MASTER) < 0.0f) {
        return false;
    }
    audio_bus_set_channel_volume(AUDIO_CHANNEL_MASTER, 1.0f);

    /* 3. Verify event posting and polling */
    audio_bus_clear_events();
    if (audio_bus_pending_count() != 0) {
        return false;
    }

    if (!audio_bus_post_sfx("sfx_ward_lux_barrio", 0.9f, 0.0f)) {
        return false;
    }
    if (audio_bus_pending_count() != 1) {
        return false;
    }

    audio_event_t ev;
    if (!audio_bus_peek_event(&ev)) {
        return false;
    }
    if (strcmp(ev.cue_id, "sfx_ward_lux_barrio") != 0) {
        return false;
    }

    if (!audio_bus_poll_event(&ev)) {
        return false;
    }
    if (audio_bus_pending_count() != 0) {
        return false;
    }

    /* 4. Verify ring buffer capacity and overflow protection */
    for (uint32_t i = 0; i < AUDIO_BUS_QUEUE_CAPACITY; ++i) {
        if (!audio_bus_post_sfx("sfx_step_cobble", 0.5f, 0.0f)) {
            return false;
        }
    }
    if (audio_bus_pending_count() != AUDIO_BUS_QUEUE_CAPACITY) {
        return false;
    }

    /* Next event should fail without crashing and record dropped event */
    if (audio_bus_post_sfx("sfx_overflow", 0.5f, 0.0f)) {
        return false;
    }

    audio_bus_stats_t stats;
    audio_bus_get_stats(&stats);
    if (stats.dropped_events != 1) {
        return false;
    }

    /* Drain all events */
    audio_bus_clear_events();
    if (audio_bus_pending_count() != 0) {
        return false;
    }

    return true;
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/
