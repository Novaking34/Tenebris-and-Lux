// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/cmd/mod.rs
// @id            cog.tools.cmd.root
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     facade_umbrella | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Command Router & Dispatcher
// @role          Central CLI Command Router, Option Parser & Pipeline Dispatcher
// @maturity      Rev 5 Baseline | In-process command router & dispatcher
// @chassis       bounded-heap | Developer tooling memory chassis; bounded stacks
// @scale         O(1) command lookup; linear parse over small CLI arg vector
// @seams         in: main.rs | uses: cmd::{assets, audit, extract, guide, validate}
// @invariants    1. Strict exit code semantics (0 = success, 1 = failure).
//                2. Deterministic repository root resolution.
//                3. Zero process spawning for in-process command execution.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Acts as the central command router, argument parser, and execution
//   orchestrator for the OmniCode developer toolchain (`tl_tools`). Dispatches
//   parsed options to domain-specific command sub-assemblies.
//
//   ### For Game Designers & Non-Programmers:
//   The central entry door when running `tl_tools` from the terminal. Whether
//   compiling the Master Guide book, auditing presentation assets, or checking
//   game balance formulas, this router translates human arguments into the
//   proper internal tool module and prints clean error messages.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: CLI parsing operates on borrowed string slices;
//     dispatches purely in-process without shell spawning or dynamic library loads.
//   - Extension Seam: New subcommands declare a new `ToolAction` variant and
//     delegate to an isolated sub-assembly module under `cmd/`.
//   - Explicit Non-Goal: Does not maintain persistent state between runs; all
//     configuration is reconstructed deterministically from arguments.
// =============================================================================

//! # Developer Toolchain Command Sub-Assemblies
//!
//! Provides modular command dispatchers across validation, documentation,
//! asset pipelines, archival extraction, and environment auditing.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

pub mod assets;
pub mod audit;
pub mod extract;
pub mod guide;
pub mod validate;

use std::env;
use std::path::{Path, PathBuf};
use crate::runner::RunnerContext;
use crate::{create_default_runner, TOOL_STANDARD, TOOL_VERSION};

pub use assets::{AssetArgs, AssetMode};
pub use audit::AuditArgs;
pub use extract::ExtractArgs;
pub use guide::{GuideArgs, GuideMode};
pub use validate::{ValidateArgs, ValidationDomain};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Constants & Target Defaults
// -----------------------------------------------------------------------------

/// Canonical path to external historical demo archive.
pub const CANONICAL_ARCHIVE_DIR: &str =
    "/home/professorseanex/Tenebris_Lux_Old_Game_Data_Archive";

/// Canonical baseline output subdirectory relative to repo root.
pub const BASELINE_OUTPUT_SUBDIR: &str = "docs/baseline";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & CLI Config
// -----------------------------------------------------------------------------

/// Subcommand action requested by the operator or CI pipeline.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::ToolAction;
///
/// let action = ToolAction::Help;
/// assert_eq!(action, ToolAction::Help);
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ToolAction {
    /// Extract historical RPG Maker MV demo data.
    Extract(ExtractArgs),
    /// Compile the unified Master Guide (.adoc, .md, .pdf).
    Guide(GuideArgs),
    /// Audit the game design curation specification document.
    Curate,
    /// Validate C11 headers, JSON data, or git commit ledger.
    Validate(ValidateArgs),
    /// Audit physical on-disk visual and audio assets against catalogs.
    Assets(AssetArgs),
    /// Run diagnostic environment topology audits.
    Audit(AuditArgs),
    /// Run the entire toolchain pipeline sequentially.
    All,
    /// Print user manual and usage syntax.
    Help,
    /// Print version, provenance, and architectural standards.
    Version,
}

