// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/cmd/validate.rs
// @id            cog.tools.cmd.validate
// @layer         Layer 3: Developer Toolchain Command Sub-Assemblies
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Validation Command Sub-Assembly
// @role          Dispatches and Executes Unified Audits Across Code & Data
// @maturity      Rev 5 Baseline | In-process command dispatcher & audit harness
// @chassis       bounded-heap | Developer tooling memory chassis; bounded stacks
// @scale         O(N) file sweeps; scales with codebase and data catalog size
// @seams         in: engine/include/, game/data/ | uses: validator/ | probes: exit
// @invariants    1. Strict exit code semantics (0 = success, 1 = failure).
//                2. Fail-fast error reporting with file, block, and line precision.
//                3. Zero process spawning for in-process validation sweeps.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Defines the central validation command handler for the OmniCode developer
//   toolchain (`tl_tools`). Unifies static audits of C11/C23 engine headers,
//   5-block JSON catalogs/manifests, and git commit/tag engineering ledgers.
//
//   ### For Game Designers & Non-Programmers:
//   Acts as the automated immune system for data and lore files. Whenever a
//   designer edits an item, enemy, or formulas JSON catalog, this tool verifies
//   that all keys, references, and mathematical formulas match engine schemas
//   before the game binary ever touches them, preventing runtime crashes.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: All validation routines are purely read-only and
//     reentrant. Config structs are passed by reference with zero heap leaks.
//   - Extension Seam: Additional validation targets (e.g. shader sources,
//     audio profiles, ROM tables) register as new `ValidationDomain` variants.
//   - Explicit Non-Goal: Does not modify or auto-format source files on disk;
//     enforces strict observational purity without silent mutations.
// =============================================================================

