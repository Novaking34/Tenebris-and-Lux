// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/cmd/extract.rs
// @id            cog.tools.cmd.extract
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Archival Extraction Sub-Assembly
// @role          Extracts & Reconciles Historical RPG Maker MV Archival Datasets
// @maturity      Rev 5 Baseline | Immutable Archival Ingestion Pipeline
// @chassis       bounded-heap | Deserialization memory allocations; bounded
// @scale         O(E) across archival entity catalogs (actors, skills, items)
// @seams         in: external archive / docs/baseline | uses: extractor/
// @invariants    1. Strict exit code semantics (0 = success, 1 = failure).
//                2. Zero mutation of source archival JSON datasets.
//                3. Universal 5-Block Lifecycle Protocol enforced.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Defines the archival extraction command handler for the OmniCode developer
//   toolchain (`tl_tools`). Ingests historical 2017–2018 RPG Maker MV demo
//   datasets, extracting damage formulas, skill trees, enemy parameters, and
//   actor records into canonical markdown baselines under `docs/baseline/`.
//
//   ### For Game Designers & Non-Programmers:
//   Safeguards the historical RPG lore and mathematical battle balance from
//   the original game demo. Extracts every weapon, armor, skill, and formula
//   into human-readable markdown so designers can consult original balance.
//
//   ### For Systems Programmers & Engine Architects:
//   - Immutability: Reads source archive datasets strictly in read-only mode.
//   - Deserialization Purity: Handles 1-based arrays and null entries from
//     legacy RPG Maker MV engines without crashing or leaking memory.
// =============================================================================

//! # Archival Demo Extraction Command Handler
//!
//! Reconciles historical 2017-2018 demo datasets into canonical engine baselines.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::{Path, PathBuf};
use crate::extractor::DemoExtractor;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Constants & Target Directories
// -----------------------------------------------------------------------------

/// Canonical path to external raw demo archive.
pub const DEFAULT_ARCHIVE_DIR: &str =
    "/home/professorseanex/Tenebris_Lux_Old_Game_Data_Archive";

/// Canonical path to generated baseline output directory.
pub const DEFAULT_BASELINE_DIR: &str = "docs/baseline";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types & Options
// -----------------------------------------------------------------------------

/// Execution parameters and runtime flags for the extraction command.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::extract::ExtractArgs;
/// use std::path::PathBuf;
///
/// let args = ExtractArgs {
///     archive_dir: PathBuf::from("/custom/archive"),
///     output_dir: PathBuf::from("docs/baseline"),
///     verbose: true,
/// };
/// assert!(args.verbose);
/// assert_eq!(args.output_dir, PathBuf::from("docs/baseline"));
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ExtractArgs {
    /// Path to external raw historical RPG Maker archive.
    pub archive_dir: PathBuf,
    /// Target output directory for extracted baseline documents.
    pub output_dir: PathBuf,
    /// Emit detailed diagnostic trace output when true.
    pub verbose: bool,
}

