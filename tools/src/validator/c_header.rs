// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/c_header.rs
// @id            tools.validator.c_header
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-09
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   C11 5-Block Header Validator. Enforces the OmniCode Universal 5-Block
//   Lifecycle Protocol, Invariant 18 (zero // comments), Anti-Domain-Bleed
//   covenants, two-sub-assembly Block 4 & Block 5 standards, and subsystem
//   archetype requirements across all headers in `engine/include/`.
// =============================================================================

//! # C11 Header Validator Engine
//!
//! Validates `engine/include/` headers against OmniCode standards, archetype
//! specifications, and invariant contracts.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

use std::fs;
use std::path::{Path, PathBuf};
use std::time::Instant;

use crate::runner::{RunnerContext, TaskRunner, TaskTelemetry};
use super::config::{HeaderArchetype, HeaderRulesConfig, SubsystemRegistry};
use super::report::{IssueSeverity, ValidationIssue, ValidationSummary};

/// Validator auditing C11 header files against 5-block architecture.
pub struct HeaderValidator {
    repo_root: PathBuf,
    registry: SubsystemRegistry,
    rules: HeaderRulesConfig,
    verbose: bool,
}

// =============================================================================
// BLOCK 3: BODY BLOCK / CORE VALIDATION IMPLEMENTATION
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

impl HeaderValidator {
    /// Creates a new HeaderValidator with loaded registry and rules.
    pub fn new(repo_root: PathBuf, registry: SubsystemRegistry, rules: HeaderRulesConfig, verbose: bool) -> Self {
        Self {
            repo_root,
            registry,
            rules,
            verbose,
        }
    }