/// Validated CLI execution configuration.
///
/// # Examples
///
/// ```rust
/// use std::path::PathBuf;
/// use tl_tools::cmd::{CliConfig, ToolAction};
///
/// let config = CliConfig {
///     action: ToolAction::Help,
///     repo_root: PathBuf::from("."),
///     archive_dir: PathBuf::from("."),
///     verbose: false,
/// };
/// assert_eq!(config.action, ToolAction::Help);
/// ```
#[derive(Debug, Clone)]
pub struct CliConfig {
    /// Action selector indicating which runner pipeline to trigger.
    pub action: ToolAction,
    /// Resolved absolute path to repository root.
    pub repo_root: PathBuf,
    /// Resolved path to external historical demo archive.
    pub archive_dir: PathBuf,
    /// Verbose diagnostic output flag.
    pub verbose: bool,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Operational Helpers & Workspace Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a candidate path is a valid repository root.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::cmd::{is_valid_repo_root, resolve_repo_root};
///
/// let root = resolve_repo_root();
/// assert!(is_valid_repo_root(&root));
/// assert!(!is_valid_repo_root(Path::new("/nonexistent/workspace")));
/// ```
pub fn is_valid_repo_root(candidate: &Path) -> bool {
    candidate.is_dir()
        && candidate.join("CMakeLists.txt").is_file()
        && candidate.join("docs").is_dir()
}

/// Resolves repository root directory by walking up from current working directory.
///
/// Falls back to canonical project root `/home/professorseanex/Tenebris and Lux`.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::resolve_repo_root;
///
/// let root = resolve_repo_root();
/// assert!(root.is_dir());
/// ```
pub fn resolve_repo_root() -> PathBuf {
    let current = env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
    if is_valid_repo_root(&current) {
        return current;
    }
    if let Some(parent) = current.parent() {
        if is_valid_repo_root(parent) {
            return parent.to_path_buf();
        }
    }
    PathBuf::from("/home/professorseanex/Tenebris and Lux")
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Primary Execution Engine, Command Handlers & CLI Dispatchers )
// =============================================================================

impl CliConfig {
    /// Dispatches execution to the corresponding command sub-assembly.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use std::path::PathBuf;
    /// use tl_tools::cmd::{CliConfig, ToolAction};
    ///
    /// let config = CliConfig {
    ///     action: ToolAction::Help,
    ///     repo_root: PathBuf::from("."),
    ///     archive_dir: PathBuf::from("."),
    ///     verbose: false,
    /// };
    /// assert!(config.execute().is_ok());
    /// ```
    pub fn execute(&self) -> Result<(), String> {
        match &self.action {
            ToolAction::Help => {
                Self::print_help();
                Ok(())
            }
            ToolAction::Version => {
                Self::print_version();
                Ok(())
            }
            ToolAction::Validate(args) => validate::execute(args),
            ToolAction::Guide(args) => guide::execute(args),
            ToolAction::Extract(args) => extract::execute(args),
            ToolAction::Assets(args) => assets::execute(args),
            ToolAction::Audit(args) => audit::execute(args),
            ToolAction::Curate => {
                let mut context = RunnerContext::new(&self.repo_root, &self.archive_dir);
                context.verbose = self.verbose;
                let engine = create_default_runner(context);
                let tel = engine.run_task("curation_audit")?;
                println!("  [SUMMARY] {}", tel.details);
                Ok(())
            }
            ToolAction::All => {
                let mut context = RunnerContext::new(&self.repo_root, &self.archive_dir);
                context.verbose = self.verbose;
                let engine = create_default_runner(context);
                let report = engine.run_pipeline()?;
                if report.is_success() {
                    Ok(())
                } else {
                    Err("Pipeline finished with errors.".to_string())
                }
            }
        }
    }

    /// Prints standard command-line help manual.
    pub fn print_help() {
        println!("============================================================");
        println!("        Tenebris and Lux Developer Toolchain (tl_tools)     ");
        println!("============================================================");
        println!("Usage: tl_tools [COMMAND] [OPTIONS]\n");
        println!("Commands:");
        println!("  extract   Extract raw RPG Maker demo data into docs/baseline");
        println!("  guide     Compile unified MASTER_GUIDE (.adoc, .md, .pdf)");
        println!("  curate    Audit game design curation specification");
        println!("  validate [headers|data|commits|all] Audit C11 headers, JSON, or git ledger");
        println!("  assets    Audit on-disk presentation assets (.png, .wav, .ogg)");
        println!("  all       Run full sequential pipeline (Audit -> Curate -> Extract -> Guide)");
        println!("  audit     Run self-diagnostic environment & toolchain audit");
        println!("  version   Print version, author, and architectural provenance");
        println!("  help      Print this usage manual\n");
        println!("Options:");
        println!("  -v, --verbose   Enable verbose diagnostic trace output");
        println!("  -h, --help      Display help manual");
        println!("============================================================");
    }

