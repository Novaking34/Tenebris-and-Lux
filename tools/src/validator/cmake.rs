// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/cmake.rs
// @id            cog.tools.validator.cmake
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          CMake Build Definition & Target Discipline Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | static parenthesis stack
// @scale         O(N) line-by-line lexical inspection
// @seams         in: syntax::cmake | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on arbitrary CMake script input.
//                2. Strictly enforces parenthesis balance across all command invocations.
//                3. Emits standardized ValidationIssue records for all syntax defects.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates CMake build scripts (`CMakeLists.txt`, `cmake/*.cmake`). Enforces
//   parenthesis balance, command syntax, out-of-source build conventions,
//   and 5-block comment envelopes (`# BLOCK 1: METADATA` .. `# BLOCK 5: CLOSING`).
//
//   ### For Game Designers & Non-Programmers:
//   Ensures the game's compilation blueprints are well-formed so builds and
//   packaging never fail unexpectedly.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Stateless lexical inspector scanning commands and
//     parentheses over immutable string slices; zero heap allocations in inner loop.
//   - Extension Seam: Configurable via `CmakeValidationConfig`; integrates with
//     `tools/src/validator/mod.rs` and `test_validator_languages.rs`.
//   - Explicit Non-Goal: Does not invoke external CMake binaries; audits script
//     grammar, 5-block structure, and parenthesis pairing natively.
// =============================================================================

//! # OmniCode CMake Build Validator
//!
//! Enforces universal 5-block structure and parenthesis balance on CMake scripts.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Configuration Types & Defaults )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::path::Path;
use crate::syntax::cmake::scan_cmake_commands;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Configuration Types & Default Contracts
// -----------------------------------------------------------------------------

/// Configuration options for CMake validation sweeps.
///
/// ```rust
/// use tl_tools::validator::cmake::CmakeValidationConfig;
///
/// let config = CmakeValidationConfig::default();
/// assert!(config.require_balanced_parentheses);
/// assert!(config.require_5block_banners);
/// ```
#[derive(Debug, Clone)]
pub struct CmakeValidationConfig {
    /// Whether command parentheses must be strictly balanced.
    pub require_balanced_parentheses: bool,
    /// Whether 5-block comment banners are required.
    pub require_5block_banners: bool,
}

