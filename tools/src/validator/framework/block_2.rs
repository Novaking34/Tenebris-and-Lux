// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/framework/block_2.rs
// @id            cog.tools.validator.framework.block_2
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Language-Agnostic Block 2 Opening & Sub-Assembly Rule Engine
// @maturity      Rev 5 Baseline | Universal Block 2 Rule Engine
// @chassis       zero-heap | Static sub-assembly sequence tables; bounded issues
// @scale         O(N) scan over parsed sub-assembly markers and include statements
// @seams         in: syntax::framework::block_2 | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on malformed or empty Block 2 structures.
//                2. Enforces ascending order on sub-assemblies 2.1 through 2.5.
//                3. Emits standardized ValidationIssue records for all structural violations.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Language-agnostic validation rule engine for Block 2 Opening. Evaluates
//   sub-assembly markers (2.1 Headers/Imports, 2.2 Constants/Macros,
//   2.3 Types/Chassis, 2.4 Statics/State, 2.5 Prototypes/Declarations),
//   and import hygiene.
//
//   ### For Game Designers & Non-Programmers:
//   Block 2 sets the stage before action begins. It makes sure ingredients
//   (like tools and settings) are gathered in an orderly sequence so nothing
//   gets mixed up before the core logic runs.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Pure rule engine operating on `Block2Opening`.
//   - Sub-Assembly Discipline: Enforces canonical sub-assembly numbers (2.1..2.5)
//     and monotonic ordering.
//   - Include Hygiene: Scans for forbidden relative directory escapes (`../`, `/home/`).
// =============================================================================

//! # OmniCode Block 2 Opening Validator
//!
//! Enforces universal Block 2 sub-assembly sequences and include hygiene.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::Path;
use crate::syntax::framework::block_2::Block2Opening;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Forbidden Patterns
// -----------------------------------------------------------------------------

/// Canonical forbidden include patterns in hermetic engine headers.
pub const DEFAULT_FORBIDDEN_INCLUDE_PATTERNS: &[&str] = &["../", "\\", "/home/", "/usr/"];

/// Canonical baseline sub-assembly markers expected in Block 2.
pub const CANONICAL_B2_SUBASSEMBLIES: &[&str] = &["2.1", "2.2", "2.3", "2.4"];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Rule Configuration
// -----------------------------------------------------------------------------

/// Configuration options for Block 2 opening rule evaluation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_2::Block2RuleConfig;
///
/// let config = Block2RuleConfig::default();
/// assert!(config.require_sub_assemblies);
/// assert!(config.enforce_ascending_sub_assemblies);
/// ```
#[derive(Debug, Clone)]
pub struct Block2RuleConfig {
    /// Whether sub-assembly markers (2.1, 2.2, 2.3, 2.4) are required.
    pub require_sub_assemblies: bool,
    /// Whether sub-assemblies must appear in strictly ascending numeric order.
    pub enforce_ascending_sub_assemblies: bool,
    /// Prohibited include path patterns (e.g. `../` directory escapes).
    pub forbidden_include_patterns: Vec<&'static str>,
}

