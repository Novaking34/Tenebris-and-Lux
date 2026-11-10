// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/json_data.rs
// @id            tools.validator.json_data
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-09
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Core data validation and schema auditing engine for Tenebris and Lux.
//   Provides automated, compile-time and CI/CD verification of all JSON data
//   files, manifests, and catalogs across `game/data/` and `engine/config/`.
// =============================================================================

//! # Data Validation & Schema Auditing Engine
//!
//! Validates `game/data/` and `engine/config/` manifests and data catalogs against
//! the OmniCode Universal 5-Block Architecture and domain invariant contracts.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.validator.json_data
// =============================================================================

use std::fs;
use std::path::{Path, PathBuf};
use std::time::Instant;

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};
use super::report::{IssueSeverity, ValidationIssue, ValidationSummary};

/// Execution options configuring the validation sweep.
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
// BLOCK 3: BODY BLOCK / CORE VALIDATION ENGINE
// Stratum: Developer Tooling | Machine: tools.validator.json_data
// =============================================================================

/// Main validator engine auditing 5-block JSON contracts.
pub struct DataValidator {
    options: ValidationOptions,
}

impl DataValidator {
    /// Creates a new data validator with specified options.
    pub fn new(options: ValidationOptions) -> Self {
        Self { options }
    }

    /// Recursively discovers all `*.json` files within a root directory.
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
                self.options.repo_root.join("data/system")
            }
        } else if self.options.target_domain == "all" {
            let p = self.options.repo_root.join("game/data");
            if p.exists() {
                p
            } else {
                self.options.repo_root.join("data")
            }
        } else {
            let p = self
                .options
                .repo_root
                .join(format!("game/data/{}", self.options.target_domain));
            if p.exists() {
                p
            } else {
                self.options
                    .repo_root
                    .join(format!("data/{}", self.options.target_domain))
            }
        };

        if !target_dir.exists() {
            return Err(format!(
                "Target data directory does not exist: {:?}",
                target_dir
            ));
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

    /// Audits an individual JSON file against universal 5-block and domain contracts.
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

        let val: serde_json::Value = match serde_json::from_str(&content) {
            Ok(v) => v,
            Err(e) => {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "JSON".to_string(),
                    field: "syntax".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Syntax error at line {}, col {}: {}", e.line(), e.column(), e),
                });
                return;
            }
        };

        let root_obj = match val.as_object() {
            Some(obj) => obj,
            None => {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "ROOT".to_string(),
                    field: "type".to_string(),
                    severity: IssueSeverity::Error,
                    message: "Root JSON entity must be an object".to_string(),
                });
                return;
            }
        };

        // 1. Enforce 5-Block Lifecycle Structure
        self.check_required_block(path, root_obj, "block_1_metadata", issues);
        self.check_required_block(path, root_obj, "block_2_opening", issues);
        self.check_required_block(path, root_obj, "block_3_body", issues);
        self.check_required_block(path, root_obj, "block_4_audit", issues);
        self.check_required_block(path, root_obj, "block_5_closing", issues);

        // 2. Validate Block 1 Metadata Contracts
        if let Some(b1) = root_obj.get("block_1_metadata").and_then(|v| v.as_object()) {
            self.validate_block_1_metadata(path, b1, issues);
        }

        // 3. Validate Block 2 Opening Contracts
        if let Some(b2) = root_obj.get("block_2_opening").and_then(|v| v.as_object()) {
            self.validate_block_2_opening(path, b2, issues);
        }

        // 4. Validate Block 3 Body & Cross-Manifest Links
        if let Some(b3) = root_obj.get("block_3_body") {
            self.validate_block_3_body(path, b3, issues);
        }

        // 5. Validate Block 4 Audit Contracts
        if let Some(b4) = root_obj.get("block_4_audit").and_then(|v| v.as_object()) {
            self.validate_block_4_audit(path, b4, issues);
        }

        // 6. Validate Block 5 Closing & Navigation
        if let Some(b5) = root_obj.get("block_5_closing").and_then(|v| v.as_object()) {
            self.validate_block_5_closing(path, b5, issues);
        }

        // 7. Domain-Specific Invariant Checks
        self.validate_domain_invariants(path, root_obj, issues);
    }

    pub fn check_required_block(
        &self,
        path: &Path,
        obj: &serde_json::Map<String, serde_json::Value>,
        block_name: &str,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !obj.contains_key(block_name) {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: block_name.to_string(),
                field: "existence".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing required 5-block section '{}'", block_name),
            });
        }
    }

    fn validate_block_1_metadata(
        &self,
        path: &Path,
        b1: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        // Project anchor
        match b1.get("project").and_then(|v| v.as_str()) {
            Some("Tenebris and Lux") => {}
            Some(other) => issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "project".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Expected project 'Tenebris and Lux', found '{}'", other),
            }),
            None => issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "project".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'project' metadata field".to_string(),
            }),
        }

        // Version contract
        match b1.get("version").and_then(|v| v.as_str()) {
            Some("0.1.0") | Some("0.1.1-dev") | Some("0.1.1") => {}
            Some(other) => issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "version".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Expected version '0.1.0' or '0.1.1-dev', found '{}'", other),
            }),
            None => issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "version".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'version' metadata field".to_string(),
            }),
        }

        // Identifier
        if b1.get("id").and_then(|v| v.as_str()).unwrap_or("").is_empty() {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "id".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing or empty 'id' metadata field".to_string(),
            });
        }

        // Invariants list
        match b1.get("invariants").and_then(|v| v.as_array()) {
            Some(invs) if !invs.is_empty() => {}
            _ => issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "invariants".to_string(),
                severity: IssueSeverity::Warning,
                message: "'invariants' array is missing or empty".to_string(),
            }),
        }
    }

    fn validate_block_2_opening(
        &self,
        path: &Path,
        b2: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !b2.contains_key("pre_conditions") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_2_opening".to_string(),
                field: "pre_conditions".to_string(),
                severity: IssueSeverity::Warning,
                message: "Missing 'pre_conditions' in opening block".to_string(),
            });
        }

        if !b2.contains_key("static_declarations") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_2_opening".to_string(),
                field: "static_declarations".to_string(),
                severity: IssueSeverity::Warning,
                message: "Missing 'static_declarations' in opening block".to_string(),
            });
        }
    }

    fn validate_block_3_body(
        &self,
        path: &Path,
        b3: &serde_json::Value,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let parent_dir = path.parent().unwrap_or_else(|| Path::new("."));

        // If body has a settings menu, check referenced files
        if let Some(settings) = b3.pointer("/menu/settings").and_then(|v| v.as_array()) {
            for setting in settings {
                if let Some(file_name) = setting.get("file").and_then(|v| v.as_str()) {
                    let target_file = parent_dir.join(file_name);
                    if !target_file.exists() {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_3_body".to_string(),
                            field: "menu.settings[].file".to_string(),
                            severity: IssueSeverity::Error,
                            message: format!("Referenced setting file does not exist: {:?}", target_file),
                        });
                    }
                }
            }
        }

        // If body has catalog items, check referenced files
        if let Some(catalogs) = b3.get("catalogs").and_then(|v| v.as_array()) {
            for cat in catalogs {
                if let Some(file_name) = cat.get("file").and_then(|v| v.as_str()) {
                    let target_file = parent_dir.join(file_name);
                    if !target_file.exists() {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_3_body".to_string(),
                            field: "catalogs[].file".to_string(),
                            severity: IssueSeverity::Error,
                            message: format!("Referenced catalog file does not exist: {:?}", target_file),
                        });
                    }
                }
            }
        }

        // If body has profile items, check referenced files
        if let Some(profiles) = b3.get("profiles").and_then(|v| v.as_array()) {
            for prof in profiles {
                if let Some(file_name) = prof.get("file").and_then(|v| v.as_str()) {
                    let target_file = parent_dir.join(file_name);
                    if !target_file.exists() {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_3_body".to_string(),
                            field: "profiles[].file".to_string(),
                            severity: IssueSeverity::Error,
                            message: format!("Referenced profile file does not exist: {:?}", target_file),
                        });
                    }
                }
            }
        }

        // If body has formats items, check referenced files
        if let Some(formats) = b3.get("formats").and_then(|v| v.as_array()) {
            for fmt in formats {
                if let Some(file_name) = fmt.get("file").and_then(|v| v.as_str()) {
                    let target_file = parent_dir.join(file_name);
                    if !target_file.exists() {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_3_body".to_string(),
                            field: "formats[].file".to_string(),
                            severity: IssueSeverity::Error,
                            message: format!("Referenced format file does not exist: {:?}", target_file),
                        });
                    }
                }
            }
        }

        // If body has subsystem manifests, check referenced paths
        if let Some(subsystems) = b3.get("subsystems").and_then(|v| v.as_array()) {
            for sub in subsystems {
                if let Some(manifest_path) = sub.get("manifest_path").and_then(|v| v.as_str()) {
                    let target_file = parent_dir.join(manifest_path);
                    if !target_file.exists() {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_3_body".to_string(),
                            field: "subsystems[].manifest_path".to_string(),
                            severity: IssueSeverity::Error,
                            message: format!("Subsystem manifest does not exist: {:?}", target_file),
                        });
                    }
                }
            }
        }
    }

    fn validate_block_4_audit(
        &self,
        path: &Path,
        b4: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !b4.contains_key("audit_metrics") && !b4.contains_key("metrics") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_4_audit".to_string(),
                field: "audit_metrics / metrics".to_string(),
                severity: IssueSeverity::Warning,
                message: "Missing 'audit_metrics' or 'metrics' in audit block".to_string(),
            });
        }

        if !b4.contains_key("verification_tripwires") && !b4.contains_key("tripwires") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_4_audit".to_string(),
                field: "verification_tripwires / tripwires".to_string(),
                severity: IssueSeverity::Warning,
                message: "Missing 'verification_tripwires' or 'tripwires' in audit block".to_string(),
            });
        }
    }

    fn validate_block_5_closing(
        &self,
        path: &Path,
        b5: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !b5.contains_key("navigation") && !b5.contains_key("execution_handshake") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_5_closing".to_string(),
                field: "navigation".to_string(),
                severity: IssueSeverity::Warning,
                message: "Closing block should declare navigation or execution_handshake".to_string(),
            });
        }

        // Validate parent_menu link if present
        if let Some(parent_menu) = b5
            .get("navigation")
            .and_then(|v| v.get("parent_menu"))
            .and_then(|v| v.as_str())
        {
            let parent_dir = path.parent().unwrap_or_else(|| Path::new("."));
            let target = parent_dir.join(parent_menu);
            if !target.exists() {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_5_closing".to_string(),
                    field: "navigation.parent_menu".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Referenced parent_menu does not exist: {:?}", target),
                });
            }
        }
    }

    fn validate_domain_invariants(
        &self,
        path: &Path,
        root: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let file_name = path
            .file_name()
            .and_then(|n| n.to_str())
            .unwrap_or("");

        match file_name {
            "error_codes.json" => {
                self.validate_error_codes(path, root, issues);
            }
            "severity_weights.json" => {
                self.validate_severity_weights(path, root, issues);
            }
            "snapshot_policy.json" => {
                self.validate_snapshot_policy(path, root, issues);
            }
            "statera_telemetry.json" => {
                self.validate_statera_telemetry(path, root, issues);
            }
            _ => {}
        }
    }

    fn validate_error_codes(
        &self,
        path: &Path,
        root: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        let mut mnemonics = std::collections::HashSet::new();
        let mut hex_codes = std::collections::HashSet::new();

        if let Some(domains) = root
            .get("block_3_body")
            .and_then(|v| v.get("domains"))
            .and_then(|v| v.as_array())
        {
            for domain in domains {
                let dom_id = domain.get("domain_id").and_then(|v| v.as_str()).unwrap_or("");
                if let Some(cats) = domain.get("categories").and_then(|v| v.as_array()) {
                    for cat in cats {
                        if let Some(subcats) = cat.get("subcategories").and_then(|v| v.as_array()) {
                            for subcat in subcats {
                                if let Some(errs) = subcat.get("errors").and_then(|v| v.as_array()) {
                                    for err in errs {
                                        let code_hex = err.get("code_hex").and_then(|v| v.as_str()).unwrap_or("");
                                        let mnemonic = err.get("mnemonic").and_then(|v| v.as_str()).unwrap_or("");

                                        // Check uniqueness
                                        if !hex_codes.insert(code_hex.to_string()) {
                                            issues.push(ValidationIssue {
                                                file: path.to_path_buf(),
                                                block: "block_3_body".to_string(),
                                                field: "code_hex".to_string(),
                                                severity: IssueSeverity::Error,
                                                message: format!("Duplicate error hex code: {}", code_hex),
                                            });
                                        }

                                        if !mnemonics.insert(mnemonic.to_string()) {
                                            issues.push(ValidationIssue {
                                                file: path.to_path_buf(),
                                                block: "block_3_body".to_string(),
                                                field: "mnemonic".to_string(),
                                                severity: IssueSeverity::Error,
                                                message: format!("Duplicate error mnemonic: {}", mnemonic),
                                            });
                                        }

                                        // Check hex prefix by domain
                                        if dom_id == "ENGINE" && !code_hex.starts_with("0x0") {
                                            issues.push(ValidationIssue {
                                                file: path.to_path_buf(),
                                                block: "block_3_body".to_string(),
                                                field: "code_hex".to_string(),
                                                severity: IssueSeverity::Error,
                                                message: format!("Engine error '{}' must use hex prefix 0x0xxx", code_hex),
                                            });
                                        } else if dom_id == "GAME" && !code_hex.starts_with("0x1") {
                                            issues.push(ValidationIssue {
                                                file: path.to_path_buf(),
                                                block: "block_3_body".to_string(),
                                                field: "code_hex".to_string(),
                                                severity: IssueSeverity::Error,
                                                message: format!("Game error '{}' must use hex prefix 0x1xxx", code_hex),
                                            });
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fn validate_severity_weights(
        &self,
        path: &Path,
        root: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if let Some(sevs) = root
            .get("block_3_body")
            .and_then(|v| v.get("severities"))
            .and_then(|v| v.as_array())
        {
            let mut prev_weight = 101;
            let mut bitmasks = std::collections::HashSet::new();

            for sev in sevs {
                let weight = sev.get("weight").and_then(|v| v.as_i64()).unwrap_or(0);
                let bitmask = sev.get("bitmask_hex").and_then(|v| v.as_str()).unwrap_or("");

                if weight >= prev_weight {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_3_body".to_string(),
                        field: "weight".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Severities must be strictly descending in weight; found {}", weight),
                    });
                }
                prev_weight = weight;

                if !bitmasks.insert(bitmask.to_string()) {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_3_body".to_string(),
                        field: "bitmask_hex".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Duplicate severity bitmask: {}", bitmask),
                    });
                }
            }
        }
    }

    fn validate_snapshot_policy(
        &self,
        path: &Path,
        root: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if let Some(cap) = root
            .get("block_3_body")
            .and_then(|v| v.pointer("/policy/ring_buffer_capacity"))
            .and_then(|v| v.as_i64())
        {
            if cap > 64 {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_3_body".to_string(),
                    field: "ring_buffer_capacity".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Snapshot capacity {} exceeds DEBUG_MAX_SNAPSHOTS limit of 64", cap),
                });
            }
        }
    }

    fn validate_statera_telemetry(
        &self,
        path: &Path,
        root: &serde_json::Map<String, serde_json::Value>,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if let Some(tiers) = root
            .get("block_3_body")
            .and_then(|v| v.pointer("/moral_tiers"))
            .and_then(|v| v.as_array())
        {
            if tiers.len() != 13 {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_3_body".to_string(),
                    field: "moral_tiers".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Expected 13 moral tiers, found {}", tiers.len()),
                });
            }
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / TASK RUNNER IMPLEMENTATION
// Stratum: Developer Tooling | Machine: tools.validator.json_data
// =============================================================================

/// Task runner adapter integrating data validation into the modular `RunnerEngine`.
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
// BLOCK 5: CLOSING BLOCK / UNIT TESTS
// Stratum: Developer Tooling | Machine: tools.validator.json_data
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_system_data_validation_passes() {
        let candidate_roots = [Path::new("."), Path::new("..")];
        let repo_root = candidate_roots
            .iter()
            .find(|p| p.join("engine/config").exists() || p.join("game/data/system").exists())
            .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."));

        let target_domain = if repo_root.join("engine/config").exists() {
            "config".to_string()
        } else {
            "system".to_string()
        };

        let options = ValidationOptions {
            target_domain,
            strict: false,
            verbose: false,
            repo_root,
        };

        let validator = DataValidator::new(options);
        let summary = validator.run_audit();
        if let Ok(res) = summary {
            assert!(
                res.is_success(false),
                "Data validation on target domain failed with {} errors",
                res.error_count()
            );
        }
    }

    #[test]
    fn test_5_block_structure_detects_missing_block() {
        let validator = DataValidator::new(ValidationOptions::default());
        let mut issues = Vec::new();
        let invalid_json = serde_json::json!({
            "block_1_metadata": { "project": "Tenebris and Lux", "version": "0.1.0" },
            "block_2_opening": {},
            "block_3_body": {}
        });

        if let Some(obj) = invalid_json.as_object() {
            validator.check_required_block(Path::new("test.json"), obj, "block_4_audit", &mut issues);
            validator.check_required_block(Path::new("test.json"), obj, "block_5_closing", &mut issues);
        }

        assert_eq!(issues.len(), 2);
        assert_eq!(issues[0].severity, IssueSeverity::Error);
    }
}

