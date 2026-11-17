// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide/manifest.rs
// @id            cog.tools.guide.manifest
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     pure_types | data_schema
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Manifest Sub-Assembly
// @role          Publication Settings, Manifest Envelopes & Volume Specifications
// @maturity      Rev 5 Baseline | 5-Block JSON manifest schemas & loaders
// @chassis       bounded-heap | Developer tooling memory chassis; bounded vectors
// @scale         O(V + C) across volumes and chapters
// @seams         in: master_guide_manifest.json | out: MasterGuideBuilder
// @invariants    1. Zero missing chapter tolerance during pre-flight audit.
//                2. Deterministic volume and chapter sequence matching ontology.
//                3. Dual granularity: unified master compendium and volume books.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Defines data schemas, configuration envelopes, and manifest parsing logic
//   for the 6-volume Tenebris and Lux Master Guide publication pipeline.
//
//   ### For Game Designers & Non-Programmers:
//   Controls table of contents, book metadata, author credits, and chapter
//   ordering defined in `docs/master_guide_manifest.json`.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Manifest types are Serde-serializable pure data
//     structures. Manifest loading is read-only and idempotent.
//   - Extension Seam: Additional volume metadata fields map directly to JSON
//     without breaking backward compatibility.
//   - Explicit Non-Goal: Does not modify source text files during manifest load.
// =============================================================================

//! # Guide Manifest & Volume Specifications
//!
//! Provides 5-block JSON manifest schemas, volume/chapter specifications, and
//! default publication configuration for the Master Guide documentation suite.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::collections::HashMap;
use std::fs;
use std::path::{Path, PathBuf};
use serde::{Deserialize, Serialize};

use super::compiler::scan_area_for_chapters_with_excludes;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Default Values & String Constants
// -----------------------------------------------------------------------------

/// Default boolean true supplier for serde default attributes.
pub fn default_true() -> bool {
    true
}

/// Canonical title for the unified Master Guide compendium.
pub fn default_master_title() -> String {
    "Tenebris and Lux: Comprehensive Master Guide".to_string()
}

/// Default publisher and author credit.
pub fn default_author() -> String {
    "CreativeWorkzStudio LLC / ProfessorSeanEX".to_string()
}

/// Baseline documentation publication revision.
pub fn default_revnumber() -> String {
    "0.2.0-dev".to_string()
}

/// Baseline publication revision date.
pub fn default_revdate() -> String {
    "2026-11-12".to_string()
}

/// Default AsciiDoc document type.
pub fn default_doctype() -> String {
    "book".to_string()
}

/// Default table-of-contents alignment.
pub fn default_toc() -> String {
    "left".to_string()
}

/// Default icon rendering mode.
pub fn default_icons() -> String {
    "font".to_string()
}

/// Default syntax highlighter.
pub fn default_highlighter() -> String {
    "rouge".to_string()
}

/// Default relative path to code snapshots manifest.
pub fn default_snapshots_manifest() -> String {
    "docs/code_snapshots_manifest.json".to_string()
}

/// Default relative directory for materialized snapshot plates.
pub fn default_snapshots_dir() -> String {
    "docs/snapshots".to_string()
}

/// Default title for the wholesale source code appendix.
pub fn default_appendix_title() -> String {
    "Wholesale Source Code Reference Plates".to_string()
}

/// Default Asciidoctor-PDF theme name.
pub fn default_theme_name() -> String {
    "cornerstone-book".to_string()
}

/// Base theme extended by default PDF theme.
pub fn default_theme_extends() -> String {
    "default".to_string()
}

