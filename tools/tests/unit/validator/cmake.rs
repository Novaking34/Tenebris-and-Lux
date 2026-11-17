// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/unit/validator/cmake.rs
// @id            test.unit.tools.validator.cmake
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | unit_isolation
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // CMake Validator Unit Suite
// @role          Unit Isolation Testing for CMake Script Rule Validator
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(L) lexical analysis across CMake commands and parentheses
// @seams         in: tl_tools::validator::cmake | pairs: CMakeLists.txt, cmake/
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Malformed CMake source records issues cleanly.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 2 Unit isolation test suite evaluating `tl_tools::validator::cmake`.
//   Validates CMake build scripts against balanced parentheses, command syntax,
//   and universal 5-block structure in complete isolation.
//
//   ### For Game Designers & Non-Programmers:
//   Ensures the build system configuration blueprints are strictly checked so
//   compiling the game engine or tooling never fails due to syntax typos.
//
//   ### For Systems Programmers & Engine Architects:
//   - Lexical Scope: Verifies command scanner, parenthesis stack depth, and
//     comment-block boundaries without spawning external cmake processes.
//   - Fault Inversion: Proves that unbalanced parentheses or unterminated blocks
//     emit structured `ValidationIssue` records rather than crashing.
// =============================================================================

//! # Tier 2 Unit Isolation Suite for CMake Script Validator
//!
//! Evaluates `tl_tools::validator::cmake` across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::Path;
use tl_tools::validator::cmake::{
    cmake_validator_status, is_cmake_validator_ready, self_audit,
    validate_cmake_content, CmakeValidationConfig,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 2: Unit CMake Validator Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs a mock CMake script path for validation testing.
fn sample_cmake_path() -> &'static Path {
    Path::new("cmake/sample.cmake")
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden validation of canonical 5-block CMake content.
///
/// Asserts that a balanced, 5-block CMake script produces zero validation issues.
#[test]
fn test_section_1_nominal_valid_cmake_passes() {
    let source = r#"# =============================================================================
# BLOCK 1: METADATA BLOCK
# =============================================================================
# @project       Tenebris and Lux
# @file          cmake/sample.cmake
# @layer         Layer 0: Core Engine

# =============================================================================
# BLOCK 2: OPENING BLOCK
# =============================================================================
cmake_minimum_required(VERSION 3.25)

# =============================================================================
# BLOCK 3: BODY BLOCK
# =============================================================================
set(CORNERSTONE_SAMPLE_FLAG ON)

# =============================================================================
# BLOCK 4: AUDIT PROCEDURES BLOCK
# =============================================================================
# Validation check

# =============================================================================
# BLOCK 5: CLOSING BLOCK
# =============================================================================
# Epilogue
"#;
    let mut issues = Vec::new();
    let config = CmakeValidationConfig::default();
    validate_cmake_content(sample_cmake_path(), source, &config, &mut issues);
    assert!(
        issues.is_empty(),
        "[SEC 1: ALWAYS PASS] Canonical 5-block CMake must produce zero issues: {:?}",
        issues
    );
}

/// Verifies nominal golden execution of the CMake validator readiness probe.
///
/// Asserts that `is_cmake_validator_ready()` returns true.
#[test]
fn test_section_1_nominal_cmake_validator_readiness_probe() {
    assert!(
        is_cmake_validator_ready(),
        "[SEC 1: ALWAYS PASS] CMake validator must report ready status"
    );
}

/// Verifies nominal golden retrieval of the CMake validator status string.
///
/// Asserts that status string contains Ready indicator.
#[test]
fn test_section_1_nominal_cmake_validator_status_string() {
    let status = cmake_validator_status();
    assert_eq!(
        status, "READY",
        "[SEC 1: ALWAYS PASS] Status string must equal READY"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when validating CMake script with unbalanced parentheses.
///
/// Asserts that unclosed parentheses are flagged with structured issues.
#[test]
fn test_section_2_never_fail_unbalanced_parentheses_recorded() {
    let bad_cmake = "set(BROKEN_FLAG ON\n";
    let mut issues = Vec::new();
    let config = CmakeValidationConfig::default();
    validate_cmake_content(sample_cmake_path(), bad_cmake, &config, &mut issues);
    assert!(
        issues.iter().any(|i| i.message.contains("parenthes") || i.message.contains("Unbalanced")),
        "[SEC 2: NEVER FAIL] Unbalanced parenthesis must be flagged"
    );
}

/// Verifies fault inversion when validating completely empty CMake content.
///
/// Asserts that empty inputs degrade gracefully to reported issues without panic.
#[test]
fn test_section_2_never_fail_empty_cmake_content_graceful() {
    let mut issues = Vec::new();
    let config = CmakeValidationConfig::default();
    validate_cmake_content(sample_cmake_path(), "", &config, &mut issues);
    assert!(
        !issues.is_empty(),
        "[SEC 2: NEVER FAIL] Empty content must report issues cleanly"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies default configuration invariants for the CMake validator.
///
/// Asserts that balanced parentheses and 5-block banners are checked by default.
#[test]
fn test_section_3_invariants_cmake_validation_config_defaults() {
    let config = CmakeValidationConfig::default();
    assert!(
        config.require_balanced_parentheses,
        "Must require balanced parentheses by default"
    );
    assert!(
        config.require_5block_banners,
        "Must require 5-block banners by default"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent validation against authentic root CMakeLists.txt script.
///
/// Asserts that the live project `CMakeLists.txt` validates with balanced parentheses.
#[test]
fn test_section_4_emergence_live_root_cmakelists_validation() {
    let root = common::resolve_repo_root();
    let cmakelists_path = root.join("CMakeLists.txt");
    if let Ok(content) = std::fs::read_to_string(&cmakelists_path) {
        let mut issues = Vec::new();
        let config = CmakeValidationConfig {
            require_balanced_parentheses: true,
            require_5block_banners: false, // Root CMake file uses top-level comments
        };
        validate_cmake_content(&cmakelists_path, &content, &config, &mut issues);
        assert!(
            issues.is_empty(),
            "[SEC 4: EMERGENCE] Root CMakeLists.txt must have balanced parentheses: {:?}",
            issues
        );
    }
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying suite constant integrity and command audit.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME must not be empty"
    );
    assert!(
        self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] validator::cmake::self_audit() must pass"
    );
    assert!(
        common::self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Common test harness self-audit must succeed"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: CMake Subsystem Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical source files required for CMake validation.
#[test]
fn test_preflight_cmake_validator_anchors() {
    let root = common::resolve_repo_root();
    assert!(
        root.join("tools/src/validator/cmake.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/validator/cmake.rs must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running CMake validator unit tests leaves zero transient artifacts.
#[test]
fn test_epilogue_clean_state_handshake() {
    let root = common::resolve_repo_root();
    assert!(
        !root.join("tools/scratch_transient.tmp").exists(),
        "[BLOCK 5: EPILOGUE] Zero transient test artifacts leaked to tools/"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 5.2: Epilogue Readiness Probe
// -----------------------------------------------------------------------------

/// Verifies that common harness remains in a ready state after test suite.
#[test]
fn test_epilogue_readiness_probe() {
    assert!(
        common::is_common_harness_ready(),
        "[BLOCK 5: EPILOGUE] Common harness must remain ready"
    );
}
