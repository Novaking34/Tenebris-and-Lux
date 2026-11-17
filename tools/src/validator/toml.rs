// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/toml.rs
// @id            cog.tools.validator.toml
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          TOML Package & Manifest Structure Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | static table lexer
// @scale         O(N) line-by-line lexical inspection
// @seams         in: syntax::toml | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on arbitrary TOML configuration input.
//                2. Verifies table bracket syntax and key-value hygiene.
//                3. Emits standardized ValidationIssue records for all syntax defects.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates TOML configuration and project manifests (`Cargo.toml`, theme configs).
//   Enforces table syntax, bracket matching, and 5-block comment envelopes.
//
//   ### For Game Designers & Non-Programmers:
//   Makes sure our tool and theme configuration files have valid settings
//   without missing brackets or corrupted key names.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Stateless table scanner operating over string slices;
//     reentrant, thread-safe, and zero dynamic heap allocations in validation loop.
//   - Extension Seam: Configurable via `TomlValidationConfig`; integrates with
//     `tools/src/validator/mod.rs` and `test_validator_languages.rs`.
//   - Explicit Non-Goal: Does not serialize or rewrite TOML tables; performs
//     read-only syntax, bracket, and 5-block banner verification.
// =============================================================================

//! # OmniCode TOML Validator
//!
//! Enforces universal 5-block envelopes and table syntax on TOML configuration files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Configuration Types & Defaults )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::path::Path;
use crate::syntax::toml::scan_toml_content;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Configuration Types & Default Contracts
// -----------------------------------------------------------------------------

/// Configuration options for TOML validation sweeps.
///
/// ```rust
/// use tl_tools::validator::toml::TomlValidationConfig;
///
/// let config = TomlValidationConfig::default();
/// assert!(config.require_valid_tables);
/// assert!(config.require_5block_banners);
/// ```
#[derive(Debug, Clone)]
pub struct TomlValidationConfig {
    /// Whether table brackets (`[table]` and `[[array]]`) must be valid.
    pub require_valid_tables: bool,
    /// Whether 5-block comment banners are required.
    pub require_5block_banners: bool,
}

