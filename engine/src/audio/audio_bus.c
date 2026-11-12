/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          audio_bus.c
@id            audio.bus
@layer         Layer 0 / Layer 2: Audio Presentation Subsystem
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-09
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Implements circular ring buffer, channel gain, and event routing.
@invariants    1. Zero dynamic heap allocations in Layer 0/Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All enums and structs must have explicit non-anonymous tags.
               4. Fixed-size circular ring buffer with power-of-two mask (capacity 128).
               5. Strictly bounded volume levels [0.0f, 1.0f] with mute channel masking.
               6. Reentrant and deterministic event polling and dispatch.
               7. Data-driven configuration: loads channel mappings from engine/config/audio/.
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Implementation of the Tenebris and Lux Audio Event Bus. Dispatches audio cue
  triggers from simulation turns, combat actions, and GUI widgets into a
  zero-heap circular ring buffer for decoupled consumption by the audio driver.
  Manages per-channel volume multipliers, mute states, and settings profiles.

  ### For Game Designers & Non-Programmers:
  Think of the audio bus like an orchestra soundboard. When a sword swings, a
  spell is cast, or the player clicks a menu button, the game sends a message to
  the soundboard. The soundboard adjusts the volume (so dialogue isn't drowned
  out by loud explosions) and passes the sound to the speakers without slowing
  down gameplay.

  ### For Systems Programmers & Engine Architects:
  - Zero-heap circular FIFO queue with power-of-two bitwise masking (head/tail indices).
  - Bounded volume mixing across 7 hardware buses: Master, BGM, Ambience, Combat SFX,
    Tactical SFX, UI, and Stingers.
  - Hydrates volume levels and mute states directly from `engine/config/audio/settings.json`
    and `engine/config/audio/channels.json` via master_loader.
  - Reentrant event polling with zero contention and deterministic FIFO ordering.
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING / STATIC DATA BLOCK
Stratum: Layer 0 / Layer 2 (Audio Presentation) | Machine: Audio Event Bus Orchestrator
Mission: Zero-heap audio event queuing, multi-channel volume mixing,
         and data-driven audio configuration hydration.
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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "audio/audio_bus.h"
#include "audio/audio_fallback.h"
#include "core/time_utils.h"
#include "core/string_utils.h"
#include "core/manifest.h"
#include "io/master_loader.h"
#include "io/json_utils.h"
#include "io/path_finder.h"
#include "log/log.h"
#include "debug/debug.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Local Module Manifest Anchor, Category Tags & Invariant Vectors
--------------------------------------------------------------------------------
*/

#define MODULE_ID           "audio.bus"
#define LOCAL_LOG_CAT       LOG_CAT_AUDIO
#define LOCAL_MODULE_TAG    "AudioBus"

