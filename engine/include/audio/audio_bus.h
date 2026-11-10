/*
================================================================================
BLOCK 1: METADATA BLOCK
( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
================================================================================
@project       Tenebris and Lux
@file          audio_bus.h
@layer         Layer 0 / Layer 2: Audio Presentation Subsystem
@version       0.1.1-dev
@interface_rev Rev 5
@date          2026-11-08
@author        CreativeWorkzStudio LLC / ProfessorSeanEX
@repository    https://github.com/Novaking34/tenebris-lux
@system        Universal Audio Event Bus & Mixer Architecture
@role          Thread-safe, zero-heap event queue, volume mixing, routing, and settings profile loader.
@invariants    1. Zero dynamic heap allocations in Layer 0/Layer 2 (ALLOC_ZERO_HEAP).
               2. Strict ISO C11 adherence (-std=c11, -Wall -Wextra -Wpedantic).
               3. All enums and structs must have explicit non-anonymous tags.
               4. Fixed-size circular ring buffer with power-of-two mask (capacity 128).
               5. Strictly bounded volume levels [0.0f, 1.0f] with mute channel masking.
               6. Reentrant and deterministic event polling and dispatch.
               7. Data-driven configuration: loads channel mappings from engine/config/audio/.
               8. Universal 5-Block Lifecycle Protocol enforced across header and source.

PURPOSE & ARCHITECTURAL ROLE:
  Public API for the Tenebris and Lux Audio Event Bus. Dispatches audio cue
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

#ifndef TENEBRIS_LUX_AUDIO_BUS_H
#define TENEBRIS_LUX_AUDIO_BUS_H

/*
================================================================================
BLOCK 2: OPENING BLOCK / TYPES & DOMAIN ONTOLOGY
Stratum: Layer 0 / Layer 2: Audio Presentation Subsystem
================================================================================
Mission: Bus sizing limits, include ladder, fallback safety integration,
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
#include "audio/audio_types.h"
#include "audio/audio_fallback.h"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.1: Pre-Conditions & Buffer Limits
--------------------------------------------------------------------------------
Compile-time limits ensuring the audio bus never breaches memory contracts.
For Designers: Guarantees sound commands cannot overflow the queue and crash the game.
For Programmers: Enforces strict zero-heap bounds (ALLOC_ZERO_HEAP) in .bss.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.2: Manifest & Pre-Flight Hooks
--------------------------------------------------------------------------------
Pre-flight verification anchor for the Audio Event Bus engine.
--------------------------------------------------------------------------------
*/

/** Unique module identifier string for the Audio Event Bus & Mixer subsystem. */
#define AUDIO_BUS_MODULE_ID            "audio.bus"

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.3: Structural Entity & Object Definitions
--------------------------------------------------------------------------------
Primary structural ontology defined in audio/audio_types.h.
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 2.4: Pre-Conditional Helper Cogs & Non-Business Inlines
--------------------------------------------------------------------------------
Pure static inline validation predicates and non-business query helpers.
Guarantees zero heap, pure reentrancy, and fast pre-flight assertion checks.
--------------------------------------------------------------------------------
*/

/**
 * @brief Predicate verifying if an audio event packet is structurally well-formed.
 *
 * @param[in] ev Pointer to audio event packet to check.
 * @return true if ev is non-NULL, type is valid, and channel is valid.
 */