/// Default output path for generated theme YAML.
pub fn default_theme_yaml_path() -> String {
    "docs/themes/cornerstone-book-theme.yml".to_string()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Manifest Envelopes
// -----------------------------------------------------------------------------

/// Specification for a single chapter included within a master volume.
///
/// # Examples
///
/// ```rust
/// use std::path::PathBuf;
/// use tl_tools::guide::ChapterSpec;
///
/// let chapter = ChapterSpec {
///     number: 1,
///     title: "Introduction".to_string(),
///     source_path: PathBuf::from("docs/lore/intro.adoc"),
/// };
/// assert_eq!(chapter.number, 1);
/// assert_eq!(chapter.title, "Introduction");
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct ChapterSpec {
    /// Sequential chapter number across the compendium or volume.
    pub number: usize,
    /// Human-readable title of the chapter.
    pub title: String,
    /// Absolute or relative path to the source AsciiDoc / Markdown file.
    pub source_path: PathBuf,
}

/// Specification for a major thematic volume within the master compendium.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::VolumeSpec;
///
/// let vol = VolumeSpec {
///     roman: "I".to_string(),
///     slug: "world_canon".to_string(),
///     title: "World Canon".to_string(),
///     description: "Lore and continental realms.".to_string(),
///     scan_areas: vec!["lore".to_string()],
///     chapters: vec![],
/// };
/// assert_eq!(vol.roman, "I");
/// assert_eq!(vol.slug, "world_canon");
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct VolumeSpec {
    /// Roman numeral designator (e.g., "I", "II", "III", "VI").
    pub roman: String,
    /// Filesystem-safe slug for artifact filenames (e.g., "world_canon").
    #[serde(default)]
    pub slug: String,
    /// Volume title.
    pub title: String,
    /// Architectural synopsis of the volume's thematic contents.
    pub description: String,
    /// Subdirectories relative to docs/ scanned for chapters.
    #[serde(default)]
    pub scan_areas: Vec<String>,
    /// Ordered list of chapters belonging to this volume.
    pub chapters: Vec<ChapterSpec>,
}

/// JSON manifest chapter descriptor within 5-block envelope.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifestChapter {
    pub title: String,
    pub rel_path: String,
}

/// JSON manifest volume descriptor within 5-block envelope.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifestVolume {
    pub roman: String,
    #[serde(default)]
    pub slug: String,
    pub title: String,
    pub description: String,
    #[serde(default)]
    pub scan_areas: Vec<String>,
    #[serde(default)]
    pub include_snapshot_appendix: Option<bool>,
    #[serde(default)]
    pub chapters: Vec<GuideManifestChapter>,
}

/// Metadata block for the Master Guide manifest.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifestMetadata {
    pub schema: String,
    pub version: String,
    pub interface_rev: String,
    pub date: String,
    pub author: String,
    pub title: String,
    #[serde(default)]
    pub invariants: Vec<String>,
}

/// Publication formatting and layout toggles.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::PublicationSettings;
///
/// let pub_settings = PublicationSettings::default();
/// assert_eq!(pub_settings.doctype, "book");
/// assert!(pub_settings.shift_chapter_headings);
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct PublicationSettings {
    #[serde(default = "default_master_title")]
    pub master_title: String,
    #[serde(default = "default_author")]
    pub author: String,
    #[serde(default = "default_revnumber")]
    pub revnumber: String,
    #[serde(default = "default_revdate")]
    pub revdate: String,
    #[serde(default = "default_doctype")]
    pub doctype: String,
    #[serde(default = "default_toc")]
    pub toc: String,
    #[serde(default = "default_icons")]
    pub icons: String,
    #[serde(default = "default_highlighter")]
    pub source_highlighter: String,
    #[serde(default = "default_true")]
    pub shift_chapter_headings: bool,
    #[serde(default = "default_true")]
    pub strip_manual_heading_numbers: bool,
    #[serde(default = "default_true")]
    pub expand_includes: bool,
    #[serde(default = "default_true")]
    pub expand_custom_attributes: bool,
}

impl Default for PublicationSettings {
    fn default() -> Self {
        Self {
            master_title: default_master_title(),
            author: default_author(),
            revnumber: default_revnumber(),
            revdate: default_revdate(),
            doctype: default_doctype(),
            toc: default_toc(),
            icons: default_icons(),
            source_highlighter: default_highlighter(),
            shift_chapter_headings: true,
            strip_manual_heading_numbers: true,
            expand_includes: true,
            expand_custom_attributes: true,
        }
    }
}

