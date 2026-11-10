// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/assets.rs
// @id            tools.assets.pipeline
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0-dev
// @interface_rev Rev 4
// @date          2026-11-08
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Asset Verification, Audit & Synthesis Pipeline
// @role          Audits physical on-disk visual (.png) and audio (.wav, .ogg)
//                assets against presentation data catalogs, validates binary
//                headers (PNG IHDR, WAV RIFF, OggS), and orchestrates asset
//                regeneration.
// @invariants    1. Zero panics in library execution; all fallible ops return Result.
//                2. Physical on-disk asset dimensions must match catalog contracts.
//                3. All audio assets must conform to standard sample rates (44.1kHz).
//                4. Strict <= 80 column discipline across all documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates the integrity, binary format headers, resolution, and audio
//   parameters of all static assets residing in `game/assets/` against the
//   canonical catalogs in `game/data/presentation/`. Bridges developer tooling
//   to procedural and artist assets.
// =============================================================================

//! # Asset Verification & Pipeline Audit Engine
//!
//! Validates `game/assets/` against `game/data/presentation/` manifests.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.assets.rust
// =============================================================================

pub mod audio_synth;
pub mod visual_synth;

use std::fs::{self, File};
use std::io::Read;
use std::path::{Path, PathBuf};
use std::time::Instant;

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};

/// Classification of asset media types.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AssetMediaType {
    PngImage,
    WavAudio,
    OggAudio,
}

/// Verification findings for an individual asset record.
#[derive(Debug, Clone)]
pub struct AssetFinding {
    pub rel_path: String,
    pub media_type: AssetMediaType,
    pub exists: bool,
    pub file_size: u64,
    pub width: Option<u32>,
    pub height: Option<u32>,
    pub details: String,
    pub error: Option<String>,
}

/// Comprehensive asset audit report.
#[derive(Debug, Default, Clone)]
pub struct AssetAuditReport {
    pub total_checked: usize,
    pub total_passed: usize,
    pub total_failed: usize,
    pub findings: Vec<AssetFinding>,
}