impl Default for Block2RuleConfig {
    fn default() -> Self {
        Self {
            require_sub_assemblies: true,
            enforce_ascending_sub_assemblies: true,
            forbidden_include_patterns: DEFAULT_FORBIDDEN_INCLUDE_PATTERNS.to_vec(),
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & Monotonic Predicates
// -----------------------------------------------------------------------------

/// Returns true if sub-assembly `second` logically follows `first` in numeric order.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Checks that 2.2 comes after 2.1, and 2.3 comes after 2.2.
/// * For Systems Programmers: Parses major and minor numbers for dot-separated versions.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_2::is_ascending_sub_assembly;
///
/// assert!(is_ascending_sub_assembly("2.1", "2.2"));
/// assert!(is_ascending_sub_assembly("2.2", "2.3"));
/// assert!(is_ascending_sub_assembly("2.4", "2.5"));
/// assert!(!is_ascending_sub_assembly("2.3", "2.1"));
/// ```
pub fn is_ascending_sub_assembly(first: &str, second: &str) -> bool {
    let parse_sub = |s: &str| -> Option<(u32, u32)> {
        let mut parts = s.split('.');
        let major = parts.next()?.parse::<u32>().ok()?;
        let minor = parts.next()?.parse::<u32>().ok()?;
        Some((major, minor))
    };

    match (parse_sub(first), parse_sub(second)) {
        (Some(a), Some(b)) => b > a,
        _ => false,
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Primary Execution Engine, Rule Evaluation & Issue Generation )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Block 2 Validation Rule Engine
// -----------------------------------------------------------------------------

/// Validates a parsed `Block2Opening` AST against structural rules.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Verifies that ingredients and setups appear in order
///   without skipping or reversing steps.
/// * For Systems Programmers: Enforces sub-assembly monotonic ordering
///   and checks include hygiene against forbidden directory escapes.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::framework::block_2::parse_block_2_lines;
/// use tl_tools::validator::framework::block_2::{validate_block_2, Block2RuleConfig};
///
/// let lines = [
///     "// Sub-Assembly 2.1: Headers & Includes".to_string(),
///     "#include <stdint.h>".to_string(),
///     "// Sub-Assembly 2.2: Constants & Macros".to_string(),
///     "#define MAX_UNITS 16".to_string(),
///     "// Sub-Assembly 2.3: Types & Chassis".to_string(),
///     "typedef struct Unit { int id; } Unit;".to_string(),
///     "// Sub-Assembly 2.4: Global State".to_string(),
///     "static Unit s_units[MAX_UNITS];".to_string(),
/// ];
/// let b2 = parse_block_2_lines(&lines).unwrap();
/// let mut issues = Vec::new();
/// validate_block_2(Path::new("engine/include/core/unit.h"), &b2, &Block2RuleConfig::default(), &mut issues);
/// assert!(issues.is_empty());
/// ```
pub fn validate_block_2(
    file_path: &Path,
    b2: &Block2Opening,
    config: &Block2RuleConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Sub-assembly sequence verification
    if config.require_sub_assemblies {
        for expected in CANONICAL_B2_SUBASSEMBLIES {
            if !b2.has_subassembly(expected) {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: format!("sub_assembly_{}", expected),
                    severity: IssueSeverity::Warning,
                    message: format!("Missing recommended Block 2 sub-assembly '{}'", expected),
                });
            }
        }
    }

    // 2. Ascending order verification
    if config.enforce_ascending_sub_assemblies && b2.sub_assemblies.len() > 1 {
        for window in b2.sub_assemblies.windows(2) {
            let prev = &window[0];
            let next = &window[1];
            if !is_ascending_sub_assembly(&prev.number, &next.number) {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "sub_assembly_ordering".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!(
                        "Sub-assembly '{}' at line {} appears after '{}' at line {}; order must be strictly ascending",
                        next.number, next.line_number, prev.number, prev.line_number
                    ),
                });
            }
        }
    }

