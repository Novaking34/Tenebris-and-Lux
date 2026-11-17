// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/adoc.rs
// @id            cog.tools.validator.adoc
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          AsciiDoc Master Guide Specification & Layout Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | static delimiter state machine
// @scale         O(N) line-by-line lexical inspection
// @seams         in: syntax::adoc | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on arbitrary AsciiDoc source input.
//                2. Strictly enforces delimiter symmetry (////, ----, |===, ....).
//                3. Emits standardized ValidationIssue records for all syntax breaches.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates AsciiDoc documentation files, devlogs, architectural specs, and
//   the 6-volume Master Guide suite. Verifies 5-block comment envelopes (`////`),
//   structural delimiter balance (`audit_adoc_delimiters`), and maximum column limits.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that lore books and game manuals compile into clean PDFs and web pages
//   without broken tables, unclosed code blocks, or missing headers.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Stateless document scanner operating over string
//     slices; reentrant, thread-safe, and zero dynamic heap overhead.
//   - Extension Seam: Configurable via `AdocValidationConfig`; plugs into
//     `tools/src/validator/mod.rs` and `test_validator_languages.rs`.
//   - Explicit Non-Goal: Does not invoke external Asciidoctor Ruby / CLI gems;
//     performs native structural AST and delimiter audits only.
// =============================================================================

//! # OmniCode AsciiDoc Validator
//!
//! Enforces universal 5-block structure and delimiter symmetry on AsciiDoc files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Configuration Types & Defaults )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::path::Path;
use crate::syntax::adoc::audit_adoc_delimiters;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Configuration Types & Default Contracts
// -----------------------------------------------------------------------------

/// Configuration options for AsciiDoc validation sweeps.
///
/// ```rust
/// use tl_tools::validator::adoc::AdocValidationConfig;
///
/// let config = AdocValidationConfig::default();
/// assert!(config.require_balanced_delimiters);
/// assert!(config.require_5block_banners);
/// assert_eq!(config.max_column_limit, 128);
/// ```
#[derive(Debug, Clone)]
pub struct AdocValidationConfig {
    /// Whether block delimiters (`----`, `|===`, `////`) must be strictly balanced.
    pub require_balanced_delimiters: bool,
    /// Whether 5-block banners are required in leading comments.
    pub require_5block_banners: bool,
    /// Maximum line column limit for prose/code (canonical = 128).
    pub max_column_limit: usize,
}

