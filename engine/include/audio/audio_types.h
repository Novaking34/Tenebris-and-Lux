/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          audio_types.h
@layer         Layer 0 / Layer 2: Audio Presentation Subsystem
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Type definitions, mixing buses, sound event packets, cue manifests.
@invariants    1. Zero dynamic heap allocations in Layer 0/Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All enums and structs must have explicit non-anonymous tags.
               4. Audio volume levels strictly bounded within [0.0f, 1.0f].
               5. Event queue capacity strictly fixed to a power-of-two size (128).
               6. Audio presentation strictly follows simulation events (read-only routing).
               7. Fixed-size string buffers for cue IDs (64 bytes) and asset paths (128 bytes).
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Defines the core ontological vocabulary for the Tenebris and Lux Audio
  subsystem. Provides multi-channel bus definitions (Master, BGM, Ambience,
  Combat SFX, Tactical SFX, UI, Stingers), zero-heap event payloads, and
  pre-allocated cue registry representations decoupled from platform backends.

  ### For Game Designers & Non-Programmers:
  This file names all the types of sounds the game can play. It defines the
  categories (music, sword clangs, menu clicks, dramatic stingers) and limits
  how loud they can be so that nothing ever plays distorted or causes clipping.

  ### For Systems Programmers & Engine Architects:
  - Canonical mixing channel enumeration (`audio_channel_t`: 7 canonical buses).
  - Strongly typed, non-anonymous structs for clean DWARF debugging (`audio_event_t`,
    `audio_cue_t`, `audio_bus_config_t`).
  - Zero dynamic heap allocation: fixed-width arrays and stack/static value objects.
  - Reentrant event representations for safe multi-bus routing.
================================================================================
*/

#ifndef TENEBRIS_LUX_AUDIO_TYPES_H
#define TENEBRIS_LUX_AUDIO_TYPES_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 / Layer 2: Audio Presentation Subsystem
================================================================================
Mission: Mixing channel enums, discrete event packets, audio limits, sound cues,
and pre-conditional static validation inlines for zero-heap audio dispatch.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Include Hierarchy: 3-Tier Downward Architecture
--------------------------------------------------------------------------------
Tier 1: Standard ISO C11 Library Headers
Tier 2: External Dependencies (None)
Tier 3: Internal Engine Foundation Headers
--------------------------------------------------------------------------------
*/

/* --- Tier 1: Standard ISO C11 Library Headers --- */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Tier 2: External Dependencies (None) --- */

/* --- Tier 3: Internal Engine Foundation Headers --- */
#include "core/manifest.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions, Constants & Buffer Limits
--------------------------------------------------------------------------------
Compile-time sizing limits for sound cue identifiers, file paths, and queues.
For Designers: Boundaries ensure sound effect names (up to 64 chars) and asset
paths (up to 128 chars) fit in fixed memory without crashing the audio system.
For Programmers: Guarantees zero heap allocation (ALLOC_ZERO_HEAP) in .rodata.
--------------------------------------------------------------------------------
*/

/** Maximum character length for sound effect or music cue identifiers. */
#define AUDIO_MAX_CUE_ID_LEN         64

/** Maximum relative path length in bytes for audio asset files. */
#define AUDIO_MAX_ASSET_PATH_LEN     128

/** Maximum pending audio events retained in the ring buffer queue. */
#define AUDIO_BUS_QUEUE_CAPACITY     128

/** Maximum hardware mixing channels supported simultaneously. */
#define AUDIO_MAX_CHANNELS           8

/** Canonical default crossfade duration in seconds (1.5s). */
#define AUDIO_DEFAULT_FADE_SEC       1.5f

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Audio Types ontology.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Audio Types & Ontological Descriptors. */
#define AUDIO_TYPES_MODULE_ID          "audio.types"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Ontological types for mixing buses, sound events, channel states, and cues.
--------------------------------------------------------------------------------
*/

/**
 * @enum audio_channel
 * @brief Multi-channel mixing buses matching presentation audio manifest.
 *
 * For Designers: The 7 distinct volume channels in the game settings menu:
 * Master (overall), BGM (music), Ambience (nature), Combat SFX (sword/shield),
 * Tactical SFX (steps/grid), UI (clicks/menus), and Stinger (victory fanfare).
 *
 * For Programmers: Explicitly numbered channel enum indexable into channel arrays.
 */
