// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/src/validator/framework/block_3.rs
// @id            cog.tools.validator.framework.block_3
// @layer         Layer 3: Systems Developer Tooling Core Library
// @archetype     operational | stateless
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-15
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Universal 5-Block Validator Suite
// @role          Language-Agnostic Block 3 Body & Function Rule Engine
// @maturity      Rev 5 Baseline | Universal Block 3 Rule Engine
// @chassis       zero-heap | Static body rule evaluation tables; bounded issues
// @scale         O(N) iteration over parsed operational functions and sub-assemblies
// @seams         in: syntax::framework::block_3 | uses: validator::report | probes: self_audit -> exit
// @invariants    1. Zero panics on empty or malformed Block 3 AST structures.
//                2. Verifies dual-audience documentation on all exported operational functions.
//                3. Emits standardized ValidationIssue records for all contract violations.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Language-agnostic validation rule engine for Block 3 Body. Evaluates
//   sub-assembly structure (3.1..3.X), dual-audience operational docstrings,
//   forbidden heap symbols (Invariant 1 ALLOC_ZERO_HEAP), and signature parity.
//
//   ### For Game Designers & Non-Programmers:
//   Block 3 is the engine's workshop. Every tool and action made here must have
//   instructions that explain how it affects the game's mechanics, so anyone
//   can adjust tuning values with confidence.
//
//   ### For Systems Programmers & Engine Architects:
//   - Ownership & Reentrancy: Pure rule engine operating on `Block3Body`.
//   - Dual-Audience Covenant: Verifies Designer and Programmer docs on functions.
//   - Invariant 1 Defense: Scans body text for prohibited dynamic allocation tokens.
// =============================================================================

//! # OmniCode Block 3 Body Validator
//!
//! Enforces universal Block 3 function docstrings, dual-audience commentary, and heap invariants.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Public Contract Limits, Command Types, Options & O(1) Pre-Conditions )
// =============================================================================

use std::path::Path;
use crate::syntax::framework::block_3::Block3Body;
use crate::validator::report::{IssueSeverity, ValidationIssue};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Defaults & Prohibited Allocation Tokens
// -----------------------------------------------------------------------------

/// Canonical forbidden dynamic memory tokens under Invariant 1 (ALLOC_ZERO_HEAP).
pub const DEFAULT_PROHIBITED_ALLOC_TOKENS: &[&str] = &["malloc(", "free(", "calloc(", "realloc("];

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Types, Enums & Rule Configuration
// -----------------------------------------------------------------------------

/// Configuration options for Block 3 body rule evaluation.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_3::Block3RuleConfig;
///
/// let config = Block3RuleConfig::default();
/// assert!(config.require_dual_audience_docstrings);
/// assert!(config.enforce_ascending_sub_assemblies);
/// ```
#[derive(Debug, Clone)]
pub struct Block3RuleConfig {
    /// Whether operational functions must have dual-audience docstrings.
    pub require_dual_audience_docstrings: bool,
    /// Whether sub-assemblies (3.1..3.X) must be strictly ascending.
    pub enforce_ascending_sub_assemblies: bool,
    /// Prohibited keywords/symbols (e.g. `malloc`, `free` under ALLOC_ZERO_HEAP).
    pub prohibited_symbols: Vec<&'static str>,
}

impl Default for Block3RuleConfig {
    fn default() -> Self {
        Self {
            require_dual_audience_docstrings: true,
            enforce_ascending_sub_assemblies: true,
            prohibited_symbols: DEFAULT_PROHIBITED_ALLOC_TOKENS.to_vec(),
        }
    }
}

// -----------------------------------------------------------------------------
// Sub-Assembly 2.3: Verification Helpers & 3.X Monotonic Predicates
// -----------------------------------------------------------------------------

/// Returns true if sub-assembly `second` logically follows `first` in 3.X numeric order.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms 3.2 comes after 3.1.
/// * For Systems Programmers: Parses major 3 and minor index integers.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_3::is_ascending_b3_sub_assembly;
///
/// assert!(is_ascending_b3_sub_assembly("3.1", "3.2"));
/// assert!(is_ascending_b3_sub_assembly("3.2", "3.3"));
/// assert!(!is_ascending_b3_sub_assembly("3.3", "3.1"));
/// ```
pub fn is_ascending_b3_sub_assembly(first: &str, second: &str) -> bool {
    let parse_sub = |s: &str| -> Option<(u32, u32)> {
        let mut parts = s.split('.');
        let major = parts.next()?.parse::<u32>().ok()?;
        let minor = parts.next()?.parse::<u32>().ok()?;
        Some((major, minor))
    };

    match (parse_sub(first), parse_sub(second)) {
        (Some((3, m1)), Some((3, m2))) => m2 > m1,
        _ => false,
    }
}

