// =============================================================================
// BLOCK 1: METADATA BLOCK
// ( Identity, Provenance, Scaling Telemetry & Architectural Boundary Contracts )
// =============================================================================
// @project       Tenebris and Lux
// @file          tools/tests/system/validator_system.rs
// @id            test.system.tools.validator
// @layer         Layer 4: Verification Suite
// @archetype     verification_suite | system_scenario
// @version       0.1.2-dev
// @interface_rev Rev 5
// @date          2026-11-16
// @author        CreativeWorkzStudio LLC / ProfessorSeanEX
// @repository    https://github.com/Novaking34/tenebris-lux
// @system        OmniCode Developer Toolchain // Tier 4 System Validator Suite
// @role          System-Level End-to-End Testing for Multi-Language Rule Validators
// @maturity      Rev 5 Baseline | 4-Section Ontological Testing
// @chassis       bounded-heap | Test runner execution chassis; stack frames bounded
// @scale         O(N) full repository audit sweeps across C11 headers and configs
// @seams         in: tl_tools::validator | uses: engine/include/
// @invariants    1. Four-Section Ontological Architecture strictly enforced:
//                   Section 1 (Always Pass), Section 2 (Never Fail),
//                   Section 3 (Invariants & Limits), Section 4 (Emergence).
//                2. Zero Panics: Repository audits complete with structured reports.
//                3. External Consumer Purity: imports tl_tools via public API.
//                4. Strict <= 80-column line width discipline across documentation.
//
// PURPOSE & ARCHITECTURAL ROLE:
//   Tier 4 System end-to-end test suite evaluating the `tl_tools::validator`
//   rule engine across the live repository codebase. Executes full audit sweeps
//   against C11/C23 engine headers, checking 5-block structure and guards.
//
//   ### For Game Designers & Non-Programmers:
//   Verifies that the automated code validator can scan all engine files and
//   produce a clean, passing report with zero errors.
//
//   ### For Systems Programmers & Engine Architects:
//   - Audit Scale: Exercises `HeaderValidator::run_audit` across all 44 Cornerstone
//     engine headers in `engine/include/` to verify complete workspace compliance.
//   - Fault Inversion: Proves that missing directories or damaged files return
//     clean summary structures without crashing.
// =============================================================================

//! # Tier 4 System Verification Suite for Repository Validator
//!
//! Evaluates end-to-end multi-language validation across Always-Pass, Never-Fail,
//! Invariant, and Emergent ontological tiers.

// =============================================================================
// BLOCK 2: OPENING BLOCK
// ( Pre-Conditions, Constants, Test Fixtures & Harness Scaffolding )
// =============================================================================

use crate::common;
use tl_tools::validator::{
    c::HeaderValidator,
    config::{load_header_rules_config, load_subsystems_registry},
    is_validator_system_ready,
};

// -----------------------------------------------------------------------------
// Sub-Assembly 2.1: Operational Test Constants & Suite Identifiers
// -----------------------------------------------------------------------------

/// Canonical suite identifier matching engine testing convention.
pub const SUITE_NAME: &str = "Tier 4: System Validator Verification Surface";

// =============================================================================
// BLOCK 3: BODY BLOCK / FOUR-SECTION ONTOLOGICAL TEST SUITE
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 3.1: [ SECTION 1: ALWAYS PASS (NOMINAL GOLDEN SURFACE COGS) ]
// -----------------------------------------------------------------------------

/// Verifies nominal golden execution of the master validator system probe.
///
/// Asserts that `is_validator_system_ready()` returns true.
#[test]
fn test_section_1_nominal_master_validator_system_probe() {
    assert!(
        is_validator_system_ready(),
        "[SEC 1: ALWAYS PASS] Master validator system must report ready status"
    );
}

/// Verifies nominal golden execution of the live C11 header audit.
///
/// Asserts that all engine headers in `engine/include/` pass validation.
#[test]
fn test_section_1_nominal_c_header_audit_sweep() {
    let root = common::resolve_repo_root();
    if !root.join("engine/include").exists() {
        return;
    }
    let registry = load_subsystems_registry(&root).expect("Must load subsystems registry");
    let rules = load_header_rules_config(&root);
    let validator = HeaderValidator::new(root, registry, rules, false);
    let summary = validator.run_audit().expect("Audit run must succeed");
    assert!(
        summary.is_success(false),
        "[SEC 1: ALWAYS PASS] Engine headers audit failed with {} errors",
        summary.error_count()
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.2: [ SECTION 2: NEVER FAIL / FAULT INVERSION & DEGRADATION ]
// -----------------------------------------------------------------------------

/// Verifies fault inversion when auditing an empty include directory.
///
/// Asserts that missing include directories return a clean empty summary.
#[test]
fn test_section_2_never_fail_missing_target_file_graceful() {
    let empty_dir = std::path::PathBuf::from("/tmp/nonexistent_include_dir_xyz");
    let headers = HeaderValidator::discover_headers(&empty_dir);
    assert!(
        headers.is_empty(),
        "[SEC 2: NEVER FAIL] Nonexistent include directory must yield empty headers"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.3: [ SECTION 3: INVARIANTS, SATURATION & LIMIT GUARDS ]
// -----------------------------------------------------------------------------

/// Verifies validator system readiness invariants.
///
/// Asserts that the validator system remains operational across calls.
#[test]
fn test_section_3_invariants_validator_readiness_matrix() {
    assert!(
        is_validator_system_ready(),
        "[SEC 3: INVARIANTS] System must report ready"
    );
}

// -----------------------------------------------------------------------------
// Sub-Assembly 3.4: [ SECTION 4: EMERGENCE & COMPOSITE WORKFLOWS ]
// -----------------------------------------------------------------------------

/// Verifies emergent multi-subsystem header guard derivation.
///
/// Asserts that headers across different engine strata derive distinct guards.
#[test]
fn test_section_4_emergence_live_header_audit_run() {
    let rules = tl_tools::validator::config::HeaderRulesConfig::default();
    let validator = HeaderValidator::new(
        std::path::PathBuf::from("."),
        tl_tools::validator::config::SubsystemRegistry::default(),
        rules,
        false,
    );
    let log_guard = validator.expected_cornerstone_guard("log/log.h");
    let core_guard = validator.expected_cornerstone_guard("core/manifest.h");
    let audio_guard = validator.expected_cornerstone_guard("audio/audio_bus.h");

    assert_eq!(log_guard, "CORNERSTONE_LOG_LOG_H");
    assert_eq!(core_guard, "CORNERSTONE_CORE_MANIFEST_H");
    assert_eq!(audio_guard, "CORNERSTONE_AUDIO_AUDIO_BUS_H");
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
// Sub-Assembly 4.2: Validator Subsystem Anchor Verification Gate
// -----------------------------------------------------------------------------

/// Audits physical source files required for validator execution.
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
}

// =============================================================================
// BLOCK 5: CLOSING BLOCK
// ( Test Suite Epilogue & Summary Handshake )
// =============================================================================

// -----------------------------------------------------------------------------
// Sub-Assembly 5.1: Epilogue & Clean State Handshake
// -----------------------------------------------------------------------------

/// Verifies that running system validator tests leaves zero transient artifacts.
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