typedef enum audio_channel {
    AUDIO_CHANNEL_MASTER = 0,      /**< Master output bus controlling overall gain. */
    AUDIO_CHANNEL_BGM,             /**< Background music orchestral streams. */
    AUDIO_CHANNEL_AMBIENCE,        /**< Environmental loops & background soundscapes. */
    AUDIO_CHANNEL_SFX_COMBAT,      /**< Combat sound effects (strikes, spells, wards). */
    AUDIO_CHANNEL_SFX_TACTICAL,    /**< Grid movement, footsteps, tile selection. */
    AUDIO_CHANNEL_UI,              /**< Menu navigation, button clicks, tab switching. */
    AUDIO_CHANNEL_STINGER,         /**< Victory stingers, battle intro fanfares. */
    AUDIO_CHANNEL_COUNT            /**< Total canonical mixing bus count. */
} audio_channel_t;

/**
 * @enum audio_event_type
 * @brief Sound event types dispatched through the audio bus.
 *
 * For Designers: The different actions sound events can trigger, such as starting
 * a song, crossfading to a battle theme, playing a sound effect, or muting a bus.
 *
 * For Programmers: Tagged command enum instructing the backend audio driver.
 */
typedef enum audio_event_type {
    AUDIO_EVENT_NONE = 0,          /**< No-op empty event slot. */
    AUDIO_EVENT_PLAY_SFX,          /**< Fire one-shot or looping sound effect. */
    AUDIO_EVENT_STOP_SFX,          /**< Stop active sound effect instance. */
    AUDIO_EVENT_PLAY_BGM,          /**< Initiate BGM track playback. */
    AUDIO_EVENT_STOP_BGM,          /**< Halt current BGM track. */
    AUDIO_EVENT_CROSSFADE_BGM,     /**< Crossfade to a new musical theme. */
    AUDIO_EVENT_PLAY_AMBIENCE,     /**< Start environmental ambient soundscape. */
    AUDIO_EVENT_STOP_AMBIENCE,     /**< Halt environmental ambience. */
    AUDIO_EVENT_CROSSFADE_AMBIENCE,/**< Crossfade ambient loop to new biome. */
    AUDIO_EVENT_PLAY_STINGER,      /**< Play high-priority triumph/defeat stinger. */
    AUDIO_EVENT_SET_VOLUME,        /**< Set individual channel volume gain. */
    AUDIO_EVENT_MUTE_CHANNEL,      /**< Toggle channel mute state. */
    AUDIO_EVENT_STOP_ALL           /**< Emergency silence all channels. */
} audio_event_type_t;

/**
 * @enum audio_ducking_state
 * @brief Ducking attenuation states for audio channels.
 *
 * For Designers: Automatically lowers background music volume when important
 * speech or fanfare stingers play so they stand out clearly.
 *
 * For Programmers: Binary state indicating whether nominal gain or ducked multiplier applies.
 */
typedef enum audio_ducking_state {
    AUDIO_DUCKING_NONE = 0,        /**< Nominal full gain. */
    AUDIO_DUCKING_ATTENUATED = 1   /**< Ducked under high-priority stinger/voice. */
} audio_ducking_state_t;

/**
 * @struct audio_channel_config
 * @brief Configuration descriptor for a single mixing bus.
 *
 * For Designers: Default settings for a bus (like maximum simultaneous sounds and volume).
 *
 * For Programmers: Immutable definition loaded from engine/config/audio/channels.json.
 */
typedef struct audio_channel_config {
    audio_channel_t channel;            /**< Channel identifier enum. */
    const char     *name;               /**< Human-readable bus name (e.g. "bgm"). */
    float           default_volume;     /**< Baseline volume [0.0f, 1.0f]. */
    uint32_t        max_polyphony;      /**< Concurrency limit (1 to 16). */
    bool            is_ducking_source;  /**< If true, triggers ducking of other buses. */
    float           duck_ratio;         /**< Ducking multiplier when ducked [0.0f, 1.0f]. */
} audio_channel_config_t;

/**
 * @struct audio_channel_state
 * @brief Runtime state of an individual audio mixing bus.
 *
 * For Designers: Live volume, mute status, and voice count of a sound channel.
 *
 * For Programmers: Mutable mixer state updated during frame audio ticks.
 */
typedef struct audio_channel_state {
    float                 volume;        /**< Active volume [0.0f, 1.0f]. */
    bool                  muted;         /**< Mute toggle flag. */
    audio_ducking_state_t ducking;       /**< Current ducking status. */
    uint32_t              active_voices; /**< Current polyphony voice count. */
} audio_channel_state_t;

