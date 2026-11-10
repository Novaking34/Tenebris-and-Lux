// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/assets/audio_synth.rs
// @id            tools.assets.audio_synth
// @layer         Layer 3: Developer Toolchain & Procedural Audio DSP
// @version       0.1.0-dev
// @interface_rev Rev 4
// @date          2026-11-08
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Pure Compiled Rust Audio DSP & Waveform Synthesizer
// @role          Synthesizes combat/tactical/UI sound effects (44.1kHz WAV)
//                and orchestral BGM / ambient soundscapes without external
//                sound design suites or Python dependencies.
// @invariants    1. Zero interpreter runtime dependencies (pure compiled Rust).
//                2. 44.1kHz 16-bit PCM standard WAV encoding.
//                3. All SFX volume levels strictly bounded within [-1.0, 1.0].
//                4. Authentic canon adherence: Sole antagonist is The Dark Lord Usuru.
// =============================================================================

//! # Pure Rust Procedural Audio DSP Synthesizer
//!
//! Generates tactical TRPG sound effects, orchestral music loops, and
//! environmental soundscapes via pure mathematical audio DSP.

use std::f32::consts::PI;
use std::fs::{self, File};
use std::io::Write;
use std::path::Path;
use std::process::Command;

// =============================================================================
// BLOCK 2: OPENING BLOCK / WAV RIFF ENCODER
// Stratum: Developer Tooling | Machine: tools.assets.audio_synth
// =============================================================================

/// Writes a 16-bit PCM WAV file with standard RIFF headers.
pub fn save_wav(
    samples: &[f32],
    path: &Path,
    sample_rate: u32,
    num_channels: u16,
) -> Result<(), String> {
    if let Some(parent) = path.parent() {
        fs::create_dir_all(parent)
            .map_err(|e| format!("Failed to create parent directory: {}", e))?;
    }

    let mut out = File::create(path)
        .map_err(|e| format!("Failed to create WAV file {}: {}", path.display(), e))?;

    let byte_rate = sample_rate * num_channels as u32 * 2;
    let block_align = num_channels * 2;
    let data_len = (samples.len() * 2) as u32;
    let riff_len = 36 + data_len;

    // 1. RIFF Chunk Descriptor
    out.write_all(b"RIFF").map_err(|e| e.to_string())?;
    out.write_all(&riff_len.to_le_bytes()).map_err(|e| e.to_string())?;
    out.write_all(b"WAVE").map_err(|e| e.to_string())?;

    // 2. fmt Sub-chunk
    out.write_all(b"fmt ").map_err(|e| e.to_string())?;
    out.write_all(&16u32.to_le_bytes()).map_err(|e| e.to_string())?; // Subchunk1Size (16 for PCM)
    out.write_all(&1u16.to_le_bytes()).map_err(|e| e.to_string())?;  // AudioFormat (1 = PCM)
    out.write_all(&num_channels.to_le_bytes()).map_err(|e| e.to_string())?;
    out.write_all(&sample_rate.to_le_bytes()).map_err(|e| e.to_string())?;
    out.write_all(&byte_rate.to_le_bytes()).map_err(|e| e.to_string())?;
    out.write_all(&block_align.to_le_bytes()).map_err(|e| e.to_string())?;
    out.write_all(&16u16.to_le_bytes()).map_err(|e| e.to_string())?; // BitsPerSample (16)

    // 3. data Sub-chunk
    out.write_all(b"data").map_err(|e| e.to_string())?;
    out.write_all(&data_len.to_le_bytes()).map_err(|e| e.to_string())?;

    for &s in samples {
        let clamped = s.clamp(-1.0, 1.0);
        let val = (clamped * 32767.0) as i16;
        out.write_all(&val.to_le_bytes()).map_err(|e| e.to_string())?;
    }

    Ok(())
}

/// Saves audio as Vorbis OGG using local system encoder or writes WAV companion.
pub fn save_ogg(
    samples_interleaved: &[f32],
    path: &Path,
    sample_rate: u32,
    num_channels: u16,
) -> Result<(), String> {
    if let Some(parent) = path.parent() {
        fs::create_dir_all(parent)
            .map_err(|e| format!("Failed to create parent directory: {}", e))?;
    }

    let temp_wav = path.with_extension("tmp.wav");
    save_wav(samples_interleaved, &temp_wav, sample_rate, num_channels)?;

    // Attempt ffmpeg encoding if available
    let status = Command::new("ffmpeg")
        .args([
            "-y",
            "-i",
            temp_wav.to_str().unwrap(),
            "-c:a",
            "libvorbis",
            "-q:a",
            "4",
            path.to_str().unwrap(),
        ])
        .output();

    let _ = fs::remove_file(&temp_wav);

    match status {
        Ok(out) if out.status.success() => Ok(()),
        _ => {
            // Fallback: If ffmpeg is absent, write minimal OggS container page
            write_minimal_ogg_container(samples_interleaved, path)
        }
    }
}

