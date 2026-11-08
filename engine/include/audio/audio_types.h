/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          audio_types.h
@layer         Layer 0 / Layer 2: Audio Presentation Subsystem
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Type definitions, mixing buses, sound event packets, cue manifests.
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP).
               2. All enums and structs must have explicit non-anonymous tags.
               3. Audio volume levels bounded between 0.0f and 1.0f.
               4. Event queue capacity strictly fixed to power-of-two size.
               5. Audio strictly follows simulation events (read-only routing).

PURPOSE & ARCHITECTURAL ROLE:
  Defines the core ontological vocabulary for the Tenebris and Lux Audio
  subsystem. Provides multi-channel bus definitions (Master, BGM, Ambience,
  Combat SFX, Tactical SFX, UI, Stingers), zero-heap event payloads, and
  pre-allocated cue registry representations decoupled from platform backends.
================================================================================
*/

#ifndef TENEBRIS_LUX_AUDIO_TYPES_H
#define TENEBRIS_LUX_AUDIO_TYPES_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define AUDIO_MAX_CUE_ID_LEN         64
#define AUDIO_MAX_ASSET_PATH_LEN     128
#define AUDIO_BUS_QUEUE_CAPACITY     128
#define AUDIO_MAX_CHANNELS           8
#define AUDIO_DEFAULT_FADE_SEC       1.5f

/**
 * Multi-channel mixing buses matching presentation audio manifest.
 */
typedef enum audio_channel {
    AUDIO_CHANNEL_MASTER = 0,      /**< Master output bus controlling overall gain */
    AUDIO_CHANNEL_BGM,             /**< Background music orchestral streams */
    AUDIO_CHANNEL_AMBIENCE,        /**< Environmental loops & background soundscapes */
    AUDIO_CHANNEL_SFX_COMBAT,      /**< Combat sound effects (strikes, spells, wards) */
    AUDIO_CHANNEL_SFX_TACTICAL,    /**< Grid movement, footsteps, tile selection */
    AUDIO_CHANNEL_UI,              /**< Menu navigation, button clicks, tab switching */
    AUDIO_CHANNEL_STINGER,         /**< Victory stingers, battle intro fanfares */
    AUDIO_CHANNEL_COUNT            /**< Total canonical mixing bus count */
} audio_channel_t;

/**
 * Sound event types dispatched through the audio bus.
 */
typedef enum audio_event_type {
    AUDIO_EVENT_NONE = 0,          /**< No-op empty event slot */
    AUDIO_EVENT_PLAY_SFX,          /**< Fire one-shot or looping sound effect */
    AUDIO_EVENT_STOP_SFX,          /**< Stop active sound effect instance */
    AUDIO_EVENT_PLAY_BGM,          /**< Initiate BGM track playback */
    AUDIO_EVENT_STOP_BGM,          /**< Halt current BGM track */
    AUDIO_EVENT_CROSSFADE_BGM,     /**< Crossfade to a new musical theme */
    AUDIO_EVENT_PLAY_AMBIENCE,     /**< Start environmental ambient soundscape */
    AUDIO_EVENT_STOP_AMBIENCE,     /**< Halt environmental ambience */
    AUDIO_EVENT_CROSSFADE_AMBIENCE,/**< Crossfade ambient loop to new biome */
    AUDIO_EVENT_PLAY_STINGER,      /**< Play high-priority triumph/defeat stinger */
    AUDIO_EVENT_SET_VOLUME,        /**< Set individual channel volume gain */
    AUDIO_EVENT_MUTE_CHANNEL,      /**< Toggle channel mute state */
    AUDIO_EVENT_STOP_ALL           /**< Emergency silence all channels */
} audio_event_type_t;

/**
 * Ducking attenuation states for audio channels.
 */
typedef enum audio_ducking_state {
    AUDIO_DUCKING_NONE = 0,        /**< Nominal full gain */
    AUDIO_DUCKING_ATTENUATED = 1   /**< Ducked under high-priority stinger/voice */
} audio_ducking_state_t;