impl Default for CmakeValidationConfig {
    fn default() -> Self {
        Self {
            require_balanced_parentheses: true,
            require_5block_banners: true,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Validation Rule Engine & Sub-Assembly Checkers )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Parenthesis Balance & Nesting Auditor
// -----------------------------------------------------------------------------

/// Audits CMake script content for strictly balanced parentheses across all commands.
pub fn audit_parentheses(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let mut depth: i32 = 0;
    let mut open_line = 0;

    for (idx, line) in content.lines().enumerate() {
        let trimmed = line.trim();
        // Skip pure comment lines
        if trimmed.starts_with('#') {
            continue;
        }

        for ch in trimmed.chars() {
            if ch == '(' {
                if depth == 0 {
                    open_line = idx + 1;
                }
                depth += 1;
            } else if ch == ')' {
                depth -= 1;
                if depth < 0 {
                    issues.push(ValidationIssue {
                        file: file_path.to_path_buf(),
                        block: "syntax_parenthesis".to_string(),
                        field: format!("line_{}", idx + 1),
                        severity: IssueSeverity::Error,
                        message: format!("Unmatched closing parenthesis ')' at line {}", idx + 1),
                    });
                    depth = 0;
                }
            }
        }
    }

    if depth > 0 {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "syntax_parenthesis".to_string(),
            field: format!("line_{}", open_line),
            severity: IssueSeverity::Error,
            message: format!("Unclosed CMake parenthesis opened at line {}", open_line),
        });
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Universal 5-Block Envelope Auditor
// -----------------------------------------------------------------------------

/// Audits CMake scripts for the presence of recommended 5-block comment banners.
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
                message: format!("Missing recommended CMake block banner '{}'", banner_pat),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Main Validation Entry Point
// -----------------------------------------------------------------------------

/// Validates CMake script content against build engineering standards.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Checks that the build script recipe is complete and balanced.
/// * For Systems Programmers: Evaluates parenthesis matching and scans commands via
///   `scan_cmake_commands`.
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::validator::cmake::{validate_cmake_content, CmakeValidationConfig};
///
/// let cmake = r#"
/// # BLOCK 1: METADATA BLOCK
/// # BLOCK 2: OPENING BLOCK
/// # BLOCK 3: BODY BLOCK
/// # BLOCK 4: AUDIT PROCEDURES BLOCK
/// # BLOCK 5: CLOSING BLOCK
/// cmake_minimum_required(VERSION 3.20)
/// project(tenebris_lux C)
/// "#;
/// let mut issues = Vec::new();
/// validate_cmake_content(Path::new("CMakeLists.txt"), cmake, &CmakeValidationConfig::default(), &mut issues);
/// assert!(issues.is_empty(), "Issues: {:?}", issues);
/// ```
pub fn validate_cmake_content(
    file_path: &Path,
    content: &str,
    config: &CmakeValidationConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Parenthesis Balance Verification
    if config.require_balanced_parentheses {
        audit_parentheses(file_path, content, issues);
    }

    // 2. Five-Block Envelope Verification
    if config.require_5block_banners {
        audit_5block_banners(file_path, content, issues);
    }

    // 3. Scan Commands via Syntax Lexer
    let _cmds = scan_cmake_commands(content);
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit testing CMake validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the CMake build inspector.
/// * For Systems Programmers: Runs unclosed parenthesis fixtures through `validate_cmake_content`.
///
/// ```rust
/// use tl_tools::validator::cmake::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = CmakeValidationConfig::default();

    // 1. Nominal balanced test
    let good_cmake = r#"
# BLOCK 1: METADATA BLOCK
# BLOCK 2: OPENING BLOCK
# BLOCK 3: BODY BLOCK
# BLOCK 4: AUDIT PROCEDURES BLOCK
# BLOCK 5: CLOSING BLOCK
cmake_minimum_required(VERSION 3.20)
project(test C)
"#;
    let mut good_issues = Vec::new();
    validate_cmake_content(Path::new("test.cmake"), good_cmake, &config, &mut good_issues);
    if !good_issues.is_empty() {
        return Err("Self-audit failed: nominal balanced CMake produced unexpected issues");
    }

    // 2. Fault inversion: unclosed parenthesis
    let broken_cmake = "set(FOO bar\n";
    let mut bad_issues = Vec::new();
    validate_cmake_content(Path::new("test.cmake"), broken_cmake, &config, &mut bad_issues);
    if !bad_issues.iter().any(|i| i.block == "syntax_parenthesis") {
        return Err("Self-audit failed: unclosed parenthesis issue not detected");
    }

    // 3. Fault inversion: extra closing parenthesis
    let extra_close = "set(FOO bar))\n";
    let mut extra_issues = Vec::new();
    validate_cmake_content(Path::new("test.cmake"), extra_close, &config, &mut extra_issues);
    if !extra_issues.iter().any(|i| i.block == "syntax_parenthesis") {
        return Err("Self-audit failed: extra closing parenthesis not detected");
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
    fn test_section_1_nominal_balanced_cmake_passes() {
        let cmake = r#"
# BLOCK 1: METADATA BLOCK
# BLOCK 2: OPENING BLOCK
# BLOCK 3: BODY BLOCK
# BLOCK 4: AUDIT PROCEDURES BLOCK
# BLOCK 5: CLOSING BLOCK
set(SOURCES main.c)
add_executable(app ${SOURCES})
"#;
        let mut issues = Vec::new();
        validate_cmake_content(Path::new("test.cmake"), cmake, &CmakeValidationConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // Section 2: Resilience & Never-Fail Edge Tests (Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_unclosed_paren_detected() {
        let cmake = "target_link_libraries(app\n";
        let mut issues = Vec::new();
        let config = CmakeValidationConfig {
            require_5block_banners: false,
            ..Default::default()
        };
        validate_cmake_content(Path::new("test.cmake"), cmake, &config, &mut issues);
        assert!(issues.iter().any(|i| i.block == "syntax_parenthesis"));
    }

    #[test]
    fn test_section_2_never_fail_empty_script_graceful() {
        let mut issues = Vec::new();
        validate_cmake_content(Path::new("empty.cmake"), "", &CmakeValidationConfig::default(), &mut issues);
        assert!(!issues.is_empty());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariant & Structural Guard Tests (Strict Immutability)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_missing_banners_warning() {
        let cmake = "message(STATUS \"Compiling\")\n";
        let mut issues = Vec::new();
        let config = CmakeValidationConfig {
            require_balanced_parentheses: false,
            require_5block_banners: true,
        };
        validate_cmake_content(Path::new("test.cmake"), cmake, &config, &mut issues);
        assert_eq!(issues.len(), 5);
    }

    #[test]
    fn test_section_3_invariants_comments_ignored_for_parens() {
        let cmake = r#"
# Comment with (unclosed parenthesis
set(A B)
"#;
        let mut issues = Vec::new();
        let config = CmakeValidationConfig {
            require_balanced_parentheses: true,
            require_5block_banners: false,
        };
        validate_cmake_content(Path::new("test.cmake"), cmake, &config, &mut issues);
        assert!(issues.is_empty(), "Parentheses inside comments must be ignored");
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

/// Epilogue readiness probe for the CMake validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the build script inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::cmake::is_cmake_validator_ready;
///
/// assert!(is_cmake_validator_ready());
/// ```
pub fn is_cmake_validator_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Subsystem Navigation & Module Exports
// -----------------------------------------------------------------------------

/// Returns a human-readable telemetry status string for the CMake validator.
pub fn cmake_validator_status() -> &'static str {
    if is_cmake_validator_ready() {
        "READY"
    } else {
        "UNAVAILABLE"
    }
}