impl Default for ExtractArgs {
    /// Creates default extraction parameters targeting the canonical archive.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use tl_tools::cmd::extract::ExtractArgs;
    ///
    /// let args = ExtractArgs::default();
    /// assert_eq!(args.verbose, false);
    /// ```
    fn default() -> Self {
        Self {
            archive_dir: PathBuf::from(DEFAULT_ARCHIVE_DIR),
            output_dir: PathBuf::from(DEFAULT_BASELINE_DIR),
            verbose: false,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / EXTRACTION EXECUTION
// ( Operational Pipelines, Engine Dispatch & Standalone CLI Bridges )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Archival Reconciliation Pipeline
// -----------------------------------------------------------------------------

/// Executes the archival extraction workflow.
///
/// Reads raw JSON files from `archive_dir` and compiles structured markdown
/// baselines into `output_dir`.
///
/// # Errors
///
/// Returns `Err(String)` if the archive directory does not exist or if required
/// historical files (e.g. `Actors.json`, `Skills.json`) cannot be parsed.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::extract::{execute, ExtractArgs};
/// use std::path::PathBuf;
///
/// let args = ExtractArgs::default();
/// // If archive is present, execute succeeds:
/// if args.archive_dir.is_dir() {
///     assert!(execute(&args).is_ok());
/// }
/// ```
pub fn execute(args: &ExtractArgs) -> Result<(), String> {
    if !has_valid_archive_source(&args.archive_dir) {
        return Err(format!(
            "Archive directory {:?} does not contain valid historical demo datasets",
            args.archive_dir
        ));
    }

    println!("============================================================");
    println!("     Tenebris and Lux - Archival Demo Extractor             ");
    println!("============================================================");
    println!("Archive Base: {:?}", args.archive_dir);
    println!("Output Dir:   {:?}", args.output_dir);

    let extractor = DemoExtractor::new(&args.archive_dir, &args.output_dir);
    let summary = extractor.run()?;

    println!("------------------------------------------------------------");
    println!("Extraction Summary:");
    println!("  Actors Extracted:      {}", summary.actor_count);
    println!("  Classes Extracted:     {}", summary.class_count);
    println!("  Skills Extracted:      {}", summary.skill_count);
    println!("  Items Extracted:       {}", summary.item_count);
    println!("  Weapons Extracted:     {}", summary.weapon_count);
    println!("  Armor Extracted:       {}", summary.armor_count);
    println!("  Enemies Extracted:     {}", summary.enemy_count);
    println!("  Troops Extracted:      {}", summary.troop_count);
    println!("============================================================");

    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Standalone CLI Bridge
// -----------------------------------------------------------------------------

/// Standalone CLI bridge used by the `demo_extractor` binary executable.
pub fn dispatch_standalone_extractor(cli_args: &[String], repo_root: &Path) -> Result<(), String> {
    let mut args = ExtractArgs {
        output_dir: repo_root.join(DEFAULT_BASELINE_DIR),
        ..Default::default()
    };

    let mut i = 1;
    while i < cli_args.len() {
        match cli_args[i].as_str() {
            "--archive" | "-a" => {
                if i + 1 < cli_args.len() {
                    args.archive_dir = PathBuf::from(&cli_args[i + 1]);
                    i += 1;
                }
            }
            "--output" | "-o" => {
                if i + 1 < cli_args.len() {
                    args.output_dir = PathBuf::from(&cli_args[i + 1]);
                    i += 1;
                }
            }
            "--verbose" | "-v" => args.verbose = true,
            "--help" | "-h" => {
                println!("Usage: demo_extractor [OPTIONS]");
                println!("  -a, --archive <dir>   Path to external raw demo archive");
                println!("  -o, --output <dir>    Path to generated output baseline directory");
                println!("  -v, --verbose         Enable diagnostic trace output");
                println!("  -h, --help            Display this help manual");
                return Ok(());
            }
            _ => {}
        }
        i += 1;
    }

    execute(&args)
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
/// use tl_tools::cmd::extract::self_audit;
///
/// assert!(self_audit());
/// ```
pub fn self_audit() -> bool {
    !DEFAULT_ARCHIVE_DIR.is_empty() && !DEFAULT_BASELINE_DIR.is_empty()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // --- Section 1: Always Pass (Nominal Defaults) ---
    #[test]
    fn test_section_1_always_pass_extract_defaults() {
        let args = ExtractArgs::default();
        assert_eq!(args.verbose, false);
        assert_eq!(args.output_dir, PathBuf::from(DEFAULT_BASELINE_DIR));
        assert_eq!(args.archive_dir, PathBuf::from(DEFAULT_ARCHIVE_DIR));
    }

    // --- Section 2: Never Fail (Preflight Diagnostic Invariant) ---
    #[test]
    fn test_section_2_never_fail_extract_self_audit() {
        assert!(self_audit(), "extract self_audit() contract must pass");
    }

    // --- Section 3: Invariants, Equality & Bounds ---
    #[test]
    fn test_section_3_invariants_args_equality() {
        let a1 = ExtractArgs::default();
        let a2 = ExtractArgs::default();
        assert_eq!(a1, a2);

        let mut a3 = ExtractArgs::default();
        a3.verbose = true;
        assert_ne!(a1, a3);
    }

    // --- Section 4: Emergence & Path Customization ---
    #[test]
    fn test_section_4_emergence_custom_archive_path() {
        let args = ExtractArgs {
            archive_dir: PathBuf::from("/custom/historical_archive"),
            output_dir: PathBuf::from("/custom/output"),
            verbose: true,
        };
        assert!(args.verbose);
        assert_eq!(args.archive_dir, PathBuf::from("/custom/historical_archive"));
        assert_eq!(args.output_dir, PathBuf::from("/custom/output"));
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Lifecycle Status, Module Epilogue & Handshake Predicates )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Archive Source Predicate
// -----------------------------------------------------------------------------

/// Verifies whether an external directory possesses valid historical archive files.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::extract::has_valid_archive_source;
/// use std::path::Path;
///
/// // Evaluates safely on any path:
/// let _ = has_valid_archive_source(Path::new("/tmp"));
/// ```
pub fn has_valid_archive_source(archive_dir: &Path) -> bool {
    archive_dir.is_dir()
        && (archive_dir.join("Old Game Data").is_dir()
            || archive_dir.join("Old Game Data/System.json").is_file()
            || archive_dir.join("System.json").is_file()
            || archive_dir.join("Actors.json").is_file())
}

/// Epilogue lifecycle readiness probe for the extract command sub-assembly.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::extract::is_extract_cmd_ready;
///
/// assert!(is_extract_cmd_ready());
/// ```
pub fn is_extract_cmd_ready() -> bool {
    self_audit()
}