static inline bool audio_bus_is_valid_event(const audio_event_t *ev) {
    return (ev != NULL) && audio_event_type_is_valid(ev->type) && audio_channel_is_valid(ev->channel);
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

/**
 * @brief Checks whether the audio event bus is initialized and active.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if the sound system is running and ready to play music and effects.
 *
 * ### For Systems Programmers & Engine Architects:
 * Queries atomic s_audio_bus.initialized flag.
 *
 * @return true if initialized; else false.
 */
bool audio_bus_is_initialized(void);

/**
 * @brief Loads hardware audio channels profile from config domain (audio/channels.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Loads the default hardware volume levels and limits for sound channels from a configuration file.
 *
 * ### For Systems Programmers & Engine Architects:
 * Deserializes channels JSON profile and applies volume and ducking parameters across channels.
 *
 * @return true if successfully loaded; false if missing or malformed.
 */
bool audio_bus_load_channels(void);

/**
 * @brief Loads user audio settings profile from config domain (audio/settings.json).
 *
 * ### For Game Designers & Non-Programmers:
 * Reads user volume preferences (like music volume slider at 75%) from user options.
 *
 * ### For Systems Programmers & Engine Architects:
 * Parses settings JSON, applying volume sliders and mute toggles directly or into out_config.
 *
 * @param[out] out_config Destination configuration struct, or NULL to apply directly.
 * @return true if successfully loaded; false if missing or malformed.
 */
bool audio_bus_load_settings(audio_bus_config_t *out_config);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.2: Audio Event Posting & Queue Dispatch Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Enqueues a raw discrete audio event into the circular ring buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Sends an audio command (like playing a sound or crossfading music) to the sound engine.
 *
 * ### For Systems Programmers & Engine Architects:
 * Enqueues audio_event_t packet into zero-heap static ring buffer with overflow protection.
 *
 * @param[in] event Event packet to post.
 * @return true if enqueued; false if ring buffer is full or invalid.
 */
bool audio_bus_post_event(const audio_event_t *event);

/**
 * @brief Convenience helper to fire a sound effect cue.
 *
 * ### For Game Designers & Non-Programmers:
 * Plays a sound effect (like a sword swing or shield block) with volume and left/right stereo position.
 *
 * ### For Systems Programmers & Engine Architects:
 * Constructs AUDIO_EVENT_PLAY_SFX event with clamped gain and pan, timestamping via monotonic clock.
 *
 * @param[in] cue_id Symbolic sound cue ID (e.g., "sfx_ward_lux").
 * @param[in] volume Instance gain [0.0f, 1.0f] (or <0.0f for default).
 * @param[in] pan    Stereo panning [-1.0f (L), +1.0f (R)].
 * @return true if enqueued; else false.
 */
bool audio_bus_post_sfx(const char *cue_id, float volume, float pan);

/**
 * @brief Convenience helper to initiate or crossfade background music.
 *
 * ### For Game Designers & Non-Programmers:
 * Starts playing a background music track, smoothly fading between songs if desired.
 *
 * ### For Systems Programmers & Engine Architects:
 * Dispatches AUDIO_EVENT_PLAY_BGM or AUDIO_EVENT_CROSSFADE_BGM to BGM channel.
 *
 * @param[in] track_id      Symbolic BGM track ID.
 * @param[in] crossfade_sec Fade transition length in seconds.
 * @return true if enqueued; else false.
 */
bool audio_bus_post_bgm(const char *track_id, float crossfade_sec);

/**
 * @brief Convenience helper to set ambient environmental soundscape.
 *
 * ### For Game Designers & Non-Programmers:
 * Sets the background ambient sounds (such as wind howling or forest creatures).
 *
 * ### For Systems Programmers & Engine Architects:
 * Dispatches ambience event packet to Ambience audio channel.
 *
 * @param[in] ambience_id   Symbolic ambience soundscape ID.
 * @param[in] crossfade_sec Fade transition length in seconds.
 * @return true if enqueued; else false.
 */
bool audio_bus_post_ambience(const char *ambience_id, float crossfade_sec);

/**
 * @brief Convenience helper to trigger a high-priority triumph/defeat stinger.
 *
 * ### For Game Designers & Non-Programmers:
 * Plays a dramatic musical fanfare (such as victory trumpets or boss defeat chords).
 *
 * ### For Systems Programmers & Engine Architects:
 * Dispatches high-priority stinger event with ducking priority over background music.
 *
 * @param[in] stinger_id Symbolic stinger ID.
 * @param[in] volume     Instance gain [0.0f, 1.0f].
 * @return true if enqueued; else false.
 */
bool audio_bus_post_stinger(const char *stinger_id, float volume);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.3: Queue Polling & Event Consumption Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Pops the oldest pending audio event from the circular ring buffer.
 *
 * ### For Game Designers & Non-Programmers:
 * Retrieves the next sound command that needs to be played by the sound card.
 *
 * ### For Systems Programmers & Engine Architects:
 * Dequeues event from ring buffer head, advancing head index with power-of-two mask.
 *
 * @param[out] out_event Buffer to receive popped event.
 * @return true if an event was popped; false if queue is empty.
 */
bool audio_bus_poll_event(audio_event_t *out_event);

/**
 * @brief Inspects the oldest pending event without removing it from queue.
 *
 * ### For Game Designers & Non-Programmers:
 * Peeks at the next sound event in line without playing it yet.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies head event without advancing head pointer.
 *
 * @param[out] out_event Buffer to receive peeked event.
 * @return true if queue has pending events; else false.
 */
bool audio_bus_peek_event(audio_event_t *out_event);

/**
 * @brief Returns number of pending events currently in the queue.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks how many sounds are waiting to be played.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns queue count atomic counter.
 */
uint32_t audio_bus_pending_count(void);

/**
 * @brief Discards all pending events in the queue.
 *
 * ### For Game Designers & Non-Programmers:
 * Clears out any queued sounds instantly (e.g. during a scene change or cutscene skip).
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets head, tail, and count indices to 0.
 */
void audio_bus_clear_events(void);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.4: Channel Mixing & Volume Control Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Adjusts volume gain for a specific mixing channel.
 *
 * ### For Game Designers & Non-Programmers:
 * Changes the volume of a specific sound category (like turning down voices or turning up music).
 *
 * ### For Systems Programmers & Engine Architects:
 * Updates channel volume slider with [0.0f, 1.0f] clamping.
 *
 * @param[in] channel Channel to adjust.
 * @param[in] volume  New volume gain [0.0f, 1.0f].
 */
void audio_bus_set_channel_volume(audio_channel_t channel, float volume);

/**
 * @brief Queries current volume gain for a channel.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks the current loudness setting of a sound channel.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns floating-point gain factor [0.0f, 1.0f].
 *
 * @param[in] channel Channel to inspect.
 * @return Volume gain [0.0f, 1.0f].
 */
float audio_bus_get_channel_volume(audio_channel_t channel);

/**
 * @brief Mutes or unmutes a specific mixing channel.
 *
 * ### For Game Designers & Non-Programmers:
 * Silences or re-enables a specific sound channel.
 *
 * ### For Systems Programmers & Engine Architects:
 * Toggles muted boolean flag on target channel descriptor.
 *
 * @param[in] channel Channel to adjust.
 * @param[in] muted   true to mute, false to unmute.
 */
void audio_bus_set_channel_muted(audio_channel_t channel, bool muted);

/**
 * @brief Queries whether a channel is currently muted.
 *
 * ### For Game Designers & Non-Programmers:
 * Checks if a sound category is currently silenced.
 *
 * ### For Systems Programmers & Engine Architects:
 * Returns channel muted boolean flag.
 */
bool audio_bus_is_channel_muted(audio_channel_t channel);

/*
--------------------------------------------------------------------------------
Sub-Assembly 3.5: Telemetry & Bus Statistics Cogs
--------------------------------------------------------------------------------
*/

/**
 * @brief Queries telemetry statistics from the audio event bus.
 *
 * ### For Game Designers & Non-Programmers:
 * Gets a report on how many sounds have played, dropped, or queued up.
 *
 * ### For Systems Programmers & Engine Architects:
 * Copies audio_bus_stats_t struct into out_stats.
 *
 * @param[out] out_stats Buffer to receive current stats.
 */
void audio_bus_get_stats(audio_bus_stats_t *out_stats);

/**
 * @brief Resets telemetry statistics counters.
 *
 * ### For Game Designers & Non-Programmers:
 * Clears the audio statistics counters.
 *
 * ### For Systems Programmers & Engine Architects:
 * Zeros out all fields in internal s_audio_bus.stats structure.
 */
void audio_bus_reset_stats(void);

/*
================================================================================
BLOCK 4: AUDIT PROCEDURES BLOCK
Stratum: Layer 2 (Audio & Acoustics) | Subsystem: Audio Event Bus Orchestrator
================================================================================
Mission: Verify incoming parameters, contracts, and schema invariants;
validate standalone function execution and system stability for Block 5.
================================================================================
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.1: Contract, Input & Cog Verification Cogs
Mission: Verify received parameters, configuration contracts, and data provenance;
verify that declared subsystem cogs run properly as standalone units.
--------------------------------------------------------------------------------
(Mixing channel volume bounds, stereo pan limits, and audio event type integrity
are verified via pre-flight checks and standalone unit cog validations.)
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
Sub-Assembly 4.2: Machine Self-Audit & Pre-Flight Validation Tripwires
Mission: Validate holistic system invariants, tripwires, and subsystem stability;
provide pre-flight clearance handshake before advancing to Block 5 execution.
--------------------------------------------------------------------------------
*/

/**
 * @brief Executes machine pre-flight diagnostic checks on audio bus invariants.
 *
 * ### For Game Designers & Non-Programmers:
 * Rapid check ensuring sound channel counts, mixing thresholds, and event queues
 * satisfy engine physical limits before sound playback begins.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates channel configuration tables, volume clamping ranges, polyphony limits,
 * queue capacity invariants, and zero-heap event buffer alignment.
 *
 * @return diag_result_t reporting categorized diagnostic outcome and severity.
 */
diag_result_t audio_bus_preflight(void);

/**
 * @brief High-level boolean self-audit wrapper for standalone test harnesses.
 *
 * ### For Game Designers & Non-Programmers:
 * Tests the game's sound mixer before audio plays to make sure sound channels,
 * volume limits, and playback queues are working cleanly without distortion.
 *
 * ### For Systems Programmers & Engine Architects:
 * Validates bus initialization, channel volume clamping, event FIFO posting
 * and polling, queue overflow resilience, and channel configuration defaults.
 *
 * @return true if all axiomatic invariants pass; false otherwise.
 */
bool audio_bus_audit(void);

/**
 * @brief Legacy preflight audit alias for backward compatibility.
 *
 * ### For Game Designers & Non-Programmers:
 * Backward-compatible sound mixer health check.
 *
 * ### For Systems Programmers & Engine Architects:
 * Forwards directly to audio_bus_audit().
 *
 * DELETE ONCE NEW SYSTEM IS UP TO DATE
 *
 * @return true if all axiomatic invariants pass; false otherwise.
 */
bool audio_bus_preflight_audit(void);

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
Mission: Run operational execution procedures, format terminal-friendly ASCII
status cards, and construct structured telemetry inspection objects.
--------------------------------------------------------------------------------
*/

/**
 * @brief Constructs a structured runtime audio telemetry report object.
 *
 * ### For Game Designers & Non-Programmers:
 * Collects a summary of sound playback performance and queue health.
 *
 * ### For Systems Programmers & Engine Architects:
 * Programmatic telemetry inspection copying live bus stats into audio_bus_stats_t.
 *
 * @param[out] out_stats Destination statistics report object.
 * @return true if statistics were populated; false on NULL argument.
 */
bool audio_bus_execute_inspect(audio_bus_stats_t *out_stats);

/**
 * @brief Formats an ASCII diagnostic card summarizing audio event bus telemetry.
 *
 * ### For Game Designers & Non-Programmers:
 * Displays a clean text card showing how many sounds played, queue fullness, and health.
 *
 * ### For Systems Programmers & Engine Architects:
 * Formats 60-column terminal ASCII status card of audio_bus_stats_t without dynamic heap allocation.
 *
 * @param[in]  stats    Optional pointer to stats to format, or NULL to inspect live bus.
 * @param[out] out_buf  Destination character buffer.
 * @param[in]  buf_size Size of destination buffer in bytes.
 * @return Number of characters written (excluding null terminator), or 0 on error.
 */
size_t audio_bus_execute_dump_card(const audio_bus_stats_t *stats,
                                   char *out_buf,
                                   size_t buf_size);

/*
--------------------------------------------------------------------------------
Sub-Assembly 5.2: Subsystem Lifecycle Initialization & Teardown
Mission: Manage subsystem lifecycle, allocate runtime ring buffers,
open/close file streams, and tear down hardware handles.
--------------------------------------------------------------------------------
*/

/**
 * @brief Initializes the global audio event bus and default channel volumes.
 *
 * ### For Game Designers & Non-Programmers:
 * Initializes sound channels, volume levels, and audio queues at game boot.
 *
 * ### For Systems Programmers & Engine Architects:
 * Resets lock-free FIFO event ring, applies channel configs, and zeroes stats.
 *
 * @param[in] config Pointer to initialization config, or NULL for defaults.
 */
void audio_bus_init(const audio_bus_config_t *config);

/**
 * @brief Shuts down the audio bus, clearing active channels and event queue.
 *
 * ### For Game Designers & Non-Programmers:
 * Silences all sounds and safely shuts down audio systems when the game closes.
 *
 * ### For Systems Programmers & Engine Architects:
 * Flushes event queue, resets active channels, and zeroes mixer memory.
 */
void audio_bus_shutdown(void);

#endif /* TENEBRIS_LUX_AUDIO_BUS_H */