/**
 * @struct audio_event
 * @brief Zero-heap discrete sound event packet passed through circular ring buffer.
 *
 * For Designers: An individual command sent to the audio player (e.g. "play sword strike").
 *
 * For Programmers: Fixed-size event packet for lock-free FIFO ring buffer transmission.
 */
typedef struct audio_event {
    audio_event_type_t type;                             /**< Event command type. */
    audio_channel_t    channel;                          /**< Target mixing channel. */
    char               cue_id[AUDIO_MAX_CUE_ID_LEN];     /**< Sound cue or track ID string. */
    float              volume;                           /**< Instance gain [0.0f, 1.0f]. */
    float              pan;                              /**< Stereo panning [-1.0f (L), +1.0f (R)]. */
    float              pitch;                            /**< Pitch scale (1.0f nominal). */
    float              fade_duration_sec;                /**< Fade or crossfade duration in seconds. */
    bool               loop;                             /**< Loop playback flag. */
    uint64_t           timestamp_us;                     /**< Monotonic timestamp in microseconds. */
} audio_event_t;

/**
 * @struct audio_sfx_cue
 * @brief Sound effect cue manifest definition.
 *
 * For Designers: Catalog entry for a sound effect (e.g., volume, pan, cooldown, and asset path).
 *
 * For Programmers: Loaded from game/data/presentation/audio/sfx.json.
 */
typedef struct audio_sfx_cue {
    char            id[AUDIO_MAX_CUE_ID_LEN];          /**< Unique sound effect identifier. */
    char            asset_path[AUDIO_MAX_ASSET_PATH_LEN]; /**< Relative path to audio sample file. */
    audio_channel_t channel;                           /**< Target mixing channel. */
    float           default_volume;                    /**< Default playback gain [0.0f, 1.0f]. */
    float           default_pan;                       /**< Default stereo pan [-1.0f, +1.0f]. */
    float           pitch_variance;                    /**< Random pitch randomization range. */
    uint32_t        cooldown_ms;                       /**< Minimum interval between plays in ms. */
    bool            loop;                              /**< Continuous loop playback flag. */
} audio_sfx_cue_t;

/**
 * @struct audio_bgm_track
 * @brief Background music theme manifest definition.
 *
 * For Designers: Catalog entry for a musical piece, including loop points and crossfades.
 *
 * For Programmers: Loaded from game/data/presentation/audio/bgm.json.
 */
typedef struct audio_bgm_track {
    char  id[AUDIO_MAX_CUE_ID_LEN];          /**< Unique musical track identifier. */
    char  asset_path[AUDIO_MAX_ASSET_PATH_LEN]; /**< Relative path to music file. */
    float default_volume;                    /**< Default playback gain [0.0f, 1.0f]. */
    float intro_len_sec;                     /**< Length of intro before loop start. */
    float loop_start_sec;                    /**< Timestamp where music loops back to. */
    float loop_end_sec;                      /**< Timestamp where track reaches end of loop. */
    float crossfade_default_sec;             /**< Default transition crossfade in seconds. */
} audio_bgm_track_t;

/**
 * @struct audio_ambience_cue
 * @brief Background environmental ambience soundscape definition.
 *
 * For Designers: Environmental sound loops (like howling wind or cathedral whispers).
 *
 * For Programmers: Loaded from game/data/presentation/audio/ambience.json.
 */
typedef struct audio_ambience_cue {
    char  id[AUDIO_MAX_CUE_ID_LEN];          /**< Unique ambience cue identifier. */
    char  asset_path[AUDIO_MAX_ASSET_PATH_LEN]; /**< Relative path to ambience audio file. */
    float default_volume;                    /**< Default playback gain [0.0f, 1.0f]. */
    float crossfade_sec;                     /**< Crossfade duration when switching biomes. */
    bool  loop;                              /**< Continuous loop playback flag. */
} audio_ambience_cue_t;

/**
 * @struct audio_bus_stats
 * @brief Telemetry and diagnostics counters for the audio event bus.
 *
 * For Designers: Inspection stats showing total sounds played and if any sounds were dropped.
 *
 * For Programmers: Live diagnostic metrics queried via audio_bus_get_stats().
 */
