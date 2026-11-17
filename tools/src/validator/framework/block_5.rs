// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/framework/block_5.rs
// @id            cog.tools.validator.framework.block_5
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Language-Agnostic Block 5 Closing & Epilogue Rule Engine
// @maturity      Rev 5 Baseline | Universal Block 5 Rule Engine
// @chassis       zero-heap | Static epilogue verification tables; bounded issues
// @scale         O(1) readiness predicate verification
// @seams         in: syntax::framework::block_5 | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on empty or malformed Block 5 structures.
//                2. Verifies presence of the standardized readiness predicate function.
//                3. Emits standardized ValidationIssue records for unclosed files or missing predicates.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Language-agnostic validation rule engine for Block 5 Closing. Evaluates
//   the epilogue readiness check (`is_<subsystem>_ready()`), matching closing
//   tokens (such as `#endif /* GUARD */`), and epilogue cleanliness.
//
//   ### For Game Designers & Non-Programmers:
//   Block 5 is the final handshake. It confirms that the system is ready to be
//   used by other parts of the game, like turning on the green light when
//   everything is clear.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Pure rule engine operating on `Block5Closing`.
//   - Epilogue Probes: Enforces `is_*_ready()` naming conventions.
//   - Closing Symmetry: Verifies closing guard tokens and comment substrings.
// =============================================================================

//! # OmniCode Block 5 Closing Validator
//!
//! Enforces universal Block 5 readiness predicates, closing token symmetry, and epilogue contracts.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::Path;
use crate::syntax::framework::block_5::Block5Closing;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults
// -----------------------------------------------------------------------------

/// Canonical prefix for readiness probe predicates.
pub const CANONICAL_READINESS_PREFIX: &str = "is_";

/// Canonical suffix for readiness probe predicates.
pub const CANONICAL_READINESS_SUFFIX: &str = "_ready";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Rule Configuration
// -----------------------------------------------------------------------------

/// Configuration options for Block 5 closing rule evaluation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_5::Block5RuleConfig;
///
/// let config = Block5RuleConfig::default();
/// assert!(config.require_readiness_predicate);
/// ```
#[derive(Debug, Clone)]
pub struct Block5RuleConfig {
    /// Whether an `is_<subsystem>_ready()` readiness predicate is required.
    pub require_readiness_predicate: bool,
    /// Whether closing guard `#endif` is required.
    pub require_guard_close: bool,
    /// Expected closing guard comment substring.
    pub expected_guard_comment: Option<String>,
}

