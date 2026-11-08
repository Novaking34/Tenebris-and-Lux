/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          audio_bus.h
@layer         Layer 0 / Layer 2: Audio Presentation Subsystem
@version       0.1.0-dev
@interface_rev Rev 4
@date          2026-11-07
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Thread-safe, zero-heap event queue, volume mixing, and routing.
@invariants    1. Zero dynamic heap allocations (ALLOC_ZERO_HEAP).
               2. All enums and structs must have explicit non-anonymous tags.
               3. Fixed-size circular ring buffer with power-of-two mask.
               4. Strictly bounded volume levels [0.0f, 1.0f].
               5. Reentrant and deterministic event polling.

PURPOSE & ARCHITECTURAL ROLE:
  Public API for the Tenebris and Lux Audio Event Bus. Dispatches audio cue
  triggers from simulation turns, combat actions, and GUI widgets into a
  zero-heap circular ring buffer for decoupled consumption by the audio driver.
================================================================================
*/

#ifndef TENEBRIS_LUX_AUDIO_BUS_H
#define TENEBRIS_LUX_AUDIO_BUS_H

/*
================================================================================
BLOCK 2: OPENING / TYPES & DOMAIN ONTOLOGY BLOCK
================================================================================
*/

#include "audio/audio_types.h"
#include <stdbool.h>
#include <stdint.h>

/*
================================================================================
BLOCK 3: BODY BLOCK / API DECLARATIONS
================================================================================
*/

/**
 * Initializes the global audio event bus and default channel volumes.
 *
 * @param[in] config Pointer to initialization config, or NULL for defaults.
 */
void audio_bus_init(const audio_bus_config_t *config);

/**
 * Shuts down the audio bus, clearing active channels and event queue.
 */
void audio_bus_shutdown(void);

/**
 * Checks whether audio bus is initialized.
 *
 * @return true if initialized; else false.
 */
bool audio_bus_is_initialized(void);

/**
 * Enqueues a raw discrete audio event into the circular ring buffer.
 *
 * @param[in] event Event packet to post.
 * @return true if enqueued; false if ring buffer is full or invalid.
 */
bool audio_bus_post_event(const audio_event_t *event);

/**
 * Convenience helper to fire a sound effect cue.
 *
 * @param[in] cue_id Symbolic sound cue ID (e.g., "sfx_ward_lux").
 * @param[in] volume Instance gain [0.0f, 1.0f] (or <0.0f for default).
 * @param[in] pan    Stereo panning [-1.0f (L), +1.0f (R)].
 * @return true if enqueued; else false.
 */
bool audio_bus_post_sfx(const char *cue_id, float volume, float pan);

/**
 * Convenience helper to initiate or crossfade background music.
 *
 * @param[in] track_id      Symbolic BGM track ID.
 * @param[in] crossfade_sec Fade transition length in seconds.
 * @return true if enqueued; else false.
 */
bool audio_bus_post_bgm(const char *track_id, float crossfade_sec);

/**
 * Convenience helper to set ambient environmental soundscape.
 *
 * @param[in] ambience_id   Symbolic ambience soundscape ID.
 * @param[in] crossfade_sec Fade transition length in seconds.
 * @return true if enqueued; else false.
 */
bool audio_bus_post_ambience(const char *ambience_id, float crossfade_sec);

/**
 * Convenience helper to trigger a high-priority triumph/defeat stinger.
 *
 * @param[in] stinger_id Symbolic stinger ID.
 * @param[in] volume     Instance gain [0.0f, 1.0f].
 * @return true if enqueued; else false.
 */
bool audio_bus_post_stinger(const char *stinger_id, float volume);

/**
 * Pops the oldest pending audio event from the circular ring buffer.
 *
 * @param[out] out_event Buffer to receive popped event.
 * @return true if an event was popped; false if queue is empty.
 */
bool audio_bus_poll_event(audio_event_t *out_event);

/**
 * Inspects the oldest pending event without removing it from queue.
 *
 * @param[out] out_event Buffer to receive peeked event.
 * @return true if queue has pending events; else false.
 */
bool audio_bus_peek_event(audio_event_t *out_event);

/**
 * Returns number of pending events currently in the queue.
 */
uint32_t audio_bus_pending_count(void);

/**
 * Discards all pending events in the queue.
 */
void audio_bus_clear_events(void);

/**
 * Adjusts volume gain for a specific mixing channel.
 *
 * @param[in] channel Channel to adjust.
 * @param[in] volume  New volume gain [0.0f, 1.0f].
 */
void audio_bus_set_channel_volume(audio_channel_t channel, float volume);

/**
 * Queries current volume gain for a channel.
 *
 * @param[in] channel Channel to inspect.
 * @return Volume gain [0.0f, 1.0f].
 */
float audio_bus_get_channel_volume(audio_channel_t channel);

/**
 * Mutes or unmutes a specific mixing channel.
 *
 * @param[in] channel Channel to adjust.
 * @param[in] muted   true to mute, false to unmute.
 */
void audio_bus_set_channel_muted(audio_channel_t channel, bool muted);

/**
 * Queries whether a channel is currently muted.
 */
bool audio_bus_is_channel_muted(audio_channel_t channel);

/**
 * Queries telemetry statistics from the audio event bus.
 *
 * @param[out] out_stats Buffer to receive current stats.
 */
void audio_bus_get_stats(audio_bus_stats_t *out_stats);

/**
 * Resets telemetry statistics counters.
 */
void audio_bus_reset_stats(void);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
================================================================================
*/

/**
 * Preflight invariant verification audit for the audio event bus.
 *
 * @return true if all axiomatic invariants pass; false otherwise.
 */
bool audio_bus_preflight_audit(void);

/*
================================================================================
BLOCK 5: CLOSING BLOCK
================================================================================
*/

#endif /* TENEBRIS_LUX_AUDIO_BUS_H */