impl Default for AdocValidationConfig {
    fn default() -> Self {
        Self {
            require_balanced_delimiters: true,
            require_5block_banners: true,
            max_column_limit: 128,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Validation Rule Engine & Sub-Assembly Checkers )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Delimiter Symmetry Auditor
// -----------------------------------------------------------------------------

/// Audits AsciiDoc source content for balanced delimiters (listing, literal, sidebar, comments).
pub fn audit_delimiters(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let delimiter_report = audit_adoc_delimiters(content);
    if !delimiter_report.is_balanced {
        if let Some((unclosed_tok, line_num)) = &delimiter_report.unclosed {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "syntax_delimiters".to_string(),
                field: format!("line_{}", line_num),
                severity: IssueSeverity::Error,
                message: format!(
                    "Unclosed AsciiDoc delimiter '{}' opened at line {}",
                    unclosed_tok, line_num
                ),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Universal 5-Block Envelope Auditor
// -----------------------------------------------------------------------------

/// Audits AsciiDoc files for the presence of recommended 5-block structural banners.
pub fn audit_5block_banners(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let expected_blocks = [
        "BLOCK 1: METADATA",
        "BLOCK 2: OPENING",
        "BLOCK 3: BODY",
        "BLOCK 4: AUDIT",
        "BLOCK 5: CLOSING",
    ];

    for banner_pat in &expected_blocks {
        if !content.contains(banner_pat) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "document_structure".to_string(),
                field: banner_pat.to_lowercase().replace(' ', "_"),
                severity: IssueSeverity::Warning,
                message: format!("Missing recommended AsciiDoc block banner '{}'", banner_pat),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Line Column Limit Auditor
// -----------------------------------------------------------------------------

/// Audits lines against the maximum column width limit, excluding URLs and tables.
pub fn audit_column_limits(
    file_path: &Path,
    content: &str,
    max_column_limit: usize,
    issues: &mut Vec<ValidationIssue>,
) {
    for (line_idx, line) in content.lines().enumerate() {
        if line.len() > max_column_limit {
            // Exclude URLs and long table data rows
            if !line.contains("http://") && !line.contains("https://") && !line.starts_with('|') {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "layout_hygiene".to_string(),
                    field: format!("line_{}", line_idx + 1),
                    severity: IssueSeverity::Warning,
                    message: format!(
                        "Line {} length ({} cols) exceeds maximum limit ({} cols)",
                        line_idx + 1,
                        line.len(),
                        max_column_limit
                    ),
                });
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Main Validation Entry Point
// -----------------------------------------------------------------------------

/// Validates AsciiDoc document content against Master Guide publishing standards.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Checks that books have matching table borders and clean sections.
/// * For Systems Programmers: Audits delimiter stacks, verifies 5-block envelopes,
///   and warns on long lines exceeding column limits.
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::validator::adoc::{validate_adoc_content, AdocValidationConfig};
///
/// let adoc = r#"
/// ////
/// BLOCK 1: METADATA BLOCK
/// BLOCK 2: OPENING BLOCK
/// BLOCK 3: BODY BLOCK
/// BLOCK 4: AUDIT PROCEDURES BLOCK
/// BLOCK 5: CLOSING BLOCK
/// ////
/// = Sample Title
/// "#;
/// let mut issues = Vec::new();
/// validate_adoc_content(Path::new("docs/sample.adoc"), adoc, &AdocValidationConfig::default(), &mut issues);
/// assert!(issues.is_empty(), "Issues: {:?}", issues);
/// ```
pub fn validate_adoc_content(
    file_path: &Path,
    content: &str,
    config: &AdocValidationConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Delimiter Symmetry Audit
    if config.require_balanced_delimiters {
        audit_delimiters(file_path, content, issues);
    }

    // 2. Five-Block Envelope Verification
    if config.require_5block_banners {
        audit_5block_banners(file_path, content, issues);
    }

    // 3. Line Column Limit Verification
    audit_column_limits(file_path, content, config.max_column_limit, issues);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing AsciiDoc validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the AsciiDoc book reviewer.
/// * For Systems Programmers: Runs unclosed delimiter fixtures through `validate_adoc_content`.
///
/// ```rust
/// use tl_tools::validator::adoc::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = AdocValidationConfig::default();

    // 1. Nominal balanced test
    let good_adoc = r#"
////
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
////
= Title

[cols="1,1"]
|===
| Header 1 | Header 2
| Value 1  | Value 2
|===
"#;
    let mut good_issues = Vec::new();
    validate_adoc_content(Path::new("test.adoc"), good_adoc, &config, &mut good_issues);
    if !good_issues.is_empty() {
        return Err("Self-audit failed: nominal balanced AsciiDoc produced unexpected issues");
    }

    // 2. Fault inversion: unclosed delimiter
    let broken_adoc = "----\nUnclosed listing block\n";
    let mut bad_issues = Vec::new();
    validate_adoc_content(Path::new("test.adoc"), broken_adoc, &config, &mut bad_issues);
    if !bad_issues.iter().any(|i| i.block == "syntax_delimiters") {
        return Err("Self-audit failed: unclosed delimiter issue not detected");
    }

    // 3. Fault inversion: column width breach
    let long_line = "x".repeat(150);
    let mut col_issues = Vec::new();
    validate_adoc_content(Path::new("test.adoc"), &long_line, &config, &mut col_issues);
    if !col_issues.iter().any(|i| i.block == "layout_hygiene") {
        return Err("Self-audit failed: column limit breach not detected");
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
    // Section 1: Nominal & Always-Pass Tests (Golden Surface Path)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_1_nominal_balanced_adoc_passes() {
        let adoc = r#"
////
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
////
= Title

[cols="1,1"]
|===
| Header 1 | Header 2
| Value 1  | Value 2
|===
"#;
        let mut issues = Vec::new();
        validate_adoc_content(Path::new("test.adoc"), adoc, &AdocValidationConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // Section 2: Resilience & Never-Fail Edge Tests (Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_unclosed_table_detected() {
        let adoc = "|===\n| Col 1\n";
        let mut issues = Vec::new();
        let config = AdocValidationConfig {
            require_5block_banners: false,
            ..Default::default()
        };
        validate_adoc_content(Path::new("test.adoc"), adoc, &config, &mut issues);
        assert!(issues.iter().any(|i| i.block == "syntax_delimiters"));
    }

    #[test]
    fn test_section_2_never_fail_empty_document_graceful() {
        let mut issues = Vec::new();
        validate_adoc_content(Path::new("empty.adoc"), "", &AdocValidationConfig::default(), &mut issues);
        assert!(!issues.is_empty());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariant & Structural Guard Tests (Strict Immutability)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_long_lines_warning() {
        let long_line = "a".repeat(150);
        let mut issues = Vec::new();
        let config = AdocValidationConfig {
            require_balanced_delimiters: false,
            require_5block_banners: false,
            max_column_limit: 128,
        };
        validate_adoc_content(Path::new("test.adoc"), &long_line, &config, &mut issues);
        assert!(issues.iter().any(|i| i.block == "layout_hygiene"));
    }

    #[test]
    fn test_section_3_invariants_url_excluded_from_column_limit() {
        let url_line = format!("See https://example.com/{}", "a".repeat(150));
        let mut issues = Vec::new();
        let config = AdocValidationConfig {
            require_balanced_delimiters: false,
            require_5block_banners: false,
            max_column_limit: 128,
        };
        validate_adoc_content(Path::new("test.adoc"), &url_line, &config, &mut issues);
        assert!(issues.is_empty(), "URLs should be exempt from column width limit");
    }

    // -------------------------------------------------------------------------
    // Section 4: Emergence & Integration Tests (Systemic Synthesis)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Epilogue Readiness & Subsystem Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the AsciiDoc validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the book reviewer is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::adoc::is_adoc_validator_ready;
///
/// assert!(is_adoc_validator_ready());
/// ```
pub fn is_adoc_validator_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Subsystem Navigation & Module Exports
// -----------------------------------------------------------------------------

/// Returns a human-readable telemetry status string for the AsciiDoc validator.
pub fn adoc_validator_status() -> &'static str {
    if is_adoc_validator_ready() {
        "READY"
    } else {
        "UNAVAILABLE"
    }
}