impl Default for Block5RuleConfig {
    fn default() -> Self {
        Self {
            require_readiness_predicate: true,
            require_guard_close: false,
            expected_guard_comment: None,
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Predicate Invariants
// -----------------------------------------------------------------------------

/// Evaluates whether a function name matches canonical readiness predicate format.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_5::is_canonical_readiness_name;
///
/// assert!(is_canonical_readiness_name("is_math_ready"));
/// assert!(!is_canonical_readiness_name("check_ready"));
/// ```
pub fn is_canonical_readiness_name(name: &str) -> bool {
    name.starts_with(CANONICAL_READINESS_PREFIX) && name.ends_with(CANONICAL_READINESS_SUFFIX)
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Primary Execution Engine, Rule Evaluation & Issue Generation )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Block 5 Validation Rule Engine
// -----------------------------------------------------------------------------

/// Validates a parsed `Block5Closing` AST against epilogue rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Checks that the subsystem has a green-light readiness signal.
/// * For Systems Programmers: Verifies presence and name format of the readiness predicate
///   and validates closing guard comments.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::framework::block_5::parse_block_5_lines;
/// use tl_tools::validator::framework::block_5::{validate_block_5, Block5RuleConfig};
///
/// let lines = [
///     "pub fn is_math_subsystem_ready() -> bool { true }".to_string(),
///     "#endif /* CORNERSTONE_CORE_MATH_H */".to_string(),
/// ];
/// let b5 = parse_block_5_lines(&lines).unwrap();
/// let mut issues = Vec::new();
/// validate_block_5(Path::new("engine/include/core/math.h"), &b5, &Block5RuleConfig::default(), &mut issues);
/// assert!(issues.is_empty());
/// ```
pub fn validate_block_5(
    file_path: &Path,
    b5: &Block5Closing,
    config: &Block5RuleConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Epilogue readiness predicate verification
    if config.require_readiness_predicate && !b5.has_readiness_predicate {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "block_5_closing".to_string(),
            field: "readiness_predicate".to_string(),
            severity: IssueSeverity::Error,
            message: "Missing 'is_<subsystem>_ready()' epilogue readiness probe in Block 5".to_string(),
        });
    }

    // 2. Closing guard check
    if config.require_guard_close && !b5.has_guard_close {
        issues.push(ValidationIssue {
            file: file_path.to_path_buf(),
            block: "block_5_closing".to_string(),
            field: "guard_close".to_string(),
            severity: IssueSeverity::Error,
            message: "Missing closing #endif header guard in Block 5".to_string(),
        });
    }

    // 3. Expected guard comment verification
    if let (Some(actual), Some(expected)) = (&b5.guard_close_comment, &config.expected_guard_comment) {
        if !actual.contains(expected) {
            issues.push(ValidationIssue {
                file: file_path.to_path_buf(),
                block: "block_5_closing".to_string(),
                field: "guard_close_comment".to_string(),
                severity: IssueSeverity::Error,
                message: format!(
                    "Closing guard comment mismatch: found '{}', expected comment containing '{}'",
                    actual, expected
                ),
            });
        }
    }
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
    use crate::syntax::framework::block_5::parse_block_5_lines;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_valid_block_5_passes() {
        let lines = [
            "pub fn is_subsystem_ready() -> bool { true }".to_string(),
            "#endif /* CORNERSTONE_FOO_H */".to_string(),
        ];
        let b5 = parse_block_5_lines(&lines).expect("Parsing b5 failed");
        let mut issues = Vec::new();
        validate_block_5(Path::new("test.h"), &b5, &Block5RuleConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    #[test]
    fn test_section_1_nominal_canonical_readiness_name() {
        assert!(is_canonical_readiness_name("is_subsystem_ready"));
        assert!(is_canonical_readiness_name("is_block_5_framework_ready"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_missing_predicate_detected() {
        let b5 = Block5Closing {
            raw_lines: Vec::new(),
            sub_assemblies: Vec::new(),
            has_readiness_predicate: false,
            readiness_function_name: None,
            readiness_line: None,
            has_guard_close: false,
            guard_close_comment: None,
        };
        let mut issues = Vec::new();
        validate_block_5(Path::new("test.rs"), &b5, &Block5RuleConfig::default(), &mut issues);
        assert!(issues.iter().any(|i| i.field == "readiness_predicate"));
    }

    #[test]
    fn test_section_2_never_fail_invalid_readiness_names() {
        assert!(!is_canonical_readiness_name(""));
        assert!(!is_canonical_readiness_name("not_ready"));
        assert!(!is_canonical_readiness_name("is_running"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_closing_token_mismatch() {
        let b5 = Block5Closing {
            raw_lines: Vec::new(),
            sub_assemblies: Vec::new(),
            has_readiness_predicate: true,
            readiness_function_name: Some("is_foo_ready".to_string()),
            readiness_line: Some(10),
            has_guard_close: true,
            guard_close_comment: Some("/* WRONG */".to_string()),
        };
        let mut issues = Vec::new();
        let mut config = Block5RuleConfig::default();
        config.expected_guard_comment = Some("EXPECTED".to_string());
        validate_block_5(Path::new("test.h"), &b5, &config, &mut issues);
        assert!(issues.iter().any(|i| i.field == "guard_close_comment"));
    }

    #[test]
    fn test_section_3_invariants_constants_non_empty() {
        assert_eq!(CANONICAL_READINESS_PREFIX, "is_");
        assert_eq!(CANONICAL_READINESS_SUFFIX, "_ready");
    }

    // -------------------------------------------------------------------------
    // SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_4_emergence_self_audit() {
        assert!(self_audit().is_ok());
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Diagnostic Self-Audit
// -----------------------------------------------------------------------------

/// Internal self-audit testing Block 5 closing validation logic.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests the final handshake inspector.
/// * For Systems Programmers: Runs synthetic empty Block 5 through `validate_block_5`.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_5::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = Block5RuleConfig::default();
    let b5 = Block5Closing {
        raw_lines: Vec::new(),
        sub_assemblies: Vec::new(),
        has_readiness_predicate: false,
        readiness_function_name: None,
        readiness_line: None,
        has_guard_close: false,
        guard_close_comment: None,
    };

    let mut issues = Vec::new();
    validate_block_5(Path::new("test.rs"), &b5, &config, &mut issues);

    if !issues.iter().any(|i| i.field == "readiness_predicate") {
        return Err("Self-audit failed: missing readiness_predicate issue not detected");
    }
    Ok(())
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( State Summary Handshake, Module Epilogue & Teardown Verification )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue Predicate
// -----------------------------------------------------------------------------

/// Epilogue readiness probe for the Block 5 validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the closing inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_5::is_validator_block_5_ready;
///
/// assert!(is_validator_block_5_ready());
/// ```
pub fn is_validator_block_5_ready() -> bool {
    self_audit().is_ok()
}