typedef struct audio_bus_stats {
    uint64_t events_posted;        /**< Total events successfully enqueued. */
    uint64_t events_consumed;      /**< Total events popped by audio consumer. */
    uint32_t queue_high_watermark; /**< Peak queue depth observed. */
    uint32_t dropped_events;       /**< Queue overflow drop count (must be 0). */
} audio_bus_stats_t;

/**
 * @struct audio_bus_config
 * @brief Global mixer configuration passed during initialization.
 *
 * For Designers: Master volume sliders for all sound channels plus sound limiter switches.
 *
 * For Programmers: Loaded from engine/config/audio/settings.json.
 */
typedef struct audio_bus_config {
    float    master_volume;        /**< Master output gain [0.0f, 1.0f]. */
    float    bgm_volume;           /**< Background music volume [0.0f, 1.0f]. */
    float    ambience_volume;      /**< Environmental ambience volume [0.0f, 1.0f]. */
    float    sfx_combat_volume;    /**< Combat sound effects volume [0.0f, 1.0f]. */
    float    sfx_tactical_volume;  /**< Tactical grid sound effects volume [0.0f, 1.0f]. */
    float    ui_volume;            /**< Interface sound effects volume [0.0f, 1.0f]. */
    float    stinger_volume;       /**< Fanfare stinger volume [0.0f, 1.0f]. */
    uint32_t sample_rate_hz;       /**< Hardware sample rate (default 44100 Hz). */
    bool     limiter_enabled;      /**< Safety brickwall limiter preventing audio clipping. */
} audio_bus_config_t;

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an audio channel identifier is valid.
 *
 * @param[in] channel Channel enum value to inspect.
 * @return true if channel is within [0 .. AUDIO_CHANNEL_COUNT - 1].
 */
static inline bool audio_channel_is_valid(audio_channel_t channel) {
    return channel >= AUDIO_CHANNEL_MASTER && channel < AUDIO_CHANNEL_COUNT;
}

/**
 * @brief Predicate verifying if an audio event type is valid.
 *
 * @param[in] type Event type enum value to inspect.
 * @return true if type is within [AUDIO_EVENT_NONE .. AUDIO_EVENT_STOP_ALL].
 */
static inline bool audio_event_type_is_valid(audio_event_type_t type) {
    return type >= AUDIO_EVENT_NONE && type <= AUDIO_EVENT_STOP_ALL;
}

/**
 * @brief Predicate verifying if a volume gain value resides within canonical bounds [0.0f, 1.0f].
 *
 * @param[in] volume Float volume level to check.
 * @return true if volume is between 0.0f and 1.0f inclusive.
 */
static inline bool audio_volume_is_valid(float volume) {
    return volume >= 0.0f && volume <= 1.0f;
}

/**
 * @brief Predicate verifying if a stereo panning value resides within [-1.0f, +1.0f].
 *
 * @param[in] pan Float pan value to check.
 * @return true if pan is between -1.0f (left) and +1.0f (right) inclusive.
 */
static inline bool audio_pan_is_valid(float pan) {
    return pan >= -1.0f && pan <= 1.0f;
}

/*
================================================================================
BLOCK 3: BODY BLOCK / CORE LOGIC & BUSINESS FUNCTIONS
Stratum: Layer 2 (Audio & Acoustics) | Subsystem: Audio Engine Core Types
================================================================================
(Audio core types define purely static structures, enums, channel configurations,
and bounds-checking inlines. Operational audio bus event dispatching and mixer
control APIs are declared in audio/audio_bus.h and audio/audio_fallback.h.)
================================================================================
*/

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Audio & Acoustics) | Subsystem: Audio Engine Core Types
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
(Audio core types define purely static structures, enums, and bounds checking inlines.
Functional audits, tripwires, and preflight routines are declared in audio_bus.h
and audio_fallback.h.)
================================================================================
*/

/*
================================================================================
BLOCK 5: CLOSING BLOCK / EXECUTABLE PROCEDURES & LIFECYCLE
Stratum: Layer 2 (Audio & Acoustics) | Subsystem: Audio Engine Core Types
================================================================================
Mission: Execute operational pipelines, format ASCII diagnostic cards,
construct programmatic inspection objects, and manage subsystem lifecycles.
================================================================================
(Audio core types define purely static structures, enums, and inline predicates.
Subsystem lifecycle initialization, event queue flushing, and teardown
procedures are declared in audio/audio_bus.h.)
================================================================================
*/

#endif /* TENEBRIS_LUX_AUDIO_TYPES_H */