//! # Validation Command Handler
//!
//! Provides unified execution for C11/C23 engine headers, 5-block JSON catalogs,
//! and git commit/tag ledger verifications.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::fs;
use std::path::{Path, PathBuf};
use std::time::Instant;
use crate::validator::{
    audit_commit_message, audit_git_history, audit_git_tags,
    load_header_rules_config, load_subsystems_registry,
    validate_rust_content, DataValidator, GitAuditReport,
    HeaderValidator, IssueSeverity, RustValidationConfig,
    ValidationIssue, ValidationOptions, ValidationSummary,
    ValidatorMode,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Constants & Target Domain Registry
// -----------------------------------------------------------------------------

/// Canonical path to C11 engine headers relative to repository root.
pub const ENGINE_INCLUDE_DIR: &str = "engine/include";

/// Canonical path to domain data catalogs relative to repository root.
pub const GAME_DATA_DIR: &str = "game/data";

/// Default git commit history inspection depth for ledger verification.
pub const DEFAULT_GIT_AUDIT_DEPTH: usize = 10;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Domain Selectors
// -----------------------------------------------------------------------------

/// Target domain selector for validation operations.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::validate::ValidationDomain;
///
/// let domain = ValidationDomain::Headers;
/// assert_eq!(domain, ValidationDomain::Headers);
/// assert_ne!(domain, ValidationDomain::Data);
/// ```
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ValidationDomain {
    /// C11/C23 engine headers under `engine/include/` (5-Block, Invariant 18).
    Headers,
    /// 5-block JSON manifests and catalogs under `game/data/`.
    Data,
    /// Conventional commit messages and milestone release tags in git history.
    Commits,
    /// Rust 2024 source files under `tools/` (5-Block, self-audit, readiness).
    Rust,
    /// Composite sweep executing all domain validators sequentially.
    All,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Validated Configuration Structs & Defaults
// -----------------------------------------------------------------------------

/// Execution parameters and runtime flags for the validation command.
///
/// Encapsulates the target domain, path resolution, severity thresholds,
/// and output formatting preferences for the audit suite.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::validate::{ValidateArgs, ValidationDomain};
/// use std::path::PathBuf;
///
/// let args = ValidateArgs {
///     domain: ValidationDomain::Data,
///     data_subdomain: "mechanics".to_string(),
///     target_paths: Vec::new(),
///     repo_root: PathBuf::from("."),
///     strict: true,
///     verbose: false,
///     json_output: false,
/// };
/// assert_eq!(args.domain, ValidationDomain::Data);
/// assert!(args.strict);
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ValidateArgs {
    /// Selected validation domain target.
    pub domain: ValidationDomain,
    /// Target sub-domain under `game/data/` (e.g. "system", "mechanics", "all").
    pub data_subdomain: String,
    /// Explicit target files or directories for localized auditing.
    pub target_paths: Vec<PathBuf>,
    /// Absolute or relative path to workspace repository root.
    pub repo_root: PathBuf,
    /// Treat warnings as hard fatal errors when true.
    pub strict: bool,
    /// Emit detailed diagnostic per-file telemetry when true.
    pub verbose: bool,
    /// Serialize audit findings to machine-readable JSON on stdout.
    pub json_output: bool,
}

impl Default for ValidateArgs {
    /// Creates default validation parameters targeting headers with standard rules.
    ///
    /// # Examples
    ///
    /// ```rust
    /// use tl_tools::cmd::validate::{ValidateArgs, ValidationDomain};
    ///
    /// let args = ValidateArgs::default();
    /// assert_eq!(args.domain, ValidationDomain::Headers);
    /// assert_eq!(args.strict, false);
    /// assert_eq!(args.verbose, false);
    /// ```
    fn default() -> Self {
        Self {
            domain: ValidationDomain::Headers,
            data_subdomain: "system".to_string(),
            target_paths: Vec::new(),
            repo_root: PathBuf::from("."),
            strict: false,
            verbose: false,
            json_output: false,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK / VALIDATION EXECUTION
// ( Operational Pipelines, Engine Dispatch & Standalone CLI Bridges )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: C11/C23 Engine Header Validation Pipeline
// -----------------------------------------------------------------------------

/// Executes header validation against all public C cogs under `engine/include/`.
///
/// Audits files for Universal 5-Block structure, Invariant 18 (strict C comments),
/// sub-assembly numbering, include ordering, C++ compatibility guards, and
/// prototype documentation parity.
///
/// # Errors
///
/// Returns `Err(String)` if any header violates mandatory invariants or if
/// the subsystems registry (`engine/config/standards/subsystems.json`) is missing.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::validate::{run_headers, ValidateArgs, ValidationDomain};
/// use std::path::PathBuf;
///
/// let args = ValidateArgs {
///     domain: ValidationDomain::Headers,
///     repo_root: PathBuf::from("."),
///     ..Default::default()
/// };
/// // Assuming valid workspace root:
/// if args.repo_root.join("engine/include").is_dir() {
///     assert!(run_headers(&args).is_ok());
/// }
/// ```
pub fn run_headers(args: &ValidateArgs) -> Result<(), String> {
    let registry = load_subsystems_registry(&args.repo_root)?;
    let rules = load_header_rules_config(&args.repo_root);
    let validator = HeaderValidator::new(
        args.repo_root.clone(),
        registry,
        rules,
        args.verbose,
    );
    let summary = validator.run_audit()?;

    if args.json_output {
        let json_report = serde_json::json!({
            "target_domain": ENGINE_INCLUDE_DIR,
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
        summary.print_custom_summary(
            "C11 Engine Headers 5-Block Standard Audit",
            ENGINE_INCLUDE_DIR,
        );
    }

    if summary.is_success(args.strict) {
        Ok(())
    } else {
        Err(format!(
            "Header validation failed with {} error(s) and {} warning(s).",
            summary.error_count(),
            summary.warning_count()
        ))
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Domain Data Catalog & JSON Envelope Audit
// -----------------------------------------------------------------------------

/// Executes data catalog validation against JSON manifests under `game/data/`.
///
/// Verifies 5-block envelope schemas, data types, cross-manifest link integrity,
/// and numerical balance formulas.
///
/// # Errors
///
/// Returns `Err(String)` if any manifest violates its schema or if cross-entity
/// references point to non-existent identifiers.
pub fn run_data(args: &ValidateArgs) -> Result<(), String> {
    let opts = ValidationOptions {
        target_domain: args.data_subdomain.clone(),
        strict: args.strict,
        verbose: args.verbose,
        repo_root: args.repo_root.clone(),
    };

    let validator = DataValidator::new(opts);
    let summary = validator.run_audit()?;

    if args.json_output {
        let json_report = serde_json::json!({
            "target_domain": args.data_subdomain,
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
        summary.print_summary(&args.data_subdomain);
    }

    if summary.is_success(args.strict) {
        Ok(())
    } else {
        Err(format!(
            "Data validation failed with {} error(s) and {} warning(s).",
            summary.error_count(),
            summary.warning_count()
        ))
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Git Commit Ledger & Release Tag Verification
// -----------------------------------------------------------------------------

/// Executes git commit history and milestone release tag validation.
///
/// Inspects the commit log to guarantee all commit messages adhere to the
/// conventional 5-block structure and milestone lineage requirements.
///
/// # Errors
///
/// Returns `Err(String)` if commits violate conventional formatting or if
/// release tag annotation messages are malformed.
pub fn run_commits(args: &ValidateArgs) -> Result<(), String> {
    let mut report = GitAuditReport::default();
    audit_git_history(Some(DEFAULT_GIT_AUDIT_DEPTH), &mut report);
    audit_git_tags(&mut report);

    if !args.json_output {
        report.print_report();
    } else {
        let json_val = serde_json::json!({
            "commits_audited": report.commits_audited,
            "commits_passed": report.commits_passed,
            "tags_audited": report.tags_audited,
            "tags_passed": report.tags_passed,
            "is_success": report.is_success(),
            "issues_count": report.issues.len(),
        });
        println!("{}", serde_json::to_string_pretty(&json_val).unwrap_or_default());
    }

    if report.is_success() {
        Ok(())
    } else {
        Err(format!(
            "Git commit auditing failed with {} violation(s).",
            report.issues.len()
        ))
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Rust 2024 Edition 5-Block Standard Verification
// -----------------------------------------------------------------------------

/// Recursively discovers `.rs` files within a directory, ignoring `target/` and `.git/`.
fn collect_rust_files(dir: &Path, files: &mut Vec<PathBuf>) {
    if let Ok(entries) = fs::read_dir(dir) {
        for entry in entries.flatten() {
            let path = entry.path();
            if path.is_dir() {
                let name = path.file_name().and_then(|n| n.to_str()).unwrap_or("");
                if name != "target" && name != ".git" {
                    collect_rust_files(&path, files);
                }
            } else if path.extension().and_then(|e| e.to_str()) == Some("rs") {
                files.push(path);
            }
        }
    }
}

/// Executes Rust 2024 Edition 5-block structural and covenant validation.
///
/// Supports evaluating an individual `.rs` file, a batch of files, or entire directory trees.
/// Defaults to auditing `tools/src/` if no specific target paths are configured.
///
/// # Errors
///
/// Returns `Err(String)` if any evaluated Rust file violates 5-block structure or rules.
pub fn run_rust(args: &ValidateArgs) -> Result<(), String> {
    let start = Instant::now();
    let config = RustValidationConfig::default();
    let mut summary = ValidationSummary::default();

    let mut files_to_scan = Vec::new();

    if args.target_paths.is_empty() {
        let tools_src = args.repo_root.join("tools/src");
        collect_rust_files(&tools_src, &mut files_to_scan);
    } else {
        for target in &args.target_paths {
            let full_path = if target.is_absolute() {
                target.clone()
            } else {
                args.repo_root.join(target)
            };

            if full_path.is_file() {
                if full_path.extension().and_then(|e| e.to_str()) == Some("rs") {
                    files_to_scan.push(full_path);
                }
            } else if full_path.is_dir() {
                collect_rust_files(&full_path, &mut files_to_scan);
            }
        }
    }

    files_to_scan.sort();
    files_to_scan.dedup();

    summary.total_files = files_to_scan.len();

    for path in &files_to_scan {
        match fs::read_to_string(path) {
            Ok(content) => {
                let mut issues = Vec::new();
                validate_rust_content(path, &content, &config, &mut issues);
                if issues.is_empty() {
                    summary.passed_files += 1;
                    if args.verbose {
                        println!("  [PASS] {}", path.display());
                    }
                } else {
                    summary.failed_files += 1;
                    summary.issues.extend(issues);
                }
            }
            Err(e) => {
                summary.failed_files += 1;
                summary.issues.push(ValidationIssue {
                    file: path.clone(),
                    block: "io".to_string(),
                    field: "read".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Failed to read file: {}", e),
                });
            }
        }
    }

    summary.duration_ms = start.elapsed().as_millis() as u64;

    let target_label = if args.target_paths.is_empty() {
        "tools/src (default)".to_string()
    } else {
        args.target_paths
            .iter()
            .map(|p| p.to_string_lossy().to_string())
            .collect::<Vec<_>>()
            .join(", ")
    };

    if !args.json_output {
        summary.print_custom_summary("Rust 2024 5-Block Standard Audit", &target_label);
    } else {
        let json_report = serde_json::json!({
            "domain": "rust",
            "targets": target_label,
            "total_files": summary.total_files,
            "passed_files": summary.passed_files,
            "failed_files": summary.failed_files,
            "error_count": summary.error_count(),
            "warning_count": summary.warning_count(),
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
    }

    if summary.is_success(args.strict) {
        Ok(())
    } else {
        Err(format!(
            "Rust validation failed with {} error(s) and {} warning(s).",
            summary.error_count(),
            summary.warning_count()
        ))
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.5: Composite Multi-Domain Validation Suite
// -----------------------------------------------------------------------------

/// Executes all validation suites sequentially in fail-fast order.
///
/// Pipeline Order:
///   1. C11 Engine Headers (`engine/include/`)
///   2. Domain Data Catalogs (`game/data/`)
///   3. Git Commit & Tag Ledger
///   4. Rust 2024 Source Files (`tools/src/`)
///
/// # Errors
///
/// Halts immediately on the first encountered domain failure.
pub fn run_all(args: &ValidateArgs) -> Result<(), String> {
    println!("\n=== [1/4] Running Header Validation ===");
    run_headers(args)?;

    println!("\n=== [2/4] Running Data Catalog Validation ===");
    let mut data_args = args.clone();
    data_args.data_subdomain = "all".to_string();
    run_data(&data_args)?;

    println!("\n=== [3/4] Running Git Ledger Audit ===");
    run_commits(args)?;

    println!("\n=== [4/4] Running Rust 2024 Code Audit ===");
    run_rust(args)?;

    println!("\n[ALL AUDITS PASSED] Clean bill of health across all domains.");
    Ok(())
}

/// Central entrypoint dispatching execution to the requested validation target.
pub fn execute(args: &ValidateArgs) -> Result<(), String> {
    match args.domain {
        ValidationDomain::Headers => run_headers(args),
        ValidationDomain::Data => run_data(args),
        ValidationDomain::Commits => run_commits(args),
        ValidationDomain::Rust => run_rust(args),
        ValidationDomain::All => run_all(args),
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.6: Standalone CLI Dispatchers & Argument Parsing Bridges
// -----------------------------------------------------------------------------

/// Standalone CLI bridge used by the `validator` binary executable.
pub fn dispatch_standalone_validator(cli_args: &[String], repo_root: &Path) -> Result<(), String> {
    let mut args = ValidateArgs {
        repo_root: repo_root.to_path_buf(),
        ..Default::default()
    };

    let mut i = 1;
    while i < cli_args.len() {
        match cli_args[i].as_str() {
            "--domain" | "-d" => {
                if i + 1 < cli_args.len() {
                    let d = &cli_args[i + 1];
                    if d == "rust" || d == "rs" {
                        args.domain = ValidationDomain::Rust;
                    } else if d == "headers" || d == "header" {
                        args.domain = ValidationDomain::Headers;
                    } else if d == "commits" || d == "commit" || d == "git" {
                        args.domain = ValidationDomain::Commits;
                    } else {
                        args.data_subdomain = d.clone();
                        args.domain = ValidationDomain::Data;
                    }
                    i += 1;
                }
            }
            "--all" | "-a" => {
                args.data_subdomain = "all".to_string();
            }
            "--strict" | "-s" => args.strict = true,
            "--verbose" | "-v" => args.verbose = true,
            "--json" | "-j" => args.json_output = true,
            "--help" | "-h" => {
                print_standalone_help();
                return Ok(());
            }
            "headers" | "header" => args.domain = ValidationDomain::Headers,
            "data" | "json" => args.domain = ValidationDomain::Data,
            "commits" | "commit" | "git" => args.domain = ValidationDomain::Commits,
            "rust" | "rs" => args.domain = ValidationDomain::Rust,
            other => {
                if !other.starts_with('-') {
                    if args.domain == ValidationDomain::Rust
                        || other.ends_with(".rs")
                        || other.contains("src/")
                        || other.contains("tests/")
                    {
                        args.domain = ValidationDomain::Rust;
                        args.target_paths.push(PathBuf::from(other));
                    } else if other == "headers" {
                        args.domain = ValidationDomain::Headers;
                    } else {
                        args.data_subdomain = other.to_string();
                        args.domain = ValidationDomain::Data;
                    }
                }
            }
        }
        i += 1;
    }

    if args.data_subdomain == "headers" {
        args.domain = ValidationDomain::Headers;
    }

    execute(&args)
}

/// Standalone CLI bridge used by the `git_validator` binary executable.
pub fn dispatch_standalone_git_validator(cli_args: &[String], _repo_root: &Path) -> Result<(), String> {
    let mut mode = ValidatorMode::FullAudit;
    let mut i = 1;
    while i < cli_args.len() {
        match cli_args[i].as_str() {
            "--hook" => {
                if i + 1 < cli_args.len() {
                    mode = ValidatorMode::HookFile(cli_args[i + 1].clone());
                    i += 1;
                }
            }
            "--recent" | "--commits" => {
                let count = if i + 1 < cli_args.len() {
                    cli_args[i + 1].parse::<usize>().unwrap_or(1)
                } else {
                    1
                };
                mode = ValidatorMode::RecentCommits(count);
                i += 1;
            }
            "--tags" => mode = ValidatorMode::TagsOnly,
            "--help" | "-h" => {
                println!("Usage: git_validator [OPTIONS]");
                println!("  --hook <file>      Audit a commit-msg hook file");
                println!("  --recent <n>       Audit last n git commits");
                println!("  --tags             Audit git tag annotation messages");
                return Ok(());
            }
            _ => {}
        }
        i += 1;
    }

    let mut report = GitAuditReport::default();
    match mode {
        ValidatorMode::HookFile(path) => {
            let content = std::fs::read_to_string(&path)
                .map_err(|e| format!("Could not read hook file {}: {}", path, e))?;
            report.commits_audited = 1;
            let issues = audit_commit_message(&path, &content);
            if issues.is_empty() {
                report.commits_passed = 1;
            } else {
                report.issues.extend(issues);
            }
        }
        ValidatorMode::RecentCommits(count) => {
            audit_git_history(Some(count), &mut report);
        }
        ValidatorMode::TagsOnly => {
            audit_git_tags(&mut report);
        }
        ValidatorMode::FullAudit => {
            audit_git_history(None, &mut report);
            audit_git_tags(&mut report);
        }
    }

    report.print_report();
    if report.is_success() {
        Ok(())
    } else {
        Err(format!("Git validation failed with {} error(s).", report.issues.len()))
    }
}

fn print_standalone_help() {
    println!("================================================================================");
    println!("       Tenebris and Lux - Data Catalog & Schema Validator CLI                   ");
    println!("================================================================================");
    println!("Usage: validator [OPTIONS] [DOMAIN]\n");
    println!("Arguments:");
    println!("  [DOMAIN]              Sub-domain under game/data/ to audit (default: 'system')");
    println!("                        Use 'all' to audit the entire game/data/ repository tree");
    println!("                        Use 'headers' to audit C11 engine headers under engine/include/\n");
    println!("Options:");
    println!("  -d, --domain <name>   Explicitly specify target sub-domain");
    println!("  -a, --all             Audit all sub-domains across game/data/");
    println!("  -s, --strict          Treat warnings as hard errors");
    println!("  -v, --verbose         Print status badge for each scanned file");
    println!("  -j, --json            Output validation results as structured JSON");
    println!("  -h, --help            Print this usage manual and exit");
    println!("================================================================================");
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Diagnostic Self-Audits, Invariant Probes & Verification Proofs )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Diagnostic Self-Audit Target
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying that validation constants and paths are valid.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::validate::self_audit;
///
/// assert!(self_audit());
/// ```
pub fn self_audit() -> bool {
    !ENGINE_INCLUDE_DIR.is_empty() && !GAME_DATA_DIR.is_empty() && DEFAULT_GIT_AUDIT_DEPTH > 0
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Invariant Unit Tests (Four-Section Ontological Structure)
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // --- Section 1: Always Pass (Nominal Defaults) ---
    #[test]
    fn test_section_1_always_pass_defaults() {
        let args = ValidateArgs::default();
        assert_eq!(args.domain, ValidationDomain::Headers);
        assert_eq!(args.data_subdomain, "system");
        assert_eq!(args.strict, false);
        assert_eq!(args.verbose, false);
        assert_eq!(args.json_output, false);
    }

    // --- Section 2: Never Fail (Fault Inversion & Safe Bounds) ---
    #[test]
    fn test_section_2_never_fail_preflight_self_audit() {
        assert!(self_audit(), "self_audit() preflight invariant must pass");
    }

    // --- Section 3: Invariants, Boundaries & Equality Contracts ---
    #[test]
    fn test_section_3_invariants_domain_equality() {
        assert_eq!(ValidationDomain::Headers, ValidationDomain::Headers);
        assert_ne!(ValidationDomain::Headers, ValidationDomain::Data);
        assert_ne!(ValidationDomain::Commits, ValidationDomain::All);
    }

    // --- Section 4: Emergence & Argument Mutation ---
    #[test]
    fn test_section_4_emergence_args_customization() {
        let mut args = ValidateArgs::default();
        args.strict = true;
        args.verbose = true;
        args.domain = ValidationDomain::Data;
        args.data_subdomain = "mechanics".to_string();

        assert!(args.strict);
        assert!(args.verbose);
        assert_eq!(args.domain, ValidationDomain::Data);
        assert_eq!(args.data_subdomain, "mechanics");
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Lifecycle Status, Module Epilogue & Handshake Predicates )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Lifecycle Status Handshake
// -----------------------------------------------------------------------------

/// Verifies that a given repository root directory has the mandatory layout
/// required to host validation operations.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::validate::is_valid_workspace_root;
/// use tl_tools::cmd::resolve_repo_root;
///
/// let root = resolve_repo_root();
/// assert!(is_valid_workspace_root(&root));
/// ```
pub fn is_valid_workspace_root(root: &Path) -> bool {
    root.join("CMakeLists.txt").is_file()
        || root.join(ENGINE_INCLUDE_DIR).is_dir()
        || root.join(GAME_DATA_DIR).is_dir()
}

/// Epilogue lifecycle readiness probe for the validation command sub-assembly.
///
/// # Examples
///
/// ```rust
/// use tl_tools::cmd::validate::is_validate_cmd_ready;
///
/// assert!(is_validate_cmd_ready());
/// ```
pub fn is_validate_cmd_ready() -> bool {
    self_audit()
}

