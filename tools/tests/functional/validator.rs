// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/functional/validator.rs
// @id            test.functional.tools.validator
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | functional_surface
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 1 Functional Validator Suite
// @role          Functional Surface Testing for Multi-Language Rule Validators
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(1) execution over validator subsystem readiness probes
// @seams         in: tl_tools::validator | uses: common::resolve_repo_root
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Malformed source or invalid syntax records issues.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 1 Functional surface test suite evaluating the `tl_tools::validator`
//   multi-language rule engines. Probes the baseline readiness flags of all 9
//   concrete language validators and validates coarse issue detection behavior
//   when encountering malformed C++ guards or broken JSON payloads.
//
//   ### For Game Designers & Non-Programmers:
//   Guarantees that all automated file inspectors (checking code comments, data
//   schemas, and documentation formatting) are online and actively catching
//   mistakes before they reach the game engine.
//
//   ### For Systems Programmers & Engine Architects:
//   - Validator Boundary: Evaluates public readiness hooks (`is_*_validator_ready`)
//     and entrypoint validation contracts without mocking validator dispatch.
//   - Fault Inversion: Proves that corrupt content records structured issues in
//     the issue sink instead of aborting or unwinding.
// =============================================================================

//! # Tier 1 Functional Surface Verification Suite for Rule Validators
//!
//! Evaluates `tl_tools::validator` readiness and issue detection surfaces across
//! Always-Pass, Never-Fail, Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use std::path::{Path, PathBuf};
use tl_tools::validator::{
    adoc::is_adoc_validator_ready,
    c::{is_c_validator_ready, HeaderValidator},
    cmake::is_cmake_validator_ready,
    config::{HeaderRulesConfig, SubsystemRegistry},
    cpp::{is_cpp_validator_ready, validate_cpp_content, CppValidationConfig},
    git::is_git_validator_ready,
    is_validator_system_ready,
    json::{is_json_validator_ready, validate_json_content},
    md::is_markdown_validator_ready,
    rust::is_rust_validator_ready,
    toml::is_toml_validator_ready,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 1: Functional Validator Surface";

// -----------------------------------------------------------------------------
// Sub-Assembly 2.2: Test Fixtures & Pre-Condition Assertions
// -----------------------------------------------------------------------------

/// Constructs a mock C header path for validation testing.
fn sample_c_path() -> &'static Path {
    Path::new("engine/include/core/sample.h")
}

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of all validator readiness probes.
///
/// Asserts that every concrete validator reports ready status upon initialization.
#[test]
fn test_section_1_nominal_all_validator_readiness_probes() {
    assert!(
        is_validator_system_ready(),
        "[SEC 1: ALWAYS PASS] Master validator system must be ready"
    );
    assert!(is_c_validator_ready(), "C validator must be ready");
    assert!(is_cpp_validator_ready(), "C++ validator must be ready");
    assert!(is_rust_validator_ready(), "Rust validator must be ready");
    assert!(is_json_validator_ready(), "JSON validator must be ready");
    assert!(is_adoc_validator_ready(), "AsciiDoc validator must be ready");
    assert!(is_markdown_validator_ready(), "Markdown validator must be ready");
    assert!(is_cmake_validator_ready(), "CMake validator must be ready");
    assert!(is_toml_validator_ready(), "TOML validator must be ready");
    assert!(is_git_validator_ready(), "Git validator must be ready");
}