/// Snapshot reference configuration within publication manifest.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::SnapshotSettings;
///
/// let snap_settings = SnapshotSettings::default();
/// assert!(snap_settings.include_wholesale_appendix);
/// assert!(snap_settings.show_line_numbers);
/// ```
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct SnapshotSettings {
    #[serde(default = "default_snapshots_manifest")]
    pub manifest_path: String,
    #[serde(default = "default_snapshots_dir")]
    pub output_dir: String,
    #[serde(default = "default_true")]
    pub include_wholesale_appendix: bool,
    #[serde(default = "default_appendix_title")]
    pub appendix_title: String,
    #[serde(default = "default_true")]
    pub show_line_numbers: bool,
}

impl Default for SnapshotSettings {
    fn default() -> Self {
        Self {
            manifest_path: default_snapshots_manifest(),
            output_dir: default_snapshots_dir(),
            include_wholesale_appendix: true,
            appendix_title: default_appendix_title(),
            show_line_numbers: true,
        }
    }
}

/// Block 2 types block for Master Guide manifest.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifestTypes {
    #[serde(default)]
    pub formats: Vec<String>,
    #[serde(default)]
    pub pdf_engine: Option<String>,
    #[serde(default)]
    pub default_pdf_theme: Option<String>,
    #[serde(default)]
    pub pdf_themes_dir: Option<String>,
    #[serde(default)]
    pub pdf_theme_data_file: Option<String>,
    #[serde(default)]
    pub toc_levels: Option<u32>,
    #[serde(default)]
    pub sectnum_levels: Option<u32>,
    #[serde(default)]
    pub line_discipline_max_cols: Option<u32>,
    #[serde(default)]
    pub publication: Option<PublicationSettings>,
    #[serde(default)]
    pub snapshots: Option<SnapshotSettings>,
    #[serde(default)]
    pub exclude_scan_dirs: Vec<String>,
}

/// Block 3 body block for Master Guide manifest.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifestBody {
    pub volumes: Vec<GuideManifestVolume>,
}

/// Block 4 audit block for Master Guide manifest.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifestAudit {
    #[serde(default)]
    pub required_volumes: usize,
    #[serde(default)]
    pub min_total_chapters: usize,
}

/// Block 5 closing block for Master Guide manifest.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifestClosing {
    #[serde(default)]
    pub publisher: String,
    #[serde(default)]
    pub status: String,
    #[serde(default)]
    pub compilation_targets: Vec<String>,
}

/// 5-block envelope representing `docs/master_guide_manifest.json`.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub struct GuideManifest {
    pub block_1_metadata: GuideManifestMetadata,
    #[serde(default, alias = "block_2_opening")]
    pub block_2_types: Option<GuideManifestTypes>,
    pub block_3_body: GuideManifestBody,
    #[serde(default)]
    pub block_4_audit: Option<GuideManifestAudit>,
    #[serde(default)]
    pub block_5_closing: Option<GuideManifestClosing>,
}

