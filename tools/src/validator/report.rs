// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Invariant Boundary Contracts & System Anchors )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/report.rs
// @id            tools.validator.report
// @layer         Layer 3: Systems Developer Tooling Core Library
// @version       0.1.0
// @interface_rev Rev 1
// @date          2026-11-09
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Standardized diagnostic reporting and ASCII summary formatting for the
//   OmniCode validator suite. Captures severity levels, per-file issue records,
//   and cumulative verification metrics.
// =============================================================================

//! # Validator Reporting & Diagnostic Tables
//!
//! Provides structured issue tracking and clean ASCII diagnostic tables.

// =============================================================================
// BLOCK 2: OPENING BLOCK / TYPES & STRUCTURES
// Stratum: Developer Tooling | Machine: tools.validator.report
// =============================================================================

use std::path::PathBuf;

/// Severity classification of a validation finding.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IssueSeverity {
    /// Invariant breach or missing required 5-block structure (Fails run).
    Error,
    /// Suspicious attribute or sub-optimal specification (Informative).
    Warning,
}

impl std::fmt::Display for IssueSeverity {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            IssueSeverity::Error => write!(f, "ERROR"),
            IssueSeverity::Warning => write!(f, "WARN "),
        }
    }
}

/// Diagnostic finding reported during file or artifact validation.
#[derive(Debug, Clone)]
pub struct ValidationIssue {
    pub file: PathBuf,
    pub block: String,
    pub field: String,
    pub severity: IssueSeverity,
    pub message: String,
}

/// Cumulative report summarizing validation results across all scanned files.
#[derive(Debug, Clone, Default)]
pub struct ValidationSummary {
    pub total_files: usize,
    pub passed_files: usize,
    pub failed_files: usize,
    pub issues: Vec<ValidationIssue>,
    pub duration_ms: u64,
}

// =============================================================================
// BLOCK 3: BODY BLOCK / METHODS & ASCII FORMATTING
// Stratum: Developer Tooling | Machine: tools.validator.report
// =============================================================================

impl ValidationSummary {
    /// Returns true if all files passed without critical errors.
    pub fn is_success(&self, strict: bool) -> bool {
        if strict {
            self.issues.is_empty()
        } else {
            !self.issues.iter().any(|i| i.severity == IssueSeverity::Error)
        }
    }

    /// Number of error-level issues reported.
    pub fn error_count(&self) -> usize {
        self.issues.iter().filter(|i| i.severity == IssueSeverity::Error).count()
    }

    /// Number of warning-level issues reported.
    pub fn warning_count(&self) -> usize {
        self.issues.iter().filter(|i| i.severity == IssueSeverity::Warning).count()
    }

    /// Formats a clean ASCII summary table to stdout for data validation audits.
    pub fn print_summary(&self, domain_label: &str) {
        self.print_custom_summary(
            "Data Catalog & Schema Validation Audit",
            &format!("game/data/{}", domain_label),
        );
    }

    /// Formats a clean ASCII summary table with custom title and target domain label.
    pub fn print_custom_summary(&self, title: &str, target_label: &str) {
        println!("================================================================================");
        println!("       Tenebris and Lux - {}        ", title);
        println!("================================================================================");
        println!("Target Domain:    {}", target_label);
        println!("Files Scanned:    {}", self.total_files);
        println!("Files Passing:    {}", self.passed_files);
        println!("Files Failing:    {}", self.failed_files);
        println!("Errors Reported:  {}", self.error_count());
        println!("Warnings Reported:{}", self.warning_count());
        println!("Elapsed Time:     {} ms", self.duration_ms);
        println!("--------------------------------------------------------------------------------");

        if !self.issues.is_empty() {
            println!("{:<5} {:<16} {:<24} {}", "SEV", "BLOCK", "FIELD", "DIAGNOSTIC");
            println!("--------------------------------------------------------------------------------");
            for issue in &self.issues {
                let file_name = issue
                    .file
                    .file_name()
                    .and_then(|n| n.to_str())
                    .unwrap_or("unknown");
                println!(
                    "[{}] {:<16} {:<24} [{}] {}",
                    issue.severity,
                    issue.block,
                    issue.field,
                    file_name,
                    issue.message
                );
            }
            println!("--------------------------------------------------------------------------------");
        }

        let verdict = if self.is_success(false) {
            "ALL INVARIANTS SATISFIED (PASS)"
        } else {
            "VALIDATION REGRESSIONS DETECTED (FAIL)"
        };
        println!("Verdict:          {}", verdict);
        println!("================================================================================");
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// Stratum: Developer Tooling | Machine: tools.validator.report
// =============================================================================

/// Internal assertion confirming diagnostic report formatting properties.
pub fn audit_report_invariants(summary: &ValidationSummary) -> Result<(), &'static str> {
    if summary.passed_files + summary.failed_files > summary.total_files {
        return Err("Sum of passed and failed files exceeds total files scanned");
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK / TESTS
// Stratum: Developer Tooling | Machine: tools.validator.report
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_report_success_predicate() {
        let mut summary = ValidationSummary {
            total_files: 5,
            passed_files: 5,
            failed_files: 0,
            issues: Vec::new(),
            duration_ms: 10,
        };
        assert!(summary.is_success(false));
        assert!(summary.is_success(true));

        summary.issues.push(ValidationIssue {
            file: PathBuf::from("dummy.json"),
            block: "block_1".to_string(),
            field: "invariants".to_string(),
            severity: IssueSeverity::Warning,
            message: "test warning".to_string(),
        });
        assert!(summary.is_success(false));
        assert!(!summary.is_success(true));

        summary.issues.push(ValidationIssue {
            file: PathBuf::from("dummy.json"),
            block: "block_1".to_string(),
            field: "id".to_string(),
            severity: IssueSeverity::Error,
            message: "test error".to_string(),
        });
        assert!(!summary.is_success(false));
    }
}