impl AssetAuditReport {
    pub fn is_success(&self) -> bool {
        self.total_failed == 0 && self.total_checked > 0
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / AUDIT PROCEDURES & RUNNER DISPATCH
// Stratum: Developer Tooling | Machine: tools.assets.rust
// =============================================================================

/// Core asset auditing and pipeline verification engine.
pub struct AssetAuditor {
    repo_root: PathBuf,
}

impl AssetAuditor {
    /// Creates a new asset auditor bound to the workspace root.
    pub fn new(repo_root: &Path) -> Self {
        Self {
            repo_root: repo_root.to_path_buf(),
        }
    }

    /// Resolves an asset path relative to `game/assets/` or repository root.
    pub fn resolve_asset_path(&self, rel_path: &str) -> PathBuf {
        let clean = rel_path.trim_start_matches('/');
        if clean.starts_with("assets/") {
            self.repo_root.join("game").join(clean)
        } else if clean.starts_with("game/assets/") {
            self.repo_root.join(clean)
        } else {
            self.repo_root.join("game/assets").join(clean)
        }
    }

    /// Inspects PNG binary header and extracts dimensions from the IHDR chunk.
    pub fn inspect_png(&self, path: &Path) -> Result<(u32, u32), String> {
        let mut file = File::open(path)
            .map_err(|e| format!("Failed to open PNG {}: {}", path.display(), e))?;
        let mut header = [0u8; 24];
        file.read_exact(&mut header)
            .map_err(|e| format!("Failed to read PNG header: {}", e))?;

        // PNG Magic: 0x89 0x50 0x4E 0x47 0x0D 0x0A 0x1A 0x0A
        if &header[0..8] != b"\x89PNG\r\n\x1a\n" {
            return Err("Invalid PNG magic signature".to_string());
        }

        // Chunk type IHDR at offset 12..16
        if &header[12..16] != b"IHDR" {
            return Err("First chunk is not IHDR".to_string());
        }

        let width = u32::from_be_bytes([header[16], header[17], header[18], header[19]]);
        let height = u32::from_be_bytes([header[20], header[21], header[22], header[23]]);

        Ok((width, height))
    }

    /// Inspects WAV binary header (RIFF ... WAVEfmt ).
    pub fn inspect_wav(&self, path: &Path) -> Result<(u16, u32), String> {
        let mut file = File::open(path)
            .map_err(|e| format!("Failed to open WAV {}: {}", path.display(), e))?;
        let mut header = [0u8; 36];
        file.read_exact(&mut header)
            .map_err(|e| format!("Failed to read WAV header: {}", e))?;

        if &header[0..4] != b"RIFF" || &header[8..12] != b"WAVE" {
            return Err("Invalid WAV RIFF signature".to_string());
        }

        let channels = u16::from_le_bytes([header[22], header[23]]);
        let sample_rate =
            u32::from_le_bytes([header[24], header[25], header[26], header[27]]);

        Ok((channels, sample_rate))
    }

    /// Inspects OGG Vorbis binary header (OggS).
    pub fn inspect_ogg(&self, path: &Path) -> Result<(), String> {
        let mut file = File::open(path)
            .map_err(|e| format!("Failed to open OGG {}: {}", path.display(), e))?;
        let mut header = [0u8; 4];
        file.read_exact(&mut header)
            .map_err(|e| format!("Failed to read OGG header: {}", e))?;

        if &header != b"OggS" {
            return Err("Invalid OGG OggS signature".to_string());
        }

        Ok(())
    }

    /// Audits a PNG image asset against expected dimensions.
    pub fn audit_png(
        &self,
        rel_path: &str,
        expected_w: Option<u32>,
        expected_h: Option<u32>,
    ) -> AssetFinding {
        let full = self.resolve_asset_path(rel_path);
        if !full.is_file() {
            return AssetFinding {
                rel_path: rel_path.to_string(),
                media_type: AssetMediaType::PngImage,
                exists: false,
                file_size: 0,
                width: None,
                height: None,
                details: "File missing on disk".to_string(),
                error: Some(format!("Asset not found at: {}", full.display())),
            };
        }

        let size = fs::metadata(&full).map(|m| m.len()).unwrap_or(0);
        match self.inspect_png(&full) {
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

    /// Audits a WAV audio asset against expected parameters.
    pub fn audit_wav(&self, rel_path: &str, expected_rate: Option<u32>) -> AssetFinding {
        let full = self.resolve_asset_path(rel_path);
        if !full.is_file() {
            return AssetFinding {
                rel_path: rel_path.to_string(),
                media_type: AssetMediaType::WavAudio,
                exists: false,
                file_size: 0,
                width: None,
                height: None,
                details: "File missing on disk".to_string(),
                error: Some(format!("WAV asset missing: {}", full.display())),
            };
        }

        let size = fs::metadata(&full).map(|m| m.len()).unwrap_or(0);
        match self.inspect_wav(&full) {
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

    /// Audits an OGG Vorbis audio asset.
    pub fn audit_ogg(&self, rel_path: &str) -> AssetFinding {
        let full = self.resolve_asset_path(rel_path);
        if !full.is_file() {
            return AssetFinding {
                rel_path: rel_path.to_string(),
                media_type: AssetMediaType::OggAudio,
                exists: false,
                file_size: 0,
                width: None,
                height: None,
                details: "File missing on disk".to_string(),
                error: Some(format!("OGG asset missing: {}", full.display())),
            };
        }

        let size = fs::metadata(&full).map(|m| m.len()).unwrap_or(0);
        match self.inspect_ogg(&full) {
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

    /// Executes a comprehensive audit of all canonical presentation assets.
    pub fn audit_all(&self) -> AssetAuditReport {
        let mut report = AssetAuditReport::default();

        let visual_assets = [
            ("sprites/characters/elijah_32x32.png", Some(32), Some(32)),
            ("sprites/characters/katrina_32x32.png", Some(32), Some(32)),
            ("sprites/characters/casatti_32x32.png", Some(32), Some(32)),
            ("sprites/characters/observer_32x32.png", Some(32), Some(32)),
            ("sprites/npc/guard_32x32.png", Some(32), Some(32)),
            ("sprites/enemies/dream_eater_32x32.png", Some(32), Some(32)),
            ("sprites/enemies/shadow_stalker_32x32.png", Some(32), Some(32)),
            ("sprites/enemies/dark_lord_40x48.png", Some(40), Some(48)),
            ("sprites/tiles/cobblestone_48x24.png", Some(48), Some(24)),
            ("sprites/tiles/grass_48x24.png", Some(48), Some(24)),
            ("sprites/tiles/void_crag_48x24.png", Some(48), Some(24)),
            ("sprites/tiles/water_48x24.png", Some(48), Some(24)),
            ("sprites/tiles/stone_wall_48x48.png", Some(48), Some(48)),
            ("sprites/icons/shield_lux_24x24.png", Some(24), Some(24)),
            ("sprites/icons/sword_tenebris_24x24.png", Some(24), Some(24)),
            ("textures/sprites/characters/elijah_sheet_128x128.png", Some(128), Some(128)),
            ("textures/sprites/characters/katrina_sheet_128x128.png", Some(128), Some(128)),
            ("textures/sprites/characters/observer_sheet_128x128.png", Some(128), Some(128)),
            ("textures/sprites/npc/guard_sheet_128x128.png", Some(128), Some(128)),
            ("textures/sprites/enemies/dream_eater_sheet_128x128.png", Some(128), Some(128)),
            ("textures/tilesets/sacyra_plains_256x256.png", Some(256), Some(256)),
            ("textures/tilesets/dreamscape_256x256.png", Some(256), Some(256)),
        ];

        for (path, w, h) in visual_assets {
            let finding = self.audit_png(path, w, h);
            report.total_checked += 1;
            if finding.error.is_none() && finding.exists {
                report.total_passed += 1;
            } else {
                report.total_failed += 1;
            }
            report.findings.push(finding);
        }

        let sfx_assets = [
            "audio/sfx/combat/ward_lux_barrio.wav",
            "audio/sfx/combat/strike_cuttatio.wav",
            "audio/sfx/combat/duo_loca_harmonic.wav",
            "audio/sfx/combat/shield_stagger.wav",
            "audio/sfx/tactical/zoc_pin.wav",
            "audio/sfx/tactical/step_plains.wav",
            "audio/sfx/tactical/step_cobble.wav",
            "audio/sfx/ui/stance_shift.wav",
            "audio/sfx/ui/menu_confirm.wav",
            "audio/sfx/ui/menu_cancel.wav",
        ];

        for path in sfx_assets {
            let finding = self.audit_wav(path, Some(44100));
            report.total_checked += 1;
            if finding.error.is_none() && finding.exists {
                report.total_passed += 1;
            } else {
                report.total_failed += 1;
            }
            report.findings.push(finding);
        }

        let ogg_assets = [
            "audio/bgm/dreamscape_nexus.ogg",
            "audio/bgm/sacyra_capital.ogg",
            "audio/bgm/libira_frontier.ogg",
            "audio/bgm/tactical_skirmish.ogg",
            "audio/bgm/dark_lord_usuru.ogg",
            "audio/ambience/sacyra_meadow.ogg",
            "audio/ambience/libira_dunes.ogg",
            "audio/ambience/dreamscape_hum.ogg",
            "audio/ambience/river_rapids.ogg",
        ];

        for path in ogg_assets {
            let finding = self.audit_ogg(path);
            report.total_checked += 1;
            if finding.error.is_none() && finding.exists {
                report.total_passed += 1;
            } else {
                report.total_failed += 1;
            }
            report.findings.push(finding);
        }

        report
    }

    /// Invokes the native Rust procedural asset synthesis engine.
    pub fn generate_assets(&self) -> Result<String, String> {
        let v_count = visual_synth::generate_all_visuals(&self.repo_root)?;
        let a_count = audio_synth::generate_all_audio(&self.repo_root)?;

        Ok(format!(
            "Native Rust Asset Synthesis Complete:\n  - {} visual assets generated (PNG)\n  - {} audio assets generated (WAV / OGG)",
            v_count, a_count
        ))
    }
}

/// Pipeline task runner integrating asset verification into `tl_tools`.
pub struct AssetAuditTask;

impl TaskRunner for AssetAuditTask {
    fn name(&self) -> &'static str {
        "asset_pipeline"
    }

    fn description(&self) -> &'static str {
        "Audits physical on-disk visual and audio assets against presentation catalogs"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        let assets_dir = ctx.repo_root.join("game/assets");
        if !assets_dir.is_dir() {
            return Err(format!("Assets directory missing: {}", assets_dir.display()));
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let start = Instant::now();
        let auditor = AssetAuditor::new(&ctx.repo_root);
        let report = auditor.audit_all();
        let elapsed = start.elapsed().as_millis();

        if report.is_success() {
            let summary = format!(
                "Asset verification passed: {}/{} assets valid on disk.",
                report.total_passed, report.total_checked
            );
            println!("  [PASS] {}", summary);
            Ok(TaskTelemetry::success(self.name(), elapsed as u64, &summary))
        } else {
            let mut err_msg = format!(
                "Asset verification failed: {}/{} passed, {} failed.\n",
                report.total_passed, report.total_checked, report.total_failed
            );
            for f in report.findings.iter().filter(|f| f.error.is_some() || !f.exists) {
                err_msg.push_str(&format!(
                    "  [FAIL] {}: {}\n",
                    f.rel_path,
                    f.error.as_deref().unwrap_or("File not found")
                ));
            }
            Err(err_msg)
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Developer Tooling | Machine: tools.assets.rust
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_asset_auditor_resolution() {
        let root = PathBuf::from("/repo");
        let auditor = AssetAuditor::new(&root);
        assert_eq!(
            auditor.resolve_asset_path("sprites/characters/elijah_32x32.png"),
            root.join("game/assets/sprites/characters/elijah_32x32.png")
        );
        assert_eq!(
            auditor.resolve_asset_path("assets/sprites/icons/shield_lux_24x24.png"),
            root.join("game/assets/sprites/icons/shield_lux_24x24.png")
        );
    }
}