    /// Recursively discovers all `*.h` files in `engine/include/`.
    pub fn discover_headers(include_dir: &Path) -> Vec<PathBuf> {
        let mut headers = Vec::new();
        if !include_dir.exists() || !include_dir.is_dir() {
            return headers;
        }

        let mut stack = vec![include_dir.to_path_buf()];
        while let Some(current) = stack.pop() {
            if let Ok(entries) = fs::read_dir(&current) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.is_dir() {
                        stack.push(path);
                    } else if path.is_file() {
                        if let Some(ext) = path.extension() {
                            if ext == "h" {
                                headers.push(path);
                            }
                        }
                    }
                }
            }
        }
        headers.sort();
        headers
    }

    /// Executes validation sweep across all registered headers.
    pub fn run_audit(&self) -> Result<ValidationSummary, String> {
        let start = Instant::now();
        let include_dir = self.repo_root.join("engine/include");

        if !include_dir.exists() {
            return Err(format!("Engine include directory not found: {:?}", include_dir));
        }

        let header_files = Self::discover_headers(&include_dir);
        let mut summary = ValidationSummary {
            total_files: header_files.len(),
            ..Default::default()
        };

        for file_path in &header_files {
            let mut file_issues = Vec::new();
            self.validate_header(file_path, &mut file_issues);

            let has_errors = file_issues.iter().any(|i| i.severity == IssueSeverity::Error);
            if has_errors {
                summary.failed_files += 1;
            } else {
                summary.passed_files += 1;
            }

            if self.verbose {
                let status_badge = if has_errors { "[FAIL]" } else { "[PASS]" };
                let rel_path = file_path
                    .strip_prefix(&self.repo_root)
                    .unwrap_or(file_path);
                println!("{} {}", status_badge, rel_path.display());
            }

            summary.issues.extend(file_issues);
        }

        summary.duration_ms = start.elapsed().as_millis() as u64;
        Ok(summary)
    }

    /// Audits an individual C11 header file.
    pub fn validate_header(&self, path: &Path, issues: &mut Vec<ValidationIssue>) {
        let rel_to_include = match path.strip_prefix(self.repo_root.join("engine/include")) {
            Ok(p) => p.to_string_lossy().to_string(),
            Err(_) => path.to_string_lossy().to_string(),
        };

        let record = self.registry.headers.get(&rel_to_include);
        if record.is_none() {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "REGISTRY".to_string(),
                field: "registration".to_string(),
                severity: IssueSeverity::Warning,
                message: format!("Header '{}' is not registered in subsystems.json", rel_to_include),
            });
        }

        let archetype = record.map(|r| r.archetype).unwrap_or(HeaderArchetype::Operational);
        let stateful = record.map(|r| r.stateful).unwrap_or(false);

        let content = match fs::read_to_string(path) {
            Ok(c) => c,
            Err(e) => {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "I/O".to_string(),
                    field: "read".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Failed to read header file: {}", e),
                });
                return;
            }
        };

        // 1. Invariant 18: Strict ISO C comments only (zero // comments)
        self.validate_invariant_18(path, &content, issues);

        // 2. Block 1: Metadata Block
        self.validate_block_1_metadata(path, &content, issues);

        // 3. Block 2: Opening Block & Include Guards
        self.validate_block_2_opening(path, &content, archetype, issues);

        // 4. Block 3: Body Block
        self.validate_block_3_body(path, &content, archetype, issues);

        // 5. Block 4: Audit Block & Verification Cogs
        self.validate_block_4_audit(path, &content, archetype, issues);

        // 6. Block 5: Closing Block, Dump Cards & Lifecycle
        self.validate_block_5_closing(path, &content, archetype, stateful, issues);
    }

    fn validate_invariant_18(&self, path: &Path, content: &str, issues: &mut Vec<ValidationIssue>) {
        for (line_num, line) in content.lines().enumerate() {
            if let Some(pos) = line.find("//") {
                // Check if the double-slash is preceded by "http:" or "https:"
                let before = &line[..pos];
                if before.ends_with("http:") || before.ends_with("https:") {
                    continue;
                }

                // Check whitelisted URLs
                let is_whitelisted = self.rules.url_whitelists.iter().any(|u| line.contains(u));
                if !is_whitelisted {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "INVARIANT_18".to_string(),
                        field: format!("line_{}", line_num + 1),
                        severity: IssueSeverity::Error,
                        message: format!("Forbidden '//' comment found at line {}: {}", line_num + 1, line.trim()),
                    });
                }
            }
        }
    }

    fn validate_block_1_metadata(&self, path: &Path, content: &str, issues: &mut Vec<ValidationIssue>) {
        if !content.contains("BLOCK 1: METADATA BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 1: METADATA BLOCK' marker".to_string(),
            });
            return;
        }

        // Check mandatory tags
        for tag in &self.rules.mandatory_metadata_tags {
            if !content.contains(tag) {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_1_metadata".to_string(),
                    field: tag.clone(),
                    severity: IssueSeverity::Error,
                    message: format!("Missing required metadata tag '{}'", tag),
                });
            }
        }

        // Check mandatory sections
        for sec in &self.rules.mandatory_metadata_sections {
            if !content.contains(sec) {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_1_metadata".to_string(),
                    field: sec.clone(),
                    severity: IssueSeverity::Error,
                    message: format!("Missing required metadata section '{}'", sec),
                });
            }
        }

        // Check project canonical anchor
        if !content.contains(&self.rules.canonical_project) {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "@project".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing canonical project '{}'", self.rules.canonical_project),
            });
        }

        // Check canonical author
        if !content.contains(&self.rules.canonical_author) {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "@author".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing canonical author '{}'", self.rules.canonical_author),
            });
        }

        // Check canonical repository
        if !content.contains(&self.rules.canonical_repository) {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: "@repository".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing canonical repository '{}'", self.rules.canonical_repository),
            });
        }
    }

    fn validate_block_2_opening(
        &self,
        path: &Path,
        content: &str,
        archetype: HeaderArchetype,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !content.contains("BLOCK 2: OPENING BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_2_opening".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 2: OPENING BLOCK' marker".to_string(),
            });
        }

        // Include guard checks
        let guard_prefix = &self.rules.include_guard_prefix;
        let has_ifndef = content.lines().any(|l| l.starts_with("#ifndef") && l.contains(guard_prefix));
        let has_define = content.lines().any(|l| l.starts_with("#define") && l.contains(guard_prefix));

        if !has_ifndef || !has_define {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_2_opening".to_string(),
                field: "include_guard".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing or invalid include guard with prefix '{}'", guard_prefix),
            });
        }

        // Anti-Domain-Bleed Covenant
        for forbidden in &self.rules.forbidden_domain_prefixes {
            let pattern = format!("#include \"{}", forbidden);
            if content.contains(&pattern) {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "anti_domain_bleed".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!("Domain bleed detected: engine header includes game domain path '{}'", forbidden),
                });
            }
        }

        // Archetype Sub-Assemblies
        if archetype == HeaderArchetype::Operational || archetype == HeaderArchetype::QuarantinedFallback {
            if !content.contains("Sub-Assembly 2.1") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.1".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.1' header in opening block".to_string(),
                });
            }
            if !content.contains("Sub-Assembly 2.2") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.2".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.2' header in opening block".to_string(),
                });
            }
            if !content.contains("Sub-Assembly 2.3") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.3".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.3' header in opening block".to_string(),
                });
            }
            if !content.contains("Sub-Assembly 2.4") {
                issues.push(ValidationIssue {
                    file: path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "Sub-Assembly 2.4".to_string(),
                    severity: IssueSeverity::Warning,
                    message: "Missing 'Sub-Assembly 2.4' header in opening block".to_string(),
                });
            }
        }
    }

    fn validate_block_3_body(
        &self,
        path: &Path,
        content: &str,
        archetype: HeaderArchetype,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !content.contains("BLOCK 3: BODY BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_3_body".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 3: BODY BLOCK' marker".to_string(),
            });
        }

        match archetype {
            HeaderArchetype::Operational => {
                if !content.contains("Sub-Assembly 3.1") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_3_body".to_string(),
                        field: "Sub-Assembly 3.1".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Missing 'Sub-Assembly 3.1' in body block".to_string(),
                    });
                }
                if !content.contains("Sub-Assembly 3.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_3_body".to_string(),
                        field: "Sub-Assembly 3.2".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Missing 'Sub-Assembly 3.2' in body block".to_string(),
                    });
                }

                // Check Dual Audience Documentation Covenant
                for sec in &self.rules.dual_audience_sections {
                    if !content.contains(sec) {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_3_body".to_string(),
                            field: "dual_audience_documentation".to_string(),
                            severity: IssueSeverity::Warning,
                            message: format!("Missing dual-audience section '{}'", sec),
                        });
                    }
                }
            }
            HeaderArchetype::QuarantinedFallback => {
                if !content.contains("Sub-Assembly 3.1") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_3_body".to_string(),
                        field: "Sub-Assembly 3.1".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Missing 'Sub-Assembly 3.1' in fallback body block".to_string(),
                    });
                }
            }
            HeaderArchetype::PureTypes => {
                if !content.contains("purely static structures") && !content.contains("pure type") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_3_body".to_string(),
                        field: "statutory_text".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Pure type header missing statutory body note".to_string(),
                    });
                }
            }
            HeaderArchetype::FacadeUmbrella => {
                let has_facade_note = content.contains("all functional APIs are declared in their respective domain headers")
                    || (content.contains("functional") && content.contains("declared in their respective domain headers"));
                if !has_facade_note {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_3_body".to_string(),
                        field: "facade_text".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Facade umbrella missing statutory body cross-reference note".to_string(),
                    });
                }
            }
        }
    }

    fn validate_block_4_audit(
        &self,
        path: &Path,
        content: &str,
        archetype: HeaderArchetype,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !content.contains("BLOCK 4: AUDIT PROCEDURES BLOCK") && !content.contains("BLOCK 4: AUDIT BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_4_audit".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 4: AUDIT PROCEDURES BLOCK' marker".to_string(),
            });
        }

        match archetype {
            HeaderArchetype::Operational | HeaderArchetype::QuarantinedFallback => {
                if !content.contains("Sub-Assembly 4.1") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "Sub-Assembly 4.1".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 4.1' in audit block".to_string(),
                    });
                }
                if !content.contains("Sub-Assembly 4.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "Sub-Assembly 4.2".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 4.2' in audit block".to_string(),
                    });
                }

                // Verify audit and preflight cog declarations
                let has_audit = content.contains(&self.rules.audit_pattern);
                let has_preflight = content.contains(&self.rules.preflight_pattern);

                if !has_audit {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "audit_cog".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Missing operational audit function matching '{}'", self.rules.audit_pattern),
                    });
                }
                if !has_preflight {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "preflight_cog".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Missing operational preflight function matching '{}'", self.rules.preflight_pattern),
                    });
                }
            }
            HeaderArchetype::PureTypes => {
                if !content.contains("Functional audits") && !content.contains("declared in") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "statutory_text".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Pure type header missing statutory audit cross-reference note".to_string(),
                    });
                }
            }
            HeaderArchetype::FacadeUmbrella => {
                if !content.contains("Sub-Assembly 4.1") || !content.contains("Sub-Assembly 4.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_4_audit".to_string(),
                        field: "cross_references".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Facade umbrella missing Sub-Assembly 4.1 or 4.2 cross-references".to_string(),
                    });
                }
            }
        }
    }

    fn validate_block_5_closing(
        &self,
        path: &Path,
        content: &str,
        archetype: HeaderArchetype,
        stateful: bool,
        issues: &mut Vec<ValidationIssue>,
    ) {
        if !content.contains("BLOCK 5: CLOSING BLOCK") && !content.contains("BLOCK 5: CLOSING & LIFECYCLE BLOCK") {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_5_closing".to_string(),
                field: "header".to_string(),
                severity: IssueSeverity::Error,
                message: "Missing 'BLOCK 5: CLOSING BLOCK' marker".to_string(),
            });
        }

        match archetype {
            HeaderArchetype::Operational | HeaderArchetype::QuarantinedFallback => {
                if !content.contains("Sub-Assembly 5.1") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "Sub-Assembly 5.1".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 5.1' in closing block".to_string(),
                    });
                }
                if !content.contains("Sub-Assembly 5.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "Sub-Assembly 5.2".to_string(),
                        severity: IssueSeverity::Error,
                        message: "Missing 'Sub-Assembly 5.2' in closing block".to_string(),
                    });
                }

                // Verify dump card declaration
                if !content.contains(&self.rules.dump_card_suffix) {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "dump_card".to_string(),
                        severity: IssueSeverity::Error,
                        message: format!("Missing diagnostic dump card declaration matching '{}'", self.rules.dump_card_suffix),
                    });
                }

                // Verify Sub-Assembly 5.2 lifecycle or statutory stateless exemption
                if stateful {
                    let has_init = content.contains("_init");
                    let has_shutdown = content.contains("_shutdown") || content.contains("_teardown");
                    if !has_init || !has_shutdown {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_5_closing".to_string(),
                            field: "lifecycle".to_string(),
                            severity: IssueSeverity::Error,
                            message: "Stateful subsystem must declare both _init and _shutdown/_teardown procedures".to_string(),
                        });
                    }
                } else {
                    let has_exemption = content.contains("Stateless module")
                        || content.contains("Stateless foundation")
                        || content.contains("pure stack-allocated execution")
                        || content.contains("Pure procedural loop")
                        || content.contains(&self.rules.stateless_exemption_literal);
                    if !has_exemption {
                        issues.push(ValidationIssue {
                            file: path.to_path_buf(),
                            block: "block_5_closing".to_string(),
                            field: "stateless_exemption".to_string(),
                            severity: IssueSeverity::Error,
                            message: "Stateless subsystem must declare statutory exemption note in Sub-Assembly 5.2".to_string(),
                        });
                    }
                }
            }
            HeaderArchetype::PureTypes => {
                if !content.contains("Pure type definition header") && !content.contains("declared in") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "statutory_text".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Pure type header missing statutory closing cross-reference note".to_string(),
                    });
                }
            }
            HeaderArchetype::FacadeUmbrella => {
                if !content.contains("Sub-Assembly 5.1") || !content.contains("Sub-Assembly 5.2") {
                    issues.push(ValidationIssue {
                        file: path.to_path_buf(),
                        block: "block_5_closing".to_string(),
                        field: "cross_references".to_string(),
                        severity: IssueSeverity::Warning,
                        message: "Facade umbrella missing Sub-Assembly 5.1 or 5.2 cross-references".to_string(),
                    });
                }
            }
        }

        // Validate final #endif comment matches include guard
        let guard_prefix = &self.rules.include_guard_prefix;
        let last_lines: Vec<&str> = content.lines().rev().take(5).collect();
        let has_matching_endif = last_lines.iter().any(|l| l.starts_with("#endif") && l.contains(guard_prefix));

        if !has_matching_endif {
            issues.push(ValidationIssue {
                file: path.to_path_buf(),
                block: "block_5_closing".to_string(),
                field: "endif".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing matching #endif comment with prefix '{}'", guard_prefix),
            });
        }
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK / TASK RUNNER IMPLEMENTATION
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

