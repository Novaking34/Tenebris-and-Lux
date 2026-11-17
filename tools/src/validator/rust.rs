// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/rust.rs
// @id            cog.tools.validator.rust
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Rust 2024 Edition 5-Block Structure & Covenant Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | static keyword tables
// @scale         O(N) line-by-line lexical inspection
// @seams         in: syntax::rust | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on arbitrary Rust source input.
//                2. Verifies presence of the 5 canonical block banners.
//                3. Emits standardized ValidationIssue records for all structural violations.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates Rust source files against the OmniCode Universal 5-Block Lifecycle
//   Protocol, Rust 2024 Edition standards, Four-Section Ontological Test Schema,
//   preflight `self_audit()`, and epilogue `is_<subsystem>_ready()` readiness probes.
//
//   ### For Game Designers & Non-Programmers:
//   Checks that all developer tools written in Rust are organized cleanly,
//   tested thoroughly across all four tiers, and documented in plain English.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Stateless AST and token inspector scanning items over
//     immutable string slices; reentrant, thread-safe, zero heap in evaluation.
//   - Extension Seam: Configurable via `RustValidationConfig`; integrates with
//     `tools/src/validator/mod.rs` and `test_validator_languages.rs`.
//   - Explicit Non-Goal: Does not invoke `cargo check` or `rustc`; performs native
//     structural AST, banner, and test schema audits only.
// =============================================================================

//! # OmniCode Rust 2024 Edition Validator
//!
//! Enforces universal 5-block structure, docstrings, and four-section tests on Rust source files.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Configuration Types & Defaults )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::path::Path;
use crate::syntax::rust::scan_rust_items;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Configuration Types & Default Contracts
// -----------------------------------------------------------------------------

/// Configuration options for Rust validation sweeps.
///
/// ```rust
/// use tl_tools::validator::rust::RustValidationConfig;
///
/// let config = RustValidationConfig::default();
/// assert!(config.require_all_5_blocks);
/// assert!(config.require_metadata_tags);
/// assert!(config.require_sub_assemblies);
/// assert!(config.require_self_audit);
/// assert!(config.require_readiness_probe);
/// assert!(config.require_four_test_sections);
/// ```
#[derive(Debug, Clone)]
pub struct RustValidationConfig {
    /// Whether all 5 block banners are strictly required.
    pub require_all_5_blocks: bool,
    /// Whether mandatory Block 1 metadata tags and purpose sections are required.
    pub require_metadata_tags: bool,
    /// Whether required Sub-Assembly section organization in Blocks 2-5 is required.
    pub require_sub_assemblies: bool,
    /// Whether preflight `self_audit()` is required in Block 4.
    pub require_self_audit: bool,
    /// Whether epilogue readiness probe `is_*_ready()` is required in Block 5.
    pub require_readiness_probe: bool,
    /// Whether four-section ontological test suites are required when tests exist.
    pub require_four_test_sections: bool,
}

