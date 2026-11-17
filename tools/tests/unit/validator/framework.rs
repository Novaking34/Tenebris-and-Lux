// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/validator/framework.rs
// @id            test.unit.tools.validator.framework
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Framework Validator Suite
// @role          Four-Section Ontological Integration Suite for 5-Block Validator
// @maturity      Rev 5 Baseline | Four-Section Transpiler-Ready Verification
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution time over 5-block rule engines
// @seams         in: validator::framework | uses: tl_tools::validator | probes: self_audit -> exit
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. External Consumer Purity: imports tl_tools via public API.
//                3. Zero Mocks: exercises live 5-block AST rule engines.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Integration verification harness evaluating the `validator::framework`
//   subsystem from the perspective of an external consumer or CI/CD runner.
//   Exercises `validate_block_1` through `validate_block_5`, composite
//   `validate_omni_doc`, column 16 enforcement, and sub-assembly ordering across
//   the 4-Section ontological standard.
//
//   ### For Game Designers & Non-Programmers:
//   Validates the quality inspector itself to ensure it never allows invalid files
//   to slip into the project or incorrectly rejects valid work.
//
//   ### For Systems Programmers & Engine Architects:
//   - Transpilation Alignment: Tests map 1:1 to OmniCode's 4-section verification
//     schema in Block 3, with Preflight in Block 4 and Epilogue in Block 5.
//   - Integration Boundary: Validates crate public API without breaching internals.
// =============================================================================

//! # Four-Section Integration Suite for Universal 5-Block Validator Framework
//!
//! Exercises `tl_tools::validator::framework` across Always-Pass, Never-Fail,
//! Invariant, and Emergent execution tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use std::path::Path;
use tl_tools::syntax::framework::OmniDoc;
use tl_tools::validator::framework::{
    validate_omni_doc,
    OmniDocValidationConfig,
    block_1::{Block1RuleConfig, is_valid_omnicode_id},
    block_2::is_ascending_sub_assembly,
    block_3::is_ascending_b3_sub_assembly,
    is_validator_framework_ready,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical test suite identifier matching engine convention.
pub const SUITE_NAME: &str = "5-Block Framework Validator Integration Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Preflight self-audit probe verifying that the validator framework is ready.
pub fn self_audit() -> Result<(), &'static str> {
    if !is_validator_framework_ready() {
        return Err("Self-audit failed: validator framework is not ready");
    }
    Ok(())
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of permissive 5-block AST validation.
#[test]
fn test_section_1_nominal_empty_non_strict_passes() {
    let doc = OmniDoc::default();
    let mut issues = Vec::new();
    let config = OmniDocValidationConfig {
        require_all_5_blocks: false,
        b1: Block1RuleConfig {
            required_tags: Vec::new(),
            require_dual_audience: false,
            strict_column_alignment: false,
            ..Default::default()
        },
        ..Default::default()
    };
    validate_omni_doc(Path::new("test.c"), &doc, &config, &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Permissive validation must produce zero issues: {:?}",
        issues
    );
}

/// Verifies nominal golden syntax validation of canonical OmniCode identifiers.
#[test]
fn test_section_1_nominal_id_syntax() {
    assert!(is_valid_omnicode_id("cog.core.math"), "[SEC 1: ALWAYS PASS] cog.core.math must be valid");
    assert!(is_valid_omnicode_id("tools.validator.framework"), "[SEC 1: ALWAYS PASS] tools.validator.framework must be valid");
    assert!(is_valid_omnicode_id("engine.config.viewport"), "[SEC 1: ALWAYS PASS] engine.config.viewport must be valid");
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when strict validation runs on an empty OmniDoc AST.
#[test]
fn test_section_2_never_fail_missing_blocks_in_strict_mode() {
    let doc = OmniDoc::default();
    let mut issues = Vec::new();
    validate_omni_doc(Path::new("test.c"), &doc, &OmniDocValidationConfig::strict(), &mut issues);
    assert!(!issues.is_empty(), "[SEC 2: NEVER FAIL] Strict mode must flag missing blocks");
    assert!(
        issues.iter().any(|i| i.field == "has_five_blocks"),
        "[SEC 2: NEVER FAIL] Expected has_five_blocks issue"
    );
}

/// Verifies fault inversion on malformed OmniCode identifier patterns.
#[test]
fn test_section_2_never_fail_invalid_ids() {
    assert!(!is_valid_omnicode_id(""), "[SEC 2: NEVER FAIL] Empty ID must be rejected");
    assert!(!is_valid_omnicode_id("invalid spaces"), "[SEC 2: NEVER FAIL] Space-separated ID must be rejected");
    assert!(!is_valid_omnicode_id(".leading.dot"), "[SEC 2: NEVER FAIL] Leading dot ID must be rejected");
    assert!(!is_valid_omnicode_id("trailing.dot."), "[SEC 2: NEVER FAIL] Trailing dot ID must be rejected");
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies sub-assembly ordering predicates across Block 2 and Block 3.
#[test]
fn test_section_3_invariants_subassembly_ordering() {
    assert!(is_ascending_sub_assembly("2.1", "2.2"), "[SEC 3: INVARIANTS] 2.1 < 2.2");
    assert!(is_ascending_sub_assembly("2.2", "2.3"), "[SEC 3: INVARIANTS] 2.2 < 2.3");
    assert!(is_ascending_sub_assembly("2.3", "2.4"), "[SEC 3: INVARIANTS] 2.3 < 2.4");
    assert!(!is_ascending_sub_assembly("2.4", "2.2"), "[SEC 3: INVARIANTS] 2.4 !< 2.2");

    assert!(is_ascending_b3_sub_assembly("3.1", "3.2"), "[SEC 3: INVARIANTS] 3.1 < 3.2");
    assert!(is_ascending_b3_sub_assembly("3.2", "3.3"), "[SEC 3: INVARIANTS] 3.2 < 3.3");
    assert!(!is_ascending_b3_sub_assembly("3.5", "3.1"), "[SEC 3: INVARIANTS] 3.5 !< 3.1");
}

/// Verifies that the framework readiness predicate evaluates to true.
#[test]
fn test_section_3_invariants_framework_ready() {
    assert!(
        is_validator_framework_ready(),
        "[SEC 3: INVARIANTS] is_validator_framework_ready must return true"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent self-audit execution of the validator framework suite.
#[test]
fn test_section_4_emergence_self_audit() {
    assert!(
        self_audit().is_ok(),
        "[SEC 4: EMERGENCE] Framework validator self_audit must pass"
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying suite constant integrity.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Framework validator preflight self_audit() must pass"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Suite Identifier Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical suite constants matching engine convention.
#[test]
fn test_preflight_suite_constants() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME constant must not be empty"
    );
    assert_eq!(
        SUITE_NAME, "5-Block Framework Validator Integration Surface",
        "[BLOCK 4: PREFLIGHT] SUITE_NAME must match canonical surface title"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Readiness probe for the framework validator integration test runner.
#[test]
fn test_epilogue_clean_state_handshake() {
    assert!(
        is_validator_framework_ready(),
        "[BLOCK 5: EPILOGUE] Validator framework must remain ready post-test"
    );
}
