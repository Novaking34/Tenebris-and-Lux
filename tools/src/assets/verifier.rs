// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/assets/verifier.rs
// @id            cog.tools.assets.verifier
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Asset Verification, Audit & Synthesis Pipeline
// @role          Binary Header Inspection & Asset Parameter Verification
// @maturity      Rev 5 Baseline | Stateless header inspector for PNG, WAV, OGG
// @chassis       bounded-heap | Fixed-size byte buffers (24B PNG, 36B WAV, 4B OGG)
// @scale         O(1) header inspection per file; bounded reads
// @seams         in: game/assets/ | uses: std::fs::File | probes: self_audit -> exit
// @invariants    1. Zero panics in library execution; all fallible ops return Result.
//                2. Physical on-disk asset dimensions must match catalog contracts.
//                3. All audio assets must conform to standard sample rates (44.1kHz).
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Inspects binary headers (PNG IHDR, WAV RIFF/fmt, OggS) and validates physical
//   resolution, channel count, and sample rate parameters against canonical
//   presentation contracts.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures that sprite sheets, combat SFX, and background music files placed
//   in `game/assets/` are valid, uncorrupted, and have the exact dimensions and
//   sample rates the game engine expects before runtime.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Pure functions operating on path slices; no shared
//     mutable state; bounded heap allocations.
//   - Extension Seam: Additional format verifiers (e.g. QOI, FLAC) register as
//     new `AssetMediaType` variants with dedicated header inspectors.
//   - Explicit Non-Goal: Does not decode full compressed image pixels or audio
//     PCM streams; validates structural headers and dimensions only.
// =============================================================================

//! # Binary Asset Header Verifier & Parameter Inspector
//!
//! Validates PNG IHDR dimensions, WAV RIFF format chunks, and OGG container headers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Types, Enums & Header Inspection Constants )
// =============================================================================

use std::fs::{self, File};
use std::io::Read;
use std::path::Path;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Constants & Magic Signatures
// -----------------------------------------------------------------------------

/// Canonical PNG magic bytes (8 bytes: 0x89 'P' 'N' 'G' '\r' '\n' 0x1A '\n').
pub const PNG_MAGIC: &[u8; 8] = b"\x89PNG\r\n\x1a\n";

/// Canonical WAV RIFF signature prefix (4 bytes: 'R' 'I' 'F' 'F').
pub const WAV_RIFF_MAGIC: &[u8; 4] = b"RIFF";

/// Canonical WAV format chunk descriptor (4 bytes: 'W' 'A' 'V' 'E').
pub const WAV_WAVE_MAGIC: &[u8; 4] = b"WAVE";

/// Canonical Ogg Vorbis container signature (4 bytes: 'O' 'g' 'g' 'S').
pub const OGG_MAGIC: &[u8; 4] = b"OggS";

/// Standard audio sample rate expected across all game sound effects (44.1 kHz).
pub const STANDARD_AUDIO_SAMPLE_RATE: u32 = 44100;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Finding Envelopes
// -----------------------------------------------------------------------------

/// Classification of supported asset media types.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AssetMediaType {
    /// Portable Network Graphics image (.png).
    PngImage,
    /// Waveform audio container (.wav).
    WavAudio,
    /// Ogg Vorbis compressed audio container (.ogg).
    OggAudio,
}

/// Verification findings for an individual on-disk asset record.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct AssetFinding {
    /// Relative path within the presentation asset tree.
    pub rel_path: String,
    /// Classified media type.
    pub media_type: AssetMediaType,
    /// Whether the file exists on disk.
    pub exists: bool,
    /// File size in bytes.
    pub file_size: u64,
    /// Measured horizontal pixel width (for visual assets).
    pub width: Option<u32>,
    /// Measured vertical pixel height (for visual assets).
    pub height: Option<u32>,
    /// Human-readable diagnostic details.
    pub details: String,
    /// Specific validation error if any rule was violated.
    pub error: Option<String>,
}

