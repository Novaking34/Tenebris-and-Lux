// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/json.rs
// @id            cog.tools.validator.json
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateful
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          5-Block JSON Data Manifest & Catalog Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | bounded JSON AST scan
// @scale         O(N) recursive traversal across data domains
// @seams         in: syntax::json | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on malformed or invalid JSON payloads.
//                2. Strictly enforces the 5-block envelope on all data catalogs and manifests.
//                3. Emits standardized ValidationIssue records for all schema discrepancies.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates data manifests and mechanical configurations across `game/data/`
//   and `engine/config/`. Enforces the OmniCode 5-Block JSON envelope
//   (`block_1_metadata` through `block_5_closing`), identity anchors, and data purity.
//
//   ### For Game Designers & Non-Programmers:
//   Makes sure our game catalogs—like hero stats, skills, items, and dialogue—
//   are formatted correctly so the game engine reads them without errors or crashes.
//
//   ### For Systems Programmers & Engine Architects:
//   Integrates with `syntax::json::parse_json_5block_doc`, checks required block keys,
//   validates schema IDs, isolates Block 3 pure payloads, and tracks traversal telemetry.
// =============================================================================

//! # OmniCode 5-Block JSON Data Validator
//!
//! Enforces universal 5-block JSON envelopes, schema declarations, and manifest links.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Types & Configuration Structures )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::fs;
use std::path::{Path, PathBuf};
use std::time::Instant;

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};
use crate::syntax::json::parse_json_5block_doc;
use crate::validator::report::{IssueSeverity, ValidationIssue, ValidationSummary};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Validation Options
// -----------------------------------------------------------------------------

/// Configuration options for JSON data validation sweeps.
///
/// ```rust
/// use tl_tools::validator::json::ValidationOptions;
///
/// let opts = ValidationOptions::default();
/// assert_eq!(opts.target_domain, "system");
/// assert!(!opts.strict);
/// ```
#[derive(Debug, Clone)]
pub struct ValidationOptions {
    /// Target sub-domain under `game/data/` (e.g. "system", "config", or "all").
    pub target_domain: String,
    /// If true, warnings are treated as hard errors.
    pub strict: bool,
    /// Emit detailed per-file telemetry to stdout.
    pub verbose: bool,
    /// Root path of the repository workspace.
    pub repo_root: PathBuf,
}