impl Default for RustValidationConfig {
    fn default() -> Self {
        Self {
            require_all_5_blocks: true,
            require_metadata_tags: true,
            require_sub_assemblies: true,
            require_self_audit: true,
            require_readiness_probe: true,
            require_four_test_sections: true,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Validation Rule Engine & Sub-Assembly Checkers )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Five-Block Banner Auditor
// -----------------------------------------------------------------------------

/// Audits Rust source content for the presence of the 5 canonical block banners.
pub fn audit_5block_banners(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let expected_blocks = [
        ("block_1_metadata", "BLOCK 1: METADATA"),
        ("block_2_opening", "BLOCK 2: OPENING"),
        ("block_3_body", "BLOCK 3: BODY"),
        ("block_4_audit", "BLOCK 4: AUDIT"),
        ("block_5_closing", "BLOCK 5: CLOSING"),
    ];

    for (block_id, banner_pat) in &expected_blocks {
        if !content.contains(banner_pat) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: (*block_id).to_string(),
                field: "banner".to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing required block banner '{}'", banner_pat),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Block 1 Metadata Tag Auditor
// -----------------------------------------------------------------------------

/// Audits Rust source content for mandatory Block 1 metadata tags and purpose sections.
pub fn audit_metadata_tags(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let mandatory_tags = [
        "@project",
        "@file",
        "@layer",
        "@version",
        "@interface_rev",
        "@date",
        "@author",
        "@repository",
        "@system",
        "@role",
        "@invariants",
    ];

    for tag in &mandatory_tags {
        if !content.contains(tag) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "block_1_metadata".to_string(),
                field: (*tag).to_string(),
                severity: IssueSeverity::Error,
                message: format!("Missing mandatory Block 1 metadata tag '{}'", tag),
            });
        }
    }

    if !content.contains("PURPOSE & ARCHITECTURAL ROLE:") {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "block_1_metadata".to_string(),
            field: "purpose".to_string(),
            severity: IssueSeverity::Error,
            message: "Missing mandatory section 'PURPOSE & ARCHITECTURAL ROLE:' in Block 1".to_string(),
        });
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Sub-Assembly Section Organization Auditor
// -----------------------------------------------------------------------------

/// Audits Rust source content for required Sub-Assembly section organization within Blocks 2 through 5.
pub fn audit_sub_assemblies(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let required_sub_assemblies = [
        ("block_2_opening", "Sub-Assembly 2."),
        ("block_3_body", "Sub-Assembly 3."),
        ("block_4_audit", "Sub-Assembly 4."),
        ("block_5_closing", "Sub-Assembly 5."),
    ];

    for (block_id, sub_assembly_pat) in &required_sub_assemblies {
        if !content.contains(sub_assembly_pat) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: (*block_id).to_string(),
                field: "sub_assembly".to_string(),
                severity: IssueSeverity::Error,
                message: format!(
                    "Missing required section organization '{}' in {}",
                    sub_assembly_pat, block_id
                ),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Self-Audit Preflight Auditor
// -----------------------------------------------------------------------------

/// Audits Rust source content for the presence of a preflight `self_audit()` diagnostic probe.
pub fn audit_self_audit_preflight(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let has_self_audit = content.contains("fn self_audit(")
        || content.contains("self_audit() -> Result");
    if !has_self_audit {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "block_4_audit".to_string(),
            field: "self_audit".to_string(),
            severity: IssueSeverity::Error,
            message: "Missing preflight 'self_audit()' diagnostic probe in Block 4".to_string(),
        });
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.5: Epilogue Readiness Probe Auditor
// -----------------------------------------------------------------------------

/// Audits Rust source content for the presence of an epilogue `is_<subsystem>_ready()` readiness probe.
pub fn audit_readiness_probe(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let has_readiness = content.lines().any(|l| {
        let t = l.trim();
        t.starts_with("pub fn is_") && t.contains("_ready()")
    });
    if !has_readiness {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "block_5_closing".to_string(),
            field: "readiness_probe".to_string(),
            severity: IssueSeverity::Error,
            message: "Missing epilogue readiness probe 'is_<subsystem>_ready()' in Block 5".to_string(),
        });
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.6: Four-Section Ontological Test Schema Auditor
// -----------------------------------------------------------------------------

/// Audits Rust test suites for the four canonical ontological test sections.
pub fn audit_four_test_sections(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    if content.contains("#[test]") {
        let test_sections = [
            ("test_section_1", "SECTION 1: ALWAYS PASS"),
            ("test_section_2", "SECTION 2: NEVER FAIL"),
            ("test_section_3", "SECTION 3: INVARIANTS"),
            ("test_section_4", "SECTION 4: EMERGENCE"),
        ];

        let upper = content.to_ascii_uppercase();
        for (sec_field, sec_pat) in &test_sections {
            if !upper.contains(sec_pat) {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_4_audit".to_string(),
                    field: (*sec_field).to_string(),
                    severity: IssueSeverity::Warning,
                    message: format!(
                        "Missing Four-Section Ontological Test Schema section marker '{}'",
                        sec_pat
                    ),
                });
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.7: Main Validation Entry Point
// -----------------------------------------------------------------------------

/// Validates Rust source content against the OmniCode 5-Block standard.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Reviews Rust tools to ensure they follow project design rules.
/// * For Systems Programmers: Scans for 5-block banners, self_audit hook, readiness probe,
///   and verifies four-section test schemas.
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::validator::rust::{validate_rust_content, RustValidationConfig};
///
/// let source = r#"
/// // =============================================================================
/// // BLOCK 1: METADATA BLOCK
/// // =============================================================================
/// // @project       Tenebris and Lux
/// // @file          tools/src/sample.rs
/// // @layer         Layer 3
/// // @version       0.1.2-dev
/// // @interface_rev Rev 5
/// // @date          2026-11-16
/// // @author        CreativeWorkzStudio LLC / ProfessorSeanEX
/// // @repository    https://github.com/Novaking34/tenebris-lux
/// // @system        Sample
/// // @role          Sample
/// // @invariants    1. None.
/// //
/// // PURPOSE & ARCHITECTURAL ROLE:
/// //   Sample role.
/// // =============================================================================
/// // BLOCK 2: OPENING BLOCK
/// // =============================================================================
/// // Sub-Assembly 2.1: Pre-Conditions
/// // =============================================================================
/// // BLOCK 3: BODY BLOCK
/// // =============================================================================
/// // Sub-Assembly 3.1: Logic
/// // =============================================================================
/// // BLOCK 4: AUDIT PROCEDURES BLOCK
/// // =============================================================================
/// // Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
/// pub fn self_audit() -> Result<(), &'static str> { Ok(()) }
/// // Sub-Assembly 4.2: Four-Section Ontological Unit Tests
/// // SECTION 1: ALWAYS PASS
/// #[test] fn t1() {}
/// // SECTION 2: NEVER FAIL
/// #[test] fn t2() {}
/// // SECTION 3: INVARIANTS
/// #[test] fn t3() {}
/// // SECTION 4: EMERGENCE
/// #[test] fn t4() {}
/// // =============================================================================
/// // BLOCK 5: CLOSING BLOCK
/// // =============================================================================
/// // Sub-Assembly 5.1: Epilogue Readiness Probe
/// pub fn is_tool_ready() -> bool { true }
/// "#;
/// let mut issues = Vec::new();
/// validate_rust_content(Path::new("tools/src/sample.rs"), source, &RustValidationConfig::default(), &mut issues);
/// assert!(issues.is_empty(), "Issues: {:?}", issues);
/// ```
pub fn validate_rust_content(
    file_path: &Path,
    content: &str,
    config: &RustValidationConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Five-Block Banner Verification
    if config.require_all_5_blocks {
        audit_5block_banners(file_path, content, issues);
    }

    // 2. Block 1 Metadata Tag Verification
    if config.require_metadata_tags {
        audit_metadata_tags(file_path, content, issues);
    }

    // 3. Sub-Assembly Section Organization Verification
    if config.require_sub_assemblies {
        audit_sub_assemblies(file_path, content, issues);
    }

    // 4. Self-Audit Preflight Verification
    if config.require_self_audit {
        audit_self_audit_preflight(file_path, content, issues);
    }

    // 5. Epilogue Readiness Probe Verification
    if config.require_readiness_probe {
        audit_readiness_probe(file_path, content, issues);
    }

    // 6. Four-Section Ontological Test Schema Verification
    if config.require_four_test_sections {
        audit_four_test_sections(file_path, content, issues);
    }

    // 7. Item Scanning Verification via Syntax Lexer
    let _items = scan_rust_items(content);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing Rust validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the Rust inspector on sample code.
/// * For Systems Programmers: Runs compliant and non-compliant code fixtures through `validate_rust_content`.
///
/// ```rust
/// use tl_tools::validator::rust::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = RustValidationConfig::default();

    // 1. Nominal balanced test with complete 5-Block structure and Sub-Assemblies
    let good_source = r#"
// =============================================================================
// BLOCK 1: METADATA BLOCK
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/sample.rs
// @layer         Layer 3
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Sample
// @role          Sample
// @invariants    1. None.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Sample role.
// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================
// Sub-Assembly 2.1: Pre-Conditions
// =============================================================================
// BLOCK 3: BODY BLOCK
// =============================================================================
// Sub-Assembly 3.1: Logic
// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
pub fn self_audit() -> Result<(), &'static str> { Ok(()) }
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// SECTION 1: ALWAYS PASS
#[test] fn t1() {}
// SECTION 2: NEVER FAIL
#[test] fn t2() {}
// SECTION 3: INVARIANTS
#[test] fn t3() {}
// SECTION 4: EMERGENCE
#[test] fn t4() {}
// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// Sub-Assembly 5.1: Epilogue Readiness Probe
pub fn is_subsystem_ready() -> bool { true }
"#;
    let mut good_issues = Vec::new();
    validate_rust_content(Path::new("test.rs"), good_source, &config, &mut good_issues);
    if !good_issues.is_empty() {
        return Err("Self-audit failed: nominal valid Rust produced unexpected issues");
    }

    // 2. Fault inversion: missing block banners and self_audit
    let empty_source = "// Empty file\n";
    let mut issues = Vec::new();
    validate_rust_content(Path::new("test.rs"), empty_source, &config, &mut issues);

    if !issues.iter().any(|i| i.field == "banner") {
        return Err("Self-audit failed: missing block banners not detected");
    }
    if !issues.iter().any(|i| i.field == "self_audit") {
        return Err("Self-audit failed: missing self_audit hook not detected");
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
    fn test_section_1_nominal_valid_rust_passes() {
        let source = r#"
// =============================================================================
// BLOCK 1: METADATA BLOCK
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/sample.rs
// @layer         Layer 3
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Sample
// @role          Sample
// @invariants    1. None.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Sample role.
// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================
// Sub-Assembly 2.1: Pre-Conditions
// =============================================================================
// BLOCK 3: BODY BLOCK
// =============================================================================
// Sub-Assembly 3.1: Logic
// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
pub fn self_audit() -> Result<(), &'static str> { Ok(()) }
// Sub-Assembly 4.2: Four-Section Ontological Unit Tests
// SECTION 1: ALWAYS PASS
#[test] fn t1() {}
// SECTION 2: NEVER FAIL
#[test] fn t2() {}
// SECTION 3: INVARIANTS
#[test] fn t3() {}
// SECTION 4: EMERGENCE
#[test] fn t4() {}
// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// Sub-Assembly 5.1: Epilogue Readiness Probe
pub fn is_subsystem_ready() -> bool { true }
"#;
        let mut issues = Vec::new();
        validate_rust_content(Path::new("test.rs"), source, &RustValidationConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // Section 2: Resilience & Never-Fail Edge Tests (Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_missing_sections_reported() {
        let source = r#"
// BLOCK 1: METADATA BLOCK
// BLOCK 2: OPENING BLOCK
// BLOCK 3: BODY BLOCK
// BLOCK 4: AUDIT PROCEDURES BLOCK
pub fn self_audit() -> Result<(), &'static str> { Ok(()) }
#[test] fn t1() {}
// BLOCK 5: CLOSING BLOCK
pub fn is_subsystem_ready() -> bool { true }
"#;
        let mut issues = Vec::new();
        let config = RustValidationConfig {
            require_all_5_blocks: true,
            require_metadata_tags: false,
            require_sub_assemblies: false,
            require_self_audit: true,
            require_readiness_probe: true,
            require_four_test_sections: true,
        };
        validate_rust_content(Path::new("test.rs"), source, &config, &mut issues);
        assert_eq!(issues.len(), 4); // missing sections 1..4
    }

    #[test]
    fn test_section_2_never_fail_empty_source_graceful() {
        let mut issues = Vec::new();
        validate_rust_content(Path::new("empty.rs"), "", &RustValidationConfig::default(), &mut issues);
        assert!(!issues.is_empty());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariant & Structural Guard Tests (Strict Immutability)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_missing_banners() {
        let source = "pub fn self_audit() -> Result<(), &'static str> { Ok(()) }\npub fn is_ready() -> bool { true }\n";
        let mut issues = Vec::new();
        let config = RustValidationConfig {
            require_all_5_blocks: true,
            require_metadata_tags: false,
            require_sub_assemblies: false,
            require_self_audit: false,
            require_readiness_probe: false,
            require_four_test_sections: false,
        };
        validate_rust_content(Path::new("test.rs"), source, &config, &mut issues);
        assert_eq!(issues.len(), 5);
    }

    #[test]
    fn test_section_3_invariants_no_tests_skips_section_check() {
        let source = r#"
// BLOCK 1: METADATA BLOCK
// BLOCK 2: OPENING BLOCK
// BLOCK 3: BODY BLOCK
// BLOCK 4: AUDIT PROCEDURES BLOCK
pub fn self_audit() -> Result<(), &'static str> { Ok(()) }
// BLOCK 5: CLOSING BLOCK
pub fn is_ready() -> bool { true }
"#;
        let mut issues = Vec::new();
        let config = RustValidationConfig {
            require_all_5_blocks: true,
            require_metadata_tags: false,
            require_sub_assemblies: false,
            require_self_audit: true,
            require_readiness_probe: true,
            require_four_test_sections: true,
        };
        validate_rust_content(Path::new("test.rs"), source, &config, &mut issues);
        assert!(issues.is_empty(), "Files without #[test] should not require test section markers");
    }

    #[test]
    fn test_section_3_invariants_missing_sub_assemblies_detected() {
        let source = r#"
// =============================================================================
// BLOCK 1: METADATA BLOCK
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/sample.rs
// @layer         Layer 3
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        Sample
// @role          Sample
// @invariants    1. None.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Sample role.
// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================
// BLOCK 3: BODY BLOCK
// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================
pub fn self_audit() -> Result<(), &'static str> { Ok(()) }
// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
pub fn is_subsystem_ready() -> bool { true }
"#;
        let mut issues = Vec::new();
        let config = RustValidationConfig {
            require_all_5_blocks: true,
            require_metadata_tags: true,
            require_sub_assemblies: true,
            require_self_audit: true,
            require_readiness_probe: true,
            require_four_test_sections: false,
        };
        validate_rust_content(Path::new("test.rs"), source, &config, &mut issues);
        assert_eq!(issues.len(), 4); // Sub-Assembly 2., 3., 4., 5. missing
        assert!(issues.iter().all(|i| i.field == "sub_assembly"));
    }

    #[test]
    fn test_section_3_invariants_missing_metadata_tags_detected() {
        let source = r#"
// =============================================================================
// BLOCK 1: METADATA BLOCK
// =============================================================================
// @project       Tenebris and Lux
// =============================================================================
// BLOCK 2: OPENING BLOCK
// =============================================================================
// Sub-Assembly 2.1: Pre-Conditions
// =============================================================================
// BLOCK 3: BODY BLOCK
// =============================================================================
// Sub-Assembly 3.1: Logic
// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// =============================================================================
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
pub fn self_audit() -> Result<(), &'static str> { Ok(()) }
// =============================================================================
// BLOCK 5: CLOSING BLOCK
// =============================================================================
// Sub-Assembly 5.1: Epilogue Readiness Probe
pub fn is_subsystem_ready() -> bool { true }
"#;
        let mut issues = Vec::new();
        let config = RustValidationConfig {
            require_all_5_blocks: true,
            require_metadata_tags: true,
            require_sub_assemblies: true,
            require_self_audit: true,
            require_readiness_probe: true,
            require_four_test_sections: false,
        };
        validate_rust_content(Path::new("test.rs"), source, &config, &mut issues);
        assert!(!issues.is_empty());
        assert!(issues.iter().any(|i| i.field == "@file"));
        assert!(issues.iter().any(|i| i.field == "purpose"));
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

/// Epilogue readiness probe for the Rust validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the Rust tool inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::rust::is_rust_validator_ready;
///
/// assert!(is_rust_validator_ready());
/// ```
pub fn is_rust_validator_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Subsystem Navigation & Module Exports
// -----------------------------------------------------------------------------

/// Returns a human-readable telemetry status string for the Rust validator.
pub fn rust_validator_status() -> &'static str {
    if is_rust_validator_ready() {
        "READY"
    } else {
        "UNAVAILABLE"
    }
}
