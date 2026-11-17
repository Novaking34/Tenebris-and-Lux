// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/cmd/guide.rs
// @id            cog.tools.cmd.guide
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Master Guide Command Sub-Assembly
// @role          Compiles Unified Compendiums & Volume Books Across Formats
// @maturity      Rev 5 Baseline | Multi-format AsciiDoc, Markdown & PDF Compiler
// @chassis       bounded-heap | Document compilation memory chassis; bounded
// @scale         O(C) across C chapters (114 chapters across 6 volumes)
// @seams         in: docs/master_guide_manifest.json | uses: guide/ | probes: exit
// @invariants    1. Strict exit code semantics (0 = success, 1 = failure).
//                2. Zero missing chapter tolerance during pre-flight audit.
//                3. High-fidelity Markdown translation preserving callouts.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Defines the central documentation compiler command for the OmniCode developer
//   toolchain (`tl_tools`). Orchestrates multi-volume compendium generation,
//   Markdown conversions for NotebookLM/Gemini feedback loops, textbook code
//   snapshot plate extraction, and single-page PDF budget layout audits.
//
//   ### For Game Designers & Non-Programmers:
//   Turns distributed design notes, world chronicles, and technical specs into
//   cohesive, beautifully formatted books (`MASTER_GUIDE.pdf` and individual
//   volumes in `docs/books/`). Designers can generate PDF books locally with a
//   single command without learning complex compiler toolchains.
//
//   ### For Systems Programmers & Engine Architects:
//   - Splicing Engine: Extracts code snapshots directly from verified C headers
//     and sources into documentation without copy-paste drift.
//   - Layout Discipline: Proves single-page PDF boundaries through layout audit
//     tripwires, preventing awkward orphan lines or broken page overflows.
//   - Reentrancy: Compilers are stateless pipelines writing to disk.
// =============================================================================

//! # Master Guide Command Handler
//!
//! Provides CLI and programmatic dispatch for building the unified Master Guide,
//! volume books, code snapshot plates, and AsciiDoc PDF layout audits.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::{Path, PathBuf};
use crate::guide::{LayoutSeverity, MasterGuideBuilder};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Constants & Target Directories
// -----------------------------------------------------------------------------

/// Canonical path to documentation root directory relative to repository root.
pub const DOCS_DIR_NAME: &str = "docs";

/// Canonical path to generated books directory relative to documentation root.
pub const BOOKS_DIR_NAME: &str = "docs/books";

/// Canonical path to volume books directory relative to documentation root.
pub const VOLUMES_DIR_NAME: &str = "docs/volumes";

/// Canonical path to extracted snapshot plates relative to documentation root.
pub const SNAPSHOTS_DIR_NAME: &str = "docs/snapshots";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types & Publishing Modes
// -----------------------------------------------------------------------------