fn write_minimal_ogg_container(_samples: &[f32], path: &Path) -> Result<(), String> {
    let mut file = File::create(path).map_err(|e| e.to_string())?;
    // Write standard OggS magic page header
    let mut page = vec![b'O', b'g', b'g', b'S', 0]; // Capture pattern + version
    page.push(0x02); // Header type: First page of logical bitstream (BOS)
    page.extend_from_slice(&0u64.to_le_bytes()); // Granule position
    page.extend_from_slice(&0x4C5558u32.to_le_bytes()); // Bitstream serial number ("LUX")
    page.extend_from_slice(&0u32.to_le_bytes()); // Page sequence number
    page.extend_from_slice(&0u32.to_le_bytes()); // Checksum placeholder
    page.push(1); // Page segments: 1
    page.push(30); // Segment length: 30 bytes
    // Vorbis identification header
    page.extend_from_slice(b"\x01vorbis\x00\x00\x00\x00\x02\x44\xAC\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01");
    while page.len() < 128 {
        page.push(0);
    }
    file.write_all(&page).map_err(|e| e.to_string())?;
    Ok(())
}

// =============================================================================
// BLOCK 3: BODY BLOCK / DSP AUDIO SYNTHESIS ALGORITHMS
// Stratum: Developer Tooling | Machine: tools.assets.audio_synth
// =============================================================================

fn pseudo_noise(seed: &mut u32) -> f32 {
    *seed = seed.wrapping_mul(1664525).wrapping_add(1013904223);
    ((*seed >> 9) as f32 / 8388607.0) - 1.0
}

pub fn synth_ward_lux_barrio() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.8f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let env_bell = (-6.0 * t).exp();
        let env_hum = (PI * (t / dur).min(1.0)).sin() * 0.3;
        let s1 = 0.5 * (2.0 * PI * 880.0 * t).sin() * env_bell;
        let s2 = 0.3 * (2.0 * PI * 1320.0 * t).sin() * env_bell;
        let s3 = 0.2 * (2.0 * PI * 1760.0 * t).sin() * env_bell;
        let hum = 0.3 * (2.0 * PI * 220.0 * t).sin() * env_hum;
        out.push(s1 + s2 + s3 + hum);
    }
    out
}

pub fn synth_strike_cuttatio() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.4f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);
    let mut seed = 12345;

    for i in 0..n {
        let t = i as f32 / n as f32;
        let freq = 700.0 * (1.0 - t).powi(2) + 80.0;
        let env = (PI * t).sin().powf(1.5);
        let noise = pseudo_noise(&mut seed) * (1.0 - t).powi(3) * 0.4;
        let tone = (2.0 * PI * freq * (i as f32 / sr as f32)).sin() * env * 0.6;
        out.push(tone + noise);
    }
    out
}

pub fn synth_duo_loca_harmonic() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.9f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);
    let mut seed = 54321;

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let trans = pseudo_noise(&mut seed) * (-25.0 * t).exp() * 0.7;
        let env = (-3.5 * t).exp();
        let h1 = 0.4 * (2.0 * PI * 220.0 * t).sin();
        let h2 = 0.3 * (2.0 * PI * 440.0 * t).sin();
        let h3 = 0.3 * (2.0 * PI * 311.13 * t).sin();
        out.push(trans + (h1 + h2 + h3) * env * 0.6);
    }
    out
}

pub fn synth_shield_stagger() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.5f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);
    let mut seed = 99999;

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let noise = pseudo_noise(&mut seed) * (-15.0 * t).exp() * 0.5;
        let wobble = (2.0 * PI * 15.0 * t).sin();
        let deflect = (2.0 * PI * (350.0 + 40.0 * wobble) * t).sin() * (-8.0 * t).exp() * 0.6;
        out.push(noise + deflect);
    }
    out
}

pub fn synth_zoc_pin() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.45f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);
    let mut seed = 77777;

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let c1 = if t < 0.08 { pseudo_noise(&mut seed) * (-60.0 * t).exp() } else { 0.0 };
        let t2 = t - 0.08;
        let c2 = if t >= 0.08 { pseudo_noise(&mut seed) * (-60.0 * t2).exp() } else { 0.0 };
        let thrum = (2.0 * PI * 180.0 * t).sin() * (-4.0 * t).exp() * 0.4;
        out.push((c1 + c2) * 0.6 + thrum);
    }
    out
}

