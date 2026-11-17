// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/report.rs
// @id            cog.tools.validator.report
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     pure_types | diagnostic_reporter
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Diagnostic Reporting, Formatting & Verification Telemetry
// @maturity      Rev 5 Baseline | Standardized Diagnostic Summaries
// @chassis       bounded-heap | Developer tooling memory chassis; bounded vectors
// @scale         O(N) across accumulated validation findings
// @seams         in: ValidationIssue | out: ASCII terminal & JSON reports
// @invariants    1. Sum of passed and failed files never exceeds total files scanned.
//                2. Zero unhandled panic states during terminal table emission.
//                3. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Standardized diagnostic reporting and ASCII summary formatting for the
//   OmniCode validator suite. Captures severity levels, per-file issue records,
//   and cumulative verification metrics.
//
//   ### For Game Designers & Non-Programmers:
//   Formats easy-to-read audit scorecards in your terminal. When running checks
//   across game data or engine headers, this report shows exactly how many
//   files passed, how many had issues, and gives clear line-by-line warnings.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Formatter operates statelessly on summary structures.
//   - Extension Seam: Can be consumed by CLI printers, JSON exporters, or CI bridges.
//   - Explicit Non-Goal: Does not perform validation logic; formats results only.
// =============================================================================

//! # Validator Reporting & Diagnostic Tables
//!
//! Provides structured issue tracking and clean ASCII diagnostic tables.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::PathBuf;

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Formatting Constants
// -----------------------------------------------------------------------------

/// Canonical table width for ASCII terminal summaries.
pub const REPORT_TABLE_WIDTH: usize = 80;

/// Default domain label used when printing data validation reports.
pub const DEFAULT_DATA_DOMAIN_LABEL: &str = "system";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Report Structures
// -----------------------------------------------------------------------------

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
///
/// # Examples
///
/// ```rust
/// use std::path::PathBuf;
/// use tl_tools::validator::report::{IssueSeverity, ValidationIssue};
///
/// let issue = ValidationIssue {
///     file: PathBuf::from("engine/include/core/manifest.h"),
///     block: "block_1".to_string(),
///     field: "invariants".to_string(),
///     severity: IssueSeverity::Warning,
///     message: "Test warning".to_string(),
/// };
/// assert_eq!(issue.severity, IssueSeverity::Warning);
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ValidationIssue {
    /// Target file where the issue was discovered.
    pub file: PathBuf,
    /// 5-block section identifier (e.g. "block_1", "block_2").
    pub block: String,
    /// Defective field or tag key (e.g. "id", "sub_assembly").
    pub field: String,
    /// Severity classification.
    pub severity: IssueSeverity,
    /// Human-readable explanation of the defect.
    pub message: String,
}