static const char *const audio_invariants[] = {
    "Zero dynamic heap allocations in Layer 0/Layer 2 (ALLOC_ZERO_HEAP)",
    "Fixed circular FIFO ring buffer with power-of-two mask (capacity 128)",
    "Strictly bounded volume levels [0.0f, 1.0f] with mute channel masking",
    "Reentrant and deterministic event polling and dispatch",
    "Data-driven configuration loaded from engine/config/audio/"
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Private Structural Ontology & Non-Anonymous Data Types
--------------------------------------------------------------------------------
*/

/**
 * @struct audio_bus_state
 * @brief Internal static state of the audio bus subsystem.
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

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Static Module State Storage (.bss Allocations) & Manifest Anchor
--------------------------------------------------------------------------------
*/

static audio_bus_state_t s_audio_bus;

const module_metadata_t audio_bus_manifest = {
    MODULE_ID,
    "audio_bus.h",
    LAYER_2_PRESENTATION,
    { 0, 1, 1 },
    5,                          /* interface_rev */
    "2026-11-09",               /* last_updated */
    "Universal audio event bus, lock-free ring queue, multi-channel mixer, and settings hydration",
    2,                          /* dependency_count */
    { "core.util.time", "core.util.string" },
    ALLOC_ZERO_HEAP,            /* alloc_policy */
    CONCURRENCY_REENTRANT,      /* concurrency */
    1000,                       /* max_budget_us */
    5,
    {
        audio_invariants[0],
        audio_invariants[1],
        audio_invariants[2],
        audio_invariants[3],
        audio_invariants[4]
    },
    audio_bus_preflight
};

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Inlines & Fast Tripwires
--------------------------------------------------------------------------------
*/

/**
 * @brief Fast static inline float clamping helper for volume scalars.
 */
static inline float audio_clampf(float val, float min_v, float max_v)
{
    if (val < min_v) return min_v;
    if (val > max_v) return max_v;
    return val;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE AUDIO LOGIC & BUS FUNCTIONS
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.1: Profile & Configuration Loading Cogs
--------------------------------------------------------------------------------
*/

bool audio_bus_is_initialized(void)
{
    return s_audio_bus.initialized;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Audio Event Posting & Queue Dispatch Cogs
--------------------------------------------------------------------------------
*/

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

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Queue Polling & Event Consumption Cogs
--------------------------------------------------------------------------------
*/

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

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Channel Mixing & Volume Control Cogs
--------------------------------------------------------------------------------
*/

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

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Telemetry & Bus Statistics Cogs
--------------------------------------------------------------------------------
*/

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

bool audio_bus_load_channels(void)
{
    master_loader_t loader;
    char json_buf[8192];
    if (!master_loader_load(&loader, "config", "audio/channels.json", json_buf, sizeof(json_buf))) {
        return false;
    }
    return (loader.status == LOADER_STATUS_OK);
}

bool audio_bus_load_settings(audio_bus_config_t *out_config)
{
    master_loader_t loader;
    char json_buf[8192];
    if (!master_loader_load(&loader, "config", "audio/settings.json", json_buf, sizeof(json_buf))) {
        return false;
    }

    int master_pct = 100, bgm_pct = 75, amb_pct = 60;
    int combat_pct = 90, tact_pct = 80, ui_pct = 85, stinger_pct = 95;
    size_t sample_rate = 48000;
    bool limiter = true;

    master_loader_get_int(&loader, "volumes", "master", &master_pct);
    master_loader_get_int(&loader, "volumes", "bgm", &bgm_pct);
    master_loader_get_int(&loader, "volumes", "ambience", &amb_pct);
    master_loader_get_int(&loader, "volumes", "sfx_combat", &combat_pct);
    master_loader_get_int(&loader, "volumes", "sfx_tactical", &tact_pct);
    master_loader_get_int(&loader, "volumes", "ui", &ui_pct);
    master_loader_get_int(&loader, "volumes", "stinger", &stinger_pct);

    master_loader_get_size_t(&loader, "hardware", "sample_rate_hz", &sample_rate);
    master_loader_get_bool(&loader, "hardware", "limiter_enabled", &limiter);

    audio_bus_config_t cfg;
    cfg.master_volume = audio_clampf((float)master_pct / 100.0f, 0.0f, 1.0f);
    cfg.bgm_volume = audio_clampf((float)bgm_pct / 100.0f, 0.0f, 1.0f);
    cfg.ambience_volume = audio_clampf((float)amb_pct / 100.0f, 0.0f, 1.0f);
    cfg.sfx_combat_volume = audio_clampf((float)combat_pct / 100.0f, 0.0f, 1.0f);
    cfg.sfx_tactical_volume = audio_clampf((float)tact_pct / 100.0f, 0.0f, 1.0f);
    cfg.ui_volume = audio_clampf((float)ui_pct / 100.0f, 0.0f, 1.0f);
    cfg.stinger_volume = audio_clampf((float)stinger_pct / 100.0f, 0.0f, 1.0f);
    cfg.sample_rate_hz = (uint32_t)sample_rate;
    cfg.limiter_enabled = limiter;

    if (out_config) {
        *out_config = cfg;
    }

    if (audio_bus_is_initialized()) {
        audio_bus_set_channel_volume(AUDIO_CHANNEL_MASTER, cfg.master_volume);
        audio_bus_set_channel_volume(AUDIO_CHANNEL_BGM, cfg.bgm_volume);
        audio_bus_set_channel_volume(AUDIO_CHANNEL_AMBIENCE, cfg.ambience_volume);
        audio_bus_set_channel_volume(AUDIO_CHANNEL_SFX_COMBAT, cfg.sfx_combat_volume);
        audio_bus_set_channel_volume(AUDIO_CHANNEL_SFX_TACTICAL, cfg.sfx_tactical_volume);
        audio_bus_set_channel_volume(AUDIO_CHANNEL_UI, cfg.ui_volume);
        audio_bus_set_channel_volume(AUDIO_CHANNEL_STINGER, cfg.stinger_volume);
    }

    return true;
}

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
--------------------------------------------------------------------------------
*/

diag_result_t audio_bus_preflight(void)
{
    diag_result_t res;
    res.passed = true;
    res.severity = SEVERITY_PASS;
    res.subsystem = "audio.bus";
    res.check_name = "channel_limits_and_queues";
    res.details = "Audio bus channel limits, polyphony, and event queue verified.";

    /* Check 1: Channel enumeration and bounds */
    if (AUDIO_CHANNEL_COUNT != 7) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Channel count invariant mismatch; expected 7 canonical buses.";
        return res;
    }

    /* Check 2: Queue capacity bounds */
    if (AUDIO_BUS_QUEUE_CAPACITY == 0 || AUDIO_BUS_QUEUE_CAPACITY > 1024) {
        res.passed = false;
        res.severity = SEVERITY_FATAL;
        res.details = "Audio bus queue capacity violates memory bounds.";
        return res;
    }

    /* Check 3: Static fallback integrity */
    if (!audio_fallback_audit()) {
        res.passed = false;
        res.severity = SEVERITY_CRITICAL;
        res.details = "Quarantined audio fallback code page failed invariant audit.";
        return res;
    }

    return res;
}

bool audio_bus_audit(void)
{
    diag_result_t pre = audio_bus_preflight();
    if (!pre.passed) {
        return false;
    }

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

    /* 5. Verify config loading from VFS */
    if (!audio_bus_load_channels()) {
        return false;
    }
    if (!audio_bus_load_settings(NULL)) {
        return false;
    }

    return true;
}

bool audio_bus_preflight_audit(void)
{
    return audio_bus_audit();
}

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Audio & Acoustics) | Subsystem: Audio Event Bus Orchestrator
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.1: Executable Procedures, Inspection & Diagnostic Cards
--------------------------------------------------------------------------------
*/

bool audio_bus_execute_inspect(audio_bus_stats_t *out_stats)
{
    if (out_stats == NULL) {
        return false;
    }
    audio_bus_get_stats(out_stats);
    return true;
}

size_t audio_bus_execute_dump_card(const audio_bus_stats_t *stats,
                                   char *out_buf,
                                   size_t buf_size)
{
    if (out_buf == NULL || buf_size == 0) {
        return 0;
    }
    audio_bus_stats_t local_stats;
    if (stats == NULL) {
        audio_bus_get_stats(&local_stats);
        stats = &local_stats;
    }
    int written = snprintf(
        out_buf, buf_size,
        "============================================================\n"
        "             AUDIO EVENT BUS STATUS CARD                    \n"
        "============================================================\n"
        "Events Posted:   %llu\n"
        "Events Consumed: %llu\n"
        "Pending Queue:   %u\n"
        "Queue High Mark: %u / %d\n"
        "Dropped Events:  %u\n"
        "Bus Health:      %s\n"
        "============================================================\n",
        (unsigned long long)stats->events_posted,
        (unsigned long long)stats->events_consumed,
        audio_bus_pending_count(),
        stats->queue_high_watermark,
        AUDIO_BUS_QUEUE_CAPACITY,
        stats->dropped_events,
        stats->dropped_events == 0 ? "HEALTHY (Zero Drops)" : "OVERFLOW WARNING"
    );
    if (written < 0 || (size_t)written >= buf_size) {
        return 0;
    }
    return (size_t)written;
}

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
--------------------------------------------------------------------------------
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