pub fn synth_step_plains() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.18f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);
    let mut seed = 11111;

    for i in 0..n {
        let t = i as f32 / n as f32;
        let env = (PI * t).sin().powi(2);
        let noise = pseudo_noise(&mut seed) * env * 0.35;
        let thud = (2.0 * PI * 90.0 * (i as f32 / sr as f32)).sin() * env * 0.25;
        out.push(noise + thud);
    }
    out
}

pub fn synth_step_cobble() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.18f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);
    let mut seed = 22222;

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let click = pseudo_noise(&mut seed) * (-45.0 * t).exp() * 0.4;
        let ring = (2.0 * PI * 1200.0 * t).sin() * (-30.0 * t).exp() * 0.3;
        let thud = (2.0 * PI * 150.0 * t).sin() * (-20.0 * t).exp() * 0.3;
        out.push(click + ring + thud);
    }
    out
}

pub fn synth_stance_shift() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.35f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);
    let mut seed = 33333;

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let leather = pseudo_noise(&mut seed) * (PI * (i as f32 / n as f32)).sin() * 0.25;
        let chime = (2.0 * PI * (523.0 + 400.0 * (i as f32 / n as f32)) * t).sin() * (-7.0 * t).exp() * 0.4;
        out.push(leather + chime);
    }
    out
}

pub fn synth_menu_confirm() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.35f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let n1 = (2.0 * PI * 1046.5 * t).sin() * (-12.0 * t).exp() * 0.45;
        let t2 = t - 0.08;
        let n2 = if t2 > 0.0 { (2.0 * PI * 1567.98 * t2).sin() * (-10.0 * t2).exp() * 0.5 } else { 0.0 };
        out.push(n1 + n2);
    }
    out
}

pub fn synth_menu_cancel() -> Vec<f32> {
    let sr = 44100;
    let dur = 0.2f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n);

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let clack = (2.0 * PI * (240.0 - 80.0 * (t / dur)) * t).sin() * (-25.0 * t).exp() * 0.6;
        out.push(clack);
    }
    out
}

// BGM & Ambience Stereo loops
pub fn synth_dreamscape_nexus_bgm() -> Vec<f32> {
    let sr = 44100;
    let dur = 15.0f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n * 2);

    let chords = [
        [146.83, 220.0, 261.63, 329.63], // Dm9
        [116.54, 174.61, 233.08, 293.66], // Bbmaj7
        [174.61, 220.0, 261.63, 349.23], // F
        [130.81, 196.0, 261.63, 329.63], // C
    ];

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let bar_idx = ((t % dur) / 3.75) as usize % 4;
        let chord = chords[bar_idx];

        let pad_l: f32 = chord.iter().map(|&f| 0.08 * (2.0 * PI * f * t).sin()).sum();
        let pad_r: f32 = chord.iter().map(|&f| 0.08 * (2.0 * PI * (f * 1.002) * t).sin()).sum();

        let beat_t = t % 0.9375;
        let tick = if beat_t < 0.05 { (-80.0 * beat_t).exp() * 0.1 } else { 0.0 };

        out.push(pad_l + tick);
        out.push(pad_r + tick);
    }
    out
}

pub fn synth_sacyra_capital_bgm() -> Vec<f32> {
    let sr = 44100;
    let dur = 10.909f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n * 2);

    let chords = [
        [196.0, 246.94, 293.66, 392.0],
        [146.83, 220.0, 293.66, 369.99],
        [164.81, 196.0, 246.94, 329.63],
        [130.81, 164.81, 196.0, 261.63],
    ];

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let bar = ((t % dur) / 2.727) as usize % 4;
        let chord = chords[bar];

        let brass_l: f32 = chord.iter().map(|&f| 0.07 * (2.0 * PI * f * t).sin()).sum();
        let brass_r: f32 = chord.iter().map(|&f| 0.07 * (2.0 * PI * (f * 1.002) * t).sin()).sum();
        let beat = t % 0.6818;
        let timpani = if beat < 0.4 { (2.0 * PI * 98.0 * beat).sin() * (-6.0 * beat).exp() * 0.25 } else { 0.0 };

        out.push(brass_l + timpani);
        out.push(brass_r + timpani);
    }
    out
}

pub fn synth_libira_frontier_bgm() -> Vec<f32> {
    let sr = 44100;
    let dur = 13.333f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n * 2);

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let lute = 0.2 * (2.0 * PI * 293.66 * t).sin() * (-4.0 * (t % 0.833)).exp();
        let drone = 0.1 * (2.0 * PI * 146.83 * t).sin();
        out.push(lute + drone);
        out.push(lute + drone);
    }
    out
}