// =============================================================================
// BLOCK 3: BODY BLOCK
// ( Primary Execution Engine, Rule Evaluation & Issue Generation )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: Block 3 Validation Rule Engine
// -----------------------------------------------------------------------------

/// Validates a parsed `Block3Body` AST against body rules and covenants.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Ensures all abilities and systems have plain English notes
///   alongside their technical definitions.
/// * For Systems Programmers: Verifies docstring presence, dual-audience split,
///   and sub-assembly 3.X sequence constraints.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::syntax::framework::block_3::parse_block_3_lines;
/// use tl_tools::validator::framework::block_3::{validate_block_3, Block3RuleConfig};
///
/// let lines = [
///     "// Sub-Assembly 3.1: Combat Mechanics".to_string(),
///     "/// Calculates damage output.".to_string(),
///     "/// * For Game Designers: Scales attack power by defense.".to_string(),
///     "/// * For Systems Programmers: Pure arithmetic function.".to_string(),
///     "int calculate_damage(int attack, int defense);".to_string(),
/// ];
/// let b3 = parse_block_3_lines(&lines).unwrap();
/// let mut issues = Vec::new();
/// validate_block_3(Path::new("game/src/combat/damage.c"), &b3, &Block3RuleConfig::default(), &mut issues);
/// assert!(issues.is_empty());
/// ```
pub fn validate_block_3(
    file_path: &Path,
    b3: &Block3Body,
    config: &Block3RuleConfig,
    issues: &mut Vec<ValidationIssue>,
) {
    // 1. Dual-audience docstring checks on parsed functions
    if config.require_dual_audience_docstrings {
        for func in &b3.functions {
            let has_designer = !func.designer_text.trim().is_empty();
            let has_programmer = !func.programmer_text.trim().is_empty();

            if !has_designer {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_3_body".to_string(),
                    field: format!("fn_{}", func.name),
                    severity: IssueSeverity::Error,
                    message: format!(
                        "Function '{}' (line {}) missing '* For Game Designers:' documentation",
                        func.name, func.line_number
                    ),
                });
            }
            if !has_programmer {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_3_body".to_string(),
                    field: format!("fn_{}", func.name),
                    severity: IssueSeverity::Error,
                    message: format!(
                        "Function '{}' (line {}) missing '* For Systems Programmers:' documentation",
                        func.name, func.line_number
                    ),
                });
            }
        }
    }

    // 2. Ascending order verification on 3.X sub-assemblies
    if config.enforce_ascending_sub_assemblies && b3.sub_assemblies.len() > 1 {
        for window in b3.sub_assemblies.windows(2) {
            let prev = &window[0];
            let next = &window[1];
            if !is_ascending_b3_sub_assembly(&prev.number, &next.number) {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_3_body".to_string(),
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
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: Prohibited Symbol Scanner
// -----------------------------------------------------------------------------

/// Checks raw body source text for prohibited symbols (e.g. dynamic heap allocation).
///
/// # Dual-Audience Documentation
/// * For Game Designers: Makes sure no slow memory leaks or uncontrolled lag sources sneak in.
/// * For Systems Programmers: Scans text for forbidden allocation calls under ALLOC_ZERO_HEAP.
///
/// # Examples
///
/// ```rust
/// use std::path::Path;
/// use tl_tools::validator::framework::block_3::validate_prohibited_symbols;
///
/// let mut issues = Vec::new();
/// validate_prohibited_symbols(Path::new("test.c"), "void* p = malloc(64);", &["malloc("], &mut issues);
/// assert_eq!(issues.len(), 1);
/// ```
pub fn validate_prohibited_symbols(
    file_path: &Path,
    source_text: &str,
    prohibited: &[&str],
    issues: &mut Vec<ValidationIssue>,
) {
    for (line_idx, line) in source_text.lines().enumerate() {
        let trimmed = line.trim();
        // Ignore comment lines
        if trimmed.starts_with("//") || trimmed.starts_with("/*") || trimmed.starts_with('*') {
            continue;
        }
        for bad_sym in prohibited {
            if line.contains(bad_sym) {
                issues.push(ValidationIssue {
                    file: file_path.to_path_buf(),
                    block: "block_3_body".to_string(),
                    field: "prohibited_symbol".to_string(),
                    severity: IssueSeverity::Error,
                    message: format!(
                        "Prohibited symbol '{}' found at line {}: Invariant 1 ALLOC_ZERO_HEAP violation",
                        bad_sym, line_idx + 1
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
    use crate::syntax::framework::block_3::parse_block_3_lines;

    // -------------------------------------------------------------------------
    // SECTION 1: ALWAYS PASS (Nominal Golden Surface Cogs)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_1_nominal_valid_block_3_passes() {
        let lines = [
            "// Sub-Assembly 3.1: Mechanics".to_string(),
            "/// Damage calculation.".to_string(),
            "/// * For Game Designers: Scales by power.".to_string(),
            "/// * For Systems Programmers: Pure arithmetic.".to_string(),
            "int calculate(int power);".to_string(),
        ];
        let b3 = parse_block_3_lines(&lines).expect("Parsing b3 failed");
        let mut issues = Vec::new();
        validate_block_3(Path::new("test.c"), &b3, &Block3RuleConfig::default(), &mut issues);
        assert!(issues.is_empty(), "Unexpected issues: {:?}", issues);
    }

    #[test]
    fn test_section_1_nominal_ascending_b3_predicate() {
        assert!(is_ascending_b3_sub_assembly("3.1", "3.2"));
        assert!(is_ascending_b3_sub_assembly("3.2", "3.3"));
        assert!(is_ascending_b3_sub_assembly("3.3", "3.4"));
    }

    // -------------------------------------------------------------------------
    // SECTION 2: NEVER FAIL (Fault Inversion & Graceful Degradation)
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_2_never_fail_detects_prohibited_symbols() {
        let mut issues = Vec::new();
        let code = "int* buf = malloc(1024);\nfree(buf);";
        validate_prohibited_symbols(Path::new("test.c"), code, &["malloc(", "free("], &mut issues);
        assert_eq!(issues.len(), 2);
    }

    #[test]
    fn test_section_2_never_fail_undocumented_function_produces_errors() {
        let b3 = Block3Body {
            raw_lines: Vec::new(),
            sub_assemblies: Vec::new(),
            functions: vec![crate::syntax::framework::block_3::FunctionDoc {
                name: "undocumented".to_string(),
                signature: "void undocumented(void);".to_string(),
                designer_text: String::new(),
                programmer_text: String::new(),
                sub_assembly: "3.1".to_string(),
                line_number: 1,
            }],
        };
        let mut issues = Vec::new();
        validate_block_3(Path::new("test.c"), &b3, &Block3RuleConfig::default(), &mut issues);
        assert!(!issues.is_empty());
        assert!(issues.iter().any(|i| i.field == "fn_undocumented"));
    }

    // -------------------------------------------------------------------------
    // SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS
    // -------------------------------------------------------------------------

    #[test]
    fn test_section_3_invariants_ascending_b3_predicate() {
        assert!(is_ascending_b3_sub_assembly("3.1", "3.2"));
        assert!(is_ascending_b3_sub_assembly("3.2", "3.10"));
        assert!(!is_ascending_b3_sub_assembly("3.2", "3.2"));
        assert!(!is_ascending_b3_sub_assembly("3.3", "3.1"));
        assert!(!is_ascending_b3_sub_assembly("2.1", "3.1"));
    }

    #[test]
    fn test_section_3_invariants_prohibited_allocation_tokens() {
        assert!(DEFAULT_PROHIBITED_ALLOC_TOKENS.contains(&"malloc("));
        assert!(DEFAULT_PROHIBITED_ALLOC_TOKENS.contains(&"free("));
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

/// Internal self-audit testing Block 3 body validation logic.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Tests that functions missing documentation are caught.
/// * For Systems Programmers: Runs synthetic function ASTs through `validate_block_3`.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_3::self_audit;
///
/// assert!(self_audit().is_ok());
/// ```
pub fn self_audit() -> Result<(), &'static str> {
    let config = Block3RuleConfig::default();
    let b3 = Block3Body {
        raw_lines: Vec::new(),
        sub_assemblies: Vec::new(),
        functions: vec![crate::syntax::framework::block_3::FunctionDoc {
            name: "undocumented_fn".to_string(),
            signature: "void undocumented_fn(void);".to_string(),
            designer_text: String::new(),
            programmer_text: String::new(),
            sub_assembly: "3.1".to_string(),
            line_number: 42,
        }],
    };

    let mut issues = Vec::new();
    validate_block_3(Path::new("test.c"), &b3, &config, &mut issues);

    if !issues.iter().any(|i| i.field == "fn_undocumented_fn") {
        return Err("Self-audit failed: undocumented function issue not detected");
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

/// Epilogue readiness probe for the Block 3 validator subsystem.
///
/// # Dual-Audience Documentation
/// * For Game Designers: Confirms the workshop inspector is ready.
/// * For Systems Programmers: Returns true when self-audit passes.
///
/// # Examples
///
/// ```rust
/// use tl_tools::validator::framework::block_3::is_validator_block_3_ready;
///
/// assert!(is_validator_block_3_ready());
/// ```
pub fn is_validator_block_3_ready() -> bool {
    self_audit().is_ok()
}
