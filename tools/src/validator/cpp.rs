// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/cpp.rs
// @id            cog.tools.validator.cpp
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          C++ Compatibility & Invariant 21 Header Linkage Validator
// @maturity      Rev 5 Baseline
// @chassis       zero-heap | static keyword tables
// @scale         O(N) line-by-line lexical inspection
// @seams         in: syntax::cpp | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on arbitrary C++ header or source input.
//                2. Strictly enforces Invariant 21 extern "C" guards on dual-compatible headers.
//                3. Emits standardized ValidationIssue records for all C++ convention breaches.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Validates C++ source code and header compatibility according to Cornerstone
//   engine standards. Enforces Invariant 21 (`extern "C"` guards), namespace
//   hygiene (prohibiting `using namespace std;` in header scopes), and 5-block
//   lifecycle envelopes.
//
//   ### For Game Designers & Non-Programmers:
//   This tool ensures our engine can talk smoothly with modern C++ graphics and
//   audio systems without running into name collisions or language barriers.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Stateless AST and lexical inspector operating over
//     immutable string slices; reentrant and thread-safe.
//   - Extension Seam: Configurable via `CppValidationConfig`; plugs into
//     `tools/src/validator/mod.rs` and `test_validator_languages.rs`.
//   - Explicit Non-Goal: Does not invoke external compilers (`clang++`, `g++`);
//     performs structural convention and linkage guard audits only.
// =============================================================================

//! # OmniCode C++ Compatibility Validator
//!
//! Enforces Invariant 21 extern "C" linkage guards and C++ header hygiene.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Configuration Types & Defaults )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Pre-Conditions & Library Imports
// -----------------------------------------------------------------------------

use std::path::Path;
use crate::syntax::cpp::{audit_extern_c_guards, ExternCGuardReport};
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Configuration Types & Default Contracts
// -----------------------------------------------------------------------------

/// Configuration options for C++ validation sweeps.
///
/// ```rust
/// use tl_tools::validator::cpp::CppValidationConfig;
///
/// let config = CppValidationConfig::default();
/// assert!(config.require_extern_c_in_headers);
/// assert!(config.prohibit_using_namespace_std);
/// assert!(config.strict_5block);
/// ```
#[derive(Debug, Clone)]
pub struct CppValidationConfig {
    /// Whether C headers exposed to C++ must have `extern "C"` guards (Invariant 21).
    pub require_extern_c_in_headers: bool,
    /// Whether `using namespace std;` is forbidden in headers.
    pub prohibit_using_namespace_std: bool,
    /// Whether 5-block comments are strictly verified.
    pub strict_5block: bool,
}