/// Task runner adapter integrating C11 header validation.
pub struct HeaderValidationTask;

impl TaskRunner for HeaderValidationTask {
    fn name(&self) -> &'static str {
        "header_validator"
    }

    fn description(&self) -> &'static str {
        "Audits C11 engine headers across engine/include/ for OmniCode 5-block compliance"
    }

    fn preflight(&self, ctx: &RunnerContext) -> Result<(), String> {
        let include_dir = ctx.repo_root.join("engine/include");
        if !include_dir.exists() {
            return Err(format!("Engine include directory not found at {:?}", include_dir));
        }
        Ok(())
    }

    fn run(&self, ctx: &RunnerContext) -> Result<TaskTelemetry, String> {
        let registry = super::config::load_subsystems_registry(&ctx.repo_root)?;
        let rules = super::config::load_header_rules_config(&ctx.repo_root);

        let validator = HeaderValidator::new(ctx.repo_root.clone(), registry, rules, ctx.verbose);
        let summary = validator.run_audit()?;

        if summary.is_success(false) {
            Ok(TaskTelemetry::success(
                self.name(),
                summary.duration_ms,
                &format!(
                    "Audited {} C11 headers cleanly (0 errors, {} warnings)",
                    summary.total_files,
                    summary.warning_count()
                ),
            ))
        } else {
            Ok(TaskTelemetry::failure(
                self.name(),
                summary.duration_ms,
                &format!(
                    "C11 header validation failed: {} errors across {} files",
                    summary.error_count(),
                    summary.failed_files
                ),
            ))
        }
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS
// Stratum: Developer Tooling | Machine: tools.validator.c_header
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_all_engine_headers_pass_validation() {
        let candidate_roots = [Path::new("."), Path::new("..")];
        let repo_root = candidate_roots
            .iter()
            .find(|p| p.join("engine/include").exists())
            .map(|p| p.canonicalize().unwrap_or_else(|_| p.to_path_buf()))
            .unwrap_or_else(|| PathBuf::from("."));

        if !repo_root.join("engine/include").exists() {
            return;
        }

        let registry = crate::validator::config::load_subsystems_registry(&repo_root)
            .expect("Failed to load subsystems registry");
        let rules = crate::validator::config::load_header_rules_config(&repo_root);

        let validator = HeaderValidator::new(repo_root, registry, rules, false);
        let summary = validator.run_audit().expect("Audit run failed");

        assert!(
            summary.is_success(false),
            "C11 Header validation failed with {} errors across {} files",
            summary.error_count(),
            summary.failed_files
        );
        assert_eq!(summary.total_files, 41, "Expected 41 engine headers to be audited");
    }
}