    /// Prints version, provenance, and standard architecture information.
    pub fn print_version() {
        println!("tl_tools v{} (Tenebris and Lux Developer Toolchain)", TOOL_VERSION);
        println!("Standard: {}", TOOL_STANDARD);
        println!("Author:   CreativeWorkzStudio LLC / ProfessorSeanEX");
        println!("Repo:     https://github.com/Novaking34/tenebris-lux");
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: CLI Argument Parsing
// -----------------------------------------------------------------------------

/// Parses command-line arguments into a validated `CliConfig`.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::{parse_cli_args, ToolAction};
///
/// let args = vec!["tl_tools".to_string(), "version".to_string()];
/// let config = parse_cli_args(&args);
/// assert_eq!(config.action, ToolAction::Version);
/// ```
pub fn parse_cli_args(args: &[String]) -> CliConfig {
    let repo_root = resolve_repo_root();
    let archive_dir = PathBuf::from(CANONICAL_ARCHIVE_DIR);
    let verbose = args.iter().any(|a| a == "--verbose" || a == "-v");

    let action = match args.get(1).map(|s| s.as_str()) {
        Some("extract") => ToolAction::Extract(ExtractArgs {
            archive_dir: archive_dir.clone(),
            output_dir: repo_root.join(BASELINE_OUTPUT_SUBDIR),
            verbose,
        }),
        Some("guide") => {
            let mode = match args.get(2).map(|s| s.as_str()) {
                Some("--master") => GuideMode::MasterOnly,
                Some("--volumes") => GuideMode::VolumesOnly,
                Some("--snapshots") => GuideMode::SnapshotsOnly,
                Some("--validate-layout") => GuideMode::ValidateLayout,
                Some("--scan") => GuideMode::ScanOnly,
                _ => GuideMode::All,
            };
            ToolAction::Guide(GuideArgs {
                mode,
                repo_root: repo_root.clone(),
            })
        }
        Some("curate") => ToolAction::Curate,
        Some("validate") => {
            let mut domain = ValidationDomain::Headers;
            let mut data_subdomain = "all".to_string();
            let mut target_paths = Vec::new();

            let mut idx = 2;
            while idx < args.len() {
                let arg = &args[idx];
                if arg == "--strict" || arg == "-s" || arg == "--verbose" || arg == "-v" || arg == "--json" || arg == "-j" {
                    idx += 1;
                    continue;
                }
                match arg.as_str() {
                    "headers" | "header" | "h" => domain = ValidationDomain::Headers,
                    "data" | "json" | "d" => domain = ValidationDomain::Data,
                    "commits" | "commit" | "git" | "c" => domain = ValidationDomain::Commits,
                    "rust" | "rs" | "r" => domain = ValidationDomain::Rust,
                    "all" | "a" => domain = ValidationDomain::All,
                    other => {
                        if domain == ValidationDomain::Rust
                            || other.ends_with(".rs")
                            || other.contains("src/")
                            || other.contains("tests/")
                        {
                            domain = ValidationDomain::Rust;
                            target_paths.push(PathBuf::from(other));
                        } else {
                            data_subdomain = other.to_string();
                            domain = ValidationDomain::Data;
                        }
                    }
                }
                idx += 1;
            }

            let strict = args.iter().any(|a| a == "--strict" || a == "-s");
            let json_output = args.iter().any(|a| a == "--json" || a == "-j");
            ToolAction::Validate(ValidateArgs {
                domain,
                data_subdomain,
                target_paths,
                repo_root: repo_root.clone(),
                strict,
                verbose,
                json_output,
            })
        }
        Some("assets") => {
            let mode = match args.get(2).map(|s| s.as_str()) {
                Some("generate") | Some("gen") => AssetMode::Generate,
                _ => AssetMode::Audit,
            };
            ToolAction::Assets(AssetArgs {
                mode,
                repo_root: repo_root.clone(),
                verbose,
            })
        }
        Some("all") => ToolAction::All,
        Some("audit") | Some("--audit") => ToolAction::Audit(AuditArgs {
            repo_root: repo_root.clone(),
            archive_dir: archive_dir.clone(),
            verbose,
        }),
        Some("version") | Some("--version") | Some("-V") => ToolAction::Version,
        Some("help") | Some("--help") | Some("-h") => ToolAction::Help,
        Some(_) | None => ToolAction::Help,
    };

    CliConfig {
        action,
        repo_root,
        archive_dir,
        verbose,
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Central Pipeline Dispatcher
// -----------------------------------------------------------------------------

/// Executes the developer toolchain CLI from raw argument vectors.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::run_toolchain;
///
/// let args = vec!["tl_tools".to_string(), "version".to_string()];
/// assert!(run_toolchain(&args).is_ok());
/// ```
pub fn run_toolchain(args: &[String]) -> Result<(), String> {
    let config = parse_cli_args(args);
    config.execute()
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
    fn test_section_1_nominal_parse_help() {
        let args = vec!["tl_tools".to_string(), "--help".to_string()];
        let cfg = parse_cli_args(&args);
        assert_eq!(cfg.action, ToolAction::Help);
    }

    #[test]
    fn test_section_1_nominal_parse_version() {
        let args = vec!["tl_tools".to_string(), "version".to_string()];
        let cfg = parse_cli_args(&args);
        assert_eq!(cfg.action, ToolAction::Version);
    }

    #[test]
    fn test_section_1_nominal_parse_validate_data() {
        let args = vec![
            "tl_tools".to_string(),
            "validate".to_string(),
            "data".to_string(),
        ];
        let cfg = parse_cli_args(&args);
        match cfg.action {
            ToolAction::Validate(v_args) => {
                assert_eq!(v_args.domain, ValidationDomain::Data);
            }
            _ => panic!("Expected Validate action"),
        }
    }

    #[test]
    fn test_section_1_nominal_parse_audit() {
        let args = vec!["tl_tools".to_string(), "audit".to_string()];
        let cfg = parse_cli_args(&args);
        match cfg.action {
            ToolAction::Audit(a_args) => {
                assert!(!a_args.verbose);
            }
            _ => panic!("Expected Audit action"),
        }
    }

    #[test]
    fn test_section_1_nominal_parse_assets() {
        let args = vec!["tl_tools".to_string(), "assets".to_string()];
        let cfg = parse_cli_args(&args);
        match cfg.action {
            ToolAction::Assets(a_args) => {
                assert_eq!(a_args.mode, AssetMode::Audit);
            }
            _ => panic!("Expected Assets action"),
        }
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_empty_args_defaults_to_help() {
        let args: Vec<String> = Vec::new();
        let cfg = parse_cli_args(&args);
        assert_eq!(cfg.action, ToolAction::Help);
    }

    #[test]
    fn test_section_2_never_fail_unknown_subcommand_defaults_to_help() {
        let args = vec!["tl_tools".to_string(), "unknown_subcommand".to_string()];
        let cfg = parse_cli_args(&args);
        assert_eq!(cfg.action, ToolAction::Help);
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_constants_non_empty() {
        assert!(!CANONICAL_ARCHIVE_DIR.is_empty());
        assert!(!BASELINE_OUTPUT_SUBDIR.is_empty());
    }

    #[test]
    fn test_section_3_invariants_repo_root_resolution() {
        let root = resolve_repo_root();
        assert!(root.is_dir());
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_run_toolchain_help_and_version() {
        let help_args = vec!["tl_tools".to_string(), "help".to_string()];
        assert!(run_toolchain(&help_args).is_ok());

        let ver_args = vec!["tl_tools".to_string(), "version".to_string()];
        assert!(run_toolchain(&ver_args).is_ok());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Performs an in-process diagnostic preflight self-audit of command router invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), String> {
    if CANONICAL_ARCHIVE_DIR.is_empty() {
        return Err("CANONICAL_ARCHIVE_DIR must not be empty".to_string());
    }
    if BASELINE_OUTPUT_SUBDIR.is_empty() {
        return Err("BASELINE_OUTPUT_SUBDIR must not be empty".to_string());
    }
    let root = resolve_repo_root();
    if !root.is_dir() {
        return Err(format!("Resolved repository root does not exist: {:?}", root));
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

/// Verifies that the command router sub-assembly is ready for command dispatch.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::is_command_router_ready;
///
/// assert!(is_command_router_ready());
/// ```
pub fn is_command_router_ready() -> bool {
    self_audit().is_ok()
}