impl Default for CppValidationConfig {
    fn default() -> Self {
        Self {
            require_extern_c_in_headers: true,
            prohibit_using_namespace_std: true,
            strict_5block: true,
        }
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Validation Rule Engine & Sub-Assembly Checkers )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Invariant 21 Extern "C" Linkage Guard Auditor
// -----------------------------------------------------------------------------

/// Audits C/C++ dual-compatible headers for Invariant 21 `extern "C"` linkage guards.
pub fn audit_extern_c_linkage(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    let audit: ExternCGuardReport = audit_extern_c_guards(content);
    if !audit.is_fully_guarded {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "block_2_opening".to_string(),
            field: "extern_c_guards".to_string(),
            severity: IssueSeverity::Error,
            message: format!(
                "Invariant 21 breach: incomplete or unbalanced extern \"C\" guards (has_open={}, has_close={})",
                audit.has_opening_guard, audit.has_closing_guard
            ),
        });
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Header Namespace Hygiene Auditor
// -----------------------------------------------------------------------------

/// Audits C++ headers for namespace pollution (specifically `using namespace std;`).
pub fn audit_namespace_hygiene(
    file_path: &Path,
    content: &str,
    issues: &mut Vec<ValidationIssue>,
) {
    for (line_idx, line) in content.lines().enumerate() {
        let trimmed = line.trim();
        if trimmed.starts_with("//") || trimmed.starts_with("/*") || trimmed.starts_with('*') {
            continue;
        }
        if trimmed.contains("using namespace std;") {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "block_2_opening".to_string(),
                field: "namespace_hygiene".to_string(),
                severity: IssueSeverity::Error,
                message: format!(
                    "Forbidden 'using namespace std;' at line {}: header namespace pollution",
                    line_idx + 1
                ),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: Universal 5-Block Envelope Auditor
// -----------------------------------------------------------------------------

/// Audits C++ source/header files for required 5-block comment banners.
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
    for block_name in &expected_blocks {
        if !content.contains(block_name) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "document_structure".to_string(),
                field: block_name.to_lowercase().replace(' ', "_"),
                severity: IssueSeverity::Warning,
                message: format!("Missing recommended banner '{}'", block_name),
            });
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: Main Validation Entry Point
// -----------------------------------------------------------------------------

/// Validates C++ source or header content against Cornerstone engine conventions.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Checks that C++ code plays nicely with the rest of the game.
/// * For Systems Programmers: Audits extern "C" linkage pairs, namespace hygiene,
///   and 5-block comment envelopes.
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::validator::cpp::{validate_cpp_content, CppValidationConfig};
///
/// let header = r#"
/// /*
/// BLOCK 1: METADATA BLOCK
/// BLOCK 2: OPENING BLOCK
/// BLOCK 3: BODY BLOCK
/// BLOCK 4: AUDIT PROCEDURES BLOCK
/// BLOCK 5: CLOSING BLOCK
/// */
/// #ifdef __cplusplus
/// extern "C" {
/// #endif
/// void engine_init(void);
/// #ifdef __cplusplus
/// }
/// #endif
/// "#;
/// let mut issues = Vec::new();
/// validate_cpp_content(Path::new("engine/include/core/engine.h"), header, &CppValidationConfig::default(), &mut issues);
/// assert!(issues.is_empty());
/// ```
pub fn validate_cpp_content(
    file_path: &Path,
    content: &str,
    config: &CppValidationConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    let is_header = file_path
        .extension()
        .and_then(|e| e.to_str())
        .map(|e| e == "h" || e == "hpp" || e == "hxx")
        .unwrap_or(false);

    // 1. Invariant 21: Extern "C" Linkage Guard Audit
    if is_header && config.require_extern_c_in_headers {
        audit_extern_c_linkage(file_path, content, issues);
    }

    // 2. Namespace Hygiene (prohibit 'using namespace std;')
    if is_header && config.prohibit_using_namespace_std {
        audit_namespace_hygiene(file_path, content, issues);
    }

    // 3. Block Banners Check
    if config.strict_5block {
        audit_5block_banners(file_path, content, issues);
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Four-Section Ontological Test Suite & Subsystem Self-Audit )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Internal Subsystem Self-Audit Probe
// -----------------------------------------------------------------------------

/// Internal self-audit verifying C++ validation rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the C++ compatibility tester.
/// * For Systems Programmers: Runs balanced and unbalanced snippets through `validate_cpp_content`.
///
/// ```rust
/// use tl_tools::validator::cpp::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = CppValidationConfig::default();

    // 1. Nominal balanced test
    let good_header = r#"
/*
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
*/
#ifdef __cplusplus
extern "C" {
#endif
void foo(void);
#ifdef __cplusplus
}
#endif
"#;
    let mut good_issues = Vec::new();
    validate_cpp_content(Path::new("test.h"), good_header, &config, &mut good_issues);
    if !good_issues.is_empty() {
        return Err("Self-audit failed: nominal balanced C++ header produced unexpected issues");
    }

    // 2. Fault inversion: unclosed guard
    let bad_header = "#ifdef __cplusplus\nextern \"C\" {\n#endif\nvoid foo();\n";
    let mut bad_issues = Vec::new();
    validate_cpp_content(Path::new("test.h"), bad_header, &config, &mut bad_issues);
    if !bad_issues.iter().any(|i| i.field == "extern_c_guards") {
        return Err("Self-audit failed: unclosed extern \"C\" guard not detected");
    }

    // 3. Fault inversion: namespace pollution
    let mut ns_issues = Vec::new();
    validate_cpp_content(Path::new("test.hpp"), "using namespace std;\n", &config, &mut ns_issues);
    if !ns_issues.iter().any(|i| i.field == "namespace_hygiene") {
        return Err("Self-audit failed: namespace pollution not detected");
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
    fn test_section_1_nominal_balanced_cpp_header_passes() {
        let content = r#"
/*
BLOCK 1: METADATA BLOCK
BLOCK 2: OPENING BLOCK
BLOCK 3: BODY BLOCK
BLOCK 4: AUDIT PROCEDURES BLOCK
BLOCK 5: CLOSING BLOCK
*/
#ifdef __cplusplus
extern "C" {
#endif

void test_fn(void);

#ifdef __cplusplus
}
#endif
"#;
        let mut issues = Vec::new();
        validate_cpp_content(Path::new("test.h"), content, &CppValidationConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    // -------------------------------------------------------------------------
    // Section 2: Resilience & Never-Fail Edge Tests (Fault Inversion)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_2_never_fail_namespace_pollution_detected() {
        let content = "using namespace std;\n";
        let mut issues = Vec::new();
        let config = CppValidationConfig {
            require_extern_c_in_headers: false,
            strict_5block: false,
            ..Default::default()
        };
        validate_cpp_content(Path::new("test.hpp"), content, &config, &mut issues);
        assert!(issues.iter().any(|i| i.field == "namespace_hygiene"));
    }

    #[test]
    fn test_section_2_never_fail_empty_content_graceful() {
        let mut issues = Vec::new();
        validate_cpp_content(Path::new("empty.cpp"), "", &CppValidationConfig::default(), &mut issues);
        // Should emit missing block banner warnings, but never panic
        assert!(!issues.is_empty());
    }

    // -------------------------------------------------------------------------
    // Section 3: Invariant & Structural Guard Tests (Strict Immutability)
    // -------------------------------------------------------------------------
    #[test]
    fn test_section_3_invariants_unbalanced_guard_detected() {
        let content = "#ifdef __cplusplus\nextern \"C\" {\n#endif\n";
        let mut issues = Vec::new();
        let config = CppValidationConfig {
            strict_5block: false,
            ..Default::default()
        };
        validate_cpp_content(Path::new("test.h"), content, &config, &mut issues);
        assert!(issues.iter().any(|i| i.field == "extern_c_guards"));
    }

    #[test]
    fn test_section_3_invariants_missing_5block_warning() {
        let content = "#ifdef __cplusplus\nextern \"C\" {\n#endif\n#ifdef __cplusplus\n}\n#endif\n";
        let mut issues = Vec::new();
        let config = CppValidationConfig {
            require_extern_c_in_headers: true,
            prohibit_using_namespace_std: true,
            strict_5block: true,
        };
        validate_cpp_content(Path::new("test.h"), content, &config, &mut issues);
        assert_eq!(issues.len(), 5, "Expected 5 block banner warnings");
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

/// Epilogue readiness probe for the C++ validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the C++ bridge inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// ```rust
/// use tl_tools::validator::cpp::is_cpp_validator_ready;
///
/// assert!(is_cpp_validator_ready());
/// ```
pub fn is_cpp_validator_ready() -> bool {
    self_audit().is_ok()
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Subsystem Navigation & Module Exports
// -----------------------------------------------------------------------------

/// Returns a human-readable telemetry status string for the C++ validator.
pub fn cpp_validator_status() -> &'static str {
    if is_cpp_validator_ready() {
        "READY"
    } else {
        "UNAVAILABLE"
    }
}