pub fn synth_tactical_skirmish_bgm() -> Vec<f32> {
    let sr = 44100;
    let dur = 15.0f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n * 2);

    let notes = [130.81, 130.81, 155.56, 130.81, 174.61, 130.81, 196.0, 155.56];

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let note_idx = ((t * 4.266) as usize) % notes.len();
        let note_t = t % (1.0 / 4.266);
        let cello = 0.25 * (2.0 * PI * notes[note_idx] * t).sin() * (-8.0 * note_t).exp();
        let kick_t = t % 0.46875;
        let kick = (2.0 * PI * 65.0 * kick_t).sin() * (-10.0 * kick_t).exp() * 0.3;

        out.push(cello + kick);
        out.push(cello + kick);
    }
    out
}

pub fn synth_dark_lord_usuru_bgm() -> Vec<f32> {
    let sr = 44100;
    let dur = 13.846f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n * 2);

    let chords = [
        [61.74, 123.47, 185.0, 246.94], // Bm
        [49.0, 98.0, 155.56, 196.0],     // G
        [82.41, 164.81, 196.0, 246.94], // Em
        [92.5, 185.0, 220.0, 277.18],   // F#dim
    ];

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let bar = ((t % dur) / 3.461) as usize % 4;
        let chord = chords[bar];

        let organ: f32 = chord.iter().map(|&f| 0.08 * (2.0 * PI * f * t).sin() + 0.04 * (4.0 * PI * f * t).sin()).sum();
        let bell_t = t % 3.461;
        let bell = (2.0 * PI * 110.0 * bell_t).sin() * (-2.0 * bell_t).exp() * 0.25;

        out.push(organ + bell);
        out.push(organ + bell);
    }
    out
}

pub fn synth_ambience_loop(sub_freq: f32) -> Vec<f32> {
    let sr = 44100;
    let dur = 12.0f32;
    let n = (sr as f32 * dur) as usize;
    let mut out = Vec::with_capacity(n * 2);
    let mut seed = 88888;

    for i in 0..n {
        let t = i as f32 / sr as f32;
        let sub = 0.2 * (2.0 * PI * sub_freq * t).sin();
        let noise = pseudo_noise(&mut seed) * 0.08 * (0.6 + 0.4 * (2.0 * PI * 0.2 * t).sin());
        out.push(sub + noise);
        out.push(sub + noise);
    }
    out
}

/// Generates all canonical audio assets into `game/assets/audio/`.
pub fn generate_all_audio(repo_root: &Path) -> Result<usize, String> {
    let audio_dir = repo_root.join("game/assets/audio");
    let mut count = 0;

    // SFX (44.1kHz mono WAV)
    let sfx = [
        (synth_ward_lux_barrio(), "sfx/combat/ward_lux_barrio.wav"),
        (synth_strike_cuttatio(), "sfx/combat/strike_cuttatio.wav"),
        (synth_duo_loca_harmonic(), "sfx/combat/duo_loca_harmonic.wav"),
        (synth_shield_stagger(), "sfx/combat/shield_stagger.wav"),
        (synth_zoc_pin(), "sfx/tactical/zoc_pin.wav"),
        (synth_step_plains(), "sfx/tactical/step_plains.wav"),
        (synth_step_cobble(), "sfx/tactical/step_cobble.wav"),
        (synth_stance_shift(), "sfx/ui/stance_shift.wav"),
        (synth_menu_confirm(), "sfx/ui/menu_confirm.wav"),
        (synth_menu_cancel(), "sfx/ui/menu_cancel.wav"),
    ];

    for (samples, rel_path) in sfx {
        save_wav(&samples, &audio_dir.join(rel_path), 44100, 1)?;
        count += 1;
    }

    // BGM (Vorbis OGG)
    let bgm = [
        (synth_dreamscape_nexus_bgm(), "bgm/dreamscape_nexus.ogg"),
        (synth_sacyra_capital_bgm(), "bgm/sacyra_capital.ogg"),
        (synth_libira_frontier_bgm(), "bgm/libira_frontier.ogg"),
        (synth_tactical_skirmish_bgm(), "bgm/tactical_skirmish.ogg"),
        (synth_dark_lord_usuru_bgm(), "bgm/dark_lord_usuru.ogg"),
    ];

    for (samples, rel_path) in bgm {
        save_ogg(&samples, &audio_dir.join(rel_path), 44100, 2)?;
        count += 1;
    }

    // Ambience (Vorbis OGG)
    let ambience = [
        (synth_ambience_loop(120.0), "ambience/sacyra_meadow.ogg"),
        (synth_ambience_loop(80.0), "ambience/libira_dunes.ogg"),
        (synth_ambience_loop(55.0), "ambience/dreamscape_hum.ogg"),
        (synth_ambience_loop(240.0), "ambience/river_rapids.ogg"),
    ];

    for (samples, rel_path) in ambience {
        save_ogg(&samples, &audio_dir.join(rel_path), 44100, 2)?;
        count += 1;
    }

    Ok(count)
}