impl AssetFinding {
    /// Returns true if the asset exists and satisfied all format invariants.
    pub fn is_clean(&self) -> bool {
        self.exists && self.error.is_none()
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Binary Header Inspection & Parameter Verification Logic )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: PNG Binary Header & IHDR Dimension Inspection
// -----------------------------------------------------------------------------

/// Inspects a PNG binary header and extracts dimensions from the IHDR chunk.
///
/// # Errors
///
/// Returns `Err(String)` if the file cannot be read, lacks PNG magic, or has
/// an invalid IHDR chunk.
pub fn inspect_png(path: &Path) -> Result<(u32, u32), String> {
    let mut file = File::open(path)
        .map_err(|e| format!("Failed to open PNG {}: {}", path.display(), e))?;
    let mut header = [0u8; 24];
    file.read_exact(&mut header)
        .map_err(|e| format!("Failed to read PNG header: {}", e))?;

    if &header[0..8] != PNG_MAGIC {
        return Err("Invalid PNG magic signature".to_string());
    }

    if &header[12..16] != b"IHDR" {
        return Err("First chunk is not IHDR".to_string());
    }

    let width = u32::from_be_bytes([header[16], header[17], header[18], header[19]]);
    let height = u32::from_be_bytes([header[20], header[21], header[22], header[23]]);

    Ok((width, height))
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: WAV RIFF / fmt Header Inspection
// -----------------------------------------------------------------------------

/// Inspects a WAV binary header and extracts channel count and sample rate.
///
/// # Errors
///
/// Returns `Err(String)` if the file cannot be read, lacks RIFF/WAVE signatures,
/// or is truncated.
pub fn inspect_wav(path: &Path) -> Result<(u16, u32), String> {
    let mut file = File::open(path)
        .map_err(|e| format!("Failed to open WAV {}: {}", path.display(), e))?;
    let mut header = [0u8; 36];
    file.read_exact(&mut header)
        .map_err(|e| format!("Failed to read WAV header: {}", e))?;

    if &header[0..4] != WAV_RIFF_MAGIC || &header[8..12] != WAV_WAVE_MAGIC {
        return Err("Invalid WAV RIFF signature".to_string());
    }

    let channels = u16::from_le_bytes([header[22], header[23]]);
    let sample_rate =
        u32::from_le_bytes([header[24], header[25], header[26], header[27]]);

    Ok((channels, sample_rate))
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: OGG Vorbis Container Header Inspection
// -----------------------------------------------------------------------------

/// Inspects an OGG Vorbis binary header for the canonical `OggS` capture pattern.
///
/// # Errors
///
/// Returns `Err(String)` if the file cannot be read or lacks `OggS` signature.
pub fn inspect_ogg(path: &Path) -> Result<(), String> {
    let mut file = File::open(path)
        .map_err(|e| format!("Failed to open OGG {}: {}", path.display(), e))?;
    let mut header = [0u8; 4];
    file.read_exact(&mut header)
        .map_err(|e| format!("Failed to read OGG header: {}", e))?;

    if &header != OGG_MAGIC {
        return Err("Invalid OGG OggS signature".to_string());
    }

    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Single Asset Auditing Routines
// -----------------------------------------------------------------------------

/// Audits a PNG image asset against expected dimensions.
pub fn audit_png_file(
    full_path: &Path,
    rel_path: &str,
    expected_w: Option<u32>,
    expected_h: Option<u32>,
) -> AssetFinding {
    if !full_path.is_file() {
        return AssetFinding {
            rel_path: rel_path.to_string(),
            media_type: AssetMediaType::PngImage,
            exists: false,
            file_size: 0,
            width: None,
            height: None,
            details: "File missing on disk".to_string(),
            error: Some(format!("Asset not found at: {}", full_path.display())),
        };
    }

    let size = fs::metadata(full_path).map(|m| m.len()).unwrap_or(0);
    match inspect_png(full_path) {
        Ok((w, h)) => {
            let mut err = None;
            if let Some(ew) = expected_w {
                if w != ew {
                    err = Some(format!("Width mismatch: expected {}, got {}", ew, w));
                }
            }
            if let Some(eh) = expected_h {
                if h != eh {
                    let e_msg = format!("Height mismatch: expected {}, got {}", eh, h);
                    err = Some(err.map_or(e_msg.clone(), |prev| format!("{}, {}", prev, e_msg)));
                }
            }

            AssetFinding {
                rel_path: rel_path.to_string(),
                media_type: AssetMediaType::PngImage,
                exists: true,
                file_size: size,
                width: Some(w),
                height: Some(h),
                details: format!("PNG {}x{} ({} bytes)", w, h, size),
                error: err,
            }
        }
        Err(e) => AssetFinding {
            rel_path: rel_path.to_string(),
            media_type: AssetMediaType::PngImage,
            exists: true,
            file_size: size,
            width: None,
            height: None,
            details: "Corrupt PNG header".to_string(),
            error: Some(e),
        },
    }
}

/// Audits a WAV audio asset against expected sample rate parameters.
pub fn audit_wav_file(
    full_path: &Path,
    rel_path: &str,
    expected_rate: Option<u32>,
) -> AssetFinding {
    if !full_path.is_file() {
        return AssetFinding {
            rel_path: rel_path.to_string(),
            media_type: AssetMediaType::WavAudio,
            exists: false,
            file_size: 0,
            width: None,
            height: None,
            details: "File missing on disk".to_string(),
            error: Some(format!("WAV asset missing: {}", full_path.display())),
        };
    }

    let size = fs::metadata(full_path).map(|m| m.len()).unwrap_or(0);
    match inspect_wav(full_path) {
        Ok((channels, rate)) => {
            let mut err = None;
            if let Some(er) = expected_rate {
                if rate != er {
                    err = Some(format!("Sample rate mismatch: expected {}Hz, got {}Hz", er, rate));
                }
            }

            AssetFinding {
                rel_path: rel_path.to_string(),
                media_type: AssetMediaType::WavAudio,
                exists: true,
                file_size: size,
                width: None,
                height: None,
                details: format!("WAV {}Hz ({} channels, {} bytes)", rate, channels, size),
                error: err,
            }
        }
        Err(e) => AssetFinding {
            rel_path: rel_path.to_string(),
            media_type: AssetMediaType::WavAudio,
            exists: true,
            file_size: size,
            width: None,
            height: None,
            details: "Corrupt WAV header".to_string(),
            error: Some(e),
        },
    }
}

/// Audits an OGG Vorbis audio container asset.
pub fn audit_ogg_file(full_path: &Path, rel_path: &str) -> AssetFinding {
    if !full_path.is_file() {
        return AssetFinding {
            rel_path: rel_path.to_string(),
            media_type: AssetMediaType::OggAudio,
            exists: false,
            file_size: 0,
            width: None,
            height: None,
            details: "File missing on disk".to_string(),
            error: Some(format!("OGG asset missing: {}", full_path.display())),
        };
    }

    let size = fs::metadata(full_path).map(|m| m.len()).unwrap_or(0);
    match inspect_ogg(full_path) {
        Ok(()) => AssetFinding {
            rel_path: rel_path.to_string(),
            media_type: AssetMediaType::OggAudio,
            exists: true,
            file_size: size,
            width: None,
            height: None,
            details: format!("OGG Vorbis container ({} bytes)", size),
            error: None,
        },
        Err(e) => AssetFinding {
            rel_path: rel_path.to_string(),
            media_type: AssetMediaType::OggAudio,
            exists: true,
            file_size: size,
            width: None,
            height: None,
            details: "Corrupt OGG header".to_string(),
            error: Some(e),
        },
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Four-Section Ontological Test Suite )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates & Self-Audit Probe
// -----------------------------------------------------------------------------

/// Preflight self-audit verifying signature constants and format definitions.
pub fn self_audit() -> Result<(), &'static str> {
    if PNG_MAGIC.len() != 8 {
        return Err("PNG magic constant must be exactly 8 bytes");
    }
    if WAV_RIFF_MAGIC.len() != 4 || WAV_WAVE_MAGIC.len() != 4 {
        return Err("WAV magic constants must be exactly 4 bytes");
    }
    if OGG_MAGIC.len() != 4 {
        return Err("OGG magic constant must be exactly 4 bytes");
    }
    if STANDARD_AUDIO_SAMPLE_RATE != 44100 {
        return Err("Standard audio sample rate must be 44100 Hz");
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Test Suite
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_always_pass_magic_constants() {
        assert_eq!(PNG_MAGIC, b"\x89PNG\r\n\x1a\n");
        assert_eq!(WAV_RIFF_MAGIC, b"RIFF");
        assert_eq!(WAV_WAVE_MAGIC, b"WAVE");
        assert_eq!(OGG_MAGIC, b"OggS");
        assert_eq!(STANDARD_AUDIO_SAMPLE_RATE, 44100);
    }

    #[test]
    fn test_section_1_always_pass_clean_finding() {
        let finding = AssetFinding {
            rel_path: "sprites/test.png".to_string(),
            media_type: AssetMediaType::PngImage,
            exists: true,
            file_size: 1024,
            width: Some(32),
            height: Some(32),
            details: "clean".to_string(),
            error: None,
        };
        assert!(finding.is_clean());
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_files() {
        let bogus = Path::new("/nonexistent/bogus_asset.png");
        let finding = audit_png_file(bogus, "bogus.png", Some(32), Some(32));
        assert!(!finding.exists);
        assert!(!finding.is_clean());
        assert!(finding.error.is_some());

        let bogus_wav = Path::new("/nonexistent/bogus.wav");
        let w_finding = audit_wav_file(bogus_wav, "bogus.wav", Some(44100));
        assert!(!w_finding.exists);
        assert!(!w_finding.is_clean());

        let bogus_ogg = Path::new("/nonexistent/bogus.ogg");
        let o_finding = audit_ogg_file(bogus_ogg, "bogus.ogg");
        assert!(!o_finding.exists);
        assert!(!o_finding.is_clean());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_media_types() {
        assert_ne!(AssetMediaType::PngImage, AssetMediaType::WavAudio);
        assert_ne!(AssetMediaType::WavAudio, AssetMediaType::OggAudio);
        assert_ne!(AssetMediaType::PngImage, AssetMediaType::OggAudio);
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
        assert!(is_verifier_ready());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Handshake & Subsystem Readiness Probe )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe & Lifecycle Handshake
// -----------------------------------------------------------------------------

/// Epilogue lifecycle readiness probe for the asset verifier cog.
///
/// # Examples
///
/// ```rust
/// use tl_tools::assets::verifier::is_verifier_ready;
///
/// assert!(is_verifier_ready());
/// ```
pub fn is_verifier_ready() -> bool {
    self_audit().is_ok()
}
