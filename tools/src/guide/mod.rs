// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/guide/mod.rs
// @id            cog.tools.guide.root
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     facade_umbrella | orchestrator
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Guide Orchestrator Sub-Assembly
// @role          Central Documentation Builder, Snapshot Integrator & Volume Compiler
// @maturity      Rev 5 Baseline | Modularized documentation pipeline
// @chassis       bounded-heap | Developer tooling memory chassis; bounded vectors
// @scale         O(V + C + S) across volumes, chapters, and snapshots
// @seams         in: docs/ | uses: guide::{manifest, theme, snapshot, compiler, pdf}
// @invariants    1. Zero missing chapter tolerance during pre-flight audit.
//                2. Deterministic volume and chapter sequence matching project ontology.
//                3. Dual granularity: unified master compendium and volume books.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Acts as the central orchestrator and facade for the Master Guide
//   compilation suite. Unifies manifest loading, textbook theming, 5-mode code
//   snapshots, AsciiDoc-to-Markdown translation, and PDF layout validation.
//
//   ### For Game Designers & Non-Programmers:
//   Coordinates the assembly of all lore, systems, architecture, and devlog
//   documents into polished multi-volume books and unified single-file compendiums.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Orchestrator delegates to stateless sub-assemblies.
//   - Extension Seam: New publication formats register in `compiler` or `pdf`.
//   - Explicit Non-Goal: Does not spawn external processes during document assembly.
// =============================================================================

//! # Master Guide Compilation Suite
//!
//! Provides modular compilation of the 6-volume Tenebris and Lux Master Guide,
//! integrating data-driven themes, textbook code snapshots, and PDF layout rules.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Submodules, Re-Exports & Public Guide Types
// -----------------------------------------------------------------------------

pub mod compiler;
pub mod manifest;
pub mod pdf;
pub mod snapshot;
pub mod theme;

use std::fs;
use std::path::{Path, PathBuf};

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};

