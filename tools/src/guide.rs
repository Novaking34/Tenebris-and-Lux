// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide.rs
// @id            tools.guide.compiler
// @layer         Layer 3: Systems Developer Tooling & Document Compilers
// @version       0.2.0-dev
// @interface_rev Rev 5
// @date          2026-11-12
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Data-Driven Master Guide, Code Snapshot & PDF Book Publisher
// @role          Document Compiler, Theme Synthesizer, Snapshot Extractor & MD/PDF Publisher
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Data-driven textbook and compendium publisher for Tenebris and Lux and the
//   Cornerstone Game Engine. Driven by JSON manifests (`master_guide_manifest.json`,
//   `guide_theme_config.json`, `code_snapshots_manifest.json`), it synchronizes:
//     1. Live source code snapshots across 5 granularities (whole, block,
//        subassembly, function, group) into `docs/snapshots/*.adoc`
//     2. Asciidoctor-PDF textbook YAML themes into `docs/themes/*.yml`
//     3. Compiled AsciiDoc master/volume books with accurate book chapter nesting
//     4. Fence-aware, comment-clean GitHub Markdown mirrors for Gemini/NotebookLM
//     5. Publication PDFs via `asciidoctor-pdf` with wholesale snapshot appendices
//
// PEDAGOGICAL MISSION & AXIOMATIC INVARIANTS:
//   1. Full AsciiDoc Fidelity: Admonitions, source callouts, sidebars, and tables
//      render cleanly across standalone .adoc, book .adoc, .md, and .pdf.
//   2. PDF Whitespace Optimization: Shifts chapter sections (+1 under `== Chapter`)
//      and synthesizes compact textbook themes so subsections never trigger
//      spurious blank chapter breaks.
//   3. Textbook Code Snapshots: Extracts real C/CMake/JSON/Rust spans wholesale
//      or as annotated inline excerpts cross-linked to Appendix A plates.
//   4. 5-Block AsciiDoc Support: Recognizes `//// ... ////` block comments and
//      `// ...` sub-assembly markers cleanly without leaking comments to Markdown.
// =============================================================================

//! # Data-Driven Master Guide, Snapshot Extractor & Multi-Volume Publisher
//!
//! Compiles standalone chapters, dynamic filesystem areas, data-driven themes,
//! and live code snapshots into unified compendiums (`MASTER_GUIDE.*`) and
//! volume textbooks (`docs/volumes/VOLUME_*.*`).

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & DATA SCHEMAS
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};
use serde::{Deserialize, Serialize};
use std::collections::{HashMap, HashSet};
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

/// Specification for a single chapter included within a master volume.
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

/// JSON manifest serialization structures following the 5-block envelope.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GuideManifestChapter {
    pub title: String,
    pub rel_path: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
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

#[derive(Debug, Clone, Serialize, Deserialize)]
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

#[derive(Debug, Clone, Serialize, Deserialize)]
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

#[derive(Debug, Clone, Serialize, Deserialize)]
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