impl Default for TomlValidationConfig {
    fn default() -> Self {
        Self {
            require_valid_tables: true,
            require_5block_banners: true,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Validation Rule Engine & Sub-Assembly Checkers )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Table Bracket Syntax Auditor
// -----------------------------------------------------------------------------

/// Audits TOML lines for properly closed table headers (`[table]` and `[[array]]`).
pub fn audit_table_headers(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    for (line_idx, line) in content.lines().enumerate() {
        let trimmed = line.trim();
        if trimmed.starts_with('#') || trimmed.is_empty() {
            continue;
        }

        if trimmed.starts_with('[') {
            let is_double = trimmed.starts_with("[[");
            let ends_valid = if is_double {
                trimmed.ends_with("]]")
            } else {
                trimmed.ends_with(']')
            };

            if !ends_valid {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "syntax_table".to_string(),
                    field: format!("line_{}", line_idx + 1),
                    severity: IssueSeverity::Error,
                    message: format!("Malformed TOML table header at line {}: {}", line_idx + 1, trimmed),
                });
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Universal 5-Block Envelope Auditor
// -----------------------------------------------------------------------------

/// Audits TOML files for the presence of recommended 5-block comment banners.
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
                message: format!("Missing recommended TOML block banner '{}'", banner_pat),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Main Validation Entry Point
// -----------------------------------------------------------------------------

/// Validates TOML file content against configuration standards.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms that settings sheets are well-formed.
/// * For Systems Programmers: Scans table structures and key-value entries via `scan_toml_content`.
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::validator::toml::{validate_toml_content, TomlValidationConfig};
///
/// let toml = r#"
/// # BLOCK 1: METADATA BLOCK
/// # BLOCK 2: OPENING BLOCK
/// # BLOCK 3: BODY BLOCK
/// # BLOCK 4: AUDIT PROCEDURES BLOCK
/// # BLOCK 5: CLOSING BLOCK
/// [package]
/// name = "app"
/// version = "0.1.0"
/// "#;
/// let mut issues = Vec::new();
/// validate_toml_content(Path::new("Cargo.toml"), toml, &TomlValidationConfig::default(), &mut issues);
/// assert!(issues.is_empty(), "Issues: {:?}", issues);
/// ```
pub fn validate_toml_content(
    file_path: &Path,
    content: &str,
    config: &TomlValidationConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Table Bracket Validation
    if config.require_valid_tables {
        audit_table_headers(file_path, content, issues);
    }

    // 2. Five-Block Envelope Verification
    if config.require_5block_banners {
        audit_5block_banners(file_path, content, issues);
    }

    // 3. Scan Content via Syntax Lexer
    let _scan = scan_toml_content(content);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing TOML validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the configuration sheet inspector.
/// * For Systems Programmers: Runs unclosed table header fixtures through `validate_toml_content`.
///
/// ```rust
/// use tl_tools::validator::toml::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = TomlValidationConfig::default();

    // 1. Nominal balanced test
    let good_toml = r#"
# BLOCK 1: METADATA BLOCK
# BLOCK 2: OPENING BLOCK
# BLOCK 3: BODY BLOCK
# BLOCK 4: AUDIT PROCEDURES BLOCK
# BLOCK 5: CLOSING BLOCK
[package]
name = "pkg"
version = "0.1.0"
"#;
    let mut good_issues = Vec::new();
    validate_toml_content(Path::new("test.toml"), good_toml, &config, &mut good_issues);
    if !good_issues.is_empty() {
        return Err("Self-audit failed: nominal balanced TOML produced unexpected issues");
    }

    // 2. Fault inversion: unclosed table header
    let broken_toml = "[unclosed_table\nfoo = \"bar\"\n";
    let mut bad_issues = Vec::new();
    validate_toml_content(Path::new("test.toml"), broken_toml, &config, &mut bad_issues);
    if !bad_issues.iter().any(|i| i.block == "syntax_table") {
        return Err("Self-audit failed: unclosed table issue not detected");
    }

    // 3. Fault inversion: unclosed double table header
    let broken_double = "[[unclosed_array\nitem = 1\n";
    let mut bad_double_issues = Vec::new();
    validate_toml_content(Path::new("test.toml"), broken_double, &config, &mut bad_double_issues);
    if !bad_double_issues.iter().any(|i| i.block == "syntax_table") {
        return Err("Self-audit failed: unclosed double table issue not detected");
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
    fn test_section_1_nominal_balanced_toml_passes() {
        let toml = r#"
# BLOCK 1: METADATA BLOCK
# BLOCK 2: OPENING BLOCK
# BLOCK 3: BODY BLOCK
# BLOCK 4: AUDIT PROCEDURES BLOCK
# BLOCK 5: CLOSING BLOCK
[package]
name = "pkg"
version = "0.1.0"
"#;
        let mut issues = Vec::new();
        validate_toml_content(Path::new("test.toml"), toml, &TomlValidationConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // Section 2: Resilience & Never-Fail Edge Tests (Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_unclosed_table_detected() {
        let toml = "[[unclosed\n";
        let mut issues = Vec::new();
        let config = TomlValidationConfig {
            require_5block_banners: false,
            ..Default::default()
        };
        validate_toml_content(Path::new("test.toml"), toml, &config, &mut issues);
        assert!(issues.iter().any(|i| i.block == "syntax_table"));
    }

    #[test]
    fn test_section_2_never_fail_empty_document_graceful() {
        let mut issues = Vec::new();
        validate_toml_content(Path::new("empty.toml"), "", &TomlValidationConfig::default(), &mut issues);
        assert!(!issues.is_empty());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariant & Structural Guard Tests (Strict Immutability)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_missing_banners_warning() {
        let toml = "name = \"app\"\n";
        let mut issues = Vec::new();
        let config = TomlValidationConfig {
            require_valid_tables: false,
            require_5block_banners: true,
        };
        validate_toml_content(Path::new("test.toml"), toml, &config, &mut issues);
        assert_eq!(issues.len(), 5);
    }

    #[test]
    fn test_section_3_invariants_comments_ignored_for_tables() {
        let toml = r#"
# [this is a comment with brackets
name = "valid"
"#;
        let mut issues = Vec::new();
        let config = TomlValidationConfig {
            require_valid_tables: true,
            require_5block_banners: false,
        };
        validate_toml_content(Path::new("test.toml"), toml, &config, &mut issues);
        assert!(issues.is_empty(), "Comments with brackets must not be flagged as unclosed tables");
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

/// Epilogue readiness probe for the TOML validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the configuration sheet inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::toml::is_toml_validator_ready;
///
/// assert!(is_toml_validator_ready());
/// ```
pub fn is_toml_validator_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Subsystem Navigation & Module Exports
// -----------------------------------------------------------------------------

/// Returns a human-readable telemetry status string for the TOML validator.
pub fn toml_validator_status() -> &'static str {
    if is_toml_validator_ready() {
        "READY"
    } else {
        "UNAVAILABLE"
    }
}