/// Loaded publishing configuration bundling publication, theme, and snapshot specs.
#[derive(Debug, Clone)]
pub struct GuideConfig {
    pub publication: PublicationSettings,
    pub snapshots: SnapshotSettings,
    pub toc_levels: u32,
    pub sectnum_levels: u32,
    pub pdf_theme_name: String,
    pub pdf_themes_dir: PathBuf,
    pub pdf_theme_data_file: Option<PathBuf>,
    pub exclude_scan_dirs: Vec<String>,
    pub volume_appendix_overrides: HashMap<String, bool>,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a volume specification satisfies structural invariants.
///
/// Requires a non-empty Roman numeral, non-empty slug, and non-empty title.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::VolumeSpec;
/// use tl_tools::guide::manifest::is_valid_volume_spec;
///
/// let vol = VolumeSpec {
///     roman: "I".to_string(),
///     slug: "world_canon".to_string(),
///     title: "World Canon".to_string(),
///     description: "Description".to_string(),
///     scan_areas: vec![],
///     chapters: vec![],
/// };
/// assert!(is_valid_volume_spec(&vol));
/// ```
pub fn is_valid_volume_spec(vol: &VolumeSpec) -> bool {
    !vol.roman.trim().is_empty()
        && !vol.slug.trim().is_empty()
        && !vol.title.trim().is_empty()
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Manifest Loading, Parsing, Validation & Fallback Construction )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Manifest Loading, Parsing, Validation & Defaults
// -----------------------------------------------------------------------------

/// Creates the baseline default configuration when manifest attributes are missing.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::guide::default_config;
///
/// let config = default_config(Path::new("."));
/// assert_eq!(config.toc_levels, 2);
/// assert_eq!(config.sectnum_levels, 3);
/// ```
pub fn default_config(repo_root: &Path) -> GuideConfig {
    GuideConfig {
        publication: PublicationSettings::default(),
        snapshots: SnapshotSettings::default(),
        toc_levels: 2,
        sectnum_levels: 3,
        pdf_theme_name: default_theme_name(),
        pdf_themes_dir: repo_root.join("docs/themes"),
        pdf_theme_data_file: Some(repo_root.join("docs/guide_theme_config.json")),
        exclude_scan_dirs: vec![
            "snapshots".to_string(),
            "themes".to_string(),
            "volumes".to_string(),
            "offline_work".to_string(),
        ],
        volume_appendix_overrides: HashMap::new(),
    }
}

/// Loads volume specifications from a data-driven 5-block JSON manifest.
pub fn load_volumes_from_manifest(
    docs_dir: &Path,
    path: &Path,
) -> Result<Vec<VolumeSpec>, String> {
    let repo_root = docs_dir.parent().unwrap_or(docs_dir);
    let (vols, _) = load_manifest_and_config(repo_root, docs_dir, path)?;
    Ok(vols)
}

/// Loads and validates manifest JSON, returning volumes and resolved config.
pub fn load_manifest_and_config(
    repo_root: &Path,
    docs_dir: &Path,
    path: &Path,
) -> Result<(Vec<VolumeSpec>, GuideConfig), String> {
    let raw = fs::read_to_string(path)
        .map_err(|e| format!("Unable to read guide manifest file: {}", e))?;
    let manifest: GuideManifest = serde_json::from_str(&raw)
        .map_err(|e| format!("Failed to parse JSON guide manifest: {}", e))?;

    let mut cfg = default_config(repo_root);
    if let Some(ref b2) = manifest.block_2_types {
        if let Some(ref theme) = b2.default_pdf_theme {
            cfg.pdf_theme_name = theme.clone();
        }
        if let Some(ref themes_dir) = b2.pdf_themes_dir {
            cfg.pdf_themes_dir = repo_root.join(themes_dir);
        }
        if let Some(ref theme_data) = b2.pdf_theme_data_file {
            cfg.pdf_theme_data_file = Some(repo_root.join(theme_data));
        }
        if let Some(levels) = b2.toc_levels {
            cfg.toc_levels = levels;
        }
        if let Some(slevels) = b2.sectnum_levels {
            cfg.sectnum_levels = slevels;
        }
        if let Some(ref p) = b2.publication {
            cfg.publication = p.clone();
        }
        if let Some(ref s) = b2.snapshots {
            cfg.snapshots = s.clone();
        }
        if !b2.exclude_scan_dirs.is_empty() {
            cfg.exclude_scan_dirs = b2.exclude_scan_dirs.clone();
        }
    }

    let mut chapter_counter = 1;
    let mut volumes = Vec::new();

    for mv in manifest.block_3_body.volumes {
        if let Some(override_flag) = mv.include_snapshot_appendix {
            cfg.volume_appendix_overrides
                .insert(mv.roman.clone(), override_flag);
        }
        let mut chapters = Vec::new();

        for ch in mv.chapters {
            chapters.push(ChapterSpec {
                number: chapter_counter,
                title: ch.title,
                source_path: docs_dir.join(ch.rel_path),
            });
            chapter_counter += 1;
        }

        for area in &mv.scan_areas {
            let scanned = scan_area_for_chapters_with_excludes(
                docs_dir,
                area,
                &cfg.exclude_scan_dirs,
            );
            for (scanned_title, scanned_path) in scanned {
                if !chapters.iter().any(|c| c.source_path == scanned_path) {
                    chapters.push(ChapterSpec {
                        number: chapter_counter,
                        title: scanned_title,
                        source_path: scanned_path,
                    });
                    chapter_counter += 1;
                }
            }
        }

        let slug = if mv.slug.is_empty() {
            format!("volume_{}", mv.roman.to_lowercase())
        } else {
            mv.slug
        };

        volumes.push(VolumeSpec {
            roman: mv.roman,
            slug,
            title: mv.title,
            description: mv.description,
            scan_areas: mv.scan_areas,
            chapters,
        });
    }

    Ok((volumes, cfg))
}

/// Writes default 5-block manifest to disk if absent.
pub fn save_default_manifest(
    docs_dir: &Path,
    manifest_path: &Path,
    volumes: &[VolumeSpec],
) -> Result<(), String> {
    let manifest = GuideManifest {
        block_1_metadata: GuideManifestMetadata {
            schema: "tenlux.data.master_guide.v2".to_string(),
            version: default_revnumber(),
            interface_rev: "Rev 5".to_string(),
            date: default_revdate(),
            author: default_author(),
            title: "Tenebris and Lux Master Guide Publication Manifest".to_string(),
            invariants: vec![
                "Zero missing chapter tolerance during pre-flight audit".to_string(),
                "Deterministic volume and chapter sequence matching project ontology".to_string(),
                "Dual granularity: unified master compendium and volume books".to_string(),
            ],
        },
        block_2_types: Some(GuideManifestTypes {
            formats: vec!["adoc".to_string(), "md".to_string(), "pdf".to_string()],
            pdf_engine: Some("asciidoctor-pdf".to_string()),
            default_pdf_theme: Some(default_theme_name()),
            pdf_themes_dir: Some("docs/themes".to_string()),
            pdf_theme_data_file: Some("docs/guide_theme_config.json".to_string()),
            toc_levels: Some(2),
            sectnum_levels: Some(3),
            line_discipline_max_cols: Some(80),
            publication: Some(PublicationSettings::default()),
            snapshots: Some(SnapshotSettings::default()),
            exclude_scan_dirs: vec![
                "snapshots".to_string(),
                "themes".to_string(),
                "volumes".to_string(),
                "offline_work".to_string(),
            ],
        }),
        block_3_body: GuideManifestBody {
            volumes: volumes
                .iter()
                .map(|v| GuideManifestVolume {
                    roman: v.roman.clone(),
                    slug: v.slug.clone(),
                    title: v.title.clone(),
                    description: v.description.clone(),
                    scan_areas: v.scan_areas.clone(),
                    include_snapshot_appendix: None,
                    chapters: v
                        .chapters
                        .iter()
                        .map(|c| GuideManifestChapter {
                            title: c.title.clone(),
                            rel_path: c
                                .source_path
                                .strip_prefix(docs_dir)
                                .unwrap_or(&c.source_path)
                                .to_string_lossy()
                                .to_string(),
                        })
                        .collect(),
                })
                .collect(),
        },
        block_4_audit: Some(GuideManifestAudit {
            required_volumes: volumes.len(),
            min_total_chapters: volumes.iter().map(|v| v.chapters.len()).sum(),
        }),
        block_5_closing: Some(GuideManifestClosing {
            publisher: "CreativeWorkzStudio Publishing".to_string(),
            status: "APPROVED".to_string(),
            compilation_targets: vec![
                "unified_compendium".to_string(),
                "volume_books".to_string(),
            ],
        }),
    };

    let json = serde_json::to_string_pretty(&manifest)
        .map_err(|e| format!("Serialization error: {}", e))?;
    fs::write(manifest_path, json)
        .map_err(|e| format!("Failed to save manifest to {:?}: {}", manifest_path, e))?;
    Ok(())
}

/// Fallback hardcoded volume list when manifest file cannot be loaded.
pub fn build_canonical_volumes(docs: &Path) -> Vec<VolumeSpec> {
    let mut chapter_id = 1;
    let mut make_ch = |title: &str, rel_path: &str| -> ChapterSpec {
        let ch = ChapterSpec {
            number: chapter_id,
            title: title.to_string(),
            source_path: docs.join(rel_path),
        };
        chapter_id += 1;
        ch
    };

    vec![
        VolumeSpec {
            roman: "I".to_string(),
            slug: "world_canon_and_continental_topography".to_string(),
            title: "World Canon, Metaphysics & Continental Topography".to_string(),
            description: "Cosmic neutrality of Lux and Tenebris, Kingdom of Sacyra, realms, and The Dark Lord.".to_string(),
            scan_areas: vec!["lore/realms".to_string()],
            chapters: vec![
                make_ch("The Primordial Neutrality of Lux and Tenebris", "lore/world_and_lore.adoc"),
            ],
        },
        VolumeSpec {
            roman: "II".to_string(),
            slug: "dramatis_personae_and_dual_protagonists".to_string(),
            title: "Dramatis Personae & Dual Protagonist Dynamics".to_string(),
            description: "Elijah the Solitary Wanderer, Katrina the Royal Sentinel, companion vocations, and the villainy of Usuru.".to_string(),
            scan_areas: vec!["lore/characters".to_string()],
            chapters: vec![
                make_ch("Dual Protagonists: Elijah & Katrina", "lore/characters.adoc"),
            ],
        },
        VolumeSpec {
            roman: "III".to_string(),
            slug: "ludology_and_tactical_trpg_mechanics".to_string(),
            title: "Ludology & Tactical TRPG Combat Mechanics".to_string(),
            description: "Turn-based tactical grid combat, Moral Sway alignment, relic stances, and 5-route narrative branching.".to_string(),
            scan_areas: vec!["mechanics".to_string()],
            chapters: vec![
                make_ch("Tactical Battle Systems & Mechanics", "mechanics/combat_system.adoc"),
            ],
        },
        VolumeSpec {
            roman: "IV".to_string(),
            slug: "demo_baseline_and_historical_archive".to_string(),
            title: "The 2017–2018 Demo Baseline & Historical Archive".to_string(),
            description: "Historical RPG Maker MV demo baseline, extracted maps, actor rosters, and archeological assets.".to_string(),
            scan_areas: vec!["archive".to_string()],
            chapters: vec![
                make_ch("Historical 2017-2018 Demo Baseline Overview", "archive/demo_baseline.adoc"),
            ],
        },
        VolumeSpec {
            roman: "V".to_string(),
            slug: "software_architecture_and_5_block_protocol".to_string(),
            title: "Software Architecture & The 5-Block Invariant Protocol".to_string(),
            description: "System boundaries, layered strata, dual-frontend GUI/CLI, and pre-flight zero-heap manifests.".to_string(),
            scan_areas: vec!["architecture".to_string(), "code".to_string()],
            chapters: vec![
                make_ch("Cornerstone & Tenebris and Lux: System Architecture Overview", "architecture/ARCHITECTURE.adoc"),
                make_ch("Zero-Disk Safe-Mode BIOS, Config ROM Vaults & Autonomous Probe Loop", "architecture/resilience_bios_and_probes.adoc"),
                make_ch("Data-Driven Architecture: Dual Data Taxonomy, Layered VFS & 5-Block JSON", "architecture/data_driven_architecture.adoc"),
                make_ch("Central Manifest Registry & Pre-Flight Loose-Bolts Inspection", "architecture/preflight_and_manifest_spec.adoc"),
                make_ch("Dual-Frontend Presentation Layer: CLI, Native X11 & Movie Reel", "architecture/dual_frontend_architecture.adoc"),
            ],
        },
        VolumeSpec {
            roman: "VI".to_string(),
            slug: "engineering_chronology_and_dev_logs".to_string(),
            title: "Engineering Chronology & Development Logs".to_string(),
            description: "Historical DevLogs, milestone post-mortems, Rev 5 modernization chronology, and release ledger.".to_string(),
            scan_areas: vec!["devlogs".to_string()],
            chapters: vec![
                make_ch("Engineering DevLog & Chronology", "devlogs/devlog_index.adoc"),
            ],
        },
    ]
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Schema & Diagnostic Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Unit Test Suite (Four-Section Ontological Schema)
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_defaults_and_construction() {
        let pub_settings = PublicationSettings::default();
        assert_eq!(pub_settings.doctype, "book");
        assert_eq!(pub_settings.master_title, default_master_title());
        assert!(pub_settings.shift_chapter_headings);

        let snap_settings = SnapshotSettings::default();
        assert_eq!(snap_settings.output_dir, default_snapshots_dir());
        assert!(snap_settings.include_wholesale_appendix);

        let cfg = default_config(Path::new("."));
        assert_eq!(cfg.toc_levels, 2);
        assert_eq!(cfg.sectnum_levels, 3);
        assert_eq!(cfg.pdf_theme_name, "cornerstone-book");
    }

    #[test]
    fn test_section_1_nominal_canonical_volumes() {
        let vols = build_canonical_volumes(Path::new("docs"));
        assert!(!vols.is_empty());
        for v in &vols {
            assert!(is_valid_volume_spec(v));
            assert!(!v.chapters.is_empty());
        }
    }

    #[test]
    fn test_section_1_nominal_manifest_serialization_round_trip() {
        let manifest = GuideManifest {
            block_1_metadata: GuideManifestMetadata {
                schema: "tenlux.test".to_string(),
                version: "0.1.0".to_string(),
                interface_rev: "Rev 5".to_string(),
                date: "2026-11-15".to_string(),
                author: "Test Author".to_string(),
                title: "Test Guide".to_string(),
                invariants: vec!["Invariant 1".to_string()],
            },
            block_2_types: Some(GuideManifestTypes {
                formats: vec!["adoc".to_string()],
                pdf_engine: Some("asciidoctor-pdf".to_string()),
                default_pdf_theme: Some("cornerstone-book".to_string()),
                pdf_themes_dir: Some("docs/themes".to_string()),
                pdf_theme_data_file: None,
                toc_levels: Some(2),
                sectnum_levels: Some(3),
                line_discipline_max_cols: Some(80),
                publication: Some(PublicationSettings::default()),
                snapshots: Some(SnapshotSettings::default()),
                exclude_scan_dirs: vec![],
            }),
            block_3_body: GuideManifestBody {
                volumes: vec![GuideManifestVolume {
                    roman: "I".to_string(),
                    slug: "vol_i".to_string(),
                    title: "Volume I".to_string(),
                    description: "First Volume".to_string(),
                    scan_areas: vec![],
                    include_snapshot_appendix: None,
                    chapters: vec![GuideManifestChapter {
                        title: "Chapter 1".to_string(),
                        rel_path: "ch1.adoc".to_string(),
                    }],
                }],
            },
            block_4_audit: Some(GuideManifestAudit {
                required_volumes: 1,
                min_total_chapters: 1,
            }),
            block_5_closing: Some(GuideManifestClosing {
                publisher: "Test".to_string(),
                status: "APPROVED".to_string(),
                compilation_targets: vec!["pdf".to_string()],
            }),
        };

        let json = serde_json::to_string(&manifest).expect("Serialization must succeed");
        let parsed: GuideManifest = serde_json::from_str(&json).expect("Deserialization must succeed");
        assert_eq!(parsed.block_1_metadata.title, "Test Guide");
        assert_eq!(parsed.block_3_body.volumes.len(), 1);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_manifest_file() {
        let bad_path = Path::new("/definitely/nonexistent/manifest.json");
        let res = load_manifest_and_config(Path::new("."), Path::new("docs"), bad_path);
        assert!(res.is_err());
        assert!(res.unwrap_err().contains("Unable to read guide manifest"));
    }

    #[test]
    fn test_section_2_never_fail_corrupted_json_content() {
        let temp_dir = std::env::temp_dir();
        let bad_file = temp_dir.join("corrupted_guide_manifest.json");
        fs::write(&bad_file, "{ not valid json").expect("Write must succeed");

        let res = load_manifest_and_config(Path::new("."), Path::new("docs"), &bad_file);
        let _ = fs::remove_file(bad_file);

        assert!(res.is_err());
        assert!(res.unwrap_err().contains("Failed to parse JSON guide manifest"));
    }

    #[test]
    fn test_section_2_never_fail_invalid_volume_predicates() {
        let bad_vol = VolumeSpec {
            roman: "   ".to_string(),
            slug: "".to_string(),
            title: "".to_string(),
            description: "".to_string(),
            scan_areas: vec![],
            chapters: vec![],
        };
        assert!(!is_valid_volume_spec(&bad_vol));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_constants_non_empty() {
        assert!(!default_master_title().is_empty());
        assert!(!default_author().is_empty());
        assert!(!default_revnumber().is_empty());
        assert!(!default_revdate().is_empty());
        assert!(!default_doctype().is_empty());
        assert!(!default_toc().is_empty());
        assert!(!default_icons().is_empty());
        assert!(!default_highlighter().is_empty());
        assert!(!default_snapshots_manifest().is_empty());
        assert!(!default_snapshots_dir().is_empty());
        assert!(!default_appendix_title().is_empty());
        assert!(!default_theme_name().is_empty());
        assert!(!default_theme_extends().is_empty());
        assert!(!default_theme_yaml_path().is_empty());
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_save_and_load_round_trip() {
        let temp_dir = std::env::temp_dir();
        let manifest_path = temp_dir.join("round_trip_guide_manifest.json");
        let vols = build_canonical_volumes(Path::new("docs"));

        let save_res = save_default_manifest(Path::new("docs"), &manifest_path, &vols);
        assert!(save_res.is_ok());

        let load_res = load_manifest_and_config(Path::new("."), Path::new("docs"), &manifest_path);
        let _ = fs::remove_file(manifest_path);

        assert!(load_res.is_ok());
        let (loaded_vols, cfg) = load_res.unwrap();
        assert_eq!(loaded_vols.len(), vols.len());
        assert_eq!(cfg.toc_levels, 2);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of guide manifest invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::manifest::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    if default_master_title().is_empty() {
        return Err("default_master_title must not be empty".to_string());
    }
    if default_author().is_empty() {
        return Err("default_author must not be empty".to_string());
    }
    if default_theme_name().is_empty() {
        return Err("default_theme_name must not be empty".to_string());
    }
    let canonical = build_canonical_volumes(Path::new("docs"));
    if canonical.is_empty() {
        return Err("build_canonical_volumes returned empty volume set".to_string());
    }
    for vol in &canonical {
        if !is_valid_volume_spec(vol) {
            return Err(format!("Canonical volume spec invalid: {:?}", vol.roman));
        }
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Verifies that the guide manifest sub-assembly is ready for operation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::manifest::is_manifest_subsystem_ready;
///
/// assert!(is_manifest_subsystem_ready());
/// ```
pub fn is_manifest_subsystem_ready() -> bool {
    self_audit().is_ok()
}