/// Cumulative report summarizing validation results across all scanned files.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::report::ValidationSummary;
///
/// let summary = ValidationSummary::default();
/// assert_eq!(summary.total_files, 0);
/// assert!(summary.is_success(false));
/// ```
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct ValidationSummary {
    /// Total number of files evaluated.
    pub total_files: usize,
    /// Number of files that satisfied all validation rules.
    pub passed_files: usize,
    /// Number of files with one or more errors.
    pub failed_files: usize,
    /// List of all discovered diagnostic issues.
    pub issues: Vec<ValidationIssue>,
    /// Wall-clock duration of audit in milliseconds.
    pub duration_ms: u64,
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Invariant Predicates
// -----------------------------------------------------------------------------

/// Evaluates whether a validation summary satisfies basic arithmetic invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::report::{is_valid_summary_counts, ValidationSummary};
///
/// let mut summary = ValidationSummary::default();
/// summary.total_files = 10;
/// summary.passed_files = 8;
/// summary.failed_files = 2;
/// assert!(is_valid_summary_counts(&summary));
/// ```
pub fn is_valid_summary_counts(summary: &ValidationSummary) -> bool {
    summary.passed_files + summary.failed_files <= summary.total_files
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Summary Aggregation, Filtering & ASCII Table Formatting Engine )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Summary Methods & ASCII Output Formatting
// -----------------------------------------------------------------------------

impl ValidationSummary {
    /// Returns true if all files passed without critical errors.
    ///
    /// In strict mode, any warning is also considered a failure.
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

/// Internal assertion confirming diagnostic report formatting properties.
pub fn audit_report_invariants(summary: &ValidationSummary) -> Result<(), &'static str> {
    if !is_valid_summary_counts(summary) {
        return Err("Sum of passed and failed files exceeds total files scanned");
    }
    Ok(())
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
    fn test_section_1_nominal_empty_summary_success() {
        let summary = ValidationSummary::default();
        assert!(summary.is_success(false));
        assert!(summary.is_success(true));
        assert_eq!(summary.error_count(), 0);
        assert_eq!(summary.warning_count(), 0);
    }

    #[test]
    fn test_section_1_nominal_warning_non_strict_success() {
        let mut summary = ValidationSummary {
            total_files: 5,
            passed_files: 5,
            failed_files: 0,
            issues: Vec::new(),
            duration_ms: 10,
        };
        summary.issues.push(ValidationIssue {
            file: PathBuf::from("dummy.json"),
            block: "block_1".to_string(),
            field: "invariants".to_string(),
            severity: IssueSeverity::Warning,
            message: "test warning".to_string(),
        });
        assert!(summary.is_success(false));
        assert!(!summary.is_success(true));
        assert_eq!(summary.warning_count(), 1);
        assert_eq!(summary.error_count(), 0);
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_error_causes_failure() {
        let mut summary = ValidationSummary {
            total_files: 1,
            passed_files: 0,
            failed_files: 1,
            issues: Vec::new(),
            duration_ms: 5,
        };
        summary.issues.push(ValidationIssue {
            file: PathBuf::from("bad.json"),
            block: "block_1".to_string(),
            field: "id".to_string(),
            severity: IssueSeverity::Error,
            message: "missing id".to_string(),
        });
        assert!(!summary.is_success(false));
        assert!(!summary.is_success(true));
        assert_eq!(summary.error_count(), 1);
    }

    #[test]
    fn test_section_2_never_fail_arithmetic_overflow_guard() {
        let summary = ValidationSummary {
            total_files: 5,
            passed_files: 4,
            failed_files: 3, // 4 + 3 > 5 -> invalid
            issues: Vec::new(),
            duration_ms: 0,
        };
        assert!(!is_valid_summary_counts(&summary));
        assert!(audit_report_invariants(&summary).is_err());
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_table_width_constant() {
        assert_eq!(REPORT_TABLE_WIDTH, 80);
        assert!(!DEFAULT_DATA_DOMAIN_LABEL.is_empty());
    }

    #[test]
    fn test_section_3_invariants_severity_display() {
        assert_eq!(format!("{}", IssueSeverity::Error), "ERROR");
        assert_eq!(format!("{}", IssueSeverity::Warning), "WARN ");
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_custom_summary_emission() {
        let mut summary = ValidationSummary {
            total_files: 2,
            passed_files: 1,
            failed_files: 1,
            issues: Vec::new(),
            duration_ms: 42,
        };
        summary.issues.push(ValidationIssue {
            file: PathBuf::from("test/file.h"),
            block: "block_2".to_string(),
            field: "sub_assembly".to_string(),
            severity: IssueSeverity::Error,
            message: "disordered sub-assembly".to_string(),
        });
        assert!(is_valid_summary_counts(&summary));
        // Verify custom printing does not panic
        summary.print_custom_summary("Test Suite Audit", "test/domain");
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Diagnostic preflight self-audit verifying reporting constants and invariants.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::report::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    if REPORT_TABLE_WIDTH != 80 {
        return Err("REPORT_TABLE_WIDTH must be exactly 80 columns");
    }
    if DEFAULT_DATA_DOMAIN_LABEL.is_empty() {
        return Err("DEFAULT_DATA_DOMAIN_LABEL must not be empty");
    }
    let default_summary = ValidationSummary::default();
    audit_report_invariants(&default_summary)?;
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Epilogue predicate affirming that report sub-assembly is operational.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::report::is_report_system_ready;
///
/// assert!(is_report_system_ready());
/// ```
pub fn is_report_system_ready() -> bool {
    self_audit().is_ok()
}