#[derive(Debug, Clone, Serialize, Deserialize)]
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

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GuideManifestBody {
    pub volumes: Vec<GuideManifestVolume>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GuideManifestAudit {
    #[serde(default)]
    pub required_volumes: usize,
    #[serde(default)]
    pub min_total_chapters: usize,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GuideManifestClosing {
    #[serde(default)]
    pub publisher: String,
    #[serde(default)]
    pub status: String,
    #[serde(default)]
    pub compilation_targets: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
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

/// Data-driven PDF textbook theme specification (`docs/guide_theme_config.json`).
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GuideThemeManifest {
    #[serde(default, alias = "block_2_opening")]
    pub block_2_types: Option<GuideThemeMeta>,
    pub block_3_body: GuideThemeBody,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GuideThemeMeta {
    #[serde(default = "default_theme_name")]
    pub theme_name: String,
    #[serde(default = "default_theme_extends")]
    pub extends: String,
    #[serde(default = "default_theme_yaml_path")]
    pub generated_yaml_path: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GuideThemeBody {
    pub page: ThemePage,
    pub typography: ThemeTypography,
    pub spacing: ThemeSpacing,
    pub breaks: ThemeBreaks,
    pub blocks: ThemeBlocks,
    pub running_chrome: ThemeRunningChrome,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ThemePage {
    pub size: String,
    pub layout: String,
    pub margin_top_in: f64,
    pub margin_right_in: f64,
    pub margin_bottom_in: f64,
    pub margin_left_in: f64,
    pub background_color: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ThemeTypography {
    pub font_color: String,
    pub base_font_size: f64,
    pub line_height_length: f64,
    pub text_align: String,
    pub link_color: String,
    pub codespan_color: String,
    pub code_font_size: f64,
    pub code_line_height: f64,
    pub code_padding: f64,
    pub code_line_gap: f64,
    pub code_background_color: String,
    pub code_border_color: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ThemeSpacing {
    pub vertical_rhythm: f64,
    pub horizontal_rhythm: f64,
    pub block_margin_bottom: f64,
    pub prose_margin_bottom: f64,
    pub list_item_spacing: f64,
    pub heading_margin_top: f64,
    pub heading_margin_bottom: f64,
    pub heading_min_height_after: f64,
    pub heading_h1_scale: f64,
    pub heading_h2_scale: f64,
    pub heading_h3_scale: f64,
    pub heading_h4_scale: f64,
    pub heading_h5_scale: f64,
    pub heading_h6_scale: f64,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ThemeBreaks {
    pub part_break_before: String,
    pub chapter_break_before: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ThemeBlocks {
    pub admonition_padding: [f64; 4],
    pub admonition_rule_color: String,
    pub admonition_rule_width: f64,
    pub sidebar_background_color: String,
    pub sidebar_border_color: String,
    pub sidebar_padding: [f64; 4],
    pub table_cell_padding: [f64; 4],
    pub table_border_color: String,
    pub table_stripe_background_color: String,
    pub table_head_background_color: String,
    pub toc_line_height: f64,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ThemeRunningChrome {
    pub header_height: f64,
    pub header_font_size: f64,
    pub header_font_color: String,
    pub header_border_color: String,
    pub header_left: String,
    pub header_right: String,
    pub footer_height: f64,
    pub footer_font_size: f64,
    pub footer_font_color: String,
    pub footer_border_color: String,
    pub footer_left: String,
    pub footer_right: String,
}

/// Supported source code snapshot granularities.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum SnapshotMode {
    Whole,
    Block,
    Subassembly,
    Function,
    Group,
}

/// Policy controlling whether a snapshot's full wholesale text appears in Appendix A.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize, Default)]
#[serde(rename_all = "snake_case")]
pub enum WholesalePolicy {
    Always,
    #[default]
    WhenExcerpted,
    Never,
}

/// Optional focused excerpt configuration for long whole/block/group snapshots.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SnapshotExcerptSpec {
    pub mode: String,
    #[serde(default)]
    pub target: Option<String>,
    #[serde(default)]
    pub start_line: Option<usize>,
    #[serde(default)]
    pub end_line: Option<usize>,
    #[serde(default)]
    pub max_lines: Option<usize>,
}

/// Inline callout badge mapped by pattern to textbook footnote commentary.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SnapshotCalloutSpec {
    #[serde(rename = "match")]
    pub pattern: String,
    pub number: u32,
    pub text: String,
}

/// Single code snapshot declaration from `docs/code_snapshots_manifest.json`.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CodeSnapshotSpec {
    pub id: String,
    pub title: String,
    pub source_file: String,
    pub mode: SnapshotMode,
    #[serde(default)]
    pub target: Option<String>,
    #[serde(default)]
    pub targets: Vec<String>,
    #[serde(default)]
    pub language: Option<String>,
    #[serde(default)]
    pub excerpt: Option<SnapshotExcerptSpec>,
    #[serde(default)]
    pub wholesale_in_appendix: WholesalePolicy,
    #[serde(default)]
    pub callout_note: Option<String>,
    #[serde(default)]
    pub callouts: Vec<SnapshotCalloutSpec>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CodeSnapshotsBody {
    pub snapshots: Vec<CodeSnapshotSpec>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CodeSnapshotsManifest {
    pub block_3_body: CodeSnapshotsBody,
}

/// Extracted contiguous line segment retaining authentic 1-based file line numbers.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ExtractedSegment {
    pub start_line: usize,
    pub end_line: usize,
    pub lines: Vec<String>,
}

/// Materialized snapshot holding both inline plate and full wholesale extraction.
#[derive(Debug, Clone)]
pub struct RenderedSnapshot {
    pub spec: CodeSnapshotSpec,
    pub lang: String,
    pub inline_segments: Vec<ExtractedSegment>,
    pub wholesale_segments: Vec<ExtractedSegment>,
    pub was_excerpted: bool,
    pub total_source_lines: usize,
    pub plate_path: PathBuf,
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

/// PDF single-page geometry and layout validation contract (`docs/adoc_pdf_layout_rules.json`).
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AdocPageGeometry {
    #[serde(default = "default_code_cols_warn")]
    pub safe_code_cols_with_linenums: usize,
    #[serde(default = "default_code_cols_no_ln")]
    pub safe_code_cols_no_linenums: usize,
    #[serde(default = "default_code_cols_hard")]
    pub hard_code_cols_max: usize,
    #[serde(default = "default_snap_rec_lines")]
    pub recommended_inline_snapshot_lines: usize,
    #[serde(default = "default_snap_max_lines")]
    pub max_inline_snapshot_lines: usize,
}

impl Default for AdocPageGeometry {
    fn default() -> Self {
        Self {
            safe_code_cols_with_linenums: default_code_cols_warn(),
            safe_code_cols_no_linenums: default_code_cols_no_ln(),
            hard_code_cols_max: default_code_cols_hard(),
            recommended_inline_snapshot_lines: default_snap_rec_lines(),
            max_inline_snapshot_lines: default_snap_max_lines(),
        }
    }
}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct AdocLayoutOpening {
    #[serde(default)]
    pub strict_target_prefixes: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AdocValidationRules {
    #[serde(default = "default_true")]
    pub require_balanced_delimiters: bool,
    #[serde(default = "default_true")]
    pub forbid_spurious_abstract_headings: bool,
    #[serde(default = "default_true")]
    pub forbid_midfile_level0_headings: bool,
    #[serde(default = "default_true")]
    pub forbid_thematic_break_in_strict_targets: bool,
    #[serde(default = "default_true")]
    pub require_5block_banners_in_strict_targets: bool,
    #[serde(default)]
    pub required_block_tokens: Vec<String>,
    #[serde(default = "default_true")]
    pub verify_include_paths_exist: bool,
    #[serde(default = "default_true")]
    pub enforce_snapshot_line_and_width_budgets: bool,
    #[serde(default = "default_true")]
    pub forbid_listing_delimiter_collisions: bool,
}

impl Default for AdocValidationRules {
    fn default() -> Self {
        Self {
            require_balanced_delimiters: true,
            forbid_spurious_abstract_headings: true,
            forbid_midfile_level0_headings: true,
            forbid_thematic_break_in_strict_targets: true,
            require_5block_banners_in_strict_targets: true,
            required_block_tokens: vec![
                "BLOCK 1:".to_string(),
                "BLOCK 2:".to_string(),
                "BLOCK 3:".to_string(),
                "BLOCK 4:".to_string(),
                "BLOCK 5:".to_string(),
            ],
            verify_include_paths_exist: true,
            enforce_snapshot_line_and_width_budgets: true,
            forbid_listing_delimiter_collisions: true,
        }
    }
}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct AdocLayoutBody {
    #[serde(default)]
    pub page_geometry: AdocPageGeometry,
    #[serde(default)]
    pub validation_rules: AdocValidationRules,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AdocLayoutRulesFile {
    #[serde(default)]
    pub block_2_opening: AdocLayoutOpening,
    #[serde(default)]
    pub block_3_body: AdocLayoutBody,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum LayoutSeverity {
    Error,
    Warning,
}

#[derive(Debug, Clone)]
pub struct AdocLayoutIssue {
    pub file: String,
    pub line: usize,
    pub severity: LayoutSeverity,
    pub rule: String,
    pub message: String,
}

#[derive(Debug, Clone, Default)]
pub struct AdocLayoutReport {
    pub files_checked: usize,
    pub snapshots_checked: usize,
    pub issues: Vec<AdocLayoutIssue>,
}

impl AdocLayoutReport {
    pub fn error_count(&self) -> usize {
        self.issues
            .iter()
            .filter(|i| i.severity == LayoutSeverity::Error)
            .count()
    }

    pub fn warning_count(&self) -> usize {
        self.issues
            .iter()
            .filter(|i| i.severity == LayoutSeverity::Warning)
            .count()
    }

    pub fn is_clean(&self) -> bool {
        self.error_count() == 0
    }
}

fn default_code_cols_warn() -> usize { 104 }
fn default_code_cols_no_ln() -> usize { 112 }
fn default_code_cols_hard() -> usize { 128 }
fn default_snap_rec_lines() -> usize { 55 }
fn default_snap_max_lines() -> usize { 68 }

fn default_true() -> bool { true }
fn default_master_title() -> String { "Tenebris and Lux: Comprehensive Master Guide".to_string() }
fn default_author() -> String { "CreativeWorkzStudio LLC / ProfessorSeanEX".to_string() }
fn default_revnumber() -> String { "0.2.0-dev".to_string() }
fn default_revdate() -> String { "2026-11-12".to_string() }
fn default_doctype() -> String { "book".to_string() }
fn default_toc() -> String { "left".to_string() }
fn default_icons() -> String { "font".to_string() }
fn default_highlighter() -> String { "rouge".to_string() }
fn default_snapshots_manifest() -> String { "docs/code_snapshots_manifest.json".to_string() }
fn default_snapshots_dir() -> String { "docs/snapshots".to_string() }
fn default_appendix_title() -> String { "Wholesale Source Code Reference Plates".to_string() }
fn default_theme_name() -> String { "cornerstone-book".to_string() }
fn default_theme_extends() -> String { "default".to_string() }
fn default_theme_yaml_path() -> String { "docs/themes/cornerstone-book-theme.yml".to_string() }

// =============================================================================
// BLOCK 3: BODY BLOCK / COMPILER, THEME ENGINE, SNAPSHOTS & TRANSLATOR
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

/// Core builder aggregating documentation into unified master and volume outputs.
pub struct MasterGuideBuilder {
    pub repo_root: PathBuf,
    pub docs_dir: PathBuf,
    pub volumes_dir: PathBuf,
    pub volumes: Vec<VolumeSpec>,
    pub config: GuideConfig,
}

impl MasterGuideBuilder {
    /// Initializes the Master Guide builder pointing to the repository root.
    pub fn new(repo_root: &Path) -> Self {
        let docs_dir = repo_root.join("docs");
        let volumes_dir = docs_dir.join("volumes");
        let manifest_path = docs_dir.join("master_guide_manifest.json");

        let (volumes, config) = if manifest_path.is_file() {
            match Self::load_manifest_and_config(repo_root, &docs_dir, &manifest_path) {
                Ok(res) => res,
                Err(err) => {
                    eprintln!(
                        "  [WARN] Failed to load {:?}: {}. Using canonical fallback.",
                        manifest_path, err
                    );
                    (Self::build_canonical_volumes(&docs_dir), Self::default_config(repo_root))
                }
            }
        } else {
            let fallback_vols = Self::build_canonical_volumes(&docs_dir);
            let _ = Self::save_default_manifest(&docs_dir, &manifest_path, &fallback_vols);
            (fallback_vols, Self::default_config(repo_root))
        };

        Self {
            repo_root: repo_root.to_path_buf(),
            docs_dir,
            volumes_dir,
            volumes,
            config,
        }
    }

    fn default_config(repo_root: &Path) -> GuideConfig {
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
    pub fn load_volumes_from_manifest(docs_dir: &Path, path: &Path) -> Result<Vec<VolumeSpec>, String> {
        let repo_root = docs_dir.parent().unwrap_or(docs_dir);
        let (vols, _) = Self::load_manifest_and_config(repo_root, docs_dir, path)?;
        Ok(vols)
    }

    fn load_manifest_and_config(
        repo_root: &Path,
        docs_dir: &Path,
        path: &Path,
    ) -> Result<(Vec<VolumeSpec>, GuideConfig), String> {
        let raw = fs::read_to_string(path)
            .map_err(|e| format!("Unable to read guide manifest file: {}", e))?;
        let manifest: GuideManifest = serde_json::from_str(&raw)
            .map_err(|e| format!("Failed to parse JSON guide manifest: {}", e))?;

        let mut cfg = Self::default_config(repo_root);
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
                cfg.volume_appendix_overrides.insert(mv.roman.clone(), override_flag);
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
                let scanned = Self::scan_area_for_chapters_with_excludes(
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

    /// Recursively scans an area directory relative to docs/ for .adoc and .md chapters.
    pub fn scan_area_for_chapters(docs_dir: &Path, area_rel: &str) -> Vec<(String, PathBuf)> {
        let excludes = vec![
            "snapshots".to_string(),
            "themes".to_string(),
            "volumes".to_string(),
            "offline_work".to_string(),
        ];
        Self::scan_area_for_chapters_with_excludes(docs_dir, area_rel, &excludes)
    }

    fn scan_area_for_chapters_with_excludes(
        docs_dir: &Path,
        area_rel: &str,
        excludes: &[String],
    ) -> Vec<(String, PathBuf)> {
        let target_dir = docs_dir.join(area_rel);
        let mut results = Vec::new();
        if !target_dir.is_dir() {
            return results;
        }

        Self::scan_dir_recursive(&target_dir, excludes, &mut results);
        results.sort_by(|a, b| a.1.cmp(&b.1));
        results
    }

    fn scan_dir_recursive(dir: &Path, excludes: &[String], out: &mut Vec<(String, PathBuf)>) {
        if let Ok(entries) = fs::read_dir(dir) {
            for entry in entries.flatten() {
                let path = entry.path();
                let name = path.file_name().and_then(|s| s.to_str()).unwrap_or("");
                if path.is_dir() {
                    if excludes.iter().any(|ex| ex.eq_ignore_ascii_case(name)) {
                        continue;
                    }
                    Self::scan_dir_recursive(&path, excludes, out);
                } else if path.is_file() {
                    let ext = path.extension().and_then(|s| s.to_str()).unwrap_or("");
                    if ext == "adoc" || ext == "md" {
                        if name.eq_ignore_ascii_case("readme.adoc")
                            || name.eq_ignore_ascii_case("readme.md")
                        {
                            continue;
                        }
                        let title = Self::extract_title_from_file(&path);
                        out.push((title, path));
                    }
                }
            }
        }
    }

    /// Extracts clean human-readable chapter titles, skipping leading 5-block //// comments.
    pub fn extract_title_from_file(path: &Path) -> String {
        if let Ok(content) = fs::read_to_string(path) {
            let mut in_comment_block = false;
            for line in content.lines().take(120) {
                let trimmed = line.trim();
                if trimmed == "////" {
                    in_comment_block = !in_comment_block;
                    continue;
                }
                if in_comment_block || trimmed.starts_with("//") || trimmed.is_empty() {
                    continue;
                }
                if trimmed.starts_with("= ") && !trimmed.starts_with("== ") {
                    return trimmed[2..].trim().to_string();
                } else if trimmed.starts_with("# ") && !trimmed.starts_with("## ") {
                    return trimmed[2..].trim().to_string();
                }
            }
        }

        let stem = path.file_stem().and_then(|s| s.to_str()).unwrap_or("Chapter");
        stem.replace('_', " ").replace('-', " ")
    }

    // -------------------------------------------------------------------------
    // Theme Synthesizer (JSON -> Asciidoctor-PDF YAML Theme)
    // -------------------------------------------------------------------------

    /// Compiles `docs/guide_theme_config.json` into `docs/themes/<name>-theme.yml`.
    pub fn sync_theme_yaml(&self) -> Result<Option<PathBuf>, String> {
        let Some(ref cfg_path) = self.config.pdf_theme_data_file else {
            return Ok(None);
        };
        if !cfg_path.is_file() {
            return Ok(None);
        }

        let raw = fs::read_to_string(cfg_path)
            .map_err(|e| format!("Failed to read PDF theme data {:?}: {}", cfg_path, e))?;
        let manifest: GuideThemeManifest = serde_json::from_str(&raw)
            .map_err(|e| format!("Failed to parse {:?}: {}", cfg_path, e))?;

        let meta = manifest.block_2_types.unwrap_or(GuideThemeMeta {
            theme_name: self.config.pdf_theme_name.clone(),
            extends: default_theme_extends(),
            generated_yaml_path: default_theme_yaml_path(),
        });
        let yaml_path = self.repo_root.join(&meta.generated_yaml_path);
        if let Some(parent) = yaml_path.parent() {
            fs::create_dir_all(parent)
                .map_err(|e| format!("Failed to create theme dir {:?}: {}", parent, e))?;
        }

        let yaml = Self::render_theme_yaml(&meta.extends, &manifest.block_3_body);
        fs::write(&yaml_path, yaml)
            .map_err(|e| format!("Failed to write theme YAML {:?}: {}", yaml_path, e))?;

        Ok(Some(yaml_path))
    }

    /// Formats a verified Asciidoctor-PDF 2.3+ theme YAML extending `default`.
    pub fn render_theme_yaml(extends: &str, body: &GuideThemeBody) -> String {
        let p = &body.page;
        let t = &body.typography;
        let s = &body.spacing;
        let br = &body.breaks;
        let bl = &body.blocks;
        let rc = &body.running_chrome;

        format!(
            "# Auto-generated from docs/guide_theme_config.json by tools/src/guide.rs\n\
             extends: {extends}\n\
             page:\n\
             \x20 size: {size}\n\
             \x20 layout: {layout}\n\
             \x20 background_color: {bg}\n\
             \x20 margin: [{mt}in, {mr}in, {mb}in, {ml}in]\n\
             \x20 margin_inner: {ml}in\n\
             \x20 margin_outer: {mr}in\n\
             base:\n\
             \x20 text_align: {align}\n\
             \x20 font_color: {fc}\n\
             \x20 font_size: {bfs}\n\
             \x20 line_height_length: {lhl}\n\
             \x20 line_height: $base_line_height_length / {bfs}\n\
             vertical_rhythm: {vr}\n\
             horizontal_rhythm: {hr}\n\
             link:\n\
             \x20 font_color: {lc}\n\
             codespan:\n\
             \x20 font_color: {cc}\n\
             heading:\n\
             \x20 text_align: left\n\
             \x20 font_color: {fc}\n\
             \x20 h1_font_size: floor($base_font_size * {h1})\n\
             \x20 h2_font_size: floor($base_font_size * {h2})\n\
             \x20 h3_font_size: round($base_font_size * {h3})\n\
             \x20 h4_font_size: round($base_font_size * {h4})\n\
             \x20 h5_font_size: round($base_font_size * {h5})\n\
             \x20 h6_font_size: round($base_font_size * {h6})\n\
             \x20 margin_top: {hmt}\n\
             \x20 margin_bottom: {hmb}\n\
             \x20 min_height_after: {hmha}\n\
             \x20 part_break_before: {pbb}\n\
             \x20 chapter_break_before: {cbb}\n\
             block:\n\
             \x20 margin_bottom: {bmb}\n\
             prose:\n\
             \x20 margin_bottom: {pmb}\n\
             list:\n\
             \x20 item_spacing: {lis}\n\
             code:\n\
             \x20 font_size: {cfs}\n\
             \x20 line_height: {clh}\n\
             \x20 padding: {cpad}\n\
             \x20 line_gap: {cgap}\n\
             \x20 background_color: {cbg}\n\
             \x20 border_color: {cbd}\n\
             \x20 border_width: 0.6\n\
             admonition:\n\
             \x20 column_rule_color: {arule}\n\
             \x20 column_rule_width: {awid}\n\
             \x20 padding: [{ap0}, {ap1}, {ap2}, {ap3}]\n\
             sidebar:\n\
             \x20 background_color: {sbg}\n\
             \x20 border_color: {sbd}\n\
             \x20 padding: [{sp0}, {sp1}, {sp2}, {sp3}]\n\
             table:\n\
             \x20 border_color: {tbd}\n\
             \x20 cell_padding: [{tp0}, {tp1}, {tp2}, {tp3}]\n\
             \x20 head:\n\
             \x20   background_color: {thbg}\n\
             \x20   font_style: bold\n\
             \x20 body:\n\
             \x20   stripe_background_color: {tsbg}\n\
             toc:\n\
             \x20 line_height: {toclh}\n\
             header:\n\
             \x20 height: {hh}\n\
             \x20 font_size: {hfs}\n\
             \x20 font_color: {hfc}\n\
             \x20 border_color: {hbc}\n\
             \x20 border_width: 0.25\n\
             \x20 vertical_align: middle\n\
             \x20 recto:\n\
             \x20   left:\n\
             \x20     content: '{hl}'\n\
             \x20   right:\n\
             \x20     content: '{hr_txt}'\n\
             \x20 verso:\n\
             \x20   left:\n\
             \x20     content: '{hr_txt}'\n\
             \x20   right:\n\
             \x20     content: '{hl}'\n\
             footer:\n\
             \x20 height: {fh}\n\
             \x20 font_size: {ffs}\n\
             \x20 font_color: {ffc}\n\
             \x20 border_color: {fbc}\n\
             \x20 border_width: 0.25\n\
             \x20 vertical_align: middle\n\
             \x20 recto:\n\
             \x20   left:\n\
             \x20     content: '{fl}'\n\
             \x20   right:\n\
             \x20     content: '{fr}'\n\
             \x20 verso:\n\
             \x20   left:\n\
             \x20     content: '{fr}'\n\
             \x20   right:\n\
             \x20     content: '{fl}'\n",
            extends = extends,
            size = p.size,
            layout = p.layout,
            bg = p.background_color,
            mt = p.margin_top_in,
            mr = p.margin_right_in,
            mb = p.margin_bottom_in,
            ml = p.margin_left_in,
            align = t.text_align,
            fc = t.font_color,
            bfs = t.base_font_size,
            lhl = t.line_height_length,
            vr = s.vertical_rhythm,
            hr = s.horizontal_rhythm,
            lc = t.link_color,
            cc = t.codespan_color,
            h1 = s.heading_h1_scale,
            h2 = s.heading_h2_scale,
            h3 = s.heading_h3_scale,
            h4 = s.heading_h4_scale,
            h5 = s.heading_h5_scale,
            h6 = s.heading_h6_scale,
            hmt = s.heading_margin_top,
            hmb = s.heading_margin_bottom,
            hmha = s.heading_min_height_after,
            pbb = br.part_break_before,
            cbb = br.chapter_break_before,
            bmb = s.block_margin_bottom,
            pmb = s.prose_margin_bottom,
            lis = s.list_item_spacing,
            cfs = t.code_font_size,
            clh = t.code_line_height,
            cpad = t.code_padding,
            cgap = t.code_line_gap,
            cbg = t.code_background_color,
            cbd = t.code_border_color,
            arule = bl.admonition_rule_color,
            awid = bl.admonition_rule_width,
            ap0 = bl.admonition_padding[0],
            ap1 = bl.admonition_padding[1],
            ap2 = bl.admonition_padding[2],
            ap3 = bl.admonition_padding[3],
            sbg = bl.sidebar_background_color,
            sbd = bl.sidebar_border_color,
            sp0 = bl.sidebar_padding[0],
            sp1 = bl.sidebar_padding[1],
            sp2 = bl.sidebar_padding[2],
            sp3 = bl.sidebar_padding[3],
            tbd = bl.table_border_color,
            tp0 = bl.table_cell_padding[0],
            tp1 = bl.table_cell_padding[1],
            tp2 = bl.table_cell_padding[2],
            tp3 = bl.table_cell_padding[3],
            thbg = bl.table_head_background_color,
            tsbg = bl.table_stripe_background_color,
            toclh = bl.toc_line_height,
            hh = rc.header_height,
            hfs = rc.header_font_size,
            hfc = rc.header_font_color,
            hbc = rc.header_border_color,
            hl = rc.header_left,
            hr_txt = rc.header_right,
            fh = rc.footer_height,
            ffs = rc.footer_font_size,
            ffc = rc.footer_font_color,
            fbc = rc.footer_border_color,
            fl = rc.footer_left,
            fr = rc.footer_right,
        )
    }

    // -------------------------------------------------------------------------
    // 5-Mode Textbook Code Snapshot Engine
    // ( whole | block | subassembly | function | group )
    // -------------------------------------------------------------------------

    /// Loads `docs/code_snapshots_manifest.json` and writes live plates into `docs/snapshots/`.
    pub fn prepare_snapshots(&self) -> Result<Vec<RenderedSnapshot>, String> {
        let manifest_path = self.repo_root.join(&self.config.snapshots.manifest_path);
        if !manifest_path.is_file() {
            return Ok(Vec::new());
        }

        let raw = fs::read_to_string(&manifest_path)
            .map_err(|e| format!("Failed to read snapshots manifest {:?}: {}", manifest_path, e))?;
        let manifest: CodeSnapshotsManifest = serde_json::from_str(&raw)
            .map_err(|e| format!("Failed to parse snapshots manifest {:?}: {}", manifest_path, e))?;

        let out_dir = self.repo_root.join(&self.config.snapshots.output_dir);
        fs::create_dir_all(&out_dir)
            .map_err(|e| format!("Failed to create snapshots dir {:?}: {}", out_dir, e))?;

        let mut rendered = Vec::with_capacity(manifest.block_3_body.snapshots.len());
        for spec in manifest.block_3_body.snapshots {
            let snap = Self::materialize_snapshot(&self.repo_root, &out_dir, spec)?;
            let plate_text = Self::format_snapshot_plate(&snap, self.config.snapshots.show_line_numbers);
            fs::write(&snap.plate_path, plate_text)
                .map_err(|e| format!("Failed to write snapshot plate {:?}: {}", snap.plate_path, e))?;
            rendered.push(snap);
        }

        let index_text = Self::format_snapshots_index(&rendered);
        fs::write(out_dir.join("INDEX.adoc"), index_text)
            .map_err(|e| format!("Failed to write snapshot INDEX.adoc: {}", e))?;

        Ok(rendered)
    }

    /// Materializes a single `CodeSnapshotSpec` from repository disk sources.
    pub fn materialize_snapshot(
        repo_root: &Path,
        out_dir: &Path,
        spec: CodeSnapshotSpec,
    ) -> Result<RenderedSnapshot, String> {
        let src_path = repo_root.join(&spec.source_file);
        let raw = fs::read_to_string(&src_path).map_err(|e| {
            format!(
                "Snapshot '{}' could not read source '{}': {}",
                spec.id, spec.source_file, e
            )
        })?;
        let all_lines: Vec<String> = raw.lines().map(|l| l.to_string()).collect();
        let total_source_lines = all_lines.len();
        let lang = spec
            .language
            .clone()
            .unwrap_or_else(|| Self::infer_language(&spec.source_file));

        let wholesale_segments = Self::extract_segments_for_spec(&all_lines, &spec, &lang)?;
        let (inline_segments, was_excerpted) = if let Some(ref ex) = spec.excerpt {
            let segs = Self::extract_excerpt_segments(&all_lines, &wholesale_segments, ex, &lang)?;
            let inline_count: usize = segs.iter().map(|s| s.lines.len()).sum();
            let whole_count: usize = wholesale_segments.iter().map(|s| s.lines.len()).sum();
            (segs, inline_count < whole_count)
        } else {
            (wholesale_segments.clone(), false)
        };

        let plate_path = out_dir.join(format!("{}.adoc", spec.id));
        Ok(RenderedSnapshot {
            spec,
            lang,
            inline_segments,
            wholesale_segments,
            was_excerpted,
            total_source_lines,
            plate_path,
        })
    }

    pub fn infer_language(source_file: &str) -> String {
        let lower = source_file.to_ascii_lowercase();
        if lower.ends_with(".c") || lower.ends_with(".h") {
            "c".to_string()
        } else if lower.ends_with(".cpp") || lower.ends_with(".hpp") {
            "cpp".to_string()
        } else if lower.ends_with(".cmake") || lower.ends_with("cmakelists.txt") {
            "cmake".to_string()
        } else if lower.ends_with(".json") {
            "json".to_string()
        } else if lower.ends_with(".rs") {
            "rust".to_string()
        } else if lower.ends_with(".yml") || lower.ends_with(".yaml") {
            "yaml".to_string()
        } else {
            "text".to_string()
        }
    }

    fn extract_segments_for_spec(
        lines: &[String],
        spec: &CodeSnapshotSpec,
        lang: &str,
    ) -> Result<Vec<ExtractedSegment>, String> {
        match spec.mode {
            SnapshotMode::Whole => {
                if lines.is_empty() {
                    return Err(format!("Snapshot '{}': source file is empty.", spec.id));
                }
                Ok(vec![ExtractedSegment {
                    start_line: 1,
                    end_line: lines.len(),
                    lines: lines.to_vec(),
                }])
            }
            SnapshotMode::Block => {
                let target = spec
                    .target
                    .as_deref()
                    .ok_or_else(|| format!("Snapshot '{}': mode 'block' requires 'target'.", spec.id))?;
                Ok(vec![Self::extract_block_segment(lines, target, lang).ok_or_else(|| {
                    format!("Snapshot '{}': block '{}' not found in {}.", spec.id, target, spec.source_file)
                })?])
            }
            SnapshotMode::Subassembly => {
                let target = spec.target.as_deref().ok_or_else(|| {
                    format!("Snapshot '{}': mode 'subassembly' requires 'target'.", spec.id)
                })?;
                Ok(vec![Self::extract_subassembly_segment(lines, target).ok_or_else(|| {
                    format!(
                        "Snapshot '{}': subassembly '{}' not found in {}.",
                        spec.id, target, spec.source_file
                    )
                })?])
            }
            SnapshotMode::Function => {
                let target = spec.target.as_deref().ok_or_else(|| {
                    format!("Snapshot '{}': mode 'function' requires 'target'.", spec.id)
                })?;
                Ok(vec![Self::extract_symbol_segment(lines, target, lang).ok_or_else(|| {
                    format!(
                        "Snapshot '{}': function/symbol '{}' not found in {}.",
                        spec.id, target, spec.source_file
                    )
                })?])
            }
            SnapshotMode::Group => {
                if spec.targets.is_empty() {
                    return Err(format!(
                        "Snapshot '{}': mode 'group' requires non-empty 'targets' array.",
                        spec.id
                    ));
                }
                let mut segs = Vec::new();
                for name in &spec.targets {
                    let seg = Self::extract_symbol_segment(lines, name, lang).ok_or_else(|| {
                        format!(
                            "Snapshot '{}': group member '{}' not found in {}.",
                            spec.id, name, spec.source_file
                        )
                    })?;
                    segs.push(seg);
                }
                Ok(Self::merge_segments(lines, segs))
            }
        }
    }

    fn extract_excerpt_segments(
        all_lines: &[String],
        wholesale: &[ExtractedSegment],
        ex: &SnapshotExcerptSpec,
        lang: &str,
    ) -> Result<Vec<ExtractedSegment>, String> {
        let cap_segment = |mut seg: ExtractedSegment, max_opt: Option<usize>| -> ExtractedSegment {
            if let Some(max_n) = max_opt {
                if seg.lines.len() > max_n && max_n > 0 {
                    seg.lines.truncate(max_n);
                    seg.end_line = seg.start_line + seg.lines.len() - 1;
                }
            }
            seg
        };

        match ex.mode.to_ascii_lowercase().as_str() {
            "lines" => {
                let s = ex.start_line.unwrap_or(1).max(1);
                let e = ex.end_line.unwrap_or(all_lines.len()).min(all_lines.len());
                if s > e || s > all_lines.len() {
                    return Ok(wholesale.to_vec());
                }
                let slice = all_lines[s - 1..e].to_vec();
                Ok(vec![cap_segment(
                    ExtractedSegment { start_line: s, end_line: e, lines: slice },
                    ex.max_lines,
                )])
            }
            "block" => {
                if let Some(ref tgt) = ex.target {
                    if let Some(seg) = Self::extract_block_segment(all_lines, tgt, lang) {
                        return Ok(vec![cap_segment(seg, ex.max_lines)]);
                    }
                }
                Ok(wholesale.to_vec())
            }
            "subassembly" => {
                if let Some(ref tgt) = ex.target {
                    if let Some(seg) = Self::extract_subassembly_segment(all_lines, tgt) {
                        return Ok(vec![cap_segment(seg, ex.max_lines)]);
                    }
                }
                Ok(wholesale.to_vec())
            }
            "symbol" | "function" => {
                if let Some(ref tgt) = ex.target {
                    if let Some(seg) = Self::extract_symbol_segment(all_lines, tgt, lang) {
                        return Ok(vec![cap_segment(seg, ex.max_lines)]);
                    }
                }
                Ok(wholesale.to_vec())
            }
            _ => {
                if let Some(first) = wholesale.first().cloned() {
                    Ok(vec![cap_segment(first, ex.max_lines)])
                } else {
                    Ok(Vec::new())
                }
            }
        }
    }

    fn merge_segments(all_lines: &[String], mut segs: Vec<ExtractedSegment>) -> Vec<ExtractedSegment> {
        if segs.is_empty() {
            return segs;
        }
        segs.sort_by_key(|s| s.start_line);
        let mut merged = Vec::new();
        let mut cur = segs[0].clone();
        for next in segs.into_iter().skip(1) {
            if next.start_line <= cur.end_line + 2 {
                let end = cur.end_line.max(next.end_line);
                cur = ExtractedSegment {
                    start_line: cur.start_line,
                    end_line: end,
                    lines: all_lines[cur.start_line - 1..end].to_vec(),
                };
            } else {
                merged.push(cur);
                cur = next;
            }
        }
        merged.push(cur);
        merged
    }

    /// Extracts a `BLOCK N` region from C/CMake/Rust/AsciiDoc or a top-level JSON key block.
    pub fn extract_block_segment(
        lines: &[String],
        target: &str,
        lang: &str,
    ) -> Option<ExtractedSegment> {
        if lang == "json" {
            return Self::extract_json_key_block(lines, target);
        }

        let needle = format!("{}:", target.trim());
        let needle_lower = needle.to_ascii_lowercase();
        let target_lower = target.trim().to_ascii_lowercase();

        let hit_idx = lines.iter().position(|line| {
            let low = line.to_ascii_lowercase();
            (low.contains(&needle_lower) || low.contains(&target_lower))
                && (low.contains("block ") || low.trim_start().starts_with("//") || low.trim_start().starts_with('#') || low.trim_start().starts_with('*') || lines_has_comment_banner(lines, line))
        })?;

        let start_idx = Self::find_banner_start(lines, hit_idx);
        let mut end_idx = lines.len();
        for i in (hit_idx + 1)..lines.len() {
            let trimmed = lines[i].trim();
            let low = trimmed.to_ascii_lowercase();
            if low.starts_with("block ") && (low.contains(':') || low.contains("block"))
                || low.contains("block 1:")
                || low.contains("block 2:")
                || low.contains("block 3:")
                || low.contains("block 4:")
                || low.contains("block 5:")
            {
                // Make sure this isn't inside ordinary prose or same banner
                if Self::is_inside_banner_frame(lines, i) {
                    end_idx = Self::find_banner_start(lines, i);
                    break;
                }
            }
        }

        while end_idx > start_idx + 1 && lines[end_idx - 1].trim().is_empty() {
            end_idx -= 1;
        }

        Some(ExtractedSegment {
            start_line: start_idx + 1,
            end_line: end_idx,
            lines: lines[start_idx..end_idx].to_vec(),
        })
    }

    /// Extracts a top-level JSON property (`"block_3_body": { ... }`) by brace depth.
    fn extract_json_key_block(lines: &[String], key: &str) -> Option<ExtractedSegment> {
        let quoted = format!("\"{}\"", key);
        let mut depth: i32 = 0;
        let mut start_idx = None;
        let mut target_depth = 0;

        for (i, line) in lines.iter().enumerate() {
            if start_idx.is_none() && depth == 1 && line.contains(&quoted) && line.contains(':') {
                start_idx = Some(i);
                target_depth = depth;
            }

            let mut in_str = false;
            let mut esc = false;
            for ch in line.chars() {
                if esc {
                    esc = false;
                    continue;
                }
                if ch == '\\' && in_str {
                    esc = true;
                    continue;
                }
                if ch == '"' {
                    in_str = !in_str;
                    continue;
                }
                if !in_str {
                    match ch {
                        '{' | '[' => depth += 1,
                        '}' | ']' => depth -= 1,
                        _ => {}
                    }
                }
            }

            if let Some(s) = start_idx {
                if depth <= target_depth {
                    return Some(ExtractedSegment {
                        start_line: s + 1,
                        end_line: i + 1,
                        lines: lines[s..=i].to_vec(),
                    });
                }
            }
        }
        None
    }

    /// Extracts a numbered `Sub-Assembly X.Y` region until the next Sub-Assembly or BLOCK.
    pub fn extract_subassembly_segment(lines: &[String], target: &str) -> Option<ExtractedSegment> {
        let norm = if target.to_ascii_lowercase().starts_with("sub-assembly") {
            target.to_string()
        } else {
            format!("Sub-Assembly {}", target)
        };
        let norm_lower = norm.to_ascii_lowercase();

        let hit_idx = lines.iter().enumerate().position(|(idx, line)| {
            let low = line.to_ascii_lowercase();
            low.contains(&norm_lower)
                && (low.trim_start().starts_with("//")
                    || low.trim_start().starts_with('#')
                    || low.trim_start().starts_with('*')
                    || Self::is_inside_banner_frame(lines, idx))
        })?;

        let start_idx = Self::find_banner_start(lines, hit_idx);
        let mut end_idx = lines.len();
        for i in (hit_idx + 1)..lines.len() {
            let low = lines[i].to_ascii_lowercase();
            let is_next_sub = low.contains("sub-assembly ")
                && (low.trim_start().starts_with("//")
                    || low.trim_start().starts_with('#')
                    || low.trim_start().starts_with('*')
                    || Self::is_inside_banner_frame(lines, i));
            let is_next_block = (low.contains("block 1:")
                || low.contains("block 2:")
                || low.contains("block 3:")
                || low.contains("block 4:")
                || low.contains("block 5:"))
                && Self::is_inside_banner_frame(lines, i);

            if is_next_sub || is_next_block {
                end_idx = Self::find_banner_start(lines, i);
                break;
            }
        }

        while end_idx > start_idx + 1 && lines[end_idx - 1].trim().is_empty() {
            end_idx -= 1;
        }

        Some(ExtractedSegment {
            start_line: start_idx + 1,
            end_line: end_idx,
            lines: lines[start_idx..end_idx].to_vec(),
        })
    }

    /// Extracts a top-level C or Rust function definition (with preceding Doxygen docstring).
    pub fn extract_symbol_segment(
        lines: &[String],
        symbol: &str,
        _lang: &str,
    ) -> Option<ExtractedSegment> {
        let mut brace_depth: i32 = 0;
        let mut in_block_comment = false;
        let mut fallback_proto: Option<ExtractedSegment> = None;

        let mut i = 0;
        while i < lines.len() {
            let line = &lines[i];
            let trimmed = line.trim();

            // Check top-level candidate only when outside braces and comments
            if brace_depth == 0
                && !in_block_comment
                && !trimmed.starts_with("//")
                && !trimmed.starts_with("/*")
                && !trimmed.starts_with('*')
                && !trimmed.starts_with('#')
                && Self::line_contains_symbol_call(trimmed, symbol)
            {
                // Prefer full function definition `{ ... }` over forward prototype `;`
                let lookahead_end = (i + 12).min(lines.len());
                let mut found_open_brace = None;
                let mut found_semi = None;
                for j in i..lookahead_end {
                    let lt = lines[j].trim();
                    if let Some(pos) = lt.find('{') {
                        if !lt[..pos].contains(';') {
                            found_open_brace = Some(j);
                            break;
                        }
                    }
                    if lt.contains(';') {
                        found_semi = Some(j);
                        break;
                    }
                }

                if let Some(brace_line) = found_open_brace {
                    let start_idx = Self::walk_back_docstring(lines, i);
                    let end_idx = Self::scan_balanced_braces(lines, brace_line)?;
                    return Some(ExtractedSegment {
                        start_line: start_idx + 1,
                        end_line: end_idx + 1,
                        lines: lines[start_idx..=end_idx].to_vec(),
                    });
                } else if let Some(semi_line) = found_semi {
                    if fallback_proto.is_none() {
                        let start_idx = Self::walk_back_docstring(lines, i);
                        fallback_proto = Some(ExtractedSegment {
                            start_line: start_idx + 1,
                            end_line: semi_line + 1,
                            lines: lines[start_idx..=semi_line].to_vec(),
                        });
                    }
                }
            }

            Self::advance_c_brace_state(line, &mut brace_depth, &mut in_block_comment);
            i += 1;
        }

        // Fallback: C `typedef struct/enum { ... } <symbol>;`
        for idx in 0..lines.len() {
            let lt = lines[idx].trim();
            if lt.starts_with("typedef ") && lt.contains(symbol) && lt.contains('{') {
                let start_idx = Self::walk_back_docstring(lines, idx);
                let end_idx = Self::scan_balanced_braces(lines, idx)?;
                return Some(ExtractedSegment {
                    start_line: start_idx + 1,
                    end_line: end_idx + 1,
                    lines: lines[start_idx..=end_idx].to_vec(),
                });
            }
        }

        fallback_proto
    }

    fn line_contains_symbol_call(trimmed: &str, symbol: &str) -> bool {
        if let Some(pos) = trimmed.find(symbol) {
            let before_ok = pos == 0
                || !trimmed[..pos]
                    .chars()
                    .last()
                    .map(|c| c.is_ascii_alphanumeric() || c == '_')
                    .unwrap_or(false);
            if !before_ok {
                return false;
            }
            let after = trimmed[pos + symbol.len()..].trim_start();
            return after.starts_with('(');
        }
        false
    }

    fn walk_back_docstring(lines: &[String], fn_idx: usize) -> usize {
        let mut idx = fn_idx;
        while idx > 0 {
            let prev = lines[idx - 1].trim();
            if prev.starts_with("CS_") || prev.starts_with("#[") {
                idx -= 1;
                continue;
            }
            if prev.starts_with("///") || prev.starts_with("//") {
                if prev.contains("====") || prev.contains("Sub-Assembly") || prev.contains("BLOCK ") {
                    break;
                }
                idx -= 1;
                continue;
            }
            if prev.ends_with("*/") {
                let mut comment_start = idx - 1;
                while comment_start > 0 && !lines[comment_start].trim().starts_with("/*") {
                    comment_start -= 1;
                }
                let head = lines[comment_start].trim();
                let is_section_banner = head == "/*"
                    && comment_start + 1 < lines.len()
                    && (lines[comment_start + 1].contains("====")
                        || lines[comment_start + 1].contains("Sub-Assembly")
                        || lines[comment_start + 1].contains("BLOCK "));
                if is_section_banner {
                    break;
                }
                idx = comment_start;
                continue;
            }
            break;
        }
        idx
    }

    fn scan_balanced_braces(lines: &[String], open_line: usize) -> Option<usize> {
        let mut depth: i32 = 0;
        let mut in_comment = false;
        let mut saw_open = false;

        for (i, line) in lines.iter().enumerate().skip(open_line) {
            Self::advance_c_brace_state_flag(line, &mut depth, &mut in_comment, &mut saw_open);
            if saw_open && depth <= 0 {
                return Some(i);
            }
        }
        None
    }

    fn advance_c_brace_state(line: &str, depth: &mut i32, in_comment: &mut bool) {
        let mut dummy = false;
        Self::advance_c_brace_state_flag(line, depth, in_comment, &mut dummy);
    }

    fn advance_c_brace_state_flag(
        line: &str,
        depth: &mut i32,
        in_comment: &mut bool,
        saw_open: &mut bool,
    ) {
        let chars: Vec<char> = line.chars().collect();
        let mut i = 0;
        let mut in_str = false;
        let mut str_delim = '"';
        while i < chars.len() {
            if *in_comment {
                if chars[i] == '*' && i + 1 < chars.len() && chars[i + 1] == '/' {
                    *in_comment = false;
                    i += 2;
                } else {
                    i += 1;
                }
                continue;
            }
            if in_str {
                if chars[i] == '\\' {
                    i += 2;
                    continue;
                }
                if chars[i] == str_delim {
                    in_str = false;
                }
                i += 1;
                continue;
            }
            if chars[i] == '/' && i + 1 < chars.len() {
                if chars[i + 1] == '/' {
                    break;
                }
                if chars[i + 1] == '*' {
                    *in_comment = true;
                    i += 2;
                    continue;
                }
            }
            if chars[i] == '"' || chars[i] == '\'' {
                in_str = true;
                str_delim = chars[i];
                i += 1;
                continue;
            }
            if chars[i] == '{' {
                *depth += 1;
                *saw_open = true;
            } else if chars[i] == '}' {
                *depth -= 1;
            }
            i += 1;
        }
    }

    fn is_inside_banner_frame(lines: &[String], idx: usize) -> bool {
        let start = idx.saturating_sub(3);
        let end = (idx + 3).min(lines.len().saturating_sub(1));
        for k in start..=end {
            let t = lines[k].trim();
            if t.starts_with("/*")
                || t.contains("==========")
                || t.starts_with("// ---")
                || t.starts_with("# ---")
                || t == "////"
            {
                return true;
            }
        }
        false
    }

    fn find_banner_start(lines: &[String], hit_idx: usize) -> usize {
        let mut start = hit_idx;
        let floor = hit_idx.saturating_sub(4);
        while start > floor {
            let prev = lines[start - 1].trim();
            if prev.starts_with("/*")
                || prev.starts_with("// ===")
                || prev.starts_with("// ---")
                || prev.starts_with("# ===")
                || prev.starts_with("# ---")
                || prev.starts_with("====")
                || prev.starts_with("----")
                || prev == "////"
            {
                start -= 1;
            } else {
                break;
            }
        }
        start
    }

    /// Formats a snapshot plate ready for inline `include::` inside AsciiDoc chapters.
    pub fn format_snapshot_plate(snap: &RenderedSnapshot, show_linenums: bool) -> String {
        let mut out = String::new();
        let first_line = snap.inline_segments.first().map(|s| s.start_line).unwrap_or(1);
        let last_line = snap.inline_segments.last().map(|s| s.end_line).unwrap_or(1);

        out.push_str(&format!("[[snap_plate_{}]]\n", snap.spec.id));
        out.push_str(&format!(
            ".{} (`{}#L{}-L{}` · Mode: `{:?}`)\n",
            snap.spec.title, snap.spec.source_file, first_line, last_line, snap.spec.mode
        ));

        let mut applied_callouts: Vec<&SnapshotCalloutSpec> = Vec::new();
        let comment_prefix = if snap.lang == "cmake" || snap.lang == "yaml" {
            "# "
        } else {
            "// "
        };

        for (seg_idx, seg) in snap.inline_segments.iter().enumerate() {
            if seg_idx > 0 {
                out.push_str(&format!(
                    "\n_Group slice {} (`#L{}-L{}`):_\n\n",
                    seg_idx + 1,
                    seg.start_line,
                    seg.end_line
                ));
            }
            if show_linenums {
                out.push_str(&format!(
                    "[source,{},linenums,start={}]\n------\n",
                    snap.lang, seg.start_line
                ));
            } else {
                out.push_str(&format!("[source,{}]\n------\n", snap.lang));
            }

            for line in &seg.lines {
                let mut annotated = line.clone();
                if snap.lang != "json" {
                    let lt = line.trim();
                    let is_comment = lt.starts_with("/*")
                        || lt.starts_with('*')
                        || lt.starts_with("//")
                        || lt.starts_with("# ");
                    if !is_comment {
                        for co in &snap.spec.callouts {
                            if !applied_callouts.iter().any(|c| c.number == co.number)
                                && line.contains(&co.pattern)
                            {
                                annotated.push_str(&format!(" {}<{}>", comment_prefix, co.number));
                                applied_callouts.push(co);
                                break;
                            }
                        }
                    }
                }
                out.push_str(&annotated);
                out.push('\n');
            }
            out.push_str("------\n");
        }

        if !applied_callouts.is_empty() {
            applied_callouts.sort_by_key(|c| c.number);
            out.push('\n');
            for co in &applied_callouts {
                out.push_str(&format!("<{}> {}\n", co.number, co.text));
            }
        }

        if let Some(ref note) = snap.spec.callout_note {
            out.push_str(&format!("\n_{}_\n", note));
        }

        if Self::snapshot_goes_in_appendix(snap) {
            let ws_first = snap.wholesale_segments.first().map(|s| s.start_line).unwrap_or(1);
            let ws_last = snap.wholesale_segments.last().map(|s| s.end_line).unwrap_or(1);
            out.push_str(&format!(
                "\nifdef::has-snapshot-appendix[Wholesale Listing: Complete verbatim code (`#L{}-L{}`) -> <<snap_wholesale_{},Appendix: {}>>.]\n",
                ws_first, ws_last, snap.spec.id, snap.spec.title
            ));
        }

        out
    }

    pub fn snapshot_goes_in_appendix(snap: &RenderedSnapshot) -> bool {
        match snap.spec.wholesale_in_appendix {
            WholesalePolicy::Always => true,
            WholesalePolicy::Never => false,
            WholesalePolicy::WhenExcerpted => snap.was_excerpted,
        }
    }

    /// Formats a wholesale reference entry appended into Appendix A of compiled books.
    pub fn format_wholesale_appendix_entry(snap: &RenderedSnapshot, show_linenums: bool) -> String {
        let mut out = String::new();
        let ws_first = snap.wholesale_segments.first().map(|s| s.start_line).unwrap_or(1);
        let ws_last = snap.wholesale_segments.last().map(|s| s.end_line).unwrap_or(1);

        out.push_str(&format!("[[snap_wholesale_{}]]\n", snap.spec.id));
        out.push_str(&format!("=== {}\n\n", snap.spec.title));
        out.push_str(&format!(
            "*Snapshot ID*: `{}` | *Mode*: `{:?}` | *File*: `{}` (`L{}–L{}` of `{} lines`) | <<snap_plate_{},Jump back to chapter text>>\n\n",
            snap.spec.id,
            snap.spec.mode,
            snap.spec.source_file,
            ws_first,
            ws_last,
            snap.total_source_lines,
            snap.spec.id
        ));

        for seg in &snap.wholesale_segments {
            if show_linenums {
                out.push_str(&format!(
                    "[source,{},linenums,start={}]\n------\n",
                    snap.lang, seg.start_line
                ));
            } else {
                out.push_str(&format!("[source,{}]\n------\n", snap.lang));
            }
            for line in &seg.lines {
                out.push_str(line);
                out.push('\n');
            }
            out.push_str("------\n\n");
        }
        out
    }

    fn format_snapshots_index(snaps: &[RenderedSnapshot]) -> String {
        let mut out = String::new();
        out.push_str("= Generated Textbook Code Snapshots Index\n\n");
        out.push_str("[cols=\"2,2,1,2,1\",options=\"header\"]\n|===\n");
        out.push_str("| Snapshot ID | Source File | Mode | Line Span | Appendix?\n\n");
        for s in snaps {
            let first = s.wholesale_segments.first().map(|x| x.start_line).unwrap_or(1);
            let last = s.wholesale_segments.last().map(|x| x.end_line).unwrap_or(1);
            out.push_str(&format!(
                "| `{}` | `{}` | `{:?}` | L{}–L{} | {}\n",
                s.spec.id,
                s.spec.source_file,
                s.spec.mode,
                first,
                last,
                if Self::snapshot_goes_in_appendix(s) { "Yes" } else { "Inline full" }
            ));
        }
        out.push_str("|===\n");
        out
    }

    // -------------------------------------------------------------------------
    // Compilation Entry Points
    // -------------------------------------------------------------------------

    /// Compiles both the unified Master Guide compendium and all individual volume books.
    pub fn build_all(&self) -> Result<(PathBuf, PathBuf, PathBuf), String> {
        let master = self.build_master()?;
        let _ = self.build_volumes()?;
        Ok(master)
    }

    fn prepare_build_assets(&self) -> Result<Vec<RenderedSnapshot>, String> {
        let _ = self.sync_theme_yaml()?;
        self.prepare_snapshots()
    }

    fn compile_pdf(&self, adoc_path: &Path, pdf_path: &Path, label: &str) {
        println!("  Compiling {} PDF: {:?} via asciidoctor-pdf...", label, pdf_path);
        let mut cmd = Command::new("asciidoctor-pdf");
        let theme_yaml = self
            .config
            .pdf_themes_dir
            .join(format!("{}-theme.yml", self.config.pdf_theme_name));
        if theme_yaml.is_file() {
            cmd.arg("-a")
                .arg(format!("pdf-theme={}", self.config.pdf_theme_name))
                .arg("-a")
                .arg(format!("pdf-themesdir={}", self.config.pdf_themes_dir.display()));
        }
        cmd.arg(adoc_path).arg("-o").arg(pdf_path);

        match cmd.status() {
            Ok(s) if s.success() => {
                println!("  [SUCCESS] {} PDF generated at: {:?}", label, pdf_path);
            }
            Ok(s) => {
                eprintln!("  [WARN] asciidoctor-pdf for {} exited with code: {:?}", label, s.code());
            }
            Err(e) => {
                eprintln!("  [WARN] asciidoctor-pdf for {} failed: {}", label, e);
            }
        }
    }

    /// Compiles the single unified master compendium (MASTER_GUIDE.{adoc, md, pdf}).
    pub fn build_master(&self) -> Result<(PathBuf, PathBuf, PathBuf), String> {
        println!(">>> Compiling Unified Master Guide across Volumes & Chapters...");
        let snapshots = self.prepare_build_assets()?;

        let adoc_path = self.docs_dir.join("MASTER_GUIDE.adoc");
        let md_path = self.docs_dir.join("MASTER_GUIDE.md");
        let pdf_path = self.docs_dir.join("MASTER_GUIDE.pdf");

        let (adoc_content, md_content) = self.assemble_content_with_snapshots(&snapshots)?;

        println!("  Writing {:?}...", adoc_path);
        fs::write(&adoc_path, &adoc_content)
            .map_err(|e| format!("Failed to write MASTER_GUIDE.adoc: {}", e))?;

        println!("  Writing {:?} (Optimized for NotebookLM & Gemini)...", md_path);
        fs::write(&md_path, &md_content)
            .map_err(|e| format!("Failed to write MASTER_GUIDE.md: {}", e))?;

        self.compile_pdf(&adoc_path, &pdf_path, "Unified Master Guide");
        println!(">>> Unified Master Guide Compilation Complete!");
        Ok((adoc_path, md_path, pdf_path))
    }

    /// Compiles individual volume books into docs/volumes/ (VOLUME_{roman}_{slug}.{adoc, md, pdf}).
    pub fn build_volumes(&self) -> Result<Vec<(String, PathBuf, PathBuf, PathBuf)>, String> {
        println!(">>> Compiling Individual Master Guide Volumes into docs/volumes/...");
        let snapshots = self.prepare_build_assets()?;
        fs::create_dir_all(&self.volumes_dir)
            .map_err(|e| format!("Failed to create volumes directory: {}", e))?;

        let mut results = Vec::new();
        for vol in &self.volumes {
            let (adoc_path, md_path, pdf_path) =
                self.compile_single_volume_with_snapshots(vol, &snapshots)?;
            results.push((vol.roman.clone(), adoc_path, md_path, pdf_path));
        }

        println!(">>> All {} Volumes Compiled Successfully!", self.volumes.len());
        Ok(results)
    }

    /// Compiles a single volume identified by Roman numeral (e.g. "I", "VI") or 1-based index ("1", "6").
    pub fn build_single_volume(&self, identifier: &str) -> Result<(PathBuf, PathBuf, PathBuf), String> {
        let vol = self
            .volumes
            .iter()
            .find(|v| {
                v.roman.eq_ignore_ascii_case(identifier)
                    || match identifier.parse::<usize>() {
                        Ok(idx) if idx > 0 && idx <= self.volumes.len() => self
                            .volumes
                            .get(idx - 1)
                            .map(|found| found.roman == v.roman)
                            .unwrap_or(false),
                        _ => false,
                    }
            })
            .ok_or_else(|| format!("Volume '{}' not found in manifest.", identifier))?;

        let snapshots = self.prepare_build_assets()?;
        fs::create_dir_all(&self.volumes_dir)
            .map_err(|e| format!("Failed to create volumes directory: {}", e))?;

        self.compile_single_volume_with_snapshots(vol, &snapshots)
    }

    fn compile_single_volume_with_snapshots(
        &self,
        vol: &VolumeSpec,
        snapshots: &[RenderedSnapshot],
    ) -> Result<(PathBuf, PathBuf, PathBuf), String> {
        let slug = if vol.slug.is_empty() {
            format!("vol_{}", vol.roman.to_lowercase())
        } else {
            vol.slug.clone()
        };

        let prefix = format!("VOLUME_{}_{}", vol.roman, slug);
        let adoc_path = self.volumes_dir.join(format!("{}.adoc", prefix));
        let md_path = self.volumes_dir.join(format!("{}.md", prefix));
        let pdf_path = self.volumes_dir.join(format!("{}.pdf", prefix));

        let (adoc_content, md_content) = self.assemble_volume_with_snapshots(vol, snapshots)?;

        println!("  Writing Volume {} AsciiDoc: {:?}...", vol.roman, adoc_path);
        fs::write(&adoc_path, &adoc_content)
            .map_err(|e| format!("Failed to write {:?}: {}", adoc_path, e))?;

        println!("  Writing Volume {} Markdown: {:?}...", vol.roman, md_path);
        fs::write(&md_path, &md_content)
            .map_err(|e| format!("Failed to write {:?}: {}", md_path, e))?;

        self.compile_pdf(&adoc_path, &pdf_path, &format!("Volume {}", vol.roman));
        Ok((adoc_path, md_path, pdf_path))
    }

    /// Assembles the unified AsciiDoc and Markdown text documents.
    pub fn assemble_content(&self) -> Result<(String, String), String> {
        let snapshots = self.prepare_snapshots().unwrap_or_default();
        self.assemble_content_with_snapshots(&snapshots)
    }

    /// Assembles the text documents for an individual standalone volume.
    pub fn assemble_volume_content(&self, vol: &VolumeSpec) -> Result<(String, String), String> {
        let snapshots = self.prepare_snapshots().unwrap_or_default();
        self.assemble_volume_with_snapshots(vol, &snapshots)
    }

    fn push_book_header(&self, adoc: &mut String, title: &str) {
        let p = &self.config.publication;
        adoc.push_str(&format!("= {}\n", title));
        adoc.push_str(&format!("{}\n", p.author));
        adoc.push_str(&format!("v{}, {}\n", p.revnumber, p.revdate));
        adoc.push_str(&format!(":doctype: {}\n", p.doctype));
        adoc.push_str(&format!(":toc: {}\n", p.toc));
        adoc.push_str(&format!(":toclevels: {}\n", self.config.toc_levels));
        adoc.push_str(&format!(":sectnumlevels: {}\n", self.config.sectnum_levels));
        adoc.push_str(&format!(":icons: {}\n", p.icons));
        adoc.push_str(&format!(":source-highlighter: {}\n", p.source_highlighter));
        adoc.push_str(":has-snapshot-appendix:\n\n");
    }

    fn prepare_chapter_body(
        &self,
        ch: &ChapterSpec,
        snapshots: &[RenderedSnapshot],
        referenced_snap_ids: &mut HashSet<String>,
    ) -> String {
        let raw_content = if ch.source_path.exists() {
            fs::read_to_string(&ch.source_path).unwrap_or_else(|_| {
                format!("Chapter content unavailable from {:?}", ch.source_path)
            })
        } else {
            format!("_File not found: {:?}_\n", ch.source_path)
        };

        let mut body = Self::strip_adoc_header(&raw_content);

        if self.config.publication.expand_custom_attributes {
            body = Self::resolve_and_strip_custom_attributes(&body);
        }
        if self.config.publication.expand_includes {
            let base_dir = ch.source_path.parent().unwrap_or(&self.docs_dir);
            body = Self::expand_includes(base_dir, &body, 0);
        }

        // Record any snapshots referenced by include or cross-reference
        for snap in snapshots {
            let inc_token = format!("{}.adoc", snap.spec.id);
            let plate_token = format!("snap_plate_{}", snap.spec.id);
            let ws_token = format!("snap_wholesale_{}", snap.spec.id);
            if raw_content.contains(&inc_token)
                || raw_content.contains(&ws_token)
                || body.contains(&plate_token)
                || body.contains(&ws_token)
            {
                referenced_snap_ids.insert(snap.spec.id.clone());
            }
        }

        if self.config.publication.shift_chapter_headings {
            body = Self::shift_section_levels(
                &body,
                1,
                self.config.publication.strip_manual_heading_numbers,
            );
        }
        Self::strip_standalone_macros(&body)
    }

    fn append_wholesale_appendix(
        &self,
        adoc: &mut String,
        md: &mut String,
        snapshots: &[RenderedSnapshot],
        referenced_ids: &HashSet<String>,
        vol_override: Option<bool>,
    ) {
        if !self.config.snapshots.include_wholesale_appendix || vol_override == Some(false) {
            return;
        }
        let entries: Vec<&RenderedSnapshot> = snapshots
            .iter()
            .filter(|s| {
                referenced_ids.contains(&s.spec.id)
                    && (Self::snapshot_goes_in_appendix(s) || vol_override == Some(true))
            })
            .collect();

        if entries.is_empty() {
            return;
        }

        let title = &self.config.snapshots.appendix_title;
        adoc.push_str(&format!("[appendix]\n[[appendix_code_snapshots]]\n== {}\n\n", title));
        adoc.push_str(
            "Complete verbatim reference plates for source files, blocks, sub-assemblies, \
             and function groups excerpted in preceding chapters.\n\n",
        );

        let mut app_md_adoc = String::new();
        for snap in entries {
            let plate = Self::format_wholesale_appendix_entry(
                snap,
                self.config.snapshots.show_line_numbers,
            );
            adoc.push_str(&plate);
            app_md_adoc.push_str(&plate);
        }

        md.push_str(&format!("## Appendix: {}\n\n", title));
        md.push_str(&Self::adoc_to_markdown(&app_md_adoc));
        md.push_str("\n---\n\n");
    }

    fn assemble_content_with_snapshots(
        &self,
        snapshots: &[RenderedSnapshot],
    ) -> Result<(String, String), String> {
        let mut adoc = String::with_capacity(400_000);
        let mut md = String::with_capacity(400_000);

        self.push_book_header(&mut adoc, &self.config.publication.master_title);
        adoc.push_str(":sectnums!:\n\n");
        adoc.push_str("[preface]\n== Executive Summary\n\n");
        adoc.push_str(
            "This Master Guide serves as the definitive single source of truth for *Tenebris and Lux*.\n\
             It unites world canon, character dossiers, ludology, the 2017–2018 demo baseline,\n\
             software architecture, and engineering dev logs into a single textbook compendium.\n\n",
        );
        adoc.push_str("[cols=\"1,3,3\",options=\"header\"]\n|===\n| Volume | Title | Scope\n");
        for vol in &self.volumes {
            adoc.push_str(&format!(
                "| Volume {} | {} ({} ch.) | {}\n",
                vol.roman,
                vol.title,
                vol.chapters.len(),
                vol.description
            ));
        }
        adoc.push_str("|===\n\n");
        adoc.push_str(":sectnums:\n\n");

        md.push_str(&format!("# {}\n\n", self.config.publication.master_title));
        md.push_str(&format!("> **Author**: {}  \n", self.config.publication.author));
        md.push_str(&format!(
            "> **Version**: {} ({})  \n",
            self.config.publication.revnumber, self.config.publication.revdate
        ));
        md.push_str("> **Standard**: OmniCode 5-Block Ontological Architecture & Layered Invariants  \n");
        md.push_str("> **Purpose**: Unified Single-File Master Compendium for Google Gemini and NotebookLM\n\n");
        md.push_str("## Executive Summary\n\n");
        md.push_str(
            "This Master Guide compiles the entire architectural, ludological, and narrative universe of **Tenebris and Lux**.\n\n",
        );

        md.push_str("## Table of Contents\n\n");
        for vol in &self.volumes {
            md.push_str(&format!("- **Volume {}: {}**\n", vol.roman, vol.title));
            for ch in &vol.chapters {
                md.push_str(&format!("  - Chapter {}: {}\n", ch.number, ch.title));
            }
        }
        md.push_str("\n---\n\n");

        let mut referenced_ids = HashSet::new();

        for vol in &self.volumes {
            adoc.push_str(&format!("= Volume {}: {}\n\n", vol.roman, vol.title));
            adoc.push_str(&format!("{}\n\n", vol.description));

            md.push_str(&format!("# Volume {}: {}\n\n", vol.roman, vol.title));
            md.push_str(&format!("_{}_\n\n", vol.description));

            for ch in &vol.chapters {
                let processed = self.prepare_chapter_body(ch, snapshots, &mut referenced_ids);
                let rel_src = ch
                    .source_path
                    .strip_prefix(&self.repo_root)
                    .unwrap_or(&ch.source_path);

                adoc.push_str(&format!("== {}\n\n", ch.title));
                adoc.push_str(&format!("*Source File*: `{}`\n\n", rel_src.display()));
                adoc.push_str(&processed);
                adoc.push_str("\n\n");

                md.push_str(&format!("## Chapter {}: {}\n\n", ch.number, ch.title));
                md.push_str(&format!("> **Source**: `{}`\n\n", rel_src.display()));
                md.push_str(&Self::adoc_to_markdown(&processed));
                md.push_str("\n\n---\n\n");
            }
        }

        self.append_wholesale_appendix(&mut adoc, &mut md, snapshots, &referenced_ids, None);
        Ok((Self::deduplicate_snapshot_plate_anchors(&adoc), md))
    }

    fn assemble_volume_with_snapshots(
        &self,
        vol: &VolumeSpec,
        snapshots: &[RenderedSnapshot],
    ) -> Result<(String, String), String> {
        let mut adoc = String::with_capacity(100_000);
        let mut md = String::with_capacity(100_000);

        self.push_book_header(&mut adoc, &format!("Volume {}: {}", vol.roman, vol.title));
        adoc.push_str(":sectnums:\n\n");
        adoc.push_str(&format!("[NOTE]\n.Volume {} Synopsis\n====\n{}\n====\n\n", vol.roman, vol.description));

        md.push_str(&format!("# Volume {}: {}\n\n", vol.roman, vol.title));
        md.push_str(&format!("> **Author**: {}  \n", self.config.publication.author));
        md.push_str(&format!(
            "> **Compendium**: Tenebris and Lux Master Guide — Volume {} ({})  \n",
            vol.roman, self.config.publication.revnumber
        ));
        md.push_str("> **Purpose**: Context Document for Google Gemini & NotebookLM Workflows\n\n");
        md.push_str("## Volume Synopsis\n\n");
        md.push_str(&format!("_{}_\n\n", vol.description));

        md.push_str("## Table of Contents\n\n");
        for (idx, ch) in vol.chapters.iter().enumerate() {
            md.push_str(&format!("{}. {}\n", idx + 1, ch.title));
        }
        md.push_str("\n---\n\n");

        let mut referenced_ids = HashSet::new();

        for (idx, ch) in vol.chapters.iter().enumerate() {
            let processed = self.prepare_chapter_body(ch, snapshots, &mut referenced_ids);
            let rel_src = ch
                .source_path
                .strip_prefix(&self.repo_root)
                .unwrap_or(&ch.source_path);

            adoc.push_str(&format!("== {}\n\n", ch.title));
            adoc.push_str(&format!("*Source File*: `{}`\n\n", rel_src.display()));
            adoc.push_str(&processed);
            adoc.push_str("\n\n");

            md.push_str(&format!("## Chapter {}: {}\n\n", idx + 1, ch.title));
            md.push_str(&format!("> **Source**: `{}`\n\n", rel_src.display()));
            md.push_str(&Self::adoc_to_markdown(&processed));
            md.push_str("\n\n---\n\n");
        }

        let vol_override = self.config.volume_appendix_overrides.get(&vol.roman).copied();
        self.append_wholesale_appendix(&mut adoc, &mut md, snapshots, &referenced_ids, vol_override);
        Ok((Self::deduplicate_snapshot_plate_anchors(&adoc), md))
    }

    /// Renames repeated `[[snap_plate_*]]` anchors within multi-chapter books so every block ID stays unique.
    fn deduplicate_snapshot_plate_anchors(adoc: &str) -> String {
        let mut out = String::with_capacity(adoc.len());
        let mut counts: HashMap<String, usize> = HashMap::new();
        let mut active_fence: Option<&'static str> = None;

        for line in adoc.lines() {
            let t = line.trim();
            if let Some(fence) = active_fence {
                if t == fence {
                    active_fence = None;
                }
                out.push_str(line);
                out.push('\n');
                continue;
            }
            if matches!(t, "----" | "------" | "...." | "////") {
                active_fence = Some(match t {
                    "------" => "------",
                    "----" => "----",
                    "...." => "....",
                    _ => "////",
                });
                out.push_str(line);
                out.push('\n');
                continue;
            }

            if t.starts_with("[[snap_plate_") && t.ends_with("]]") {
                let id = &t[2..t.len() - 2];
                let entry = counts.entry(id.to_string()).or_insert(0);
                *entry += 1;
                if *entry == 1 {
                    out.push_str(line);
                } else {
                    out.push_str(&format!("[[{}_ref{}]]", id, *entry));
                }
            } else {
                out.push_str(line);
            }
            out.push('\n');
        }
        out
    }

    // -------------------------------------------------------------------------
    // Chapter Pre-Processing Helpers
    // -------------------------------------------------------------------------

    /// Strips leading 5-block `////` metadata block and standalone AsciiDoc document header.
    pub fn strip_adoc_header(content: &str) -> String {
        let all_lines: Vec<&str> = content.lines().collect();
        let mut idx = 0;

        // Pass 1: Skip leading whitespace and leading `//// ... ////` comment blocks
        while idx < all_lines.len() {
            let t = all_lines[idx].trim();
            if t.is_empty() || (t.starts_with("//") && t != "////") {
                idx += 1;
            } else if t == "////" {
                idx += 1;
                while idx < all_lines.len() && all_lines[idx].trim() != "////" {
                    idx += 1;
                }
                if idx < all_lines.len() {
                    idx += 1;
                }
            } else {
                break;
            }
        }

        // Pass 2: Consume standalone `= Document Title`, author/revision, and `:attr:` lines
        if idx < all_lines.len() {
            let first = all_lines[idx].trim();
            if first.starts_with("= ") && !first.starts_with("== ") {
                idx += 1;
                let mut seen_attr = false;
                let mut meta_lines = 0;
                while idx < all_lines.len() {
                    let t = all_lines[idx].trim();
                    if t.is_empty() {
                        idx += 1;
                        break;
                    }
                    if t.starts_with(':') {
                        seen_attr = true;
                        idx += 1;
                    } else if !seen_attr && meta_lines < 2 && !t.starts_with("==") && t != "////" {
                        // Author or version/date revision line
                        meta_lines += 1;
                        idx += 1;
                    } else {
                        break;
                    }
                }
                // Consume trailing header attribute lines separated by comments/blank lines
                while idx < all_lines.len() {
                    let t = all_lines[idx].trim();
                    if t.is_empty() || t.starts_with(':') {
                        idx += 1;
                    } else {
                        break;
                    }
                }
            }
        }

        let remaining = &all_lines[idx..];
        if remaining.is_empty() {
            content.to_string()
        } else {
            remaining.join("\n")
        }
    }

    /// Recursively inlines `include::path[]` relative to `base_dir` (capped depth = 6).
    pub fn expand_includes(base_dir: &Path, content: &str, depth: usize) -> String {
        if depth >= 6 {
            return content.to_string();
        }
        let mut out = String::with_capacity(content.len());
        let mut active_fence: Option<&'static str> = None;

        for line in content.lines() {
            let t = line.trim();
            if let Some(fence) = active_fence {
                if t == fence {
                    active_fence = None;
                }
                out.push_str(line);
                out.push('\n');
                continue;
            }
            if t == "----" || t == "------" || t == "...." || t == "////" {
                active_fence = Some(if t == "------" {
                    "------"
                } else if t == "----" {
                    "----"
                } else if t == "...." {
                    "...."
                } else {
                    "////"
                });
                out.push_str(line);
                out.push('\n');
                continue;
            }

            if let Some(rest) = t.strip_prefix("include::") {
                if let Some(bracket_idx) = rest.find('[') {
                    let rel = rest[..bracket_idx].trim();
                    let target = base_dir.join(rel);
                    if target.is_file() {
                        if let Ok(inc_raw) = fs::read_to_string(&target) {
                            let child_dir = target.parent().unwrap_or(base_dir);
                            let expanded = Self::expand_includes(child_dir, &inc_raw, depth + 1);
                            out.push_str(&expanded);
                            if !expanded.ends_with('\n') {
                                out.push('\n');
                            }
                            continue;
                        }
                    }
                }
            }
            out.push_str(line);
            out.push('\n');
        }
        out
    }

    /// Resolves chapter-scoped custom `:key: val` definitions outside code fences.
    pub fn resolve_and_strip_custom_attributes(content: &str) -> String {
        let builtins: HashSet<&'static str> = [
            "toc",
            "toclevels",
            "sectnums",
            "sectnumlevels",
            "icons",
            "source-highlighter",
            "doctype",
            "pdf-theme",
            "pdf-themesdir",
            "has-snapshot-appendix",
            "chapter-signifier",
            "appendix-caption",
        ]
        .into_iter()
        .collect();

        let mut attrs: HashMap<String, String> = HashMap::new();
        let mut kept_lines = Vec::new();
        let mut in_fence: Option<&'static str> = None;

        for line in content.lines() {
            let t = line.trim();
            if let Some(f) = in_fence {
                if t == f {
                    in_fence = None;
                }
                kept_lines.push(line.to_string());
                continue;
            }
            if matches!(t, "----" | "------" | "...." | "////") {
                in_fence = Some(match t {
                    "------" => "------",
                    "----" => "----",
                    "...." => "....",
                    _ => "////",
                });
                kept_lines.push(line.to_string());
                continue;
            }

            if t.starts_with(':') {
                if let Some(end_col) = t[1..].find(':') {
                    let key = &t[1..1 + end_col];
                    let val = t[2 + end_col..].trim();
                    let valid_key = !key.is_empty()
                        && !key.starts_with('!')
                        && !key.ends_with('!')
                        && key.chars().all(|c| c.is_ascii_alphanumeric() || c == '-' || c == '_');
                    if valid_key && !builtins.contains(key) && !val.is_empty() {
                        attrs.insert(key.to_string(), val.to_string());
                        continue;
                    }
                }
            }
            kept_lines.push(line.to_string());
        }

        let joined = kept_lines.join("\n");
        if attrs.is_empty() {
            return joined;
        }
        let mut res = joined;
        for (k, v) in &attrs {
            res = res.replace(&format!("{{{}}}", k), v);
        }
        res
    }

    fn strip_standalone_macros(content: &str) -> String {
        let mut out = String::with_capacity(content.len());
        let mut in_fence: Option<&'static str> = None;

        for line in content.lines() {
            let t = line.trim();
            if let Some(f) = in_fence {
                if (f == "```" && t.starts_with("```")) || t == f {
                    in_fence = None;
                }
                out.push_str(line);
                out.push('\n');
                continue;
            }
            if t.starts_with("```") {
                in_fence = Some("```");
                out.push_str(line);
                out.push('\n');
                continue;
            }
            if matches!(t, "----" | "------" | "...." | "////") {
                in_fence = Some(match t {
                    "------" => "------",
                    "----" => "----",
                    "...." => "....",
                    _ => "////",
                });
                out.push_str(line);
                out.push('\n');
                continue;
            }

            if t == "toc::[]" || t == "[abstract]" {
                continue;
            }
            out.push_str(line);
            out.push('\n');
        }
        out
    }

    /// Shifts AsciiDoc headings by `+shift` ONLY outside delimited blocks (`----`, `====`, `////`, `****`, `|===`, ```` ``` ````).
    pub fn shift_section_levels(content: &str, shift: usize, strip_manual_nums: bool) -> String {
        let mut out = String::with_capacity(content.len() + 256);
        let mut fence_stack: Vec<&'static str> = Vec::new();

        for line in content.lines() {
            let t = line.trim();
            let in_verbatim = fence_stack
                .iter()
                .any(|f| *f == "----" || *f == "------" || *f == "...." || *f == "////" || *f == "```");

            if let Some(&top) = fence_stack.last() {
                if (top == "|===" && t == "|===")
                    || (top == "```" && t.starts_with("```"))
                    || (top != "|===" && top != "```" && t == top)
                {
                    fence_stack.pop();
                    out.push_str(line);
                    out.push('\n');
                    continue;
                }
            }

            if !in_verbatim {
                let maybe_fence = if t.starts_with("```") {
                    Some("```")
                } else {
                    match t {
                        "------" => Some("------"),
                        "----" => Some("----"),
                        "...." => Some("...."),
                        "////" => Some("////"),
                        "====" => Some("===="),
                        "****" => Some("****"),
                        "|===" => Some("|==="),
                        "--" => Some("--"),
                        _ => None,
                    }
                };
                if let Some(f) = maybe_fence {
                    fence_stack.push(f);
                    out.push_str(line);
                    out.push('\n');
                    continue;
                }
            }

            // Only shift true AsciiDoc section headings outside verbatim/table fences
            let allow_heading = !in_verbatim && !fence_stack.contains(&"|===");
            if allow_heading {
                let eq_count = line.chars().take_while(|&c| c == '=').count();
                if (2..=5).contains(&eq_count) && line.as_bytes().get(eq_count) == Some(&b' ') {
                    let new_level = (eq_count + shift).min(6);
                    let rest = line[eq_count + 1..].trim();
                    let cleaned = if strip_manual_nums {
                        Self::strip_manual_section_prefix(rest)
                    } else {
                        rest
                    };
                    for _ in 0..new_level {
                        out.push('=');
                    }
                    out.push(' ');
                    out.push_str(cleaned);
                    out.push('\n');
                    continue;
                }
            }

            out.push_str(line);
            out.push('\n');
        }
        out
    }

    fn strip_manual_section_prefix(heading: &str) -> &str {
        let bytes = heading.as_bytes();
        let mut i = 0;
        let mut saw_dot = false;
        let mut digits_before_dot = 0;

        while i < bytes.len() && bytes[i].is_ascii_digit() {
            digits_before_dot += 1;
            i += 1;
        }
        if digits_before_dot == 0 || digits_before_dot > 2 {
            return heading;
        }
        while i < bytes.len() {
            if bytes[i] == b'.' {
                saw_dot = true;
                i += 1;
                while i < bytes.len() && bytes[i].is_ascii_digit() {
                    i += 1;
                }
            } else {
                break;
            }
        }
        if !saw_dot {
            return heading;
        }
        while i < bytes.len() && bytes[i] == b' ' {
            i += 1;
        }
        if i < bytes.len() && bytes[i].is_ascii_alphabetic() {
            &heading[i..]
        } else {
            heading
        }
    }

    // -------------------------------------------------------------------------
    // Full-AsciiDoc Fence-Aware Markdown Translator
    // -------------------------------------------------------------------------

    /// Converts full AsciiDoc (fences, admonitions, sidebars, comments, tables, callouts) to GFM.
    pub fn adoc_to_markdown(adoc: &str) -> String {
        let lines: Vec<&str> = adoc.lines().collect();
        let mut md = String::with_capacity(adoc.len());

        let mut in_comment_block = false;
        let mut in_code_block = false;
        let mut code_delim = "";
        let mut in_admonition: Option<String> = None;
        let mut in_sidebar = false;
        let mut in_table = false;
        let mut table_cols_expected: Option<usize> = None;
        let mut table_cells: Vec<String> = Vec::new();
        let mut inferred_cols_from_first_group: Option<usize> = None;

        let mut pending_attr: Option<String> = None;
        let mut pending_caption: Option<String> = None;

        let mut i = 0;
        while i < lines.len() {
            let line = lines[i];
            let trimmed = line.trim();

            // 1. Inside 5-block AsciiDoc //// comment block -> omit completely
            if in_comment_block {
                if trimmed == "////" {
                    in_comment_block = false;
                }
                i += 1;
                continue;
            }

            // 2. Inside code / literal block (`----`, `....`, or legacy ```` ``` ````) -> pass verbatim
            if in_code_block {
                if (code_delim == "```" && trimmed.starts_with("```")) || trimmed == code_delim {
                    in_code_block = false;
                    code_delim = "";
                    md.push_str(if in_admonition.is_some() || in_sidebar {
                        "> ```\n"
                    } else {
                        "```\n"
                    });
                } else {
                    if in_admonition.is_some() || in_sidebar {
                        md.push_str("> ");
                    }
                    md.push_str(line);
                    md.push('\n');
                }
                i += 1;
                continue;
            }

            // Legacy markdown ```lang fence embedded inside .adoc
            if trimmed.starts_with("```") {
                in_code_block = true;
                code_delim = "```";
                pending_attr = None;
                pending_caption = None;
                if in_admonition.is_some() || in_sidebar {
                    md.push_str(&format!("> {}\n", trimmed));
                } else {
                    md.push_str(&format!("{}\n", trimmed));
                }
                i += 1;
                continue;
            }

            // 3. Opening //// comment block
            if trimmed == "////" {
                in_comment_block = true;
                pending_attr = None;
                pending_caption = None;
                i += 1;
                continue;
            }

            // 4. Single-line AsciiDoc comment (`// ...`) outside code blocks -> omit
            if trimmed == "//" || trimmed.starts_with("// ") || trimmed.starts_with("///") {
                i += 1;
                continue;
            }

            // 5. Code / literal fences (`----`, `------`, or `....`)
            if trimmed == "----" || trimmed == "------" || trimmed == "...." {
                if let Some(cap) = pending_caption.take() {
                    md.push_str(&format!("**{}**\n\n", cap));
                }
                let lang = pending_attr
                    .take()
                    .and_then(|attr| Self::extract_source_lang(&attr))
                    .unwrap_or_default();
                in_code_block = true;
                code_delim = if trimmed == "------" {
                    "------"
                } else if trimmed == "----" {
                    "----"
                } else {
                    "...."
                };
                if in_admonition.is_some() || in_sidebar {
                    md.push_str(&format!("> ```{}\n", lang));
                } else {
                    md.push_str(&format!("```{}\n", lang));
                }
                i += 1;
                continue;
            }

            // 6. Example / Admonition delimiter (`====`)
            if trimmed == "====" {
                if in_admonition.is_some() {
                    in_admonition = None;
                    md.push('\n');
                } else {
                    let kind = pending_attr
                        .take()
                        .and_then(|a| Self::extract_admonition_kind(&a))
                        .unwrap_or_else(|| "NOTE".to_string());
                    let title_suffix = pending_caption
                        .take()
                        .map(|c| format!(": {}", c))
                        .unwrap_or_default();
                    md.push_str(&format!("> **[{}]**{}\n> \n", kind, title_suffix));
                    in_admonition = Some(kind);
                }
                i += 1;
                continue;
            }

            // 7. Sidebar delimiter (`****`)
            if trimmed == "****" {
                if in_sidebar {
                    in_sidebar = false;
                    md.push('\n');
                } else {
                    in_sidebar = true;
                    let title = pending_caption
                        .take()
                        .unwrap_or_else(|| "Sidebar".to_string());
                    pending_attr = None;
                    md.push_str(&format!("> **{}**\n> \n", title));
                }
                i += 1;
                continue;
            }

            // 8. Table delimiter (`|===`)
            if trimmed == "|===" {
                if in_table {
                    let cols = table_cols_expected
                        .or(inferred_cols_from_first_group)
                        .unwrap_or(1)
                        .max(1);
                    Self::flush_markdown_table(&mut md, &table_cells, cols);
                    md.push('\n');
                    in_table = false;
                    table_cols_expected = None;
                    inferred_cols_from_first_group = None;
                    table_cells.clear();
                } else {
                    if let Some(cap) = pending_caption.take() {
                        md.push_str(&format!("**{}**\n\n", cap));
                    }
                    let attr = pending_attr.take().unwrap_or_default();
                    table_cols_expected = Self::extract_table_col_count(&attr);
                    inferred_cols_from_first_group = None;
                    table_cells.clear();
                    in_table = true;
                }
                i += 1;
                continue;
            }

            // 9. Table body accumulation (supports single-line `| a | b` AND vertical `a|` cells)
            if in_table {
                if trimmed.is_empty() {
                    if inferred_cols_from_first_group.is_none() && !table_cells.is_empty() {
                        inferred_cols_from_first_group = Some(table_cells.len());
                    }
                    i += 1;
                    continue;
                }
                if trimmed == "a|" || trimmed.starts_with("a| ") {
                    let text = trimmed[2..].trim().to_string();
                    table_cells.push(text);
                } else if let Some(rest) = trimmed.strip_prefix('|') {
                    // Check if single-line multi-column row contains inline ` a| ` markers
                    let normalized = rest.replace(" a|", "|").replace("|a|", "|");
                    let parts: Vec<&str> = normalized.split('|').collect();
                    let push_count = if parts.len() > 1 && normalized.trim_end().ends_with('|') {
                        parts.len() - 1
                    } else {
                        parts.len()
                    };
                    for cell in parts.into_iter().take(push_count) {
                        table_cells.push(cell.trim().to_string());
                    }
                    if push_count > 1 && inferred_cols_from_first_group.is_none() {
                        inferred_cols_from_first_group = Some(push_count);
                    }
                } else if let Some(last) = table_cells.last_mut() {
                    let piece = if let Some(bullet) = trimmed.strip_prefix("* ") {
                        format!("• {}", bullet.trim())
                    } else {
                        trimmed.to_string()
                    };
                    if last.is_empty() {
                        *last = piece;
                    } else {
                        last.push_str("<br>");
                        last.push_str(&piece);
                    }
                }
                i += 1;
                continue;
            }

            // 10. Open block / attribute lines / anchors / macros
            if trimmed == "--" || trimmed == "toc::[]" || trimmed.starts_with("[[") && trimmed.ends_with("]]") {
                i += 1;
                continue;
            }
            if trimmed.starts_with(':') && !trimmed.starts_with("::") {
                i += 1;
                continue;
            }
            if trimmed.starts_with('[') && trimmed.ends_with(']') && !trimmed.contains("](") {
                pending_attr = Some(trimmed.to_string());
                i += 1;
                continue;
            }
            if trimmed.starts_with('.') && trimmed.len() > 1 && !trimmed.starts_with("..") {
                let next_is_block = lines
                    .get(i + 1)
                    .map(|nl| {
                        let nt = nl.trim();
                        nt.starts_with('[')
                            || matches!(nt, "----" | "...." | "====" | "****" | "|===")
                    })
                    .unwrap_or(false);
                if next_is_block {
                    pending_caption = Some(trimmed[1..].trim().to_string());
                    i += 1;
                    continue;
                }
            }

            // Unwrap ifdef::has-snapshot-appendix[...]
            let processed_line = Self::unwrap_snapshot_ifdef(line);
            let pt = processed_line.trim();

            if in_admonition.is_some() || in_sidebar {
                if pt.is_empty() {
                    md.push_str(">\n");
                } else {
                    md.push_str(&format!("> {}\n", Self::convert_inline_xrefs(&processed_line)));
                }
                i += 1;
                continue;
            }

            // 11. Paragraph-level AsciiDoc admonitions (`NOTE: ...`) & Q&A/definition terms (`Term::`)
            if let Some((tag, rest)) = Self::split_inline_admonition(pt) {
                md.push_str(&format!("> **{}:** {}\n", tag, Self::convert_inline_xrefs(rest)));
                i += 1;
                continue;
            }
            if pt.ends_with("::") && pt.len() > 2 && !pt.starts_with(':') {
                let term = pt[..pt.len() - 2].trim();
                md.push_str(&format!("**{}**  \n", Self::convert_inline_xrefs(term)));
                i += 1;
                continue;
            }

            // 12. Headings (longest prefix first)
            pending_attr = None;
            pending_caption = None;
            let converted = if let Some(h) = pt.strip_prefix("====== ") {
                format!("###### {}", h.trim())
            } else if let Some(h) = pt.strip_prefix("===== ") {
                format!("##### {}", h.trim())
            } else if let Some(h) = pt.strip_prefix("==== ") {
                format!("#### {}", h.trim())
            } else if let Some(h) = pt.strip_prefix("=== ") {
                format!("### {}", h.trim())
            } else if let Some(h) = pt.strip_prefix("== ") {
                format!("## {}", h.trim())
            } else if let Some(h) = pt.strip_prefix("= ") {
                format!("# {}", h.trim())
            } else {
                Self::convert_inline_xrefs(&processed_line)
            };

            md.push_str(&converted);
            md.push('\n');
            i += 1;
        }

        md
    }

    fn extract_source_lang(attr: &str) -> Option<String> {
        let inner = attr.trim_start_matches('[').trim_end_matches(']');
        let parts: Vec<&str> = inner.split(',').collect();
        if parts.first().map(|s| s.trim()) == Some("source") {
            let lang = parts.get(1).map(|s| s.trim()).unwrap_or("");
            if !lang.is_empty() && !lang.starts_with('%') && !lang.contains('=') {
                return Some(lang.to_string());
            }
        }
        None
    }

    fn extract_admonition_kind(attr: &str) -> Option<String> {
        let inner = attr.trim_start_matches('[').trim_end_matches(']').trim();
        let upper = inner.to_ascii_uppercase();
        for kind in ["NOTE", "TIP", "IMPORTANT", "WARNING", "CAUTION"] {
            if upper == kind || upper.starts_with(&format!("{},", kind)) {
                return Some(kind.to_string());
            }
        }
        None
    }

    fn flush_markdown_table(md: &mut String, cells: &[String], cols: usize) {
        if cells.is_empty() {
            return;
        }
        for (row_idx, chunk) in cells.chunks(cols).enumerate() {
            let mut row: Vec<String> = chunk.to_vec();
            while row.len() < cols {
                row.push(String::new());
            }
            md.push_str(&format!("| {} |\n", row.join(" | ")));
            if row_idx == 0 {
                let sep: Vec<&str> = std::iter::repeat("---").take(cols).collect();
                md.push_str(&format!("| {} |\n", sep.join(" | ")));
            }
        }
    }

    fn extract_table_col_count(attr: &str) -> Option<usize> {
        let pos = attr.find("cols=")?;
        let rest = &attr[pos + 5..];
        let quoted = rest.trim_start_matches('"');
        let spec = quoted.split('"').next()?;
        if let Some(star) = spec.strip_suffix('*') {
            if let Ok(n) = star.trim().parse::<usize>() {
                if n > 0 {
                    return Some(n);
                }
            }
        }
        let count = spec.split(',').filter(|s| !s.trim().is_empty()).count();
        if count > 0 { Some(count) } else { None }
    }

    fn split_inline_admonition(line: &str) -> Option<(&'static str, &str)> {
        for tag in ["NOTE", "TIP", "IMPORTANT", "WARNING", "CAUTION"] {
            let prefix = format!("{}: ", tag);
            if let Some(rest) = line.strip_prefix(&prefix) {
                return Some((tag, rest));
            }
        }
        None
    }

    fn unwrap_snapshot_ifdef(line: &str) -> String {
        let t = line.trim();
        if let Some(rest) = t.strip_prefix("ifdef::has-snapshot-appendix[") {
            if let Some(inner) = rest.strip_suffix(']') {
                return inner.to_string();
            }
        }
        line.to_string()
    }

    fn convert_inline_xrefs(line: &str) -> String {
        let mut out = String::with_capacity(line.len());
        let mut rest = line;
        while let Some(start) = rest.find("<<") {
            out.push_str(&rest[..start]);
            let after_open = &rest[start + 2..];
            if let Some(end) = after_open.find(">>") {
                let inner = &after_open[..end];
                if let Some((anchor, label)) = inner.split_once(',') {
                    out.push_str(&format!("[{}](#{})", label.trim(), anchor.trim()));
                } else {
                    out.push_str(&format!("[{}](#{})", inner.trim(), inner.trim()));
                }
                rest = &after_open[end + 2..];
            } else {
                out.push_str("<<");
                rest = after_open;
            }
        }
        out.push_str(rest);
        out
    }

    // -------------------------------------------------------------------------
    // Canonical Fallback Definitions (Zero-Disk / Bootstrap Safety)
    // -------------------------------------------------------------------------

    fn save_default_manifest(
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
                compilation_targets: vec!["unified_compendium".to_string(), "volume_books".to_string()],
            }),
        };

        let json = serde_json::to_string_pretty(&manifest)
            .map_err(|e| format!("Serialization error: {}", e))?;
        fs::write(manifest_path, json)
            .map_err(|e| format!("Failed to save manifest to {:?}: {}", manifest_path, e))?;
        Ok(())
    }

    fn build_canonical_volumes(docs: &Path) -> Vec<VolumeSpec> {
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
                description: "Cosmic neutrality of Lux and Tenebris, chivalric Kingdom of Sacyra, continental realms, and the existential menace of The Dark Lord.".to_string(),
                scan_areas: vec!["lore/realms".to_string()],
                chapters: vec![
                    make_ch("The Primordial Neutrality of Lux and Tenebris", "lore/world_and_lore.adoc"),
                ],
            },
            VolumeSpec {
                roman: "V".to_string(),
                slug: "software_architecture_and_5_block_protocol".to_string(),
                title: "Software Architecture & The 5-Block Invariant Protocol".to_string(),
                description: "System boundaries, layered strata, dual-frontend GUI/CLI, and pre-flight zero-heap diagnostic manifests.".to_string(),
                scan_areas: vec!["architecture".to_string(), "code".to_string()],
                chapters: vec![
                    make_ch("Cornerstone & Tenebris and Lux: System Architecture Overview", "architecture/ARCHITECTURE.adoc"),
                    make_ch("Zero-Disk Safe-Mode BIOS, Config ROM Vaults & Autonomous Probe Loop", "architecture/resilience_bios_and_probes.adoc"),
                    make_ch("Data-Driven Architecture: Dual Data Taxonomy, Layered VFS & 5-Block JSON", "architecture/data_driven_architecture.adoc"),
                    make_ch("Central Manifest Registry & Pre-Flight Loose-Bolts Inspection", "architecture/preflight_and_manifest_spec.adoc"),
                    make_ch("Dual-Frontend Presentation Layer: CLI, Native X11 & Movie Reel", "architecture/dual_frontend_architecture.adoc"),
                ],
            },
        ]
    }

    // -------------------------------------------------------------------------
    // PDF Page-Budget & 5-Block AsciiDoc Layout Validator
    // -------------------------------------------------------------------------

    /// Loads PDF single-page layout geometry and formatting rules from docs/adoc_pdf_layout_rules.json.
    pub fn load_layout_rules(&self) -> Result<AdocLayoutRulesFile, String> {
        let rules_path = self.docs_dir.join("adoc_pdf_layout_rules.json");
        if !rules_path.is_file() {
            return Ok(AdocLayoutRulesFile {
                block_2_opening: AdocLayoutOpening {
                    strict_target_prefixes: vec![
                        "docs/README.adoc".to_string(),
                        "docs/architecture/".to_string(),
                        "docs/code/README.adoc".to_string(),
                        "docs/code/logging_system.adoc".to_string(),
                        "docs/code/debug_system.adoc".to_string(),
                        "docs/code/io_pipeline.adoc".to_string(),
                        "docs/code/log/".to_string(),
                        "docs/code/debug/".to_string(),
                        "docs/code/io/".to_string(),
                    ],
                },
                block_3_body: AdocLayoutBody {
                    page_geometry: AdocPageGeometry::default(),
                    validation_rules: AdocValidationRules {
                        required_block_tokens: vec![
                            "BLOCK 1:".to_string(),
                            "BLOCK 2:".to_string(),
                            "BLOCK 3:".to_string(),
                            "BLOCK 4:".to_string(),
                            "BLOCK 5:".to_string(),
                        ],
                        ..AdocValidationRules::default()
                    },
                },
            });
        }

        let raw = fs::read_to_string(&rules_path)
            .map_err(|e| format!("Failed to read layout rules {:?}: {}", rules_path, e))?;
        serde_json::from_str::<AdocLayoutRulesFile>(&raw)
            .map_err(|e| format!("Failed to parse layout rules {:?}: {}", rules_path, e))
    }

    fn is_strict_layout_target(rel_repo_path: &str, rules: &AdocLayoutRulesFile) -> bool {
        let normalized = rel_repo_path.replace('\\', "/");
        rules.block_2_opening.strict_target_prefixes.iter().any(|pref| {
            let p = pref.trim_start_matches("./");
            normalized == p || (p.ends_with('/') && normalized.starts_with(p))
        })
    }

    /// Validates all volume chapters, strict 5-block docs, and generated snapshot plates against PDF layout rules.
    pub fn validate_adoc_layout(&self) -> Result<AdocLayoutReport, String> {
        let rules = self.load_layout_rules()?;
        let mut report = AdocLayoutReport::default();
        let mut checked_paths: HashSet<PathBuf> = HashSet::new();

        let mut candidate_paths: Vec<PathBuf> = Vec::new();
        for rel in [
            "docs/README.adoc",
            "docs/code/README.adoc",
            "docs/architecture/README.adoc",
        ] {
            let p = self.repo_root.join(rel);
            if p.is_file() {
                candidate_paths.push(p);
            }
        }
        for vol in &self.volumes {
            for ch in &vol.chapters {
                if ch.source_path.is_file() {
                    candidate_paths.push(ch.source_path.clone());
                }
            }
        }
        for area in ["architecture", "code"] {
            let area_dir = self.docs_dir.join(area);
            if area_dir.is_dir() {
                let mut stack = vec![area_dir];
                while let Some(dir) = stack.pop() {
                    if let Ok(entries) = fs::read_dir(&dir) {
                        for entry in entries.flatten() {
                            let ep = entry.path();
                            if ep.is_dir() {
                                stack.push(ep);
                            } else if ep.extension().and_then(|s| s.to_str()) == Some("adoc") {
                                candidate_paths.push(ep);
                            }
                        }
                    }
                }
            }
        }

        candidate_paths.sort();
        for path in candidate_paths {
            if !checked_paths.insert(path.clone()) {
                continue;
            }
            if path.extension().and_then(|s| s.to_str()) != Some("adoc") {
                continue;
            }
            let raw = match fs::read_to_string(&path) {
                Ok(s) => s,
                Err(_) => continue,
            };
            let rel = path
                .strip_prefix(&self.repo_root)
                .unwrap_or(&path)
                .to_string_lossy()
                .to_string();
            let strict = Self::is_strict_layout_target(&rel, &rules);
            let mut file_issues = Self::validate_adoc_text(
                &rel,
                path.parent().unwrap_or(&self.docs_dir),
                &raw,
                strict,
                false,
                &rules,
            );
            report.files_checked += 1;
            report.issues.append(&mut file_issues);
        }

        // Audit extracted snapshot plates if present on disk
        let snap_dir = self.repo_root.join(&self.config.snapshots.output_dir);
        if snap_dir.is_dir() {
            if let Ok(entries) = fs::read_dir(&snap_dir) {
                let mut snap_files: Vec<PathBuf> = entries
                    .flatten()
                    .map(|e| e.path())
                    .filter(|p| {
                        p.file_name()
                            .and_then(|s| s.to_str())
                            .map(|n| n.starts_with("snap_") && n.ends_with(".adoc"))
                            .unwrap_or(false)
                    })
                    .collect();
                snap_files.sort();
                for path in snap_files {
                    if let Ok(raw) = fs::read_to_string(&path) {
                        let rel = path
                            .strip_prefix(&self.repo_root)
                            .unwrap_or(&path)
                            .to_string_lossy()
                            .to_string();
                        let mut issues = Self::validate_adoc_text(
                            &rel,
                            &snap_dir,
                            &raw,
                            false,
                            true,
                            &rules,
                        );
                        report.snapshots_checked += 1;
                        report.issues.append(&mut issues);
                    }
                }
            }
        }

        Ok(report)
    }

    /// Validates one AsciiDoc text buffer for PDF single-page fit, fence integrity, includes, and 5-block banners.
    pub fn validate_adoc_text(
        rel_label: &str,
        base_dir: &Path,
        content: &str,
        strict_5block: bool,
        is_snapshot_plate: bool,
        rules: &AdocLayoutRulesFile,
    ) -> Vec<AdocLayoutIssue> {
        let mut issues = Vec::new();
        let geom = &rules.block_3_body.page_geometry;
        let cfg = &rules.block_3_body.validation_rules;

        let mut fence_stack: Vec<(&'static str, usize)> = Vec::new();
        let mut seen_title = false;
        let mut seen_blocks: HashSet<String> = HashSet::new();
        let mut current_code_lines: usize = 0;
        let mut current_code_start: usize = 0;
        let mut first_nonblank_checked = false;

        let lines: Vec<&str> = content.lines().collect();
        for (idx, line) in lines.iter().enumerate() {
            let line_no = idx + 1;
            let trimmed = line.trim();

            if !first_nonblank_checked && !trimmed.is_empty() {
                first_nonblank_checked = true;
                if strict_5block && trimmed != "////" {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: line_no,
                        severity: LayoutSeverity::Error,
                        rule: "require_5block_banners_in_strict_targets".to_string(),
                        message: "Strict 5-block AsciiDoc file must open with leading //// BLOCK 1 banner before doc title.".to_string(),
                    });
                }
            }

            let in_verbatim = fence_stack.iter().any(|(f, _)| {
                matches!(*f, "////" | "----" | "------" | "...." | "```")
            });
            let active_comment = fence_stack.last().map(|(f, _)| *f) == Some("////");
            let active_code = fence_stack.last().map(|(f, _)| *f);

            if active_comment {
                for tok in &cfg.required_block_tokens {
                    if trimmed.contains(tok.as_str()) {
                        seen_blocks.insert(tok.clone());
                    }
                }
                if trimmed == "////" {
                    fence_stack.pop();
                }
                continue;
            }

            if matches!(active_code, Some("----") | Some("------") | Some("....") | Some("```")) {
                let top = active_code.unwrap_or("----");
                let closed = (top == "```" && trimmed.starts_with("```")) || trimmed == top;
                if closed {
                    if is_snapshot_plate
                        && cfg.enforce_snapshot_line_and_width_budgets
                        && current_code_lines > geom.max_inline_snapshot_lines
                    {
                        issues.push(AdocLayoutIssue {
                            file: rel_label.to_string(),
                            line: current_code_start,
                            severity: LayoutSeverity::Warning,
                            rule: "snapshot_single_page_height".to_string(),
                            message: format!(
                                "Inline snapshot listing is {} lines (exceeds {} single-page limit; cap excerpt.max_lines).",
                                current_code_lines, geom.max_inline_snapshot_lines
                            ),
                        });
                    }
                    fence_stack.pop();
                    current_code_lines = 0;
                    continue;
                }

                if cfg.forbid_listing_delimiter_collisions && top == "----" && trimmed == "----" {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: line_no,
                        severity: LayoutSeverity::Error,
                        rule: "forbid_listing_delimiter_collisions".to_string(),
                        message: "Nested 4-hyphen line inside ---- fence closes listing prematurely.".to_string(),
                    });
                }

                current_code_lines += 1;
                let char_len = line.chars().count();
                let is_box_art = line.contains('┌') || line.contains('│') || line.contains('└');
                if !is_box_art && (strict_5block || is_snapshot_plate) && cfg.enforce_snapshot_line_and_width_budgets {
                    if char_len > geom.hard_code_cols_max {
                        issues.push(AdocLayoutIssue {
                            file: rel_label.to_string(),
                            line: line_no,
                            severity: LayoutSeverity::Error,
                            rule: "hard_code_cols_max".to_string(),
                            message: format!(
                                "Code line width {} exceeds hard PDF wrap limit {} cols.",
                                char_len, geom.hard_code_cols_max
                            ),
                        });
                    } else if char_len > geom.safe_code_cols_with_linenums {
                        issues.push(AdocLayoutIssue {
                            file: rel_label.to_string(),
                            line: line_no,
                            severity: LayoutSeverity::Warning,
                            rule: "safe_code_cols_with_linenums".to_string(),
                            message: format!(
                                "Code line width {} exceeds {} cols safe linenums budget (may soft-wrap in PDF).",
                                char_len, geom.safe_code_cols_with_linenums
                            ),
                        });
                    }
                }
                continue;
            }

            // Outside verbatim fences: handle matching delimiters for `|===`, `====`, `****`, `--`
            if let Some(&(top, _)) = fence_stack.last() {
                let closed = match top {
                    "|===" => trimmed == "|===",
                    "====" => trimmed == "====",
                    "****" => trimmed == "****",
                    "--" => trimmed == "--",
                    _ => false,
                };
                if closed {
                    fence_stack.pop();
                    continue;
                }
            }

            if !in_verbatim {
                let opening = if trimmed.starts_with("```") {
                    Some("```")
                } else {
                    match trimmed {
                        "////" => Some("////"),
                        "------" => Some("------"),
                        "----" => Some("----"),
                        "...." => Some("...."),
                        "====" => Some("===="),
                        "****" => Some("****"),
                        "|===" => Some("|==="),
                        "--" => Some("--"),
                        _ => None,
                    }
                };
                if let Some(tok) = opening {
                    if matches!(tok, "----" | "------" | "...." | "```") {
                        current_code_lines = 0;
                        current_code_start = line_no;
                    }
                    fence_stack.push((tok, line_no));
                    continue;
                }
            }

            // Document title vs spurious mid-file `= ` headings
            if trimmed.starts_with("= ") && !trimmed.starts_with("== ") {
                if !seen_title {
                    seen_title = true;
                } else if cfg.forbid_midfile_level0_headings && !is_snapshot_plate {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: line_no,
                        severity: LayoutSeverity::Error,
                        rule: "forbid_midfile_level0_headings".to_string(),
                        message: "Mid-file level-0 (= ) heading injects spurious book Parts during PDF compilation.".to_string(),
                    });
                }
            }

            // Forbid [abstract] right above == section heading (book mode scrambles numbering & wastes blank pages)
            if cfg.forbid_spurious_abstract_headings && trimmed == "[abstract]" {
                let next_trimmed = lines
                    .get(idx + 1)
                    .map(|l| l.trim())
                    .unwrap_or("");
                if next_trimmed.starts_with("== ") {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: line_no,
                        severity: if strict_5block {
                            LayoutSeverity::Error
                        } else {
                            LayoutSeverity::Warning
                        },
                        rule: "forbid_spurious_abstract_headings".to_string(),
                        message: "[abstract] directly above == heading scrambles Asciidoctor book chapter numbers.".to_string(),
                    });
                }
            }

            // Forbid legacy thematic breaks `'''` in strict modernized files
            if strict_5block && cfg.forbid_thematic_break_in_strict_targets && trimmed == "'''" {
                issues.push(AdocLayoutIssue {
                    file: rel_label.to_string(),
                    line: line_no,
                    severity: LayoutSeverity::Error,
                    rule: "forbid_thematic_break_in_strict_targets".to_string(),
                    message: "Legacy ''' horizontal rule creates unnecessary vertical PDF page breaks; use section hierarchy instead.".to_string(),
                });
            }

            // Check relative include:: targets resolve cleanly
            if cfg.verify_include_paths_exist && trimmed.starts_with("include::") {
                if let Some(rest) = trimmed.strip_prefix("include::") {
                    if let Some(br) = rest.find('[') {
                        let rel_inc = rest[..br].trim();
                        if !rel_inc.contains('{') && !base_dir.join(rel_inc).is_file() {
                            issues.push(AdocLayoutIssue {
                                file: rel_label.to_string(),
                                line: line_no,
                                severity: LayoutSeverity::Error,
                                rule: "verify_include_paths_exist".to_string(),
                                message: format!(
                                    "Include target '{}' does not resolve relative to '{}'.",
                                    rel_inc,
                                    base_dir.display()
                                ),
                            });
                        }
                    }
                }
            }
        }

        if cfg.require_balanced_delimiters && !fence_stack.is_empty() {
            for (tok, opened_line) in fence_stack {
                issues.push(AdocLayoutIssue {
                    file: rel_label.to_string(),
                    line: opened_line,
                    severity: LayoutSeverity::Error,
                    rule: "require_balanced_delimiters".to_string(),
                    message: format!("Unclosed AsciiDoc delimiter '{}' opened at line {}.", tok, opened_line),
                });
            }
        }

        if strict_5block && cfg.require_5block_banners_in_strict_targets {
            for req in &cfg.required_block_tokens {
                if !seen_blocks.contains(req) {
                    issues.push(AdocLayoutIssue {
                        file: rel_label.to_string(),
                        line: 1,
                        severity: LayoutSeverity::Error,
                        rule: "require_5block_banners_in_strict_targets".to_string(),
                        message: format!("Missing 5-block AsciiDoc comment banner containing '{}'.", req),
                    });
                }
            }
        }

        issues
    }
}

fn lines_has_comment_banner(lines: &[String], target_line: &str) -> bool {
    let pos = lines.iter().position(|l| std::ptr::eq(l.as_str(), target_line));
    if let Some(idx) = pos {
        MasterGuideBuilder::is_inside_banner_frame(lines, idx)
    } else {
        false
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / RUNNER TASK
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

/// Modular runner task wrapping the MasterGuideBuilder.
pub struct MasterGuideTask;

impl TaskRunner for MasterGuideTask {
    fn name(&self) -> &'static str {
        "master_guide"
    }

    fn description(&self) -> &'static str {
        "Compiles the data-driven Master Guide, snapshots & volume books (.adoc, .md, .pdf)"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        let builder = MasterGuideBuilder::new(&ctx.repo_root);
        let mut missing_count = 0;
        let mut total_count = 0;

        for vol in &builder.volumes {
            for ch in &vol.chapters {
                total_count += 1;
                if !ch.source_path.exists() {
                    eprintln!("  [MISSING CHAPTER] {:?}", ch.source_path);
                    missing_count += 1;
                }
            }
        }

        if missing_count > 0 {
            return Err(format!(
                "Master Guide preflight failed: {}/{} chapters missing on disk.",
                missing_count, total_count
            ));
        }

        println!(
            "  [PASS] Preflight verified: all {} chapters present across {} volumes.",
            total_count,
            builder.volumes.len()
        );
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let builder = MasterGuideBuilder::new(&ctx.repo_root);
        let (adoc_p, md_p, pdf_p) = builder.build_master()?;
        let mut tel = TaskTelemetry::success(
            self.name(),
            0,
            "Compiled unified MASTER_GUIDE (.adoc, .md, .pdf) successfully.",
        )
        .with_artifact(adoc_p)
        .with_artifact(md_p)
        .with_artifact(pdf_p);

        if let Ok(vol_artifacts) = builder.build_volumes() {
            for (_roman, vadoc, vmd, vpdf) in vol_artifacts {
                tel = tel.with_artifact(vadoc).with_artifact(vmd).with_artifact(vpdf);
            }
        }

        Ok(tel)
    }

    fn post_verify(&self, ctx: &RunnerContext, telemetry: &TaskTelemetry) -> Result<(), String> {
        let adoc_path = ctx.docs_dir.join("MASTER_GUIDE.adoc");
        let md_path = ctx.docs_dir.join("MASTER_GUIDE.md");

        if !adoc_path.is_file() || !md_path.is_file() {
            return Err("Expected master guide artifacts were not generated.".to_string());
        }

        for path in &telemetry.artifacts {
            if path.exists() {
                println!("  [POST-VERIFY] Artifact confirmed: {:?}", path);
            }
        }

        Ok(())
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS & INVARIANT VERIFICATION
// Stratum: Developer Tooling | Machine: tools.guide.rust
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_adoc_header_stripping_standard_and_5block() {
        let legacy = "= Document Title\nAuthor\nv1.0\n:toc: left\n\n== Section 1\nContent";
        let stripped = MasterGuideBuilder::strip_adoc_header(legacy);
        assert!(stripped.starts_with("== Section 1"));

        let five_block = "\
////
BLOCK 1: METADATA BLOCK
@id docs.test
////

= 5-Block Chapter Title
CreativeWorkzStudio LLC
v0.2.0-dev, 2026-11-12
:toc: macro

////
BLOCK 2: OPENING BLOCK
////

== Section One
Body text";
        let stripped_5 = MasterGuideBuilder::strip_adoc_header(five_block);
        assert!(stripped_5.contains("BLOCK 2: OPENING BLOCK"));
        assert!(!stripped_5.contains("BLOCK 1: METADATA BLOCK"));
        assert!(!stripped_5.contains(":toc: macro"));

        let title_path = std::env::temp_dir().join("test_5block_title.adoc");
        let _ = fs::write(&title_path, five_block);
        assert_eq!(MasterGuideBuilder::extract_title_from_file(&title_path), "5-Block Chapter Title");
        let _ = fs::remove_file(title_path);
    }

    #[test]
    fn test_shift_section_levels_ignores_fences_and_strips_manual_nums() {
        let adoc = "\
== 1. Intro
=== 1.1 Subtopic
[source,cmake]
----
== Not a heading inside code fence
----
==== Deeper
== 2017–2018 Demo Notes
";
        let shifted = MasterGuideBuilder::shift_section_levels(adoc, 1, true);
        assert!(shifted.contains("=== Intro\n"));
        assert!(shifted.contains("==== Subtopic\n"));
        assert!(shifted.contains("== Not a heading inside code fence\n"));
        assert!(shifted.contains("===== Deeper\n"));
        assert!(shifted.contains("=== 2017–2018 Demo Notes\n"));
    }

    #[test]
    fn test_adoc_to_markdown_fences_admonitions_comments_and_tables() {
        let adoc = "\
////
Hidden 5-block banner
////
// Hidden subassembly comment
== Architecture

[NOTE]
====
Remember that *Layers 0-2* have zero heap!
====

[source,c,linenums,start=10]
----
/* ==== inside code stays ==== */
int x = 1;
----

[cols=\"1,2\",options=\"header\"]
|===
| Layer | Purpose
| Layer 0 | Core utilities
|===

[cols=\"1,1,2\",options=\"header\"]
|===
| Profile | Root
| Scope

| Machine Profiles
| `engine/config/`
a|
* `viewport/`: window layout.
* `io/`: VFS routes.
|===

[qanda]
Why zero heap?::
  Zero heap keeps every byte visible in static ELF sections.

```text
  [Core] ---> [Sim]
```
";
        let md = MasterGuideBuilder::adoc_to_markdown(adoc);
        assert!(!md.contains("Hidden 5-block banner"));
        assert!(!md.contains("Hidden subassembly comment"));
        assert!(!md.contains("####"));
        assert!(md.contains("## Architecture"));
        assert!(md.contains("> **[NOTE]**"));
        assert!(md.contains("> Remember that *Layers 0-2* have zero heap!"));
        assert!(md.contains("```c\n/* ==== inside code stays ==== */\nint x = 1;\n```"));
        assert!(md.contains("| Layer | Purpose |\n| --- | --- |\n| Layer 0 | Core utilities |"));
        assert!(md.contains(
            "| Machine Profiles | `engine/config/` | • `viewport/`: window layout.<br>• `io/`: VFS routes. |"
        ));
        assert!(md.contains("**Why zero heap?**"));
        assert!(md.contains("Zero heap keeps every byte visible in static ELF sections."));
        assert!(md.contains("```text\n  [Core] ---> [Sim]\n```"));
    }

    #[test]
    fn test_snapshot_extraction_all_5_modes() {
        let c_lines: Vec<String> = "\
/*
================================================================================
BLOCK 1: METADATA BLOCK
================================================================================
*/

/*
================================================================================
BLOCK 2: OPENING BLOCK
================================================================================
*/
/* --- Sub-Assembly 2.1: Limits --- */
#define MAX_X 16

/* --- Sub-Assembly 2.2: Structs --- */
typedef struct foo_s { int v; } foo_t;

/** Adds two integers. */
int helper_add(int a, int b) {
    return a + b;
}

int middle_unrelated(void) {
    return 0;
}

/** Multiplies after calling helper_add. */
int worker_mul(int x, int y) {
    int s = helper_add(x, y);
    return s * 2;
}
"
        .lines()
        .map(|s| s.to_string())
        .collect();

        // Mode 1: Block
        let blk = MasterGuideBuilder::extract_block_segment(&c_lines, "BLOCK 1", "c").unwrap();
        assert_eq!(blk.start_line, 1);
        assert!(blk.lines.iter().any(|l| l.contains("BLOCK 1")));
        assert!(!blk.lines.iter().any(|l| l.contains("BLOCK 2")));

        // Mode 2: Sub-Assembly
        let sub = MasterGuideBuilder::extract_subassembly_segment(&c_lines, "2.1").unwrap();
        assert!(sub.lines.iter().any(|l| l.contains("MAX_X 16")));
        assert!(!sub.lines.iter().any(|l| l.contains("foo_t")));

        // Mode 3: Single function (must NOT match call inside worker_mul)
        let f = MasterGuideBuilder::extract_symbol_segment(&c_lines, "helper_add", "c").unwrap();
        assert!(f.lines[0].contains("Adds two integers"));
        assert!(f.lines.last().unwrap().contains('}'));
        assert!(!f.lines.iter().any(|l| l.contains("worker_mul")));

        // Mode 4: Group across non-contiguous functions
        let g1 = MasterGuideBuilder::extract_symbol_segment(&c_lines, "helper_add", "c").unwrap();
        let g2 = MasterGuideBuilder::extract_symbol_segment(&c_lines, "worker_mul", "c").unwrap();
        let merged = MasterGuideBuilder::merge_segments(&c_lines, vec![g1, g2]);
        assert_eq!(merged.len(), 2);

        // Mode 5: JSON block extraction
        let json_lines: Vec<String> = "{\n  \"block_1_metadata\": {\n    \"id\": \"x\"\n  },\n  \"block_3_body\": {\n    \"a\": 1,\n    \"b\": [2, 3]\n  }\n}\n"
            .lines()
            .map(|s| s.to_string())
            .collect();
        let jblk = MasterGuideBuilder::extract_block_segment(&json_lines, "block_3_body", "json").unwrap();
        assert!(jblk.lines[0].contains("block_3_body"));
        assert!(jblk.lines.iter().any(|l| l.contains("\"b\": [2, 3]")));
    }

    #[test]
    fn test_extract_title_from_adoc_header() {
        let temp_dir = std::env::temp_dir();
        let test_file = temp_dir.join("test_chapter_title.adoc");
        let _ = fs::write(&test_file, "= Canonical Test Chapter Title\nAuthor Name\n\n== Body\nText");
        let title = MasterGuideBuilder::extract_title_from_file(&test_file);
        let _ = fs::remove_file(test_file);
        assert_eq!(title, "Canonical Test Chapter Title");
    }

    #[test]
    fn test_volume_slug_and_roman_spec() {
        let vol = VolumeSpec {
            roman: "IV".to_string(),
            slug: "demo_baseline".to_string(),
            title: "Demo Baseline".to_string(),
            description: "Historical extraction".to_string(),
            scan_areas: vec!["game_design/demo_baseline".to_string()],
            chapters: vec![],
        };
        assert_eq!(vol.roman, "IV");
        assert_eq!(vol.slug, "demo_baseline");
    }

    #[test]
    fn test_pdf_layout_validator_and_six_dash_fences() {
        let mut rules = AdocLayoutRulesFile {
            block_2_opening: AdocLayoutOpening::default(),
            block_3_body: AdocLayoutBody::default(),
        };
        rules.block_3_body.validation_rules.required_block_tokens = vec![
            "BLOCK 1:".to_string(),
            "BLOCK 2:".to_string(),
            "BLOCK 3:".to_string(),
            "BLOCK 4:".to_string(),
            "BLOCK 5:".to_string(),
        ];

        let valid_doc = "\
////
BLOCK 1: METADATA BLOCK
////
= Sample Guide
Author
v0.2.0-dev, 2026-11-12

////
BLOCK 2: OPENING BLOCK
////

////
BLOCK 3: BODY BLOCK
////
== Core

[source,c]
------
/* ---- nested 4 hyphens stay inside 6-dash block ---- */
int ok = 1;
------

////
BLOCK 4: AUDIT BLOCK
////

////
BLOCK 5: CLOSING BLOCK
////
";
        let ok_issues = MasterGuideBuilder::validate_adoc_text(
            "docs/code/log/sample.adoc",
            Path::new("."),
            valid_doc,
            true,
            false,
            &rules,
        );
        assert!(ok_issues.is_empty(), "Expected clean validation, got {:?}", ok_issues);

        let md = MasterGuideBuilder::adoc_to_markdown(valid_doc);
        assert!(md.contains("```c\n/* ---- nested 4 hyphens stay inside 6-dash block ---- */\nint ok = 1;\n```"));

        let broken_doc = "= Title Without Leading Block 1\n\n[abstract]\n== Broken Abstract Chapter\n'''\n";
        let bad_issues = MasterGuideBuilder::validate_adoc_text(
            "docs/code/log/broken.adoc",
            Path::new("."),
            broken_doc,
            true,
            false,
            &rules,
        );
        assert!(bad_issues.iter().any(|i| i.rule == "require_5block_banners_in_strict_targets"));
        assert!(bad_issues.iter().any(|i| i.rule == "forbid_spurious_abstract_headings"));
        assert!(bad_issues.iter().any(|i| i.rule == "forbid_thematic_break_in_strict_targets"));
    }
}