/**
 * Configuration descriptor for a single mixing bus.
 */
typedef struct audio_channel_config {
    audio_channel_t channel;
    const char *name;
    float default_volume;          /**< Baseline volume [0.0f, 1.0f] */
    uint32_t max_polyphony;        /**< Concurrency limit (1 to 16) */
    bool is_ducking_source;        /**< If true, triggers ducking of others */
    float duck_ratio;              /**< Ducking multiplier if ducked [0.0f, 1.0f] */
} audio_channel_config_t;

/**
 * Runtime state of an individual audio mixing bus.
 */
typedef struct audio_channel_state {
    float volume;                  /**< Active volume [0.0f, 1.0f] */
    bool muted;                    /**< Mute toggle */
    audio_ducking_state_t ducking; /**< Current ducking status */
    uint32_t active_voices;        /**< Current polyphony voice count */
} audio_channel_state_t;

/**
 * Zero-heap discrete sound event packet passed through circular ring buffer.
 */
typedef struct audio_event {
    audio_event_type_t type;
    audio_channel_t channel;
    char cue_id[AUDIO_MAX_CUE_ID_LEN];
    float volume;                  /**< Instance gain [0.0f, 1.0f] */
    float pan;                     /**< Stereo panning [-1.0f (L), +1.0f (R)] */
    float pitch;                   /**< Pitch scale (1.0f nominal) */
    float fade_duration_sec;       /**< Fade or crossfade duration in seconds */
    bool loop;                     /**< Loop playback flag */
    uint64_t timestamp_us;         /**< Monotonic timestamp in microseconds */
} audio_event_t;

/**
 * Sound effect cue manifest definition.
 */
typedef struct audio_sfx_cue {
    char id[AUDIO_MAX_CUE_ID_LEN];
    char asset_path[AUDIO_MAX_ASSET_PATH_LEN];
    audio_channel_t channel;
    float default_volume;
    float default_pan;
    float pitch_variance;
    uint32_t cooldown_ms;
    bool loop;
} audio_sfx_cue_t;

/**
 * Background music theme manifest definition.
 */
typedef struct audio_bgm_track {
    char id[AUDIO_MAX_CUE_ID_LEN];
    char asset_path[AUDIO_MAX_ASSET_PATH_LEN];
    float default_volume;
    float intro_len_sec;
    float loop_start_sec;
    float loop_end_sec;
    float crossfade_default_sec;
} audio_bgm_track_t;

/**
 * Background environmental ambience soundscape definition.
 */
typedef struct audio_ambience_cue {
    char id[AUDIO_MAX_CUE_ID_LEN];
    char asset_path[AUDIO_MAX_ASSET_PATH_LEN];
    float default_volume;
    float crossfade_sec;
    bool loop;
} audio_ambience_cue_t;

/**
 * Telemetry and diagnostics counters for the audio event bus.
 */
typedef struct audio_bus_stats {
    uint64_t events_posted;        /**< Total events successfully enqueued */
    uint64_t events_consumed;      /**< Total events popped by audio consumer */
    uint32_t queue_high_watermark; /**< Peak queue depth observed */
    uint32_t dropped_events;       /**< Queue overflow drop count (should be 0) */
} audio_bus_stats_t;

/**
 * Global mixer configuration passed during initialization.
 */
typedef struct audio_bus_config {
    float master_volume;
    float bgm_volume;
    float ambience_volume;
    float sfx_combat_volume;
    float sfx_tactical_volume;
    float ui_volume;
    float stinger_volume;
    uint32_t sample_rate_hz;
    bool limiter_enabled;
} audio_bus_config_t;

/*
================================================================================
BLOCK 3: BODY BLOCK
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

#endif /* TENEBRIS_LUX_AUDIO_TYPES_H */