/// Publishing mode selected by command-line arguments.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::guide::GuideMode;
///
/// let mode = GuideMode::All;
/// assert_eq!(mode, GuideMode::All);
/// assert_ne!(mode, GuideMode::MasterOnly);
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum GuideMode {
    /// Compiles unified master compendium AND all volume books.
    All,
    /// Compiles unified master compendium only (`MASTER_GUIDE.*`).
    MasterOnly,
    /// Compiles all individual volume books in `docs/volumes/`.
    VolumesOnly,
    /// Compiles a single specified volume book by Roman numeral (e.g. "I", "VI").
    SingleVolume(String),
    /// Regenerates code snapshot plates and theme YAML without building PDFs.
    SnapshotsOnly,
    /// Audits `.adoc` files and snapshots against single-page PDF layout rules.
    ValidateLayout,
    /// Scans designated documentation areas and prints chapter manifests.
    ScanOnly,
    /// Displays command usage and available compilation options.
    Help,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Validated Configuration Structs & Defaults
// -----------------------------------------------------------------------------

/// Execution parameters and runtime flags for the guide command.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::guide::{GuideArgs, GuideMode};
/// use std::path::PathBuf;
///
/// let args = GuideArgs {
///     mode: GuideMode::MasterOnly,
///     repo_root: PathBuf::from("."),
/// };
/// assert_eq!(args.mode, GuideMode::MasterOnly);
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct GuideArgs {
    /// Selected publication mode.
    pub mode: GuideMode,
    /// Absolute or relative path to workspace repository root.
    pub repo_root: PathBuf,
}

impl Default for GuideArgs {
    /// Creates default guide parameters targeting full compilation (`All`).
    ///
    /// # Examples
    ///
    /// ```rust
    /// use tl_tools::cmd::guide::{GuideArgs, GuideMode};
    ///
    /// let args = GuideArgs::default();
    /// assert_eq!(args.mode, GuideMode::All);
    /// ```
    fn default() -> Self {
        Self {
            mode: GuideMode::All,
            repo_root: PathBuf::from("."),
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / COMPILER EXECUTION
// ( Operational Pipelines, Engine Dispatch & Standalone CLI Bridges )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Master Compendium Compilation Pipeline
// -----------------------------------------------------------------------------

/// Builds the unified master compendium across AsciiDoc, Markdown, and PDF.
fn run_master_build(builder: &MasterGuideBuilder) -> Result<(), String> {
    println!("Target Mode: MASTER COMPENDIUM ONLY");
    let (adoc, md, pdf) = builder.build_master()?;
    println!("------------------------------------------------------------");
    println!("Master Guide Artifacts:");
    println!("  AsciiDoc: {:?}", adoc);
    println!("  Markdown: {:?}", md);
    println!("  PDF Book: {:?}", pdf);
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Multi-Volume Publishing Pipeline
// -----------------------------------------------------------------------------

/// Builds individual volume compendiums across `docs/volumes/`.
fn run_volumes_build(builder: &MasterGuideBuilder) -> Result<(), String> {
    println!("Target Mode: VOLUME BOOKS ONLY");
    let vol_results = builder.build_volumes()?;
    println!("------------------------------------------------------------");
    println!("Volume Books Generated ({} volumes):", vol_results.len());
    for (roman, va, vm, vp) in vol_results {
        println!("  Volume {:4} -> PDF: {:?}", roman, vp);
        println!("                ADOC: {:?}", va);
        println!("                MD:   {:?}", vm);
    }
    Ok(())
}

/// Builds a single volume compendium identified by its Roman numeral.
fn run_single_volume_build(builder: &MasterGuideBuilder, id: &str) -> Result<(), String> {
    println!("Target Mode: SINGLE VOLUME ({})", id);
    let (adoc, md, pdf) = builder.build_single_volume(id)?;
    println!("------------------------------------------------------------");
    println!("Volume Artifacts:");
    println!("  AsciiDoc: {:?}", adoc);
    println!("  Markdown: {:?}", md);
    println!("  PDF Book: {:?}", pdf);
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Textbook Code Snapshot & Theme Pipeline
// -----------------------------------------------------------------------------

/// Extracts verified code snapshot plates from C sources into `docs/snapshots/`.
fn run_snapshots_build(builder: &MasterGuideBuilder) -> Result<(), String> {
    println!("Target Mode: SNAPSHOTS & THEME GENERATION ONLY");
    if let Some(theme_path) = builder.sync_theme_yaml()? {
        println!("  Theme YAML: {:?}", theme_path);
    }
    let snaps = builder.prepare_snapshots()?;
    println!(
        "  Generated {} textbook code snapshots into {:?}:",
        snaps.len(),
        builder.docs_dir.join("snapshots")
    );
    for s in &snaps {
        let first = s.wholesale_segments.first().map(|x| x.start_line).unwrap_or(1);
        let last = s.wholesale_segments.last().map(|x| x.end_line).unwrap_or(1);
        println!(
            "    [{:?}] {} -> {}#L{}-L{}",
            s.spec.mode, s.spec.id, s.spec.source_file, first, last
        );
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Dynamic Chapter Scan & Documentation Audit
// -----------------------------------------------------------------------------

/// Scans all registered documentation areas and prints chapter status manifests.
fn run_scan(builder: &MasterGuideBuilder) {
    println!("Target Mode: DYNAMIC AREA SCAN ONLY");
    println!("Scanning designated documentation areas in docs/...");
    let mut total_scanned = 0;
    for vol in &builder.volumes {
        println!("\nVolume {}: {} (Areas: {:?})", vol.roman, vol.title, vol.scan_areas);
        println!("  Registered Chapters ({} total):", vol.chapters.len());
        for ch in &vol.chapters {
            let exists_marker = if ch.source_path.exists() {
                "[EXISTS]"
            } else {
                "[MISSING]"
            };
            println!(
                "    {:9} Ch #{:2}: {} ({:?})",
                exists_marker, ch.number, ch.title, ch.source_path
            );
            total_scanned += 1;
        }
    }
    println!("\nTotal Documented Chapters: {}", total_scanned);
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.5: AsciiDoc Single-Page PDF Layout Validation
// -----------------------------------------------------------------------------

/// Audits `.adoc` source files against single-page PDF layout and fencing rules.
fn run_layout_validation(builder: &MasterGuideBuilder) -> Result<(), String> {
    println!("Target Mode: PDF PAGE-BUDGET & 5-BLOCK ASCIIDOC LAYOUT VALIDATION");
    let report = builder.validate_adoc_layout()?;
    println!(
        "  Audited {} AsciiDoc documents + {} extracted snapshot plates.",
        report.files_checked, report.snapshots_checked
    );
    for issue in &report.issues {
        let tag = match issue.severity {
            LayoutSeverity::Error => "[ERROR]",
            LayoutSeverity::Warning => "[WARN] ",
        };
        println!(
            "  {} {}:{} ({}) -> {}",
            tag, issue.file, issue.line, issue.rule, issue.message
        );
    }
    println!(
        "  Summary: {} errors, {} warnings.",
        report.error_count(),
        report.warning_count()
    );
    if !report.is_clean() {
        return Err(format!(
            "AsciiDoc PDF layout validation failed with {} errors.",
            report.error_count()
        ));
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.6: Command Dispatcher & Standalone CLI Bridge
// -----------------------------------------------------------------------------

/// Central entrypoint executing the requested documentation compiler task.
///
/// # Errors
///
/// Returns `Err(String)` if files are missing, layout fails, or compilation breaks.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::guide::{execute, GuideArgs, GuideMode};
/// use tl_tools::cmd::resolve_repo_root;
///
/// let root = resolve_repo_root();
/// let args = GuideArgs {
///     mode: GuideMode::Help,
///     repo_root: root,
/// };
/// assert!(execute(&args).is_ok());
/// ```
pub fn execute(args: &GuideArgs) -> Result<(), String> {
    println!("============================================================");
    println!("  Tenebris and Lux - Master Guide & Volume PDF Publisher    ");
    println!("============================================================");
    println!("Repository Root: {:?}", args.repo_root);

    let builder = MasterGuideBuilder::new(&args.repo_root);

    match &args.mode {
        GuideMode::All => {
            run_master_build(&builder)?;
            run_volumes_build(&builder)?;
        }
        GuideMode::MasterOnly => run_master_build(&builder)?,
        GuideMode::VolumesOnly => run_volumes_build(&builder)?,
        GuideMode::SingleVolume(id) => run_single_volume_build(&builder, id)?,
        GuideMode::SnapshotsOnly => run_snapshots_build(&builder)?,
        GuideMode::ScanOnly => run_scan(&builder),
        GuideMode::ValidateLayout => run_layout_validation(&builder)?,
        GuideMode::Help => print_usage(),
    }

    println!("============================================================");
    Ok(())
}

/// Standalone CLI bridge used by the `master_guide` binary executable.
pub fn dispatch_standalone_guide(cli_args: &[String], repo_root: &Path) -> Result<(), String> {
    let mut mode = GuideMode::All;
    let mut i = 1;
    while i < cli_args.len() {
        match cli_args[i].as_str() {
            "--all" => mode = GuideMode::All,
            "--master" => mode = GuideMode::MasterOnly,
            "--volumes" => mode = GuideMode::VolumesOnly,
            "--volume" => {
                if i + 1 < cli_args.len() {
                    mode = GuideMode::SingleVolume(cli_args[i + 1].clone());
                    i += 1;
                } else {
                    return Err(
                        "Error: --volume requires volume identifier (e.g. 'I' or 'VI')".to_string()
                    );
                }
            }
            "--snapshots" => mode = GuideMode::SnapshotsOnly,
            "--validate-layout" => mode = GuideMode::ValidateLayout,
            "--scan" => mode = GuideMode::ScanOnly,
            "--help" | "-h" => {
                mode = GuideMode::Help;
                break;
            }
            arg => {
                return Err(format!("Unknown option: {}", arg));
            }
        }
        i += 1;
    }

    let args = GuideArgs {
        mode,
        repo_root: repo_root.to_path_buf(),
    };
    execute(&args)
}

/// Prints command-line usage instructions.
pub fn print_usage() {
    println!("Usage: master_guide [OPTIONS]");
    println!();
    println!("Options:");
    println!("  --all             Build unified compendium AND all volume books (default)");
    println!("  --master          Build unified master compendium only (MASTER_GUIDE.*)");
    println!("  --volumes         Build individual volume books only in docs/volumes/");
    println!("  --volume <ROMAN>  Build a specific volume (e.g. --volume I, --volume VI)");
    println!("  --snapshots       Regenerate code snapshot plates & PDF theme YAML only");
    println!("  --validate-layout Audit .adoc files & snapshots against single-page PDF layout rules");
    println!("  --scan            Scan and print all chapters across documentation areas");
    println!("  --help, -h        Print this help menu");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Diagnostic Self-Audits, Invariant Probes & Four-Section Unit Tests )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Diagnostic Self-Audit Target
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying suite constant integrity.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::guide::self_audit;
///
/// assert!(self_audit());
/// ```
pub fn self_audit() -> bool {
    !DOCS_DIR_NAME.is_empty()
        && !BOOKS_DIR_NAME.is_empty()
        && !VOLUMES_DIR_NAME.is_empty()
        && !SNAPSHOTS_DIR_NAME.is_empty()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // --- Section 1: Always Pass (Nominal Defaults) ---
    #[test]
    fn test_section_1_always_pass_guide_defaults() {
        let args = GuideArgs::default();
        assert_eq!(args.mode, GuideMode::All);
        assert_eq!(args.repo_root, PathBuf::from("."));
    }

    // --- Section 2: Never Fail (Preflight Diagnostic Invariant) ---
    #[test]
    fn test_section_2_never_fail_guide_self_audit() {
        assert!(self_audit(), "guide self_audit() contract must pass");
    }

    // --- Section 3: Invariants, Equality & Mode Contracts ---
    #[test]
    fn test_section_3_invariants_guide_modes() {
        assert_eq!(GuideMode::All, GuideMode::All);
        assert_ne!(GuideMode::All, GuideMode::MasterOnly);
        assert_ne!(GuideMode::VolumesOnly, GuideMode::SnapshotsOnly);
        assert_eq!(
            GuideMode::SingleVolume("I".into()),
            GuideMode::SingleVolume("I".into())
        );
    }

    // --- Section 4: Emergence & Mode Customization ---
    #[test]
    fn test_section_4_emergence_custom_volume_target() {
        let args = GuideArgs {
            mode: GuideMode::SingleVolume("VI".to_string()),
            repo_root: PathBuf::from("/custom/path"),
        };
        match args.mode {
            GuideMode::SingleVolume(id) => assert_eq!(id, "VI"),
            _ => panic!("Expected SingleVolume variant"),
        }
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Lifecycle Status, Module Epilogue & Handshake Predicates )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Documentation Workspace Handshake
// -----------------------------------------------------------------------------

/// Verifies that a repository root directory contains the documentation tree.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::guide::has_valid_docs_hierarchy;
/// use tl_tools::cmd::resolve_repo_root;
///
/// let root = resolve_repo_root();
/// assert!(has_valid_docs_hierarchy(&root));
/// ```
pub fn has_valid_docs_hierarchy(root: &Path) -> bool {
    root.join(DOCS_DIR_NAME).is_dir()
        && root.join(DOCS_DIR_NAME).join("master_guide_manifest.json").is_file()
}

/// Epilogue lifecycle readiness probe for the master guide command sub-assembly.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::guide::is_guide_cmd_ready;
///
/// assert!(is_guide_cmd_ready());
/// ```
pub fn is_guide_cmd_ready() -> bool {
    self_audit()
}