// Re-export primary types for public consumption and backward compatibility
pub use compiler::{
    adoc_to_markdown, expand_includes, extract_title_from_file,
    scan_area_for_chapters, shift_section_levels, strip_adoc_header,
};
pub use manifest::{
    default_config, load_manifest_and_config, load_volumes_from_manifest,
    save_default_manifest, ChapterSpec, GuideConfig, GuideManifest,
    GuideManifestBody, GuideManifestChapter, GuideManifestMetadata,
    GuideManifestTypes, GuideManifestVolume, PublicationSettings,
    SnapshotSettings, VolumeSpec,
};
pub use pdf::{
    compile_pdf, is_strict_layout_target, load_layout_rules, validate_adoc_layout,
    validate_adoc_text, AdocLayoutBody, AdocLayoutIssue, AdocLayoutOpening,
    AdocLayoutReport, AdocLayoutRulesFile, AdocPageGeometry, AdocValidationRules,
    LayoutSeverity,
};
pub use snapshot::{
    extract_block_segment, extract_excerpt_segments, extract_segments_for_spec,
    extract_subassembly_segment, extract_symbol_segment, format_snapshot_plate,
    format_snapshots_index, format_wholesale_appendix_entry, infer_language,
    is_inside_banner_frame, materialize_snapshot, merge_segments, prepare_snapshots,
    snapshot_goes_in_appendix, CodeSnapshotSpec, CodeSnapshotsBody,
    CodeSnapshotsManifest, ExtractedSegment, RenderedSnapshot, SnapshotCalloutSpec,
    SnapshotExcerptSpec, SnapshotMode, WholesalePolicy,
};
pub use theme::{
    render_theme_yaml, sync_theme_yaml, GuideThemeBody, GuideThemeManifest,
    GuideThemeMeta, ThemeBlocks, ThemeBreaks, ThemePage, ThemeRunningChrome,
    ThemeSpacing, ThemeTypography,
};

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( MasterGuideBuilder Orchestration Engine & Facade Methods )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: MasterGuideBuilder Orchestration Engine
// -----------------------------------------------------------------------------

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
    ///
    /// # Examples
    ///
    /// ```rust
    /// use std::path::Path;
    /// use tl_tools::guide::MasterGuideBuilder;
    ///
    /// let builder = MasterGuideBuilder::new(Path::new("."));
    /// assert_eq!(builder.config.toc_levels, 2);
    /// ```
    pub fn new(repo_root: &Path) -> Self {
        let docs_dir = repo_root.join("docs");
        let volumes_dir = docs_dir.join("volumes");
        let manifest_path = docs_dir.join("master_guide_manifest.json");

        let (volumes, config) = if manifest_path.is_file() {
            match load_manifest_and_config(repo_root, &docs_dir, &manifest_path) {
                Ok(res) => res,
                Err(err) => {
                    eprintln!(
                        "  [WARN] Failed to load {:?}: {}. Using canonical fallback.",
                        manifest_path, err
                    );
                    (
                        manifest::build_canonical_volumes(&docs_dir),
                        default_config(repo_root),
                    )
                }
            }
        } else {
            let fallback_vols = manifest::build_canonical_volumes(&docs_dir);
            let _ = save_default_manifest(&docs_dir, &manifest_path, &fallback_vols);
            (fallback_vols, default_config(repo_root))
        };

        Self {
            repo_root: repo_root.to_path_buf(),
            docs_dir,
            volumes_dir,
            volumes,
            config,
        }
    }

    /// Loads snapshot plates into disk cache.
    pub fn prepare_snapshots(&self) -> Result<Vec<RenderedSnapshot>, String> {
        snapshot::prepare_snapshots(&self.repo_root, &self.config)
    }

    /// Prepares build assets including theme YAML and snapshots.
    pub fn prepare_build_assets(&self) -> Result<Vec<RenderedSnapshot>, String> {
        let _ = self.sync_theme_yaml()?;
        self.prepare_snapshots()
    }

    /// Synchronizes data-driven theme JSON into Asciidoctor-PDF YAML theme file.
    pub fn sync_theme_yaml(&self) -> Result<Option<PathBuf>, String> {
        theme::sync_theme_yaml(&self.repo_root, &self.config)
    }

    /// Compiles both the unified Master Guide compendium and all individual volume books.
    pub fn build_all(&self) -> Result<(PathBuf, PathBuf, PathBuf), String> {
        let master = self.build_master()?;
        let _ = self.build_volumes()?;
        Ok(master)
    }

    /// Compiles the single unified master compendium (MASTER_GUIDE.{adoc, md, pdf}).
    pub fn build_master(&self) -> Result<(PathBuf, PathBuf, PathBuf), String> {
        println!(">>> Compiling Unified Master Guide across Volumes & Chapters...");
        let snapshots = self.prepare_build_assets()?;

        let adoc_path = self.docs_dir.join("MASTER_GUIDE.adoc");
        let md_path = self.docs_dir.join("MASTER_GUIDE.md");
        let pdf_path = self.docs_dir.join("MASTER_GUIDE.pdf");

        let (adoc_content, md_content) = compiler::assemble_content_with_snapshots(
            &self.repo_root,
            &self.docs_dir,
            &self.volumes,
            &self.config,
            &snapshots,
        )?;

        println!("  Writing {:?}...", adoc_path);
        fs::write(&adoc_path, &adoc_content)
            .map_err(|e| format!("Failed to write MASTER_GUIDE.adoc: {}", e))?;

        println!("  Writing {:?} (Optimized for NotebookLM & Gemini)...", md_path);
        fs::write(&md_path, &md_content)
            .map_err(|e| format!("Failed to write MASTER_GUIDE.md: {}", e))?;

        pdf::compile_pdf(
            &adoc_path,
            &pdf_path,
            "Unified Master Guide",
            &self.config,
        );
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
    pub fn build_single_volume(
        &self,
        identifier: &str,
    ) -> Result<(PathBuf, PathBuf, PathBuf), String> {
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

        let (adoc_content, md_content) = compiler::assemble_volume_with_snapshots(
            &self.repo_root,
            &self.docs_dir,
            vol,
            &self.config,
            snapshots,
        )?;

        println!("  Writing Volume {} AsciiDoc: {:?}...", vol.roman, adoc_path);
        fs::write(&adoc_path, &adoc_content)
            .map_err(|e| format!("Failed to write {:?}: {}", adoc_path, e))?;

        println!("  Writing Volume {} Markdown: {:?}...", vol.roman, md_path);
        fs::write(&md_path, &md_content)
            .map_err(|e| format!("Failed to write {:?}: {}", md_path, e))?;

        pdf::compile_pdf(
            &adoc_path,
            &pdf_path,
            &format!("Volume {}", vol.roman),
            &self.config,
        );
        Ok((adoc_path, md_path, pdf_path))
    }

    /// Assembles the unified AsciiDoc and Markdown text documents.
    pub fn assemble_content(&self) -> Result<(String, String), String> {
        let snapshots = self.prepare_snapshots().unwrap_or_default();
        compiler::assemble_content_with_snapshots(
            &self.repo_root,
            &self.docs_dir,
            &self.volumes,
            &self.config,
            &snapshots,
        )
    }

    /// Assembles the text documents for an individual standalone volume.
    pub fn assemble_volume_content(&self, vol: &VolumeSpec) -> Result<(String, String), String> {
        let snapshots = self.prepare_snapshots().unwrap_or_default();
        compiler::assemble_volume_with_snapshots(
            &self.repo_root,
            &self.docs_dir,
            vol,
            &self.config,
            &snapshots,
        )
    }

    /// Loads layout rules from `docs/adoc_pdf_layout_rules.json`.
    pub fn load_layout_rules(&self) -> Result<AdocLayoutRulesFile, String> {
        pdf::load_layout_rules(&self.docs_dir)
    }

    /// Validates all chapters and plates against PDF layout geometry rules.
    pub fn validate_adoc_layout(&self) -> Result<AdocLayoutReport, String> {
        pdf::validate_adoc_layout(
            &self.repo_root,
            &self.docs_dir,
            &self.volumes,
            &self.config,
        )
    }

    // -------------------------------------------------------------------------
    // Backward-Compatible Facade Delegates
    // -------------------------------------------------------------------------

    pub fn load_volumes_from_manifest(
        docs_dir: &Path,
        path: &Path,
    ) -> Result<Vec<VolumeSpec>, String> {
        manifest::load_volumes_from_manifest(docs_dir, path)
    }

    pub fn scan_area_for_chapters(docs_dir: &Path, area_rel: &str) -> Vec<(String, PathBuf)> {
        compiler::scan_area_for_chapters(docs_dir, area_rel)
    }

    pub fn extract_title_from_file(path: &Path) -> String {
        compiler::extract_title_from_file(path)
    }

    pub fn render_theme_yaml(extends: &str, body: &GuideThemeBody) -> String {
        theme::render_theme_yaml(extends, body)
    }

    pub fn strip_adoc_header(content: &str) -> String {
        compiler::strip_adoc_header(content)
    }

    pub fn expand_includes(base_dir: &Path, content: &str, depth: usize) -> String {
        compiler::expand_includes(base_dir, content, depth)
    }

    pub fn shift_section_levels(content: &str, shift: usize, strip_manual_nums: bool) -> String {
        compiler::shift_section_levels(content, shift, strip_manual_nums)
    }

    pub fn adoc_to_markdown(adoc: &str) -> String {
        compiler::adoc_to_markdown(adoc)
    }

    pub fn materialize_snapshot(
        repo_root: &Path,
        out_dir: &Path,
        spec: CodeSnapshotSpec,
    ) -> Result<RenderedSnapshot, String> {
        snapshot::materialize_snapshot(repo_root, out_dir, spec)
    }

    pub fn infer_language(source_file: &str) -> String {
        snapshot::infer_language(source_file)
    }

    pub fn format_snapshot_plate(snap: &RenderedSnapshot, show_linenums: bool) -> String {
        snapshot::format_snapshot_plate(snap, show_linenums)
    }

    pub fn snapshot_goes_in_appendix(snap: &RenderedSnapshot) -> bool {
        snapshot::snapshot_goes_in_appendix(snap)
    }

    pub fn format_wholesale_appendix_entry(
        snap: &RenderedSnapshot,
        show_linenums: bool,
    ) -> String {
        snapshot::format_wholesale_appendix_entry(snap, show_linenums)
    }

    pub fn validate_adoc_text(
        rel_label: &str,
        base_dir: &Path,
        content: &str,
        strict_5block: bool,
        is_snapshot_plate: bool,
        rules: &AdocLayoutRulesFile,
    ) -> Vec<AdocLayoutIssue> {
        pdf::validate_adoc_text(
            rel_label,
            base_dir,
            content,
            strict_5block,
            is_snapshot_plate,
            rules,
        )
    }

    pub fn extract_block_segment(
        lines: &[String],
        target: &str,
        lang: &str,
    ) -> Option<ExtractedSegment> {
        snapshot::extract_block_segment(lines, target, lang)
    }

    pub fn extract_subassembly_segment(
        lines: &[String],
        target: &str,
    ) -> Option<ExtractedSegment> {
        snapshot::extract_subassembly_segment(lines, target)
    }

    pub fn extract_symbol_segment(
        lines: &[String],
        symbol: &str,
        lang: &str,
    ) -> Option<ExtractedSegment> {
        snapshot::extract_symbol_segment(lines, symbol, lang)
    }

    pub fn merge_segments(
        all_lines: &[String],
        segs: Vec<ExtractedSegment>,
    ) -> Vec<ExtractedSegment> {
        snapshot::merge_segments(all_lines, segs)
    }

    pub fn is_inside_banner_frame(lines: &[String], idx: usize) -> bool {
        snapshot::is_inside_banner_frame(lines, idx)
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( TaskRunner Implementation & Unit Test Suite )
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
                tel = tel
                    .with_artifact(vadoc)
                    .with_artifact(vmd)
                    .with_artifact(vpdf);
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
    fn test_section_1_nominal_builder_construction() {
        let builder = MasterGuideBuilder::new(Path::new("."));
        assert_eq!(builder.config.toc_levels, 2);
        assert!(!builder.volumes.is_empty());
    }

    #[test]
    fn test_section_1_nominal_subsystems_readiness() {
        assert!(manifest::is_manifest_subsystem_ready());
        assert!(theme::is_theme_subsystem_ready());
        assert!(snapshot::is_snapshot_subsystem_ready());
        assert!(compiler::is_compiler_subsystem_ready());
        assert!(pdf::is_pdf_subsystem_ready());
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_invalid_volume_index() {
        let builder = MasterGuideBuilder::new(Path::new("."));
        let res = builder.build_single_volume("9999");
        assert!(res.is_err());
        assert!(res.unwrap_err().contains("not found in manifest"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_builder_directories() {
        let builder = MasterGuideBuilder::new(Path::new("."));
        assert_eq!(builder.docs_dir, Path::new("./docs"));
        assert_eq!(builder.volumes_dir, Path::new("./docs/volumes"));
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_task_metadata() {
        let task = MasterGuideTask;
        assert_eq!(task.name(), "master_guide");
        assert!(task.description().contains("Master Guide"));
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of all guide sub-assemblies.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    manifest::self_audit()?;
    theme::self_audit()?;
    snapshot::self_audit()?;
    compiler::self_audit()?;
    pdf::self_audit()?;
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Verifies that the entire guide module tree is operational and ready.
///
/// # Examples
///
/// ```rust
/// use tl_tools::guide::is_guide_subsystem_ready;
///
/// assert!(is_guide_subsystem_ready());
/// ```
pub fn is_guide_subsystem_ready() -> bool {
    self_audit().is_ok()
}
