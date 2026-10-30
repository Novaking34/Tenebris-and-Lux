// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/main.rs
// @id            tools.cli.main
// @layer         Layer 3: Application & CLI Tooling Strata
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-10-29
// @updated       2026-10-29
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Central CLI dispatcher and pipeline runner executable for the Tenebris and
//   Lux developer toolchain (`tl_tools`). Connects command-line arguments to the
//   modular `RunnerEngine`, orchestrating historical data extraction, game
//   design curation audits, and multi-format Master Guide compilation in-process
//   with high-resolution telemetry.
//
// PEDAGOGICAL MISSION:
//   Demonstrates the Universal 5-Block Lifecycle Protocol in an executable entry
//   point. By migrating from external process shelling (`Command::new("cargo")`)
//   to direct in-memory invocation via the `TaskRunner` trait:
//     1. Eliminates process spawn overhead and path discovery fragility.
//     2. Unifies diagnostic manifests across Rust, C11, and documentation.
//     3. Prepares the toolchain to seamlessly integrate future runners
//        (C# / dotnet 10 tools, C++ simulation units, and LeTBS grid testbeds).
//
// AXIOMATIC INVARIANTS:
//   1. Strict exit code semantics (0 = success, 1 = failure).
//   2. Fail-fast preflight validation before any mutation or compilation.
//   3. Deterministic path resolution relative to repository workspace root.
//   4. Strict line discipline <= 80 columns across all comments.
// =============================================================================

//! # Developer Toolchain CLI Entry Point & Pipeline Runner
//!
//! Parses command-line arguments and dispatches tasks to the [`RunnerEngine`]
//! defined in the [`tl_tools`] library.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & CLI CONFIGURATION
// Stratum: Developer Tooling | Machine: tools.cli.main
// =============================================================================

use std::env;
use std::path::PathBuf;
use std::process::ExitCode;
use tl_tools::runner::RunnerContext;
use tl_tools::{create_default_runner, TOOL_STANDARD, TOOL_VERSION};

/// Subcommand action requested by the operator or CI pipeline.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ToolAction {
    /// Extract historical RPG Maker MV demo data.
    Extract,
    /// Compile the unified Master Guide (.adoc, .md, .pdf).
    Guide,
    /// Audit the game design curation specification document.
    Curate,
    /// Run the entire toolchain pipeline sequentially.
    All,
    /// Run diagnostic environment topology audits.
    Audit,
    /// Print user manual and usage syntax.
    Help,
    /// Print version, provenance, and architectural standards.
    Version,
}

/// Validated CLI execution configuration.
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

// =============================================================================
// BLOCK 3: BODY BLOCK / RUNNER DISPATCH LOGIC
// Stratum: Developer Tooling | Machine: tools.cli.main
// =============================================================================

impl CliConfig {
    /// Dispatches execution through the modular `RunnerEngine`.
    pub fn execute(&self) -> Result<(), String> {
        let mut context = RunnerContext::new(&self.repo_root, &self.archive_dir);
        context.verbose = self.verbose;

        let engine = create_default_runner(context);

        match self.action {
            ToolAction::Help => {
                Self::print_help();
                Ok(())
            }
            ToolAction::Version => {
                Self::print_version();
                Ok(())
            }
            ToolAction::Extract => {
                let tel = engine.run_task("demo_extractor")?;
                println!("  [SUMMARY] {}", tel.details);
                Ok(())
            }
            ToolAction::Guide => {
                let tel = engine.run_task("master_guide")?;
                println!("  [SUMMARY] {}", tel.details);
                Ok(())
            }
            ToolAction::Curate => {
                let tel = engine.run_task("curation_audit")?;
                println!("  [SUMMARY] {}", tel.details);
                Ok(())
            }
            ToolAction::Audit => {
                let tel = engine.run_task("env_audit")?;
                println!("  [SUMMARY] {}", tel.details);
                Ok(())
            }
            ToolAction::All => {
                let report = engine.run_pipeline()?;
                if report.is_success() {
                    Ok(())
                } else {
                    Err("Pipeline finished with errors.".to_string())
                }
            }
        }
    }

    /// Prints standard command-line help text.
    pub fn print_help() {
        println!("============================================================");
        println!("        Tenebris and Lux Developer Toolchain (tl_tools)     ");
        println!("============================================================");
        println!("Usage: tl_tools [COMMAND] [OPTIONS]\n");
        println!("Commands:");
        println!("  extract   Extract raw RPG Maker demo data into docs/baseline");
        println!("  guide     Compile unified MASTER_GUIDE (.adoc, .md, .pdf)");
        println!("  curate    Audit game design curation specification");
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

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / ARGUMENT PARSING
// Stratum: Developer Tooling | Machine: tools.cli.main
// =============================================================================

/// Resolves repository root directory by walking up from current executable.
fn resolve_repo_root() -> PathBuf {
    let current = env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
    if current.join("CMakeLists.txt").exists() && current.join("docs").exists() {
        return current;
    }
    if let Some(parent) = current.parent() {
        if parent.join("CMakeLists.txt").exists() && parent.join("docs").exists() {
            return parent.to_path_buf();
        }
    }
    PathBuf::from("/home/professorseanex/Tenebris and Lux")
}

/// Parses command-line arguments into a validated `CliConfig`.
pub fn parse_cli_args(args: &[String]) -> CliConfig {
    let repo_root = resolve_repo_root();
    let archive_dir = PathBuf::from(
        "/home/professorseanex/Tenebris_Lux_Old_Game_Data_Archive",
    );

    let action = match args.get(1).map(|s| s.as_str()) {
        Some("extract") => ToolAction::Extract,
        Some("guide") => ToolAction::Guide,
        Some("curate") => ToolAction::Curate,
        Some("all") => ToolAction::All,
        Some("audit") | Some("--audit") => ToolAction::Audit,
        Some("version") | Some("--version") | Some("-V") => ToolAction::Version,
        Some("help") | Some("--help") | Some("-h") => ToolAction::Help,
        Some(_) | None => ToolAction::Help,
    };

    CliConfig {
        action,
        repo_root,
        archive_dir,
        verbose: args.iter().any(|a| a == "--verbose" || a == "-v"),
    }
}

// =============================================================================
// BLOCK 5: CLOSING / CLI ENTRY BLOCK
// Stratum: Developer Tooling | Machine: tools.cli.main
// =============================================================================

fn main() -> ExitCode {
    let args: Vec<String> = env::args().collect();
    let config = parse_cli_args(&args);

    match config.execute() {
        Ok(()) => ExitCode::SUCCESS,
        Err(err) => {
            eprintln!("Error: {}", err);
            ExitCode::FAILURE
        }
    }
}
