// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/asset_generator.rs
// @id            tools.bin.asset_generator
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       0.1.0-dev
// @interface_rev Rev 4
// @date          2026-11-08
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Standalone Asset Generator & Catalog Audit Binary
// @role          Provides operator command-line interface to audit, verify,
//                and regenerate game assets across audio and visual catalogs.
// @invariants    1. Strict exit code propagation (0 = success, 1 = failure).
//                2. Deterministic path resolution relative to workspace root.
//                3. Universal 5-Block Lifecycle Protocol enforced.
// =============================================================================

//! # Standalone Asset Generator & Verification Binary

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & CLI CONFIGURATION
// Stratum: Developer Tooling | Machine: tools.bin.asset_generator
// =============================================================================

use std::env;
use std::path::PathBuf;
use std::process::ExitCode;

use tl_tools::assets::AssetAuditor;

#[derive(Debug, Clone)]
enum AssetCommand {
    Audit,
    Generate,
    Help,
}

#[derive(Debug, Clone)]
struct CliArgs {
    command: AssetCommand,
    repo_root: PathBuf,
    verbose: bool,
}

impl CliArgs {
    fn parse() -> Self {
        let args: Vec<String> = env::args().collect();
        let mut command = AssetCommand::Audit;
        let mut verbose = false;

        let cwd = env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
        let mut repo_root = cwd.clone();
        if repo_root.ends_with("tools") {
            if let Some(parent) = repo_root.parent() {
                repo_root = parent.to_path_buf();
            }
        }

        let mut i = 1;
        while i < args.len() {
            match args[i].as_str() {
                "audit" | "--audit" => command = AssetCommand::Audit,
                "generate" | "--generate" | "gen" => command = AssetCommand::Generate,
                "help" | "--help" | "-h" => command = AssetCommand::Help,
                "-v" | "--verbose" => verbose = true,
                "--root" => {
                    if i + 1 < args.len() {
                        repo_root = PathBuf::from(&args[i + 1]);
                        i += 1;
                    }
                }
                _ => {}
            }
            i += 1;
        }

        Self {
            command,
            repo_root,
            verbose,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / EXECUTION DISPATCH
// Stratum: Developer Tooling | Machine: tools.bin.asset_generator
// =============================================================================

fn print_help() {
    println!("============================================================");
    println!("    Tenebris and Lux - Asset Pipeline Tool (asset_generator) ");
    println!("============================================================");
    println!("Usage: asset_generator [COMMAND] [OPTIONS]\n");
    println!("Commands:");
    println!("  audit       Audit all on-disk visual (.png) & audio (.wav/.ogg) assets");
    println!("  generate    Trigger procedural asset synthesizer to produce all assets");
    println!("  help        Display this help message\n");
    println!("Options:");
    println!("  -v, --verbose    Show detailed per-asset metadata and inspection");
    println!("  --root <PATH>    Override repository workspace root directory");
    println!("============================================================");
}

fn main() -> ExitCode {
    let args = CliArgs::parse();

    match args.command {
        AssetCommand::Help => {
            print_help();
            ExitCode::SUCCESS
        }
        AssetCommand::Generate => {
            println!("============================================================");
            println!("  [PIPELINE] Launching Procedural Asset Synthesizer...");
            println!("============================================================");
            let auditor = AssetAuditor::new(&args.repo_root);
            match auditor.generate_assets() {
                Ok(stdout) => {
                    println!("{}", stdout);
                    println!("  [SUCCESS] Asset synthesis completed successfully.");
                    ExitCode::SUCCESS
                }
                Err(e) => {
                    eprintln!("  [ERROR] Asset synthesis failed: {}", e);
                    ExitCode::FAILURE
                }
            }
        }
        AssetCommand::Audit => {
            println!("============================================================");
            println!("  [AUDIT] Verifying On-Disk Presentation Assets...");
            println!("  Workspace Root: {}", args.repo_root.display());
            println!("============================================================");

            let auditor = AssetAuditor::new(&args.repo_root);
            let report = auditor.audit_all();

            for f in &report.findings {
                if let Some(err) = &f.error {
                    eprintln!("  [FAIL] {} => {}", f.rel_path, err);
                } else if args.verbose {
                    println!("  [PASS] {} => {}", f.rel_path, f.details);
                }
            }

            println!("------------------------------------------------------------");
            println!(
                "  Result: {}/{} assets valid on disk ({} failed)",
                report.total_passed, report.total_checked, report.total_failed
            );
            println!("============================================================");

            if report.is_success() {
                println!("  [PASS] All visual and audio assets conform to contracts.");
                ExitCode::SUCCESS
            } else {
                eprintln!("  [FAIL] One or more assets failed verification.");
                ExitCode::FAILURE
            }
        }
    }
}