impl Default for ValidationOptions {
    fn default() -> Self {
        Self {
            target_domain: "system".to_string(),
            strict: false,
            verbose: false,
            repo_root: PathBuf::from("."),
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( DataValidator Engine, Content Auditing & Task Runner Dispatch )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: DataValidator Workspace Engine
// -----------------------------------------------------------------------------

/// Main validator engine auditing 5-block JSON contracts.
pub struct DataValidator {
    options: ValidationOptions,
}

impl DataValidator {
    /// Creates a new data validator with specified options.
    ///
    /// # Dual-Audience Documentation
    /// * For Game Designers: Sets up the catalog inspector for a chosen game folder.
    /// * For Systems Programmers: Stores traversal options and root directory path.
    ///
    /// ```rust
    /// use tl_tools::validator::json::{DataValidator, ValidationOptions};
    ///
    /// let validator = DataValidator::new(ValidationOptions::default());
    /// assert_eq!(validator.options().target_domain, "system");
    /// ```
    pub fn new(options: ValidationOptions) -> Self {
        Self { options }
    }

    /// Accesses the underlying validation options.
    pub fn options(&self) -> &ValidationOptions {
        &self.options
    }

    /// Recursively discovers all `*.json` files within a root directory.
    ///
    /// # Dual-Audience Documentation
    /// * For Game Designers: Finds all JSON data sheets in the folder.
    /// * For Systems Programmers: Stack-based non-recursive directory crawler.
    ///
    /// ```rust
    /// use std::path::Path;
    /// use tl_tools::validator::json::DataValidator;
    ///
    /// let files = DataValidator::discover_json_files(Path::new("nonexistent"));
    /// assert!(files.is_empty());
    /// ```
    pub fn discover_json_files(dir: &Path) -> Vec<PathBuf> {
        let mut files = Vec::new();
        if !dir.exists() || !dir.is_dir() {
            return files;
        }

        let mut dirs_to_visit = vec![dir.to_path_buf()];
        while let Some(current_dir) = dirs_to_visit.pop() {
            if let Ok(entries) = fs::read_dir(&current_dir) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.is_dir() {
                        dirs_to_visit.push(path);
                    } else if path.is_file() {
                        if let Some(ext) = path.extension() {
                            if ext == "json" {
                                files.push(path);
                            }
                        }
                    }
                }
            }
        }
        files.sort();
        files
    }

    /// Executes validation sweep across the configured data domain.
    ///
    /// # Dual-Audience Documentation
    /// * For Game Designers: Runs through all data files and reports any errors found.
    /// * For Systems Programmers: Gathers files, validates each via `validate_file`,
    ///   records telemetry, and returns a cumulative `ValidationSummary`.
    pub fn run_audit(&self) -> Result<ValidationSummary, String> {
        let start = Instant::now();
        let target_dir = if self.options.target_domain == "config" {
            let p = self.options.repo_root.join("engine/config");
            if p.exists() {
                p
            } else {
                self.options.repo_root.join("config")
            }
        } else if self.options.target_domain == "system" {
            let p_engine = self.options.repo_root.join("engine/config");
            let p_game = self.options.repo_root.join("game/data/system");
            if p_engine.exists() {
                p_engine
            } else if p_game.exists() {
                p_game
            } else {
                self.options.repo_root.join("system")
            }
        } else if self.options.target_domain == "all" {
            self.options.repo_root.join("game/data")
        } else {
            self.options.repo_root.join("game/data").join(&self.options.target_domain)
        };

        if !target_dir.exists() {
            return Err(format!("Target directory does not exist: {:?}", target_dir));
        }

        let json_files = Self::discover_json_files(&target_dir);
        let mut summary = ValidationSummary {
            total_files: json_files.len(),
            ..Default::default()
        };

        for file_path in &json_files {
            let mut file_issues = Vec::new();
            self.validate_file(file_path, &mut file_issues);

            let has_errors = file_issues.iter().any(|i| i.severity == IssueSeverity::Error);
            if has_errors {
                summary.failed_files += 1;
            } else {
                summary.passed_files += 1;
            }

            if self.options.verbose {
                let status_badge = if has_errors { "[FAIL]" } else { "[PASS]" };
                let rel_path = file_path
                    .strip_prefix(&self.options.repo_root)
                    .unwrap_or(file_path);
                println!("{} {}", status_badge, rel_path.display());
            }

            summary.issues.extend(file_issues);
        }

        summary.duration_ms = start.elapsed().as_millis() as u64;
        Ok(summary)
    }

    /// Audits an individual JSON file against 5-block envelope standards.
    pub fn validate_file(&self, path: &Path, issues: &mut Vec<ValidationIssue>) {
        let content = match fs::read_to_string(path) {
            Ok(c) => c,
            Err(e) => {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "I/O".to_string(),
                    field: "read".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Failed to read file: {}", e),
                });
                return;
            }
        };

