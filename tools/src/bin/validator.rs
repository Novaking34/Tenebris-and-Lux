// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/bin/validator.rs
// @id            tools.bin.validator
// @layer         Layer 3: Systems Developer Tooling Standalone Binary
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-05
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Standalone command-line data validation binary for Tenebris and Lux.
//   Audits JSON manifests and catalogs across `game/data/` for Universal 5-Block
//   Lifecycle Protocol compliance, cross-manifest link integrity, and domain
//   invariant contracts.
//
// PEDAGOGICAL MISSION:
//   Eliminates fragile ad-hoc validation scripts by providing a native, fast,
//   zero-warning tool that can run locally during development or headlessly in
//   CI/CD pipelines to guarantee all shipped data assets are bulletproof.
//
// AXIOMATIC INVARIANTS:
//   1. Strict exit code propagation (0 = all data valid, 1 = validation errors).
//   2. Fail-fast error reporting detailing exact file, block, field, and message.
//   3. Deterministic path resolution relative to repository workspace root.
//   4. Strict line discipline <= 80 columns across all documentation.
// =============================================================================

//! # Standalone Data Validator Binary
//!
//! Audits `game/data/` for OmniCode 5-Block compliance and invariant integrity.

// =============================================================================
// BLOCK 2: OPENING BLOCK / IMPORTS & CLI CONFIGURATION
// Stratum: Developer Tooling | Machine: tools.bin.validator
// =============================================================================

use std::env;
use std::path::PathBuf;
use std::process::ExitCode;

use tl_tools::validator::{DataValidator, ValidationOptions};

/// Parsed CLI configuration for the standalone validator.
#[derive(Debug, Clone)]
struct CliArgs {
    domain: String,
    strict: bool,
    verbose: bool,
    json_output: bool,
    show_help: bool,
    repo_root: PathBuf,
}

impl CliArgs {
    fn parse() -> Self {
        let args: Vec<String> = env::args().collect();
        let mut domain = "system".to_string();
        let mut strict = false;
        let mut verbose = false;
        let mut json_output = false;
        let mut show_help = false;

        let mut i = 1;
        while i < args.len() {
            match args[i].as_str() {
                "--domain" | "-d" => {
                    if i + 1 < args.len() {
                        domain = args[i + 1].clone();
                        i += 1;
                    }
                }
                "--all" | "-a" => {
                    domain = "all".to_string();
                }
                "--strict" | "-s" => {
                    strict = true;
                }
                "--verbose" | "-v" => {
                    verbose = true;
                }
                "--json" | "-j" => {
                    json_output = true;
                }
                "--help" | "-h" => {
                    show_help = true;
                }
                other => {
                    if !other.starts_with('-') {
                        domain = other.to_string();
                    }
                }
            }
            i += 1;
        }

        let repo_root = resolve_repo_root();

        Self {
            domain,
            strict,
            verbose,
            json_output,
            show_help,
            repo_root,
        }
    }

    fn print_help() {
        println!("================================================================================");
        println!("       Tenebris and Lux - Data Catalog & Schema Validator CLI                   ");
        println!("================================================================================");
        println!("Usage: validator [OPTIONS] [DOMAIN]\n");
        println!("Arguments:");
        println!("  [DOMAIN]              Sub-domain under game/data/ to audit (default: 'system')");
        println!("                        Use 'all' to audit the entire game/data/ repository tree\n");
        println!("Options:");
        println!("  -d, --domain <name>   Explicitly specify target sub-domain (e.g. system, mechanics)");
        println!("  -a, --all             Audit all sub-domains across game/data/");
        println!("  -s, --strict          Treat warnings as hard errors (fail on any issue)");
        println!("  -v, --verbose         Print status badge for each scanned file");
        println!("  -j, --json            Output validation results as structured JSON");
        println!("  -h, --help            Print this usage manual and exit");
        println!("================================================================================");
    }
}

/// Resolves repository root directory by walking up from current working directory.
fn resolve_repo_root() -> PathBuf {
    let current = env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
    if current.join("CMakeLists.txt").exists() && current.join("game").exists() {
        return current;
    }
    if let Some(parent) = current.parent() {
        if parent.join("CMakeLists.txt").exists() && parent.join("game").exists() {
            return parent.to_path_buf();
        }
    }
    PathBuf::from("/home/professorseanex/Tenebris and Lux")
}

// =============================================================================
// BLOCK 3: BODY BLOCK / EXECUTION DISPATCH
// Stratum: Developer Tooling | Machine: tools.bin.validator
// =============================================================================

fn run_validator(args: &CliArgs) -> Result<bool, String> {
    let options = ValidationOptions {
        target_domain: args.domain.clone(),
        strict: args.strict,
        verbose: args.verbose,
        repo_root: args.repo_root.clone(),
    };

    let validator = DataValidator::new(options);
    let summary = validator.run_audit()?;

    if args.json_output {
        let json_report = serde_json::json!({
            "target_domain": args.domain,
            "total_files": summary.total_files,
            "passed_files": summary.passed_files,
            "failed_files": summary.failed_files,
            "errors": summary.error_count(),
            "warnings": summary.warning_count(),
            "duration_ms": summary.duration_ms,
            "verdict": if summary.is_success(args.strict) { "PASS" } else { "FAIL" },
            "issues": summary.issues.iter().map(|i| {
                serde_json::json!({
                    "file": i.file.to_string_lossy(),
                    "block": i.block,
                    "field": i.field,
                    "severity": format!("{}", i.severity),
                    "message": i.message
                })
            }).collect::<Vec<_>>()
        });
        println!("{}", serde_json::to_string_pretty(&json_report).unwrap_or_default());
    } else {
        summary.print_summary(&args.domain);
    }

    Ok(summary.is_success(args.strict))
}

// =============================================================================
// BLOCK 4: AUDIT & TRIPWIRE CHECKS
// Stratum: Developer Tooling | Machine: tools.bin.validator
// =============================================================================

// Invariants asserted via ExitCode semantics.

// =============================================================================
// BLOCK 5: CLOSING BLOCK / CLI ENTRY POINT
// Stratum: Developer Tooling | Machine: tools.bin.validator
// =============================================================================

fn main() -> ExitCode {
    let args = CliArgs::parse();

    if args.show_help {
        CliArgs::print_help();
        return ExitCode::SUCCESS;
    }

    match run_validator(&args) {
        Ok(true) => ExitCode::SUCCESS,
        Ok(false) => ExitCode::FAILURE,
        Err(e) => {
            eprintln!("Error executing data validator: {}", e);
            ExitCode::FAILURE
        }
    }
}