    // 3. Include hygiene check
    for include in &b2.includes {
        for forbidden in &config.forbidden_include_patterns {
            if include.path.contains(forbidden) {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_2_opening".to_string(),
                    field: "include_hygiene".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!(
                        "Prohibited include pattern '{}' found in include path '{}'",
                        forbidden, include.path
                    ),
                });
            }
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
    use crate::syntax::framework::block_2::parse_block_2_lines;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_ascending_sub_assemblies() {
        let lines = [
            "// Sub-Assembly 2.1: Headers & Includes".to_string(),
            "#include <stdint.h>".to_string(),
            "// Sub-Assembly 2.2: Constants & Macros".to_string(),
            "#define FOO 1".to_string(),
            "// Sub-Assembly 2.3: Types & Chassis".to_string(),
            "typedef int Foo;".to_string(),
            "// Sub-Assembly 2.4: Global State".to_string(),
            "static Foo s_foo;".to_string(),
        ];
        let b2 = parse_block_2_lines(&lines).expect("Parsing b2 failed");
        let mut issues = Vec::new();
        validate_block_2(Path::new("test.h"), &b2, &Block2RuleConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    #[test]
    fn test_section_1_nominal_valid_ordering() {
        assert!(is_ascending_sub_assembly("2.1", "2.2"));
        assert!(is_ascending_sub_assembly("2.2", "2.3"));
        assert!(is_ascending_sub_assembly("2.3", "2.4"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_detects_forbidden_includes() {
        let b2 = Block2Opening {
            raw_lines: Vec::new(),
            sub_assemblies: Vec::new(),
            includes: vec![crate::syntax::framework::block_2::IncludeDirective {
                path: "../secret/header.h".to_string(),
                is_quoted: true,
                is_system: false,
                line_number: 5,
            }],
            has_baseline_subassemblies: false,
        };
        let mut issues = Vec::new();
        let mut config = Block2RuleConfig::default();
        config.require_sub_assemblies = false;
        validate_block_2(Path::new("test.h"), &b2, &config, &mut issues);
        assert!(issues.iter().any(|i| i.field == "include_hygiene"));
    }

    #[test]
    fn test_section_2_never_fail_out_of_order_sub_assemblies() {
        let b2 = Block2Opening {
            raw_lines: Vec::new(),
            sub_assemblies: vec![
                crate::syntax::framework::block_2::SubAssemblyMarker {
                    number: "2.3".to_string(),
                    title: "Types".to_string(),
                    line_number: 10,
                },
                crate::syntax::framework::block_2::SubAssemblyMarker {
                    number: "2.1".to_string(),
                    title: "Headers".to_string(),
                    line_number: 20,
                },
            ],
            includes: Vec::new(),
            has_baseline_subassemblies: false,
        };
        let mut issues = Vec::new();
        validate_block_2(Path::new("test.h"), &b2, &Block2RuleConfig::default(), &mut issues);
        assert!(issues.iter().any(|i| i.field == "sub_assembly_ordering"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_ordering_predicate() {
        assert!(is_ascending_sub_assembly("2.1", "2.2"));
        assert!(is_ascending_sub_assembly("2.2", "2.3"));
        assert!(!is_ascending_sub_assembly("2.2", "2.2"));
        assert!(!is_ascending_sub_assembly("2.3", "2.2"));
        assert!(!is_ascending_sub_assembly("invalid", "2.1"));
    }

    #[test]
    fn test_section_3_invariants_canonical_subassemblies() {
        assert_eq!(CANONICAL_B2_SUBASSEMBLIES.len(), 4);
        assert_eq!(CANONICAL_B2_SUBASSEMBLIES[0], "2.1");
        assert_eq!(CANONICAL_B2_SUBASSEMBLIES[3], "2.4");
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

/// Internal self-audit testing Block 2 opening validation logic.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests that out-of-order sub-assemblies trigger warnings.
/// * For Systems Programmers: Runs synthetic sub-assembly lists through `validate_block_2`.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_2::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let mut config = Block2RuleConfig::default();
    config.enforce_ascending_sub_assemblies = true;

    let b2 = Block2Opening {
        raw_lines: Vec::new(),
        sub_assemblies: vec![
            crate::syntax::framework::block_2::SubAssemblyMarker {
                number: "2.3".to_string(),
                title: "Types".to_string(),
                line_number: 10,
            },
            crate::syntax::framework::block_2::SubAssemblyMarker {
                number: "2.1".to_string(),
                title: "Headers".to_string(),
                line_number: 20,
            },
        ],
        includes: Vec::new(),
        has_baseline_subassemblies: false,
    };

    let mut issues = Vec::new();
    validate_block_2(Path::new("test.c"), &b2, &config, &mut issues);

    if !issues.iter().any(|i| i.field == "sub_assembly_ordering") {
        return Err("Self-audit failed: out-of-order sub-assembly issue not detected");
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

/// Epilogue readiness probe for the Block 2 validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the opening inspector is operational.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_2::is_validator_block_2_ready;
///
/// assert!(is_validator_block_2_ready());
/// ```
pub fn is_validator_block_2_ready() -> bool {
    self_audit().is_ok()
}