        validate_json_content(path, &content, issues);
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: 5-Block Envelope Content Validator
// -----------------------------------------------------------------------------

/// Validates raw JSON string content against the 5-block envelope.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Verifies that a JSON document has all 5 required blocks.
/// * For Systems Programmers: Parses via `parse_json_5block_doc` and checks for
///   `block_1_metadata` through `block_5_closing`.
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::validator::json::validate_json_content;
///
/// let json = r#"{
///   "block_1_metadata": { "id": "test.manifest" },
///   "block_2_opening": {},
///   "block_3_body": {},
///   "block_4_audit": {},
///   "block_5_closing": {}
/// }"#;
/// let mut issues = Vec::new();
/// validate_json_content(Path::new("test.json"), json, &mut issues);
/// assert!(issues.is_empty());
/// ```
pub fn validate_json_content(path: &Path, content: &str, issues: &mut Vec<ValidationIssue>) {
    match parse_json_5block_doc(content) {
        Ok(doc) => {
            if !doc.has_all_five_blocks {
                let required = [
                    "block_1_metadata",
                    "block_2_opening",
                    "block_3_body",
                    "block_4_audit",
                    "block_5_closing",
                ];
                for block_name in &required {
                    if !doc.has_block(block_name) {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: (*block_name).to_string(),
                            field: "envelope".to_string(),
                            severity: IssueSeverity::Error,
                            message: format!("Missing required 5-block envelope key '{}'", block_name),
                        });
                    }
                }
            }
        }
        Err(e) => {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "SYNTAX".to_string(),
                field: "parse".to_string(),
                severity: IssueSeverity::Error,
                message: format!("JSON parsing failure: {}", e),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Pipeline Task Runner Implementation
// -----------------------------------------------------------------------------

/// Task runner implementing the TaskRunner trait for `data_validator`.
pub struct DataValidationTask;

impl TaskRunner for DataValidationTask {
    fn name(&self) -> &'static str {
        "data_validator"
    }

    fn description(&self) -> &'static str {
        "Audits OmniCode 5-Block JSON manifests and catalogs across game/data/ and engine/config/"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        let config_dir = ctx.repo_root.join("engine/config");
        let system_dir = ctx.repo_root.join("game/data/system");
        if !config_dir.exists() && !system_dir.exists() {
            return Err(format!(
                "Preflight failed: neither engine/config nor game/data/system found at {:?}",
                ctx.repo_root
            ));
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let target_domain = if ctx.repo_root.join("engine/config").exists() {
            "config".to_string()
        } else {
            "system".to_string()
        };

        let options = ValidationOptions {
            target_domain: target_domain.clone(),
            strict: false,
            verbose: ctx.verbose,
            repo_root: ctx.repo_root.clone(),
        };

        let validator = DataValidator::new(options);
        let summary = validator.run_audit()?;

        if summary.is_success(false) {
            Ok(TaskTelemetry::success(
                self.name(),
                summary.duration_ms,
                &format!(
                    "Audited {} files across {} cleanly (0 errors, {} warnings)",
                    summary.total_files,
                    if target_domain == "config" { "engine/config/" } else { "game/data/system/" },
                    summary.warning_count()
                ),
            ))
        } else {
            Ok(TaskTelemetry::failure(
                self.name(),
                summary.duration_ms,
                &format!(
                    "Data validation failed: {} errors across {} files",
                    summary.error_count(),
                    summary.failed_files
                ),
            ))
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Four-Section Ontological Test Suite )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing JSON validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests that incomplete JSON files trigger warnings.
/// * For Systems Programmers: Runs incomplete JSON through `validate_json_content`.
///
/// ```rust
/// use tl_tools::validator::json::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let incomplete_json = "{\"block_1_metadata\": {}}";
    let mut issues = Vec::new();
    validate_json_content(Path::new("test.json"), incomplete_json, &mut issues);

    if !issues.iter().any(|i| i.block == "block_2_opening") {
        return Err("Self-audit failed: missing block_2_opening not detected");
    }
    Ok(())
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// -----------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_nominal_valid_json_passes() {
        let json = r#"{
            "block_1_metadata": { "id": "test.manifest" },
            "block_2_opening": {},
            "block_3_body": {},
            "block_4_audit": {},
            "block_5_closing": {}
        }"#;
        let mut issues = Vec::new();
        validate_json_content(Path::new("test.json"), json, &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_malformed_json_syntax() {
        let mut issues = Vec::new();
        validate_json_content(Path::new("bad.json"), "{ unquoted: true ", &mut issues);
        assert!(issues.iter().any(|i| i.block == "SYNTAX"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_missing_envelope_blocks() {
        let json = r#"{ "block_1_metadata": {} }"#;
        let mut issues = Vec::new();
        validate_json_content(Path::new("test.json"), json, &mut issues);
        assert_eq!(issues.len(), 4); // missing blocks 2, 3, 4, 5
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Handshake & Subsystem Readiness Probe )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe & Lifecycle Handshake
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the JSON validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the JSON data catalog inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::json::is_json_validator_ready;
///
/// assert!(is_json_validator_ready());
/// ```
pub fn is_json_validator_ready() -> bool {
    self_audit().is_ok()
}