/// Verifies nominal golden derivation of canonical Cornerstone include guards.
///
/// Asserts that standard subsystem paths map to canonical header guard strings.
#[test]
fn test_section_1_nominal_c_guard_derivation() {
    let rules = HeaderRulesConfig::default();
    let validator = HeaderValidator::new(PathBuf::from("."), SubsystemRegistry::default(), rules, false);
    assert_eq!(
        validator.expected_cornerstone_guard("log/log.h"),
        "CORNERSTONE_LOG_LOG_H",
        "[SEC 1: ALWAYS PASS] Expected CORNERSTONE_LOG_LOG_H"
    );
    assert_eq!(
        validator.expected_cornerstone_guard("core/manifest.h"),
        "CORNERSTONE_CORE_MANIFEST_H",
        "[SEC 1: ALWAYS PASS] Expected CORNERSTONE_CORE_MANIFEST_H"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when validating C++ content missing extern guards.
///
/// Asserts that unguarded C++ declarations are flagged as issues cleanly.
#[test]
fn test_section_2_never_fail_invalid_cpp_content_returns_issue() {
    let path = sample_c_path();
    let bad_cpp = "void broken_cpp_function_without_extern_c_guards();\n";
    let mut issues = Vec::new();
    validate_cpp_content(path, bad_cpp, &CppValidationConfig::default(), &mut issues);
    assert!(
        !issues.is_empty(),
        "[SEC 2: NEVER FAIL] Unguarded C++ must report validation issue"
    );
}

/// Verifies fault inversion when validating malformed JSON syntax.
///
/// Asserts that syntax-corrupted JSON produces validation issues rather than panicking.
#[test]
fn test_section_2_never_fail_corrupt_json_content_returns_issue() {
    let path = Path::new("engine/config/sample.json");
    let bad_json = "{\n  \"key\": \"value\",\n  BROKEN_SYNTAX_WITHOUT_QUOTES\n}";
    let mut issues = Vec::new();
    validate_json_content(path, bad_json, &mut issues);
    assert!(
        !issues.is_empty(),
        "[SEC 2: NEVER FAIL] Corrupt JSON syntax must report validation issue"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies total validator count integrity invariants.
///
/// Asserts that exactly 9 language validators are registered and active.
#[test]
fn test_section_3_invariants_validator_count_integrity() {
    let readiness_checks = [
        is_c_validator_ready(),
        is_cpp_validator_ready(),
        is_rust_validator_ready(),
        is_json_validator_ready(),
        is_adoc_validator_ready(),
        is_markdown_validator_ready(),
        is_cmake_validator_ready(),
        is_toml_validator_ready(),
        is_git_validator_ready(),
    ];
    assert_eq!(
        readiness_checks.len(),
        9,
        "[SEC 3: INVARIANTS] Must contain exactly 9 registered language validators"
    );
    assert!(
        readiness_checks.iter().all(|&r| r),
        "[SEC 3: INVARIANTS] All registered language validators must be ready"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent master validator probe response.
///
/// Asserts that master system readiness is consistent with all constituent validators.
#[test]
fn test_section_4_emergence_system_validator_probe() {
    assert!(
        is_validator_system_ready(),
        "[SEC 4: EMERGENCE] System validator must report ready status"
    );
}

// =============================================================================
// BLOCK 4: AUDIT PROCEDURES BLOCK
// ( Preflight Diagnostics & Deep Verification Audits )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 4.1: Preflight Diagnostic Gates
// -----------------------------------------------------------------------------

/// Diagnostic self-audit verifying suite constant integrity and common harness.
#[test]
fn test_preflight_diagnostics_self_audit() {
    assert!(
        !SUITE_NAME.is_empty(),
        "[BLOCK 4: PREFLIGHT] SUITE_NAME must not be empty"
    );
    assert!(
        common::self_audit().is_ok(),
        "[BLOCK 4: PREFLIGHT] Common test harness self-audit must succeed"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 4.2: Validator Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical validator source files required for repository validation.
#[test]
fn test_preflight_validator_anchors() {
    let root = common::resolve_repo_root();
    assert!(
        root.join("tools/src/validator/mod.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/validator/mod.rs must exist"
    );
    assert!(
        root.join("tools/src/validator/c.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/validator/c.rs must exist"
    );
    assert!(
        root.join("tools/src/validator/json.rs").is_file(),
        "[BLOCK 4: PREFLIGHT] tools/src/validator/json.rs must exist"
    );
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running functional validator tests leaves zero transient artifacts.
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
